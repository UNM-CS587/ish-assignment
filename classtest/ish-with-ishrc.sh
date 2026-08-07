#!/bin/sh
# Runs ish with a known ~/.ishrc in place, for the startup-file test cases.
#
# run-shelltest.sh points HOME at a scratch directory, so the .ishrc written
# here is the scratch one and a student's real ~/.ishrc is never touched. The
# save and restore below is a second line of defense for anyone running this
# script by hand outside the harness.
SAVED=""
if [ -f "$HOME/.ishrc" ]; then
    SAVED=$(mktemp)
    /bin/cp "$HOME/.ishrc" "$SAVED"
fi

restore() {
    /bin/rm -f "$HOME/.ishrc"
    if [ -n "$SAVED" ]; then
        /bin/cp "$SAVED" "$HOME/.ishrc"
        /bin/rm -f "$SAVED"
    fi
}
trap restore EXIT INT TERM

/bin/cp ishrc.ish "$HOME/.ishrc"
$1
