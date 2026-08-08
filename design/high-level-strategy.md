# `ish` High-Level Design and Implementation Strategy

Status: revision 5, for review. Revised against the repository at commit
`e2d89aa`. Revision 4 reported seven defects in the released tests; all seven
are fixed, and this revision folds the fixes into the behavior contract. No
implementation work starts until this is approved.

Section 2 lists what changed and what it costs. Section 9 lists two items I
found while checking the fixes. Neither blocks implementation, and both cost
time during development rather than points.

## 1. Scope

This document covers the design of the `ish` shell described in `ish.man.pdf`
and the order in which its features will be built. It derives the required
behavior from three sources: the manual page, the feature list in `README.md`,
and the graded cases in `classtest/`. Where the manual page is silent, the test
cases decide; where both are silent, `csh` decides. `README.md` now says the
manual page wins outright over the README, and asks that disagreements be
reported; section 9 does that.

## 2. What the released assignment changed

Revision 2 was written against `0b1a76e`, before the assignment was finalized.
Thirty-one files changed between there and the release, and twelve more between
the release and `e2d89aa`. Nine changes reach the design.

**The graded suite no longer runs in the source tree.** Every shelltest
category now goes through `classtest/run-shelltest.sh`, which copies the
fixtures into a fresh `mktemp -d`, runs `shelltest` there with `-o 20`, and
deletes the directory afterward. I confirmed the effect: a full `ctest` run
leaves `git status` clean. Three consequences for me. Each category gets its
own scratch directory, so `ctest -j` is safe. A hung shell now fails one case
in twenty seconds instead of burning the whole autograder step. And the
fixtures the tests reference are copies, so nothing I do can leave the tracked
`testscript.sh` at mode 000.

**`getpwuid` decides where home is, and `$HOME` is a trap.** `README.md` now
says so outright, and `run-shelltest.sh` enforces it: it exports
`HOME=$SCRATCH`, looks the real home up through `getpwuid`, and passes *that*
path as shelltest's `-DHOME`. A shell that reads the inherited `HOME` lands in
the scratch directory and fails the `cd` cases. Revision 3 decided to read
`HOME` out of `extern char **environ`; that is now wrong, and section 4
replaces it. Revision 2's original `getpwuid` decision was right after all.

Two consequences follow from the same change. `.ishrc` has to live in the real
home directory, because nothing the harness sets can move it, so
`ish-with-ishrc.sh` writes `$ISHHOME/.ishrc` and restores whatever was there.
And `run-shelltest.sh` now exits 77 — ctest "skipped" — for every shelltest
category if `getpwuid` reports no writable home, or if a `.ishrc` already
exists in it. Section 6 covers what that costs me.

**The error wording is `csh`'s throughout, periods included.**
`basic-error.test` expects `/bin/invalid: Command not found.`, `builtin.test`
expects `foo: Command not found.`, `environment.test` expects
`echo: Command not found.`, and `redirection-error.test` now expects
`does-not-exist.txt: No such file or directory.` — every graded exact match
ends in a period. Revision 2 needed a rule that distinguished an unset `PATH`
from an empty one; revision 4 needed a rule that special-cased one test file.
Neither is needed now. Section 4 states the single rule that replaces both.

**The harness stopped depending on the checkout and on the environment.**
`run-shelltest.sh` now runs `chmod 755` over the copied fixtures, so
`basic.test` and `pipeline.test` dropped the `/bin/chmod` lines they used to
carry in their input — and `pipeline.test`'s expected prompt count dropped with
them. The `getpwuid` lookup and the `~/.ishrc` guard moved into a sourceable
`classtest/ishhome.sh` that the three script-driven categories now source too,
and the lookup uses `python3` rather than `perl`. When `CI` is set, every guard
exits 1 instead of 77, so a skipped category can no longer be graded as a pass.

**`kill %job` is graded now.** `job-control-signals.sh` gained a case that
backgrounds a job, reads back the job number the shell reports rather than
assuming numbers are reused, sends `kill %N`, and checks with `ps` that the
process is gone or a zombie. The manual page says `kill` sends TERM and then
CONT, so a stopped job has to die too.

**The alias display format flipped.** `builtin.test` expected `foo=/bin/echo`;
it now expects `foo` TAB `/bin/echo`, which is what `csh` prints. I verified
the format against `/bin/csh -f -c 'alias foo /bin/echo; alias'` on this
machine.

