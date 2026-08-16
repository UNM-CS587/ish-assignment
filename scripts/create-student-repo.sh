#!/usr/bin/env bash
#
# Create and configure the per-student `ish` repository in the class GitHub
# organization. Students submit a GitHub ID through Canvas; the instructor
# runs this script over those IDs.
#
# For each student the script:
#   1. creates a private repository from the assignment template,
#   2. enables GitHub Actions so the test workflows run,
#   3. gives the student write access to it,
#   4. gives the staff team write access to it,
#   5. protects `main` with a ruleset that requires a pull request and a
#      passing student-test run,
#   6. invites the student to the organization so organization-billed
#      Codespaces are available to them, and
#   7. adds the student to the students team.
#
# Re-running the script over a student who already has a repository reapplies
# every setting and leaves the contents alone, so it is safe to run again
# after a roster change.
#
# Requires the `gh` CLI authenticated as an organization owner with the `repo`
# and `admin:org` scopes. See scripts/README.md for the one-time organization
# setup this assumes.

set -euo pipefail

ORG=UNM-CS587
TEMPLATE=UNM-CS587/ish-assignment
PREFIX=ish
STAFF_TEAM=staff
STUDENT_TEAM=students
STATUS_CHECK=run-student-tests
# The app whose check runs count as $STATUS_CHECK; pinning it keeps another
# app from reporting a passing check under the same name.
ACTIONS_APP_ID=15368
RULESET_NAME='Student Test Check'
DRY_RUN=false

usage() {
	cat <<EOF
Usage: ${0##*/} [options] <github-id>...
       ${0##*/} [options] --roster <file>

Options:
  --roster <file>   Read GitHub IDs from <file>, one per line. Blank lines,
                    number-sign comments, surrounding whitespace, and a
                    leading at-sign are ignored.
  --org <name>      Organization that owns the student repositories.
                    Default: $ORG
  --template <o/r>  Template repository to generate each student repository
                    from. Default: $TEMPLATE
  --prefix <name>   Repository name prefix; each repository is named
                    <prefix>-<github-id>. Default: $PREFIX
  --staff-team <s>  Slug of the organization team that gets write access and
                    ruleset bypass. Pass an empty string to skip.
                    Default: $STAFF_TEAM
  --student-team <s>
                    Slug of the organization team each student joins. Grant
                    it no repository access. Pass an empty string to skip.
                    Default: $STUDENT_TEAM
  --dry-run         Report what would change without changing anything.
  -h, --help        Print this message.
EOF
}

log() { printf '%s\n' "$*"; }
warn() { printf 'warning: %s\n' "$*" >&2; }
die() {
	printf 'error: %s\n' "$*" >&2
	exit 1
}

# Print the command instead of running it under --dry-run. Read-only queries
# call `gh` directly so that a dry run still reports real repository state.
run() {
	if $DRY_RUN; then
		log "  would run: $*"
		return 0
	fi
	"$@"
}

students=()
roster=

while [[ $# -gt 0 ]]; do
	case $1 in
	--roster)
		roster=${2:?--roster needs a file}
		shift 2
		;;
	--org)
		ORG=${2:?--org needs a name}
		shift 2
		;;
	--template)
		TEMPLATE=${2:?--template needs owner/repo}
		shift 2
		;;
	--prefix)
		PREFIX=${2:?--prefix needs a name}
		shift 2
		;;
	--staff-team)
		STAFF_TEAM=${2-}
		shift 2
		;;
	--student-team)
		STUDENT_TEAM=${2-}
		shift 2
		;;
	--dry-run)
		DRY_RUN=true
		shift
		;;
	-h | --help)
		usage
		exit 0
		;;
	-*) die "unknown option: $1" ;;
	*)
		students+=("$1")
		shift
		;;
	esac
done

