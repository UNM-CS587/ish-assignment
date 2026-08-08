# Resolves the home directory ish will actually use and refuses to run on top
# of an existing startup file. Sourced by run-shelltest.sh and by the three
# script-driven categories, all of which start an ish that reads ~/.ishrc
# through getpwuid(3) no matter what $HOME says, so no environment setting can
# steer them away from the student's real file.
#
# Sets and exports ISHHOME. Calls ish_bail on a condition the tests cannot work
# around; the caller may call it too.

# A skip is the right answer locally: the student can fix their environment and
# run the tests again. Under CI it is not, because ctest reports a skipped
# category as a pass and the autograder awards full marks on exit 0 -- so a
# stub shell would score 100 in an environment that skipped every category.
ish_bail() {
    if [ -n "${CI:-}" ]; then
        exit 1
    fi
    exit 77
}

if [ -n "${CI:-}" ]; then
    ISH_BAIL="FAILED"
else
    ISH_BAIL="SKIPPED"
fi

# python3 rather than perl: the devcontainer's postCreateCommand installs
# neither, and python3 is in the base image, on the GitHub runners, and on
# MacOS. getent(1) is on none of the three.
ISHHOME=$(python3 -c 'import os, pwd; print(pwd.getpwuid(os.getuid()).pw_dir)' 2>/dev/null || true)
if [ -z "$ISHHOME" ] || [ ! -d "$ISHHOME" ] || [ ! -w "$ISHHOME" ]; then
    echo "$ISH_BAIL: getpwuid(3) reports no writable home directory for uid"
    echo "$(id -u), so cd with no arguments and ~/.ishrc have nothing to"
    echo "resolve to. Run the test suite as a user with a home directory."
    ish_bail
fi
# Resolve symlinks: /bin/pwd inside ish prints the physical path, so any path
# an expectation is built from has to be physical too.
ISHHOME=$(cd "$ISHHOME" && pwd -P)
export ISHHOME

# The .ishrc cases install a startup file in that home directory, because that
# is where ish looks for it. Refuse to run on top of one that is already there:
# overwriting it would destroy the student's file, and leaving it in place
# would feed extra commands into every case that counts prompts and fail them
# for a reason the output never shows.
if [ -e "$ISHHOME/.ishrc" ]; then
    echo "$ISH_BAIL: $ISHHOME/.ishrc exists. ish finds its startup file through"
    echo "getpwuid(3) rather than \$HOME, so the tests cannot redirect it into"
    echo "a sandbox. Move the file aside and run the tests again."
    ish_bail
fi
