#!/usr/bin/env bash
# PACSCORDER development test for drive failure and return (slice C5; not canonical).
#
# Case A — drive missing at start, then returning (OQ-129): drive B's directory
#   cannot be created because a plain file blocks its path; after 6 s the file
#   is removed. Expect an ALERT for B at start, B rejoining at the next split,
#   drive A recording throughout, and from B's first file on, the same file
#   names with the same contents on both drives (byte for byte apart from the
#   header time stamps, which each drive's muxer takes from the wall clock;
#   tests/mp4_same.py).
# Case B — drive full during a recording (OQ-129): drive B is a small, empty
#   filesystem — $PACS_TEST_FULL_DIR if set (e.g. an 8 MB tmpfs, as in
#   tools/linux_check.sh), otherwise an 8 MB macOS RAM disk (hdiutil); skipped
#   when neither is available. Expect an ALERT for B, drive A recording every
#   file, and a clean exit.
#
# Proves the drive-handling logic on the development machine only.
set -euo pipefail

BIN=${1:?usage: smoke_drives.sh <path to pacscorder>}
HERE=$(cd "$(dirname "$0")" && pwd)
T=$(mktemp -d "${TMPDIR:-/tmp}/pacs-drives.XXXXXX")
RAMDEV=""
FULL=""
cleanup() {
  [ -n "$RAMDEV" ] && hdiutil detach "$RAMDEV" -force >/dev/null 2>&1
  [ -n "${PACS_TEST_FULL_DIR:-}" ] && [ -n "$FULL" ] && rm -rf "$FULL"
  rm -rf "$T"
}
trap cleanup EXIT
fail() {
  echo "FAIL: $*" >&2
  for l in "$T"/*.log; do [ -f "$l" ] && tail -15 "$l" >&2; done
  # Keep the evidence when asked: PACS_TEST_LOGDIR=<dir> copies logs and files there.
  if [ -n "${PACS_TEST_LOGDIR:-}" ]; then
    d="$PACS_TEST_LOGDIR/drives-$(date +%Y%m%d-%H%M%S)-$$"; mkdir -p "$d"
    cp -R "$T"/*.log "$T"/a "$T"/blk "$d"/ 2>/dev/null || true
    echo "evidence kept in $d" >&2
  fi
  exit 1
}
COMMON=(--backend dev --width 1280 --height 720 --fps 60 --split-seconds 4)

# --- case A: missing at start, then returns ------------------------------------
touch "$T/blk"                      # a file where B's parent directory should be
"$BIN" "${COMMON[@]}" --duration 17 --rec-dir "$T/a" --rec-dir "$T/blk/b" >"$T/a.log" 2>&1 &
APP=$!
sleep 6
rm "$T/blk"                         # the drive "comes back"
wait "$APP" || fail "case A: pacscorder exited with an error"

grep -q "ALERT: recording drive $T/blk/b" "$T/a.log" || fail "case A: no alert for the missing drive"
grep -q "rejoins from file" "$T/a.log" || fail "case A: the returning drive did not rejoin"
na=$(ls "$T/a"/*.mp4 | wc -l | tr -d ' ')
nb=$(ls "$T/blk/b"/*.mp4 2>/dev/null | wc -l | tr -d ' ')
[ "$na" -ge 4 ] || fail "case A: drive A has only $na files"
[ "$nb" -ge 1 ] && [ "$nb" -lt "$na" ] || fail "case A: drive B has $nb files (A has $na)"
for f in "$T/blk/b"/*.mp4; do
  twin="$T/a/$(basename "$f")"
  [ -f "$twin" ] || fail "case A: $(basename "$f") exists on B but not on A"
  same=$(python3 "$HERE/mp4_same.py" "$f" "$twin") \
    || fail "case A: $(basename "$f") differs between the drives"
  case "$same" in *" 0 of "*) ;; *) echo "NOTE: $(basename "$f"): $same (allowed, OQ-133)" ;; esac
done
caseA="A: drive A $na files, drive B rejoined with $nb matching files"

# --- case B: drive fills up ------------------------------------------------------
caseB="B: skipped (needs PACS_TEST_FULL_DIR or macOS hdiutil)"
if [ -n "${PACS_TEST_FULL_DIR:-}" ]; then
  FULL="$PACS_TEST_FULL_DIR/rec-$$"
elif command -v hdiutil >/dev/null && command -v diskutil >/dev/null; then
  RAMDEV=$(hdiutil attach -nomount ram://16384 | awk '{print $1}')     # 8 MB
  VOL="PACSFULL$$"
  diskutil erasevolume HFS+ "$VOL" "$RAMDEV" >/dev/null || fail "case B: cannot format the RAM disk"
  FULL="/Volumes/$VOL/rec"
fi
if [ -n "$FULL" ]; then
  "$BIN" "${COMMON[@]}" --duration 12 --rec-dir "$T/full_a" --rec-dir "$FULL" \
    >"$T/b.log" 2>&1 || fail "case B: pacscorder exited with an error"
  grep -q "ALERT: recording drive $FULL" "$T/b.log" || fail "case B: no alert for the full drive"
  nfa=$(ls "$T/full_a"/*.mp4 | wc -l | tr -d ' ')
  [ "$nfa" -ge 3 ] || fail "case B: drive A has only $nfa files"
  for f in "$T/full_a"/*.mp4; do
    LC_ALL=C grep -q -a moof "$f" || fail "case B: $f is not a fragmented MP4"
  done
  grep -q "drive $FULL: .* [1-9][0-9]* failure(s)" "$T/b.log" \
    || fail "case B: no failure counted for the full drive"
  caseB="B: drive filled up, alert raised, drive A recorded $nfa files"
fi

echo "PASS (development drive test): $caseA; $caseB"
