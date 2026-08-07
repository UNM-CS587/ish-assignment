# CS587 - Advanced Operating Systems Project 1: The `ish` shell

## Due Date
  * Code and Report Due Date: Friday, September 11, 2026, 11:59pm
  * In-class Code Test: Monday, September 14, 2026, 9:00am

## Assignment Overview
Your task is to design and implement a basic UNIX command shell in a provided
C++ framework with the assistance of AI coding agents. As part of this task, 
you also have to convince your boss (the class instructor) that:
  1. You actually understand the key concepts of the problem you're solving.
  2. The completed program actually solves the problem generally instead of
     regurgitating answers to the provided test cases.

The Required Program Features section below lists the features you must
implement and demonstrate understanding of for full credit.

For this assignment, you will:
  1. Turn in a GitHub repository that includes all of your source code, test
     cases, and revision history. This revision history should document your
     development process as you incrementally add features to the shell,
     including any AI prompting and token usage associated with these commits.
     If you submit a repository with minimal revision history (e.g. a single 
     commit to the main branch that implements all features), you will be
     required to meet with the professor, explain your development process
     and code, and demonstrate that you understand the code to receive credit.
  2. Turn in a written report describing the high-level approach that your 
     source code uses to provide the key features you implemented (as a LaTeX
     document in the turned-in GitHub repository - source code in 
     [report/report.tex](report/report.tex), which the build system will 
     compile into report.pdf).
  3. Take an in-class, closed-book test on how your shell implements these key
     features.

*Your final grade on this assignment will be the minimum of your grade on 
these three components.  Correct code that passes all test cases without
demonstrated student understanding will receive no credit!*

## Required Program Features

Your shell *must* use the C++ parser provided in the src/ directory and 
implement the features described in the included ISH manual page (ish.man.pdf). 
Note that the parser does not yet parse job control or pipelines, but for full 
credit on the assignment you will need to extend it to do so.

The manual page is the specification. Where this README and the manual page
disagree, the manual page wins; tell me about the disagreement so I can fix
the README. The page is reprinted from the course this assignment came from
originally, so its footer reads `CSc 552` rather than CS587. Ignore that.

The classtest/ directory holds the graded test cases, worth 100 points total
on the `.github/workflows/classtests.yml` autograder, split into the nine
categories below. Three of those categories bundle two ctest targets under a
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

A job reference is a `%` followed by a **job number** -- the same number
`jobs` lists and the `[N]` startup message reports -- so `fg %1`, `bg %2`,
and `kill %1` all name job 1 and job 2, not processes 1 and 2. The manual
page's `%1234` example reuses the process ID printed on the line above it,
which contradicts its own job control paragraph; when the manual page is
unclear, follow `csh`, and `csh` takes a job number here. The test cases
assume the job number.

Every diagnostic `ish` prints -- a command it cannot find, a redirection it
cannot open, a job number that isn't there -- goes to stderr, not stdout. The
test cases compare the two streams separately, so a shell that reports errors
on stdout fails them even when the wording is right.

The autograder does not check every feature the manual page specifies. It
checks the nine categories above. The rest of the manual page is still
required, is still worth implementing, and is fair game on the in-class code
test.

If you are in doubt about the functionality of `ish` or how it should behave in
a particular situation, model the behavior on that of `csh`.  If you have 
specific questions about the project, ask in the class Discord.

## Program Requirements, Restrictions, Starter Source Code

Your shell will be written in C++, compile using cmake, and produce an 
executable named `ish`. I have provided you this GitHub repository with the
basic compilation and software engineering infrastructure, most of 
a C++ parser for the assignment (the [src/](src/) directory), functional
testcases which will be used to grade the correctness of your program, and
unit tests for the parser. Note, however, the provided C++ starter code does *not* 
parse background jobs or pipelines; to complete that portion of the 
assignment, you will need to (with AI help) add support for parsing job
control and pipelines to the provided parser.

In terms of the system interfaces and libraries you may and may not use:
  1. You may use general data structures abstractions to help you store, 
     organize, and iterate through data (lists, hash tables, trees, etc.).
     This includes many of the C++ standard template library functions and 
     Boost library functions.
  1. You may *not* use library functions that make process or environment
     management system calls on your behalf; your shell should call all
     needed system calls directly. Specifically, you *must* use the `fork`
     and `execve` C system calls to start new processes and the `dup`,
     `dup2`, or `pipe` system calls to handle file redirection. You may
     *not* use other variants of `exec` or the `system` call provided by
     the C standard library, their C++ equivalents, or any other mechanism
     for creating processes or redirecting file I/O. 
  1. Similarly, you must manually maintain the environment as data 
     passed to the `execve` call. The manual page names `putenv` and 
     `getenv` specifically, and the rule above already rules out `setenv`,
     `unsetenv`, and their C++ equivalents. Note that `setenv` and 
     `unsetenv` are also the names of two builtins you must implement; the
     ban is on the C library routines, not on the builtins.
  1. You must cite any library you use outside of the standard C and C++ 
     libraries, including any Boost libraries you use besides the Boost
     Spirit X3 Parser already used.

