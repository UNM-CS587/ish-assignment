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
# Resolve symlinks now: /bin/pwd inside ish prints the physical path, so a
# HOME that traverses a symlink (macOS /var, some WSL setups) would make a
# correct cd fail the comparison.
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

# Point HOME at the scratch directory. This keeps ~/.ishrc handling inside the
# test sandbox: the suite can neither destroy a student's real ~/.ishrc nor
# read one and fail every test that counts prompts.
HOME="$SCRATCH"
export HOME

# uname -n reports what gethostname(2) returns, which is what ish itself will
# use to build the prompt.
PROMPT="$(uname -n)%"

# -o bounds each case. Without it shelltest waits forever, and the most likely
# student bug (a parent that never closes the pipe write end) hangs the whole
# run with no output.
exec shelltest -o 20 \
    -DPROMPT="$PROMPT" \
    -DSHELL="$ISH" \
    -DHOME="$SCRATCH" \
    "$TESTFILE"
