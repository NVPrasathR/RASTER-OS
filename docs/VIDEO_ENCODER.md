# PACSCORDER Video Encoder

| | |
|---|---|
| Document status | DRAFT — design reference built from source research. No encoder code exists. |
| Last updated | 2026-10-09 |
| Applies to | Pi 4 Model B, CM4, Pi 5, CM5 — the "Encoder" stage of REQ-ARCH-001; REQ-ENC-001 (H.264 only, for all outputs: owner 2026-10-08, "H.264 only for now", OQ-103). H.265 is deferred as REQ-ENC-002 (DEFERRED; not in current scope); its research is kept in §4A as the evidence for REQ-ENC-002. Two simultaneous H.264 encodes: one recording encode and one live encode shared by RTMP and WebRTC (owner 2026-10-08, "Separate record + live", answer to OQ-005; CM4 concurrency OQ-115, CM5 OQ-059). *(Added 2026-10-09.)* Live encode: under 1 s camera-to-viewer for WebRTC viewers, RTMP best-effort (owner 2026-10-08; REQ-STR-002, REQ-STR-001; OQ-116 ANSWERED; §3.14, §4.3, §7.1; OQ-125 to OQ-127). Recording encode: fragmented MP4 mirrored to NVMe SSD and HDD (ADR-009, ACCEPTED; §7.2; OQ-117, OQ-118). Bitrate and rate control still open (OQ-005). *(Superseded 2026-10-09: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005); §1, §3.6, §7, §7.1, §7.2.)* *(2026-10-09: owner decisions — the < 1 s target is judged at the 95th percentile with LAN-only WebRTC viewers (OQ-008); recordings run until stopped or the disk is full (OQ-006 ANSWERED); one drive full or failed, and file splitting: OQ-129.)* *(Later on 2026-10-09: owner decision — if one mirrored drive fills, is absent or fails during a recording, recording continues on the drive that still works and the operator is alerted; file splitting, the alert method (OQ-091) and whether a returning drive is used again are still open (OQ-129; §7.2).)* *(Superseded 2026-10-09, later still: owner decisions — a recording started with one drive missing runs on the available drive with an operator alert, and recordings are split into a new file every 30 minutes (OQ-129; §7.2). OQ-129 stays OPEN only for whether a returning drive is used again; the alert method is OQ-091.)* |
| Verification | Source research of 2026-10-06, plus research topic H (H.265/HEVC) of 2026-10-08, plus research topics J (recording storage and power loss) and K (live latency) of 2026-10-08, added here 2026-10-09 ([REFERENCES.md](REFERENCES.md)). Nothing in this document has been tested on PACSCORDER hardware; no hardware exists as of 2026-10-08. *(Still none as of 2026-10-09.)* |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 7, 8, 10, 22, 23, 25 |

| Item | Status |
|---|---|
| REQ-ENC-001 (video encoding) implementation | NOT STARTED |
| H.265 (HEVC) software encoding, any platform (§4A; deferred — REQ-ENC-002; not in current scope) | NOT STARTED |
| TEST-ENC-001 — Sustained real-time H.264 encode (H.265 deferred) | BLOCKED — HARDWARE REQUIRED |
| TEST-DMA-001 (DMABUF capture → encoder buffer sharing) | BLOCKED — HARDWARE REQUIRED |
| ADR-004 product platform | OPEN (bring-up evaluates CM4 and CM5 side by side, owner 2026-10-07; decided from measurements) |
| ADR-005 capture pixel format | PROPOSED (UYVY) — not accepted |
| ADR-007 userspace media framework | OPEN |

Fact references such as `[D-10]` point to [REFERENCES.md](REFERENCES.md). Facts of tier `community` are written as reports ("reported by …"). Calculations are marked **reasoning** and name their inputs.

---

## 1. What this document answers

This document answers the Rule 25 question "How is video encoded?" for each candidate platform.

Short answer: the encoder is **different on each platform pair**, and nothing about it is decided yet. *(2026-10-08: the owner has since set the codec — H.264 only, OQ-103 — and the number of encodes — two, recording and live, OQ-005. Bitrate, rate control, latency, platform (ADR-004) and framework (ADR-007) are still open.)* *(Superseded in part 2026-10-09: the owner set the live latency target on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers only, RTMP best-effort (OQ-116 ANSWERED) — and the recording format: fragmented MP4, mirrored (ADR-009). Bitrate, rate control, platform and framework are still open.)* *(Superseded in part 2026-10-09, later: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005). Platform (ADR-004) and framework (ADR-007) are still open.)*

- **Pi 4 Model B / CM4 (BCM2711).** A hardware H.264 encoder inside the VideoCore firmware is exposed as a V4L2 memory-to-memory device by the downstream-only `bcm2835-codec` driver [D-02], [D-05], [D-08]. Its official specification is 1080p30 encode [D-10].
- **Pi 5 / CM5 (BCM2712).** There is no hardware video encoder. H.264 is encoded in software on the Arm CPU [D-31], [G-22].
- **Codec, bitrate, rate control, latency target and number of simultaneous encodes** are UNDEFINED — OWNER DECISION REQUIRED (OQ-005). The platform is OPEN (ADR-004, OQ-011). The framework is OPEN (ADR-007, OQ-015). *(Superseded in part, 2026-10-08: the owner chose the codecs on 2026-10-07 — H.264 **and** H.265 (HEVC) for recording and streaming (REQ-ENC-001). Which output uses which codec is OQ-103. Bitrate, rate control, latency target and the number of simultaneous encodes are still UNDEFINED (OQ-005).)* *(Superseded 2026-10-08, later: the owner answered OQ-103 with "H.264 only for now". Every output — recording, RTMP and WebRTC — uses H.264 (REQ-ENC-001). H.265 is deferred and recorded as REQ-ENC-002 (DEFERRED). Bitrate, rate control, latency target and the number of simultaneous encodes are still UNDEFINED (OQ-005).)* *(Superseded in part, 2026-10-08, two encodes: the number of simultaneous encodes is now set by the owner — see the next bullet. Bitrate, rate control and latency target are still UNDEFINED (OQ-005, OPEN).)* *(Superseded in part 2026-10-09: the latency target is set — under 1 s camera-to-viewer for WebRTC viewers; RTMP best-effort (owner, 2026-10-08; OQ-116 ANSWERED; REQ-STR-002, REQ-STR-001). Bitrate and rate control are still UNDEFINED (OQ-005, OPEN). No capture-to-file latency target for the recording encode has been set (OQ-005).)* *(Superseded 2026-10-09, later: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005); see the bitrate bullet below. OQ-005 stays OPEN for the recording encode's profile, level and B-frames and for a capture-to-file latency target for recordings, if one is required.)* *(Superseded 2026-10-09, latest: owner decisions — recording encode **H.264 High profile, Level 4.2, no B-frames** (uniform on CM4 and CM5) and a recording **capture-to-file latency target of under 1 s glass-to-disk**. **OQ-005 is ANSWERED.** Which drive and statistic the latency is judged against is OQ-130; whether 25 Mbit/s fits Level 4.2 is OQ-073.)*
- **Two simultaneous H.264 encodes** *(added 2026-10-08; owner: "Separate record + live", answer to OQ-005; REQ-ENC-001)*. One capture feeds (1) a **recording** encode and (2) one **live** encode that RTMP and WebRTC share. Per platform:
  - **Pi 4 Model B / CM4:** both encodes would run on the one hardware encoder, a single V4L2 M2M device [D-06], [D-08]. How many encode contexts it runs concurrently is not in the source register: UNKNOWN — HARDWARE TEST REQUIRED, KERNEL SOURCE INSPECTION REQUIRED (OQ-115). Reasoning: two 1080p30 encodes need 2 × 244,800 = 489,600 macroblocks/s, the rate of one 1080p60 encode and about 2.0× the 1080p30 specification [D-10], [D-52] (RISK-002).
  - **Pi 5 / CM5:** both encodes run in software on the CPU [G-22], [D-31]. Reasoning from [G-22]: roughly twice the encode CPU of one encode; the only official figure is for one 1080p30 encode (OQ-059; RISK-003).
  - **Live encode constraints (reasoning, as in REQ-ENC-001):** it must be receivable by WebRTC browsers. RFC 7742 requires browsers to support Constrained Baseline [F-36]; libwebrtc assumes Constrained Baseline Level 3.1 when no level is signalled [F-39]; 1080p needs Level 4 or above [F-40]; MediaMTX reports that browsers do not accept B-frames in WebRTC [F-45] (community); SPS/PPS must be sent in-band [F-36]. The Pi 4/CM4 encoder produces no B-frames [D-14], offers Constrained Baseline [D-11] and can repeat SPS/PPS [D-15]. Settings: §7.
  - Audio is unchanged: still two audio encodes, AAC (recording and RTMP) and Opus (WebRTC) [F-31], [F-41] (OQ-063); outside this document.
- **H.265 (HEVC), all four platforms** *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)*. No candidate platform has a hardware HEVC encoder [D-24], [D-31]. Reasoning: H.265 is therefore encoded in software on the Arm CPU on Pi 4 Model B, CM4, Pi 5 and CM5, including the boards that have a hardware H.264 encoder. The software encoder is x265 (GStreamer `x265enc` or FFmpeg `libx265`) [H-09], [H-11]. No source gives a PACSCORDER-relevant cost figure. See §4A and RISK-022.
- **Bring-up platforms** *(added 2026-10-08)*. The owner decided on 2026-10-07 that bring-up evaluates CM4 and CM5 side by side. ADR-004 stays OPEN until the TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 results exist.
- **Latency and recording decisions of 2026-10-08** *(added 2026-10-09; research topics J and K)*.
  - **Live encode:** it serves a < 1 s camera-to-viewer target for WebRTC viewers; RTMP shares it on a best-effort basis (OQ-116 ANSWERED). Encoder-side latency facts: §3.14 (Pi 4/CM4), §4.3 (Pi 5/CM5). Settings: §7.1 (keyframes, B-frames, frames held back; OQ-127). Budget: [PERFORMANCE.md](PERFORMANCE.md) §7 (OQ-125, RISK-031). *(2026-10-09: owner decisions — the target is judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running); WebRTC viewers are on the LAN only (OQ-008; internet viewers not in current scope, OQ-128, RISK-033).)*
  - **Recording encode:** written as fragmented MP4 to an NVMe SSD and a USB-to-SATA HDD at once (ADR-009, ACCEPTED). Reasoning: its keyframe interval bounds the fragment and file-split granularity (§7.2; OQ-118). *(Verifier pass, 2026-10-09: that holds for FFmpeg `+frag_keyframe` [J-45] and `splitmuxsink` [J-44]; whether GStreamer `mp4mux` fragments start at keyframes is not in the register — NEEDS VERIFICATION, §7.2.)* The HDD branch must not back-pressure the encoders (OQ-117, RISK-028). *(2026-10-09: recordings have no duration limit — they run until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED). Whether long recordings are split into several files, and what happens when one mirrored drive fills, is absent or fails first: OQ-129.)* *(Later on 2026-10-09: the owner decided the second part — if one mirrored drive fills, is absent or fails during a recording, recording continues on the drive that still works and the operator is alerted (OQ-129, still OPEN for file splitting, the alert method (OQ-091) and drive return; §7.2).)* *(Superseded in part later on 2026-10-09: the owner decided the first part too — recordings are split into a new file every 30 minutes — and that a recording started with one drive missing runs on the available drive with an operator alert (OQ-129; §7.2). Still open: drive return (OQ-129); alert method (OQ-091).)*
- **Bitrate and rate control — owner decisions of 2026-10-09** *(added 2026-10-09, later; OQ-005)*.
  - **Live encode** (shared by RTMP and WebRTC): **CBR at 17 Mbit/s** ("CBR 17 Mbit/s"). **Recording encode:** **25 Mbit/s, VBR** ("25 Mbit/s"; VBR, the encoder default, was stated in the question). The options were based on the CM4 encoder's range of 25 kbit/s to 25 Mbit/s, default 10 Mbit/s, VBR or CBR [D-13], and on YouTube's H.264 1080p60 recommendation of 17 Mbit/s (minimum 6) with CBR [H-29], [K-17]. YouTube is one example destination; the RTMP destinations are still OQ-007.
  - The values were given without a per-mode breakdown. Whether they apply unchanged to every input mode (for example 1080p30, 720p) is not stated (OQ-005).
  - **OQ-005 stays OPEN** for the recording encode's profile, level and B-frames and for a capture-to-file latency target for recordings, if one is required. *(Superseded 2026-10-09, later: both are now decided — see the next block. **OQ-005 is ANSWERED.**)*
- **Recording encode profile, level, B-frames and latency — owner decisions of 2026-10-09 (later)** *(added 2026-10-09, later; OQ-005)*.
  - The recording encode is **H.264 High profile, Level 4.2, with no B-frames**, the same settings on CM4 and CM5 ("High profile, Level 4.2, no B-frames — uniform across CM4 and CM5"). Reasoning: High profile is the CM4 encoder's default and one it offers [K-30], [D-11]; Level 4.2 because 1080p60 exceeds Level 4.0's macroblock rate [F-40] (§6); no B-frames so both platforms produce comparable files — the CM4 hardware encoder emits none [D-14] and CM5's `x264enc` must be forced B-frame-free (`bframes=0`, caps `profile=baseline`, or `tune=zerolatency`) because its medium-preset default is 3 B-frames [K-29] (§7.1). The exact profile/level control values are NEEDS VERIFICATION (OQ-101). Whether 25 Mbit/s fits Level 4.2's maximum bitrate is OQ-073 (§6).
  - The recording path has a **capture-to-file latency target of under 1 s glass-to-disk** ("Require a bounded capture-to-file latency (e.g. < 1 s glass-to-disk)"). Consequence (reasoning): the mirrored HDD branch can stall up to about 3.0 s from standby to ready [J-30], so the bound cannot hold on the HDD branch during a stall unless it is decoupled or buffered (OQ-117, RISK-028; §7.2). Against which drive the bound is judged, and over what statistic and sample count, is OQ-130. Measured in TEST-REC-001.
  - With these, **OQ-005 is fully decided for the current scope** (codec, encodes, bitrate, rate control, live and recording latency, recording profile/level/B-frames). Remaining consequences are tracked as OQ-073, OQ-115, OQ-059, OQ-118 and OQ-130.
  - **Pi 4 Model B / CM4:** both values lie inside the encoder's range, and 25 Mbit/s is its maximum [D-13]. They are set through `V4L2_CID_MPEG_VIDEO_BITRATE` and `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` (§3.6). Whether the range applies to each of two concurrent encode sessions, and whether the encoder sustains both encodes at these rates, is OQ-115.
  - **Pi 5 / CM5:** the `x264enc` bitrate and rate-control property names and units are not in the register: NEEDS VERIFICATION. BUILD TEST REQUIRED. The CPU load of two software encodes at these rates is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059).
  - **Profile and level:** whether 17 and 25 Mbit/s fit the maximum bitrate of the H.264 profile and level each encode signals is not in the register — [F-40] covers frame size and macroblock rate only: NEEDS VERIFICATION. DATASHEET REQUIRED (the H.264 specification; OQ-073; §6).

Position of the encoder in the mandated pipeline (REQ-ARCH-001):

```text
TC358743 → CSI-2 → Unicam (Pi 4/CM4) or RP1 CFE (Pi 5/CM5) → V4L2 capture node
        → DMABUF ─┬→ Encoder 1, recording encode → H.264 bitstream → Recorder
                  └→ Encoder 2, live encode      → H.264 bitstream → RTMP / WebRTC
```

*(2026-10-08, two encodes.)* Until the owner chose "Separate record + live" (OQ-005), the diagram showed one encoder feeding "Recorder / RTMP / WebRTC". It now shows one capture feeding two H.264 encodes (REQ-ENC-001). Reasoning: this is still the "DMABUF → Encoder" stage of REQ-ARCH-001, with two encodes in that stage. On Pi 4/CM4 both encodes would be contexts on the same hardware encoder (OQ-115); on Pi 5/CM5 both are software encodes (OQ-059). How one capture buffer reaches both encoders is in [DMA.md](DMA.md) §9.

The diagram said "H.264 bitstream" until 2026-10-08. H.265 was added after the owner's codec decision (REQ-ENC-001). *(2026-10-08, later: the owner deferred H.265 — "H.264 only for now", OQ-103; REQ-ENC-002 DEFERRED — so the diagram reads "H.264 bitstream" again. In between it read "H.264 and/or H.265 bitstream".)* Reasoning (as in §2 for Pi 5/CM5): a software encoder reads frames from CPU-accessible memory and is not a V4L2 device that imports DMABUFs, so the "DMABUF → Encoder" step applies to software H.264 (and to H.265, if REQ-ENC-002 is re-activated) only as described in [DMA.md](DMA.md) §8 and §8A.

Receivers: Unicam on Pi 4/CM4 [C-09]; RP1 CFE on Pi 5/CM5 [C-29], which is always used in Media Controller mode with this bridge [C-11].

Capture is described in [CSI_PIPELINE.md](CSI_PIPELINE.md) and [V4L2.md](V4L2.md). Buffer sharing is described in [DMA.md](DMA.md). Budgets and measurement plans are in [PERFORMANCE.md](PERFORMANCE.md). Consumers of the bitstream are described in [RECORDING.md](RECORDING.md) and [STREAMING.md](STREAMING.md).

---

## 2. Platform comparison: Pi 4 / CM4 versus Pi 5 / CM5

