#!/usr/bin/env bash
#
# Drop each student from write access to read access on their `ish`
# repository at the code deadline, and record the commit that will be graded.
#
# The student keeps read access, so they can still see their work, clone it,
# and read the graded state. Staff access arrives through the staff team and
# is left alone; `--unlock` restores write access to one student who has an
# extension or a regrade.
#
# Requires the `gh` CLI authenticated as an organization owner with the `repo`
# scope. See scripts/README.md.

set -euo pipefail

ORG=UNM-CS587
PREFIX=ish
STAFF_TEAM=staff
PERMISSION=pull
DRY_RUN=false

usage() {
	cat <<EOF
Usage: ${0##*/} [options] [github-id...]

With no GitHub IDs, acts on every $PREFIX-* repository in $ORG. With IDs,
acts only on those students' repositories.

Options:
  --unlock          Restore write access instead of removing it.
  --org <name>      Organization that owns the student repositories.
                    Default: $ORG
  --prefix <name>   Repository name prefix. Default: $PREFIX
  --staff-team <s>  Slug of the staff team, whose members are never demoted.
                    Pass an empty string to skip the lookup.
                    Default: $STAFF_TEAM
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

run() {
	if $DRY_RUN; then
		log "  would run: $*"
		return 0
	fi
	"$@"
}

students=()

while [[ $# -gt 0 ]]; do
	case $1 in
	--unlock)
		PERMISSION=push
		shift
		;;
	--org)
		ORG=${2:?--org needs a name}
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

command -v gh &>/dev/null || die 'the `gh` CLI is not installed; see https://cli.github.com'
gh auth status &>/dev/null || die 'run `gh auth login` first'

repos=()
if [[ ${#students[@]} -gt 0 ]]; then
	repos=("${students[@]/#/$PREFIX-}")
else
	while IFS= read -r name; do repos+=("$name"); done < <(
		gh repo list "$ORG" --limit 500 --json name \
			--jq ".[].name | select(startswith(\"$PREFIX-\"))"
	)
fi
[[ ${#repos[@]} -gt 0 ]] || die "no $PREFIX-* repositories found in $ORG"

staff=()
if [[ -n $STAFF_TEAM ]]; then
	while IFS= read -r member; do staff+=("$member"); done < <(
		gh api --paginate "/orgs/$ORG/teams/$STAFF_TEAM/members" --jq '.[].login' 2>/dev/null
	)
fi

# The student is the only direct collaborator the provisioning script adds;
# staff reach the repository through the staff team instead. Reading the
# collaborator list rather than the repository name keeps this correct even
# for a repository that was set up by hand.
set_student_permission() {
	local full="$ORG/$1"
	local sha collaborator changed=false

	sha=$(gh api "/repos/$full/commits/main" --jq .sha 2>/dev/null) || {
		warn "$full: cannot read main; skipping"
		return 1
	}
	log "$full $sha"

	while IFS= read -r collaborator; do
		[[ " ${staff[*]-} " == *" $collaborator "* ]] && continue
		changed=true
		run gh api --silent --method PUT "/repos/$full/collaborators/$collaborator" \
			-f permission="$PERMISSION" ||
			{
				warn "$full: could not set $collaborator to $PERMISSION"
				return 1
			}
		log "  $collaborator -> $PERMISSION"
	done < <(gh api --paginate "/repos/$full/collaborators?affiliation=direct" --jq '.[].login')

	$changed || warn "$full: no student collaborator to change"
	return 0
}

failed=()
for repo in "${repos[@]}"; do
	set_student_permission "$repo" || failed+=("$repo")
done

log ''
log "updated $((${#repos[@]} - ${#failed[@]})) of ${#repos[@]} repositories to $PERMISSION"
if [[ ${#failed[@]} -gt 0 ]]; then
	log "failed: ${failed[*]}"
	exit 1
fi
