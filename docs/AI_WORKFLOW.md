# AI Coding Tools, Environments, and Workflow

This document supports the [`ish` assignment README](README.md). It covers
the AI coding harnesses, MCP services, sandboxing options, model guidance,
and recommended development workflow for the assignment.

## AI Coding Tools

### AI Coding Harnesses
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

### AI MCP Agents
Model Context Protocol (MCP) services provide additional features that allow
an agentic harness to better access system services. Here are some potential
ones you might want to consider:
  1. For this project, I recommend that you set up a GitHub MCP interface so 
     your agent harness can read and file issues, open and update pull requests,
     and check Actions run status directly. Otherwise, you will need to copy 
     that information back and forth by hand. Use the official GitHub MCP server
     (https://github.com/github/github-mcp-server); each harness above 
     documents how to add an MCP server to it (in Claude Code, for example, 
     `claude mcp add -s user --transport http github https://api.githubcopilot.com/mcp -H "Authorization: Bearer YOUR_ACTUAL_TOKEN_HERE"`).
     Use a GitHub personal access token scoped to your repository only here,
     not a broad or account-wide token.
  1. You may also want to set up an MCP web search service, since some
     agent harnesses and backend models (e.g. the UNM Claude API models)
     block direct WebSearch tool calls. I use FireCrawl as my MCP web
     search interface; you can use whichever you prefer.

## AI Agent Sandboxing

AI agents may run shell commands and compile and execute your code on your 
behalf, which can be risky; an AI agent could, for example, delete all of your
files if you were not paying attention. As a result, agentic harnesses by
default ask your permission before they do anything that might be dangerous,
which can mean you spend a lot of time watching an agent churn and approving
each step.

An alternative to this I strongly recommend is to run your AI agents in a 
_sandbox_, basically a separate account, container, or virtual machine which
limits what the agent can do both in terms of local files and the network.
By doing so, you can YOLO the agent and have the agent framework approve every
request, relying on the sandbox's file and network protections to prevent it 
from harming anything. 

Potential sandboxing solutions include:
  1. Docker Desktop (https://docs.docker.com/ai/sandboxes/) and the sbx command.
     Runs on a wide range of platforms and is free for individual use.
  1. sandvault (MacOS only) - a dedicated microVM solution for OS X that, along 
     with using a separate shared directory between a sandvault user and a 
     regular account is a lightweight sandboxing approach. What I use.
  1. Other OS-level tools to run AI code separately from your host account, 
     such as a VSCode dev container, a virtual machine, or other OS-level 
     sandboxing (e.g.  `sandbox-exec` on MacOS, a restricted user account or 
     namespace on Linux).  This keeps a runaway or misdirected command
     from reaching the rest of your files, credentials, or system.

## AI Model Availability

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

## AI Software Engineering Workflows

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
