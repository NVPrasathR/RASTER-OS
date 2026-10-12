#!/usr/bin/env bash
# PACSCORDER — TEST-ENC-001 Stage 0: encoder-only capacity test on the CM4 (bcm2835-codec)
#
# STATUS: NOT YET RUN ON PACSCORDER HARDWARE. Written 2026-10-12 from the source register
# (docs/REFERENCES.md); fact IDs in [brackets]. Procedure and interpretation: docs/TESTING.md,
# TEST-ENC-001 "Stage 0". This script prints measurements, never PASS/FAIL: pass thresholds
# are UNDEFINED (OQ-017).
#
# Question it answers (OQ-115, RISK-002): how many macroblocks per second does the BCM2711
# hardware H.264 encoder sustain with one and with two concurrent encode sessions, at the
# owner's encode settings (OQ-005)? It needs only a CM4 on its IO Board running Raspberry Pi
# OS — no TC358743. Input is a synthetic videotestsrc pattern [L-38], [L-39].
#
# It does NOT measure: real HDMI capture, DMABUF import, audio, latency per frame, or
# synthetic-vs-real content effects. Those stay in TEST-ENC-001 proper.
#
# Modes:
#   throughput (default) — un-paced: videotestsrc is-live=false, fakesink/filesink without
#       sync, so each session runs as fast as the encoder allows [L-38], [L-40]. Reports
#       achieved fps = frames / wall time, and achieved bitrate from the output file size.
#   realtime — paced: is-live=true, fpsdisplaysink with sync=true and video-sink=fakesink,
#       text-overlay=false; reads "rendered/dropped/average" from last-message via
#       gst-launch-1.0 -v [L-41], [L-42]. Using fpsdisplaysink after an encoder (H.264 caps)
#       is NEEDS VERIFICATION (OQ-101): the register attests its properties, not this use.
#
# Usage: encoder_capacity_test.sh [--mode throughput|realtime] [--frames N] [--seconds S]
#            [--patterns "snow smpte"] [--format I420|NV12|UYVY] [--runs "R1 R3"]
#            [--outdir DIR] [--keep]

set -u
MODE=throughput
FRAMES=1800                 # throughput mode: frames per session
SECONDS_RT=60               # realtime mode: seconds per run
PATTERNS="snow smpte"       # snow: high-entropy worst case; smpte: default pattern [L-38]
FORMAT=I420                 # encoder input format; UYVY = TC358743 format [D-17], [D-18]
RUNS_SEL=""
OUTBASE=/dev/shm/pacscorder-enc-test
KEEP=0
DEV=/dev/video11            # bcm2835-codec encode node [D-06], [D-07]

usage() { sed -n '2,29p' "$0"; }
while [ $# -gt 0 ]; do
  case "$1" in
    --mode) MODE=$2; shift 2;;
    --frames) FRAMES=$2; shift 2;;
    --seconds) SECONDS_RT=$2; shift 2;;
    --patterns) PATTERNS=$2; shift 2;;
    --format) FORMAT=$2; shift 2;;
    --runs) RUNS_SEL=$2; shift 2;;
    --outdir) OUTBASE=$2; shift 2;;
    --keep) KEEP=1; shift;;
    -h|--help) usage; exit 0;;
    *) echo "unknown option: $1" >&2; usage; exit 2;;
  esac
done
case "$MODE" in throughput|realtime) ;; *) echo "bad --mode" >&2; exit 2;; esac

# Run matrix. Session = ROLE:WIDTHxHEIGHT@FPS.
#   REC  = recording encode (OQ-005): High profile, Level 4.2, VBR 25 Mbit/s, no B-frames
#          (the encoder never emits B-frames [D-14], [K-30]).
#   LIVE = live encode (OQ-005): CBR 17 Mbit/s; Constrained Baseline (TESTING.md live-encode
#          settings; [F-36]); level from the mode; SPS/PPS repeated [D-15], [D-37].
# Macroblock rates: 1080p30 = 244,800, 1080p60 = 489,600 MB/s [L-14]; the official
# specification is 1080p30 encode [D-10], [L-29].
RUNS="
R1 REC:1920x1080@30
R2 REC:1920x1080@60
R3 REC:1920x1080@30 LIVE:1920x1080@30
R4 REC:1920x1080@60 LIVE:1920x1080@30
R5 REC:1920x1080@60 LIVE:1920x1080@60
R6 REC:1920x1080@60 LIVE:1280x720@60
R7 REC:1920x1080@60 LIVE:1280x720@30
"

