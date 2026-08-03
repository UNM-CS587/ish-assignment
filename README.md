# CS587 - Advanced Operating Systems Project 1: The `ish` shell

## Due Date
  * Code and Report Due Date: Sunday, September 6, 2026, 11:59pm
  * In-class Code Test: Monday, September 7, 2026, 9:00am

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
     document in the turned-in GitHub repository - source code in report/report.tex,
     which the build system will compile into report.pdf).
  3. Take an in-class, closed-book test on how your shell implements these key
     features.
*Your final grade on this assignment will be the minimum of your grade on 
these three components.  Correct code that passes all test cases without
demonstrated understanding will receive no credit!*

## Required Program Features

Your shell *must* use the C++ parser provided in the src/ directory and 
implement the features described in the included ISH manual page (ish.man.pdf). 
Note that the parser does not yet parse job control or pipelines, but for full 
credit on the assignment you will need to extend it to do so.

The classtest/ directory holds the graded test cases, worth 100 points total
on the `.github/workflows/classtests.yml` autograder, split into the nine
categories below. Corner Tests runs two ctest targets under that one
10-point weight: `corner.test` (shelltest cases) and `resource-leak.sh`
(a script that drives ish directly to check for leaked file descriptors
and zombie processes, since that isn't expressible as shelltest
input/output matching).

  * Basic Tests - 15 points
    * Correct shell prompt as described in the manual page
    * Clean shell exit at EOF
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
    * PATH searching skips a nonexistent directory instead of failing, and
      setting PATH to empty is handled gracefully rather than crashing
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

One requirement isn't checked by an automated test in classtest/ and is
assessed by hand or at the in-class test instead: the shell should not leak
memory over a session. `resource-leak.sh` catches leaked file descriptors
and zombie processes (both are directly observable from outside the
process), but a memory leak needs a tool like valgrind to detect reliably,
which isn't part of this project's toolchain.

The point breakdown above is drawn directly from `classtest/CMakeLists.txt`
and `.github/workflows/classtests.yml`; results appear as a GitHub Actions
check on every pull request into `main`. Job control and pipeline error
handling is graded within the Job Control Tests and Pipeline Tests
categories above; only basic execution and redirection have dedicated
error categories.

If you are in doubt about the functionality of `ish` or how it should behave in
a particular situation, model the behavior on that of `csh`.  If you have 
specific questions about the project, ask in the class Discord.

## Starter Source Code and Programming Assignment Restrictions 

Your shell should be written in C++, compile using cmake, and produce an 
executable named `ish`. I have provided a C++ parser for `ish` in this 
assignment in the src/ directory. However, the C++ starter code does *not* 
yet parse background jobs or pipelines. To complete that portion of the 
assignment, you will need to (with AI help) add support for parsing job
control and pipelines to the provided C++ parser.

Your code must be "clean" -- it must compile without warnings or 
unresolved references on a standard UNIX system with the Boost C++ libraries and
latex installed. I also expect your programs to be well organized and easy 
to read (in addition to correct).

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
     passed to the `execve` call and may not use the `getenv` or `setenv` C 
     library calls or their C++ equivalent.
  1. You must cite any library you use outside of the standard C and C++ 
     libraries, including any Boost libraries you use besides the Boost
     Spirit X3 Parser already used.

## Computer Development Environments

You should ensure that you have a high-quality environment for authoring, 
compiling, and running your program. You may use the development environment
of your choice to do so, though I provide some advice on what to look for
in a development environment below. Any modern Linux, Windows, or MacOS 
system should be able to build and install the shell with the proper
tools installed, as described below.

### Code Authoring Tools
You may use any IDE or programming environment with which you are comfortable
and productive to write your program. This includes, but is not limited to,
VSCode and its extensions, JetBrains CLion, Emacs, or a more traditional 
command line interfaces with editors such as VIM. 

As general advice for choosing an IDE: *pick one you like and learn to use
it well.* Personally, my goal is to minimize the amount of time my fingers 
leave the keyboard, as the keyboard is, for most activities, a much
higher-bandwidth input device than a mouse. That means you really want to
learn the keyboard shortcuts in your chosen IDE.

### Compilation Environments

You should be able to compile this code on any system that supports UNIX
development, including modern Windows, Macintosh, and Linux systems. You
will need to install a modern C++ compiler, the Boost C++ libraries, 
Google Test, and a LaTeX environment on these systems.

