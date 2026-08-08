# File containing AI-generated notes from previous sessions to allow 
# agents to preserve key information between runs

## Session: 2026-08-07 — design revision against the released assignment

**State.** Design only. No shell code written yet; `src/` is still the
unmodified starter. `design/high-level-strategy.md` is now at revision 5,
against `e2d89aa`. Revisions 3 and 4 were written against test files that have
since been corrected; every defect revision 4 reported is fixed.

**Baseline verified this session.** `cmake` configures, the build is
warning-free under `-Wall -Wextra`, `ctest` reports 12 of 13 targets failing in
40 seconds with no hangs and no skips, `StudentTests` (32 parser cases) passes,
and `git status` is clean afterward. The `~/.ishrc` guard was checked directly:
with a file in place the categories report `***Skipped`, `CI=1` turns the same
runs into `***Failed`, and the file comes back unmodified.

**Design decisions that are not obvious from the code.**

- The home directory for `cd` with no argument and for `~/.ishrc` comes from
  `getpwuid(getuid())->pw_dir`, never from `HOME`.
  `classtest/run-shelltest.sh` deliberately points `HOME` at the scratch
  directory and passes the `getpwuid` result as shelltest's `-DHOME`, so a
  shell calling `getenv("HOME")` fails the `cd` cases. `ish` reads nothing
  from the inherited environment, which is why `int main()` with no parameters
  is the right signature.
- Error wording follows `csh`, split by where the call failed and by `errno`,
  not by whether the name contains a `/`: `execve` failing with `ENOENT`
  prints `name: Command not found.`; `execve` failing with any other `errno`
  prints `name: strerror(errno)`, so `EACCES` prints `Permission denied`; a
  failed `open` for a redirection prints `path: strerror(errno)`. All three
  end with a period; both graded exact matches agree, and `README.md` states
  the rule. One `perror`-style helper covers every case.
- Every pipeline, of one stage or ten, becomes one job in one process group, so
  job control applies to pipelines without separate work.
- `tcsetpgrp` is guarded by `isatty(STDIN_FILENO)`: every graded test drives
  `ish` from a file or a FIFO with no controlling terminal, and stderr must be
  empty unless a case asks otherwise.

**Open items for the instructor** (section 9 of the design doc). Nothing
blocks implementation. Two left, both about how the harness behaves when the
shell misbehaves. A `shelltest -o 20` timeout kills `ish-with-ishrc.sh` before
its restore trap runs, leaving the fixture `.ishrc` in the real home directory;
the next run's guard then skips or fails everything and blames a file the
student never created. And `pipeline-signals.sh` sends `SIGTSTP` to a process
group that is its own until `ish` calls `setpgid`, so it stops itself and burns
the full 300-second `ctest` timeout; `set -m` gives `ish` its own group, and a
`kill -CONT` in `cleanup` keeps a stopped shell from hanging `wait`.

**Trap to remember.** A `~/.ishrc` in the real home makes every category exit
77 and the whole run report `100% tests passed`. Read the per-test lines, not
the summary; run `CI=1 ctest` before a pull request, where the same conditions
fail instead; and keep `~/.ishrc` out of the way while developing.

**Next steps.** Get the design approved, then start phase 0/1 on
`chore/build-baseline` and `feature/shell-loop`. Phases 1 through 6 are worth
65 points and depend on nothing after them.
