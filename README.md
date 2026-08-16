# CS587 - Advanced Operating Systems Project 1: The `ish` shell

## Due Date
  * Repository Link and Report Due Date: Friday, September 11, 2026, 11:59pm
  * Repository Grading Due Date: Saturday, September 12, 2026, 12:00pm
  * In-class Test: Monday, September 14, 2026, 9:00am

## Assignment Goals
  1. Gain experience with and understanding of the structure of an OS (UNIX) 
     command interpreter, and the UNIX process and file APIs. (Required)
  1. Gain experience writing (optionally with AI assistance) reports 
     describing the results of a project. (Required)
  1. Gain experience using AI tools to effectively research and implement 
     system software features. (Optional but encouraged)

## Assignment Overview
Your task is to design and implement a basic UNIX command shell in a provided
C++ framework. You are encouraged to do so with the assistance of (provided) 
AI agents for both researching the concepts and features you will be 
implementing and, if you choose, actually implementing those features in C++. 
As part of this task, you also have to convince your boss (the class 
instructor) that:
  1. You actually understand the key concepts of the problem you're solving 
     and are not just the meaty appendage of an AI.
  1. The completed program actually solves the problem generally instead of
     regurgitating answers to the provided test cases (or illegally changing 
     the provided test cases!).

The Required Program Features section below lists the features you must
implement and demonstrate understanding of for full credit.

For this assignment, you will:
  1. Turn in the URL of your GitHub repository that includes all of your source
     code, test cases, and revision history. This revision history should 
     document your development process as you incrementally add features to
     the shell.  If you submit a repository with minimal revision history (e.g. 
     a single commit to the main branch that implements all features), you will
     be required to meet with the professor, explain your development process
     and code, and demonstrate that you understand the code to receive credit.
     The repository link is due via Canvas at the time given above and will be
     graded based on its contents at a slightly later time also given above.
  2. Turn in a written report describing the high-level approach that your 
     source code uses to provide the key features you implemented. The LaTeX
     source must be in the turned-in GitHub repository in 
     [report/report.tex](report/report.tex). You will also submit the report 
     PDF via Canvas by the date and time listed above.
  3. Take an in-class, closed-book test on how your shell uses UNIX system 
     calls and other tools to implement these key features at the date and time
     listed above.

The goal of this assignment is for you to *understand* the core system 
software concepts and their use in implementing key system features, not to 
merely pass program test cases. As such, your final grade on this assignment 
will be the *minimum* of your grade on these three components. *Correct code 
that passes all test cases and a report that describes how it does so will 
receive no credit without demonstrated understanding by you on the in-class 
exam.*

## Required Program Features

Your shell *must* use the C++ parser provided in the src/ directory and 
implement the features described in the included `ish` manual page 
[ish.man.pdf](ish.man.pdf).  Note that the parser does not yet parse job control
or pipelines, but for full credit on the assignment you will need to extend it 
to do so.

The manual page and the provided tests in the [classtest](classtest) directory
specify what you must implement, and this README clarifies points those 
documents leave open or get wrong. Where they disagree, the graded test cases 
win, then this README, then the manual page, then `csh`. If none of them 
settles a question, emulate `csh`, and tell me about the gap so I can fix this 
file. Note also that the manual page is reprinted from the course this 
assignment came from originally, so its footer reads `CSc 552` rather than 
CS587; ignore that.

### Test Case Details

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

One consequence is worth planning around: Corner Tests and Pipeline Tests both
include cases that background a job, so neither category can reach full marks
until job control works. Sequence your work by feature rather than by the
category names alone.

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

### Additional Program Requirements, Restrictions, and Starter Source Code

Your shell will be written in C++, compile using cmake, and produce an 
executable named `ish`. I have provided you this GitHub repository with
  1. Basic compilation and software engineering infrastructure,
  1. Most of a C++ parser for the assignment ([src/parser.hpp](src/parser.hpp)),
  1. A basic command data structure that represents the state of a parsed 
     command ([src/command.cpp](src/command.cpp)),
  1. A simple read/print loop ([src/ish.cpp](srcs/ish.cpp)) that demonstrates
     the use of the parser and command structure,
  1. Testcases which will be used to grade the correctness of your program
     ([classtest/](classtest/)), and
  1. Additional correctness testcases and infrastructure not used for grading
     but helpful for ongoing continuous integratoin and pull request testing
     ([studenttest/](studenttest)). 