need() { command -v "$1" >/dev/null 2>&1 || { echo "missing command: $1" >&2; exit 3; }; }
need gst-launch-1.0; need v4l2-ctl; need vcgencmd; need awk; need date; need timeout

STAMP=$(date +%Y%m%d-%H%M%S)
OUT="$OUTBASE/$STAMP"
mkdir -p "$OUT" || exit 3
LOG="$OUT/run.log"
CSV="$OUT/results.csv"
say() { echo "$*" | tee -a "$LOG"; }

now() { date +%s.%N; }
mbs_per_frame() { awk -v w="$1" -v h="$2" 'BEGIN{printf "%d", int((w+15)/16)*int((h+15)/16)}'; }  # [L-09]
soc() {  # one line of SoC state: h264 clock [L-49], temperature [L-50], throttling bits [L-51]
  echo "h264_clock=$(vcgencmd measure_clock h264 | cut -d= -f2) temp=$(vcgencmd measure_temp | cut -d= -f2) throttled=$(vcgencmd get_throttled | cut -d= -f2)"
}

# ---------------------------------------------------------------- pre-flight
say "PACSCORDER TEST-ENC-001 Stage 0 — $STAMP — mode=$MODE format=$FORMAT"
say "NOT YET RUN ON PACSCORDER HARDWARE until this log is recorded in docs/TESTING.md"
{ uname -a; cat /etc/os-release 2>/dev/null; gst-launch-1.0 --version; v4l2-ctl --version 2>/dev/null; vcgencmd version; } >"$OUT/environment.txt" 2>&1
[ -e "$DEV" ] || { say "ERROR: $DEV not present (bcm2835-codec encode node [D-06])"; exit 4; }
# config.txt clock and memory settings [L-36], [L-37], [D-48]
for f in /boot/firmware/config.txt /boot/config.txt; do
  [ -r "$f" ] && { echo "== $f"; grep -E '^[[:space:]]*(gpu_freq|h264_freq|core_freq|gpu_mem|over_voltage|arm_freq|dtoverlay)' "$f"; }
done >"$OUT/config_txt.txt" 2>&1
if grep -qE '^[[:space:]]*gpu_mem[[:space:]]*=[[:space:]]*16\b' "$OUT/config_txt.txt"; then
  say "ERROR: gpu_mem=16 selects the cut-down firmware, which removes codec support [D-48]"; exit 4
fi
# Encoder controls: a misspelt extra-controls field is silently skipped [L-55], so check
# every name used below against the device's control list [L-56], [L-63].
v4l2-ctl -d "$DEV" -L >"$OUT/controls.txt" 2>&1
for c in video_bitrate_mode video_bitrate h264_i_frame_period repeat_sequence_header; do
  grep -qE "^[[:space:]]*$c[[:space:]]" "$OUT/controls.txt" || { say "ERROR: control '$c' not listed by v4l2-ctl -L; see $OUT/controls.txt"; exit 4; }
done
# Input formats come from the firmware at probe [D-16], [L-52]; check the one requested.
v4l2-ctl -d "$DEV" --list-formats-out >"$OUT/formats_out.txt" 2>&1   # [D-17]
fourcc=$FORMAT; [ "$FORMAT" = I420 ] && fourcc=YU12
grep -q "'$fourcc'" "$OUT/formats_out.txt" || { say "ERROR: input format $FORMAT ($fourcc) not listed; see $OUT/formats_out.txt"; exit 4; }
say "pre-flight OK; SoC: $(soc)"

echo "mode,pattern,run,session,role,width,height,fps_target,profile,level,rate_mode,bitrate_set,frames,elapsed_s,fps_achieved,fps_ratio,bytes,kbps_achieved,mbps_session,rendered,dropped,soc_before,soc_after" >"$CSV"

