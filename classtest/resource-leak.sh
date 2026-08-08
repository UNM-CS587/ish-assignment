#!/bin/bash
# Tests that ish doesn't leak file descriptors or leave zombie/orphaned
# child processes behind after running several rounds of redirected and
# backgrounded commands. Needs direct inspection of the running ish
# process (open fd count, child process state), which isn't expressible
# as shelltest input/output matching, so (like job-control-signals.sh and
# pipeline-signals.sh) this drives ish over a FIFO instead.
set -u

ISH="$1"

# ish reads ~/.ishrc through getpwuid(3), so an existing one runs before this
# script's first command and shifts the open-descriptor baseline the checks
# below compare against exactly.
. "$(dirname "$0")/ishhome.sh"

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

# Count ish's open file descriptors, using /proc where available (Linux,
# including the CI runner this is graded on) and falling back to lsof
# elsewhere (e.g. running the suite locally on MacOS). Prints nothing if
# neither is available.
fd_count() {
  if [ -d "/proc/$1/fd" ]; then
    ls "/proc/$1/fd" 2>/dev/null | wc -l | tr -d ' '
  elif command -v lsof >/dev/null 2>&1; then
    lsof -p "$1" 2>/dev/null | wc -l | tr -d ' '
  fi
}

# Opening a FIFO write-only blocks until a reader opens it, which would hang
# this script forever if ish died before its first read. Opening read-write
# never blocks, so a dead ish shows up as the explicit check below rather than
# as a test that never finishes.
"$ISH" <"$FIFO" >"$LOG" 2>&1 &
ISH_PID=$!
exec 3<>"$FIFO"
kill -0 "$ISH_PID" 2>/dev/null || fail "ish exited immediately instead of reading commands"
sleep 0.5

BASELINE_FDS=$(fd_count "$ISH_PID")
[ -n "$BASELINE_FDS" ] || fail "could not count ish's open file descriptors (no /proc and no lsof on this system); cannot check for fd leaks here"

# Redirected commands each open at least one file; ish should close it
# again once the command finishes.
send "/bin/echo redirect test > $WORKDIR/out.txt"
send "/bin/cat < $WORKDIR/out.txt > $WORKDIR/out2.txt"
send "/bin/cat < $WORKDIR/out.txt >> $WORKDIR/out2.txt"
sleep 0.5
AFTER_REDIRECT_FDS=$(fd_count "$ISH_PID")
[ "$AFTER_REDIRECT_FDS" -eq "$BASELINE_FDS" ] \
  || fail "fd count grew from $BASELINE_FDS to $AFTER_REDIRECT_FDS after redirected commands; ish is leaking file descriptors"

# Backgrounded jobs that finish should be reaped, not left as zombies, and
# shouldn't leave any fds open either.
send "/bin/sleep 0.2 &"
send "/bin/sleep 0.2 &"
send "/bin/sleep 0.2 &"
wait_for '\[3\] [0-9]+' || fail "third background job never printed its [3] pid"
sleep 1
send "jobs"
sleep 0.5
tail -5 "$LOG" | grep -Eiq 'running|stop' \
  && fail "a background job is still reported active after it should have finished"

ZOMBIES=$(ps -A -o ppid=,stat= 2>/dev/null | awk -v ppid="$ISH_PID" '$1 == ppid && $2 ~ /^Z/' | wc -l | tr -d ' ')
[ "$ZOMBIES" -eq 0 ] \
  || fail "ish left $ZOMBIES zombie child process(es) after its background jobs finished"

AFTER_JOBS_FDS=$(fd_count "$ISH_PID")
[ "$AFTER_JOBS_FDS" -eq "$BASELINE_FDS" ] \
  || fail "fd count grew from $BASELINE_FDS to $AFTER_JOBS_FDS after backgrounding jobs; ish is leaking file descriptors"

echo "PASS"
send "quit"