| Aspect | Pi 4 Model B / CM4 (BCM2711) | Pi 5 / CM5 (BCM2712) |
|---|---|---|
| Hardware H.264 encoder | Yes. `bcm2835-codec`, a V4L2 M2M driver over VCHIQ to the firmware component `ril.video_encode` [D-02], [D-08] | **No** [D-31], [G-22]. The `bcm2835-codec` module is built in `bcm2712_defconfig` [D-04] but cannot probe (reasoning) [D-28], [D-29] |
| Official encode figure | H.264 1080p30 encode [D-10] | "H264 1080p30 encode (from ISP) ~30–40% CPU", in software [G-22], [D-30] |
| 1080p60 encode | Unproven. Needs 2.0× the specified macroblock rate (reasoning) [D-52]. Raspberry Pi engineer 6by9 reported it as an "edge case" on the hardware encoder [D-50] (community). RISK-002, OQ-056 | No official figure. Raspberry Pi engineer 6by9 reported (forum, 2023-10-17) 1080p60 software encode from camera capture as "easily achievable" [D-50]. Not measured for TC358743 input. RISK-003, OQ-059 |
| Two simultaneous H.264 encodes (recording + live) — added 2026-10-08 (owner, OQ-005) | Both would run on the one `bcm2835-codec` encoder, a single M2M device [D-06], [D-08]. Concurrent encode contexts: not in the source register — UNKNOWN (OQ-115). Reasoning: two 1080p30 encodes = 489,600 MB/s = one 1080p60 encode = 2.0× the specification [D-10], [D-52]. RISK-002 | Both in software on the CPU [G-22]. Reasoning from [G-22]: roughly double the encode CPU of one encode; not measured. OQ-059, RISK-003 |
| Frame-size limit | 32×32 to 1920×1920; no 4K encode [D-09] | No hardware block. Raspberry Pi engineer 6by9 reported 4K software encode at "at least 20fps" [D-50] (community) |
| HEVC (H.265) encode | No [D-24] | No; only HEVC *decode* is in hardware [D-30], [D-31] |
| H.265 — added 2026-10-08; deferred — REQ-ENC-002; not in current scope (was "required, REQ-ENC-001" until OQ-103 was answered on 2026-10-08) | Software only, on the Arm CPU (reasoning from [D-24]). Same encoders as Pi 5/CM5: `x265enc` [H-11], [H-12] (CORRECTED), `libx265` [H-08], [H-09]. See §4A | Software only, on the Arm CPU (reasoning from [D-31]). `x265enc` [H-11], [H-12] (CORRECTED), `libx265` [H-08], [H-09]. See §4A |
| CPU core (relevant to x265 SIMD; H.265 deferred — REQ-ENC-002) — added 2026-10-08 | Cortex-A72, defined by GCC 14 as Armv8-A + CRC: no DotProd, I8MM, SVE or SVE2 [H-05] | Cortex-A76, defined by GCC 14 as Armv8.2-A + F16, RCPC, DOTPROD: x265's Neon DotProd kernels can apply; I8MM, SVE and SVE2 cannot [H-04], [H-05] |
| H.265 cost evidence — added 2026-10-08; deferred — REQ-ENC-002; not in current scope | Community benchmark only: `libx265` "Live" 4.33 FPS on a Pi 400 (BCM2711, Cortex-A72 @ 1.80 GHz) [H-21], in a test that is not a 1080p60 live measurement [H-22] (community) | Community only: `libx265` "Live" 10.00 FPS on Pi 5 [H-20], same caveat [H-22]; a Raspberry Pi engineer stated that software H.265 encode "is too intensive an operation to perform at any significant resolution" [H-19] (community) |
| H.264 profiles | Baseline, Constrained Baseline, Main, High (default High) [D-11] | Depends on the software encoder. `openh264enc`: constrained-baseline, baseline, main, constrained-high, high [D-41]. x264 profile options: NEEDS VERIFICATION |
| B-frames | Never produced [D-14]. *(2026-10-09: re-confirmed in `rpi-6.18.y`, min 0 and max 0 [K-30].)* | Encoder setting. `rpicam-apps` uses `max_b_frames=1` in normal mode [D-35]; its low-latency mode drops B-frames [D-32]. *(2026-10-09: GStreamer 1.26 `x264enc` runs x264's medium preset by default — 3 B-frames — unless `tune=zerolatency`, an explicit `bframes=0` or a Baseline profile in caps is set (CORRECTED) [K-29]; §7.1.)* |
| Bitrate range | 25 kbit/s – 25 Mbit/s, VBR or CBR [D-13] *(2026-10-09: owner decisions — live encode CBR 17 Mbit/s, recording encode 25 Mbit/s VBR (OQ-005); both inside this range, and 25 Mbit/s is its maximum (reasoning from [D-13]). Per concurrent session: OQ-115)* | Not researched for x264/openh264 — NEEDS VERIFICATION *(2026-10-09: the same decided values apply; the `x264enc` bitrate and rate-control property names and units are still NEEDS VERIFICATION. CPU load of two software encodes at these rates: OQ-059)* |
| Raw input formats | Read from firmware at probe; the driver table includes UYVY [D-16]. UYVY was in a 2021 listing posted by a Raspberry Pi engineer [D-17] | Planar and semi-planar YUV (plus GRAY8 for `x264enc`): `x264enc` [D-40], FFmpeg `libx264` [D-43], `openh264enc` (I420 only) [D-41]. **Packed UYVY is not accepted** by any of them [D-40], [D-41], [D-43] |
| UYVY from TC358743 | Reported by a Raspberry Pi engineer as accepted directly by the encoder [D-18] | Must be converted per frame before encoding [D-40], [D-43]. Offload path UNKNOWN (OQ-060) |
| Hardware format converter | `/dev/video12` simple ISP M2M device [D-25], [D-07] | No `bcm2835-codec` ISP, because `bcm2835-codec` cannot probe (reasoning) [D-29]. PiSP back end exists [D-51], [G-17]; standalone use UNKNOWN (OQ-060) |
| DMABUF into encoder | Yes, both queues, `videobuf2-dma-contig`, single plane [D-19], [D-20], [D-21] | No V4L2 encoder device. A software encoder reads frames from CPU-accessible memory (reasoning from [G-22]); see [DMA.md](DMA.md) |
| Latency | No source figure *(Superseded 2026-10-09: a Raspberry Pi engineer reported about 10 ms for 720p on a Pi 4, with no extra buffers held (community source) [K-33]; reasoning (CORRECTED): about 23 ms at 1080p if it scales with macroblocks, for one stream on the encoder [K-45]. `v4l2h264enc` reports 0 latency to GStreamer [K-32]. §3.14.)* | Software encoders "generally output frames with a longer latency than the old hardware encoders" [D-32], [G-22] *(2026-10-09: same statement in [K-39]. With `tune=zerolatency` x264 holds no frames back [K-27], [K-28]; the per-frame encode time on BCM2712 is undocumented [K-45] (OQ-059). §4.3.)* |
| GStreamer element | `v4l2h264enc` [D-37], [D-38] | `x264enc` (official replacement) [D-37], [D-40]; `openh264enc` [D-41] |
| FFmpeg encoder | `h264_v4l2m2m` [D-42] | `libx264` (requires `--enable-gpl`) [D-42] |
| Licensing notes | Encoder runs in the proprietary GPU firmware [D-08], [G-69] | x264 is GPL [D-47] (RISK-015) |
| Licensing notes, H.265 — added 2026-10-08; deferred — REQ-ENC-002; not in current scope (but see §8 on `libx265` as an FFmpeg dependency) | x265 is GPL v2 or later, or commercially licensed; neither licence covers HEVC patents [H-39] (§8; OQ-087, OQ-109) | as Pi 4 / CM4 [H-39] |
| CSI-2 lanes feeding the encoder (capture side) | Pi 4 Model B: 2 [C-01]. CM4: CAM0 2, CAM1 4 [C-02] | 4 per port [C-04], [C-05] |

Pi 4 Model B and CM4 share the BCM2711 encoder specification [D-10]. The researched sources show no encoder difference between them; the difference that matters for encoding is the number of capture lanes [C-01], [C-02] (§3.13). Pi 5 and CM5 share BCM2712; neither product brief nor the CM5 datasheet lists an encoder [D-31].

*(Added 2026-10-08.)* For H.265 the two SoC pairs differ in CPU core, not in encoder hardware. Neither has an HEVC encoder [D-24], [D-31]. BCM2711 uses Cortex-A72 and BCM2712 uses Cortex-A76, and only Cortex-A76 has the DotProd extension that x265 can use [H-04], [H-05]. [H-04] and [H-05] name CM4 and CM5. Reasoning: they apply equally to Pi 4 Model B and Pi 5, because [H-05] identifies the cores by SoC. The owner's side-by-side bring-up of CM4 and CM5 (ADR-004) therefore also compares software H.265 on the two cores (OQ-104, OQ-105). *(Superseded 2026-10-08, later: H.265 is deferred — REQ-ENC-002; not in current scope. For encoding, the current bring-up compares CM4 and CM5 on H.264 only; the H.265 comparison applies only if REQ-ENC-002 is re-activated, and OQ-104 and OQ-105 stay OPEN but not in current scope.)*

---

## 3. Pi 4 / CM4 — hardware H.264 encoder (`bcm2835-codec`)

### 3.1 Driver, Kconfig symbol and module

| Item | Value | Source |
|---|---|---|
| Source file | `drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c` in `raspberrypi/linux` `rpi-6.18.y` (the default branch as of 2026-10-06 [D-01]) | [D-02] |
| Kconfig symbol | `VIDEO_CODEC_BCM2835` (tristate "BCM2835 Video codec support") | [D-02], [E-46] |
| Module | `bcm2835-codec.ko` | [D-02], [E-46] |
| Depends on | `MEDIA_SUPPORT && MEDIA_CONTROLLER`, `VIDEO_DEV && (ARCH_BCM2835 \|\| COMPILE_TEST)` | [D-02] |
| Selects | `BCM2835_VCHIQ_MMAL`, `VIDEOBUF2_DMA_CONTIG`, `V4L2_MEM2MEM_DEV` | [D-02] |
| Pulled in by `BCM2835_VCHIQ_MMAL` | `BCM2835_VCHIQ` (VCHIQ core) and `BCM_VC_SM_CMA` (`vc-sm-cma` shared memory) | [D-03], [E-46] |
| Raspberry Pi defconfigs | `bcm2711_defconfig` and `bcm2712_defconfig`: `CONFIG_BCM2835_VCHIQ=y`, `CONFIG_VIDEO_CODEC_BCM2835=m`, `CONFIG_VIDEO_ISP_BCM2835=m` | [D-04], [E-46], [G-18] |
| Upstream status | **Downstream only.** Mainline `drivers/staging/vc04_services/Kconfig` sources only `bcm2835-audio`; `bcm2835-codec/Kconfig` does not exist in mainline | [D-05] |
| Raspberry Pi OS Lite 2026-10-06 | Ships `bcm2835-codec` in the module trees of both kernels (reasoning on confirmed inputs; CORRECTED verdict, the correction concerns per-board differences) | [G-71] |
| Buildroot master Pi 4 / Pi 5 defconfigs (kernel pinned to `raspberrypi/linux` commit `21b41014…`, Linux 6.12.61) | `bcm2835-codec/Kconfig` exists at that commit | [D-49] |

Consequences:

- Reasoning from [D-05]: the encoder driver is available only with a Raspberry Pi kernel tree, not a mainline kernel. This matters for ADR-003 (OS/build basis, ACCEPTED: Raspberry Pi OS with `rpi-image-gen`).
- The encoder work is done by the VideoCore firmware, reached over VCHIQ via the MMAL component `ril.video_encode` [D-08]. Firmware matters (see §3.12).

### 3.2 Device nodes

`bcm2835-codec` registers five M2M video devices. The node numbers are *requested* through module parameters [D-06]:

| Requested node | Role | Module parameter | Official description |
|---|---|---|---|
| `/dev/video10` | H.264 (and other) decode | `decode_video_nr=10` | Video decode [D-07] |
| `/dev/video11` | **H.264 encode** | `encode_video_nr=11` | Video encode [D-07] |
| `/dev/video12` | Simple ISP (convert/scale) | `isp_video_nr=12` | "Simple ISP, can perform conversion and resizing between RGB/YUV formats" [D-07] |
| `/dev/video18` | Deinterlace | `deinterlace_video_nr=18` | [D-27] |
| `/dev/video31` | JPEG encode | `encode_image_nr=31` | [D-06] |

The separate `bcm2835-isp` driver (the "fully programmable ISP", `video13`–`video16` in the official list [D-07]) creates two instances with base node numbers 13 and 20 [D-26].

- The node numbers that actually appear on a PACSCORDER image are UNKNOWN — HARDWARE TEST REQUIRED (OQ-043).
- **PROPOSED (Claude's reasoning; to be decided under ADR-007, which is OPEN):** the application finds the encoder by its card name `bcm2835-codec-encode` [D-08], not by a hard-coded `/dev/video11` path.
- *(Added 2026-10-08, two encodes.)* There is one encode node [D-06]. Reasoning: the recording encode and the live encode (owner, OQ-005) would therefore both use this node, as two encode sessions at the same time. Whether `bcm2835-codec` and the firmware allow two concurrent encode sessions, and at what combined rate, is not in the source register: UNKNOWN — KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED (OQ-115).

### 3.3 Interface: V4L2 stateful memory-to-memory encoder

| Property | Value | Source |
|---|---|---|
| Device capabilities | `V4L2_CAP_VIDEO_M2M_MPLANE \| V4L2_CAP_STREAMING` | [D-08] |
| Media entity function | `MEDIA_ENT_F_PROC_VIDEO_ENCODER` | [D-08] |
| Card name | `bcm2835-codec-encode` | [D-08] |
| Raw input queue | `V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`, I/O modes `VB2_MMAP \| VB2_DMABUF` | [D-19] |
| Bitstream output queue | `CAPTURE_MPLANE`, I/O modes `VB2_MMAP \| VB2_DMABUF` | [D-19] |
| Memory ops | `vb2_dma_contig_memops` | [D-19] |
| Timestamps | `V4L2_BUF_FLAG_TIMESTAMP_COPY`: input timestamps are copied to the encoded buffers | [D-19] |
| Planes | Always one memory plane, also for planar YUV420/NV12 | [D-21] |

In V4L2 M2M terms, the *OUTPUT* queue carries raw frames into the encoder and the *CAPTURE* queue returns the H.264 bitstream [D-19].

### 3.4 Size limits

The decode and encode roles clamp frame size to `MAX_W_CODEC` 1920 × `MAX_H_CODEC` 1920, with a minimum of 32×32. Only the ISP role allows 16384×16384. **4K H.264 hardware encode is not possible on Pi 4/CM4** [D-09]. The TC358743 driver's DV-timings range of 640–1920 × 350–1200 [A-08] lies inside this limit (reasoning).

### 3.5 Performance specification and the 1080p60 gap

| Statement | Source |
|---|---|
| Official BCM2711 multimedia specification: "H.264 (1080p60 decode, 1080p30 encode)" | [D-10] (official-rpi) |
| The driver says "the hardware spec is level 4.0"; higher levels exist to signal correct headers and "may not be able to keep up with real-time" | [D-12] (kernel source) |
| 1080p60 needs 489,600 macroblocks/s: 2.0× the 1080p30 specification and about 1.13× the officially documented 720p120 recipe, for which the documentation suggests `gpu_freq=550` | [D-52] (reasoning) |
| Reported by Raspberry Pi engineer 6by9 (forum, 2023-10-17): 1080p60 "was edge case on the hardware encode" | [D-50] (community) |

**Consequence:** REQ-CAP-001 (1080p60 capture) plus REQ-ENC-001 on CM4 depends on an encode rate the hardware is not specified for. This is RISK-002 and OQ-056. It is retired only by TEST-ENC-001 (sustained 1080p60 encode; the "at least 10 minutes" duration quoted in RISK-002 comes from a research open question, topic D, and the owner has not accepted it, OQ-010, OQ-017). Whether an overclock such as `gpu_freq=550` would be acceptable in the product is OWNER DECISION REQUIRED (OQ-096); the encode measurement with and without it belongs to OQ-056. The macroblock budget is in §6 and in [PERFORMANCE.md](PERFORMANCE.md).

*(Added 2026-10-08, two encodes.)* The owner requires two simultaneous encodes, recording and live (OQ-005; REQ-ENC-001). On CM4 both would run on this encoder, so the gap above now concerns the sum of the two encodes (reasoning). Reasoning: two 1080p30 encodes already need 489,600 macroblocks/s, the same as one 1080p60 encode and 2.0× the specification [D-10], [D-52]. If both encodes run at a 1080p60 capture rate, the sum is 979,200 macroblocks/s, 4.0× the specification (reasoning: 2 × 489,600 [F-40], [D-52]). Further combinations are in [PERFORMANCE.md](PERFORMANCE.md) §4.1. Whether the encoder runs two encodes at once, and at what combined rate, is OQ-115 (RISK-002). If it cannot, a split such as a lower-resolution live encode or one encode in software is an owner choice in OQ-115.

### 3.6 Encoder controls

All controls below are read from the driver source [D-11]–[D-15]. The "PACSCORDER value" column is not decided unless stated.

| Control | Range / values | Driver default | Source | PACSCORDER value |
|---|---|---|---|---|
| `V4L2_CID_MPEG_VIDEO_H264_PROFILE` | Baseline, Constrained Baseline, Main, High | High | [D-11] | UNDEFINED (OQ-005). Constrained Baseline for WebRTC: PROPOSED (§7). *(2026-10-08, two encodes: Constrained Baseline is PROPOSED for the live encode, which RTMP and WebRTC share; the recording encode's profile is UNDEFINED, OQ-005)* *(2026-10-09, later: OQ-005 stays open for a capture-to-file latency target and for the recording encode's profile, level and B-frames; the profile remains UNDEFINED)* |
| `V4L2_CID_MPEG_VIDEO_H264_LEVEL` | 1.0 – 5.1 | 4.0 | [D-12] | Must match the encoded mode (§6) |
| `V4L2_CID_MPEG_VIDEO_BITRATE` | 25,000 – 25,000,000 bit/s, step 25,000 | 10,000,000 | [D-13] | UNDEFINED (OQ-005) *(Superseded 2026-10-09: owner decision — live encode 17,000,000; recording encode 25,000,000 (OQ-005). Reasoning from [D-13]: both are whole multiples of the 25,000 step (17,000,000 / 25,000 = 680; 25,000,000 / 25,000 = 1,000), and 25,000,000 is the maximum. Per concurrent session: OQ-115. Fit with the signalled level: OQ-073)* |
| `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` | VBR, CBR | VBR | [D-13] | UNDEFINED (OQ-005) *(Superseded 2026-10-09: owner decision — live encode CBR; recording encode VBR, the driver default (OQ-005))* |
| `V4L2_CID_MPEG_VIDEO_B_FRAMES` | 0 only (min 0, max 0) | 0 | [D-14], [K-30] (2026-10-09) | — (always 0) |
| `V4L2_CID_MPEG_VIDEO_GOP_SIZE` | 0 – 0x7FFFFFFF | 60 | [D-14], [K-30] (2026-10-09) | UNDEFINED (OQ-005). *(2026-10-09: live encode OQ-127; recording encode OQ-118, because the recording keyframe interval bounds the fragment length — reasoning, §7.2; with FFmpeg `+frag_keyframe`; for GStreamer `mp4mux`, NEEDS VERIFICATION)* |
| `V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER` | 0 / 1 (SPS/PPS inline with every IDR) | 0 (off) | [D-15] | 1 for streaming: PROPOSED (§7). *(2026-10-08: that is the live encode; recording encode UNDEFINED)* |
| `H264_MIN_QP` | 0 – 51 | 20 | [D-15] | UNDEFINED |
| `H264_MAX_QP` | 0 – 51 | 51 | [D-15] | UNDEFINED |
| `FORCE_KEY_FRAME` | Control type and range not recorded in [D-15] — NEEDS VERIFICATION. *(2026-10-09: the driver implements it by setting `MMAL_PARAMETER_VIDEO_REQUEST_I_FRAME`, and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe, so an IDR can be requested on demand, for example when a new viewer joins [K-31]. Type and range are still not in the register.)* | — | [D-15], [K-31] (2026-10-09) | Use is a design decision (ADR-007). *(2026-10-09: on-demand keyframes for WebRTC viewer joins, OQ-127; §7.1)* |
| `HEADER_MODE` | joined with first frame | — | [D-15] | — |

Further facts:

- Every I-frame the encoder produces is an IDR frame [D-14].
- GStreamer `v4l2h264enc` sets these controls through its `extra-controls` property [D-53]. Control names attested in sources for that property are `repeat_sequence_header` [D-37], `h264_profile`, `h264_level` and `video_bitrate` (reported in a Raspberry Pi engineer's forum instructions, community source) [D-54]. The names of the other controls in `extra-controls` form are NEEDS VERIFICATION.
- *(Added 2026-10-08, two encodes.)* Reasoning: with a recording encode and a live encode (OQ-005), every "PACSCORDER value" above is chosen per encode. The live encode carries the WebRTC constraints of §7; the recording encode does not. Whether the driver keeps separate control values for two concurrent encode sessions on `/dev/video11` is not in the source register: KERNEL SOURCE INSPECTION REQUIRED (OQ-115).

### 3.7 Raw input formats

- The accepted input formats are **not hard-coded in the driver**. At probe, the driver asks the firmware (`MMAL_PARAMETER_SUPPORTED_ENCODINGS`) and keeps only the formats that also appear in its static table. That table contains YUV420, YVU420, NV12, NV21, RGB565, YUYV, UYVY, YVYU, VYUY, NV12_COL128, RGB24, BGR24, BGR32, RGBA32, Bayer and grey formats [D-16].
- A Raspberry Pi engineer posted a 2021 listing of `v4l2-ctl --list-formats-out -d 11` that showed 13 formats: YU12, YV12, NV12, NV21, RGBP, RGB3, BGR3, XB24, XR24, YUYV, YVYU, UYVY, VYUY [D-17] (community).
- A Raspberry Pi engineer stated that both TC358743 driver output formats, RGB888 and UYVY, "are supported by the MMAL (and V4L2) video encoder" [D-18] (community).
- `rpicam-apps` feeds the encoder `V4L2_PIX_FMT_YUV420`, not UYVY [D-34].

UNKNOWN — VERIFICATION REQUIRED:

- The format list reported by the firmware that PACSCORDER ships. HARDWARE TEST REQUIRED (OQ-057). The attested command is in §11.
- Whether UYVY input costs encoder throughput compared with YUV420. If it does, the pipeline would become capture → `/dev/video12` ISP → encoder. VENDOR CONFIRMATION REQUIRED and HARDWARE TEST REQUIRED (OQ-057).
- Colour signalling. TC358743 UYVY output is BT.601 limited range, reported as `V4L2_COLORSPACE_SMPTE170M` [B-34]. How the encoder must signal this in the bitstream is HARDWARE TEST REQUIRED and KERNEL SOURCE INSPECTION REQUIRED (OQ-041).

### 3.8 DMABUF input (zero-copy from capture)

Facts:

| Constraint | Detail | Source |
|---|---|---|
| I/O modes | Both queues accept MMAP and DMABUF | [D-19] |
| Contiguity | An imported DMABUF must be one DMA-contiguous region at least as large as the plane; otherwise import fails with "contiguous chunk is too small" | [D-20] |
| Planes | One plane per buffer, so one DMABUF fd holds all planes contiguously | [D-21] |
| Line stride (ENCODE role) | `bytesperline` rounded up to 64 bytes for YUV420/YVU420 and YUYV/UYVY/YVYU/VYUY; 32 bytes for NV12/NV21/RGB24/BGR24 | [D-22] |
| Height alignment | The encoder does not align height to 16; only DECODE and ENCODE_IMAGE do | [D-22] |
| Capture side (Pi 4/CM4) | Unicam uses `videobuf2-dma-contig` and has no IOMMU (reasoning from kernel source; CORRECTED verdict) | [C-53] |

**Reasoning (inputs: 2 bytes per UYVY pixel from the 1920×1080 frame size of 4,147,200 bytes [C-53]; 64-byte alignment [D-22]):** a 1920-pixel UYVY line is 3,840 bytes = 60 × 64, so it is already aligned. A 720-pixel line is 1,440 bytes, which is not a multiple of 64. Whether Unicam's capture stride meets the encoder's alignment for every supported width is UNKNOWN — KERNEL SOURCE INSPECTION REQUIRED, then HARDWARE TEST REQUIRED (OQ-058).

Reference implementations and framework support:

- `rpicam-apps` opens `/dev/video11`, queues camera buffers on the OUTPUT queue with `V4L2_MEMORY_DMABUF`, and reads the bitstream from MMAP CAPTURE buffers [D-34].
- GStreamer V4L2 M2M elements (including `v4l2h264enc`) have `output-io-mode` and `capture-io-mode` properties whose values include `dmabuf-import` [D-53].
- Upstream FFmpeg's V4L2 M2M code allocates only `V4L2_MEMORY_MMAP` buffers, so `h264_v4l2m2m` copies each raw frame [D-44].
- Raspberry Pi OS's patched FFmpeg (7.1.5) adds DMABUF input when the input pixel format is `AV_PIX_FMT_DRM_PRIME` [D-45].
- Buildroot master packages upstream FFmpeg 6.1.5 without Raspberry Pi V4L2/DRM_PRIME patches [D-46] (CORRECTED verdict).

*(Added 2026-10-08, two encodes.)* Candidate (reasoning; not decided, not run): with a recording encode and a live encode (OQ-005), the same Unicam capture buffer would be imported as a DMABUF by both encode sessions, so that neither copies the frame. Whether one DMABUF can be queued on two encode sessions at once, and how many capture buffers are then in flight, is UNKNOWN — KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED (OQ-058, OQ-061, OQ-115). See [DMA.md](DMA.md) §6 and §9.1.

Status: zero-copy capture → encoder on PACSCORDER is NOT STARTED; TEST-DMA-001 is BLOCKED — HARDWARE REQUIRED. See [DMA.md](DMA.md) and REQ-DMA-001.

### 3.9 Encoded (CAPTURE) buffer size

- Default `sizeimage` of an encoded buffer: 768 KiB when width × height > 1280 × 720, otherwise 512 KiB. Clients may request a larger `sizeimage` [D-23].
- The driver notes that some frames of the 1080p "Big Buck Bunny" test sequence exceed 512 KiB [D-23].
- **Reasoning (inputs: maximum bitrate 25,000,000 bit/s [D-13]; 60 frames/s):** the *average* encoded frame at the maximum bitrate is 25,000,000 / 60 / 8 ≈ 52,083 bytes (≈ 51 KiB). The average does not bound individual frames.
- The worst-case IDR frame size at PACSCORDER's bitrate is UNKNOWN — HARDWARE TEST REQUIRED (OQ-056). Whether 768 KiB is enough is part of TEST-ENC-001.
- *(Added 2026-10-08, two encodes.)* Reasoning: the recording encode and the live encode each return their own bitstream, so each needs its own encoded buffers, and the two may run at different bitrates (OQ-005). The worst-case frame size is therefore recorded per encode in TEST-ENC-001. The CMA cost is in [DMA.md](DMA.md) §4.3.
- *(Added 2026-10-09; owner decisions on OQ-005.)* The two encodes do run at different bitrates: recording 25 Mbit/s VBR, live 17 Mbit/s CBR. **Reasoning, same method as above** (inputs: those bitrates; 30 and 60 frames/s; 8 bits per byte): the *average* encoded frame is about 104,167 bytes (≈ 101.7 KiB) for the recording encode at 1080p30 (25,000,000 / 30 / 8), and about 70,833 bytes (≈ 69.2 KiB) at 30 fps or about 35,417 bytes (≈ 34.6 KiB) at 60 fps for the live encode (17,000,000 / 30 / 8 and 17,000,000 / 60 / 8); the recording encode at 60 fps is the ≈ 51 KiB above. Averages do not bound individual frames, and how far a VBR or CBR frame may exceed the average is not in the register. The worst-case frame size per encode is still UNKNOWN — HARDWARE TEST REQUIRED (OQ-056; TEST-ENC-001).

### 3.10 No HEVC encoder

`bcm2835-codec`'s compressed formats are H264, JPEG, MJPEG, MPEG4, H263, MPEG2 and VC1_ANNEX_G. There is no HEVC. The separate Raspberry Pi HEVC driver (`VIDEO_RPI_HEVC_DEC`, module `rpi-hevc-dec`) is a stateless *decoder* only [D-24]. No candidate platform can encode HEVC in hardware [D-24], [D-31].

*(Added 2026-10-08.)* H.265 is still required (owner, 2026-10-07; REQ-ENC-001). Reasoning from [D-24]: on Pi 4/CM4, H.265 must be encoded in software by x265 on the Cortex-A72 cores [H-05]. The hardware encoder serves H.264 only. x265 also needs planar input, not the TC358743's UYVY [H-10] (CORRECTED), [H-13]. So for H.265 the direct-UYVY advantage of §3.7 does not apply, and each frame must be converted first (§4A.3). Treat this as the highest-risk H.265 combination: community evidence puts the Cortex-A72 lowest [H-21] (community), and the core lacks DotProd [H-05] (RISK-022). This is reasoning, not a measurement (OQ-104). *(Superseded 2026-10-08, later: H.265 is no longer required. The owner answered OQ-103 with "H.264 only for now", so H.265 is deferred — REQ-ENC-002; not in current scope — and in the current scope Pi 4/CM4 encodes H.264 only (§3). This paragraph is kept as evidence for REQ-ENC-002.)*

### 3.11 Helper M2M devices on Pi 4 / CM4

| Device | What it is | Relevance to PACSCORDER | Source |
|---|---|---|---|
| `/dev/video12` (`bcm2835-codec` ISP role) | Simple M2M converter/scaler; `MEDIA_ENT_F_PROC_VIDEO_SCALER`; MMAL `ril.isp`; up to 16384×16384; MMAP/DMABUF on both queues | Candidate UYVY → YUV420/NV12 converter between capture and encoder, if direct UYVY encode proves unsuitable (OQ-057) | [D-25], [D-07] |
| `bcm2835-isp` (`video13`… and `video20`…) | Two instances; each has 1 output node, 2 capture nodes and 1 stats node; 64–16384 pixels; formats include UYVY, YUV420, NV12 | Not planned. Listed for completeness | [D-26] |
| `/dev/video18` (deinterlace) | MMAL `ril.image_fx`. It uses the advanced algorithm only when crop width ≤ 800, so 1920-wide 1080i gets the fast algorithm | Reasoning: **not needed for the TC358743 path**, because the TC358743 driver rejects interlaced input with `-ERANGE` [B-27] (RISK-009) | [D-27] |

The GStreamer element `v4l2convert` is registered only by the V4L2 probe [E-33]. Which device it binds to on PACSCORDER (`/dev/video12` or `/dev/video18`) is UNKNOWN — HARDWARE TEST REQUIRED (OQ-057).

### 3.12 Firmware variant and GPU memory caveat

- The Pi 4 **cut-down firmware** (`start4cd.elf` / `fixup4cd.dat`) **removes codec support**. The only way to select it is `gpu_mem=16` [D-48]. Reasoning from [D-48]: a Pi 4/CM4 PACSCORDER image that uses the hardware encoder must therefore not set `gpu_mem=16`.
- Buildroot's `rpi-firmware` package offers `BR2_PACKAGE_RPI_FIRMWARE_VARIANT_PI4` (default), `_PI4_X` ("more audio/video codecs") and `_PI4_CD` (cut-down) [D-48], [E-13].
- Buildroot's sample Pi 4 `config.txt` uses `start4.elf` and `gpu_mem_256/512/1024=100` [E-17].
- Whether the standard `start4.elf` is enough for H.264 encode and the ISP, and which `gpu_mem` and CMA values the codec needs, is UNKNOWN — VENDOR CONFIRMATION REQUIRED and HARDWARE TEST REQUIRED (OQ-048).
- The GPU firmware is proprietary, binary-only and no-modification [G-69] (CORRECTED verdict) — see §8.

### 3.13 Pi 4 Model B versus CM4

The encoder is the same on both boards (BCM2711) [D-10]. The difference is capture:

- Pi 4 Model B has one 2-lane camera connector [C-01]. Over 2 lanes, 1080p60 cannot be captured in either format, and the best UYVY mode is 1080p50 [C-37], [C-48]. A Pi 4 Model B product therefore never presents 1080p60 to the encoder (reasoning).
- Under REQ-CAP-007 (owner, 2026-10-07), Pi 4 Model B and CM4 CAM0 [C-02] remain candidates for the 2-lane configuration, which captures every rate the 2-lane link carries, up to 1080p50 in UYVY [C-37], [C-48] (ADR-004, OPEN). Reasoning: 1080p50 is also above the encoder's official 1080p30 specification [D-10]. Whether captured rates must be encoded at full rate is not specified (OQ-005). Not tested on PACSCORDER hardware.
- CM4 CAM1 has 4 lanes [C-02]. Official documentation states that 4 lanes on a Compute Module can receive 1080p60 in either format [C-37], and the bandwidth calculation agrees [C-49]. A 4-lane port is necessary for 1080p60 but not shown to be sufficient: at the default 972 Mbit/s per lane the driver activates only 3 of the 4 lanes for 1080p60 UYVY [C-47], and capture with 3 of 4 lanes is unproven (OQ-038). The link-frequency choice is ADR-008 (PROPOSED; OQ-099). If 1080p60 capture works there, a CM4 CAM1 product could present 1080p60 to the encoder, which is outside its specification (§3.5) (reasoning). Not tested on PACSCORDER hardware.
- *(Added 2026-10-08.)* Owner, 2026-10-07: bring-up evaluates **CM4 and CM5 side by side**, and ADR-004 stays OPEN until measured. CM4 offers the 2-lane CAM0 and the 4-lane CAM1 on one module [C-02]. Reasoning: CM4 can therefore cover both REQ-CAP-007 configurations in bring-up. For H.264 it uses the hardware encoder (§3) — *(2026-10-08: for both H.264 encodes, recording and live (OQ-005); whether that one encoder runs both is OQ-115)*. For H.265 — deferred (REQ-ENC-002; not in current scope) — it would use software x265 on the CPU (§3.10, §4A). Pi 4 Model B is not in the owner's bring-up pair, but the H.264 sections above still apply to it.

### 3.14 Encode latency (added 2026-10-09)

*(Research topic K, after the owner set < 1 s camera-to-viewer for WebRTC viewers on 2026-10-08; OQ-116 ANSWERED.)* The live encode's latency is part of that budget. Nothing below has been measured on PACSCORDER hardware.

| Item | Fact | Source |
|---|---|---|
| Frames held back | B-frames are limited to min 0, max 0, so the encoder never emits them; defaults are profile High, level 4.0 and a GOP of 60; maximum width and height 1920 | [K-30] |
| Reported latency, community | Raspberry Pi engineer 6by9 stated (raspberrypi/linux issue #7313, April 2026) that the hardware encoder holds no extra buffers because it has no B-frame support, and that its latency depends on the frame's macroblock count: about 10 ms for 720p on a Pi 4, about 40 ms for 1080p on Pi 0–3 (pipelined, so 30 fps is still reached). The issue reporter measured median 9.7 ms, p95 14.3 ms and max 18.5 ms at 1280x720@60 on a Pi 4B. No 1080p figure for BCM2711 is given. | [K-33] (community) |
| Latency reported to GStreamer | GStreamer 1.26 `v4l2videoenc` reports encoder latency as min_buffers × frame duration, and a FIXME comment admits this is not a true latency. `bcm2835-codec` exposes `V4L2_CID_MIN_BUFFERS_FOR_CAPTURE` only on the decoder, so `v4l2h264enc` on BCM2711 reports 0 latency to the pipeline. | [K-32] |
| On-demand keyframe | `V4L2_CID_MPEG_VIDEO_FORCE_KEY_FRAME` sets `MMAL_PARAMETER_VIDEO_REQUEST_I_FRAME`; GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe | [K-31] |

Reasoning (not measurements):

- **1080p estimate** (CORRECTED) [K-45]: scaling the 720p figure by macroblocks (8160 / 3600 = 2.27) gives about 23 ms at 1080p; the reporter's 720p p95 and max scale to about 32 ms and 42 ms. 6by9's statement that latency follows the macroblock count is qualitative, not a measured curve.
- **Two encodes.** That estimate assumes one stream on the encoder. PACSCORDER's recording encode would share it (whether the encoder runs both is OQ-115), so the live encode's latency under contention is unknown (research gap, topic K; OQ-125, OQ-115).
- **Under-reported latency** (research design risk, topic K — not a register fact; RISK-024): because `v4l2h264enc` reports 0 [K-32], GStreamer's pipeline latency and A/V-sync calculations leave out the real encode time, roughly 10–23 ms by [K-33] and [K-45] (OQ-112, OQ-126).
- **Platforms.** [K-30] to [K-32] are kernel and GStreamer source for BCM2711, and [K-33] was measured on a Pi 4B. Reasoning: Pi 4 Model B and CM4 share the BCM2711 encoder specification [D-10], so these facts apply to both. Pi 5 and CM5 have no hardware encoder (§4.1); their encode latency is in §4.3.

Measured per-frame encode latency at 1080p30, alone and with the recording encode running: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-125, OQ-115; TEST-ENC-001). Research names QBUF-to-DQBUF timing, as in issue #7313, as the method (research open question, topic K).

---

## 4. Pi 5 / CM5 — software encoding

### 4.1 No hardware H.264 encoder

| Evidence | Tier | Source |
|---|---|---|
| "Raspberry Pi 5 uses software video encoders" | official-rpi | [G-22], [D-32], [E-52] |
| BCM2712: 4Kp60 HEVC hardware decode; "Other CODECs run in software" | official-rpi | [D-30], [G-22] |
| The Pi 5 product brief, CM5 product brief and CM5 datasheet list only a 4Kp60 HEVC decoder; no hardware video encoder | official-rpi | [D-31] |
| The VCHIQ driver, parent of `bcm2835-codec`, matches only bcm2835/bcm2836/bcm2711 compatibles, and none of the BCM2712 DT files checked (`bcm2712.dtsi`, `bcm2712-ds.dtsi`, `bcm2712-rpi.dtsi`, `bcm2712-rpi-5-b.dts`, `bcm2712-rpi-cm5.dtsi`) contains a vchiq node | kernel-source | [D-28] |
| Reasoning: on Pi 5/CM5, `bcm2835-codec` cannot probe, so no `bcm2835-codec-encode` device exists, even though `CONFIG_VIDEO_CODEC_BCM2835=m` is in `bcm2712_defconfig` | reasoning | [D-29], [D-04], [G-18] |
| The BCM2712 DT defines `hevc_dec` and `pisp_be` behind iommu2. Its comment mentions "(unused) H264 accelerators", but no H.264 encoder node or driver is instantiated | kernel-source | [D-51] |
| Reported by Raspberry Pi engineer jamesh (forum, 2023-10-17): "The 2712 does NOT have a H264 HW block for encoding or decoding" | community | [D-50] |

PACSCORDER treats hardware H.264 encode on Pi 5/CM5 as **unavailable**. What the "(unused) H264 accelerators" comment refers to is UNKNOWN — VENDOR CONFIRMATION REQUIRED (OQ-059). RISK-003 records the impact.

### 4.2 The only official CPU figure

Official BCM2712 figure: **"H264 1080p30 encode (from ISP) ~30–40% CPU"** [G-22], [D-30].

Everything else about Pi 5/CM5 encode cost is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059):

- Whether "~30–40% CPU" means all CPU cores together or one core. VENDOR CONFIRMATION REQUIRED (OQ-059). (The BCM2712 core count and core type are not in the source register.) *(Superseded in part, 2026-10-08: the core type is now in the register, Cortex-A76 [H-05]. An official BCM2712 core count is still not in it.)*
- *(Added 2026-10-08; H.265 deferred — REQ-ENC-002; not in current scope.)* How this figure relates to H.265. Research noted that combining it with the community `libx265`/`libx264` ratio of about 6.6 [H-23] gives opposite feasibility conclusions under the two readings (all cores or one core) (research open question, topic H; OQ-059). Reasoning: no H.265 CPU budget is derived from that combination here. The ratio comes from a different harness (vbench clips, `-threads 1`, a 2022 x265 snapshot) [H-22], and the percentage's meaning is unknown.
- Whether the figure applies to the TC358743 path. Reasoning: that path is not expected to come "from ISP", because Raspberry Pi engineers reported that libcamera does not support the bridge [C-41]. Its UYVY input must also be converted first [D-43].
- Cost at 1080p50 and 1080p60.
- Cost of several simultaneous encodes (recording + RTMP + WebRTC) (OQ-005). *(Superseded in part, 2026-10-08, two encodes: the owner chose two encodes — recording, and one live encode shared by RTMP and WebRTC (OQ-005). On Pi 5/CM5 both are software encodes on the same CPU. Reasoning from [G-22]: roughly double the encode CPU of one encode — about 60–80 % for two 1080p30 encodes, in whatever unit the official figure uses — if the cost scales linearly with the number of encodes, which no source establishes. The cost of two concurrent encodes at 1080p30, 1080p50 and 1080p60 from TC358743 input is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059; RISK-003).)* *(2026-10-09: the bitrates are now set — live 17 Mbit/s CBR, recording 25 Mbit/s VBR (owner, OQ-005). The official figure [G-22] states no bitrate, so the CPU cost of the two software encodes at these rates is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059).)*
- *(Added 2026-10-08, two encodes.)* Whether one UYVY → planar conversion feeds both encodes, or each encode converts separately. Reasoning: one conversion can serve both only if both encodes take the same format and resolution; I420 is accepted by `x264enc`, `libx264` and `openh264enc` [D-40], [D-41], [D-43]. A design point for ADR-007 (OQ-060).