# ---------------------------------------------------------------- one session
# args: run pattern idx role w h fps ; writes $OUT/<run>-<pattern>-<idx>.{log,h264,res}
session() {
  local run=$1 pat=$2 idx=$3 role=$4 w=$5 h=$6 fps=$7
  local tag="$run-$pat-$idx" mbf prof lvl rmode br rsh live n sink
  mbf=$(mbs_per_frame "$w" "$h")
  if [ "$role" = REC ]; then
    prof=high; lvl=4.2; rmode=0; br=25000000; rsh=0      # VBR=0 [L-60]; 25 Mbit/s is the control's maximum [D-13]
  else
    prof=constrained-baseline; rmode=1; br=17000000; rsh=1   # CBR=1 [L-60]; SPS/PPS repeat [D-15]
    if [ $((mbf*fps)) -le 245760 ]; then lvl=4; else lvl=4.2; fi   # Level 4 = '4' in caps [L-61]; limits [L-03], [L-05]
  fi
  if [ "$MODE" = throughput ]; then live=false; n=$FRAMES
    sink="filesink location=$OUT/$tag.h264 sync=false"   # filesink [J-40]; 'location' NEEDS VERIFICATION (OQ-101);
                                                         # sync=false set explicitly: GstBaseSink defaults to sync=TRUE [L-40]
  else live=true; n=$((SECONDS_RT*fps))
    sink="fpsdisplaysink video-sink=fakesink text-overlay=false"   # [L-41]; after an encoder: NEEDS VERIFICATION
  fi
  # Profile and level in caps override extra-controls [L-61], [L-62]; I-frame period 60 is the default [K-30], [L-57].
  # Guard: a hung session must not block the run. The verifier note on [L-63] records an
  # unverified 2022 community report that CBR mode hung /dev/video11 on a Pi 4. Exit code 124
  # from timeout means the session was stopped — record it as a finding.
  local t0 t1 dur limit
  if [ "$MODE" = throughput ]; then dur=$(( (n + fps - 1) / fps )); else dur=$SECONDS_RT; fi
  limit=$(( 4 * dur + 60 ))
  t0=$(now)
  timeout "$limit" gst-launch-1.0 -e -v \
    videotestsrc is-live=$live num-buffers=$n pattern=$pat ! \
    "video/x-raw,format=$FORMAT,width=$w,height=$h,framerate=$fps/1" ! \
    v4l2h264enc extra-controls="controls,video_bitrate_mode=$rmode,video_bitrate=$br,h264_i_frame_period=60,repeat_sequence_header=$rsh" ! \
    "video/x-h264,profile=(string)$prof,level=(string)$lvl" ! \
    $sink >"$OUT/$tag.log" 2>&1
  local rc=$?
  t1=$(now)
  echo "$role $w $h $fps $prof $lvl $rmode $br $n $t0 $t1 $rc $mbf" >"$OUT/$tag.res"
}

