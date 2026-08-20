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
  - texlive-latex-extra
  - latexmk
Compiling on a RedHat system will require installing similar packages using the
RedHat tools.

### Windows
On Windows systems, you can use Windows Subsystem for Linux 2 (WSL2) to run 
a Linux kernel and development environment on a Windows system. I suggest 
running Ubuntu using WSL2; if you do so you can follow the advice above on
needed packages.

### MacOS 
To compile on a MacOS system, I suggest you use homebrew (https://brew.sh) 
to install UNIX development packages. You can also use homebrew on Linux or 
WSL2. If using homebrew, you'll want the following packages:
  - cmake
  - boost
  - shelltestrunner
  - texlive
  - googletest

### GitHub Codespaces

A Codespace is a Linux container that GitHub runs for you, reachable from a
browser or from a local VS Code or JetBrains IDE attached to it. Your
assignment repository lives in the class organization, and the organization
has enabled Codespaces access for outside collaborators, so Codespaces on it
are billed to the class rather than to you. This is the quickest way to get a
working environment, and it is a useful fallback when something breaks in
your local one the night before the deadline.

The repository ships a `.devcontainer/devcontainer.json` that starts from
Microsoft's C++ image and installs Boost, shelltestrunner, Google Test, and
LaTeX. Codespaces reads that file, so `cmake` and `ctest` work with nothing
for you to install. To start one, open your repository on GitHub, click
**Code**, choose the **Codespaces** tab, and create a Codespace on the branch
you want to work on. The first one takes a few minutes while the packages
install; later ones start from a cached image and are much faster. From
there, follow the build and test instructions below unchanged.

The Codespace terminal is a real pseudo-terminal, so `ish` sees a terminal on
stdin and interactive job control behaves as it does locally: ^Z stops the
foreground job, and `fg` and `bg` resume it.

Three things to watch:
  * **Your work only survives if you push it.** The container's disk belongs
    to the Codespace. Deleting the Codespace deletes anything you have not
    committed and pushed.
  * **Codespaces cost core-hours, even idle ones.** A Codespace stops on its
    own after 30 minutes of inactivity but keeps consuming storage until you
    delete it, against the class's shared spending limit. Stop or delete
    Codespaces you are done with.
  * **Keep credentials out of the repository.** If you run an AI coding
    harness inside the Codespace, put its API key in a Codespaces secret
    (your account Settings > Codespaces > Secrets), which arrives as an
    environment variable, rather than committing it.

Running an agent inside a Codespace also gives you the isolation
[docs/AI_WORKFLOW.md](AI_WORKFLOW.md) recommends: the container holds a clone
of one repository and nothing else of yours, so a misdirected command cannot
reach the rest of your files.

## Compiling the shell in the development environment

This project builds out of source, into a `build/` directory that `.gitignore`
already excludes. Configure once, from the top of the repository:

```
cmake -B build
```

Then build after every change:

```
cmake --build build
```

That produces the shell at `build/src/ish` and the parser unit tests at
`build/studenttest/parser_test`. Run the shell by hand to try it:

```
./build/src/ish
```

The starter shell parses a command line and prints the `command` structure the
parser produced; it executes nothing. Expect that output until you write the
execution code. If you add a source file, re-run `cmake -B build` so CMake
picks it up.

### Running the test cases

`ctest` runs the graded class tests and the student tests together:

```
ctest --test-dir build
```

Add `--output-on-failure` to see what a failing case produced, and `-R` to run
one category while you work on it:

```
ctest --test-dir build -R RedirectionTests --output-on-failure
```

Read the per-test lines rather than the summary. A category `ctest` cannot run
in your environment reports as skipped and still leaves the summary reading
`100% tests passed`; [docs/CLARIFICATIONS.md](CLARIFICATIONS.md) lists the
conditions that cause this.

### Building the report

The report is a separate target rather than part of the default build, so a
LaTeX error cannot break the build the test cases depend on:

```
cmake --build build --target generate_pdf
```

The PDF lands at `build/report/report.pdf`. That is the file you submit to
Canvas. CMake only defines this target if it found `latexmk` when you
configured, and it says so in the configure output when it did not; install a
TeX distribution and re-run `cmake -B build` to get the target back.

## GitHub Software Engineering Workflow

I recommend you use a two-tier branching model: `main`, and short-lived feature
branches derived from `main` (e.g. `feature/pipeline-parsing`).
  * `main` holds only tested, working code. The class instructor grades
    this branch, so nothing incomplete or broken belongs here.
  * Feature branches (e.g. `feature/pipeline-parsing`, `feature/job-control`)
    hold the work for one feature at a time. Branch from `main`, add the
    code and test cases for that feature together, and once the relevant
    test cases pass, open a pull request back into `main`.

This is a simplified version of the branching model Vincent Driessen
described in "A successful Git branching model" (nvie.com, 2010); we are
using a two-tier (main/feature) version of this workflow instead of the
more complex three-tier (main/develop/feature) version because you will
be working as a single developer implementing features sequentially. 
The three-tier version is helpful when multiple developers are 
developing features on several feature branches at the same time.

Your repository arrives with this workflow enforced. A ruleset named
`Student Test Check`, which you can read under Settings > Rules > Rulesets,
protects `main` so that:
  * Commits cannot be pushed to it directly; all changes arrive through a
    pull request.
  * The GitHub Actions workflow `.github/workflows/studenttests.yml` must
    pass before a pull request can be merged to `main`.
  * The pull request branch must be current with `main` before it merges, so
    merge `main` into your feature branch when `main` has moved ahead.
  * `main` cannot be deleted or force-pushed.

A pull request needs no approvals, since you cannot approve your own; you
open it, wait for the check, and merge it yourself. If a pull request sits
unmergeable and you cannot tell why, read the check's log under the Actions
tab rather than working around the ruleset. Ask the instructor if you need it
changed.