Note that the the provided C++ starter code does *not* parse background jobs 
or pipelines; to complete that portion of the assignment, you will need to (potentially with AI help) add support for parsing 
job control and pipelines to the provided parser.

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
     passed to the `execve` call; as described in the `ish` manual page,
     you may not use the `putenv` and `getenv` calls or other similar functions
     such as `setenv` or `unsetenv` or their C++ equivalents. Note that 
     `setenv` and `unsetenv` are also the names of two builtins you must 
     implement; the ban is on the C library routines, not on the builtins.
  1. You must cite any library you use outside of the standard C and C++ 
     libraries, including any Boost libraries you use besides the Boost
     Spirit X3 Parser already used. As a general guideline, not external
     library you use may not, in the judgement of hte instructor, trivialize 
     any of the learning goals of the assignment. I strongly recommend that you
     ask explicit permission for any external source code you plan to use 
     in your implementation.

Finally, your code must be "clean" -- it must compile without warnings or 
unresolved references on a standard UNIX system with the Boost C++ libraries and
latex installed. I also expect your programs to be well organized and easy 
to read (in addition to correct).

### Provided Software Testing Features

I have provided Testcases that test correctness of the shell output and of key 
shell components in the provided repository. Specifically:
  - Class grading tests on which your shell is graded are specified in the 
    classtest/ directory and are run by the github workflow described in 
    .github/workflows/classtests.yml; these tests are purely functional and 
    work by executing the shell on commit to the main branch. *Do not change 
    classtests.yml or any of the tests in the classtest/ directory. You or an AI
    changing class testing infrastructure to increase your grade will be handled
    as academic dishonesty.*
  - Infrastructure for student tests which are run on commit to the main branch 
    is provided in the studenttest/ directory and are directed by the workflow 
    specified in .github/workflows/studenttests.yml. I have provided Google Test
    test cases for the C++ parser already and encourage you to add your own 
    tests to run as you develop your shell.
  - Be sure to properly protect your main branch so that it is only committed
    to via pull requests from feature branches, and that you only work on 
    feature branches. More information on a suggested GitHub workflow
    can be found in [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md).

## Required Report 

In addition to the source code you must implement, you must also write a report
providing a high-level overview of how you:
  1. Implemented each of the features described above using the UNIX system 
     call interface,
  1. How you verified the correctness of these modules so that they don't just 
     pass the functional tests but actually implement the feature generally, and
  1. Your experience using AI tools and/or available reference information to
     research and implement these features. 

Your report should describe a high-level design of the general strategy of each
major feature that highlights and demonstrates understanding of
the system interfaces used for a given feature; it should *not* provide detailed
design notes on minor semantic details, for example from the AI detail design of
the feature implementation.

You may use AI tools to assist you in editing and revising this report, but *you
should provide the draft text* that the AIs help you revise describing your code
and how it works.  Specifically, I suggest you write the text yourself and then
have the AI critique your writing versus the writing guidance in 
[docs/WRITING.md](docs/WRITING.md). A suggested outline for this report is 
included in the comments in [report/report.tex](report/report.tex).

Grading on the written report will be as follows:
  * Presentation and Readability: 20%
  * Understanding of core concepts in the shell: 50%
    * Process Creation and Command Execution: 15%
    * Alias and Environment Handling: 10%
    * Error Handling/`.ishrc`/Startup: 5%
    * I/O Redirection: 10%
    * Job Control: 5%
    * Pipelines: 5%
  * Discussion of Testing Strategy: 15%
  * Discussion of External References/AI Usage: 15%

## In-Class Test
The in-class exam on shell concepts will a written paper exam consisting of 
short-answer questions. These questions will quiz your understanding of the 
basic strategy for implementing each of the features described in the 
requiremets sections and in your report. The primary focus of these questions
will be on the (1) system calls used in your shell, (2) what those systems calls
do, and (3) how they were used to implement different features in your shell.
The quiz may also include questions on general shell structure and strategy 
(e.g. alias and enviornment handling, error handling, etc.). The breakdown of 
points on the in-class exam will correspond roughly though not necessarily
exactly with the breakdown of points between shell features in the test cases.

## Additional Assignment Details

### Assignment download and submission
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

### Computer Development Environments
You should ensure that you have a high-quality environment for authoring, 
compiling, and running your program. You may use the development environment
of your choice to do so, though I provide some advice on what to look for
in a development environment below. Any modern Linux, Windows, or MacOS 
system should be able to build and install the shell with the proper
tools installed, as described below. More information on suggested general
development environments and a general software engineering workflow
can be found in [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)