Reported, not measured on PACSCORDER: Raspberry Pi engineer 6by9 wrote that software 1080p60 encode from camera capture "is easily achievable" [D-50] (community). The official `rpicam-vid` documentation says its low-latency mode "will still easily achieve 1080p30" [D-32].

The CPU budget and measurement plan are in [PERFORMANCE.md](PERFORMANCE.md).

### 4.3 Latency

- Official: Raspberry Pi 5's software encoders "generally output frames with a longer latency than the old hardware encoders" [D-32], [G-22]. No figure is given.
- Official: `rpicam-vid --low-latency` reduces latency by no longer using B-frames and arithmetic coding [D-32].
- `rpicam-apps` implements low-latency mode with `libx264` preset `ultrafast`, tune `zerolatency`, slice threading with 4 slices and `refs=1`. Both modes use motion estimation `dia` and `rc-lookahead 0` [D-35].
- Measured encode latency on PACSCORDER: UNKNOWN — HARDWARE TEST REQUIRED (OQ-059). The end-to-end latency target is UNDEFINED — OWNER DECISION REQUIRED (OQ-005, OQ-008). See [PERFORMANCE.md](PERFORMANCE.md). *(Superseded in part 2026-10-09: the owner set the target on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers; RTMP best-effort (OQ-116 ANSWERED; REQ-STR-002, REQ-STR-001). Measured encode latency is still UNKNOWN — HARDWARE TEST REQUIRED (OQ-059, OQ-125). How the target is judged — statistic, number of samples, conditions — is still OWNER DECISION REQUIRED (OQ-008).)* *(Superseded in part 2026-10-09, later: owner decision — judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running), LAN viewers only; sample count and run length still open (OQ-008).)*
- *(Added 2026-10-08, two encodes.)* Reasoning: the glass-to-glass latency target concerns the live encode (RTMP and WebRTC); the recording encode has a capture-to-file target (OQ-005). *(Superseded in part 2026-10-09: the live target applies to WebRTC viewers only; RTMP is best-effort (OQ-116 ANSWERED). No capture-to-file target for the recording encode has been set (OQ-005).)* The MediaMTX project reports that browsers do not accept B-frames in WebRTC [F-45] (community source), so the live encode uses none, as in the low-latency mode above. Whether the recording encode may use other settings, such as B-frames (possible in software: `rpicam-apps`' normal-mode `libx264` uses `max_b_frames=1` [D-35]; the Pi 4/CM4 encoder never produces them [D-14]), is UNDEFINED (OQ-005). How two concurrent software encodes affect the live encode's latency is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059). *(2026-10-09, later: OQ-005 now stays open for the recording encode's profile, level and B-frames and for a capture-to-file latency target — the owner set the bitrates (live 17 Mbit/s CBR, recording 25 Mbit/s VBR) but no capture-to-file target. The recording encode's B-frame setting was not part of that decision and remains UNDEFINED (OQ-005).)* *(Superseded 2026-10-09, latest: owner decisions (OQ-005, now ANSWERED) — the recording encode is **High profile, Level 4.2, no B-frames**, so its B-frame setting is now decided (none), and the recording path has a **capture-to-file latency target of under 1 s glass-to-disk**; which drive and statistic it is judged against is OQ-130.)*

*(Added 2026-10-09; research topic K, after the owner set < 1 s camera-to-viewer for WebRTC viewers, OQ-116 ANSWERED.)*

- **Official.** On Raspberry Pi 5, `rpicam-vid --low-latency` reduces encoding latency at slightly lower coding efficiency, because "B frames and arithmetic coding will no longer be used"; Pi 5 software encoders generally have longer latency than the old hardware encoders. The documentation does not say whether `rpicam-apps` can drive a TC358743 [K-39].
- **x264 `zerolatency` tune.** It sets `rc.i_lookahead=0`, `i_sync_lookahead=0`, `i_bframe=0`, `b_sliced_threads=1`, `b_vfr_input=0` and `rc.b_mb_tree=0`. The CLI help lists the equivalent as `--bframes 0 --force-cfr --no-mbtree --sync-lookahead 0 --sliced-threads --rc-lookahead 0` [K-27] (quoted from x264's help text; NOT YET RUN ON PACSCORDER HARDWARE).
- **Frames held back.** x264 buffers `frames.i_delay` frames before output: the B-frame count (raised to `rc_lookahead` if MB-tree or VBV is on), plus frame threads minus 1, plus `sync_lookahead`, plus `vfr_input`. Frame threads are 1 when sliced threads are on. With `zerolatency` every term is 0, so no frames are held back, and GStreamer `x264enc` reports a latency of 0 × frame duration [K-28].
- **Defaults trap** (CORRECTED) [K-29]. In GStreamer 1.26 `x264enc`, the element's property-default string (`bframes=0`, `rc-lookahead=40`, `threads=0`, `sliced-threads=false` and so on) is applied only when `speed-preset` is None (0) and no tune is set. `speed-preset` defaults to medium (6), so by default `x264enc` runs x264's medium preset — 3 B-frames, rc-lookahead 40, MB-tree on — unless downstream caps force a profile such as baseline. Only properties set explicitly are layered on top. The `bframes=0` property default therefore does not guarantee a B-frame-free stream: set `tune=zerolatency`, set `bframes=0` explicitly, or force `profile=baseline` in caps.
- **Reasoning from [K-28] and [K-45].** The reported latency counts held-back frames only, not the time to encode a frame. That per-frame time on BCM2712 is undocumented, so the CM5 encode term of the live budget is unknown until measured (OQ-059, OQ-125; [PERFORMANCE.md](PERFORMANCE.md) §7).
- **Backlog** (research design risk, topic K — not a register fact; RISK-032): if x264 cannot finish each frame within 33.3 ms at 30p, queues grow by one frame period per queued frame (reasoning part of [K-36]) without bound, so the live branch would need leaky queues, or a lower live resolution or frame rate (OQ-126, OQ-059). Both encodes, recording and live, share the CPU (§4.2).
- **Pi 5 versus CM5.** [K-39] names Raspberry Pi 5; research topic K named CM5. Reasoning (as in §4.7): both use BCM2712 with no hardware encoder [D-31], so these facts apply to both.

Measured per-frame x264 time at 1080p30 with `tune=zerolatency` and a fast preset, while the recording encode also runs: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-059, OQ-125; TEST-ENC-001). Whether the shipped `x264enc` output really has no B-frames with the chosen settings: BUILD TEST REQUIRED (research gap, topic K; OQ-127).

