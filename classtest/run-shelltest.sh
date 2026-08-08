#!/bin/sh
# Runs one shelltest .test file against ish in a scratch directory.
#
# The .test files reference fixtures by relative path and several of them
# create, rewrite, or chmod files as a side effect of what they test. Running
# them directly in classtest/ therefore leaves untracked scratch files in the
# git tree, makes results depend on what a previous run left behind, and lets a
# wrong shell leave the tracked testscript.sh at mode 000. Copying the fixtures
# into a fresh directory per test avoids all of that, and gives each test its
# own copies so ctest -j is safe.
#
# The prompt and HOME are resolved here rather than baked in at CMake configure
# time, so a hostname that changes with the network or a rebuilt container
# doesn't silently produce a wrong expected prompt.
#
# Usage: run-shelltest.sh <fixture-dir> <ish-binary> <test-file> [--needs-nonroot]

set -eu

FIXTURES="$1"
ISH="$2"
TESTFILE="$3"
NEEDS_NONROOT="${4:-}"

# Root bypasses permission bits, so a test that asserts "Permission denied"
# cannot pass as root no matter how the target is created. Report that as a
# skip with a reason rather than as a mysterious output mismatch. CI runs as a
# normal user, so grading is unaffected.
if [ "$NEEDS_NONROOT" = "--needs-nonroot" ] && [ "$(id -u)" = "0" ]; then
    echo "SKIPPED: $TESTFILE asserts a permission-denied error, which cannot"
    echo "happen when running as root. Run the test suite as a non-root user."
    exit 77
fi

SCRATCH=$(mktemp -d)
# Resolve symlinks now: /bin/pwd inside ish prints the physical path, so any
# path an expectation is built from has to be physical too. macOS puts mktemp
# output under /var, which is a symlink to /private/var.
SCRATCH=$(cd "$SCRATCH" && pwd -P)

cleanup() {
    chmod -R u+rwX "$SCRATCH" 2>/dev/null || true
    rm -rf "$SCRATCH"
}
trap cleanup EXIT INT TERM

cp "$FIXTURES"/*.test "$FIXTURES"/*.sh "$FIXTURES"/*.ish "$FIXTURES"/redirectin.txt "$SCRATCH"/

# A directory the tests can redirect into to provoke a permission error,
# instead of depending on / and /etc being unwritable. Those are writable for a
# root default user (devcontainer, bare docker run, some WSL images), where the
# old tests both failed and left files behind on the student's system.
mkdir "$SCRATCH/nowrite"
chmod 500 "$SCRATCH/nowrite"

cd "$SCRATCH"

# The manual page says no environment variables are set for ish initially, so
# ish cannot learn where home is from $HOME. Like csh, it has to ask
# getpwuid(3). Point $HOME at the scratch directory and take the expected home
# from getpwuid instead: a shell that calls getenv("HOME") then lands in the
# scratch directory and fails the cd cases, which is the point.
HOME="$SCRATCH"
export HOME

ISHHOME=$(perl -e 'print((getpwuid($<))[7])' 2>/dev/null || true)
if [ -z "$ISHHOME" ] || [ ! -d "$ISHHOME" ] || [ ! -w "$ISHHOME" ]; then
    echo "SKIPPED: getpwuid(3) reports no writable home directory for uid"
    echo "$(id -u), so cd with no arguments and ~/.ishrc have nothing to"
    echo "resolve to. Run the test suite as a user with a home directory."
    exit 77
fi
# Physical path, for the reason given above the scratch directory.
ISHHOME=$(cd "$ISHHOME" && pwd -P)
export ISHHOME

# The .ishrc cases install a startup file in that home directory, because that
# is where ish looks for it and no $HOME override can move it. Refuse to run
# on top of one that is already there: overwriting it would destroy the
# student's file, and leaving it in place would feed extra commands into every
# case that counts prompts and fail them for a reason the output never shows.
if [ -e "$ISHHOME/.ishrc" ]; then
    echo "SKIPPED: $ISHHOME/.ishrc exists. ish finds its startup file through"
    echo "getpwuid(3) rather than \$HOME, so the tests cannot redirect it into"
    echo "a sandbox. Move the file aside and run the tests again."
    exit 77
fi

# uname -n reports what gethostname(2) returns, which is what ish itself will
# use to build the prompt.
PROMPT="$(uname -n)%"

# -o bounds each case. Without it shelltest waits forever, and the most likely
# student bug (a parent that never closes the pipe write end) hangs the whole
# run with no output.
exec shelltest -o 20 \
    -DPROMPT="$PROMPT" \
    -DSHELL="$ISH" \
    -DHOME="$ISHHOME" \
    "$TESTFILE"