### AI Coding Tools, Environments, and Workflow
You are expected (but not required) to use AI coding tools to complete this 
assignment; however, the goal is for you to understand the system interfaces
and the structure of a command interpreter, not simply to pass test cases.
As such, I suggest that any use you make of AI tools be *highly* interactive, 
where you are working with the AI to find references on how to implement features
that you read, compare those references versus the design the AI proposes and 
any test cases for a feature, and spot-check the implementation and test cases
to make sure it actually does what needs to be done. *If you use AI tools 
simply as a way to pass test cases without understanding how and why things
work, you will not pass this assignment*.

It is feasible to manually complete this assignment in the provided 
time, but one of the goals of this assignment is to ensure you are comfortable 
using modern AI software engineering tools. Later in the semester we will use 
those tools to implement, evaluate, and analyze modern systems techniques; the
use of AI tools will allow us to be much more aggressive in the system
software techniques we are able to do in these projects.

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

## Use of External Sources and Academic Honesty

As a general principle, you may read and use existing publicly-available 
information as long as that use (1) is clearly docimented and cited, (2) all
code and documentation you write either directly or with the assistance of an
AI is your own intellectual work, and (3) any used information aligns with the learning goals of the assignment and will not trivialize any of the learning 
goals of the assignments as described above. If in doubt, *ask*!

As specific examples:
  - You may use AI tools to aid you in implementing `ish`, but all such work 
    must be carefully documented, include specific efforts to ensure that you 
    understand and are guiding the the coding of these features. This effort
    must be thoroughly documented in both your commit history and in your 
    written report. 

  - You may *read* other refeences materals such as books on UNIX 
    process and file interfaces, public shell implementations, and best 
    practices for using AI tools to more effectively meet the goals of the 
    assignment.  Any such usage must be specifically documented in your source 
    code and report. You may also share pointers to public general information 
    and general advice with fellow students. 

  - You may use publicly available AI agent workflows, MCP agents, and skills to
    implement your shell *as long as they are general purpose and not specific 
    to UNIX shell implementation*.

  - You may *not* use any code from publicly available shell implementations in
    you implementation, either directly or by laundering them through an AI 
    (e.g., you may not prompt an AI with "Integrate the code implementing 
    feature XYZ on [linked web page] into the shell implementation".)

  - You may *not* share any of your source code or the code generated by an AI 
    at your direction (including test cases!) with your fellow students. 
    Similarly, you may *not* share your specific agent prompts and files (e.g.,
    CLAUDE.md, MEMORY.md, etc.) or AI-generated design documents with your
    fellow students, as learning to create, modify, and use these files well 
    is a goal of this assignment.

  - Leaving your code or other material in a public location (e.g., in a shared 
    world-readable directory, in a public git repository, etc.) will be treated
    the same as explicitly providing that material to another student.

Violating any of these will be considered and reported as academic dishonesty, 
result in a grade of 0 in the class, and will be repoorted to the Dean of 
Students.

## Late Submission Policy
The assignment will not be accepted late without prior arrangement with the 
class instructor or documented extraordinary circumstances outside the control
of the student. The state of your repository at the time it is due is what will
be used to determine your grade.

## Additional Advice
To implement `ish`, you will be creating a process that forks off other 
processes, which in turn forks off more processes, etc. If you inadvertently 
fork too many processes, you will cause Unix to run out, making yourself and 
everyone else on the machine very unhappy. _Be careful about this._

A few final bits of advice:
  1. First, and most importantly, get started early; you almost certainly have a
     lot to learn about AI workflows, GitHub software engineering workflows, and
     UNIX process management before you can start implementing anything.
  1. Second, once you have a good understanding of what you are being asked to 
     do, work *one feature at a time in a separate feature branch* in 
     collaboration with the provided AI models to design, create test cases 
     for, implement, and document (in [report/report.tex](report/report.tex)) 
     those features.
  1. Use pull requests to merge individual features to the main branch as 
     they are completed, paying attention to previous test cases to make sure 
     they don't break as you implement new features.

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
    required, is still worth implementing, and is fair game on the in-class code
    test.

  - If you are in doubt about the functionality of `ish` or how it should behave
    in a particular situation, model the behavior on that of `csh`.  If you have
    specific questions about the project, ask in the class Discord.

