# Instructor Scripts

These scripts are for the class instructor, not for students. They provision
the per-student repositories the assignment is turned in through, and close
them at the code deadline.

## create-student-repo.sh

Students submit a GitHub ID through its Canvas assignment, due 9:00am on
Wednesday, August 19, 2026. This script turns each ID into a private
repository in the class organization named `ish-<github-id>`, generated from
the `ish-assignment` template, with:

  * GitHub Actions enabled, so the class and student test workflows run,
  * write access for the student and for the staff team,
  * a `Student Test Check` ruleset on `main` matching the one on this
    repository: no direct pushes, no deletion, no force pushes, and a passing
    `run-student-tests` check before a pull request can merge,
  * an organization invitation for the student, which is what makes
    organization-billed Codespaces available to them, and
  * membership in the students team.

Instructors bypass the ruleset, so you can push a fix to a student repository
directly when you need to.

Run it over a roster file or over IDs on the command line:

```
gh auth login                                  # as an organization owner
./scripts/create-student-repo.sh --dry-run --roster roster.txt
./scripts/create-student-repo.sh --roster roster.txt
```

The roster file holds one GitHub ID per line and ignores blank lines,
`#` comments, surrounding whitespace, and a leading `@`. Re-running the
script reapplies every setting and never touches repository contents, so run
it again as late adds come in. It reports each repository it could not
finish and exits nonzero, rather than stopping at the first failure.

`gh auth status` must show the `repo` and `admin:org` scopes; the
organization invitation, the team grants, and the team memberships need
`admin:org`. Run `--dry-run` first and read what it intends to do.

## lock-student-repos.sh

At the code deadline, 11:59pm on Friday, September 11, 2026, this script
drops every student from write access to read access. It prints the `main`
commit it found on each repository as it goes, which is the record of what
you graded, so keep the output:

```
./scripts/lock-student-repos.sh --dry-run
./scripts/lock-student-repos.sh | tee graded-commits.txt
```

With no arguments it acts on every `ish-*` repository in the organization;
name GitHub IDs to limit it to those students. It demotes the direct
collaborators on each repository and skips staff team members, so it does
not depend on the repository name matching the student's ID.

Students keep read access and can still clone and read their work. To give
one student write access back for an extension or a regrade:

```
./scripts/lock-student-repos.sh --unlock somestudent
```

## One-time organization setup

The scripts assume the class organization is already set up this way:

  1. **Base permissions: None** (Settings > Member privileges). Students are
     organization members, so any wider base permission would give every
     student read access to every other student's repository.
  1. **A `staff` team** holding the instructor and any graders. The script
     grants it write access to each student repository and lets it bypass the
     ruleset. Pass `--staff-team ''` if you would rather not use one.
  1. **A `students` team** with no repository access of its own, for reaching
     the class through Codespaces policy, organization settings, and
     announcements. Grant this team access to nothing: any repository
     permission on it hands every student access to every other student's
     work. Pass `--student-team ''` to skip it.
  1. **`ish-assignment` marked as a template repository** (Settings >
     Template repository). The script generates each student repository from
     it, which gives the student a clean single-commit history to build their
     own revision history on top of.
  1. **A GitHub Team plan or an Education upgrade.** Rulesets on private
     repositories need one. Without it, repository creation and access still
     work, and the script warns that `main` is unprotected.
  1. **Codespaces enabled** (Settings > Codespaces) for organization members,
     with a spending limit set. `docs/DEVELOPMENT.md` tells students how to
     use them.

## Grading

Only `main` is graded, from the state of each repository when write access
ended. To clone the whole set after locking:

```
gh repo list UNM-CS587 --limit 200 --json name --jq '.[].name | select(startswith("ish-"))' |
  while read -r repo; do git clone --branch main "git@github.com:UNM-CS587/$repo.git"; done
```
