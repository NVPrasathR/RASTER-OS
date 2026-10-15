#!/usr/bin/env bash
# PACSCORDER development smoke test for live publishing (slice C2; not canonical).
#
# Starts a private MediaMTX on localhost (RTSP :18554, RTMP :11935), runs the
# recorder with the dev backend publishing the live encode over RTSP and RTMP,
# and while it runs reads both streams back from MediaMTX with ffprobe:
# each must be H.264 Constrained Baseline at the capture size and about half
# the capture rate. Then checks that the recording was not disturbed.
# A second run publishes to a port where nothing listens: the publisher must
# fail and retry while the recording still completes (ADR-015). The RTMP stream
# key must not appear in the log (only rtmp://host/application/***).
# First, without MediaMTX: an RTMP URL without the stream part must be refused
# at start (exit 2) — GStreamer 1.26's rtmp2sink cannot use it (found by
# tools/linux_check.sh, 2026-10-13).
#
# Needs: mediamtx and ffprobe on PATH (skips with exit 77 otherwise).
# Proves the publishing logic on the development machine only.
set -euo pipefail

BIN=${1:?usage: smoke_publish.sh <path to pacscorder>}

# --- run 0: an RTMP URL without application/stream is refused at start --------
T0=$(mktemp -d "${TMPDIR:-/tmp}/pacs-pub0.XXXXXX")
rc=0
"$BIN" --backend dev --duration 1 --rec-dir "$T0" \
       --rtmp-url rtmp://127.0.0.1:11935/rtmplive >/dev/null 2>&1 || rc=$?
rm -rf "$T0"
[ "$rc" = 2 ] || { echo "FAIL: an RTMP URL without the stream part was not refused (exit $rc)" >&2; exit 1; }

command -v mediamtx >/dev/null && command -v ffprobe >/dev/null \
  || { echo "SKIP: mediamtx or ffprobe not installed"; exit 77; }

T=$(mktemp -d "${TMPDIR:-/tmp}/pacs-pub.XXXXXX")
MTX_PID=""
cleanup() { [ -n "$MTX_PID" ] && kill "$MTX_PID" 2>/dev/null; rm -rf "$T"; }
trap cleanup EXIT
fail() { echo "FAIL: $*" >&2; [ -f "$T/run.log" ] && tail -20 "$T/run.log" >&2; exit 1; }

cat > "$T/mediamtx.yml" <<'YML'
logLevel: warn
rtspAddress: :18554
rtpAddress: :18000
rtcpAddress: :18001
rtmpAddress: :11935
hls: no
webrtc: no
srt: no
paths:
  all_others:
YML
# Run MediaMTX inside the temporary directory: it writes a generated TLS
# certificate and private key (auto.crt, auto.key) into its working directory.
(cd "$T" && exec mediamtx mediamtx.yml) >"$T/mediamtx.log" 2>&1 &
MTX_PID=$!
sleep 1
kill -0 "$MTX_PID" 2>/dev/null || { cat "$T/mediamtx.log" >&2; fail "mediamtx did not start"; }

# --- run 1: publish RTSP + RTMP while recording -------------------------------
"$BIN" --backend dev --width 1280 --height 720 --fps 60 --duration 14 \
       --split-seconds 5 --rec-dir "$T/rec" \
       --rtsp-url rtsp://127.0.0.1:18554/live \
       --rtmp-url rtmp://127.0.0.1:11935/live/rtmp >"$T/run.log" 2>&1 &
APP=$!
sleep 6

probe() {   # $1 = MediaMTX path; prints codec,profile,width,height,avg_frame_rate
  ffprobe -v error -rtsp_transport tcp -select_streams v:0 \
    -show_entries stream=codec_name,profile,width,height,avg_frame_rate \
    -of csv=p=0 "rtsp://127.0.0.1:18554/$1"
}
rtsp_info=$(probe live) || fail "cannot read the RTSP-published stream"
rtmp_info=$(probe live/rtmp) || fail "cannot read the RTMP-published stream"
wait "$APP" || fail "pacscorder exited with an error"

for pair in "RTSP:$rtsp_info" "RTMP:$rtmp_info"; do
  name=${pair%%:*}; info=${pair#*:}
  case "$info" in h264,*) ;; *) fail "$name stream is not H.264: $info" ;; esac
  case "$info" in *"Constrained Baseline"*) ;; *) fail "$name stream is not Constrained Baseline: $info" ;; esac
  case "$info" in *,1280,720,*) ;; *) fail "$name stream is not 1280x720: $info" ;; esac
  rate=${info##*,}
  awk -v r="$rate" 'BEGIN { split(r, a, "/"); f = (a[2] ? a[1] / a[2] : a[1]); exit !(f > 25 && f < 35) }' \
    || fail "$name stream rate is $rate, expected about 30"
done
grep -q 'RTSP publisher started' "$T/run.log" || fail "RTSP publisher did not report start"
grep -q 'RTMP publisher started: rtmp://127.0.0.1:11935/live/\*\*\*' "$T/run.log" \
  || fail "RTMP publisher did not report start with the stream key hidden"
! grep -q 'live/rtmp' "$T/run.log" || fail "the RTMP stream key was printed in the log"
ls "$T/rec"/*.mp4 >/dev/null 2>&1 || fail "no recording files"

# --- run 2: publisher target down; recording must still complete -------------
"$BIN" --backend dev --width 1280 --height 720 --fps 60 --duration 12 \
       --split-seconds 5 --rec-dir "$T/rec2" \
       --rtsp-url rtsp://127.0.0.1:18599/nobody >"$T/run2.log" 2>&1 \
  || { cat "$T/run2.log" >&2; fail "recording stopped because the publisher failed"; }
grep -q 'publisher .* failed' "$T/run2.log" || fail "the failing publisher was not reported"
grep -q 'restart(s)' "$T/run2.log" || fail "no restart count reported"
n2=$(ls "$T/rec2"/*.mp4 2>/dev/null | wc -l | tr -d ' ')
[ "$n2" -ge 2 ] || fail "only $n2 recording files while the publisher was failing"

echo "PASS (development publishing test): RTSP [$rtsp_info], RTMP [$rtmp_info];"
echo "     with the RTSP target down the recording completed ($n2 files) and the publisher retried;"
echo "     an RTMP URL without the stream part was refused at start; the stream key was not logged"
