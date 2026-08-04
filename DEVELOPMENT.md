
# Computer Development Environments

You should ensure that you have a high-quality environment for authoring, 
compiling, and running your program. You may use the development environment
of your choice to do so, though I provide some advice on what to look for
in a development environment below. Any modern Linux, Windows, or MacOS 
system should be able to build and install the shell with the proper
tools installed, as described below.

## Code Authoring Tools
You may use any IDE or programming environment with which you are comfortable
and productive to write your program. This includes, but is not limited to,
VSCode and its extensions, JetBrains CLion, Emacs, or a more traditional 
command line interfaces with editors such as VIM. 

As general advice for choosing an IDE: *pick one you like and learn to use
it well.* Personally, my goal is to minimize the amount of time my fingers 
leave the keyboard, as the keyboard is, for most activities, a much
higher-bandwidth input device than a mouse. That means you really want to
learn the keyboard shortcuts in your chosen IDE.

## Compilation Environments

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
described in "A successful Git branching model" (nvie.com, 2010).
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