if [[ -n $roster ]]; then
	[[ -r $roster ]] || die "cannot read roster file: $roster"
	while IFS= read -r line || [[ -n $line ]]; do
		line=${line%%#*}
		line=${line//[[:space:]]/}
		line=${line#@}
		[[ -n $line ]] && students+=("$line")
	done <"$roster"
fi

[[ ${#students[@]} -gt 0 ]] || {
	usage >&2
	exit 2
}

for student in "${students[@]}"; do
	[[ $student =~ ^[A-Za-z0-9](-?[A-Za-z0-9])*$ && ${#student} -le 39 ]] ||
		die "not a valid GitHub ID: $student"
done

command -v gh &>/dev/null || die 'the `gh` CLI is not installed; see https://cli.github.com'
gh auth status &>/dev/null || die 'run `gh auth login` first'
gh repo view "$TEMPLATE" --json isTemplate --jq '.isTemplate' | grep -qx true ||
	die "$TEMPLATE is not a template repository; enable Settings > Template repository on it"

# Instructors need to reach past the ruleset to fix a student repository, and
# organization owners do not bypass rulesets unless they are named here.
bypass='{"actor_id":1,"actor_type":"OrganizationAdmin","bypass_mode":"always"}'
if [[ -n $STAFF_TEAM ]]; then
	staff_team_id=$(gh api "/orgs/$ORG/teams/$STAFF_TEAM" --jq .id 2>/dev/null) ||
		die "no team \`$STAFF_TEAM\` in $ORG; create it or pass --staff-team ''"
	bypass="$bypass,{\"actor_id\":$staff_team_id,\"actor_type\":\"Team\",\"bypass_mode\":\"always\"}"
fi

[[ -z $STUDENT_TEAM ]] || gh api "/orgs/$ORG/teams/$STUDENT_TEAM" --silent 2>/dev/null ||
	die "no team \`$STUDENT_TEAM\` in $ORG; create it or pass --student-team ''"

ruleset_json=$(
	cat <<EOF
{
  "name": "$RULESET_NAME",
  "target": "branch",
  "enforcement": "active",
  "bypass_actors": [$bypass],
  "conditions": { "ref_name": { "include": ["refs/heads/main"], "exclude": [] } },
  "rules": [
    { "type": "deletion" },
    { "type": "non_fast_forward" },
    {
      "type": "pull_request",
      "parameters": {
        "required_approving_review_count": 0,
        "dismiss_stale_reviews_on_push": false,
        "require_code_owner_review": false,
        "require_last_push_approval": false,
        "required_review_thread_resolution": false,
        "allowed_merge_methods": ["merge", "squash", "rebase"]
      }
    },
    {
      "type": "required_status_checks",
      "parameters": {
        "strict_required_status_checks_policy": true,
        "do_not_enforce_on_create": false,
        "required_status_checks": [
          { "context": "$STATUS_CHECK", "integration_id": $ACTIONS_APP_ID }
        ]
      }
    }
  ]
}
EOF
)

provision() {
	local student=$1
	local repo="$ORG/$PREFIX-$student"
	log "$student -> $repo"

	if gh repo view "$repo" &>/dev/null; then
		log '  repository already exists; reapplying settings'
	else
		run gh repo create "$repo" --private --template "$TEMPLATE" \
			--description "CS587 Project 1 (ish shell) repository for $student" ||
			return 1
		# Generating a repository from a template is asynchronous, and the
		# settings below expect a populated default branch.
		$DRY_RUN || for _ in {1..15}; do
			gh api "/repos/$repo/commits?per_page=1" &>/dev/null && break
			sleep 2
		done
	fi

	run gh api --silent --method PUT "/repos/$repo/actions/permissions" \
		-F enabled=true -f allowed_actions=all ||
		warn "$repo: could not enable Actions"

	run gh api --silent --method PUT "/repos/$repo/collaborators/$student" \
		-f permission=push ||
		{
			warn "$repo: could not invite $student as a collaborator"
			return 1
		}

	[[ -n $STAFF_TEAM ]] &&
		{ run gh api --silent --method PUT "/orgs/$ORG/teams/$STAFF_TEAM/repos/$repo" \
			-f permission=push ||
			warn "$repo: could not grant $STAFF_TEAM write access"; }

	local ruleset_id
	ruleset_id=$(gh api "/repos/$repo/rulesets" --jq ".[] | select(.name==\"$RULESET_NAME\") | .id" 2>/dev/null) || ruleset_id=
	if [[ -n $ruleset_id ]]; then
		run gh api --silent --method PUT "/repos/$repo/rulesets/$ruleset_id" --input - <<<"$ruleset_json" ||
			warn "$repo: could not update the $RULESET_NAME ruleset"
	else
		run gh api --silent --method POST "/repos/$repo/rulesets" --input - <<<"$ruleset_json" ||
			warn "$repo: could not protect the default branch (rulesets on private repositories need a GitHub Team plan)"
	fi

	gh api "/orgs/$ORG/members/$student" &>/dev/null ||
		run gh api --silent --method PUT "/orgs/$ORG/memberships/$student" -f role=member ||
		warn "$student: could not invite to $ORG; Codespaces will not be billed to the organization"

	[[ -n $STUDENT_TEAM ]] &&
		{ run gh api --silent --method PUT "/orgs/$ORG/teams/$STUDENT_TEAM/memberships/$student" \
			-f role=member ||
			warn "$student: could not add to the $STUDENT_TEAM team"; }

	log "  https://github.com/$repo"
	return 0
}

failed=()
for student in "${students[@]}"; do
	provision "$student" || failed+=("$student")
done

log ''
log "provisioned $((${#students[@]} - ${#failed[@]})) of ${#students[@]} repositories"
if [[ ${#failed[@]} -gt 0 ]]; then
	log "failed: ${failed[*]}"
	exit 1
fi
