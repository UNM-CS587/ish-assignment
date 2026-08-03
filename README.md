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

If you are in doubt about the functionality of `ish` or how it should behave in
a particular situation, model the behavior on that of `csh`.  If you have 
specific questions about the project, ask in the class Discord.

## Starter Source Code and Programming Assignment Restrictions 

Your shell should be written in C++, compile using cmake, and produce an 
executable named `ish`. I have provided a C++ parser for 'ish' in this 
assignment in the src/ directory. However, the C++ starter code does *not* 
yet parse background jobs or pipelines. To complete that portion of the 
assignment, you will need to (with AI help) add support for parsing job
control and pipelines to the provided C++ parser.
modify the parser.

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
     management system calls on your behalf; you shell should call all needed
     system calls directly. Specifically, 
  1. Specifically, you //must// use the `fork` and `execve` C system calls to 
     start new processes and the `dup`, `dup2`, or `pipe` system calls to 
     handle file redirection. You may *not* use other variants of `exec` or 
     the `system` call provided by the C standard library, their C++ 
     equivalents, or any other mechanism for creating processes or redirecting 
     file I/O. 
  1. Similarly, you must manually manually maintain the environment as data 
     passed to the `execve` call and may not use the `getenv` or `setenv` C 
     library calls or their C++ equivalent.
  1. You must cite any library you use outside of the standard C and C++ 
     libraries, including any Boost libraries you use besides the Boost
     Spirit X3 Parser already used.

## Computer Development Environments

You should ensure that you have a high-quality environment for authoring, 
compiling, and running your program. You may use the develooment environment
of your choice to do so, though I provide some advice on what to look for
in a development environment below. Any modern Linux, Windows, or MacOS 
system should be able to do build and install the shell with the proper
rools installed, as described below.

### Code Authoring Tools
You may use any IDE or programming enviornment with which you are comfortable
and productive to write your program. This includes but it not limited to
VSCode and its extensions, JetBrains CLion, Emacs, or a more traditional 
command line interfaces with editors such as VIM. 

As a general advice for choosing an IDE: //pick one you like and learn to use
it well.// Personally, my goal is to minimize teh amount of time my fingers 
leave the keyboard, as the keyboard is, for most activities, a much high
bandwidth input device than a mouse. That means you really want to learn the 
keyboard shortcuts in your chosen IDE.

### Compilation Environments

You should be able to compile this code on any system that supports UNIX
development, includign modern Windows, Macintosh, and Linux systems. You
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

## Github Software Engineering Workflow

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
You are //expected// to use AI coding tools to complete this assignment.
It is feasible to manually complete this assignment in the provided 
time, but one of the goals of this assignment is to ensure you are comfortable 
using modern AI software engineering tools. That is because later in the 
semester we will use those tools to implement, evaluate, and analyze modern 
systems techniques that will //not// be feasible to code by hand in the 
provided time. Below is information getting started using AI coding tools,
accessing AI models for this class using those tools, and a recommended
AI software engineering workflow.

### AI Coding Tools

Any of the following AI coding harnesses will work for this assignment;
pick one and learn it well rather than switching between several.
  * **Claude Code** (Anthropic) - a terminal-based agent. Install with the
    native installer described at https://code.claude.com/docs/en/setup,
    or with Homebrew (`brew install --cask claude-code`) on MacOS or
    Linuxbrew.
  * **Codex CLI** (OpenAI) - a terminal-based agent. Install with
    `npm install -g @openai/codex` (requires Node.js 18+, available via
    `apt install nodejs npm` on Ubuntu or `brew install node`) or with
    `brew install --cask codex`. Documentation and source are at
    https://github.com/openai/codex.
  * **Cline** - a VS Code extension. Install it from the VS Code
    Extensions panel (search "Cline") or from the Marketplace at
    https://marketplace.visualstudio.com/items?itemName=saoudrizwan.claude-dev.
    Documentation is at https://docs.cline.bot.
  * **Zoo Code** - a VS Code extension and community fork of Roo Code.
    Install it from the VS Code Extensions panel (search "Zoo Code") or
    see https://www.zoocode.dev. Documentation is at
    https://docs.zoocode.dev.

