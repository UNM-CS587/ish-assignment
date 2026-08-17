The classtest/ directory holds the graded test cases, worth 100 points total
on the `.github/workflows/classtests.yml` autograder, split into the nine
categories below. These test cases mostly use the `shelltest` program to drive
testing, though some job control and pipeline tests also use custom shell
scripts.

  * Basic Tests - 15 points
    * Correct shell prompt as described in the manual page: the machine's
      hostname, a `%`, and then one space, with no newline. A page of
      typeset text cannot show you a trailing space, so it is spelled out
      here; the tests compare it byte for byte.
    * Clean shell exit at EOF: print `exit` and a newline, then terminate.
      The manual page describes termination by `quit` and by SIGTERM but
      says nothing about end of input, so follow `csh` and do this.
    * Run a command by its full path, with no arguments, one argument,
      multiple arguments, or a quoted argument
    * Run more than one command in a session, including several combined
      with `;` on one line
    * Run a command given by a relative path

  * Basic Errors - 5 points
    * Report a nonexistent program the way `csh` does
    * Report a non-executable program the way `csh` does

  * Builtin Tests - 10 points
    * `cd` builtin works, including with no arguments and inside a
      semicolon-separated command sequence (verify with `/bin/pwd`)
    * Adding an alias works
    * Printing aliases works
    * Removing an alias works
    * `quit` builtin causes the shell to exit properly and cleanly

  * Environment Tests - 20 points
    * The environment starts empty
    * Environment variables set with `setenv` are correctly passed to child
      processes
    * `printenv` (`setenv` with no arguments) works
    * `unsetenv` works
    * PATH searching works
    * PATH searching skips a nonexistent directory instead of failing, and
      setting PATH to empty is handled gracefully rather than crashing

  * Redirection Tests - 15 points
    * Redirect output to a simple file, including a file that already
      exists (rewrite, not merge)
    * Appending output to a file with `>>` works
    * Redirect combined stdout and stderr with `>&`
    * Redirect input from a simple file with `<`

  * Redirection Errors - 5 points
    * Redirecting output to a file you don't have permission to write is
      reported the way `csh` does
    * Redirecting input from a file that doesn't exist is reported the way
      `csh` does
    * Ambiguous redirection, on both input and output, is detected and
      reported

  * Corner Tests - 10 points
    * Adding an alias combined with `;` on the same line works
    * Redirecting both input and output on the same command works
    * `.ishrc` executes on startup, and the shell continues execution
      afterward
    * Environment and aliases changed in `.ishrc` are visible in the
      interactive shell afterward
    * The shell does not leak file descriptors or leave zombie child
      processes after running redirected and backgrounded commands

  * Job Control Tests - 10 points
    * Start a job in the background with `&`, reporting its job number and
      process ID
    * `&` returns to the prompt immediately rather than waiting for the job
      to finish
    * Stop a running job with ^Z, then use `bg` and `fg` to continue it in
      the background or foreground, respectively
    * List the status of running jobs with the `jobs` builtin
    * Send a signal to a job with the `kill` builtin
    * Report the completion of background jobs before the next prompt
    * Referencing a job number that doesn't exist is reported as an error

  * Pipeline Tests - 10 points
    * Run a two-process pipeline that connects the first command's stdout
      to the second command's stdin, with both processes running
      simultaneously
    * Handle pipelines with arbitrarily many processes (tested up to 10)
    * `|&` connects both stdout and stderr into the pipeline; a plain `|`
      leaves stderr going to the terminal
    * Support all job control features - backgrounding, `jobs`, ^Z/`bg`/`fg`
      - on a pipeline as a single job

Three of those categories bundle two ctest targets under a
single weight, because `ctest -R` matches test names by substring rather than
exactly:

  * `ctest -R CornerTests` also runs `resource-leak.sh`
  * `ctest -R JobControlTests` also runs `job-control-signals.sh`
  * `ctest -R PipelineTests` also runs `pipeline-signals.sh`

Those three scripts drive `ish` directly instead of going through shelltest,
because what they check -- leaked file descriptors, zombie children, and the
delivery of a real SIGTSTP to a job's process group -- cannot be expressed as
input/output matching. They are graded, and they are worth as much as the
shelltest cases beside them.

One consequence is worth planning around: Corner Tests and Pipeline Tests both
include cases that background a job, so neither category can reach full marks
until job control works. Sequence your work by feature rather than by the
category names alone.

