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

# ish reads ~/.ishrc through getpwuid(3), so an existing one runs before this
# script's first command and puts extra lines in the log the checks below grep.
. "$(dirname "$0")/ishhome.sh"

WORKDIR=$(mktemp -d)
FIFO="$WORKDIR/in"
LOG="$WORKDIR/out"
mkfifo "$FIFO"

cleanup() {
  exec 3>&- 2>/dev/null
  kill "$ISH_PID" 2>/dev/null
  wait "$ISH_PID" 2>/dev/null
  # A failure below can leave the long-lived job of the kill test behind, and
  # killing ish does not reap it. Without this a failed run leaves a sleep on
  # the student's machine for half a minute.
  [ -n "${KILL_PID:-}" ] && kill "$KILL_PID" 2>/dev/null
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

# Byte offset of the end of the log, so a later check can read only what the
# shell printed after this point. ish writes its prompt without a newline, so
# the whole session is a handful of lines and a plain `tail -n` would re-read
# what an earlier command printed -- a check for the absence of a word then
# matches the very line it was supposed to supersede.
mark() {
  wc -c < "$LOG"
}

since() {
  tail -c "+$(($1 + 1))" "$LOG"
}

# True while pid $1 exists and has not become a zombie. ish reports a finished
# job before its next prompt, so a job killed with no command after it stays
# unreaped, and kill -0 alone would call it alive.
job_alive() {
  local st
  st=$(ps -o stat= -p "$1" 2>/dev/null | tr -d ' ')
  [ -n "$st" ] && [ "${st#Z}" = "$st" ]
}

# Opening a FIFO write-only blocks until a reader opens it, which would hang
# this script forever if ish died before its first read. Opening read-write
# never blocks, so a dead ish shows up as the explicit check below rather than
# as a test that never finishes.
"$ISH" <"$FIFO" >"$LOG" 2>&1 &
ISH_PID=$!
exec 3<>"$FIFO"
kill -0 "$ISH_PID" 2>/dev/null || fail "ish exited immediately instead of reading commands"

# Start a job in the background and find its process ID from the "[1] pid"
# message the man page requires when a job is backgrounded. The job has to
# outlast the fixed delays below: sleep counts the time it spends stopped
# against its argument, so a job shorter than the roughly two seconds this
# script spends getting to fg is already gone by the time fg runs.
send "/bin/sleep 5 &"
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

# bg should resume it and jobs should stop calling it stopped. %1 is the job
# number reported in "[1] pid" above, not a process ID.
send "bg %1"
sleep 0.5
BG_MARK=$(mark)
send "jobs"
sleep 0.5
since "$BG_MARK" | grep -Eiq 'stop' \
  && fail "job 1 is still reported stopped after bg"
kill -0 "$JOB_PID" 2>/dev/null || fail "job 1's process is gone after bg"

# fg should block until the job actually finishes, not return immediately.
START=$(date +%s)
send "fg %1"
send "/bin/echo FG_RETURNED"
wait_for 'FG_RETURNED' 75 || fail "fg %1 never returned"
ELAPSED=$(( $(date +%s) - START ))
[ "$ELAPSED" -ge 1 ] \
  || fail "fg %1 returned in ${ELAPSED}s without waiting for the job to finish"

# kill %job must signal the job it names, not a process whose ID happens to
# match. The man page says it sends TERM and then CONT, so the job's process is
# gone afterwards. Job 1 has finished, and the man page doesn't say whether the
# next job reuses its number, so read the number the shell actually reports.
KILL_MARK=$(mark)
send "/bin/sleep 30 &"
sleep 0.5
KILL_JOB=$(since "$KILL_MARK" | grep -Eo '\[[0-9]+\] [0-9]+' | tail -1)
[ -n "$KILL_JOB" ] || fail "backgrounding a second job printed no [N] pid line"
KILL_NUM=$(echo "$KILL_JOB" | sed -E 's/\[([0-9]+)\].*/\1/')
KILL_PID=$(echo "$KILL_JOB" | sed -E 's/\[[0-9]+\] //')

send "kill %$KILL_NUM"
for _ in $(seq 1 25); do
  job_alive "$KILL_PID" || break
  sleep 0.2
done
job_alive "$KILL_PID" \
  && fail "kill %$KILL_NUM left the job's process $KILL_PID running"

echo "PASS"
send "quit"
