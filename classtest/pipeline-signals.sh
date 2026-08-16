#!/bin/bash
# Tests that job control's stop/continue signals apply to every process in a
# backgrounded pipeline, not just one, per the "Support all job control
# features on pipelines" requirement in the README. See
# job-control-signals.sh for why this needs a real signal instead of a
# shelltest < block, and for the FIFO-driving approach used here.
#
# set -m matters as much as set -u here. bash does not enable job control in a
# script, so without it ish stays in this script's process group, and the
# `kill -TSTP "-$PGID"` below names that group and stops the test itself --
# which ctest then reports only after waiting out the full TIMEOUT. With it,
# ish gets a process group of its own and the checks fail in seconds.
set -u
set -m

ISH="$1"

# ish reads ~/.ishrc through getpwuid(3), so an existing one runs before this
# script's first command and puts extra lines in the log the checks below grep.
. "$(dirname "$0")/ishhome.sh"

WORKDIR=$(mktemp -d)
FIFO="$WORKDIR/in"
LOG="$WORKDIR/out"
mkfifo "$FIFO"

cleanup() {
  exec 3>&- 2>/dev/null
  # A stopped ish does not die from the TERM below, and the wait then blocks
  # forever. Continue it first so the signal can be delivered.
  kill -CONT "$ISH_PID" 2>/dev/null
  kill "$ISH_PID" 2>/dev/null
  wait "$ISH_PID" 2>/dev/null
  rm -rf "$WORKDIR"
}
trap cleanup EXIT

fail() {
  echo "FAIL: $1"
  echo "--- captured ish output ---"
  cat "$LOG"
  exit 1
}

send() {
  echo "$1" >&3
}

wait_for() {
  local pattern="$1" max_iters="${2:-25}" i=0
  while ! grep -Eq "$pattern" "$LOG" 2>/dev/null; do
    i=$((i + 1))
    [ "$i" -ge "$max_iters" ] && return 1
    sleep 0.2
  done
  return 0
}

# Byte offset of the end of the log, so a later check can read only what the
# shell printed after this point. See job-control-signals.sh for why a plain
# `tail -n` re-reads output an earlier command produced.
mark() {
  wc -c < "$LOG"
}

since() {
  tail -c "+$(($1 + 1))" "$LOG"
}

# Opening a FIFO write-only blocks until a reader opens it, which would hang
# this script forever if ish died before its first read. Opening read-write
# never blocks, so a dead ish shows up as the explicit check below rather than
# as a test that never finishes.
"$ISH" <"$FIFO" >"$LOG" 2>&1 &
ISH_PID=$!
exec 3<>"$FIFO"
kill -0 "$ISH_PID" 2>/dev/null || fail "ish exited immediately instead of reading commands"

# Background a two-stage pipeline and find one of its process IDs from the
# "[1] pid pid" message. The pipeline has to outlast the fixed delays below:
# sleep counts the time it spends stopped against its argument, so a pipeline
# shorter than the roughly two seconds this script spends getting to fg is
# already gone by the time fg runs.
send "/bin/sleep 5 | /bin/cat &"
wait_for '\[1\] [0-9]+ [0-9]+' || fail "backgrounded pipeline never printed [1] pid pid"
PIPE_PID=$(grep -Eo '\[1\] [0-9]+' "$LOG" | tail -1 | sed -E 's/\[1\] //')
[ -n "$PIPE_PID" ] || fail "could not parse a pid from the backgrounded pipeline"

# A real terminal delivers CTRL-Z to the whole foreground process group at
# once, so stop the pipeline's process group rather than a single pid. This
# means a shell that doesn't give each pipeline its own process group (e.g.
# via setpgid) will end up stopping itself along with the pipeline, and the
# checks below will time out and fail -- that is the expected outcome for
# such a bug, since the pipeline stops being able to answer "jobs".
PGID=$(ps -o pgid= -p "$PIPE_PID" 2>/dev/null | tr -d ' ')
[ -n "$PGID" ] || fail "could not look up the pipeline's process group"
kill -TSTP "-$PGID" 2>/dev/null || fail "could not send SIGTSTP to process group $PGID"
sleep 0.5
send "jobs"
sleep 0.5
tail -5 "$LOG" | grep -Eiq '\[1\].*stop' \
  || fail "jobs did not report the pipeline as stopped after SIGTSTP"

# %1 is the job number reported in "[1] pid pid" above, not a process ID.
send "bg %1"
sleep 0.5
BG_MARK=$(mark)
send "jobs"
sleep 0.5
since "$BG_MARK" | grep -Eiq 'stop' \
  && fail "pipeline is still reported stopped after bg"

# fg should block until every stage of the pipeline has finished.
START=$(date +%s)
send "fg %1"
send "/bin/echo FG_RETURNED"
wait_for 'FG_RETURNED' 75 || fail "fg %1 never returned"
ELAPSED=$(( $(date +%s) - START ))
[ "$ELAPSED" -ge 1 ] \
  || fail "fg %1 returned in ${ELAPSED}s without waiting for the pipeline to finish"

echo "PASS"
send "quit"
