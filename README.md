[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/U3fB7APX)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-2972f46106e565e64193e422d61a12cf1da4916b45550586e14ef0a7c637dd04.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=17984862)
# CS587 - Advanced Operating Systems Project 1: The `ish` shell

## Due Date
  * Code and Report Due Date: Sunday, September 6, 2026, 11:59pm
  * In-class Code Test: Monday, September 7, 2026, 9:00am

## Assignment Overview
Your task is to design and implement a basic UNIX command shell in a provided C++ framework 
with the assistance of AI coding agents. 
As part of this task, you also have to convince your boss (the class instructor) that:
  1. You actually understand the key concepts of the problem you're solving 
  2. The completed program actually solves the problem generally instead of regurgitating answers 
     to the provided test cases
The features you must implement and demonstrate understanding of for full credit do not include 
pipelines or job control; however, you may implement and demonstrate understand of these features 
for extra credit.

For this assignment, you will:
  1. Turn in a github repository that includes all of your source code, test cases, and revsion
     history. This revision history should document your development process as you incrementally
     add features to the shell, including any AI prompting associated with these commites. If you 
     submit a repository with minimal revision history (e.g. a single commit to the main branch 
     that implements all features), you will be required to meet with the professor and explain 
     your development process and code.
  2. Turn in a written report describing the high-level approach that your source code uses to 
     providing the key features you implemented.
  3. Take an in-class, closed-book test on how your shell implements these key features.
*Your final grade on this assignment will be the minimum of your grade on these three components.
Correct code without demonstrated understanding will receive no credit!*

## Required Program Features

Your shell *must* use the C++ parser parser provided in the src/ directory and implement a subset of 
the features described in the included ISH manual page (ish.man/pdf). Notably, you do *not* need 
to implement job control or pipelines, though you may implement job control and piplines, along with
test cases that demonstrate their correctness for 10% extra credit each (10% for job control, 15% for 
pipelines).

The basic features you are required to implement for full credit are:
  * Basic command execution - 25%
    * Correct shell prompt as described in the manual page
    * Run command with full command name
    * Run command with full command name and arguments
    * 'cd' builtin works (check with /bin/pwd)
    * `quit` builtin and EOF cause shell to exit properly and cleanly

  * Envirionment and alias handling - 25%
    * Environment variables correctly passed to child
    * PATH searching works
    * PATH in wrong syntax handled well
    * `printenv` (`setenv` with no arguments) works
    * `unsetenv` works
    * Adding an alias works
    * Removing an alias works

  * Miscellaneous - 10%
    * .ishrc executed properly

  * File redirection - 25%
    * Redirect output to a simople file
    * Appending to a file works
    * Redirection of stdout and stderr works
    * Redirect input from a simple file
    * Ambiguous redirections are detected and reported
    * Redirection of both input and output works.

  * Error handling - 15%
    * The shell should fail gracefully and report errors like `csh` does when permissions or something else goes wrong doing thie things described above.
   
The features you //may// implement for extra credit that are described in the ISH man page are:
  * Job control (the & separator, ^Z handling, and the bg/fg/jobs builtins) - 10%
  * Pipelines - 15%

## Starter Source Code and Programming Assignment Restrictions 

Your shell should be written in C++, compile using cmake, and produce an executable named `ish`. I have provided a C++ parser for the subset of 'ish' assigned in this assignment in the src/ directory; the C++ starter code does *not* parse pipelines as they are not required so, to receive credit for pipeline extra credit, you will need to modify the parser to handle pipelines.

Your code must also be "clean" -- it must compile without warnings or unresolved references on a standard UNIX system with the Boost C++ libraries installed. The pre-provided test environment uses an Ubuntu container for testing. I have tested the provided code on the CS department Ubuntu systems, on Ubuntu running on WSL2 on a Windows systems, and MacOS. Your programs are expected to be well organized and easy to read, as well as correct.

You must use the `fork` and `execve` C system calls to start new processes and the `dup` or `dup2` command to handle file redirection. You may *not* use other variants of `exec` or the `system` call, their C++ equivalents, or any other mechanism for creating processes or redirecting file I/O.

## Provided of AI Coding Tools
You are //expected// to use AI coding tools to complete this assignment - while it is feasible to manually complete this assignment in the provided time, one of the goals of this assignment is to ensure you are comfortable with modern AI software engineering tools so that later in the semester we can use those tools to implement, evaluate, and analyze modern systems techniques that would //not// be feasible to code by hand in the provided time.

Here describe the AI coding tools provided to the students through AI-Verde.

## Testing

Testcases that test correctness of the shell output and of key shell components are included in the provided repository. These tests are only run on pulls to the main branch. I encourage you   will be provided approximately one week after the assignment is posted. Additional tests that test your shell error handling will be used for final testing ofyour program. A pull request will be submitted to your github repository adding test features when these testcases are available.

## Assignment submission

You will use GitHub classroom to submit a working program on or before the due date. Be sure to commit and push all of your changes to the `main` branch on your repository prior to the due date!

## Supporting and Reference Materials

Before starting, you should become familiar with the Unix system calls defined in Section 2. of the UNIX manual There are also several library routines in Section 3 that provide convenient interfaces to some of the more cryptic system calls. However, you may _not_ use the library routine `system` nor any of the routines prohibitied on the `ish` man page.  Several chapters of the Richard Stevens book _Advanced Programming in the UNIX Environment_ contain helpful information; Chapters 7, 8, and 9 are especially relevant, and this book is available online for free through the UNM library.

## Additional Advice

To implement `ish`, you will be creating a process that forks off other processes, which in turn forks off more processes, etc. If you inadvertently fork too many processes, you will cause Unix to run out, making yourself and everyone else on the machine very unhappy. _Be careful about this.

If you are in doubt about the functionality of `ish` or how it should behave in a particular situation, model the behavior on that of `csh`.  If you have specific questions about the project, ask in Discord. 

A few final bits of advice. First, and most importantly, get started early; you almost certainly have a lot to learn before you can start implementing anything.  Second, once you have a good understanding of what you are being asked to do, I strongly suggest that you develop a detailed design, implementation, and testing plan.  My personal style is to get functionality working one step at a time, for example, processing of simple commands, then environment handling and PATH searching, then I/O redirection. 