### Linux
The pre-provided test environment uses an Ubuntu container on github for 
testing, and the code should compile and run with ease on Ubuntu and related
systems.  To compile on an Ubuntu system, make sure you have the following 
packages installed:
  - build-essential
  - cmake
  - libboost-all-dev 
  - shelltestrunner 
  - libgtest-dev
  - texlive-latex-base
  - texlive-latex-extras
  - latexmk
Compiling on a RedHat system will require installing similar packages using the
RedHat tools.

### Windows
On Windows systems, you can use Windows Subsystem for Linux 2 (WSL2) to run 
a Linux kernel and development environment on a Windows system. I suggest 
running Ubuntu using WSL2; if you do so you can follow the advice above on
needed packages.

### MacOS 
To compile on a MacOS system, I suggest you use use homebrew (https://brew.sh) 
to install UNIX development packages. You can also use homebrew on Linux or 
WSL2. If using homebrew, you'll want the following packages:
  - boost
  - shelltestrunner
  - texlive
  - googletest

## GitHub Software Engineering Workflow

Use a three-tier branching model: `main`, `develop`, and short-lived feature
branches cut from `develop`.
  * `main` holds only released, working code. The class instructor grades
    this branch, so nothing incomplete or broken belongs here.
  * `develop` is the integration branch. Merge each finished feature into
    `develop` through a pull request once its tests pass.
  * Feature branches (e.g. `feature/pipeline-parsing`, `feature/job-control`)
    hold the work for one feature at a time. Branch from `develop`, add the
    code and test cases for that feature together, and open a pull request
    back into `develop` when it's ready.

This is a simplified version of the branching model Vincent Driessen
described in "A successful Git branching model" (nvie.com, 2010); GitHub's
own "GitHub flow" guide (docs.github.com) documents a lighter-weight
alternative if you'd rather skip the `develop` branch on a project this
size.

Protect `main` and `develop` in the repository settings so that:
  * Commits cannot be pushed to them directly; all changes arrive through a
    pull request.
  * The GitHub Actions workflows (`.github/workflows/classtests.yml` on
    `main`, `.github/workflows/studenttests.yml` on `develop` and `main`)
    must pass before a pull request can merge.

This gives you the same safety net you'll rely on in later, larger
projects: broken code never lands on the branch that's graded or the
branch your teammates build on, and every merge has a passing test run to
point to.

## AI Coding Tools, Environments, and Workflow
You are *expected* to use AI coding tools to complete this assignment.
It is feasible to manually complete this assignment in the provided 
time, but one of the goals of this assignment is to ensure you are comfortable 
using modern AI software engineering tools. That is because later in the 
semester we will use those tools to implement, evaluate, and analyze modern 
systems techniques that will *not* be feasible to code by hand in the 
provided time.

See [AI_WORKFLOW.md](AI_WORKFLOW.md) for the AI coding harnesses, MCP
services, sandboxing options, model guidance, and recommended development
workflow for this assignment.

## Testing

Testcases that test correctness of the shell output and of key shell components
are included in the provided repository. Specifically:
  - Class grading tests on which your shell is graded are specified in the 
    classtest/ directory and are run by the github workflow described in 
    .github/workflows/classtests.yml; these tests are purely operational and 
    work by executing the shell on commit to the main branch. *Do not change 
    classtests.yml or any of the tests in the classtest/ directory. Changing
    class testing infrastructure to increase your grade will be handled as
    academic dishonesty.*
  - Infrastructure for student tests which are run on commit to the main and 
    develop branch is provided in the studenttest/ directory and are directed
    by the workflow specified in .github/workflows/studenttests.yml. I have
    provided Google Test test cases for the C++ parser already and encourage
    you to add your own tests to run as you develop your shell. Unless you
    are doing extra credit parts of the assignment, these tests will not
    impact your grade.

## Assignment download and submission

You will fork the main assignment repository provided by the class instructor
for the class *in the class GitHub organization*. The class instructor will 
then snapshot and grade your repository at the due time. Only your `main`
GitHub branch will be graded. You must ensure that your completed work is on
this branch!  Be sure to commit and push all of your changes to the `main`
branch on your repository prior to the due date!

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
     for, implement, and document (in report.tex) those features.
  1. Use pull requests to merge individual features to the develop branch as 
     they are completed, paying attention to previous test cases to make sure 
     they don't break as you develop new features. Note that by default, the 
     develop branch is protected so only pull requests that pass all of their 
     test cases can be merged to develop.
  1. As developed features are completed and documented, periodically use pull 
     requests to merge features to `main` and ensure that the class tests are 
     also making progress. While having separate `main` and `develop` branches 
     is not necessary on a project of this scale, it is good practice to get 
     into in preparation for the larger projects you will be working on later 
     this semester.