### 4.4 Software encoders and their input formats

| Encoder | Framework / package | Raw input formats accepted | Packed UYVY? | Output | Licence | Source |
|---|---|---|---|---|---|---|
| `x264enc` | GStreamer, plugin `x264`, GStreamer Ugly Plug-ins | Y444, Y42B, I420, YV12, NV12, GRAY8, plus 10-bit variants (Y444_10LE, I422_10LE, I420_10LE) | **No** | H.264; profile options NEEDS VERIFICATION | x264 is GPL [D-47] | [D-40] |
| `libx264` | FFmpeg | 8-bit: YUV420P, YUVJ420P, YUV422P, YUVJ422P, YUV444P, YUVJ444P, NV12, NV16, NV21 | **No** | H.264 | FFmpeg must be built with `--enable-gpl` [D-42] | [D-43] |
| `openh264enc` | GStreamer, plugin `openh264`, GStreamer Bad Plug-ins | I420 only | **No** | constrained-baseline, baseline, main, constrained-high, high; `byte-stream`, `alignment=au` | NOT RESEARCHED (OQ-087) | [D-41] |

`x264enc` property values attested in sources: `speed-preset` value 1 = `ultrafast`; `tune` includes `zerolatency` (0x00000004) [D-40].

**Consequence for ADR-005 (UYVY, PROPOSED).** On Pi 5/CM5, every TC358743 UYVY frame must be converted to a planar or semi-planar format before any of these encoders can use it [D-40], [D-43]. Hardware help for that conversion is not established:

- There is no `bcm2835-codec` ISP on Pi 5/CM5, because `bcm2835-codec` cannot probe there (reasoning) [D-29].
- The PiSP back end is present in the BCM2712 DT [D-51] and built as a module [G-17]. Whether it can run standalone as a UYVY → I420/NV12 converter outside libcamera is UNKNOWN — KERNEL SOURCE INSPECTION REQUIRED, then HARDWARE TEST REQUIRED (OQ-060).
- If no hardware block can do it, the conversion runs on the CPU, and its cost adds to §4.2. That cost is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059, OQ-060).

### 4.5 `rpicam-apps` libx264 settings (reference only)

`rpicam-apps` selects its H.264 encoder per platform as described in [D-33]. It is not one of the ADR-007 framework options, and it is not a capture candidate for PACSCORDER (reasoning): Raspberry Pi engineers reported that libcamera does not support the TC358743 [C-41], and whether `rpicam-apps` can capture from the TC358743 at all is NEEDS VERIFICATION. Its `libx264` settings are the only Raspberry Pi-maintained `libx264` configuration found, so they are recorded here as a reference only [D-35]:

| Setting | Normal mode | `--low-latency` mode |
|---|---|---|
| Preset | `superfast` | `ultrafast` |
| Tune | — | `zerolatency` |
| Threading | frame threading | slice threading, 4 slices |
| B-frames | `max_b_frames=1` | not used [D-32] |
| Reference frames | — | `refs=1` |
| Partitions | `i8x8,i4x4` | — |
| Motion estimation | `dia` | `dia` |
| `rc-lookahead` | 0 | 0 |

Where the table shows "—", [D-35] gives no value.

*(Added 2026-10-09.)* The Raspberry Pi documentation does not say whether `rpicam-apps` can drive a TC358743 [K-39], so whether `--low-latency` applies to PACSCORDER at all is still NEEDS VERIFICATION. BUILD TEST REQUIRED (research open question, topic K). If it does not, its equivalent settings must be made in GStreamer or FFmpeg (§4.3, §7.1).

### 4.6 Packages

- Raspberry Pi OS (trixie): x264 comes from Debian trixie (`2:0.164.3108+git31e19f9-2+b1`); `archive.raspberrypi.com` has no x264 package [G-30]. FFmpeg is the Raspberry Pi build `8:7.1.5-0+deb13u1+rpt2` [G-29]. Whether `x264enc` is packaged and installed on the PACSCORDER image is NEEDS VERIFICATION (BUILD TEST REQUIRED).
- Buildroot: `x264enc` comes from `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264`, which selects `BR2_PACKAGE_X264` [D-47], [E-36]. FFmpeg is built with `--enable-libx264` only when both `BR2_PACKAGE_X264=y` and `BR2_PACKAGE_FFMPEG_GPL=y` [D-46], [E-36].

### 4.7 Pi 5 versus CM5

Both use BCM2712 device trees (`bcm2712-rpi-5-b.dts`, `bcm2712-rpi-cm5.dtsi` [D-28]), and neither lists a hardware encoder [D-31]. Reasoning: the encoder design is therefore the same on both. Differences in cooling and sustained CPU throughput between the two boards are UNKNOWN — HARDWARE TEST REQUIRED (see [PERFORMANCE.md](PERFORMANCE.md), OQ-010, OQ-059).

*(Added 2026-10-08.)* CM5 is the BCM2712 board in the owner's side-by-side bring-up (ADR-004). On CM5, H.264 and H.265 are both software encodes on the same CPU [D-31], [G-22]. An H.264 WebRTC track alongside H.265 would therefore be a second concurrent software video encode (reasoning; OQ-108, OQ-104). Whether CM5 throttles under sustained all-core encode load is UNKNOWN — HARDWARE TEST REQUIRED (OQ-104). *(Superseded in part, 2026-10-08, later: H.265 is deferred — REQ-ENC-002; not in current scope. In the current scope CM5 runs software H.264 only, so the H.265 concurrency point applies only if REQ-ENC-002 is re-activated. CPU, thermal load and concurrency of software H.264 on Pi 5/CM5 remain OQ-059.)* *(2026-10-08, two encodes: the current scope requires CM5 to run two concurrent software H.264 encodes — recording and live (owner, OQ-005) — so the concurrency part of OQ-059 now applies; whether CM5 sustains them is UNKNOWN — HARDWARE TEST REQUIRED; RISK-003.)*

---

## 4A. H.265 (HEVC): software encoding on every platform (deferred — REQ-ENC-002; not in current scope)

*(Section added 2026-10-08 from research topic H. Owner decision of 2026-10-07: H.264 **and** H.265 for recording and streaming (REQ-ENC-001). Nothing in this section has been run on PACSCORDER hardware. Implementation: NOT STARTED.)*

**Scope (2026-10-08, later): deferred — REQ-ENC-002; not in current scope.** The owner answered OQ-103 with "H.264 only for now": every output (recording, RTMP, WebRTC) uses H.264 (REQ-ENC-001), and H.265 is recorded as REQ-ENC-002 (DEFERRED; no tests planned). This section is kept unchanged as the evidence for REQ-ENC-002 and applies only if the owner re-activates it. OQ-104 to OQ-109, RISK-022 and RISK-025 stay OPEN but are not in current scope.

### 4A.1 Why software, and where it runs

- No candidate platform has a hardware HEVC encoder: Pi 4/CM4 [D-24]; Pi 5/CM5 [D-30], [D-31].
- Reasoning: H.265 is encoded by x265 on the Arm CPU on all four platforms. On Pi 4 Model B/CM4 it runs alongside the hardware H.264 encoder (§3). On Pi 5/CM5 it shares the CPU with the software H.264 encoder (§4).
- Which outputs (recording, RTMP, WebRTC) use H.265, in which modes, and whether software-only H.265 is acceptable: OWNER DECISION REQUIRED (OQ-103). Risk: RISK-022 (High). *(Superseded 2026-10-08, later: OQ-103 ANSWERED — no output uses H.265 for now (owner: "H.264 only for now"; REQ-ENC-002 DEFERRED). RISK-022 stays OPEN, not in current scope.)*

Candidate data path (reasoning; not decided, not run):

```text
capture buffer, UYVY (TC358743)                                        [C-53]
   │ CPU mapping
   ▼
UYVY → planar 4:2:0 (I420) or 4:2:2 (Y42B) conversion                 [H-10], [H-13], [H-43]
   (CPU, unless a hardware converter is found: Pi 4/CM4 OQ-057, Pi 5/CM5 OQ-060)
   ▼
x265 4.1 (GStreamer x265enc or FFmpeg libx265), Arm CPU                [H-01], [H-09], [H-11]
   ▼
H.265 byte-stream, alignment=au (x265enc)                              [H-13]
   ▼
recorder (MP4 / Matroska) · RTMP (Enhanced RTMP; FFmpeg only) · SRT · WebRTC     §4A.7
```

### 4A.2 Encoders and packages

| Item | Fact | Source |
|---|---|---|
| x265 library | Debian trixie ships x265 4.1-2 (shared library package `libx265-215`), built for arm64 among other architectures | [H-01] |
| Raspberry Pi override | None. `archive.raspberrypi.com` has no x265 package, so Raspberry Pi OS uses Debian's x265 4.1-2 unchanged | [H-02] |
| Debian build options | Assembly enabled on arm64. 10-bit and 12-bit libraries are linked into the one 8-bit shared library, so `libx265-215` provides Main, Main10 and Main12 | [H-03] |
| FFmpeg `libx265` | The Raspberry Pi FFmpeg source package 8:7.1.5-0+deb13u1+rpt2 is configured with `--enable-libx265` in every flavour [H-08]. Its arm64 `libavcodec61` depends on `libx265-215 (>= 4.1)` and contains the encoder "libx265 H.265 / HEVC" [H-09] | [H-08], [H-09] |
| GStreamer `x265enc` | Plugin `x265` in gst-plugins-bad. Debian trixie's `gstreamer1.0-plugins-bad` 1.26.2-3+deb13u3 (arm64) ships `libgstx265.so` [H-11]. Raspberry Pi overrides gst-plugins-bad1.0 with 1.26.2-3+rpt4+deb13u3, which still build-depends on `libx265-dev` and installs `libgstx265.so` (CORRECTED) [H-12] | [H-11], [H-12] |
| Other HEVC encoders in trixie | kvazaar 2.3.1-2 and the HM reference software 18.0-2, which is not suitable for real-time use. Neither is wired into the distribution's FFmpeg or GStreamer. SVT-HEVC is not packaged, and gst-plugins-bad is built with `-Dsvthevcenc=disabled` (CORRECTED) | [H-18] |
| Newer x265 | 4.2 (19 April 2026) gives "8% faster encoding speed compared to v4.1" from NEON/SVE work; 4.3 (31 July 2026) improves Neon and SVE kernels further. Trixie's 4.1-2 lacks both; only their Neon parts can help Cortex-A72/A76 | [H-07] |

UNKNOWN — VERIFICATION REQUIRED:

