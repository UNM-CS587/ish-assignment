# AI Coding Tools, Environments, and Workflow

This document supports the [`ish` assignment README](../README.md). It covers
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
     agent harnesses and backend models (the class LLM gateway among them)
     block direct WebSearch tool calls. I use FireCrawl as my MCP web
     search interface; you can use whichever you prefer.

### AI Agent Skills

Most AI agent harnesses have the ability to include _skills_ for the AI agent
which provide the AI specific instructions on how to do different things well.
If you want to try this out, there are a wide range of them in the Claude and 
Codex skill marketplaces. [_Superpowers_](https://github.com/obra/superpowers)
is one of the more popular sets of complex skills for medium scale software 
engineering systems.

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
  1. Builtin agent harness sandbox, for example Claude Code CLI's /sandbox 
     command
  1. Other OS-level tools to run AI code separately from your host account, 
     such as a VSCode dev container, a virtual machine, or other OS-level 
     sandboxing (e.g.  `sandbox-exec` on MacOS, a restricted user account or 
     namespace on Linux).  This keeps a runaway or misdirected command
     from reaching the rest of your files, credentials, or system.

## AI Model Availability

The class provides an LLM Lite gateway though the University of Arizona-hosted 
[AI Verde](chat.cyverse.ai) system for you to use to access a variety of AI models.
This includes:
  1. Unlimited access to state-of-the-art open models hosted on the NSF-funded
  [National Research Platform](www.nrp.ai) and [Jetstream 2](jetstream-cloud.org)
  systems. These models are, however, running on public systems and shared between 
  multiple users nation-wide, and their performance can vary significantly based on
  demand.
  1. Budgeted-limited access ($20/student/month) to Anthropic frontier models, 
  including Haiku, Sonnet, and Opus (not Fable).

You may also use other AI models (e.g. free models and personal subscriptions) if 
you choose, but I encourage you to use the =provided models to understand (and 
document!) their capabilities and shortcomings.

To access these models, you will need to:
  1. Login to AI Verde using your UNM netid 
  1. Copy your student-specific API endpoints, API keys, and available model names 
     for the `UNM - CS587` `UNM - CS587 - Claude` connection points.
  1. Set up your local agentic harness or model router to use these endpoints.

Specific details on how to do each of these steps are provided in the subsections below.

### Getting API information from AI Verde
As a first step, log in to AI Verde (https://chat.cyverse.ai) using your UNM NetID. 
when you first connect to AI Verde, it will ask you to select your identity provider;
select "University of New Mexico" as your identity provider, and then authenticate
with UNM to log in to AI Verde.

After you log in, you will see two projects to which you have access: `UNM - CS587` and
`UNM - CS587 - Claude`. For each of these projects, select the "Details" button, click on
`API Key`, and then copy provided API key and URL to your local computer.

Detailed documentation on usinug these endpoints is available from the API Documentation
link on that webpage; the process for using these endpoints with Codex CLI and Claude Code
are provided below.

### Configuring Codex CLI to use these endpoints

To configure Codex CLI to use these endpoints, you need to set the appropriate 
configuration options in either your environment, `${HOME}/.codex/config.toml`,
or both. I strongly recommend *not* storing API keys in general configuration
files; I keep mine in a password manager and when I log in, run a shell command
that downloads and sets them in environment variables which configuration files
then access. If you don't want to go that far, here's a simple setup:

In `${HOME}/.codex/config.toml`, add these lines to tell it to use the ai-verde
endpoint with an API key taken from the shell's environment:
```
[model_providers.ai-verde]
name = "ai-verde"
base_url = "https://llm-api.cyverse.ai/v1"
env_key = "AI_VERDE_API_KEY"
wire_api = "responses"

model_provider = "ai-verde"
model = "nrp/qwen3-small"
```

You can then set the environment variable `AI_VERDE_API_KEY` to the API for the
endpoint you want, and use the Codex CLI `/model' command to select the model
to use.

### Configuring Claude Code to use these endpoints
To configure Claude Code CLU to use these endpoints, you can create a 
project-specific settings file that sets the endpoint and again takes the API 
key from your environment. Specifically, I've already configured 
`.claude/settings.json` to include the following lines so that the API key is again
taken from the environment:
```
{
  "env": {
    "ANTHROPIC_BASE_URL": "https://llm-api.cyverse.ai",
    "ANTHROPIC_MODEL": "nrp/qwen3-small",
    "CLAUDE_CODE_DISABLE_EXPERIMENTAL_BETAS": "1"
  },
  "apiKeyHelper": "echo $AI_VERDE_API_KEY"
}
```

### Configuring other harnesses to use these endpoints
Most every AI harness can talk to the provided AI endpoints; consult the documentation
of your specific AI agent, from [AI Verde](https://aiverde-docs.cyverse.ai/api/), and
from [LiteLLM Gateway](https://www.litellm.ai), which AI Verde is an instance of,
for more information on how to do so.

### Available Models

#### Open Models
For the open model API key, the following models are available:
  * nrp/glm-4.7
  * nrp/minimax-m2
  * nrp/gpt-oss
  * nrp/gemma
  * nrp/kimi - Fast and reasonable open coding model
  * nrp/glm-5
  * nrp/qwen3 - large but open slow coding model
  * nrp/qwen3-small - smaller but still effective codeing model
  * js2/gpt-oss-120b - large and fast general purpose model
  * phi-4-multimodal-instruct

To get started, I suggest trying out `nrp/qwen3-small` for standard tasks and `nrp/qwen3`
for in-depth coding tasks that you're willing to wait for. Feel free to try other models, 
however; the National Research Platform provides a [comparison of the features of the models
available via NRP](https://nrp.ai/documentation/userdocs/ai/llm-managed/models/) as well
as a [status page](https://nrp.ai/llm-status) with the availability and current demands on 
its models.

**XXX Note that Open Model access through AI Verde is currently hit-or-miss and I am troubleshooting
it XXX**

#### Claude Models
For the Claude endpoint, the following models are available, listed from least capable
and expensive to most capable and expensive:
  * unm/claude-haiku-4-5
  * unm/claude-sonnet-4-6
  * unm/claude-sonnet-5
  * unm/claude-opus-4-8
  * unm/claude-opus-5

I recommend `unm/claude-sonnet-5` as a good default for this project if you're using 
Anthropic models.

**XXX Note that Claude access through AI Verde is currently broken and I am troubleshooting
it XXX**

## Limits on AI token usage.
AI Models are not cheap to use, and if you seek to aggressively use AI models
to complete this project, you will need to be thoughtful with AI usage because
of the performance and budget limits on these endpoints. Because you will likely 
be switching AI models between sessions, you should also make sure that your 
repository documents the information needed for a new AI session to get started 
quickly, for example by generating high-level design documents that can be 
shared between sessions and make sure to write the MEMORY.md file at the 
end of a session.

Not every request needs your most capable, most expensive model. Use a
fast, cheap model for mechanical work (boilerplate test cases, formatting,
straightforward bug fixes, text editing) and reserve your most capable model 
for work that needs deep reasoning, such as designing the job-control and pipeline
extensions to the parser or tracking down a race condition in process
management. Watch your token use on AI Verde as you go: a session that dumps 
large files or long command output into the model's context repeatedly burns
through tokens without adding understanding, so ask the agent to summarize
or point it at specific line ranges instead of whole files where you can.

## AI Software Engineering Workflows

A productive pattern for working with an AI coding agent on one feature at
a time:
  1. Branch. Create a feature branch off `main` for the single feature
     you're adding (e.g. `feature/io-redirection`).
  2. Orient the agent. Point it at the relevant section of the manual page
     and the existing parser code.
  3. Ask for a plan before code. Have the agent describe its approach and
     the test cases it intends to add before it writes implementation
     code, and review that plan yourself.
  4. Implement in small steps. Ask for one piece of functionality at a
     time (e.g. parse the `>` and `>>` operators before implementing
     redirection), and run the test suite after each step.
  5. Review the diff. Read every change the agent makes; do not accept a
     change you can't explain in the in-class test or to the instructor.
  6. Commit with a real message. Record what the feature is and, per this
     project's grading requirements, note what you prompted the AI to do.
  7. Open the pull request into `main` once the feature's tests pass
     locally.

For example, adding output redirection might go: branch
`feature/output-redirection` off `main`; ask the agent to explain how
the provided parser represents redirection targets; ask it to draft
failing test cases for `cmd > file` and `cmd >> file`; ask it to implement
the `dup2`-based redirection in the executor; run the test suite; commit;
open a pull request into `main`.

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
