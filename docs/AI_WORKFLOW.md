# AI Coding Tools, Environments, and Workflow

This document supports the [`ish` assignment README](../README.md). It covers
the AI coding harnesses, MCP services, sandboxing options, model guidance,
and recommended development workflow for the assignment.

To use AIs for coding, you'll need:
  1. An *harness* that connects an AI model to you and the outside world
  1. Access to a model through either a subscription or endpoint
  1. To understand some basics about the capabilities and costs of models

This section provides you this information.

## AI Coding Harnesses
Any of the following AI coding harnesses will work for this assignment;
pick one and learn it well rather than switching between several.
  * **Claude Code** (Anthropic) - a terminal-based agent. Install with the
    native installer described at https://code.claude.com/docs/en/setup,
    or with Homebrew (`brew install --cask claude-code`) on MacOS or
    Linuxbrew.
  * **OpenCode** - An open-source coding agent that works with a wide
    range of models and interfaces. You can get documentation and download
    it from the [OpenCode Website](https://opencode.ai).
  * **Cline** - a VS Code extension. Install it from the VS Code
    Extensions panel (search "Cline") or from the Marketplace at
    https://marketplace.visualstudio.com/items?itemName=saoudrizwan.claude-dev.
    Documentation is at https://docs.cline.bot.
  * **Codex CLI** (OpenAI) - [SEE WARNING BELOW] a terminal-based agent from 
    OpenAI. Install with `npm install -g @openai/codex` (requires Node.js 18+, 
    available via `apt install nodejs npm` on Ubuntu or `brew install node`) 
    or with `brew install --cask codex`. Documentation and source are at
    https://github.com/openai/codex. **WARNING**: Codex no longer works with
    many Lite LLM gateways, including the AI Verde endpoint we are using in 
    this class. If you want to use it, please contact me and I will work 
    with you to get you the endpoint configuration.

## Accessing AI Models

AI models can be accessed through a variety of ways, including web chat interfaces,
but when using an AI coding harness they are typically accessed through either a 
subscription service or an API endpoint.
   * AI subscription services (e.g. Claude Pro/Max from Anthropic) are generally 
     specific to a single harness/AI provider and provide a daily, weekly, or monthly
     budget of AI token to use for (generally) a fixed fee.
   * API endpoints allow you to connect an AI harness directly to a model provider
     and, in the case of commercial providers, bills for usage.

The class provides you API access to AI models through the [AI Verde](chat.cyverse.ai) 
LLM Lite gateway hosted at the University of Arizona. Your endpoints on this gateway are 
backed by both open models hosted on NSF-funded systems, and by a UNM funded Claude 
Code API backend. There are no limits on your use of the open models, while you have 
$15/week budget for the Claude Code endpoint.  You may also use other AI models (e.g.,
other free free models or personal subscriptions to which you have access) if 
you choose, but I encourage you to use the provided models to understand (and 
document!) their capabilities and shortcomings.

To access these models, you will need to:
  1. Login to AI Verde using your UNM netid 
  1. Copy your student-specific API endpoints, API keys, and available model names 
     from the `API` tab of the `UNM - CS587` `UNM - CS587 - Claude` connection points.
  1. Set up your local agentic harness or model router to use these endpoints.

Specific details on how to do each of these steps are provided below.

### Getting API information from AI Verde
As a first step, log in to [AI Verde](https://chat.cyverse.ai) using your UNM NetID.
When you first connect to AI Verde, it will ask you to select your identity provider;
select "University of New Mexico" as your identity provider, and then authenticate
with UNM to log in to AI Verde.

After you log in, you will see two projects to which you have access: `UNM - CS587` and
`UNM - CS587 - Claude`. For each of these projects, select the "Details" button, click on
`API Key`, and then copy the provided API key and URL to your local computer.

Detailed documentation on using these endpoints is available from the API Documentation
link on that webpage; the process for using these endpoints with Claude Code
is provided below.

### Configuring your harness to use these endpoints.

Almost every AI harness can talk to the provided AI endpoints. I've provided information
on a how to connect two of them below, but if you need additional information, please 
consult the documentation of your specific AI agent harness, the
[AI Verde API documentation](https://aiverde-docs.cyverse.ai/api/), and the
[LiteLLM Gateway documentation](https://www.litellm.ai) which AI Verde is an instance of.

#### Claude Code
Claude Code to use these endpoints
To configure Claude Code CLI to use these endpoints, you can create either a user-specific
or project-specific settings file that sets the endpoint and takes the API 
key from your environment as well as any other harness-specific information needed to connect 
the harness to the endpooint. I have provided example project-specific files for
Claude Code in the project's [.claude/ directory](../.claude). Simply copy the file
you want to use to `.claude/settings.json`, set the `AI_VERDE_API_KEY` environment variable
to the matching Claude or Open API key, and start claude _from the top-level project
directory_. 

#### Codex CLI

#### Note on storing API Keys.

### Available Models

#### Claude Models
For the Claude endpoint, the following models are available, listed from least capable
and expensive to most capable and expensive:
  * unm-cs587/claude-haiku-4-5
  * unm-cs587/claude-sonnet-4-6
  * unm-cs587/claude-sonnet-5
  * unm-cs587/claude-opus-4-8
  * unm-cs587/claude-opus-5

I recommend `unm-cs587/claude-sonnet-5` as a good default for this project if you're using 
Anthropic models.

#### Open Models
For the open model API key, the following models are available:
  * nrp/glm-4.7
  * nrp/minimax-m2
  * nrp/gpt-oss
  * nrp/gemma
  * nrp/kimi
  * nrp/glm-5
  * nrp/qwen3
  * nrp/qwen3-small
  * js2/gpt-oss-120b
  * phi-4-multimodal-instruct

To get started, I suggest trying out `nrp/qwen3-small` for standard tasks and `nrp/qwen3`
for in-depth coding tasks that you're willing to wait for. If you're using it with Claude Code, 
refer to it as `nrp/qwen3-small[1m]` so that Claude knows to use the full 1 million token context
window. Feel free to try other models, too; the National Research Platform provides a 
[comparison of the features of the models available via 
NRP](https://nrp.ai/documentation/userdocs/ai/llm-managed/models/) as well
as a [status page](https://nrp.ai/llm-status) with the availability and current demands on 
its models.

## Limits on AI token usage.
AI Models are not cheap to use, and if you seek to aggressively use AI models
to complete this project, you will need to be thoughtful with AI usage because
of the performance and budget limits on these endpoints. Because you will likely 
be switching AI models between sessions, you should also make sure that your 
repository documents the information needed for a new AI session to get started 
quickly, for example by generating high-level design documents that can be 
shared between sessions.

Not every request needs your most capable, most expensive model. Use a
fast, cheap model for mechanical work (boilerplate test cases, formatting,
straightforward bug fixes, text editing) and reserve your most capable model 
for work that needs deep reasoning, such as designing the job-control and pipeline
extensions to the parser or tracking down a race condition in process
management. Watch your token use on AI Verde as you go: a session that dumps 
large files or long command output into the model's context repeatedly burns
through tokens without adding understanding, so ask the agent to summarize
or point it at specific line ranges instead of whole files where you can.

## Advanced AI Harness Capabilities and Techniques
In addition to standard AI usage, most AI harnesses feature a range of 
more sophisticated capabilities to increase the capabilities for carrying out
complex tasks. In addition, there are a variety of sopisticated techniques
you can take advantage of to use these harnesses more effectively.

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

### AI Agent Sandboxing

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

## AI Software Engineering Workflows
Once your AI harness and models are well-configured, you are ready to use
AI techniques to assist you in writing code. Because agents make mistakes,
it is imperative that you use an effective software engineering workflow
to manage your AI junior software engineer. 

A simple, productive pattern for working with an AI coding agent 
is to focus on implementing on one feature at a time:
  1. Branch. Create a feature branch off `main` for the single feature
     you're adding (e.g. `feature/io-redirection`).
  2. Orient the agent. Point it at the relevant section of the manual page,
     requirements document, and the existing code.
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

Finally, at the end of each work session, ask your agent to summarize the session
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