Your code must be "clean" -- it must compile without warnings or 
unresolved references on a standard UNIX system with the Boost C++ libraries and
latex installed. I also expect your programs to be well organized and easy 
to read (in addition to correct).

## Required Report 
In addition to the source code you must implement, you must also write a report
describing how you implemented each of the features described above using the
UNIX system call interface, how you verified the correctness of these modules
so that they don't just pass the functional tests but actually implement the 
feature generally, and your experience using AI tools to implement these 
features. You may use AI tools to assist you in editing and revising this 
report, but *you should provide the draft text, bullet points, or other 
content* that the AIs help you revise describing your code and how it works.
A suggested outline for this report is included in 
[report/report.tex](report/report.tex).

## Provided Software Testing Features

Testcases that test correctness of the shell output and of key shell components
are included in the provided repository. Specifically:
  - Class grading tests on which your shell is graded are specified in the 
    classtest/ directory and are run by the github workflow described in 
    .github/workflows/classtests.yml; these tests are purely functional and 
    work by executing the shell on commit to the main branch. *Do not change 
    classtests.yml or any of the tests in the classtest/ directory. Changing
    class testing infrastructure to increase your grade will be handled as
    academic dishonesty.*
  - Infrastructure for student tests which are run on commit to the main and 
    develop branch is provided in the studenttest/ directory and are directed
    by the workflow specified in .github/workflows/studenttests.yml. I have
    provided Google Test test cases for the C++ parser already and encourage
    you to add your own tests to run as you develop your shell. 

## Assignment download and submission

You will fork the main assignment repository provided by the class instructor
for the class *in the class GitHub organization*. The class instructor will 
then snapshot and grade your repository at the due time. Only your `main`
GitHub branch will be graded. You must ensure that your completed work is on
this branch!  Be sure to commit and push all of your changes to the `main`
branch on your repository prior to the due date!

Two things about forks will bite you if you don't know about them:
  1. GitHub disables Actions on a new fork. Open the Actions tab on your fork
     and enable workflows, or nothing will run and you will get no test
     feedback at all.
  1. Branch protection is a repository setting, not a file, so none of the
     protection on the instructor's repository travels with your fork. See
     [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md) for how to recreate it on
     yours if you want it.

## Computer Development Environments

You should ensure that you have a high-quality environment for authoring, 
compiling, and running your program. You may use the development environment
of your choice to do so, though I provide some advice on what to look for
in a development environment below. Any modern Linux, Windows, or MacOS 
system should be able to build and install the shell with the proper
tools installed, as described below. More information on suggested general
development environments and a general software engineering workflow
can be found in [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)

## AI Coding Tools, Environments, and Workflow
You are *expected* to use AI coding tools to complete this assignment.
It is feasible to manually complete this assignment in the provided 
time, but one of the goals of this assignment is to ensure you are comfortable 
using modern AI software engineering tools. That is because later in the 
semester we will use those tools to implement, evaluate, and analyze modern 
systems techniques that will *not* be feasible to code by hand in the 
provided time.

See [docs/AI_WORKFLOW.md](docs/AI_WORKFLOW.md) for the AI coding harnesses, MCP
services, sandboxing options, model guidance, and a suggested development
workflow for this assignment.

## Supporting and Reference Materials

Before starting, you should become familiar with the Unix system calls defined 
in Section 2 of the UNIX manual. There are also several library routines in 
Section 3 that provide convenient interfaces to some of the more cryptic system
calls. However, you may _not_ use the library routine `system` nor any of the 
routines prohibited on the `ish` man page.  Several chapters of the Richard 
Stevens book _Advanced Programming in the UNIX Environment_ contain helpful 
information; Chapters 7, 8, and 9 are especially relevant, and this book is 
available online for free through the UNM library.

## Additional Advice

To implement `ish`, you will be creating a process that forks off other 
processes, which in turn forks off more processes, etc. If you inadvertently 
fork too many processes, you will cause Unix to run out, making yourself and 
everyone else on the machine very unhappy. _Be careful about this._

A few final bits of advice:
  1. First, and most importantly, get started early; you almost certainly have a
     lot to learn about AI workflows, GitHub software engineering workflows, and
     UNIX process management, before you can start implementing anything.
  1. Second, once you have a good understanding of what you are being asked to 
     do, work _one feature at a time in a separate feature branch of `develop`_
     in collaboration with the provided AI models to design, create test cases 
     for, implement, and document (in [report/report.tex](report/report.tex)) 
     those features.
  1. Use pull requests to merge individual features to the develop branch as 
     they are completed, paying attention to previous test cases to make sure 
     they don't break as you develop new features.
  1. As developed features are completed and documented, periodically use pull 
     requests to merge features to `main` and ensure that the class tests are 
     also making progress. While having separate `main` and `develop` branches 
     is not necessary on a project of this scale, it is good practice to get 
     into in preparation for the larger projects you will be working on later 
     this semester.
