# CS587 - Advanced Operating Systems Project 1: The `ish` shell

## Due Date
  * Code and Report Due Date: Sunday, September 6, 2026, 11:59pm
  * In-class Code Test: Monday, September 7, 2026, 9:00am

## Assignment Overview
Your task is to design and implement a basic UNIX command shell in a provided
C++ framework with the assistance of AI coding agents. As part of this task, 
you also have to convince your boss (the class instructor) that:
  1. You actually understand the key concepts of the problem you're solving
  2. The completed program actually solves the problem generally instead of
     regurgitating answers to the provided test cases
The features you must implement and demonstrate understanding of for full
credit.

For this assignment, you will:
  1. Turn in a github repository that includes all of your source code, test
     cases, and revision history. This revision history should document your
     development process as you incrementally add features to the shell,
     including any AI prompting and token usage associated with these commits.
     If you submit a repository with minimal revision history (e.g. a single 
     commit to the main branch that implements all features), you will be
     required to meet with the professor,explain your development process
     and code, and demonstrate that you understand the code to receive credit.
  2. Turn in a written report describing the high-level approach that your 
     source code uses to providing the key features you implemented (as a LaTeX
     document in the turned-in github repository - source code in report/report.tex,
     which the build system will compile into report.pdf).
  3. Take an in-class, closed-book test on how your shell implements these key
     features.
*Your final grade on this assignment will be the minimum of your grade on 
these three components.  Correct code that passes all test cases without
demonstrated understanding will receive no credit!*

## Required Program Features

Your shell *must* use the C++ parser parser provided in the src/ directory and 
implement the features described in the included ISH manual page (ish.man.pdf). 
Note that the parser does not yet parse job control or pipelines, but for full 
credit on the assignment you will need to extend it to do so.

The basic features you are required to implement for full credit are:
  * Basic command execution - 25%
    * Correct shell prompt as described in the manual page
    * Run command with full command name
    * Run command with full command name and arguments
    * 'cd' builtin works (check with /bin/pwd)
    * `quit` builtin and EOF cause shell to exit properly and cleanly

  * Envirionment and alias handling - 20%
    * Environment variables correctly passed to child
    * PATH searching works
    * PATH in wrong syntax handled well
    * `printenv` (`setenv` with no arguments) works
    * `unsetenv` works
    * Adding an alias works
    * Removing an alias works

  * Miscellaneous - 10%
    * .ishrc executed properly
    * Shell continues execution after ishrc execution
    * Environment and aliases changed in .ishrc are visible in the interactive shell
    * The shell should not leak resources, for example file descriptors, process
      IDs, or memory

  * File redirection - 20%
    * Redirect output to a simople file
    * Appending to a file works
    * Redirection of stdout and stderr works
    * Redirect input from a simple file
    * Ambiguous redirections are detected and reported
    * Redirection of both input and output works.

  * Job control - 10%
    * Start jobs in the background
    * Stop running jobs using ^Z
    * Use 'bg' and 'fg' to continue running jobs in the background 
      or foreground, respectively
    * List the status of running jobs using the 'jobs' builtin
    * Report the competion of background jobs prior to issuing a 
      new prompt

  * Pipelines - 5%
    * Run two-process pipelines that couple the output of the first
      to the input of the second, with both jobs running simultaneously.
    * Handle pipelines with arbitrarily many processes (tested up to 10)
    * Handle pipelines that include error output
    * Support all job control features on pipelines

  * Error handling - Tested in each of the areas above
    * The shell should fail gracefully and report errors like `csh` does when 
      permissions or something else goes wrong doing thie things described 
      above.

The breakdown of scoring for these features is provided in the test cases that
run on commits to the main branch and run from the file
.github/worflows/classtests.yml; the resutls of this test can be seen in the
github actions runner when you commit to your main branch.

The features you //may// implement for extra credit that are described in 
the ISH man page are:
  * Job control (the & separator, ^Z handling, and the bg/fg/jobs builtins) - 10%
  * Pipelines - 15%

If you are in doubt about the functionality of `ish` or how it should behave in a 
particular situation, model the behavior on that of `csh`.  If you have specific 
questions about the project, ask in the class Discord.

## Starter Source Code and Programming Assignment Restrictions 

Your shell should be written in C++, compile using cmake, and produce an 
executable named `ish`. I have provided a C++ parser for the subset of 
'ish' assigned in this assignment in the src/ directory; the C++ starter 
code does *not* parse background jobs or pipelines as they are not required;
to complete the extra credit portions of the assignment, you will need to 
modify the parser.