**Two test cases got sharper.** `basic-error.test` no longer accepts
`No such file or directory` for a file that exists at mode 000, so `EACCES` and
`ENOENT` have to be reported apart. `redirection.test` gained a case that runs
`./pipesplit.sh >& redirectboth.txt` and requires both `stdout-line` and
`stderr-line` in the file, so `>&` cannot be implemented as stderr-only
redirection.

**The build is stricter and the point weights moved.** `-Wall -Wextra` is on
for the whole project, `CMAKE_CXX_STANDARD 20` now reaches `studenttest/`,
Boost is `REQUIRED`, and `latexmk` is optional with `generate_pdf` no longer an
`ALL` target. Separately, both workflows now run on `develop` as well as
`main`, so merging a feature branch to `develop` returns a graded score.

Three revision-2 concerns are fixed and I have removed them: `latexmk` is no
longer `REQUIRED`, the devcontainer installs gtest and TeX, and the
`classtests.yml` YAML branch-list bug is gone.

## 3. Starting point

Verified on this machine at `166be4e`: `cmake` configures, `cmake --build`
produces `ish` and `parser_test` with no warnings under `-Wall -Wextra`, and
`ctest` reports 12 of 13 targets failing in 40 seconds with no hangs and no skips.
`StudentTests` is the one that passes — all 32 GoogleTest parser cases are
green. The starter code provides:

- `src/parser.hpp` — a Boost Spirit X3 grammar that parses words, quoting,
  backslash escapes, redirection operators, and `;`-separated command lists. It
  does not parse `|`, `|&`, or `&`.
- `src/command.hpp` — an `ish::command` holding a name, arguments, a
  redirection list, and a foreground flag no one sets.
- `src/ish.cpp` — a read loop that prints `> `, parses each line, and dumps the
  parse tree. It executes nothing.
- `classtest/` — the graded cases, the `run-shelltest.sh` harness, and three
  driver scripts: `job-control-signals.sh`, `pipeline-signals.sh`, and
  `resource-leak.sh`.
- `studenttest/parser_test.cpp` — 32 GoogleTest cases for the parser.

Everything below the parse tree has to be written.

The autograder's nine `ctest -R` patterns are prefix matches, so three
categories run two targets each: `CornerTests` also runs
`CornerTestsResourceLeak`, `JobControlTests` also runs
`JobControlTestsSignals`, and `PipelineTests` also runs
`PipelineTestsSignals`. `README.md` now documents all three. Those scripts
carry the same weight as the shelltest cases beside them.

Category weights, which set the order of work in section 7: Basic 15,
Environment 20, Redirection 15, Builtin 10, Corner 10, Job Control 10, Pipeline
10, Basic Errors 5, Redirection Errors 5.

## 4. Behavior contract

The test cases pin down output formats the manual page leaves loose. I
confirmed the shelltest v3 semantics below by experiment rather than reading
them off the file format, because three of them constrain the design.

**Prompt.** `run-shelltest.sh` computes `PROMPT="$(uname -n)%"` at test time.
Expected lines such as `PROMPT hello` show the prompt carries a trailing space,
so the prompt string is `gethostname()` plus `"% "`. `README.md` now states
this explicitly. The current `> ` prompt is wrong. `uname -n` reports what
`gethostname(2)` returns, so the two cannot disagree; the configure-time
hostname that revision 2 had to worry about is gone.

**The home directory comes from `getpwuid`, never from `$HOME`.**
`builtin.test` runs `cd` with no argument and compares `/bin/pwd` against
shelltest's `HOME`, which `run-shelltest.sh` sets to the value
`getpwuid(getuid())->pw_dir` returns. The inherited `HOME` points at the
scratch directory instead, deliberately, so that a shell reading it fails.
`ish-with-ishrc.sh` installs its fixture in the same `getpwuid` directory. So
`ish` calls `getpwuid(getuid())` once at startup, keeps `pw_dir`, and uses it
for `cd` with no argument and for `~/.ishrc`.

This is consistent with "initially no environment variables are set for `ish`."
`ish` has no `HOME` to read, so the password file is the only place the
information exists. The `envp` handed to `execve` starts empty and grows only
through `setenv`; the home directory never enters it, and `corner.test`'s
`printenv` line confirms that. `ish` therefore needs nothing from
`extern char **environ`, which is why `int main()` with no parameters is now
the right signature.

