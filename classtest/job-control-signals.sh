#!/bin/bash
# Tests the parts of job control that need a real stop signal: the same
# SIGTSTP that pressing CTRL-Z sends on a real terminal. shelltest feeds ish
# its input from a plain file, not a pseudo-terminal, so a literal ^Z byte
# typed into a shelltest < block never reaches a terminal driver and never
# becomes a real signal. This script drives ish over a FIFO instead, so it
# can send a genuine `kill -TSTP` at the right moment and measure how long
# `fg` actually blocks.
set -u

ISH="$1"
WORKDIR=$(mktemp -d)
FIFO="$WORKDIR/in"
LOG="$WORKDIR/out"
mkfifo "$FIFO"

cleanup() {
  exec 3>&- 2>/dev/null
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

# Wait up to max_iters * 0.2s for pattern $1 to appear in $LOG.
wait_for() {
  local pattern="$1" max_iters="${2:-25}" i=0
  while ! grep -Eq "$pattern" "$LOG" 2>/dev/null; do
    i=$((i + 1))
    [ "$i" -ge "$max_iters" ] && return 1
    sleep 0.2
  done
  return 0
}

# Opening a FIFO for writing blocks until a reader opens it, so start ish
# reading from it first, then open our write end.
"$ISH" <"$FIFO" >"$LOG" 2>&1 &
ISH_PID=$!
exec 3>"$FIFO"

# Start a job in the background and find its process ID from the "[1] pid"
# message the man page requires when a job is backgrounded.
send "/bin/sleep 2 &"
wait_for '\[1\] [0-9]+' || fail "background job never printed its [1] pid"
JOB_PID=$(grep -Eo '\[1\] [0-9]+' "$LOG" | tail -1 | sed -E 's/\[1\] //')
[ -n "$JOB_PID" ] || fail "could not parse the backgrounded job's pid"

# Stop it the same way CTRL-Z would on a real terminal.
kill -TSTP "$JOB_PID" 2>/dev/null || fail "could not send SIGTSTP to pid $JOB_PID"
sleep 0.5
send "jobs"
sleep 0.5
tail -5 "$LOG" | grep -Eiq '\[1\].*stop' \
  || fail "jobs did not report job 1 as stopped after SIGTSTP"

# bg should resume it and jobs should stop calling it stopped.
send "bg %1"
sleep 0.5
send "jobs"
sleep 0.5
tail -5 "$LOG" | grep -Eiq 'stop' \
  && fail "job 1 is still reported stopped after bg"
kill -0 "$JOB_PID" 2>/dev/null || fail "job 1's process is gone after bg"

# fg should block until the job actually finishes, not return immediately.
START=$(date +%s)
send "fg %1"
send "/bin/echo FG_RETURNED"
wait_for 'FG_RETURNED' || fail "fg %1 never returned"
ELAPSED=$(( $(date +%s) - START ))
[ "$ELAPSED" -ge 1 ] \
  || fail "fg %1 returned in ${ELAPSED}s without waiting for the job to finish"

echo "PASS"
send "quit"
