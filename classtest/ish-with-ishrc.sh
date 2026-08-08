#!/bin/sh
# Runs ish with a known ~/.ishrc in place, for the startup-file test cases.
#
# ish finds its startup file through getpwuid(3), not $HOME, so this has to
# write into the real home directory; run-shelltest.sh cannot redirect it into
# the scratch sandbox and refuses to run when a .ishrc is already there. The
# save and restore below is a second line of defense for anyone running this
# script by hand outside the harness.
ISHHOME="${ISHHOME:-$(perl -e 'print((getpwuid($<))[7])')}"
SAVED=""
if [ -f "$ISHHOME/.ishrc" ]; then
    SAVED=$(mktemp)
    /bin/cp "$ISHHOME/.ishrc" "$SAVED"
fi

restore() {
    /bin/rm -f "$ISHHOME/.ishrc"
    if [ -n "$SAVED" ]; then
        /bin/cp "$SAVED" "$ISHHOME/.ishrc"
        /bin/rm -f "$SAVED"
    fi
}
trap restore EXIT INT TERM

/bin/cp ishrc.ish "$ISHHOME/.ishrc"
$1