**Exit status must always be zero.** No graded case carries a `>=` field, and
shelltest treats an omitted exit-status field as *expected zero*, not as
"ignore". So `ish` returns 0 at EOF and from `quit` no matter how the last
command fared. This is easy to get wrong by propagating a child's status.

**Stderr must be empty unless a case says otherwise.** An omitted `>2` field
also means *expected empty*, not "ignore". Most graded cases omit it. That
rules out incidental diagnostics anywhere on the normal path: no warning when
`~/.ishrc` is absent, no "cannot set terminal process group" when `ish` runs
without a controlling terminal, no parse or job-table chatter. Every message
`ish` prints has to be one a test asks for.

**Termination.** Both EOF and `quit` print `exit` and a newline, matching
`csh`. `basic.test` expects `PROMPT exit` after empty input, and `builtin.test`
expects the same after `quit`. `README.md` now states the EOF behavior.

**Diagnostics go to stderr.** Several cases assert stdout holds nothing but
prompts while stderr carries the message, and `README.md` now says so for every
diagnostic. `ish.cpp`'s current `Parse error:` write to stdout has to move.

**Error message wording.** `>2` blocks with body text are exact matches — I
confirmed that a trailing period alone fails a case. `README.md` now
states the rule, and the tests agree with it. `csh` splits its diagnostics by
*where* the failure happened and by *which* `errno` came back, not by whether
the name contains a `/`:

- A failed `execve` with `ENOENT` prints `name: Command not found.` This holds
  whether the name is a bare word `PATH` could not resolve or a full path that
  does not exist. `csh` reaches it by leaving its saved message empty for
  `ENOENT` and falling through to a fixed string.
- A failed `execve` with any other `errno` prints `name: strerror(errno)`.
  `EACCES` on an existing file therefore prints `Permission denied`, not
  `Command not found.`
- A failed `open` or `dup2` for a redirection prints `path: strerror(errno)`.

All three end with a period, and both graded exact matches now agree.

| Case | `ish` must print | Real `csh` prints |
| --- | --- | --- |
| `/bin/invalid` | `/bin/invalid: Command not found.` | same |
| `./testscript.sh` mode 000 | must contain `Permission denied` | `./testscript.sh: Permission denied.` |
| `foo`, `PATH` unset | `foo: Command not found.` | same |
| `echo`, `PATH` set empty | `echo: Command not found.` | same |
| `> nowrite/redirectout.txt` | must contain `nowrite/redirectout.txt: Permission denied` | same |
| `< does-not-exist.txt` | `does-not-exist.txt: No such file or directory.` | same |
| `ls > foo > bar` | must contain `Ambiguous output redirect` or `syntax error` | `Ambiguous output redirect.` |
| `ls < foo < bar` | must contain `Ambiguous input redirect.` or `syntax error` | `Ambiguous input redirect.` |

No row diverges from `csh` any longer. One `perror`-style helper that prints
`name: message.` to stderr covers every row, which removes the whole class of
bug revisions 2 through 4 kept tripping over.

**Other formats.** `alias` with no arguments prints `name` TAB `value`.
`setenv` with no arguments prints `NAME=value`. The two builtins look parallel
and are not. Backgrounding prints `[1] 1234`; backgrounding a pipeline prints
every stage's PID, `[1] 1234 1235`. `jobs` must print a line containing `[N]`,
the word `Stopped` for a stopped job, and the command text. The signal scripts
match these with case-insensitive regexps, so exact spacing is free.
`README.md` now documents all four formats, so they are specified rather than
inferred from the cases.

**Job references are job numbers.** `fg %1`, `bg %2`, and `kill %1` name jobs,
not process IDs, and `fg %99` with no jobs must produce a stderr message
containing `99`. `README.md` settles this against the manual page's `%1234`
example. `job-control-signals.sh` reads the job number back out of the
shell's own `[N] pid` line rather than assuming a value, so `ish` is free to
reuse numbers or not — but `kill %N` has to signal the job that line named, and
the process has to be gone or reaped shortly after. The manual page's TERM-then-
CONT sequence matters here: a stopped job that receives only TERM stays
stopped.

**Startup.** `corner.test` runs `ish-with-ishrc.sh`, which installs
`ishrc.ish` as `.ishrc` in the `getpwuid` home, saving and restoring anything
already there. The file sets an environment variable and an
alias in addition to echoing, and the expected output requires both to survive
into the interactive session:

```
Out of ISHRC
PROMPT ISHRCVAR=from_ishrc
PROMPT hello from alias
PROMPT exit
```

So `.ishrc` runs in the shell process itself, not a subshell, and prints no
prompt while it runs. The `printenv` line also confirms `ish` injects nothing
into the exported environment on its own — in particular, not the home
directory it read for its own use.

**Resource limits.** `resource-leak.sh` compares `ish`'s open descriptor count
against a startup baseline after three redirected commands and again after
three background jobs, requiring exact equality both times. It then requires
zero zombie children, and requires the output after `jobs` to contain nothing
matching `running|stop` once the background jobs have finished. Three design
consequences: the parent closes every redirection and pipe descriptor once the
children hold their copies, every child is reaped, and a finished job leaves
the job table once its completion has been reported. A completion report
worded like `csh`'s `[1]  Done  /bin/sleep 0.2` satisfies the last check; one
that says `Running` or `Stopped` does not.

## 5. Architecture

The shell is a `Shell` object holding all mutable state, driven by a read loop.
One module per report section, which keeps the code and `report/report.tex`
aligned.

```
ish.cpp          main(); constructs Shell, runs .ishrc, runs the read loop
shell.{hpp,cpp}  Shell: owns env, aliases, job table; read loop; .ishrc
parser.hpp       grammar (extended for '|', '|&', '&')
command.{hpp,cpp} command and pipeline data structures
environment.{hpp,cpp}  variable map, envp construction, PATH resolution
builtins.{hpp,cpp}     cd, alias, unalias, setenv, unsetenv, jobs, bg, fg,
                       kill, quit
executor.{hpp,cpp}     redirection setup, fork/execve, pipe wiring, pgroups
jobs.{hpp,cpp}         job table, waitpid polling, status reporting
```

Data flow for one input line:

```
getline
  -> parser              vector<pipeline>
  -> alias substitution  (one level, command name only)
  -> per pipeline:
       builtin?  -> run in the shell
       else      -> resolve every stage's executable path
                 -> open redirection files in the parent
                 -> pipe(), fork(), setpgid(), dup2(), execve()
                 -> close every descriptor the parent still holds
                 -> foreground: tcsetpgrp + waitpid(-pgid, WUNTRACED)
                    background: record job, print "[N] pid..."
  -> report and purge finished jobs
  -> prompt
```

### Parser extension