# ---------------------------------------------------------------- matrix
echo "$RUNS" | while read -r run sessions_a sessions_b; do
  [ -z "$run" ] && continue
  if [ -n "$RUNS_SEL" ] && ! echo " $RUNS_SEL " | grep -q " $run "; then continue; fi
  for pat in $PATTERNS; do
    # generator control: videotestsrc alone must outrun the target, or the result is generator-limited
    t0=$(now)
    gst-launch-1.0 -q videotestsrc is-live=false num-buffers=600 pattern=$pat ! \
      "video/x-raw,format=$FORMAT,width=1920,height=1080,framerate=60/1" ! fakesink >/dev/null 2>&1
    t1=$(now)
    gen_fps=$(awk -v a="$t0" -v b="$t1" 'BEGIN{printf "%.1f", 600/(b-a)}')
    say "[$run/$pat] generator alone at 1920x1080: $gen_fps fps (needs comfortably more than the per-session target; else results are generator-limited)"

    before=$(soc)
    sampler_log="$OUT/$run-$pat-soc.log"
    ( while :; do echo "$(date +%s) $(soc)"; sleep 1; done ) >"$sampler_log" 2>&1 &
    sampler=$!
    idx=0; pids=""
    for s in $sessions_a $sessions_b; do
      idx=$((idx+1))
      role=${s%%:*}; rest=${s#*:}; wh=${rest%@*}; fps=${rest#*@}; w=${wh%x*}; h=${wh#*x}
      session "$run" "$pat" "$idx" "$role" "$w" "$h" "$fps" &
      pids="$pids $!"
    done
    for p in $pids; do wait "$p"; done
    kill "$sampler" 2>/dev/null; wait "$sampler" 2>/dev/null
    after=$(soc)

    total_mbps=0
    for i in $(seq 1 $idx); do
      tag="$run-$pat-$i"
      read -r role w h fps prof lvl rmode br n t0 t1 rc mbf <"$OUT/$tag.res"
      elapsed=$(awk -v a="$t0" -v b="$t1" 'BEGIN{printf "%.3f", b-a}')
      bytes=""; kbps=""; rendered=""; dropped=""
      if [ "$MODE" = throughput ]; then
        fps_ach=$(awk -v n="$n" -v e="$elapsed" 'BEGIN{printf "%.2f", n/e}')
        bytes=$(stat -c %s "$OUT/$tag.h264" 2>/dev/null || echo 0)
        kbps=$(awk -v b="$bytes" -v n="$n" -v f="$fps" 'BEGIN{printf "%.0f", b*8/(n/f)/1000}')  # over media duration
        [ "$KEEP" = 1 ] || rm -f "$OUT/$tag.h264"
      else
        lastmsg=$(grep -o 'rendered: [0-9]*, dropped: [0-9]*[^"]*' "$OUT/$tag.log" | tail -1)   # [L-42]
        rendered=$(echo "$lastmsg" | sed -E 's/.*rendered: ([0-9]+).*/\1/')
        dropped=$(echo "$lastmsg" | sed -E 's/.*dropped: ([0-9]+).*/\1/')
        fps_ach=$(echo "$lastmsg" | sed -nE 's/.*average: ([0-9.]+).*/\1/p')
        [ -z "$fps_ach" ] && fps_ach=0
      fi
      ratio=$(awk -v a="$fps_ach" -v t="$fps" 'BEGIN{printf "%.2f", a/t}')
      mbps=$(awk -v a="$fps_ach" -v m="$mbf" 'BEGIN{printf "%.0f", a*m}')
      total_mbps=$(awk -v x="$total_mbps" -v y="$mbps" 'BEGIN{printf "%.0f", x+y}')
      if [ "$rc" = 124 ]; then say "[$run/$pat] session $i ($role) STOPPED BY TIMEOUT (possible encoder hang) — see $OUT/$tag.log"
      elif [ "$rc" != 0 ]; then say "[$run/$pat] session $i ($role) gst-launch exit code $rc — see $OUT/$tag.log"; fi
      echo "$MODE,$pat,$run,$i,$role,$w,$h,$fps,$prof,$lvl,$rmode,$br,$n,$elapsed,$fps_ach,$ratio,$bytes,$kbps,$mbps,$rendered,$dropped,\"$before\",\"$after\"" >>"$CSV"
      meets=no; awk -v r="$ratio" 'BEGIN{exit !(r>=1.0)}' && meets=yes
      say "[$run/$pat] s$i $role ${w}x${h}@$fps $prof L$lvl: achieved ${fps_ach} fps (ratio $ratio, meets target rate: $meets) ${kbps:+bitrate ${kbps} kbit/s }${dropped:+dropped $dropped }MB/s $mbps"
    done
    say "[$run/$pat] combined macroblock rate: $total_mbps MB/s (official 1080p30 spec = 244800 [L-14], [D-10]); SoC before: $before; after: $after"
  done
done

say "Results: $CSV ; logs: $OUT"
say "Record the results in docs/TESTING.md TEST-ENC-001 (Stage 0) with date, CM4 variant, IO Board, cooling, OS image and this log."
