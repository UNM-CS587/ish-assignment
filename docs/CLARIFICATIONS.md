## Appendix: Additional Shell Semantics Clarifications

Below are additional semantics clarifications for the shell where the 
test case and manual page documentation are unclear. Note that I exposed 
these ambiguities by implementing the shell myself in collaboration with an
AI. This kind of collaboration is what is required to use these tools well and
to make AI coding effective.

  - A job reference is a `%` followed by a **job number** -- the same number
    `jobs` lists and the `[N]` startup message reports -- so `fg %1`, `bg %2`,
    and `kill %1` all name job 1 and job 2, not processes 1 and 2. The manual
    page's `%1234` example happens to use `1234` as the job number, which 
    only coincidentally matches the process ID printed on the line above it. 
    Use the job number, not the process ID, as the argument to 
    `fg`, `bg`, and `kill` builtins.

  - `cd` with no argument and the `~/.ishrc` startup file both need the user's
    home directory, and the manual page says no environment variables are set 
    for `ish` initially so you cannot read the `$HOME` environment variable. 
    Instead, get the directory from `getpwuid(3)`, as `csh` does. 

  - Two graded cases compare the wording of an error byte for byte, so report 
    errors the way `csh` does. A failed `execve(2)` with `ENOENT` prints 
    `name: Command not found.`, both for a bare name `PATH` could not resolve 
    and for a full path that does not exist. A failed `execve(2)` with any 
    other `errno` prints `name: ` followed by `strerror(errno)`, so a file that 
    exists but is not executable prints `Permission denied.` A redirection that 
    cannot be opened prints the path, a colon, and `strerror(errno)`. All three 
    end with a period.

  - Builtins normally run in the main shell and can read and write redirected
    stdin, stdout, and stderr, but builtins in pipelines or in the background 
    should run in subshells, separate from the main shell process; this is the 
    same strategy `csh` uses.

  - Alias substitution happens per command, immediately before that command 
    runs, rather than across the whole line at parse time. This is a deliberate
    departure from `csh`, and a graded corner case turns on it: `alias foo 
    /bin/echo; foo FOO!` must print `FOO!`, where `csh` prints `foo: Command 
    not found.` because `foo` is not yet an alias when `csh` substitutes the 
    second command on the line.

  - `ish` allows only one level of aliasing per the man page ("if an alias uses 
    an alias, the second alias is ignored."). This too differs from `csh`, 
    which keeps substituting until nothing changes: after `alias a /bin/echo` 
    and `alias b a`, the command `b hi` prints `hi` under `csh` and reports 
    `a: Command not found.` under `ish`.

  - The manual page gives no wording for `cd`, `alias`, or `unalias` called with
    the wrong number of arguments. No graded case checks these, and any 
    reasonable diagnostic on stderr is acceptable. `csh`'s wording (e.g., 
    `cd: Too many arguments.`, `unalias: Too few arguments.`) is a safe choice,
    and it matches the wording the manual page gives for `setenv`.

  - `kill` does not take a signal number argument (e.g., `kill -9 %1` reads `-9`
    as the job reference and reports no such job).

  - `&` terminates a command; it does not separate commands the way `;` does. 
    `/bin/sleep 5 & /bin/echo hi` is a parse error rather than two commands, so 
    the grammar you extend does not have to handle `&` mid-line.

  - `&` on a lone builtin (e.g. `setenv A B &`) causes it to run in a subshell
    which discards the state the builtin changed.

  - Several other graded cases compare output against `csh`'s formats, so match
    those too. `alias` with no arguments prints one alias per line as the name, 
    a tab, then the value. `setenv` with no arguments prints one variable per 
    line as `NAME=value`. `jobs` prints the job number in brackets, then the 
    job's status, then the command the job is running, and calls a job stopped 
    by a TSTP signal `Stopped`. The report the shell prints when a background 
    job finishes carries the same bracketed job number and names the command 
    as well.

  - A skipped test category is not a passed one. `ctest` refuses to run some
    categories when your environment cannot support them, e.g. if you are root, 
    you have no home directory, or you already have a `~/.ishrc` that the 
    startup-file cases would otherwise overwrite. It then can report `100% 
    tests passed` incorrectly. Be sure to read `ctest`'s per-test lines rather 
    than its summary, and fix the condition the skip message names. The 
    autograder runs the same tests with `CI` set, where those conditions fail 
    instead of skipping.

  - The autograder does not check every feature the manual page specifies. It
    checks the nine categories above. The rest of the manual page is still
    required, is still worth implementing, and is fair game on the in-class
    test.

  - If you are in doubt about the functionality of `ish` or how it should behave
    in a particular situation, model the behavior on that of `csh`.  If you have
    specific questions about the project, ask in the class Discord.