The grammar grows one level to match the manual page's `list > pipeline >
simple command` structure:

```
list     := pipeline % (';' | '&')      // '&' backgrounds the pipeline before it
pipeline := commandel % ('|&' | '|')    // '|&' merges the stage's stderr
```

`parseCommands` returns `std::vector<ish::pipeline>` instead of
`std::vector<ish::command>`. A `pipeline` holds its stages, a background flag,
and the source text for `jobs` to print. The per-stage "merge stderr into the
pipe" flag lives on the upstream `command`. The existing background flag on
`command` moves to `pipeline`, since a job is a pipeline, not a command.

Two points revision 1 got wrong and revision 2 corrected still hold. `:` is
already in `ishpunct`, so `setenv PATH /bin:/usr/bin` parses today. The
existing `x3::symbols` table already longest-matches `>>&` over `>>`, so only
the new `|&` versus `|` alternation needs attention; putting the pipe operators
in a symbol table of their own gets the same behavior for free. `%` is already
a token character, so `fg %1` parses.

Changing the parser's return type means `studenttest/parser_test.cpp` is
updated in the same branch.

### Process model

Every pipeline, of one stage or ten, becomes one job in one process group. That
single model makes job control apply to pipelines for free, which is what the
Pipeline Tests require.

- The parent resolves all executables *before* forking anything, so a bad name
  anywhere aborts the whole pipeline without leaving orphans, as the manual
  page requires.
- The parent opens redirection files, so `< nonexistent` and an unwritable `>`
  target are reported by the shell before any process starts.
- The first child's PID becomes the process group ID. Both parent and child
  call `setpgid` to close the race over which runs first.
- Foreground jobs get the terminal with `tcsetpgrp`, guarded by
  `isatty(STDIN_FILENO)`. Every graded test drives `ish` from a file or a FIFO
  with no controlling terminal, and an unguarded `tcsetpgrp` would either fail
  loudly or stop the shell. Process groups stay mandatory:
  `pipeline-signals.sh` sends `SIGTSTP` to the pipeline's process group and
  times out if the shell shares it.
- The shell ignores `SIGINT`, `SIGQUIT`, `SIGTSTP`, `SIGTTIN`, and `SIGTTOU`;
  children reset them to `SIG_DFL` between `fork` and `execve`.
- Status is polled with `waitpid(-1, ..., WNOHANG | WUNTRACED | WCONTINUED)`
  before each prompt. The manual page forbids a `SIGCHLD` handler.
- Every pipe and redirection descriptor is closed in the parent immediately
  after the forks and in each child before `execve`. `resource-leak.sh` checks
  this by counting descriptors.

### Restricted interfaces

The assignment bans `system`, the non-`execve` `exec` variants, `getenv`,
`setenv`, `putenv`, and `unsetenv`; `README.md` is now explicit that the ban
covers the C library routines and not the builtins of the same names. `ish`
therefore keeps a `std::map<std::string, std::string>` of exported variables,
builds a fresh `char *const envp[]` at each `execve`, and searches `PATH`
itself with `stat` and `access`. It reads nothing from the inherited
environment: the home directory comes from `getpwuid`, as section 4 describes,
and is never exported.

`environment.test` covers both `PATH` corner cases directly: a list containing
`/does/not/exist` must skip that entry and find the command later in the list,
and `setenv PATH` with no value must leave a bare name unresolvable while a
full path still runs.

## 6. Testing constraints the harness imposes

Worth knowing before writing tests, because two of these will otherwise look
like shell bugs.

**A skip still reads as a pass locally, and there are three ways to skip.**
`RedirectionTests` and `RedirectionErrors` bail when run as root, because root
ignores the mode-500 `nowrite` directory those cases redirect into. All twelve
graded targets bail if `getpwuid` reports no writable home, and all twelve bail
if a `.ishrc` already sits in that home; `classtest/ishhome.sh` holds both
checks and every category sources it. When `CI` is set the bail is exit 1, so
the autograder cannot be fooled. Locally it is still exit 77, `ctest` still
prints `100% tests passed`, and the run still exits 0. I verified all of this:
with a `~/.ishrc` in place the categories report `***Skipped`, `CI=1` turns the
same runs into `***Failed`, and my `~/.ishrc` came back unmodified.

So my routine is to read `ctest`'s per-test lines rather than its summary,
treat any skip as a broken setup, and run `CI=1 ctest` before opening a pull
request. The `.ishrc` path is the one that will bite me, because the assignment
tells me to implement `~/.ishrc` and I will naturally have one, and there is no
`HOME` override that moves it: `ish` finds the file through `getpwuid`. I will
keep `~/.ishrc` out of the way while developing and install it only to
hand-test startup. Section 9 item 1 covers the case where the tests leave one
behind for me.

Student tests accumulate across phases in `studenttest/`. `studenttests.yml`
now runs `ctest -R StudentTests`, so a `.test` file registered through
`add_test` in `studenttest/CMakeLists.txt` runs in CI — but only if its name
matches that pattern. The parser gets GoogleTest cases; behavior shelltest can
express gets a student `.test` file mirroring the class cases with different
inputs, so passing is evidence of a general implementation rather than of
matching the graded strings.

## 7. Implementation strategy

One feature per branch off `develop`, each branch adding its tests and its
`report/report.tex` section alongside the code, each merged by pull request.
The phases are ordered so that no phase depends on a later one, and the class
tests now run on `develop`, so every merge returns a graded score.

| # | Branch | Turns on | Pts |
| --- | --- | --- | --- |
| 0 | `chore/build-baseline` | build and test run locally | — |
| 1 | `feature/shell-loop` | prompt, EOF, `exit` | part of 15 |
| 2 | `feature/exec` | Basic Tests, Basic Errors | 20 |
| 3 | `feature/environment` | Environment Tests | 20 |
| 4 | `feature/builtins` | Builtin Tests | 10 |
| 5 | `feature/redirection` | Redirection, Redirection Errors | 20 |
| 6 | `feature/ishrc` | `corner.test` cases | part of 10 |
| 7 | `feature/pipeline-parsing` | parser unit tests | — |
| 8 | `feature/pipelines` | foreground `pipeline.test` cases | part of 10 |
| 9 | `feature/job-control` | Job Control, rest of Pipeline and Corner | 20 |
| 10 | `docs/report` | report and cleanup | — |

Corner Tests and Pipeline Tests each split across two phases, because
`ctest -R CornerTests` also runs `resource-leak.sh`, which needs background
jobs, and two `pipeline.test` cases background a pipeline. Both categories
finish at phase 9. Phases 1 through 6 are worth 65 of the 100 points and depend
on nothing after them, so an unfinished assignment fails in the least expensive
order.

**Phase 0 — build baseline.** Record the toolchain and confirm `ctest` runs end
to end against the unmodified starter code. Done: `cmake` configures with
`/Library/TeX/texbin` on `PATH`, the build is warning-free under
`-Wall -Wextra`, 12 of 13 targets fail cleanly in 40 seconds with no skips, and
`git status` is clean afterward. Each later phase re-checks all four.

**Phase 1 — shell loop.** Replace `ish.cpp`'s dump loop with a `Shell` object:
`gethostname()`-based prompt, `exit` on EOF, exit status 0, empty lines
ignored, parse errors to stderr. Verify: the first case in `basic.test`.

**Phase 2 — process creation.** `fork`, `execve`, `waitpid` for a foreground
command given by absolute or relative path, plus the two error families from
section 4. `basic-error.test` now separates `EACCES` from `ENOENT`, so the
child reports the real `errno` rather than a fixed string. The environment
passed to `execve` is empty, which is correct at this stage. Verify: Basic
Tests and Basic Errors.

**Phase 3 — environment.** The variable map, `setenv`/`printenv`/`unsetenv`,
`envp` construction, and `PATH` search including the missing-directory and
empty-`PATH` cases. Verify: Environment Tests.

**Phase 4 — builtins and aliases.** `cd` via `chdir`, using the `getpwuid`
home-directory rule from section 4; `quit`; the alias table, one-level substitution applied after
parsing and before execution, and `alias`/`unalias` with the tab-separated
display format. Verify: Builtin Tests, and the alias-with-semicolon case in
`corner.test`.

**Phase 5 — redirection.** Parent-side `open`, child-side `dup2`, truncate
versus append, `>&` and `>>&` for both streams, ambiguity detection, and
closing the parent's copies. Verify: Redirection Tests including the new
combined-stream case, Redirection Errors, and the input-plus-output case in
`corner.test`.

**Phase 6 — startup file.** Read `.ishrc` from the `getpwuid` home directory in
the shell process through
the same command path with prompting suppressed, then continue into the
interactive loop. A missing or unreadable file is silently ignored. Verify: all
four `corner.test` cases, including alias and environment persistence.

**Phase 7 — pipeline and background parsing.** Grammar and data structure
changes only, no execution changes: the `pipeline` type, `|`, `|&`, `&`, and an
updated `studenttest/parser_test.cpp`. A one-stage pipeline must execute
exactly as before, so every previously passing category stays green. Verify:
parser unit tests, no regressions.

**Phase 8 — pipeline execution.** `pipe`, the fork loop, descriptor closing,
stderr merging for `|&`, pre-resolution of every stage, and process group
creation. Foreground only. Verify: the four non-background cases in
`pipeline.test`.

**Phase 9 — job control.** The job table, `&` and its `[N] pid...` report,
`tcsetpgrp` handoff, signal disposition, `waitpid` polling, status reports and
job purging before the prompt, and the `jobs`, `bg`, `fg`, and `kill` builtins.
Because phase 8 already made every job a process group, pipelines get job
control without separate work. Verify: Job Control Tests, the backgrounded
`pipeline.test` cases, and all three driver scripts including
`resource-leak.sh`.

**Phase 10 — report and cleanup.** Fill in `report/report.tex` and confirm
`cmake --build build --target generate_pdf` succeeds; it is no longer built by
default, so nothing else will catch a LaTeX error. The report sections on my
experience, my verification reasoning, and my AI tool usage are mine to draft.

## 8. Verification beyond the graded tests

The assignment grades understanding, not just passing tests, and the in-class
closed-book test grades it again, so each phase carries checks the class tests
do not make:

- Path resolution against a `PATH` with duplicate, trailing-colon, unreadable,
  and non-directory entries.
- Redirection where the target is a directory, a symlink, or `/dev/full`.
- A pipeline whose first stage exits before the last stage reads, and one whose
  last stage exits first, checking for `SIGPIPE` and for a hang.
- A builtin as a non-final pipeline stage, which the manual page says runs in a
  subshell.
- Descriptor and zombie counts after several hundred redirected and
  backgrounded commands, not the handful `resource-leak.sh` uses.
- Quoted, escaped, and empty arguments reaching `execve` intact, checked with a
  helper that prints its own `argv` and `envp`.
- The manual page features nothing in `classtest/` asserts: `>>&`,
  double quotes and backslash escapes, `alias word1` with one argument, a
  pipeline aborted because one component failed to resolve, TERM terminating
  the shell and QUIT being ignored, and the 1024-character word limit.

## 9. Items for you before implementation starts

Every item revision 4 raised is fixed, and I re-ran the suite to confirm it:
12 of 13 targets fail in 40 seconds with no skips, `git status` is clean
afterward, and the `~/.ishrc` guard skips locally, fails under `CI=1`, and
leaves an existing file untouched. Two things surfaced while checking, both
about how the harness behaves when my shell misbehaves. Neither costs points.

### 1. A timed-out `.ishrc` case leaves the fixture in my home directory

`corner.test`'s last two cases run `./ish-with-ishrc.sh SHELL`, which copies
`ishrc.ish` to `$ISHHOME/.ishrc` and restores the previous state from a
`trap ... EXIT INT TERM`. When `shelltest -o 20` times a case out, it kills
that wrapper hard enough that the trap does not run. I reproduced it outside
the tree: a shell that hangs leaves `.ishrc` sitting in the home directory
afterward.

The `.ishrc` cases are exactly the ones a half-built shell hangs on, so I
expect to hit this. The consequence is self-perpetuating: on the next run the
`ishhome.sh` guard finds a `.ishrc`, skips every category — or, under `CI`,
fails every category — and tells me to move aside a file I never created. The
guard prevents the run that would have cleaned it up.

`run-shelltest.sh` can close it without any content comparison. It already
traps `EXIT INT TERM`, and by the time it starts `shelltest` the guard has
proved `$ISHHOME/.ishrc` does not exist, so anything there at the end belongs
to the test run:

```sh
cleanup() {
    chmod -R u+rwX "$SCRATCH" 2>/dev/null || true
    rm -rf "$SCRATCH"
    # The guard above proved this file did not exist when we started, so
    # whatever is here now is the fixture a timed-out case left behind.
    # shelltest kills ish-with-ishrc.sh too hard for its own trap to fire.
    rm -f "$ISHHOME/.ishrc"
}
```

That trap fires on the `TIMEOUT 300` `ctest` sends and on a Ctrl-C, so only a
`SIGKILL` escapes it. Worth doing: without it, the first hang costs me a
debugging session on a message that describes a condition I did not cause.

### 2. `pipeline-signals.sh` stops itself when `ish` does not call `setpgid`

The script looks up the pipeline's process group and sends `SIGTSTP` to it:

```sh
PGID=$(ps -o pgid= -p "$PIPE_PID" | tr -d ' ')
kill -TSTP "-$PGID"
```

A shell that has not implemented process groups yet leaves its children in its
own group, and `ish` itself is in the script's group, because `bash` does not
enable job control in a script. I confirmed both: a background job in a
non-interactive `bash` script shares the script's PGID, and adding `set -m`
gives it a group of its own. So `-$PGID` is the test script's own process
group, and the script stops itself. `ctest` then waits out its full 300-second
`TIMEOUT` before failing.

The script's comment anticipates the failure — "the checks below will time out
and fail" — but not the cost. Five minutes per run, on the category I will be
iterating on hardest, for the whole stretch between starting pipelines and
finishing job control.

Adding `set -m` alongside `set -u` fixes it. `ish` then gets its own process
group, `SIGTSTP` reaches `ish` and its children rather than the script, and
the checks fail in about five seconds instead of five minutes.

One consequence to handle in the same change: a stopped `ish` does not die
from the `kill "$ISH_PID"` in `cleanup`, and `wait "$ISH_PID"` then blocks
forever, which reintroduces the 300-second wait through the back door. A
`kill -CONT "$ISH_PID"` before the `kill` closes that. It is worth adding to
`job-control-signals.sh` too, which has the same `cleanup` and can leave a
stopped shell behind the same way.