Set up an MCP (Model Context Protocol) interface to GitHub so your agent
can read and file issues, open and update pull requests, and check
Actions run status directly, instead of you copying that information
back and forth by hand. Use the official GitHub MCP server
(https://github.com/github/github-mcp-server); each harness above
documents how to add an MCP server to it (in Claude Code, for example,
`claude mcp add github -e GITHUB_PERSONAL_ACCESS_TOKEN=$GITHUB_PAT -- docker run -i --rm -e GITHUB_PERSONAL_ACCESS_TOKEN ghcr.io/github/github-mcp-server`).
Use a GitHub personal access token scoped to this repository only, not a
broad or account-wide token.

Because an AI agent working on this assignment will run shell commands and
compile and execute your code on your behalf, run its harness in a sandbox
rather than directly against your host account: a container (Docker or a
VSCode dev container), a virtual machine, or OS-level sandboxing (e.g.
`sandbox-exec` on MacOS, a restricted user account or namespace on Linux).
This keeps a runaway or misdirected command (an infinite fork loop is a
real risk in this assignment, see the Additional Advice section below)
from reaching the rest of your files, credentials, or system.

### AI Model Availability

TODO(instructor): Add the AI Verde access URL and signup/quota instructions here.

Not every request needs your most capable, most expensive model. Use a
fast, cheap model for mechanical work (boilerplate test cases, formatting,
straightforward bug fixes) and reserve your most capable model for work
that needs real reasoning, such as designing the job-control and pipeline
extensions to the parser or tracking down a race condition in process
management. Watch your token use as you go: a session that dumps large
files or long command output into the model's context repeatedly burns
through tokens without adding understanding, so ask the agent to summarize
or point it at specific line ranges instead of whole files where you can.

### AI Software Engineering Workflows

A productive pattern for working with an AI coding agent on one feature at
a time:
  1. Branch. Create a feature branch off `develop` for the single feature
     you're adding (e.g. `feature/io-redirection`).
  2. Orient the agent. Point it at the relevant section of the manual page
     and the existing parser code.
  3. Ask for a plan before code. Have the agent describe its approach and
     the test cases it intends to add before it writes implementation
     code, and review that plan yourself.
  4. Implement in small steps. Ask for one piece of functionality at a
     time (e.g. parse the `>` and `>>` operators before wiring redirection
     into the executor), and run the test suite after each step.
  5. Review the diff. Read every change the agent makes; do not accept a
     change you can't explain in the in-class test or to the instructor.
  6. Commit with a real message. Record what the feature is and, per this
     project's grading requirements, note what you prompted the AI to do.
  7. Open the pull request into `develop` once the feature's tests pass
     locally.

For example, adding output redirection might go: branch
`feature/output-redirection` off `develop`; ask the agent to explain how
the provided parser represents redirection targets; ask it to draft
failing test cases for `cmd > file` and `cmd >> file`; ask it to implement
the `dup2`-based redirection in the executor; run the test suite; commit;
open a pull request into `develop`.

At the end of each work session, ask your agent to summarize the session
into a short `MEMORY.md` file covering:
  * What feature or bug you were working on, and its current state (done,
    blocked, half-implemented).
  * Any design decisions you made and why, especially ones that aren't
    obvious from the code itself.
  * Open questions or next steps for the next session.

This costs a few minutes at the end of a session and saves you from
re-explaining context to the agent, or to yourself, every time you start a
new one. It also becomes useful material for the "how did you develop
this" conversation with the instructor.

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
for the class //in the class github organization.// Ths class instructor will 
then snapshot and grade your repository at the due time. Only your 'main' github
branch will be graded. You must ensure that your completed work is on this 
branch!  Be sure to commit and push all of your changes to the `main` branch 
on your repository prior to the due date!

## Supporting and Reference Materials

Before starting, you should become familiar with the Unix system calls defined 
in Section 2 of the UNIX manual. There are also several library routines in 
Section 3 that provide convenient interfaces to some of the more cryptic system
calls. However, you may _not_ use the library routine `system` nor any of the 
routines prohibitied on the `ish` man page.  Several chapters of the Richard 
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
     lot to learn about AI workflows, github software engineering workflows, and
     UNIX process management, before you can start implementing anything.
  1. Second, once you have a good understanding of what you are being asked to 
     do, work //one feature at a time in a separate feature branch of develop//
     in collaboration with the provided AI models to design, create test cases 
     for, implement, and document (in report.tex) those features.
  1. Use pull requests to merge individual features to the develop branch as 
     they are completed, paying attention to previous test cases to make sure 
     they don't break as you develop new features. Note that by default, the 
     develop branch is protected so only pull reqeusts that pass all of their 
     test cases can be merged to develop.
  1. As developed features are completed and documented, periodically use pull 
     requests to merge features to 'main' and ensure that hte class tests are 
     also making progress. While having separate 'main' and 'develop' branches 
     is not necessary on a project of this scale, it is good practice to get 
     into in preparation for the larger projects you will be working on later 
     this semester.
