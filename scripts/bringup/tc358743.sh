#!/usr/bin/env bash
# PACSCORDER — TC358743 bring-up helper for the CM4 (legacy video-node mode, ADR-010).
#
# STATUS: NOT YET RUN ON PACSCORDER HARDWARE.
# Supports TEST-DRV-002 (EDID), TEST-CAP-001 (DV timings) and a first capture
# check before TEST-CAP-002. Every run writes a log for the test record.
#
# Commands and their sources (docs/REFERENCES.md):
#   EDID:     v4l2-ctl --set-edid pad=<pad>,type=hdmi | file=<file>     [B-24]
#             ('hdmi' is v4l2-ctl's built-in CTA-861 EDID up to 1080p60 [B-24];
#              the product EDID content is an owner decision, OQ-002)
#             no EDID loaded => the source sends nothing (community) [M-50]
#   Timings:  v4l2-ctl --set-dv-bt-timings query                        [C-37], [M-50]
#   Capture:  v4l2-ctl --stream-mmap --stream-count <n>                 [L-46]
#             (prints a running fps figure [L-48])
# NEEDS VERIFICATION (OQ-101): the pad number on the legacy video node, the
# '-d <node>' form for these options, and '--set-fmt-video=pixelformat=UYVY'
# (UYVY per ADR-005, PROPOSED; 6by9's sequence sets pixelformat=UYVY).
#
# Usage: tc358743.sh [-d /dev/videoN] [-e EDID_FILE] [-n FRAMES] [-o LOGDIR] <edid|timings|format|capture|all>
set -euo pipefail

DEV=/dev/video0        # node number UNKNOWN until bring-up (OQ-043)
EDID_FILE=""
FRAMES=600
LOGDIR=./bringup-logs
PAD=0

usage() { sed -n '2,24p' "$0"; }
while getopts "d:e:n:o:h" opt; do
  case "$opt" in
    d) DEV=$OPTARG ;;
    e) EDID_FILE=$OPTARG ;;
    n) FRAMES=$OPTARG ;;
    o) LOGDIR=$OPTARG ;;
    h) usage; exit 0 ;;
    *) usage; exit 2 ;;
  esac
done
shift $((OPTIND - 1))
CMD=${1:-}
[ -n "$CMD" ] || { usage; exit 2; }

command -v v4l2-ctl >/dev/null || { echo "v4l2-ctl not found (package v4l-utils)" >&2; exit 3; }
[ -e "$DEV" ] || { echo "$DEV does not exist" >&2; exit 3; }

mkdir -p "$LOGDIR"
LOG="$LOGDIR/tc358743-$(date +%Y%m%d-%H%M%S)-$CMD.log"
run() { echo "+ $*" | tee -a "$LOG"; "$@" 2>&1 | tee -a "$LOG"; return "${PIPESTATUS[0]}"; }

{ echo "PACSCORDER TC358743 bring-up, $(date -Iseconds), device $DEV"; uname -a; } >>"$LOG"

do_edid() {
  if [ -n "$EDID_FILE" ]; then
    run v4l2-ctl -d "$DEV" --set-edid "pad=$PAD,file=$EDID_FILE"
  else
    run v4l2-ctl -d "$DEV" --set-edid "pad=$PAD,type=hdmi"
  fi
}
do_timings() { run v4l2-ctl -d "$DEV" --set-dv-bt-timings query; }
do_format()  { run v4l2-ctl -d "$DEV" --set-fmt-video=pixelformat=UYVY; }
do_capture() { run v4l2-ctl -d "$DEV" --stream-mmap --stream-count "$FRAMES"; }

case "$CMD" in
  edid)    do_edid ;;
  timings) do_timings ;;
  format)  do_format ;;
  capture) do_capture ;;
  all)
    do_edid
    echo "waiting 2 s for the source to react to the EDID (hot-plug)" | tee -a "$LOG"
    sleep 2
    do_timings
    do_format
    do_capture
    ;;
  *) usage; exit 2 ;;
esac
echo "log: $LOG"
