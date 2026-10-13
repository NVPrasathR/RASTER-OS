#!/usr/bin/env bash
# PACSCORDER development smoke test (not a canonical hardware test).
#
# Runs the recorder with the dev backend (videotestsrc + x264enc stand-ins) for
# 12 s at 1280x720@60, splitting every 4 s and mirroring to two directories,
# then checks: clean exit (end of stream reached); at least two files in each
# directory; identical names and the same contents in both (the mirror,
# ADR-009: byte for byte apart from the header time stamps, tests/mp4_same.py);
# every file a fragmented MP4 ('ftyp' and 'moof' boxes [J-39]);
# the recording encode nearly every captured frame and the live branch about
# half of them (60 -> 30 fps, OQ-131); and, if ffprobe is installed, each file
# readable, starting within one frame period of 0, in High profile (OQ-005).
#
# Proves the pipeline logic on the development machine only.
set -euo pipefail

BIN=${1:?usage: smoke_record.sh <path to pacscorder>}
HERE=$(cd "$(dirname "$0")" && pwd)
T=$(mktemp -d "${TMPDIR:-/tmp}/pacs-smoke.XXXXXX")
trap 'rm -rf "$T"' EXIT

fail() { echo "FAIL: $*" >&2; exit 1; }

"$BIN" --backend dev --width 1280 --height 720 --fps 60 \
       --duration 12 --split-seconds 4 --fragment-ms 500 \
       --rec-dir "$T/a" --rec-dir "$T/b" >"$T/run.log" 2>&1 \
  || { cat "$T/run.log" >&2; fail "pacscorder exited with an error"; }

line=$(grep 'frames captured' "$T/run.log" || true)
[ -n "$line" ] || fail "no frame counts in the output"
cap=$(echo "$line" | sed -E 's/.*captured ([0-9]+).*/\1/')
rec=$(echo "$line" | sed -E 's/.*recording encode ([0-9]+).*/\1/')
live=$(echo "$line" | sed -E 's/.*live encode ([0-9]+).*/\1/')
[ "$cap" -gt 600 ] || fail "only $cap frames captured in 12 s at 60 fps"
[ $((rec * 100)) -ge $((cap * 95)) ] || fail "recording encoded $rec of $cap frames"
[ $((live * 100)) -ge $((cap * 45)) ] && [ $((live * 100)) -le $((cap * 55)) ] \
  || fail "live branch passed $live of $cap frames (expected about half)"

names_a=$(cd "$T/a" && ls -1 *.mp4 2>/dev/null || true)
names_b=$(cd "$T/b" && ls -1 *.mp4 2>/dev/null || true)
count=$(printf '%s\n' "$names_a" | grep -c . || true)
[ "$count" -ge 2 ] || fail "expected at least 2 files in $T/a, found $count"
[ "$names_a" = "$names_b" ] || fail "mirror directories differ: [$names_a] vs [$names_b]"

for d in a b; do
  for f in "$T/$d"/*.mp4; do
    size=$(wc -c < "$f" | tr -d ' ')
    [ "$size" -gt 1024 ] || fail "$f is only $size bytes"
    LC_ALL=C grep -q -a ftyp "$f" || fail "$f has no ftyp box"
    LC_ALL=C grep -q -a moof "$f" || fail "$f is not fragmented (no moof box)"
    if command -v ffprobe >/dev/null 2>&1; then
      info=$(ffprobe -v error -select_streams v:0 \
             -show_entries stream=profile,start_time -of compact=p=0 "$f") \
        || fail "ffprobe cannot read $f"
      case "$info" in *"profile=High"*) ;; *) fail "$f is not High profile: $info" ;; esac
      start=$(echo "$info" | sed -E 's/.*start_time=([0-9.]+).*/\1/')
      awk -v t="$start" 'BEGIN { exit !(t < 1/60) }' \
        || fail "$f does not start within one frame of 0: $info"
    fi
  done
done
for f in "$T/a"/*.mp4; do
  same=$(python3 "$HERE/mp4_same.py" "$f" "$T/b/$(basename "$f")") \
    || fail "mirror copies of $(basename "$f") differ"
  case "$same" in *" 0 of "*) ;; *) echo "NOTE: $(basename "$f"): $same (allowed, OQ-133)" ;; esac
done

echo "PASS (development smoke test): $count fragmented MP4 files, the same in 2 directories;"
echo "     frames captured $cap, recording $rec, live $live"