- Whether `x265enc` and `libx265` are installed on the PACSCORDER image: NEEDS VERIFICATION (BUILD TEST REQUIRED), as for `x264enc` in §4.6. *(2026-10-08, later — H.265 deferred: `x265enc` is needed only if REQ-ENC-002 is re-activated; its `libgstx265.so` ships inside `gstreamer1.0-plugins-bad` [H-11], [H-12] (CORRECTED), so it is present whenever that package is installed. `libx265-215` is still pulled in whenever the Raspberry Pi FFmpeg's arm64 `libavcodec61` is installed, because that package depends on it [H-09]; it is then present as an FFmpeg dependency, not as a PACSCORDER encoder. Licensing consequences: §8.)*
- Whether Raspberry Pi or Debian will provide an x265 newer than 4.1 for trixie, or PACSCORDER would carry one itself, with maintenance outside the distribution (ADR-003): VENDOR CONFIRMATION REQUIRED (OQ-105).

### 4A.3 Input formats: planar only

| Encoder | Accepted raw input | Packed UYVY? | Semi-planar NV12? | Output | Source |
|---|---|---|---|---|---|
| GStreamer `x265enc` (1.26.2) | Y444, Y42B, I420 at 8-bit; Y444/I422/I420 at 10- and 12-bit LE when the linked library provides those depths (Debian's does [H-03]) | **No** | **No** | `video/x-h265`, `stream-format=byte-stream`, `alignment=au` | [H-13] |
| FFmpeg `libx265` (7.1.5) | Planar (or gray) only. With an 8-bit-only library: yuv420p, yuvj420p, yuv422p, yuvj422p, yuv444p, yuvj444p, gbrp, gray8. With Debian's multi-depth `libx265-215`, the 10- and 12-bit planar variants are added (CORRECTED) | **No** | **No** | H.265 | [H-10] |

Consequences:

- Every TC358743 UYVY frame must be converted to a planar format before x265 can use it, on every platform [H-10] (CORRECTED), [H-13]. Software H.264 on Pi 5/CM5 has the same constraint (§4.4). For H.265 it now applies to Pi 4 Model B/CM4 as well.
- Reasoning [H-43]: on the CPU at 1080p60, the conversion to I420 reads about 249 MB/s and writes about 187 MB/s before x265 starts. Rates for 1080p50 and 1080p30 and the per-frame buffer size are in [DMA.md](DMA.md) §8A. The H.265 CPU budget line is in [PERFORMANCE.md](PERFORMANCE.md) §5.3.
- Reasoning (inputs [D-40], [D-41], [D-43], [H-10], [H-13]): x265 does not accept NV12, which `x264enc` and `libx264` do. I420 (`yuv420p`) is accepted by all five software encoders in this document: `x264enc`, `libx264`, `openh264enc`, `x265enc` and `libx265`. One conversion can therefore feed both a software H.264 and an H.265 encode only if it outputs a planar format such as I420. Whether one conversion is shared is a pipeline design point for ADR-007 (OQ-060).
- Pi 4 Model B/CM4: the `/dev/video12` ISP M2M device is a candidate UYVY → YUV420 converter [D-25], [D-07]. Whether it can feed x265 at the required rate is UNKNOWN — HARDWARE TEST REQUIRED (OQ-057). Pi 5/CM5: hardware offload is UNKNOWN (OQ-060).

### 4A.4 Presets, tune and latency

| Item | Fact | Source |
|---|---|---|
| `x265enc` properties | `speed-preset`: ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow, placebo; default medium. `tune`: psnr, ssim, grain, zerolatency, fastdecode, animation; default ssim. `bitrate` in kbit/s, default 2048, max 102400. `key-int-max`: default 0 = x265 default. `option-string` for raw x265 options | [H-14] |
| x265 4.1 `ultrafast` preset | Max CU 32, min CU 16; 3 B-frames (b-adapt off); lookahead 5; scenecut off; rdLevel 2; 1 reference; DIA motion search; subme 0; SAO, sign hiding, weighted prediction and AQ off | [H-17] |
| x265 4.1 `tune=zerolatency` | B-frames 0, b-adapt off, lookahead depth 0, scenecut and histogram scenecut off, cuTree off, `frameNumThreads=1`. Wavefront (WPP) row parallelism stays on, so frame-level parallelism is disabled and only WPP/pool threading remains | [H-16] |
| `x265enc` reported latency (1.26.2) | Hard-coded 5 frames unless `tune=zerolatency` (then 0); 25 fps assumed when the frame rate is unknown. Latency computed from the encoder parameters arrived only in 1.26.8, and neither Debian nor Raspberry Pi backports it | [H-15] |
| FFmpeg `libx265` threads | The wrapper copies `avctx->thread_count` into x265's `frameNumThreads` after applying preset and tune (CORRECTED) | [H-10] |

Reasoning from these facts (not measurements):

- `ultrafast` alone still uses 3 B-frames and a 5-frame lookahead [H-17]. Adding `tune=zerolatency` removes both [H-16].
- In FFmpeg, the thread count replaces `tune=zerolatency`'s single frame thread unless it is set explicitly ([H-10], [H-16]; same reading as ADR-007). Research names `-threads 1` and `-x265-params frame-threads=1` as the explicit settings (research gap, topic H, not a register fact; BUILD TEST REQUIRED; OQ-104).
- In GStreamer 1.26.2 without `tune=zerolatency`, the 5 frames that `x265enc` reports [H-15] do not come from the real settings. At the frame periods in [PERFORMANCE.md](PERFORMANCE.md) §7, 5 frames are 83.3 ms at 1080p60, 100 ms at 1080p50 and 166.7 ms at 1080p30. A live pipeline would then budget latency with a value that may be wrong.
- Whether browsers accept H.265 B-frames in WebRTC is not in the source register. The MediaMTX report on B-frames covers H.264 only [F-45] (community). NEEDS VERIFICATION (OQ-108).
- Measured H.265 encode latency, frame rate and CPU load on CM4 and CM5: UNKNOWN — HARDWARE TEST REQUIRED (OQ-104).

### 4A.5 Cost evidence: no PACSCORDER-relevant figure exists

| Evidence | Tier | Source |
|---|---|---|
| Research reports that a site search found no raspberrypi.com document or product brief giving an HEVC software-encode figure (absence cannot be proven exhaustively) | community (tier of the register entry) | [H-19] |
| Reported by Raspberry Pi engineer 6by9 on the official forum, 24 October 2024: "Software H265 (HEVC) encode is too intensive an operation to perform at any significant resolution." The original poster reported that `libx265` made the Pi 5 "unresponsive" | community | [H-19] |
| Reported by a community benchmark (OpenBenchmarking, 2023), Pi 5 (Cortex-A76 @ 2.40 GHz): `libx265` Live 10.00 FPS, Upload 1.68, Platform 3.26, Video On Demand 3.27; `libx264` Live 66.17 FPS | community | [H-20] |
| Same test, reported: Raspberry Pi 400 (BCM2711, Cortex-A72 @ 1.80 GHz) 4.33 FPS; Pi 5 9.99 FPS | community | [H-21] |
| The test is not a 1080p60 live measurement. It encodes vbench clips with `-threads 1`. Its Live scenario uses `-preset veryfast -tune zerolatency` in the visible branch. It builds x265 from a 2022-10-28 snapshot, which predates x265 4.0's Arm optimisations | community | [H-22] |
| Reasoning: in that harness on Pi 5, `libx265` was about 6.6 times slower than `libx264` (66.17 / 10.00). This is a relative cost only, not a prediction of PACSCORDER throughput | reasoning | [H-23] |
| x265 4.0 added Arm SIMD that its release notes say gives "up to 57% faster encoding compared to release 3.6" (DotProd, I8MM, SVE2). 4.1 added no new Arm SIMD work | vendor-other | [H-06] |

Reading of the evidence (Claude's reasoning, not a measurement):

- The evidence points against real-time 1080p H.265 in software. It points more strongly against it on Pi 4 Model B/CM4 than on Pi 5/CM5: BCM2711's Cortex-A72 lacks DotProd [H-05], and that core scored lowest in the community test [H-21]. This matches RISK-022.
- The figures do not transfer to PACSCORDER, for four reasons:
  - The harness is not a 1080p60 live encode [H-22].
  - Research asked whether the harness counts frames twice, which would inflate its "Live" figures (research open question, topic H, not a register fact; OQ-104).
  - The benchmark's x265 predates the 4.0 Arm optimisations, while Raspberry Pi OS ships 4.1 [H-22], [H-06], [H-01]. How large any resulting gain is remains unknown.
  - The CM4's clock frequency is not in the source register, so the Pi 400 result at 1.80 GHz [H-21] cannot be carried over to CM4.
- **H.265 throughput, CPU load, temperature and latency on CM4 and CM5: UNKNOWN — HARDWARE TEST REQUIRED (OQ-104).** The H.265 CPU budget line is in [PERFORMANCE.md](PERFORMANCE.md) §5.3.

### 4A.6 SIMD paths: DotProd on CM5 only

| Fact | Source |
|---|---|
| x265 4.1 on Linux/AArch64 detects CPU features at run time by default. It enables the Neon DotProd kernels only when `getauxval(AT_HWCAP)` reports ASIMDDP (bit 20). SVE comes from `AT_HWCAP` bit 22, and I8MM and SVE2 from `AT_HWCAP2`. I8MM and SVE are masked off when DotProd is absent, and SVE2 when SVE is absent | [H-04] |
| GCC 14 defines `cortex-a72` (the BCM2711 core) as Armv8-A + CRC and `cortex-a76` (the BCM2712 core) as Armv8.2-A + F16, RCPC and DOTPROD. So x265's Neon DotProd paths can apply only on CM5; I8MM, SVE and SVE2 paths apply on neither board | [H-05] |
| Debian's x265 4.1-2 build enables assembly on arm64 | [H-03] |

| Platform | DotProd kernels | I8MM / SVE / SVE2 | Status |
|---|---|---|---|
| Pi 4 Model B, CM4 (BCM2711, Cortex-A72) | Cannot apply [H-05] | Cannot apply [H-05] | Which Neon paths x265 uses there: UNKNOWN — BUILD TEST REQUIRED (OQ-105) |
| Pi 5, CM5 (BCM2712, Cortex-A76) | Can apply, if the binary contains them and the kernel reports ASIMDDP [H-04], [H-05] | Cannot apply [H-05] | Whether Debian's binary contains the DotProd kernels, and whether the CM5 kernel reports `asimddp`: UNKNOWN — BUILD TEST REQUIRED, HARDWARE TEST REQUIRED (OQ-105) |

The Pi 4 Model B and Pi 5 rows extend [H-05] by SoC (reasoning; see §2).

### 4A.7 Outputs: what the distribution stacks can carry

Details belong in [RECORDING.md](RECORDING.md) and [STREAMING.md](STREAMING.md). This summary records only what constrains the encoder.

| Output | H.265 facts | Source | Open |
|---|---|---|---|
| Recording | GStreamer 1.26.2 `qtmux`/`mp4mux` and `matroskamux` accept `video/x-h265` (`hvc1` or `hev1`, `alignment=au`). `x265enc` outputs byte-stream, so `h265parse` is needed before them. `matroskamux` warns that `hev1` is not officially supported [H-37]. FFmpeg 7.1.5 tags HEVC in MP4 as `hev1`, `hvc1` or `dvh1`: with `hev1` parameter sets may be in the elementary stream, with `hvc1` they shall not be. Its Matroska muxer handles HEVC [H-38] | [H-37], [H-38] | OQ-006 *(ANSWERED 2026-10-09)* |
| RTMP | Legacy FLV/RTMP has no HEVC; HEVC needs Enhanced RTMP [F-31]. FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP as `hvc1`; its `rtmp_enhanced_codecs` option accepts only `hvc1`, `av01` and `vp09` [H-26]. GStreamer 1.26.2 `flvmux` has no H.265 [H-27], and `eflvmux` first appears in 1.28 (CORRECTED) [F-34]. YouTube Live lists H.265 over RTMP/RTMPS [H-29] | [F-31], [F-34], [H-26], [H-27], [H-29] | OQ-106, OQ-107, RISK-025 |
| SRT / MPEG-TS | GStreamer 1.26.2 `mpegtsmux` accepts H.265 byte-stream. Trixie ships the SRT and MPEG-TS plugins. The Raspberry Pi FFmpeg is built with `--enable-libsrt` | [H-30] | OQ-076 |
| WebRTC | RFC 7742 does not require H.265 [H-32]. RFC 7798 defines the HEVC RTP payload, and `rtph265pay` implements it; profile-id, tier-flag and level-id in its caps arrived only in 1.26.4 [H-31]. Chrome 136+ enables H.265 only with platform hardware support [H-33]. Safari 18.0 added the standard payload [H-34]. No Firefox support was found [H-35]. Edge 147 is reported not to enable it by default (community) [H-36] | [H-31] to [H-36] | OQ-108, RISK-019 |

Reasoning from the WebRTC row ([F-36], [H-32] to [H-36]): an H.264 WebRTC track remains necessary even if WebRTC also carries H.265 (RISK-022). On Pi 5/CM5 that is a second concurrent software video encode (§4.7, §4A.8).

YouTube Live's encoder settings recommend these values [H-29]:

- keyframes every 2 s, and never more than 4 s apart;
- CBR;
- at 1080p60, 4 Mbps minimum and 12 Mbps recommended for H.265, against 6 Mbps and 17 Mbps for H.264.

They are one destination's recommendations, not PACSCORDER parameters (OQ-005, OQ-007). *(2026-10-09: for H.264 the owner has since chosen CBR at 17 Mbit/s for the live encode, an option based on these figures [H-29], [K-17] (OQ-005). YouTube remains one example destination (OQ-007). The H.265 figures stay deferred — REQ-ENC-002.)*

### 4A.8 Concurrency with H.264

- **Pi 4 Model B / CM4.** Reasoning: H.264 can run on the hardware encoder (§3) while H.265 runs on the CPU. The CPU also does the UYVY → planar conversion for x265 (§4A.3) and the audio encodes (REQ-CAP-006; OQ-063). Whether both video encodes sustain the required modes together is UNKNOWN — HARDWARE TEST REQUIRED (OQ-104).
- **Pi 5 / CM5.** Both codecs are software encodes on the same CPU (reasoning from [D-31], [G-22]). Whether H.264, H.265, the conversion and audio all fit is UNKNOWN — HARDWARE TEST REQUIRED (OQ-104, OQ-059).
- **All platforms.** The number of simultaneous encodes is still UNDEFINED (OQ-005). Which outputs use H.265 is OQ-103. *(2026-10-08, later: OQ-103 ANSWERED — none for now; REQ-ENC-002 DEFERRED, so this concurrency case is not in current scope.)* *(2026-10-08, two encodes: the number of encodes is now set — two H.264 encodes, recording and live (OQ-005); see §1. Their concurrency is OQ-115 on CM4 and OQ-059 on CM5.)*

---

## 5. Framework elements and encoder names

The userspace framework is OPEN (ADR-007, OQ-015). The names below are what each option would use.

| Framework | Pi 4 / CM4 (hardware) | Pi 5 / CM5 (software) |
|---|---|---|
| GStreamer | `v4l2h264enc`. Not a static element: it is registered at plugin load by probing M2M devices, with rank `GST_RANK_PRIMARY + 1` and klass `Codec/Encoder/Video/Hardware`. If the name is taken, it registers as `v4l2<basename>h264enc` [D-38], [G-23]. Probing is compiled in only when the meson option `v4l2-probe` is true; upstream defaults to true [D-38], [E-33]. In Buildroot this needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y`; without it, `v4l2h264enc` and `v4l2convert` are not registered [D-39], [E-32]. Outputs `stream-format=byte-stream`, `alignment=au` [F-35] | `x264enc` [D-40]; `openh264enc` [D-41]. Buildroot: `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264` [D-47] |
| FFmpeg | `h264_v4l2m2m` ("V4L2 mem2mem H.264 encoder wrapper"; built when `v4l2_m2m` is enabled) [D-42]. Upstream it uses MMAP only and forces B-frames to 0 [D-44]. Raspberry Pi OS's build adds DMABUF input [D-45] | `libx264`. It is in FFmpeg's `EXTERNAL_LIBRARY_GPL_LIST`, so FFmpeg must be built with `--enable-gpl` [D-42] |
| `rpicam-apps` (reference only, camera stack) | Detects VC4 from a V4L2 card named `bcm2835-isp` and uses its hardware `H264Encoder` [D-33] on `/dev/video11` with DMABUF input [D-34] | Detects PiSP from card `pispbe` and switches to libav with `libav_video_codec = "libx264"` [D-33], [D-35] |
| Direct V4L2 application | Opens the encoder M2M device itself (§3.3) | Links a software encoder library. The library APIs were not researched — NEEDS VERIFICATION |
| **H.265, all platforms** (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | GStreamer `x265enc` [H-11], [H-12] (CORRECTED); FFmpeg `libx265` [H-08], [H-09]; direct application: links libx265 (API not researched — NEEDS VERIFICATION). The same three options apply on Pi 4/CM4, because there is no hardware HEVC encoder [D-24] | as Pi 4 / CM4 [D-31] |

**Official streaming example (fragments only)** [D-37]. The official Raspberry Pi documentation gives a GStreamer pipeline that uses:

- in the base pipeline (the hardware-encoder case, which applies to Pi 4/CM4 — reasoning): `v4l2h264enc extra-controls="controls,repeat_sequence_header=1"` with caps `video/x-h264,level=(string)4`;
- on Raspberry Pi 5: replace that encoder with `x264enc speed-preset=1 threads=1`, and replace the decoder `v4l2h264dec` with `avdec_h264`.

Notes on these fragments (reasoning):

- `level=(string)4` is enough for 1080p30, but not for 1080p50 or 1080p60 (§6, [F-40]).
- `speed-preset=1` is `ultrafast` [D-40]; the exact effect of the `threads` property is not in the source register (NEEDS VERIFICATION). Whether `x264enc speed-preset=1 threads=1` sustains 1080p50 or 1080p60 from TC358743 input is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059).
- *(Added 2026-10-08, two encodes.)* The official example has one encoder. PACSCORDER needs two (recording and live; OQ-005), so whichever framework is chosen runs two encoder instances fed from one capture: two `v4l2h264enc` (or two sessions of a direct V4L2 application) on Pi 4/CM4, and two `x264enc`, `libx264` or `openh264enc` instances on Pi 5/CM5 (reasoning). How each framework hands one capture to two encoders is not in the source register: NEEDS VERIFICATION (ADR-007, OQ-015). Concurrency limits: OQ-115 (CM4), OQ-059 (CM5).

---

## 6. H.264 levels and macroblock rates

The H.264 level limits below are taken from H.264 Table A-1 as encoded in FFmpeg's `h264_levels.c` [F-40]. The ITU-T specification itself was not cited directly (research gap, OQ-073).

| Level | MaxFS (macroblocks per frame) | MaxMBPS (macroblocks per second) | Source |
|---|---|---|---|
| 3.1 | 3,600 | 108,000 | [F-40] |
| 4 | 8,192 | 245,760 | [F-40] |
| 4.2 | 8,704 | 522,240 | [F-40] |

A 1920×1080 frame is coded as 120 × 68 = **8,160 macroblocks**, because 1080 rows are coded as 68 macroblock rows (1088/16) [F-40], [D-36]. That exceeds the Level 3.1 MaxFS [F-40].

| Mode | Macroblocks/s | Minimum level (Table A-1) | Ratio to the Pi 4/CM4 1080p30 encode specification | Source |
|---|---|---|---|---|
| 1080p30 | 244,800 | Level 4 | 1.0× | [F-40], [D-36], [D-52] |
| 1080p50 | 408,000 (reasoning: 8,160 × 50) | Above Level 4 (245,760). Level 4.2 (522,240) suffices. Whether Level 4.1 suffices is not in the source register — NEEDS VERIFICATION | 1.67× (reasoning: 408,000 / 244,800) | reasoning on [F-40] |
| 1080p60 | 489,600 | Level 4.2 | 2.0× | [F-40], [D-36], [D-52] |
| 720p120 (officially documented Pi 4 high-frame-rate recipe, with `--level 4.2` and `gpu_freq=550` suggested) | 432,000 (80 × 45 × 120) | recipe uses 4.2 | 1.76× (reasoning: 432,000 / 244,800) | [D-52] |

Rules that follow from the sources:

- `rpicam-apps` forces level 4.2 when the macroblock rate exceeds 245,760 MB/s, for both the hardware encoder and `libx264` [D-36].
- The Pi 4/CM4 driver accepts levels 1.0–5.1 (default 4.0), but says that its hardware specification is level 4.0 and that higher levels may not keep up with real time [D-12].
- **PROPOSED (Claude's reasoning; parameters are OQ-005):** PACSCORDER sets the signalled level from the actual encoded mode using Table A-1. That means at least Level 4 for 1080p30 and Level 4.2 for 1080p60 [F-40]. It never relies on the encoder default.
- *(Added 2026-10-08, two encodes; reasoning.)* Each of the two encodes (recording and live; OQ-005) signals its own level from its own mode, because the Table A-1 limits apply to one bitstream [F-40]. The level does not describe the encoder's total load. On Pi 4/CM4 the two encodes share one encoder whose hardware specification is level 4.0 [D-12], so their *sum* is compared with the 1080p30 specification (§3.5; [PERFORMANCE.md](PERFORMANCE.md) §4.1; OQ-115). The live encode's level also has to suit WebRTC negotiation (§6.2).
- *(Added 2026-10-09; owner decisions on OQ-005.)* **Level and bitrate.** The bitrates are now set: live encode 17 Mbit/s CBR, recording encode 25 Mbit/s VBR. Reasoning: the signalled profile and level must allow each encode's bitrate as well as its macroblock rate. The maximum bitrate per profile and level in H.264 Table A-1 is not in the register — [F-40] covers frame size and macroblock rate only — so whether 17 and 25 Mbit/s fit the level each encode signals is NEEDS VERIFICATION. DATASHEET REQUIRED (the H.264 specification; OQ-073). No level bitrate limit is stated in this document.
- *(Added 2026-10-08; H.265 deferred — REQ-ENC-002; not in current scope.)* **H.265 levels.** The tables above are H.264 only. H.265 level and tier limits are not in the source register — NEEDS VERIFICATION. A related limitation: GStreamer 1.26.2 `rtph265pay` does not put profile-id, tier-flag or level-id in its output caps; that arrived in 1.26.4 [H-31] (OQ-108).

### 6.1 Pitfall in the published TC358743 example

As reported in Raspberry Pi engineer 6by9's TC358743 install instructions (Pi 0–4, forum, 2020-08-05; community source) [D-54]:

- the example pipeline feeds UYVY from `v4l2src` straight into `v4l2h264enc`, with `extra-controls="controls,h264_profile=4,h264_level=10,video_bitrate=256000;"` and caps `framerate=30/1,format=UYVY`;
- `h264_level=10` is `V4L2_MPEG_VIDEO_H264_LEVEL_3_2` (**Level 3.2**), not level 4.x;
- `h264_profile=4` is **High** [D-54].

Reasoning: Table A-1 says 1080p30 needs at least Level 4 [F-40], so this example under-signals the level for any 1080p input. Its 256 kbit/s bitrate is the example's own value, not a PACSCORDER requirement. *(2026-10-09: PACSCORDER's bitrates are now 17 Mbit/s CBR for the live encode and 25 Mbit/s VBR for the recording encode (owner, OQ-005).)* **PROPOSED (Claude's reasoning): PACSCORDER does not copy these values.** Level follows §6, and profile follows §7 and OQ-005.

### 6.2 WebRTC level negotiation

- `profile-level-id` `42e01f` decodes to Constrained Baseline Level 3.1 (reasoning) [F-38].
- libwebrtc defines `42e01f` as its Constrained Baseline profile-level-id, assumes Constrained Baseline Level 3.1 when `profile-level-id` is absent, and advertises Baseline, Constrained Baseline and Main at Level 3.1 [F-39].
- A strict `42e01f` negotiation does not cover 1080p [F-40].

Whether browsers decode a 1080p stream after a Level 3.1 negotiation, or PACSCORDER must signal Level 4.0 or 4.2, is UNKNOWN — HARDWARE TEST REQUIRED (OQ-073, RISK-019, TEST-STR-002). Details are in [STREAMING.md](STREAMING.md).

*(Added 2026-10-08, two encodes.)* This negotiation concerns the **live** encode, which WebRTC shares with RTMP (owner, OQ-005). Reasoning: the profile and level chosen for WebRTC are therefore also what RTMP carries. The recording encode is not negotiated with a browser.

*(Added 2026-10-09; owner decision on OQ-005.)* The live encode is now CBR at 17 Mbit/s. Whether that bitrate fits the level negotiated with browsers is part of OQ-073 (register note of 2026-10-09): NEEDS VERIFICATION. DATASHEET REQUIRED (the H.264 specification), then TEST-STR-002.

---

## 7. Streaming-relevant encoder settings

The settings below are **PROPOSED**: Claude's reasoning from the cited facts. Parameter values belong to OQ-005, and the framework is ADR-007 (OPEN). None has been implemented or tested. *(Superseded in part 2026-10-09: the owner set the bitrate and rate control — live encode CBR 17 Mbit/s, recording encode 25 Mbit/s VBR (OQ-005); see the bitrate row added to the table below. OQ-005 now stays open for the recording encode's profile, level and B-frames and for a capture-to-file latency target; the other values in the table are not decided by it — keyframes OQ-127 (live) and OQ-118 (recording), level OQ-073.)*

*(Added 2026-10-08, two encodes.)* The owner chose "Separate record + live" (OQ-005; REQ-ENC-001). The streaming settings below therefore apply to the **live** encode, which RTMP and WebRTC share. Reasoning, as recorded in REQ-ENC-001: because browsers receive the live encode over WebRTC, it needs Constrained Baseline [F-36], [F-39], a level of 4 or above at 1080p [F-40], no B-frames (reported by the MediaMTX project [F-45], community source) and in-band SPS/PPS [F-36]. On Pi 4/CM4 the encoder already produces no B-frames [D-14], offers Constrained Baseline [D-11] and can repeat SPS/PPS [D-15]. The **recording** encode is not bound by WebRTC; its profile, GOP, B-frames (software only; never on Pi 4/CM4 [D-14]) and bitrate are UNDEFINED (OQ-005). Whether every RTMP destination accepts a Constrained Baseline stream is not in the source register: NEEDS VERIFICATION (OQ-007). The statuses in the table are unchanged. *(Superseded in part 2026-10-09: the recording encode's bitrate is set — 25 Mbit/s VBR — and the live encode is CBR at 17 Mbit/s (owner, OQ-005). The recording encode's profile, GOP and B-frames were not part of that decision and remain UNDEFINED; OQ-005 keeps them open together with a capture-to-file latency target (recording keyframe interval: OQ-118). Whether every RTMP destination accepts 17 Mbit/s CBR is likewise not in the register beyond YouTube's recommendation [H-29], [K-17]: NEEDS VERIFICATION (OQ-007).)*

| Setting | Why (source) | Pi 4 / CM4 hardware encoder | Pi 5 / CM5 software encoder | Status |
|---|---|---|---|---|
| Repeat SPS/PPS in-band with every IDR | RFC 7742: WebRTC SPS/PPS MUST be sent in-band, and `sprop-parameter-sets` MUST NOT be in SDP [F-36]. The GStreamer streaming example in the official Raspberry Pi documentation enables `repeat_sequence_header=1` [D-37] | `V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER` defaults to **0 (off)**, so it must be set to 1 [D-15] | x264/openh264 option name: NEEDS VERIFICATION | PROPOSED |
| No B-frames | The MediaMTX project reports that browsers deliberately do not support H.264 B-frames in WebRTC [F-45] (community). Official: dropping B-frames lowers latency [D-32]. *(2026-10-09: MediaMTX's documentation states the same and recommends Baseline with Opus, register tier `vendor-other` [K-04]; the register tiers the same kind of MediaMTX source `community` in [F-45], and both are attributed to MediaMTX here (REFERENCES.md, Register notes, 2026-10-09). YouTube recommends 2 B-frames for its RTMP ingest [K-17]; reasoning: the shared live encode cannot follow both — OQ-127, §7.1)* | Never produced [D-14]. Upstream FFmpeg `h264_v4l2m2m` also forces 0 [D-44] | `rpicam-apps` normal mode uses `max_b_frames=1` [D-35]; its low-latency mode drops them [D-32]. `x264enc` property name: NEEDS VERIFICATION *(Superseded 2026-10-09: the property is `bframes`, default 0, but by default it is not applied — set `tune=zerolatency`, set `bframes=0` explicitly, or force `profile=baseline` in caps (CORRECTED) [K-29]; `zerolatency` sets `i_bframe=0` [K-27]. §4.3.)* | PROPOSED (RISK-019) |
| Constrained Baseline profile for the WebRTC output | RFC 7742 requires WebRTC browsers to implement H.264 Constrained Baseline [F-36]. libwebrtc's built-in encoder encodes only Constrained Baseline [F-39] | Available; driver default is High [D-11] | `openh264enc` offers constrained-baseline [D-41]. `x264enc` profile setting: NEEDS VERIFICATION *(Superseded in part 2026-10-09: `x264enc` applies a profile from downstream caps, for example `profile=baseline` [K-29]. Whether caps can request Constrained Baseline specifically: still NEEDS VERIFICATION; BUILD TEST REQUIRED.)* | PROPOSED for WebRTC. Profile for recording/RTMP: UNDEFINED (OQ-005). *(2026-10-08, two encodes: RTMP shares the live encode with WebRTC, so this PROPOSED profile also reaches RTMP; recording-encode profile UNDEFINED, OQ-005)* |
| Signalled level matches the mode | Table A-1 [F-40]; §6 | Level control, default 4.0 [D-12] | `rpicam-apps` rule: 4.2 above 245,760 MB/s [D-36] | PROPOSED |
| Bitrate and rate control (row added 2026-10-09) | Owner decision of 2026-10-09 (OQ-005): live encode CBR 17 Mbit/s; recording encode 25 Mbit/s VBR. The options were based on the CM4 encoder's range and modes [D-13] and on YouTube's H.264 1080p60 recommendation of 17 Mbit/s with CBR [H-29], [K-17]; YouTube is one example destination (OQ-007) | Live: `V4L2_CID_MPEG_VIDEO_BITRATE` 17,000,000 with `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` CBR. Recording: 25,000,000 (the maximum) with VBR (the default) [D-13]. Per concurrent session: OQ-115 | `x264enc` bitrate and rate-control property names and units: not in the register — NEEDS VERIFICATION (BUILD TEST REQUIRED). CPU load at these rates: OQ-059 | Values: owner decision (OQ-005). Setting method PROPOSED; implementation NOT STARTED. Per-mode values not given (OQ-005); fit with the signalled level: OQ-073 |
| Keyframe (IDR) interval and on-demand keyframes | Needed by viewers joining a stream (reasoning). Value UNDEFINED (OQ-005). *(2026-10-09: YouTube recommends a 2 s keyframe frequency, not over 4 s [K-17]; live encode OQ-127, §7.1; recording encode OQ-118, §7.2)* | GOP default 60; `FORCE_KEY_FRAME` exists; every I-frame is IDR [D-14], [D-15]. *(2026-10-09: GOP 60 re-confirmed [K-30]; an IDR can be requested on demand through `v4l2videoenc` [K-31])* | NEEDS VERIFICATION | UNDEFINED |
| H.264 bitstream format for FLV/RTMP | `flvmux` needs `stream-format=avc` [F-34] (CORRECTED verdict); `v4l2h264enc` outputs `byte-stream`, so an `h264parse` (or equivalent) is needed between them [F-35] | applies | applies to any byte-stream encoder (reasoning) | Design input for ADR-007 |
| Colour description | TC358743 UYVY output is BT.601 limited range, `SMPTE170M` [B-34] | How to set the bitstream colour description: NEEDS VERIFICATION | NEEDS VERIFICATION | UNKNOWN (OQ-041) |
| Timestamps and frame rate | Input timestamps are copied to encoded buffers [D-19]. The TC358743 driver reports fractional rates such as 59.94 as integer-rate pixel clocks [B-28] | applies | applies | UNKNOWN (OQ-040) |
| H.265 for live outputs (RTMP, WebRTC): `tune=zerolatency`, with frame threads set explicitly in FFmpeg (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | `tune=zerolatency` removes B-frames and lookahead [H-16], and makes GStreamer 1.26.2 `x265enc` report 0 instead of a hard-coded 5 frames of latency [H-15]. FFmpeg's thread count otherwise replaces its single frame thread [H-10] (§4A.4) | applies (software x265 on the CPU, §3.10) | applies | PROPOSED. Throughput cost of disabling frame-level parallelism [H-16]: UNKNOWN (OQ-104). Whether H.265 is used live at all: OQ-103 *(ANSWERED 2026-10-08: not for now — H.264 only; applies only if REQ-ENC-002 is re-activated)* |
| H.265 parameter sets in-band (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | FFmpeg's `hev1` MP4 form allows parameter sets in the elementary stream; `hvc1` does not [H-38]. Reasoning: viewers joining a live stream need them repeated, as for H.264 above | applies | applies | The x265 option name that repeats them is not in the source register — NEEDS VERIFICATION |

Codec choice context: legacy RTMP/FLV carries H.264 (AVC) as its only modern video codec; HEVC needs Enhanced RTMP [F-31]. No candidate platform encodes HEVC in hardware [D-24], [D-31]. Audio encoding (AAC for RTMP, Opus for WebRTC [F-31], [F-41]) is outside this document; see OQ-063. *(2026-10-08: the two-video-encode decision does not change audio — there are still two audio encodes, AAC for recording and RTMP and Opus for WebRTC; OQ-063.)*

*(Added 2026-10-08.)* The owner has since chosen H.264 **and** H.265 (REQ-ENC-001); per-output use is OQ-103. *(Superseded 2026-10-08, later: the owner answered OQ-103 with "H.264 only for now" — H.264 for every output; H.265 is deferred, REQ-ENC-002.)* The H.265 transport facts are summarised in §4A.7 (deferred — REQ-ENC-002; not in current scope). Audio is required (owner, 2026-10-07: OQ-004 ANSWERED, REQ-CAP-006 DRAFT). Audio encoding stays outside this document, but it shares the CPU with software video encoding (§4A.8; in the current scope, software H.264 on Pi 5/CM5, §4.2 — §4A.8 applies only if REQ-ENC-002 is re-activated). The encoders available in Raspberry Pi OS are:

- FFmpeg: the native `aac` encoder and the `libopus` wrapper; there is no `libfdk_aac` [I-39], [I-40], [I-41].
- GStreamer: `voaacenc`, `avenc_aac` and `opusenc`; `fdkaacenc` is not shipped [I-44], [I-45], [I-46].
- `fdk-aac` itself is non-free and, according to Debian, incompatible with every GPL version [I-43].

Encoder choice and cost remain OQ-063; see [RECORDING.md](RECORDING.md) and [STREAMING.md](STREAMING.md).

### 7.1 Live encode and the < 1 s WebRTC target (added 2026-10-09)

**Owner decision of 2026-10-08 (OQ-116 ANSWERED).** Under 1 s camera-to-viewer for WebRTC viewers only (REQ-STR-002). RTMP outputs are best-effort, with latency set by the receiving platform (REQ-STR-001). Bitrate and rate control are still open (OQ-005, OPEN). *(Superseded 2026-10-09: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005); see the bitrate row of the table below.)* How the target is judged — statistic, number of samples, conditions — is not defined: OWNER DECISION REQUIRED (OQ-008). *(Superseded in part 2026-10-09: owner decision — judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running), with WebRTC viewers on the LAN only; sample count and run length still open (OQ-008).)* The live encode serves both outputs (OQ-005), so its settings are chosen for the WebRTC target. Everything below is PROPOSED or open; nothing has been implemented or tested.

| Item | Pi 4 Model B / CM4 (`bcm2835-codec`) | Pi 5 / CM5 (software x264) |
|---|---|---|
| Frames held back by the encoder | None: B-frames limited to 0 [K-30]; a Raspberry Pi engineer stated it holds no extra buffers (community source) [K-33] | None with `tune=zerolatency` [K-27], [K-28]. By default `x264enc` runs the medium preset with 3 B-frames and rc-lookahead 40 (CORRECTED) [K-29] |
| Per-frame encode time | Reported about 10 ms for 720p on a Pi 4 (community source) [K-33]; reasoning (CORRECTED): about 23 ms at 1080p for one stream [K-45]; with the recording encode on the same encoder: unknown (OQ-115) | Unknown until measured [K-45] (OQ-059); Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39] |
| Latency reported to GStreamer | 0 [K-32]; reasoning from [K-33]: this understates the real encode time (RISK-024) | 0 × frame duration with `zerolatency` [K-28]; reasoning: held-back frames only, not compute time |
| Default GOP | 60 [K-30] | `x264enc` keyframe-interval property and default: not in the register — NEEDS VERIFICATION |
| On-demand keyframe | Force-keyframe through GStreamer 1.26 `v4l2videoenc` [K-31] | `x264enc` behaviour on a force-keyframe request: not in the register — NEEDS VERIFICATION |
| Profile for browsers | Default High; Constrained Baseline available [K-30], [D-11] | `profile=baseline` can be forced through downstream caps [K-29] |
| Bitrate and rate control (row added 2026-10-09; owner decision, OQ-005) | CBR at 17 Mbit/s through `V4L2_CID_MPEG_VIDEO_BITRATE` and `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` [D-13]; whether the bitrate or CBR changes the per-frame encode time is not in the register (OQ-125) | `x264enc` property names and units: NEEDS VERIFICATION. [K-28] counts VBV among the conditions that raise x264's held-back frames to `rc_lookahead`; `zerolatency` sets `rc_lookahead` to 0 [K-27], so even with VBV on no frames are held back (reasoning). Whether a CBR setting on `x264enc` turns VBV on is not in the register — NEEDS VERIFICATION |

**Keyframe interval** (OQ-127; RISK-019):

- YouTube recommends a 2 s keyframe frequency ("Do not exceed 4 seconds") and CBR [K-17].
- Reasoning from [K-30] and [K-17] (as in REQ-ENC-001): the CM4 default GOP of 60 is 2 s at 30 fps and 1 s at 60 fps, within YouTube's recommendation.
- Reasoning from [K-30] (as in OQ-127): at 30 fps, without an on-demand keyframe, a new WebRTC viewer may wait up to about 2 s for a decodable frame — twice the whole < 1 s target. On CM4 an IDR can be requested when a viewer joins [K-31].
- Whether MediaMTX passes a WebRTC viewer's keyframe request (PLI/FIR) back to an RTSP-publishing pipeline is undocumented (research open question, topic K): UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED (OQ-127).

**B-frames and profile** (OQ-127, OQ-073; RISK-019):

- MediaMTX documents that browsers deliberately do not support H.264 with B-frames over WebRTC and recommends H.264 Baseline with Opus [K-04]. WebRTC publishing to MediaMTX with `whipclientsink` needs the Baseline profile [K-05].
- YouTube recommends "2 B-Frames", "1 Reference Frame" and "CABAC" under advanced settings; the B-frame advice conflicts with the B-frame-free stream WebRTC requires [K-17].
- Reasoning: one live encode cannot follow both. Because the < 1 s target is for WebRTC viewers (OQ-116), the live encode stays B-frame-free and RTMP receives the same stream. Research expects YouTube quality at a given bitrate may drop without B-frames (research design risk, topic K — not a register fact). Whether YouTube and other destinations accept the stream with acceptable quality: VENDOR CONFIRMATION REQUIRED (OQ-127).
- On CM4 the stream is B-frame-free by construction [K-30], but the default profile is High [K-30]; whether browsers accept High over WHEP from MediaMTX is a research open question (topic K) — BUILD TEST REQUIRED (OQ-073). On CM5 the stream is B-frame-free only if `x264enc` is configured explicitly [K-29].

**Budget and queues.** The documented capture and encode terms for a CM4 1080p30 LAN viewer come to about 56 ms typical and 75 ms worst case (reasoning, CORRECTED; a labelled budget, not a measurement) [K-45]. Inputs: about 33.3 ms of capture readout plus the §3.14 encode estimate of about 23 ms (typical) or about 42 ms (from the reporter's 18.5 ms maximum); 33.3 + 22.7 ≈ 56 ms and 33.3 + 41.9 ≈ 75 ms. The full budget is in [PERFORMANCE.md](PERFORMANCE.md) §7, and the live-path element defaults are in [STREAMING.md](STREAMING.md) §4.6 (OQ-125, OQ-126; RISK-031, RISK-032).

### 7.2 Recording encode and fragmented MP4 (added 2026-10-09)

**Owner decision of 2026-10-08 (ADR-009, ACCEPTED).** Recordings are MP4, written fragmented for power-loss safety, and every recording is mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure (owner's words: "Self-powered enclosure"). Storage, filesystem and muxer details are in [RECORDING.md](RECORDING.md). This section records only what reaches the encoder.

| Item | Fact | Source |
|---|---|---|
| FFmpeg 7.1 fragments | A fragmented file stays decodable if writing is interrupted, while a normal MOV/MP4 is undecodable if not properly finished; the downside is lower compatibility. `movflags +frag_keyframe` starts a new fragment at each video keyframe. `hybrid_fragmented` writes a fragmented file and converts it to a normal one at the end. | [J-45] |
| GStreamer 1.26.2 fragments | `mp4mux`/`qtmux` `fragment-duration` is in milliseconds, defaults to 0 and produces a fragmented file when > 0. `fragment-mode` is `dash-or-mss` (default) or `first-moov-then-finalise`. | [J-39] |
| File splitting | `splitmuxsink` starts a new file at a video keyframe when the contents are about to cross `max-size-time` or `max-size-bytes`; the minimum file is one GOP. | [J-44] |

Reasoning (not measurements):

- **Fragment length.** With FFmpeg `+frag_keyframe`, the recording encode's keyframe interval sets the fragment length (as in OQ-118, from [J-45]). Whether GStreamer `mp4mux` places fragment boundaries at keyframes when `fragment-duration` is set is not in the register: NEEDS VERIFICATION. BUILD TEST REQUIRED (OQ-118).
- **File splits.** With `splitmuxsink`, files can split only at keyframes, so the recording GOP is also the smallest split step [J-44]. *(2026-10-09: recordings have no duration limit — they run until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED). Whether long recordings are split at all, and at what interval, is OWNER DECISION REQUIRED (OQ-129, OPEN).)* *(Superseded 2026-10-09, later still: owner decision — recordings are split into a new file every 30 minutes (OQ-129). Reasoning from [J-36]: about 5.67 GB per 30-minute file per drive at 25 Mbit/s plus the research's assumed 192 kbit/s AAC. Candidate mechanism, not a decision (ADR-007 `OPEN`): `splitmuxsink` starts a new file at a video keyframe when the contents are about to cross `max-size-time`, so each file would end at the keyframe before the 30-minute mark, and it can request keyframes upstream (`send-keyframe-requests`, effective only when `max-size-bytes` = 0) [J-44]. Reasoning: such a request would go to the recording encode; whether the recording encoder on each platform acts on it is not in the register: NEEDS VERIFICATION; BUILD TEST REQUIRED (OQ-118). Whether `splitmuxsink` works with `mp4mux` in fragmented mode, and how FFmpeg would split, is not in the register either: NEEDS VERIFICATION (OQ-118).)*
- **Power loss.** The amount lost at a power cut depends on the fragment duration (OQ-118, OQ-119; RISK-030). With `+frag_keyframe`, a shorter recording keyframe interval therefore shortens fragments. Whether, and by how much, a shorter interval raises the bitrate needed for the same quality is not in the register (NEEDS VERIFICATION).
- **Per platform.** On Pi 4 Model B / CM4 the recording keyframe interval is the encoder's `V4L2_CID_MPEG_VIDEO_GOP_SIZE` (default 60 [K-30]); every I-frame is IDR [D-14]. Reasoning: at that default and `+frag_keyframe`, fragments would be 2 s at 30 fps and 1 s at 60 fps. On Pi 5 / CM5 the `x264enc` keyframe-interval property is not in the register (NEEDS VERIFICATION).
- **Independent of the live encode.** The recording encode is not bound by WebRTC (§7), so its keyframe interval can differ from the live one (OQ-005, OQ-118). B-frames in the recording encode are possible only in software on Pi 5 / CM5 and are UNDEFINED (§4.3; OQ-005); on Pi 4 / CM4 there are none [K-30]. *(2026-10-09, later: the recording keyframe interval remains UNDEFINED (OQ-118) and its B-frames remain UNDEFINED (OQ-005); the recording bitrate is set — 25 Mbit/s VBR.)* *(Superseded 2026-10-09, later still: owner decision (OQ-005) — the recording encode is **H.264 High profile, Level 4.2, no B-frames**, the same on CM4 and CM5. B-frames are now decided (none), overriding the "UNDEFINED" above: CM4 emits none [D-14], and CM5 must force `bframes=0`/caps `profile=baseline`/`tune=zerolatency` [K-29]. Profile High and Level 4.2 (1080p60 exceeds Level 4.0's macroblock rate [F-40], §6); whether 25 Mbit/s fits Level 4.2's maximum bitrate is OQ-073. The keyframe interval is still OQ-118.)*
- **Capture-to-file latency** *(added 2026-10-09, later; owner decision, OQ-005)*. The recording path has a **capture-to-file latency target of under 1 s glass-to-disk**. Reasoning: an HDD standby-to-ready stall of up to about 3.0 s [J-30] (RISK-028) means the bound cannot hold on the HDD branch during a stall unless that branch is decoupled or buffered (OQ-117). Against which drive the bound is judged (the fast NVMe copy, the HDD copy, or both), and over what statistic and sample count, is OQ-130. Measured in TEST-REC-001. *(OQ-130 ANSWERED 2026-10-09: judged against the **primary recording target** — the **NVMe SSD** — at the **95th percentile**; the HDD is a **lagging mirror** not bound by the target, realised by the decoupled HDD branch (OQ-117).)*
- **Mirror, not a third encode.** ADR-009 writes the one recording bitstream to two file writers, so the mirror adds no encoder load. An HDD stall — one Seagate BarraCuda 2.5-inch HDD family (an example, not a general figure) takes 2.5 s typical, 3.0 s maximum from standby to ready (CORRECTED) [J-30] — must not back-pressure the recording encode. On CM4 that encode would share the hardware encoder with the live encode (whether it runs both is OQ-115), so a stall reaching it could reach the live stream (RISK-028; OQ-117, OQ-126).
- **Bitrate and storage.** Reasoning [J-36]: per recording destination, H.264 at 8 / 12 / 25 Mbit/s plus 192 kbit/s AAC writes about 1.02 / 1.52 / 3.15 MB/s. These are research examples, not an owner choice; the recording bitrate is still OQ-005. *(Superseded in part 2026-10-09: owner decision — the recording encode is 25 Mbit/s VBR (OQ-005), the highest of these examples and the CM4 encoder's maximum [D-13]. Reasoning [J-36] at that rate with 192 kbit/s AAC (an assumption of [J-36]; container overhead excluded): about 3.15 MB/s and 11.34 GB per hour per destination, about 88 h per 1 TB, and about 6.30 MB/s for the mirrored system. [J-36] computes with a constant 25 Mbit/s; the actual average and peak write rate of the VBR encode are not in the register — HARDWARE TEST REQUIRED (TEST-REC-001). Budget per board: [PERFORMANCE.md](PERFORMANCE.md) §4.4.)*
- **One drive full, absent or failed** *(added 2026-10-09; owner decision, OQ-129)*. If one mirrored drive fills, is absent or fails during a recording, recording continues on the drive that still works and the operator is alerted. Reasoning: the recording encode therefore keeps running when one of its two file writers stops, so the failure must be confined to that writer's branch, as for HDD stalls (OQ-117, RISK-028). Still open (OQ-129): whether and at what interval long recordings are split into files (see **File splits** above), how the operator is alerted (OQ-091), and whether a drive that comes back during a recording is used again. How `mp4mux`, `splitmuxsink` or the FFmpeg MP4 muxer behave on a full disk is not in the register: NEEDS VERIFICATION. BUILD TEST REQUIRED (OQ-129). *(Superseded in part 2026-10-09, later still: file splitting is decided — a new file every 30 minutes (see **File splits** above) — and a recording started with one drive missing runs on the available drive with an operator alert (owner; OQ-129). Reasoning: the recording encode then starts with one file writer instead of two. Still open: drive return (OQ-129); alert method (OQ-091).)*

---

## 8. Licensing

| Item | Fact | Source | Open question |
|---|---|---|---|
| x264 | Buildroot's x264 help text says x264 is released under the GNU GPL | [D-47] | OQ-087 |
| FFmpeg + libx264 | `libx264` is in FFmpeg's `EXTERNAL_LIBRARY_GPL_LIST`; FFmpeg must be built with `--enable-gpl` | [D-42] | OQ-087 |
| Buildroot FFmpeg | `BR2_PACKAGE_FFMPEG_GPL=y` on its own changes `FFMPEG_LICENSE` from "LGPL-2.1+, libjpeg license" to "LGPL-2.1+, libjpeg license and GPL-2.0+". `--enable-libx264` is passed only when both `BR2_PACKAGE_X264=y` and `BR2_PACKAGE_FFMPEG_GPL=y` (CORRECTED verdict) | [D-46] | OQ-087 |
| openh264 | Licence terms not researched | — | OQ-087 |
| Pi 4/CM4 hardware encoder | Encoding runs in the VideoCore firmware [D-08]. The licence file of the firmware commit pinned by Buildroot 2026.08 (`boot/LICENCE.broadcom`) is a binary-only, no-modification licence restricted to use "for the purposes of developing for, running or using a Raspberry Pi device", although Buildroot labels the package BSD-3-Clause. Raspberry Pi OS installs equivalent, newer, proprietary GPU firmware (CORRECTED verdict). The licence text of the Raspberry Pi OS firmware package was not quoted in the register — NEEDS VERIFICATION | [D-08], [G-69] | OQ-088 |
| H.264 patents | Not researched. Applies to hardware and software encode | — | OQ-086 (LEGAL CLARIFICATION REQUIRED) |
| x265 (added 2026-10-08; as a PACSCORDER encoder, deferred — REQ-ENC-002; still shipped as an FFmpeg dependency, see the note below the table) | Copyright MulticoreWare. Licensed under GPL version 2 "or (at your option) any later version", and also under a commercial proprietary licence. The x265 documentation states that neither licence covers HEVC patents | [H-39] | OQ-087 |
| Raspberry Pi FFmpeg (added 2026-10-08) | Links `libx264` and `libx265`, which are on FFmpeg's `EXTERNAL_LIBRARY_GPL_LIST`, so it is a GPL build | [I-39], [H-09] | OQ-087 |
| HEVC patents (added 2026-10-08; deferred — REQ-ENC-002; not in current scope, but see the note below the table) | Two pools quote per-unit rates. VCL Advance (the former Via LA HEVC/VVC programme, acquired by Access Advance as of 15 December 2025): $0.00 for units 1–100,000, then $0.30 (Region 1) or $0.20 (Region 2) per unit [H-40]. Access Advance says a licence is "most likely" needed for any product that can encode and/or decode HEVC [H-41]. Its "Connected Home & Other Devices" rate for devices over $80 is $1.111 (Region 1) / $0.555 (Region 2) per unit in compliance, without trademark discount [H-42] | [H-40], [H-41], [H-42] | OQ-109 (LEGAL CLARIFICATION REQUIRED) |

On Pi 5/CM5, software encoding is unavoidable [G-22], [D-31], so the licence of whichever software encoder is chosen applies on those platforms (reasoning): x264 is GPL [D-47]; the openh264 licence was not researched (OQ-087). H.264 patent questions apply to hardware and software encoding alike (OQ-086). This is RISK-015, which is retired only by legal review (LEGAL CLARIFICATION REQUIRED).

*(Added 2026-10-08.)* Reasoning from [D-24], [D-31] and [H-39]–[H-42]: H.265 is software-encoded on **every** platform. x265's GPL obligations (or a commercial x265 licence) and HEVC patent licensing therefore apply on Pi 4 Model B/CM4 as well as on Pi 5/CM5. They come on top of whatever H.264 licensing applies (OQ-086). Research asks whether a commercial x265 licence should be bought instead of meeting x265's GPL obligations (research open question, topic H; OQ-087). Which pool category and region apply to PACSCORDER is LEGAL CLARIFICATION REQUIRED (OQ-109). This extends RISK-015 and is part of RISK-022.

*(Superseded in part, 2026-10-08, later: H.265 is deferred — REQ-ENC-002; not in current scope — so PACSCORDER does not encode H.265, and the x265 and HEVC-patent points above apply in full only if REQ-ENC-002 is re-activated. One point remains in the current scope. The Raspberry Pi FFmpeg's arm64 `libavcodec61` depends on `libx265-215` [H-09], so x265 is on the image whenever that FFmpeg is, and that FFmpeg stays a GPL build [I-39], [H-09]. Whether shipping `libx265` only as an unused FFmpeg dependency has x265-licence or HEVC-patent consequences is not established: LEGAL CLARIFICATION REQUIRED (OQ-087, OQ-109).)*

---

## 9. Open items that block the encoder design

| OQ | Question (short) | Resolution marker |
|---|---|---|
| OQ-005 | Codec, bitrate, rate control, latency target, number of simultaneous encodes. *(2026-10-08: codec answered by the owner on 2026-10-07 — H.264 and H.265, REQ-ENC-001; the rest is still open)* *(2026-10-08, later: codec narrowed to H.264 only — "H.264 only for now", OQ-103 ANSWERED; H.265 deferred, REQ-ENC-002)* *(2026-10-08, two encodes: number of encodes set by the owner — "Separate record + live", two H.264 encodes, recording and one live encode shared by RTMP and WebRTC; bitrate, rate control and latency still open; OQ-005 stays OPEN)* *(2026-10-09: latency target set by the owner on 2026-10-08 — < 1 s camera-to-viewer for WebRTC viewers, RTMP best-effort (OQ-116 ANSWERED); bitrate and rate control still open; OQ-005 stays OPEN)* *(2026-10-09, later: bitrate and rate control set by the owner — live CBR 17 Mbit/s, recording 25 Mbit/s VBR, no per-mode breakdown; OQ-005 stays OPEN for the recording encode's profile, level and B-frames and for a capture-to-file latency target for recordings, if one is required)* *(2026-10-09, latest: owner decisions — recording encode High profile, Level 4.2, no B-frames, and a recording capture-to-file latency target under 1 s glass-to-disk; **OQ-005 ANSWERED**; new OQ-130 for which drive and statistic)* | OWNER DECISION REQUIRED |
| OQ-116 | Does the < 1 s target apply to RTMP? (added to this table 2026-10-09) **ANSWERED 2026-10-08: WebRTC viewers only; RTMP best-effort.** | — (ANSWERED; was OWNER DECISION REQUIRED) |
| OQ-125 | Measured camera-to-viewer latency of the WebRTC path, including the encode term on CM4 and CM5 (added 2026-10-09; §3.14, §4.3, §7.1) | HARDWARE TEST REQUIRED; DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED |
| OQ-126 | Live-path element latencies and queue policy, including encoder backlog on CM5 (added 2026-10-09; §4.3) | BUILD TEST REQUIRED; HARDWARE TEST REQUIRED |
| OQ-127 | Live keyframe interval, on-demand keyframes and B-frame settings shared by RTMP and WebRTC (added 2026-10-09; §7.1) | VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED |
| OQ-117 | HDD-branch buffering, so that HDD stalls cannot back-pressure the encoders or the live path (added 2026-10-09; §7.2) | BUILD TEST REQUIRED; HARDWARE TEST REQUIRED |
| OQ-118 | Fragmented MP4: fragment duration, which the recording keyframe interval bounds with FFmpeg `+frag_keyframe` (GStreamer `mp4mux`: NEEDS VERIFICATION) (added 2026-10-09; §7.2) *(2026-10-09, later still: also whether `splitmuxsink` can wrap `mp4mux` in fragmented mode for the owner's 30-minute files, and how FFmpeg would split (OQ-129); §7.2)* | BUILD TEST REQUIRED; OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-129 | Mirrored recording without a duration limit: one drive full, absent or failed; whether long recordings are split into several files, and at what interval (added 2026-10-09 after OQ-006 was ANSWERED; split step: §7.2) *(2026-10-09, later: owner decision — recording continues on the drive that still works and the operator is alerted; still open: file splitting, the alert method (OQ-091) and whether a returning drive is used again; OQ-129 stays OPEN; §7.2)* *(2026-10-09, later still: owner decisions — a recording started with one drive missing runs on the available drive with an operator alert; recordings are split into a new file every 30 minutes. OQ-129 stays OPEN only for drive return; alert method OQ-091; split mechanism OQ-118; §7.2)* | OWNER DECISION REQUIRED; BUILD TEST REQUIRED; HARDWARE TEST REQUIRED |
| OQ-103 | Which outputs use H.265, in which modes; is software-only H.265 acceptable? (added 2026-10-08) **ANSWERED 2026-10-08: no output uses H.265 for now — H.264 only for recording, RTMP and WebRTC; H.265 deferred (REQ-ENC-002). It no longer blocks the encoder design.** | — (ANSWERED; was OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED) |
| OQ-104 | Software H.265 throughput, latency and CPU headroom on CM4 and CM5 (added 2026-10-08). *Not in current scope (H.265 deferred, REQ-ENC-002); OPEN* | HARDWARE TEST REQUIRED; BUILD TEST REQUIRED |
| OQ-105 | x265 SIMD paths active on CM4 and CM5; newer x265 for trixie (added 2026-10-08). *Not in current scope (H.265 deferred, REQ-ENC-002); OPEN* | BUILD TEST REQUIRED; HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED |
| OQ-106 | HEVC over Enhanced RTMP at the RTMP destinations (added 2026-10-08). *Not in current scope (H.265 deferred, REQ-ENC-002); OPEN* | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-107 | HEVC-over-RTMP muxing path with GStreamer 1.26.2 (added 2026-10-08). *Not in current scope (H.265 deferred, REQ-ENC-002); OPEN* | BUILD TEST REQUIRED; OWNER DECISION REQUIRED |
| OQ-108 | H.265 in WebRTC: which viewer browsers and devices (added 2026-10-08). *Not in current scope (H.265 deferred, REQ-ENC-002); OPEN* | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-109 | HEVC patent licensing (added 2026-10-08). *Not in current scope (H.265 deferred, REQ-ENC-002); OPEN. See §8 on `libx265` as an FFmpeg dependency* | LEGAL CLARIFICATION REQUIRED |
| OQ-011 | Product platform (ADR-004) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-015 | Userspace media framework (ADR-007) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-003 | Accepted capture pixel format (ADR-005) | OWNER DECISION REQUIRED |
| OQ-040 | Fractional frame rates (59.94 vs 60) for encoder timestamps | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-041 | Colourimetry and quantisation range for the encoder | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED |
| OQ-043 | Runtime device node numbers | HARDWARE TEST REQUIRED |
| OQ-048 | Pi 4/CM4 firmware variant and GPU memory for the codec | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-056 | Pi 4/CM4 hardware encode at 1080p60; worst-case IDR size | HARDWARE TEST REQUIRED |
| OQ-096 | Is a GPU overclock (for example `gpu_freq=550`) acceptable in the product? | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-057 | Pi 4/CM4 encoder input formats and conversion path | HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED |
| OQ-058 | Zero-copy DMABUF from Unicam into the encoder | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-059 | Pi 5/CM5 software encode: CPU, thermal, latency, concurrency. *(2026-10-08: concurrency now means the two software H.264 encodes, recording and live)* *(2026-10-09: latency now includes the per-frame x264 time that the live budget needs, §4.3)* *(2026-10-09, later: two software encodes at the decided bitrates, live 17 Mbit/s CBR and recording 25 Mbit/s VBR, OQ-005; `x264enc` bitrate property names and units NEEDS VERIFICATION)* | HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED |
| OQ-115 | Two concurrent H.264 encodes (recording + live) on the CM4 hardware encoder; acceptable split if not (added to this table 2026-10-08) *(2026-10-09: also the live encode's latency under that contention, §3.14)* *(2026-10-09, later: at the decided bitrates, live 17 Mbit/s CBR and recording 25 Mbit/s VBR, OQ-005; whether the [D-13] range applies per session)* | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED |
| OQ-060 | Pi 5/CM5 UYVY-to-planar conversion offload | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-061 | CMA budget per platform (encoder buffers included) | HARDWARE TEST REQUIRED |
| OQ-063 | Audio encoder choice and cost. *(2026-10-08: audio is required, OQ-004 ANSWERED; it shares the CPU with software video encoding, §4A.8)* *(2026-10-08, later: H.265 deferred — in the current scope, software H.264 on Pi 5/CM5, §4.2)* | HARDWARE TEST REQUIRED; LEGAL CLARIFICATION REQUIRED |
| OQ-073 | H.264 level signalling for 1080p WebRTC *(2026-10-09: also whether the decided 17 and 25 Mbit/s fit the maximum bitrate of the profile and level each encode signals — not in the register; §6)* | HARDWARE TEST REQUIRED *(2026-10-09: for the bitrate part, DATASHEET REQUIRED — the H.264 specification)* |
| OQ-086 | H.264 patent licensing | LEGAL CLARIFICATION REQUIRED |
| OQ-087 | GPL and source-offer compliance | LEGAL CLARIFICATION REQUIRED; BUILD TEST REQUIRED |
| OQ-088 | Proprietary GPU firmware licence | LEGAL CLARIFICATION REQUIRED |

---

## 10. Traceability (Rule 12)

```text
REQ-ENC-001 (DRAFT)
    ↓
Design: this document; ADR-004 (OPEN), ADR-005 (PROPOSED), ADR-007 (OPEN)
    ↓
Implementation: NOT STARTED
    ↓
Test: TEST-ENC-001, TEST-DMA-001 — BLOCKED — HARDWARE REQUIRED
    ↓
Result: none
```

*(Added 2026-10-08, later.)* H.265 is traced separately, as the deferred requirement:

```text
REQ-ENC-002 (DEFERRED, owner 2026-10-08: "H.264 only for now", OQ-103)
    ↓
Design: §4A and the H.265 rows and notes of §2, §3.10, §5–§8 — evidence only, not in current scope
    ↓
Implementation: NOT STARTED
    ↓
Test: none planned (deferred; H.265 runs of TEST-ENC-001 are not run in current scope)
    ↓
Result: none
```

Risks: RISK-002 (Pi 4/CM4 1080p60 encode unproven), RISK-003 (no hardware encoder on Pi 5/CM5), RISK-015 (GPL and patent licensing), RISK-019 (WebRTC profile/level constraints), RISK-020 (CMA sizing).

*(Added 2026-10-08.)* Codecs are H.264 and H.265 (owner, 2026-10-07). ADR-004 is still OPEN; bring-up evaluates CM4 and CM5 side by side. RISK-022 asks for H.265 runs on CM4 and CM5 as part of TEST-ENC-001, whose canonical title was changed on 2026-10-08 to "Sustained real-time H.264 / H.265 encode". Further risks:

- RISK-015, now extended to x265 and HEVC patents (§8);
- RISK-022, H.265 required but software-only on every candidate (§4A);
- RISK-025, HEVC over RTMP may force a split GStreamer/FFmpeg architecture (§4A.7).

*(Superseded 2026-10-08, later: the owner answered OQ-103 with "H.264 only for now". REQ-ENC-001 is H.264 only, for all outputs; H.265 is REQ-ENC-002 (DEFERRED). TEST-ENC-001 is now titled "Sustained real-time H.264 encode (H.265 deferred)"; its H.265 runs are deferred, not run in current scope. RISK-022 and RISK-025 stay OPEN but are not in current scope. The RISK-015 extension to x265 and HEVC patents applies in full only if REQ-ENC-002 is re-activated; the `libx265` FFmpeg-dependency question stays open (§8; OQ-087, OQ-109).)*

*(Added 2026-10-08, two encodes.)* REQ-ENC-001 now requires two simultaneous H.264 encodes, recording and live (owner: "Separate record + live", OQ-005). Effects on the risks, statuses unchanged:

- RISK-002 now also covers two concurrent encodes on the CM4 hardware encoder: two 1080p30 encodes already equal one 1080p60 encode in macroblock rate (reasoning [D-10], [D-52]; OQ-115);
- RISK-003 now covers two concurrent software encodes on Pi 5/CM5 (reasoning from [G-22]; OQ-059);
- RISK-019 applies to the live encode, which RTMP and WebRTC share (§6.2, §7).

TEST-ENC-001 includes a two-encode run on CM4 and on CM5 (§11, check E-3; [PERFORMANCE.md](PERFORMANCE.md) §10.3).

*(Added 2026-10-09; owner decisions of 2026-10-08 on latency (OQ-116 ANSWERED) and recording (ADR-009); research topics J and K.)* Effects on the risks, statuses and severities unchanged:

- RISK-019 now also covers the keyframe interval and the YouTube B-frame recommendation that the B-frame-free live encode cannot follow [K-17] (§7.1; OQ-127);
- RISK-024 now also covers `v4l2h264enc` reporting 0 latency on BCM2711 [K-32], so pipeline latency and A/V sync leave out the encode time (§3.14);
- RISK-028: an HDD stall in the mirrored recording must not back-pressure the recording encode, which on CM4 would share the hardware encoder with the live encode (§7.2; OQ-117; OQ-115);
- RISK-031: the < 1 s WebRTC target is unproven; the encode term is about 23 ms on CM4 by reasoning [K-45] and unknown on CM5 (§3.14, §4.3; OQ-125);
- RISK-032: with its defaults, `x264enc` runs 3 B-frames and rc-lookahead 40 [K-29], which x264 holds frames back for [K-28]; and a CM5 encode slower than one frame period builds a backlog (§4.3; OQ-126).

*(Added 2026-10-09, later; owner decisions on OQ-005 — live CBR 17 Mbit/s, recording 25 Mbit/s VBR — and on OQ-129 — continue on the remaining drive.)* Effects on the risks, statuses and severities unchanged:

- RISK-002 and RISK-003: the two concurrent encodes now have set bitrates; whether CM4 sustains both at these rates is OQ-115, and the CPU cost on CM5 is OQ-059 (§1, §4.2);
- RISK-019: OQ-073, which RISK-019 tracks, now also covers whether 17 and 25 Mbit/s fit the signalled profile and level (§6, §6.2);
- RISK-028: a failed or full drive must not end the recording — recording continues on the other drive and the operator is alerted (OQ-129); the stall concern, which is about a slow HDD, is unchanged (§7.2).

The REQ-ENC-001 trace chain above is unchanged: Implementation NOT STARTED; TEST-ENC-001 BLOCKED — HARDWARE REQUIRED.

---

## 11. First encoder checks — NOT YET RUN ON PACSCORDER HARDWARE

These checks gather the facts that OQ-043, OQ-057 and the platform comparison need. The full procedures, pass criteria and results belong in [TESTING.md](TESTING.md) (TEST-ENC-001, TEST-DMA-001). Measurement methods are in [PERFORMANCE.md](PERFORMANCE.md).

**Check E-1 (Pi 4 / CM4): encoder node and input formats.** NOT YET RUN ON PACSCORDER HARDWARE.

```bash
v4l2-ctl --list-formats-out -d 11
```

- Source of the command: [D-17] (community listing from 2021).
- Expected, according to sources only:
  - the device is `bcm2835-codec-encode`, Video Output Multiplanar [D-08], [D-17];
  - UYVY is in the list [D-17];
  - the list depends on the firmware [D-16].
- If the encoder is not at node 11, use the node found for card `bcm2835-codec-encode` (OQ-043). The command that enumerates devices is NEEDS VERIFICATION (OQ-101).
- Record the full list, the firmware version and the kernel version. The commands that read the kernel and firmware versions are not attested in the register: NEEDS VERIFICATION (OQ-101).

**Check E-2 (Pi 5 / CM5): absence of a hardware encoder node.** NOT YET RUN ON PACSCORDER HARDWARE.

- Expected (reasoning [D-29]): no device with card name `bcm2835-codec-encode` exists.
- The command that enumerates devices is NEEDS VERIFICATION (OQ-101).

**Check E-3 (all platforms): sustained encode.** This is TEST-ENC-001. See [PERFORMANCE.md](PERFORMANCE.md) §10.3 (procedure for TEST-ENC-001) for the metrics to record. *(Added 2026-10-08.)* Include H.265 runs on CM4 and CM5 (RISK-022, OQ-104); [PERFORMANCE.md](PERFORMANCE.md) §10.3 lists them. *(Superseded 2026-10-08, later: the H.265 runs are deferred, not run in current scope (REQ-ENC-002; OQ-103 ANSWERED). [PERFORMANCE.md](PERFORMANCE.md) §10.3 keeps them as the reference procedure for when REQ-ENC-002 is re-activated.)* *(Added 2026-10-08, two encodes.)* TEST-ENC-001 includes a **two-encode run**: the recording encode and the live encode at the same time, from one capture, on CM4 (both on the hardware encoder; OQ-115, RISK-002) and on CM5 (both in software; OQ-059, RISK-003). The live encode uses the §7 settings. Frame rate, drops, latency and largest frame are recorded per encode. [PERFORMANCE.md](PERFORMANCE.md) §10.3 lists the runs. *(Added 2026-10-09; scope notes from OQ-125, OQ-127 and research topic K; not accepted criteria.)* The two-encode run also records the live encode's per-frame latency: on CM4 with the recording encode on the same hardware encoder (§3.14; OQ-115, OQ-125), and on CM5 the per-frame x264 time with `tune=zerolatency` (§4.3; OQ-059). On CM5 it checks that the `x264enc` output has no B-frames with the chosen settings (BUILD TEST REQUIRED; OQ-127). Research names QBUF-to-DQBUF timing for CM4 and inspecting `x264enc` debug output or slice types for CM5 (research open question and research gap, topic K); neither method is in the register, and both are NOT YET RUN ON PACSCORDER HARDWARE. *(Added 2026-10-09, later; scope notes from the owner decision on OQ-005, not accepted criteria.)* The two-encode run uses the decided bitrates — live encode CBR 17 Mbit/s, recording encode 25 Mbit/s VBR — on CM4 (OQ-115) and CM5 (OQ-059), and records each encode's output bitrate. The method for measuring the output bitrate is not in the register: NEEDS VERIFICATION.

**Check E-4 (CM4 and CM5, any kept platform): x265 encoders and SIMD paths** *(added 2026-10-08)*. NOT YET RUN ON PACSCORDER HARDWARE. **Deferred — REQ-ENC-002; not run in current scope.** Kept as the reference check for when REQ-ENC-002 is re-activated.

- Expected, according to sources only:
  - `libgstx265.so` is installed with `gstreamer1.0-plugins-bad` [H-11], [H-12];
  - the FFmpeg `libavcodec61` contains the encoder "libx265 H.265 / HEVC" [H-09];
  - x265 is 4.1 [H-01], [H-02].
- Expected, by reasoning from [H-04] and [H-05]:
  - on CM5, x265 reports Neon DotProd among its CPU capabilities, provided the binary contains the kernels and the kernel reports ASIMDDP;
  - on CM4, x265 reports no DotProd.
- Record the x265 version and the CPU capabilities x265 reports on each board (OQ-105).
- The commands that list GStreamer elements and FFmpeg encoders, and that show x265's CPU capabilities, are not attested in the source register: NEEDS VERIFICATION (OQ-101, OQ-105). BUILD TEST REQUIRED.

---

## 12. Verification status

### Verified from sources (fact IDs)

"Verified from sources" means that the cited source says this (see [REFERENCES.md](REFERENCES.md), "What a verdict means"). It does not mean the behaviour has been observed on PACSCORDER hardware.

- Pi 4/CM4 driver, Kconfig, module, defconfigs, downstream-only status: [D-01], [D-02], [D-03], [D-04], [D-05], [E-46], [G-18], [G-71], [D-49].
- Device nodes and M2M interface: [D-06], [D-07], [D-08], [D-19], [D-21].
- Size limits, specification, levels, controls: [D-09], [D-10], [D-11], [D-12], [D-13], [D-14], [D-15], [D-52]; community report [D-50].
- Input formats: [D-16], [D-34]; community reports [D-17], [D-18]. Colour: [B-34].
- DMABUF constraints and framework support: [D-19], [D-20], [D-21], [D-22], [C-53], [D-34], [D-44], [D-45], [D-46], [D-53].
- Encoded buffer size: [D-23]. No HEVC encode: [D-24].
- Helper devices: [D-25], [D-26], [D-27], [B-27], [E-33].
- Firmware variant: [D-48], [E-13], [E-17], [G-69].
- Pi 5/CM5 no hardware encoder: [D-28], [D-29], [D-30], [D-31], [D-51], [G-22], [E-52]; community report [D-50].
- Pi 5/CM5 software encoders, latency and packages: [D-32], [D-35], [D-40], [D-41], [D-42], [D-43], [D-47], [E-36], [G-17], [G-29], [G-30]; community report [C-41].
- Framework names: [D-33], [D-37], [D-38], [D-39], [D-42], [D-47], [E-32], [F-35], [G-23].
- Levels and WebRTC: [D-36], [D-52], [F-38], [F-39], [F-40]; community report [D-54].
- Streaming settings: [D-15], [D-37], [F-31], [F-34], [F-36], [F-41], [B-28]; community report [F-45].
- Capture-side context: [A-08], [C-01], [C-02], [C-04], [C-05], [C-09], [C-11], [C-29], [C-37], [C-47], [C-48], [C-49].
- *(Added 2026-10-08.)* H.265 (§2, §3.10, §4A, §5–§8; deferred — REQ-ENC-002; kept as its evidence):
  - packages and encoders: [H-01], [H-02], [H-03], [H-07], [H-08], [H-09], [H-11], [H-12] (CORRECTED), [H-18] (CORRECTED);
  - input formats: [H-10] (CORRECTED), [H-13];
  - presets, tune and latency: [H-14], [H-15], [H-16], [H-17];
  - SIMD: [H-04], [H-05], [H-06];
  - cost evidence: community reports [H-19], [H-20], [H-21], [H-22]; reasoning [H-23];
  - outputs: [H-26], [H-27], [H-29], [H-30], [H-31], [H-32], [H-33], [H-34], [H-35], [H-37], [H-38], with [F-31], [F-34] (CORRECTED), [F-36]; community reports [H-36], [F-45];
  - licensing: [H-39], [H-40], [H-41], [H-42], [I-39];
  - conversion traffic: reasoning [H-43].
- *(Added 2026-10-08.)* Audio encoders, context only (§7): [I-39], [I-40] (CORRECTED), [I-41], [I-43], [I-44], [I-45], [I-46].
- *(Added 2026-10-08, two encodes; owner decision OQ-005.)* Two-encode statements (§1, §2, §3.2, §3.5, §3.6, §3.8, §3.9, §4.2, §4.3, §5–§7, §10, §11): [D-06], [D-08], [D-10], [D-11], [D-12], [D-14], [D-15], [D-31], [D-35] (added in the verifier pass), [D-40], [D-41], [D-43], [D-52], [F-31], [F-36], [F-39], [F-40], [F-41], [G-22]; community report [F-45]. The combined macroblock rates, the doubling of the [G-22] CPU figure and the per-encode settings are Claude's reasoning, not register facts. The number of concurrent encode sessions the CM4 encoder supports is not in the register (OQ-115).
- *(Added 2026-10-09; research topics J and K of 2026-10-08; owner decisions OQ-116 and ADR-009.)* All with verdict `CONFIRMED` or `CORRECTED`:
  - Pi 4/CM4 encode latency, controls and reporting (§2, §3.6, §3.14, §7, §7.1): [K-30], [K-31], [K-32]; community report [K-33]; reasoning [K-45] (CORRECTED).
  - Pi 5/CM5 software-encode latency (§2, §4.3, §4.5, §7, §7.1): [K-27], [K-28], [K-29] (CORRECTED), [K-39]; reasoning [K-45] (CORRECTED); reasoning sentence of [K-36].
  - Live encode settings for WebRTC and RTMP (§7, §7.1): [K-04], [K-05], [K-17].
  - Recording encode and fragmented MP4 (§7.2): [J-39], [J-44], [J-45], [J-30] (CORRECTED); reasoning [J-36].
  - 18 entries new to this document: J-30, J-36, J-39, J-44, J-45, K-04, K-05, K-17, K-27, K-28, K-29, K-30, K-31, K-32, K-33, K-36, K-39, K-45. `CORRECTED`, used in corrected wording only: J-30, K-29, K-45. `community`, worded as a report: K-33. `reasoning`, labelled as reasoning: J-36, K-45, and the reasoning sentence of K-36.
  - Statements marked *research gap*, *research open question* or *research design risk* come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json) and are not register facts. The fragment and file-split granularity, the 2 s viewer-join wait and the effect of the mirror on the encoders are Claude's reasoning.
- *(Added 2026-10-09, later; owner decisions on OQ-005 — live CBR 17 Mbit/s, recording 25 Mbit/s VBR — and OQ-129.)* Bitrate, rate-control and drive-failure statements (§1, §2, §3.6, §3.9, §4.2, §4.3, §4A.7, §6, §6.1, §6.2, §7, §7.1, §7.2, §9–§11) cite [D-13], [F-40], [G-22], [H-29], [J-36] (reasoning), [K-17], [K-27] and [K-28], all already cited in this document; no entry is new. The step-multiple check, the average encoded frame sizes, the x264 VBV reading and the consequences of the drive-failure policy are Claude's reasoning, not register facts. No register fact gives the maximum bitrate per H.264 profile and level (OQ-073), and none gives `x264enc` bitrate property names or units.
- *(Added 2026-10-09, later still; owner decisions on OQ-129 — start on the available drive; split every 30 minutes.)* The notes in the header, §1, §7.2 and the §9 OQ-118 and OQ-129 rows cite [J-36] (reasoning) and [J-44], both already cited in this document; no entry is new. The 5.67 GB file size and the statement that a keyframe request would go to the recording encode are Claude's reasoning.

### Verified on PACSCORDER hardware

**Nothing** (no hardware exists as of 2026-10-06). Every statement above about PACSCORDER behaviour is a design input, not a result. The checks in §11 and TEST-ENC-001 / TEST-DMA-001 are BLOCKED — HARDWARE REQUIRED. *(Re-checked 2026-10-08: still nothing; no hardware exists. Every H.265 statement in §4A is a design input, not a result.)* *(Re-checked 2026-10-08, two encodes: no two-encode run exists on any platform.)* *(Re-checked 2026-10-09: no encode latency, keyframe or fragmented-MP4 measurement exists on any platform.)* *(Re-checked 2026-10-09, later: no encode has run at the decided bitrates on any platform.)*

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against REFERENCES.md. Changes: citations added ([D-01], [D-04], [D-05], [C-09], [C-11], [C-29]); reasoning and CORRECTED-verdict labels added; uncited claims removed or marked (quad-core CPU, `FORCE_KEY_FRAME` control type, `threads=1` meaning); Pi 4 vs CM4 and CM4 CAM1 1080p60 wording narrowed to what sources say; "decided with ADR-007" changed to OPEN wording; firmware-licence and Pi 5/CM5 GPL statements narrowed to the evidence; the libwebrtc `42e01f` statement aligned with [F-39]; cross-reference corrected to PERFORMANCE.md §10.3 | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: `gpu_freq=550` acceptability now points to OQ-096 (OQ-056 kept for the measurement) and OQ-096 added to the §9 table; [D-50] 1080p60/4K statements attributed to 6by9 with community label; the RISK-002 "at least 10 minutes" duration labelled as a research open question (OQ-010, OQ-017); §3.13 now says a 4-lane CM4 CAM1 port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s [C-47], OQ-038; link frequency ADR-008 / OQ-099); version-recording commands in check E-1 marked NEEDS VERIFICATION (OQ-101) Final verification pass (same date): the device-enumeration command in checks E-1 and E-2 linked to OQ-101, whose scope note now names it. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: §3.13 adds that Pi 4 Model B and CM4 CAM0 remain 2-lane candidates under REQ-CAP-007 (OQ-001 ANSWERED; ADR-004 OPEN), and that the 2-lane maximum of 1080p50 UYVY is also above the encoder's official 1080p30 specification (reasoning [D-10], [C-37], [C-48]; full-rate encode not specified, OQ-005). No new fact ID cited. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §3.1 consequence: "ADR-003 (OS/build basis, PROPOSED)" → "ADR-003 (OS/build basis, ACCEPTED: Raspberry Pi OS with `rpi-image-gen`)". No evidence, other ADR status (ADR-004 OPEN, ADR-005 PROPOSED, ADR-007 OPEN) or implementation status changed. | Claude (session 2026-10-07) |
| 2026-10-08 | H.265 added from research topic H, plus the owner decisions of 2026-10-07 (second set). Changes: <br>• Header: "Applies to" and "Verification" updated; status rows added for H.265 (NOT STARTED) and for ADR-004 (OPEN; CM4 and CM5 side by side). <br>• §1: OQ-005 codec bullet marked superseded in part (H.264 + H.265, REQ-ENC-001; per output OQ-103); new bullets for H.265 on every platform and for the side-by-side bring-up; pipeline diagram now reads "H.264 and/or H.265 bitstream" (original wording noted). <br>• §2: rows for H.265, CPU core and SIMD, H.265 cost evidence and H.265 licensing; note on the SoC-pair difference. <br>• §3.10: H.265 on Pi 4/CM4 is software x265 on the Cortex-A72. §3.13: CM4 + CM5 side-by-side note. <br>• §4.2: "core type not in register" marked superseded in part ([H-05]); warning against combining [G-22] with [H-23]. §4.7: CM5 concurrency note. <br>• New §4A (H.265 software encoding: packages, planar-only input, presets/tune/latency, cost evidence, DotProd on CM5 only, outputs, concurrency). <br>• §5: H.265 framework row. §6: H.265 levels not in the register. §7: two PROPOSED H.265 rows; codec and audio context updated (OQ-004 ANSWERED; audio encoders [I-39]–[I-46]). §8: x265, GPL FFmpeg and HEVC patent rows, plus reasoning that they apply on every platform. <br>• §9: OQ-103 to OQ-109 added; OQ-005 and OQ-063 rows annotated. §10: H.265 risks (RISK-022, RISK-025, extended RISK-015). §11: E-3 note and new check E-4 (x265 presence and SIMD). §12: H and I fact lists. <br>New citations: H-01 to H-23, H-26, H-27, H-29 to H-43, I-39 to I-41, I-43 to I-46. No REQ or ADR status changed; no measurement added. | Claude (session 2026-10-08) |
| 2026-10-08 | TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (owner chose H.264 + H.265 on 2026-10-07); ID unchanged. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header "Applies to" (H.264 only; H.265 = REQ-ENC-002, DEFERRED), H.265 status row labelled, TEST-ENC-001 row uses the canonical title "Sustained real-time H.264 encode (H.265 deferred)"; §1 codec bullet superseded note, H.265 bullet labelled, pipeline diagram back to "H.264 bitstream" (history kept in the note); §2 H.265 rows labelled, CM4/CM5 H.265 comparison note superseded; dated notes in §3.10, §3.13, §4.2, §4.7; §4A heading labelled "(deferred — REQ-ENC-002; not in current scope)" with a scope note, OQ-103 answer noted in §4A.1 and §4A.8, §4A.2 package note (`x265enc` needed only if REQ-ENC-002 is re-activated; `libx265-215` still pulled in by FFmpeg's `libavcodec61` [H-09]); §5, §6, §7 H.265 rows labelled (PROPOSED unchanged), §7 codec and audio-CPU context; §8 x265 and HEVC-patent rows labelled, note on `libx265` as an FFmpeg dependency (LEGAL CLARIFICATION REQUIRED; OQ-087, OQ-109); §9 OQ-005, OQ-063, OQ-103 (ANSWERED) and OQ-104–OQ-109 (not in current scope) rows; §10 REQ-ENC-002 trace chain and superseded note (RISK-022, RISK-025 OPEN, not in current scope); §11 E-3 H.265 runs and E-4 deferred, not run in current scope; §12 H.265 fact list labelled. H.265 research kept as REQ-ENC-002 evidence. No other decision or status changed; no ID added; no fact ID new to this document. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): header "Applies to"; §1 codec bullet superseded in part and new two-encode bullet (CM4 both on the one hardware encoder, OQ-115, 2 × 1080p30 = 1080p60 rate = 2.0× spec [D-10], [D-52]; CM5 both in software, ≈ double CPU from [G-22], OQ-059; live-encode WebRTC constraints [F-36], [F-39], [F-40], [F-45], [D-11], [D-14], [D-15]; audio still AAC + Opus); pipeline diagram now one capture → recording encode + live encode (old wording noted); §2 new two-encode row; §3.2 one encode node, concurrent sessions UNKNOWN (OQ-115); §3.5 combined-rate note (4.0× for two 1080p60 encodes, reasoning); §3.6 profile and repeat-header values per encode, note on per-session controls; §3.8 shared capture DMABUF candidate (OQ-058, OQ-061, OQ-115); §3.9 encoded buffers per encode; §3.13 CM4 note; §4.2 several-encodes bullet superseded in part (≈ 60–80 % reasoning, linear scaling not established) and shared-conversion bullet (OQ-060); §4.3 live vs recording latency and B-frames; §4.7 and §4A.8 notes; §5 two encoder instances per framework (NEEDS VERIFICATION); §6 per-encode level, summed load on CM4; §6.2 negotiation concerns the live encode, shared with RTMP; §7 live-encode scope paragraph, Constrained Baseline row and audio note (statuses unchanged; RTMP acceptance of Constrained Baseline NEEDS VERIFICATION, OQ-007); §9 OQ-005 and OQ-059 rows annotated, OQ-115 row added; §10 risk note (RISK-002, RISK-003, RISK-019); §11 E-3 two-encode run on CM4 and CM5; §12 fact list and re-check note. No status changed; no ID added; no fact ID new to this document. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — §1 "nothing about it is decided yet" annotated (codec and number of encodes now set; parameters, platform, framework open); CM4 "both encodes run" → "would run" in §1 and the §2 row (concurrency is OQ-115); §3.5 "On CM4 both run" → "would run" and the 2 × 1080p30 sentence labelled "Reasoning"; §4.3 B-frame sentence reworded as a MediaMTX community report [F-45] and the software B-frame example cited [D-35]; §4.7 CM5 note now says the scope *requires* two software encodes and that sustaining them is UNKNOWN (OQ-059); §7 live-encode paragraph: no-B-frames worded as a MediaMTX report (community source); §10 RISK-002 bullet says "in macroblock rate"; §12 two-encode fact list adds [D-35]. No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header "Last updated", "Applies to" (live encode < 1 s for WebRTC viewers, RTMP best-effort; recording fragmented MP4 mirrored; OQ-116, OQ-117, OQ-118, OQ-125 to OQ-127) and "Verification" (topics J and K); §1 short-answer note and OQ-005 bullet superseded in part (latency set; no capture-to-file target), new bullet on the 2026-10-08 decisions; §2 B-frames row [K-30], [K-29] and Latency row superseded ([K-33] community, [K-45], [K-32], [K-39], [K-27], [K-28]); §3.6 B_FRAMES, GOP_SIZE and FORCE_KEY_FRAME rows ([K-30], [K-31]; OQ-127, OQ-118); new §3.14 Pi 4/CM4 encode latency ([K-30]–[K-33], [K-45]; under-reported latency, RISK-024; contention, OQ-115); §4.3 target line and two-encode note superseded in part, new Pi 5/CM5 bullets ([K-39], [K-27], [K-28], [K-29] CORRECTED, [K-45], backlog research design risk with [K-36]; OQ-059, OQ-126); §4.5 rpicam-apps TC358743 note [K-39]; §7 table: B-frames row ([K-04], [K-17], `x264enc` property superseded [K-29], [K-27]), Constrained Baseline row (`x264enc` profile via caps [K-29]), keyframe row ([K-17], [K-30], [K-31]); new §7.1 live encode and the < 1 s target ([K-04], [K-05], [K-17], [K-27]–[K-33], [K-39], [K-45]; OQ-127, OQ-073) and §7.2 recording encode and fragmented MP4 ([J-30], [J-36], [J-39], [J-44], [J-45]; OQ-117, OQ-118, OQ-119); §9 OQ-005, OQ-059, OQ-115 rows annotated, rows OQ-116 (ANSWERED), OQ-125, OQ-126, OQ-127, OQ-117, OQ-118 added; §10 risk note (RISK-019, RISK-024, RISK-028, RISK-031, RISK-032); §11 E-3 latency and B-frame scope notes; §12 J and K fact list (18 new entries). No requirement, ADR, risk, OQ or test status changed. Verifier pass (same date): "the recording keyframe interval bounds the fragment length" qualified in §1, the §3.6 GOP row and the §9 OQ-118 row — it holds for FFmpeg `+frag_keyframe` [J-45] and `splitmuxsink` [J-44], while GStreamer `mp4mux` fragment boundaries are NEEDS VERIFICATION ([J-39] does not say they start at keyframes); CM4 recording encode "shares" → "would share" the hardware encoder (OQ-115) in §3.14, §7.2 and the §10 RISK-028 bullet; §4.3 and §7.1 — how the < 1 s target is judged is still open (OQ-008); §4.3 x264 CLI fragment marked NOT YET RUN ON PACSCORDER HARDWARE, backlog bullet cites the reasoning part of [K-36]; §7 B-frames row — MediaMTX tier difference ([K-04] `vendor-other`, [F-45] `community`) noted with the register note, "cannot follow both" labelled reasoning; §7.1 budget inputs shown (33.3 + 22.7 ≈ 56 ms; 33.3 + 41.9 ≈ 75 ms); §7.2 HDD "self-powered enclosure or hub" → "self-powered enclosure" (the owner's words), "the register's example 2.5-inch HDD" → one Seagate BarraCuda 2.5-inch family, an example [J-30]. No fact ID added. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 (latency judged at the 95th percentile; LAN-only WebRTC viewers; recording until stopped or disk full — OQ-006 ANSWERED, OQ-008, OQ-128/RISK-033 not in current scope, new OQ-129): header "Applies to" — dated note (95th percentile with LAN-only viewers; duration until stopped or disk full; OQ-129); §1 "Latency and recording decisions of 2026-10-08" — live-encode bullet note (95th percentile; LAN only; OQ-128, RISK-033 not in current scope) and recording-encode bullet note (OQ-006 ANSWERED; file splitting and one-drive-full or failure behaviour: OQ-129); §4.3 "how the target is judged" note superseded in part; §4A.7 Recording row — OQ-006 marked ANSWERED; §7.1 "how the target is judged" sentence superseded in part; §7.2 "File splits" note (no duration limit; whether and at what interval to split is OQ-129); §9 open-items table — OQ-129 row added after OQ-118. Original text kept; no citation added or removed; no requirement, ADR, risk, OQ or test status changed. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09, later (live CBR 17 Mbit/s and recording 25 Mbit/s VBR — OQ-005; continue on the remaining drive if one fails — OQ-129): header "Applies to" — "Bitrate and rate control still open" superseded, OQ-129 policy note; §1 — short-answer note and OQ-005 bullet superseded (OQ-005 stays OPEN only for a capture-to-file latency target), recording-encode bullet notes the OQ-129 decision, new bullet "Bitrate and rate control — owner decisions of 2026-10-09" (basis [D-13], [H-29], [K-17]; no per-mode breakdown; CM4 controls [D-13], per-session range OQ-115; CM5 `x264enc` property names NEEDS VERIFICATION, CPU OQ-059; level fit NEEDS VERIFICATION, OQ-073); §2 "Bitrate range" row notes; §3.6 profile row (OQ-005 no longer names the recording profile), BITRATE (17,000,000 / 25,000,000; step-multiple reasoning) and BITRATE_MODE (CBR / VBR) rows superseded; §3.9 average encoded frame sizes at the decided rates (reasoning; worst case still OQ-056); §4.2 CPU at the decided rates UNKNOWN ([G-22] states no bitrate; OQ-059); §4.3 recording B-frames no longer under OQ-005; §4A.7 YouTube note (17 Mbit/s CBR chosen from these figures; YouTube an example, OQ-007); §6 new level-and-bitrate bullet (no level bitrate limit in the register; DATASHEET REQUIRED, OQ-073); §6.1 256 kbit/s note; §6.2 live 17 Mbit/s and OQ-073; §7 intro and live/recording paragraph superseded in part, new "Bitrate and rate control" table row; §7.1 "still open" sentence superseded, new table row (CM4 controls; CM5 x264 VBV reading from [K-27], [K-28], CBR-to-VBV NEEDS VERIFICATION); §7.2 "Bitrate and storage" superseded in part ([J-36] at 25 Mbit/s: ≈ 3.15 MB/s, ≈ 11.34 GB/h per destination, ≈ 88 h per 1 TB, ≈ 6.30 MB/s mirrored; VBR rate HARDWARE TEST REQUIRED), "Independent of the live encode" note, new "One drive full, absent or failed" bullet (OQ-129; OQ-091; full-disk muxer behaviour NEEDS VERIFICATION); §9 OQ-005, OQ-059, OQ-115, OQ-073 and OQ-129 rows annotated; §10 risk note (RISK-002, RISK-003, RISK-019, RISK-028); §11 E-3 scope note (decided bitrates; output-bitrate method NEEDS VERIFICATION); §12 fact-list note and re-check. Original text kept; no fact ID new to this document; no requirement, ADR, risk, OQ or test status changed. Main session, same date: three notes that said OQ-005 "no longer names" the recording encode's profile, B-frames or keyframe interval aligned with the register, where OQ-005 keeps the recording profile, level and B-frames open. Main session, same date: OQ-005 remainder wording aligned with the register — it also keeps the recording encode's profile, level and B-frames open. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 on OQ-129 (start on the available drive when one is missing; split recordings every 30 minutes): header "Applies to" — superseded note (OQ-129 OPEN only for drive return; alert method OQ-091); §1 recording-encode bullet — superseded-in-part note; §7.2 "File splits" — "OWNER DECISION REQUIRED (OQ-129, OPEN)" superseded (30-minute files, about 5.67 GB per file per drive, reasoning from [J-36]; `splitmuxsink` as a candidate mechanism, not a decision, ADR-007 `OPEN` [J-44]; whether each platform's recording encoder acts on keyframe requests, fragmented-mode support and FFmpeg splitting NEEDS VERIFICATION, OQ-118); §7.2 "One drive full, absent or failed" — superseded-in-part note (start with one writer when a drive is missing; drive return and alert method still open); §9 OQ-118 and OQ-129 rows annotated; Verification status — note (no new entry). No REQ, ADR, RISK, OQ or test status changed here. | Claude (session 2026-10-09) |