Your code must also be "clean" -- it must compile without warnings or 
unresolved references on a standard UNIX system with the Boost C++ libraries 
installed. The pre-provided test environment uses an Ubuntu container for 
testing. I have tested the provided code on the CS department Ubuntu systems, 
on Ubuntu running on WSL2 on a Windows systems, and MacOS. Your programs are 
expected to be well organized and easy to read, as well as correct. To compile
on an Ubuntu system, make sure you ahve the following packages installed:
  - libboost-all-dev 
  - shelltestrunner 
  - libgtest-dev
  - texlive-latex-base
  - latexmk

You must use the `fork` and `execve` C system calls to start new processes 
and the `dup` or `dup2` command to handle file redirection. You may *not* use 
other variants of `exec` or the `system` call, their C++ equivalents, or any 
other mechanism for creating processes or redirecting file I/O.

## Provided AI Coding Tools
You are //expected// to use AI coding tools to complete this assignment - 
while it is feasible to manually complete this assignment in the provided 
time, one of the goals of this assignment is to ensure you are comfortable 
with modern AI software engineering tools so that later in the semester we 
can use those tools to implement, evaluate, and analyze modern systems 
techniques that would //not// be feasible to code by hand in the provided time.

TODO: Here describe the AI coding tools provided to the students through 
AI-Verde and the suggested AI software engineering workflow, including 
claude/codex, connecting to the API, using AI Verde to select the model 
being used and to keep track of the tokens used, and how this integrates 
with github.

# General development tools

TODO: Provide advice on development tools. Fingers on keys being the most
important one, but noting the useful packages in vscode they might want to 
look at.

## Testing

Testcases that test correctness of the shell output and of key shell components
are included in the provided repository. Specifically:
  - Class gradding tests on which your shell is graded are specifed in the 
    classtests/ directory and are run by the github workflow described in 
    .github/workflows/classtests.yml; these tests are purely operational and 
    work by executing the shell on commit to the main branch. //Do not change 
    classroom.yml or any of the tests in the classtests/ directory. Changing
    class testing infrastructure to increase your grade will be handled as
    academic dishonesty.// 
  - Infrastructure for student tests which are run on commit to the main and 
    develop branch is provided in the tests/ directory and are directed by the 
    workflow specified in .github/workflows/studenttests.yml. I have provided 
    google test testcases for the C++ parser already and encorage you to add 
    your own tests to run as you develop your shell. Unless you are doing extra
    credit parts of the assignment, these tests will not impact your grade.

## Assignment download and submission

You will fork the main assignment repository provided by the class instructor
for the class //in the class github organization.// Ths class instructor will then
snapshot and grade your repository at the due time. Only your 'main' github 
branch will be graded. You must ensure that your completed work is on this branch!
Be sure to commit and push all of your changes to the `main` branch on your 
repository prior to the due date!

## Supporting and Reference Materials

Before starting, you should become familiar with the Unix system calls defined in
Section 2 of the UNIX manual. There are also several library routines in Section 3 
that provide convenient interfaces to some of the more cryptic system calls. However, 
you may _not_ use the library routine `system` nor any of the routines prohibitied on 
the `ish` man page.  Several chapters of the Richard Stevens book _Advanced Programming 
in the UNIX Environment_ contain helpful information; Chapters 7, 8, and 9 are 
especially relevant, and this book is available online for free through the UNM 
library.

## Additional Advice

To implement `ish`, you will be creating a process that forks off other processes, which 
in turn forks off more processes, etc. If you inadvertently fork too many processes, you 
will cause Unix to run out, making yourself and everyone else on the machine very unhappy. 
_Be careful about this._

A few final bits of advice:
  - First, and most importantly, get started early; you almost certainly have a lot to 
    learn about AI workflows, github software engineering workflows, and UNIX process 
    management, before you can start implementing anything. 
  - Second, once you have a good understanding of what you are being asked to do, work
    //one feature at a time in a separate feature branch of develop// in collaboration 
    with the provided AI models to design, create test cases for, implement, and document
    (in report.tex) those features.
  - Use pull requests to merge individual features to the develop branch as they are 
    completed, paying attention to previous test cases to make sure they don't break 
    as you develop new features. Note that by default, the develop branch is protected
    so only pull reqeusts that pass all of their test cases can be merged to develop.
  - As developed features are completed and documented, periodically use pull requests 
    to merge features to 'main' and ensure that hte class tests are also making 
    progress. While having separate 'main' and 'develop' branches is not necessary on
    a project of this scale, it is good practice to get into in preparation for the 
    larger projects you will be working on later this semester.
