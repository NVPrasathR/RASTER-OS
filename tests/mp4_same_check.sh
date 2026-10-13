#!/usr/bin/env bash
# PACSCORDER development test for tests/mp4_same.py (not canonical).
#
# Muxes one encoded stream twice, more than a second apart, so the two files
# hold the same frames but different header time stamps (mp4mux writes the
# wall-clock second; see mp4_same.py). Checks that cmp sees them as different,
# that mp4_same.py sees them as the same recording, and that mp4_same.py does
# report a single changed byte in the media data.
#
# Needs: gst-launch-1.0 with x264enc, matroskamux and mp4mux, and python3
# (skips with exit 77 otherwise).
set -euo pipefail

HERE=$(cd "$(dirname "$0")" && pwd)
command -v gst-launch-1.0 >/dev/null && command -v python3 >/dev/null \
  || { echo "SKIP: gst-launch-1.0 or python3 not installed"; exit 77; }
T=$(mktemp -d "${TMPDIR:-/tmp}/pacs-mp4same.XXXXXX")
trap 'rm -rf "$T"' EXIT
fail() { echo "FAIL: $*" >&2; exit 1; }

gst-launch-1.0 -q videotestsrc num-buffers=120 ! video/x-raw,width=320,height=240,framerate=60/1 \
  ! x264enc speed-preset=ultrafast key-int-max=60 ! h264parse ! matroskamux \
  ! filesink location="$T/in.mkv" || fail "cannot encode the test stream"
for n in 1 2; do
  gst-launch-1.0 -q filesrc location="$T/in.mkv" ! matroskademux ! h264parse \
    ! mp4mux fragment-duration=1000 ! filesink location="$T/m$n.mp4" || fail "cannot mux copy $n"
  [ "$n" = 2 ] || sleep 1.2          # the second copy is stamped at least one second later
done

if cmp -s "$T/m1.mp4" "$T/m2.mp4"; then
  fail "the two copies are byte-identical; expected different header times"
fi
same=$(python3 "$HERE/mp4_same.py" "$T/m1.mp4" "$T/m2.mp4") \
  || fail "mp4_same.py reports a difference beyond the header times"

# Change one byte inside the last media data box: that must be reported.
python3 -c 'import sys
p = sys.argv[1]; d = bytearray(open(p, "rb").read())
i = d.rfind(b"mdat") + 20; d[i] ^= 0xFF; open(p, "wb").write(d)' "$T/m2.mp4"
if python3 "$HERE/mp4_same.py" "$T/m1.mp4" "$T/m2.mp4" >/dev/null 2>&1; then
  fail "mp4_same.py did not report a changed media byte"
fi

echo "PASS (development test of mp4_same.py): copies muxed 1.2 s apart differ for cmp;"
echo "     mp4_same.py: $same; a changed media byte is reported"
