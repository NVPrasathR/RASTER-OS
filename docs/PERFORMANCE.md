# PACSCORDER Performance: Budgets and Measurement Plan

| | |
|---|---|
| Document status | DRAFT — budgets calculated from source research; **no measurement exists** |
| Last updated | 2026-10-08 |
| Applies to | Pi 4 Model B, CM4, Pi 5, CM5; the 2-lane and the 4-lane configuration — REQ-PERF-001, REQ-CAP-001, REQ-CAP-006, REQ-CAP-007, REQ-CAP-008, REQ-ENC-001 (H.264 only, owner 2026-10-08: "H.264 only for now", OQ-103), REQ-DMA-001. H.265 is deferred as REQ-ENC-002 (DEFERRED; not in current scope); its budget inputs are kept in §5.3 as evidence. Two simultaneous H.264 encodes — recording, and one live encode shared by RTMP and WebRTC (owner 2026-10-08, "Separate record + live", OQ-005); their combined load is budgeted in §4 and §5 (CM4 OQ-115, CM5 OQ-059) |
| Verification | Source research of 2026-10-06, plus research topics H (H.265/HEVC) and I (HDMI audio) of 2026-10-08 ([REFERENCES.md](REFERENCES.md)). Nothing has been measured or tested on PACSCORDER hardware; no hardware exists as of 2026-10-08. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 8, 10, 22, 23, 25 |

| Item | Status |
|---|---|
| REQ-PERF-001 (sustained operation) implementation | NOT STARTED |
| TEST-CAP-002 (1080p60 capture: frame rate and frame integrity) | BLOCKED — HARDWARE REQUIRED |
| TEST-ENC-001 — Sustained real-time H.264 encode (H.265 deferred) | BLOCKED — HARDWARE REQUIRED |
| TEST-PERF-001 (soak: thermal, CPU, CMA, frame drops) | BLOCKED — HARDWARE REQUIRED |
| Measurements recorded | None (§9) |

Fact references such as `[C-46]` point to [REFERENCES.md](REFERENCES.md). Facts of tier `community` are written as reports ("reported by …"). Every calculation is marked **reasoning** and names its inputs.

---

## 1. How to read this document

- A **budget** is the demand on a resource (link bandwidth, macroblock rate, CPU, memory, time), calculated from source facts. A **measurement** is a value observed on PACSCORDER hardware. Under Rule 23, a measurement (priority 1) overrides every budget here. When a measurement contradicts a budget, record the measurement in §9 and in [TESTING.md](TESTING.md). Keep the budget text and add a dated note; do not delete it (Rule 21).
- The budgets below are **design inputs, not results**. None of them proves that PACSCORDER meets a requirement.
- Units:
  - Gbit/s and Mbit/s are decimal (10⁹, 10⁶).
  - MB next to a source value follows that source. The buffer sizes of [C-53] are decimal (16.6 MB = 16,588,800 bytes). The device-tree and overlay CMA sizes of [E-47] and [C-40] are binary (64 MB = `0x4000000` = 67,108,864 bytes).
  - MiB/KiB are binary.
  - "MB/s" in §4 means *macroblocks per second*, as in [D-36] and [F-40].

---

## 2. Performance targets — UNDEFINED

No performance target has been set by the owner. REQ-PERF-001 is PROPOSED, and its parameters are open. The capture-rate scope was set by the owner on 2026-10-07 (REQ-CAP-007, last row below); it is a capture requirement, not a performance target.

| Target | Value | Resolution |
|---|---|---|
| Continuous operation (soak) duration | UNDEFINED | OWNER DECISION REQUIRED (OQ-010) |
| Acceptable frame-drop threshold | UNDEFINED | OWNER DECISION REQUIRED (OQ-010) |
| Ambient operating temperature range, enclosure, cooling | UNDEFINED | OWNER DECISION REQUIRED (OQ-010) |
| End-to-end latency (glass-to-glass for WebRTC; capture-to-file for recording) | UNDEFINED | OWNER DECISION REQUIRED (OQ-005, OQ-008) |
| Bitrate(s) and number of simultaneous encodes | Number of encodes *(2026-10-08)*: two simultaneous H.264 encodes — one recording encode and one live encode shared by RTMP and WebRTC (owner, "Separate record + live", answer to OQ-005; REQ-ENC-001). Bitrate(s) and rate control: UNDEFINED. *(Earlier entry, superseded: both UNDEFINED.)* This sets the encode load; it is not a performance target | OWNER DECISION REQUIRED for bitrate and rate control (OQ-005, OPEN). Concurrency: CM4 OQ-115, CM5 OQ-059 |
| Boot-to-first-frame time | UNDEFINED | OWNER DECISION REQUIRED (OQ-068) |
| Capture rates per lane configuration | Owner, 2026-10-07 (OQ-001 ANSWERED): both a 2-lane and a 4-lane configuration, each capturing every frame rate its link carries (REQ-CAP-007, DRAFT). Recorded interpretation: 1080p60 required on the 4-lane configuration; the 2-lane configuration is limited to 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48] (§3.2). The exact mode list is UNDEFINED | OWNER DECISION REQUIRED (OQ-002; fractional rates OQ-040) |
| Codecs (added 2026-10-08) | Owner, 2026-10-08 (answer to OQ-103): "H.264 only for now" — H.264 for recording, RTMP and WebRTC (REQ-ENC-001, DRAFT). H.265 is deferred (REQ-ENC-002, DEFERRED; not in current scope). *(Earlier entry, superseded: owner, 2026-10-07, H.264 **and** H.265 for recording and streaming; which output used which codec was UNDEFINED.)* This is a codec requirement, not a performance target | OQ-103 ANSWERED (2026-10-08) |
| HDMI audio (added 2026-10-08) | Owner, 2026-10-07: required in recordings and streams (OQ-004 ANSWERED; REQ-CAP-006, DRAFT). Channel count, sample rates and the A/V synchronisation tolerance are UNDEFINED | OWNER DECISION REQUIRED (OQ-004 answer; tolerance OQ-112) |
| Platforms measured in bring-up (added 2026-10-08) | Owner, 2026-10-07: CM4 and CM5 side by side; the product platform is decided from the measurements (ADR-004 OPEN) | Measurements in §9 (none yet) |

---

## 3. CSI-2 bandwidth budget

### 3.1 Payload versus link capacity

The payload uses the driver's formula, which counts active pixels only. The frame rates come from the CEA timings: 1080p60 = 148.5 MHz / (2200 × 1125); 1080p50 = 148.5 MHz / (2640 × 1125); 1080p30 = 74.25 MHz / (2200 × 1125) [C-46].

| Mode | UYVY (16 bpp) payload | RGB888 (24 bpp) payload |
|---|---|---|
| 1080p60 | 1.990656 Gbit/s | 2.985984 Gbit/s |
| 1080p50 | 1.658880 Gbit/s | 2.488320 Gbit/s |
| 1080p30 | 0.995328 Gbit/s | 1.492992 Gbit/s |

Source: [C-46] (reasoning).

Link capacity is 2 × link frequency per lane [C-46]:

| Link frequency | Per lane | 2 lanes | 4 lanes |
|---|---|---|---|
| 297 MHz (`link-frequency=297000000`) | 594 Mbit/s | 1.188 Gbit/s | 2.376 Gbit/s |
| 486 MHz (overlay default) | 972 Mbit/s | 1.944 Gbit/s | 3.888 Gbit/s |

Source: [C-46], [C-48], [A-26] (reasoning).

### 3.2 Summary: lanes needed and feasibility per mode

The driver requests `DIV_ROUND_UP(payload, bit rate per lane)` lanes [C-47]. A request above the DT `data-lanes` value is rejected at STREAMON by Unicam and by CFE. A lower count, such as 3 of 4, is accepted [C-16], [C-47].

| Mode / format | Lanes requested at 972 Mbit/s [C-47] | Lanes requested at 594 Mbit/s [C-47] | 2-lane link, 972 Mbit/s [C-48] | 4-lane link, 972 Mbit/s [C-49] |
|---|---|---|---|---|
| 1080p60 UYVY | 3 | 4 | **No** (102.4%) | Yes by bandwidth, but only 3 of 4 lanes are active, and capture on 3 of 4 lanes is unproven (OQ-038) |
| 1080p60 RGB888 | 4 | 6 | **No** (153.6%) | Yes (76.8% of 3.888 Gbit/s, the highest of all six) |
| 1080p50 UYVY | 2 | 3 | Yes (85.3%) | Yes (still 2 lanes active, 85.3%) |
| 1080p50 RGB888 | 3 | 5 | **No** (128%) | Yes by bandwidth (3 of 4 lanes active), but there is an open image-corruption report (below) |
| 1080p30 UYVY | 2 | 2 | Yes (51.2%) | Yes |
| 1080p30 RGB888 | 2 | 3 | Yes (76.8%) | Yes |

At 594 Mbit/s:

- On 2 lanes, only 1080p30 UYVY fits (83.8%) [C-48].
- On 4 lanes, 1080p60 UYVY (4 lanes, 83.8%), 1080p50 UYVY and both 1080p30 formats fit. 1080p50 RGB888 and 1080p60 RGB888 are rejected [C-49].

These results match the official Raspberry Pi statement: 2 lanes support at most 1080p30 RGB888 or 1080p50 YUV422, and 4 lanes on a Compute Module support 1080p60 in either format [C-37].

**Budget per lane configuration (REQ-CAP-007).** The owner requires both a 2-lane and a 4-lane configuration, each capturing every frame rate its link carries (owner, 2026-10-07; OQ-001 ANSWERED). The platform for each configuration is not chosen (ADR-004, OPEN). Reasoning from the table above:

| Configuration | Candidate connectors | Fits at 972 Mbit/s | Does not fit | Sources |
|---|---|---|---|---|
| 2-lane | Pi 4 Model B, CM4 CAM0, or a 2-lane bridge board on any port (reasoning; OQ-021) | 1080p30 UYVY, 1080p30 RGB888, 1080p50 UYVY; 720p60 in both formats (1 and 2 lanes) | 1080p50 RGB888, 1080p60 UYVY, 1080p60 RGB888 | [C-01], [C-02], [C-37], [C-48], [B-33] |
| 4-lane | CM4 CAM1, Pi 5, CM5 | All six 1080p30/50/60 × UYVY/RGB888 combinations (1080p60 UYVY on 3 of 4 lanes, OQ-038) | — | [C-02], [C-04], [C-05], [C-37], [C-49] |

- At 594 Mbit/s the 2-lane configuration carries only 1080p30 UYVY [C-48]; the 4-lane limits at that rate are listed above [C-49].
- **HDMI sources (REQ-CAP-008).** Sources are ATEM switcher outputs and cameras connected directly (models OQ-102). The ATEM Mini Pro outputs 1080p23.98 to 1080p60 with no 720p or 1080i [F-23]. Reasoning (inputs [F-23], [C-48]): on the 2-lane configuration its 1080p59.94 and 1080p60 standards exceed the link budget, and 1080p50 fits only in UYVY. Camera output modes are UNKNOWN (OQ-102). *(Superseded in part, 2026-10-08: OQ-102 was ANSWERED by the owner on 2026-10-07. There is no model list: sources are any HDMI camera, plus ATEM switcher outputs. Camera output modes therefore stay UNKNOWN per camera. What PACSCORDER accepts is defined by the supported-mode matrix and the EDID (OQ-002). Representative cameras and an ATEM are still needed for TEST-CAP-001 and TEST-CAP-004.)*
- **EDID (reasoning; OQ-002).** Because the two configurations carry different mode sets [C-48], [C-49], the EDID may need to differ per lane configuration; see [CSI_PIPELINE.md](CSI_PIPELINE.md) §11.3.

Reasoning from [C-47] and [C-49]: a 4-lane port is necessary for 1080p60, but it is not shown to be sufficient for 1080p60 UYVY. At the default 972 Mbit/s the driver activates 3 of the 4 lanes for that mode, which is unproven (OQ-038). A 4-lane port also gives no extra per-lane margin to modes the driver packs into fewer lanes. The link-frequency choice is ADR-008 (PROPOSED): keep 486 MHz everywhere, and evaluate 297 MHz (4 active lanes for 1080p60 UYVY) only on CM4 CAM1 in TEST-CAP-002. The owner decision is OQ-099.

**Per-line check (stricter than the driver's frame-average formula)** [C-50], reasoning:

- HDMI line periods are 14.815 µs (1080p60), 17.778 µs (1080p50) and 29.630 µs (1080p30).
- At 972 Mbit/s, one active line takes:
  - on 2 lanes: 15.80 µs (UYVY) or 23.70 µs (RGB888);
  - on 4 lanes: 7.90 µs (UYVY) or 11.85 µs (RGB888).
- So 2-lane 1080p60 UYVY and 2-lane 1080p50 RGB888 cannot keep up.
- 2-lane 1080p50 UYVY has about 2.0 µs of slack per line, before LP/HS transition overhead. As reported by Raspberry Pi engineer 6by9 from his reading of Toshiba's spreadsheet (community input to [C-50]), that overhead is about 675 ns, and the minimum rate is 898.12 Mbit/s per lane — about 7.6% margin at 972 Mbit/s [C-50]. Caveat: the ~675 ns figure was reported only for 2-lane 1080p50 UYVY at 972 Mbit/s (community input to [C-50]). Its value at other lane counts or at 594 Mbit/s is not in the source register, so it must not be reused for other configurations without that caveat (DATASHEET REQUIRED; OQ-027, OQ-035).

**Known quality risk.** An open issue reports corrupted images at 1080p50 RGB24 on a 4-lane CM4, where the driver chose 3 lanes [C-43] (community). A Raspberry Pi engineer attributed it to the hard-coded FIFO trigger level of 374 (Toshiba's spreadsheet gives a minimum of 120) and to the driver's lane formula using active height instead of total line time [C-43] (RISK-006, OQ-035). ADR-005 (PROPOSED) prefers UYVY partly for this reason. Reasoning (see [CSI_PIPELINE.md](CSI_PIPELINE.md) §10.6): the officially supported 2-lane 1080p50 UYVY mode has the same per-active-lane load (85.3%) and the same per-line transmit time (15.80 µs of 17.778 µs) as that reported-corrupt 3-lane 1080p50 RGB888 case. This does not show that UYVY is affected, but 1080p50 UYVY belongs in the same test scope (TEST-CAP-004, RISK-006).

### 3.3 Per-platform link summary

| Platform / connector | Data lanes | Highest 1080p mode by bandwidth (972 Mbit/s) | Receiver limits | Sources |
|---|---|---|---|---|
| Pi 4 Model B | 2 | 1080p50 UYVY or 1080p30 RGB888. **1080p60 not feasible**; 2-lane configuration candidate only (REQ-CAP-007) | Unicam: up to 1 Gbit/s per lane (max link frequency 500 MHz). Reasoning: 972 Mbit/s is 97.2% of that | [C-01], [C-48], [C-07] |
| CM4 CAM0 | 2 | as Pi 4 Model B (2-lane configuration candidate only) | as above | [C-02], [C-48], [C-07] |
| CM4 CAM1 | 4 | 1080p60, either format, by bandwidth. 1080p60 UYVY uses 3 of 4 lanes at 972 Mbit/s, unproven (OQ-038; ADR-008). 4-lane configuration candidate | as above | [C-02], [C-37], [C-49], [C-07] |
| Pi 5 (either port) | 4 | 1080p60 by bandwidth (1080p60 UYVY: 3 of 4 lanes, OQ-038). 4-lane configuration candidate; 2-lane only with a 2-lane bridge board (OQ-021). No official TC358743 documentation exists for Pi 5 [C-38] | RP1: up to 1.5 Gbit/s per lane; two 4-lane D-PHYs shared by CSI-2 and DSI, 8 Gbit/s in total. The CFE programs its D-PHY for 999 Mbit/s for this bridge | [C-04], [C-49], [C-30], [C-31], [C-52] |
| CM5 MIPI0 / MIPI1 | 4 | as Pi 5 | as Pi 5 | [C-05], [C-49], [C-30], [C-31] |
| **PACSCORDER bridge board** | **UNKNOWN — VERIFICATION REQUIRED**; one 2-lane and one 4-lane configuration required (REQ-CAP-007); whether one board design serves both: OQ-021 | — | — | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-021, OQ-018) |

Further limits:

- The TC358743 transmitter supports up to 4 lanes at up to 1 Gbit/s per lane [A-05].
- Its HDMI input is limited to a 165 MHz TMDS clock [A-07]. The driver's DV-timings capability is 13–165 MHz [A-08].
- The 1080p60 pixel clock of 148.5 MHz [C-46] is within that range (reasoning).
- On Pi 5/CM5, the 999 Mbit/s D-PHY setting matches the default 972 Mbit/s link but not 594 Mbit/s [C-52]. This is RISK-011 and OQ-050. The link-frequency recommendation is ADR-008 (PROPOSED; OQ-099).

---

## 4. Encoder macroblock-rate budget

### 4.1 Demand per mode

A 1920×1080 frame is coded as 120 × 68 = 8,160 macroblocks [F-40], [D-36]. H.264 Table A-1 (as encoded in FFmpeg) gives a MaxMBPS of 245,760 for Level 4 and 522,240 for Level 4.2 [F-40].

| Mode | Macroblocks/s | Ratio to the Pi 4/CM4 specification (1080p30 = 244,800 MB/s [D-10], [D-52]) | Share of Level 4.2 MaxMBPS (reasoning) | Minimum H.264 level | Sources |
|---|---|---|---|---|---|
| 1080p30 | 244,800 | 1.0× | 46.9% | Level 4 | [F-40], [D-36], [D-52] |
| 1080p50 | 408,000 (reasoning: 8,160 × 50) | 1.67× (reasoning) | 78.1% | Above Level 4; Level 4.2 suffices; Level 4.1 NEEDS VERIFICATION | reasoning on [F-40] |
| 1080p60 | 489,600 | 2.0× | 93.8% | Level 4.2 | [F-40], [D-36], [D-52] |
| 720p120 (official Pi 4 recipe, `gpu_freq=550` suggested) | 432,000 | 1.76× (reasoning) | 82.7% | recipe uses 4.2 | [D-52] |

Reasoning inputs: share = macroblocks/s ÷ 522,240 [F-40]; ratio = macroblocks/s ÷ 244,800 [D-52].

**Two encodes (added 2026-10-08).** The owner requires two simultaneous H.264 encodes: a recording encode and one live encode shared by RTMP and WebRTC ("Separate record + live", OQ-005; REQ-ENC-001). Each encode's resolution and frame rate are not decided (OQ-005). The combinations below are therefore examples. **Reasoning** (inputs: 8,160 macroblocks per 1080p frame [F-40], [D-36]; 3,600 per 720p frame, 80 × 45 [D-52]; specification 244,800 MB/s [D-10], [D-52]); combined demand = sum over both encodes:

| Recording encode + live encode | Combined macroblocks/s | Ratio to the Pi 4/CM4 1080p30 specification | Applies to |
|---|---|---|---|
| 1080p30 + 1080p30 | 489,600 | 2.0× (same as one 1080p60 encode) | any configuration |
| 1080p50 + 1080p30 | 652,800 | 2.67× | 2-lane maximum UYVY rate (§3.2) with a 30 fps live encode |
| 1080p50 + 1080p50 | 816,000 | 3.33× | 2-lane maximum UYVY rate, both encodes at full rate |
| 1080p60 + 720p30 | 597,600 | 2.44× | 4-lane; lower-resolution live encode (one split named in OQ-115) |
| 1080p60 + 1080p30 | 734,400 | 3.0× | 4-lane; 30 fps live encode |
| 1080p60 + 1080p60 | 979,200 | 4.0× | 4-lane, both encodes at full rate |

- Each encode signals its own H.264 level from its own mode; the Table A-1 limits apply per bitstream, not to the sum [F-40] (reasoning). On Pi 4/CM4 the sum is what one hardware encoder has to deliver (§4.2).
- Reasoning from [F-40]: a 720p30 encode is 3,600 macroblocks per frame and 108,000 per second, exactly the Level 3.1 MaxFS and MaxMBPS. 1080p needs Level 4 or above [F-40], while libwebrtc assumes Level 3.1 when no level is signalled [F-39]. Which live resolution and level browsers accept is OQ-073 (RISK-019); which split is acceptable is OQ-115.
- A lower-resolution live encode needs scaling before the encoder. On Pi 4/CM4 the `/dev/video12` ISP can resize [D-07], [D-25]; whether it does so for this path, and at what cost, is UNKNOWN — HARDWARE TEST REQUIRED (OQ-057, OQ-115).

*(Added 2026-10-08; H.265 deferred — REQ-ENC-002; not in current scope.)* The table above is H.264 only. H.265 level and tier limits are not in the source register (NEEDS VERIFICATION), so no H.265 level row is given. No platform has a hardware HEVC encoder [D-24], [D-31], so no hardware-rate specification exists to compare an H.265 load against. The H.265 limit is CPU time on every platform (§5.3).

### 4.2 Budget per platform

- **Pi 4 / CM4 (hardware).**
  - The specified capacity is 1080p30 [D-10], and the driver calls level 4.0 the hardware specification [D-12].
  - 1080p60 needs 2.0× the specified rate, and about 1.13× the officially documented 720p120 recipe [D-52].
  - A Raspberry Pi engineer reported 1080p60 as an "edge case" on the hardware encoder [D-50] (community).
  - **The available margin is UNKNOWN — HARDWARE TEST REQUIRED** (RISK-002, OQ-056, TEST-ENC-001).
  - Whether a GPU overclock such as `gpu_freq=550` (suggested in the official 720p120 recipe [D-52]) would be acceptable in the product is OWNER DECISION REQUIRED (OQ-096).
  - A Pi 4 Model B never presents more than 1080p50 UYVY to the encoder, because of its 2 lanes [C-01], [C-48]; that is 1.67× the specification (reasoning). The same ceiling applies to any 2-lane configuration of REQ-CAP-007. On a 4-lane configuration (CM4 CAM1) 1080p60 is captured; whether it must also be encoded at that rate is REQ-ENC-001 / OQ-005.
  - *(Added 2026-10-08, two encodes.)* Both H.264 encodes, recording and live (owner, OQ-005), would run on this one encoder, which is a single V4L2 M2M device [D-06], [D-08]. Its budget is the **sum** of the two encodes (§4.1 two-encode table). Reasoning: two 1080p30 encodes already need 2.0× the specification, the same as one 1080p60 encode [D-10], [D-52]; two full-rate 1080p60 encodes need 4.0× (reasoning). How many encode sessions the encoder runs at once is not in the source register: **UNKNOWN — HARDWARE TEST REQUIRED, KERNEL SOURCE INSPECTION REQUIRED (OQ-115; RISK-002; TEST-ENC-001 two-encode run, §10.3).** If it cannot sustain both, the split (for example a lower-resolution live encode, or one encode in software, §5.2) is an owner choice in OQ-115.
- **Pi 5 / CM5 (software).** There is no hardware encoder, so no macroblock-rate specification exists; encoding runs in software [G-22], so the limit is CPU time (§5) (reasoning). *(2026-10-08, two encodes: both H.264 encodes run on this CPU; §5.1; OQ-059, RISK-003.)*
- **H.265, all platforms (added 2026-10-08; deferred — REQ-ENC-002; not in current scope).** No candidate has a hardware HEVC encoder [D-24], [D-31]. Reasoning: on Pi 4 Model B and CM4 as well, H.265 is a CPU load (§5.2, §5.3), whatever the hardware H.264 encoder's margin is.
- **Simultaneous encodes.** If recording, RTMP and WebRTC need separate encodes (OQ-005), the demand is the sum of (macroblocks per frame × frames/s) over all encodes (reasoning). Whether the Pi 4/CM4 hardware encoder can run several encode sessions at once, and at what total rate, is UNKNOWN — HARDWARE TEST REQUIRED. *(Added 2026-10-08.)* With H.264 and H.265 both required (REQ-ENC-001), an H.265 output adds a software encode on every platform. If WebRTC needs an H.264 track alongside H.265 (OQ-108), CM5 runs two software video encodes at once (reasoning; OQ-104). *(Superseded 2026-10-08, later: H.265 is deferred — REQ-ENC-002; not in current scope. In the current scope every encode is H.264 (OQ-103 ANSWERED); the H.265 cases apply only if REQ-ENC-002 is re-activated.)* *(Superseded in part, 2026-10-08, two encodes: the owner answered the "if" — there are two H.264 encodes, recording and one live encode shared by RTMP and WebRTC, not three ("Separate record + live", OQ-005). The sum rule applies to those two (§4.1). Concurrency on the Pi 4/CM4 hardware encoder is OQ-115; on Pi 5/CM5 it is OQ-059.)*

### 4.3 Bitrate, storage and network

- The Pi 4/CM4 encoder bitrate range is 25 kbit/s – 25 Mbit/s, with a default of 10 Mbit/s [D-13].
- **Reasoning (input [D-13]):** video only, without audio or container overhead:
  - at 25 Mbit/s: 25 × 10⁶ × 3600 / 8 = 11.25 GB (decimal) per hour;
  - at 10 Mbit/s: 4.5 GB per hour.
- Pi 5/CM5: the bitrate ranges of `x264enc`, `libx264` and `openh264enc` were not researched — NEEDS VERIFICATION. The storage arithmetic above depends only on the bitrate, so it applies to any encoder at the same bitrate (reasoning).
- *(Added 2026-10-08; H.265 deferred — REQ-ENC-002; not in current scope.)* H.265, all platforms: GStreamer `x265enc` takes `bitrate` in kbit/s, with a default of 2048 and a maximum of 102400 [H-14]. The `libx265` range is not in the source register — NEEDS VERIFICATION.
- *(Added 2026-10-08.)* One destination's figures, for scale only. YouTube Live recommends these 1080p60 bitrates [H-29]:
  - H.265: 4 Mbps minimum, 12 Mbps recommended;
  - H.264: 6 Mbps minimum, 17 Mbps recommended.

  **Reasoning (input [H-29]):** video only, at the recommended rates, that is 12 × 10⁶ × 3600 / 8 = 5.4 GB (decimal) per hour for H.265 and 7.65 GB per hour for H.264. These are YouTube's ingest recommendations, not PACSCORDER bitrates (OQ-005, OQ-007). *(2026-10-08, later: the H.265 figures are deferred — REQ-ENC-002; not in current scope. In the current scope only the H.264 figures apply.)*
- The PACSCORDER bitrate, storage medium and its sustained write rate, and network uplink capacity are UNKNOWN — OWNER DECISION REQUIRED / HARDWARE TEST REQUIRED (OQ-005, OQ-006, OQ-007, OQ-008).
- *(Added 2026-10-08, two encodes; reasoning.)* With a recording encode and a live encode (OQ-005), the two bitrates are separate. Storage depends only on the recording encode's bitrate, using the arithmetic above. Network load depends on the live encode's bitrate and on how RTMP and WebRTC deliver it (OQ-007, OQ-008). Both bitrates are UNDEFINED (OQ-005). Whether the Pi 4/CM4 range of [D-13] applies to each encode session separately is not in the source register (OQ-115).

---

## 5. CPU budget

### 5.1 Pi 5 / CM5

The **only official figure** is: **"H264 1080p30 encode (from ISP) ~30–40% CPU"** on BCM2712 [G-22], [D-30].

Everything else is UNKNOWN — HARDWARE TEST REQUIRED.

| Load component | Budget | Status / resolution |
|---|---|---|
| H.264 encode, 1080p30 | ~30–40% CPU (official, "from ISP") [G-22] | Whether this means all CPU cores together or one core: UNKNOWN — VENDOR CONFIRMATION REQUIRED (OQ-059). The BCM2712 core count is not in the source register. Whether it applies to TC358743 input: UNKNOWN — HARDWARE TEST REQUIRED. Reasoning: that input is not expected to come "from ISP", because Raspberry Pi engineers reported that libcamera does not support the bridge [C-41]. *(2026-10-08: the figure is for one encode; PACSCORDER requires two — see the simultaneous-encodes row)* |
| H.264 encode, 1080p50 / 1080p60 | UNKNOWN | HARDWARE TEST REQUIRED (OQ-059). A Raspberry Pi engineer reported 1080p60 software encode from camera capture as "easily achievable" [D-50] (community); this is not a PACSCORDER budget |
| **H.265 encode (x265), 1080p30 / 1080p50 / 1080p60** (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | **UNKNOWN.** No official figure was found [H-19]. Summary of §5.3: reported by a Raspberry Pi engineer as "too intensive an operation to perform at any significant resolution" [H-19] (community); a community benchmark reports 10.00 FPS for `libx265` "Live" on Pi 5 in a test that is not a 1080p60 live measurement [H-20], [H-22] (community) | HARDWARE TEST REQUIRED (OQ-104; SIMD paths OQ-105; RISK-022). Details and caveats in §5.3 |
| UYVY → I420/NV12 (or other planar) conversion per frame | UNKNOWN | Required because `x264enc`, `libx264` and `openh264enc` do not accept packed UYVY [D-40], [D-41], [D-43]. Hardware offload: UNKNOWN (OQ-060). *(Added 2026-10-08; H.265 deferred — REQ-ENC-002; not in current scope.)* For H.265 the output must be planar (I420 or Y42B), because `x265enc` and `libx265` accept neither UYVY nor NV12 [H-10] (CORRECTED), [H-13]. Memory traffic at 1080p60 to I420: about 249 MB/s read and 187 MB/s written, before x265 starts (reasoning [H-43]; other modes in [DMA.md](DMA.md) §8A). CPU time: still UNKNOWN. *(Added 2026-10-08, two encodes; reasoning.)* One conversion can feed both H.264 encodes only if both take the same format and resolution; I420 suits `x264enc`, `libx264` and `openh264enc` [D-40], [D-41], [D-43]. A lower-resolution live encode needs its own scaled frames. Whether one conversion is shared: ADR-007, OQ-060 |
| V4L2 capture handling | UNKNOWN | HARDWARE TEST REQUIRED |
| Additional simultaneous encodes (recording + RTMP + WebRTC) | UNKNOWN | HARDWARE TEST REQUIRED (OQ-005, OQ-059). *(Added 2026-10-08.)* This includes H.264 and H.265 running together (OQ-103, OQ-104). If WebRTC keeps an H.264 track beside H.265, that is two software video encodes on this CPU (OQ-108). *(Superseded 2026-10-08, later: OQ-103 ANSWERED — H.264 only; H.264 + H.265 concurrency applies only if REQ-ENC-002 is re-activated. Several simultaneous H.264 encodes remain OQ-005, OQ-059.)* *(Superseded in part, 2026-10-08, two encodes: the owner chose two H.264 encodes — recording, and one live encode shared by RTMP and WebRTC ("Separate record + live", OQ-005). Both are software encodes on this CPU. Reasoning from [G-22]: roughly double the encode CPU of one encode, that is about 60–80 % for two 1080p30 encodes in the unit of the official figure, if the cost scales linearly with the number of encodes, which no source establishes. Whether that unit is all cores or one core decides how much headroom remains (OQ-059). Two encodes at 1080p50 or 1080p60: UNKNOWN. HARDWARE TEST REQUIRED (OQ-059; RISK-003; TEST-ENC-001 two-encode run and TEST-PERF-001, §10.3, §10.4).)* |
| Muxing, RTMP, WebRTC (RTP, ICE, DTLS/SRTP) | UNKNOWN | HARDWARE TEST REQUIRED; see [STREAMING.md](STREAMING.md) |
| Audio capture and encode | UNKNOWN | OQ-004, OQ-063. *(Superseded in part, 2026-10-08: audio is required, OQ-004 ANSWERED. The cost is still UNKNOWN — HARDWARE TEST REQUIRED, OQ-063.)* Encoders available: FFmpeg native `aac` and `libopus` [I-39], [I-41]; GStreamer `voaacenc`, `avenc_aac` and `opusenc` [I-44], [I-45], [I-46]. Reasoning (as recorded in OQ-063, from [H-26] and [F-41]): RTMP from FFmpeg and WebRTC together need two audio encodes, AAC and Opus. `opusenc` accepts only 48, 24, 16, 12 or 8 kHz, so a 44.1 kHz source needs resampling first [I-47]; that resampling is an extra CPU load (reasoning; rate handling OQ-111). *(2026-10-08: unchanged by the two-video-encode decision — still two audio encodes, AAC for recording and RTMP and Opus for WebRTC [F-31], [F-41]; OQ-063.)* |
| ATEM integration | No separate load expected (reasoning): the ATEM integration is HDMI capture of the ATEM output only (REQ-ATEM-001, REQ-CAP-008; owner 2026-10-07), which is the capture load above | OQ-009 ANSWERED. Network tally/control and RTMP exchange are not in current scope; they would add load only if the owner adds them |
| **Total and remaining headroom** | **UNKNOWN** | TEST-ENC-001, TEST-PERF-001. *(2026-10-08: with both H.264 encodes running; OQ-059)* |

### 5.2 Pi 4 / CM4

| Load component | Budget | Status / resolution |
|---|---|---|
| H.264 encode | Done by the VideoCore firmware through `ril.video_encode` [D-08]. The Arm CPU cost of driving it is UNKNOWN. *(2026-10-08, two encodes: both the recording and the live encode would run on this encoder (OQ-005); the cost of driving two encode sessions, and whether the encoder runs both, are UNKNOWN — OQ-115, RISK-002)* | HARDWARE TEST REQUIRED |
| UYVY input | A Raspberry Pi engineer reported it as accepted directly by the encoder [D-18] (community). Otherwise the `/dev/video12` ISP M2M device can convert [D-25]. CPU cost UNKNOWN | HARDWARE TEST REQUIRED (OQ-057) |
| Raw-frame copy into the encoder | None if capture buffers are imported as DMABUF [D-19], [D-34]. One copy per frame if upstream FFmpeg `h264_v4l2m2m` is used, because it is MMAP-only [D-44]. CPU cost UNKNOWN. *(2026-10-08, two encodes; reasoning: with an MMAP-only path the copy would occur once per encode, so twice per frame; with DMABUF import both encodes could import the same capture buffer — [DMA.md](DMA.md) §9.1, OQ-058)* | HARDWARE TEST REQUIRED (OQ-058, ADR-007) |
| **H.265 encode (software x265 on the BCM2711 Cortex-A72)** (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | **UNKNOWN.** The hardware encoder has no HEVC [D-24], so H.265 is a CPU load here too (reasoning). The Cortex-A72 has no DotProd, I8MM, SVE or SVE2 [H-05]. The only figure is community-reported: 4.33 FPS for `libx265` "Live" on a Pi 400 (BCM2711, Cortex-A72 @ 1.80 GHz) in a test that is not a 1080p60 live measurement [H-21], [H-22]. The CM4's clock is not in the source register, so that figure cannot be carried over to CM4 | HARDWARE TEST REQUIRED (OQ-104, OQ-105; RISK-022). §5.3 |
| UYVY → planar conversion for H.265 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | Needed here too: `x265enc` and `libx265` do not accept UYVY [H-10] (CORRECTED), [H-13]. It is not needed for the hardware H.264 path (row above). On the CPU at 1080p60: about 249 MB/s read and 187 MB/s written (reasoning [H-43]). Whether the `/dev/video12` ISP can do it instead is UNKNOWN (OQ-057). CPU time UNKNOWN | HARDWARE TEST REQUIRED (OQ-057, OQ-104) |
| Software x264 fallback on the BCM2711 Arm CPU (if hardware cannot reach the required rate) | No official figure (research gap, topic D). The CPU core type and count are not in the source register. *(Superseded in part, 2026-10-08: the core type is now in the register, Cortex-A72 [H-05]. An official core count is still not in it.)* *(2026-10-08, two encodes: OQ-115 names "one encode in software" as one possible split if the hardware encoder cannot run both encodes; that would put one H.264 encode, plus a UYVY → planar conversion [D-43], on this CPU. Cost UNKNOWN)* | UNKNOWN — HARDWARE TEST REQUIRED (OQ-056, OQ-115) |
| Streaming, audio | UNKNOWN | as for Pi 5 / CM5 (*2026-10-08:* audio is required; encoders and the two-encode reasoning are in the §5.1 audio row; CPU cost OQ-063) |
| ATEM integration | No separate load expected (reasoning; HDMI capture only, §5.1) | OQ-009 ANSWERED |
| **Total and remaining headroom** | **UNKNOWN** | TEST-ENC-001, TEST-PERF-001. *(2026-10-08: with both H.264 encodes on the hardware encoder; OQ-115)* |

### 5.3 H.265 (HEVC) budget line — CM4 and CM5 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope)

**Scope (2026-10-08, later): deferred — REQ-ENC-002; not in current scope.** The owner answered OQ-103 with "H.264 only for now": every output uses H.264 (REQ-ENC-001), and H.265 is recorded as REQ-ENC-002 (DEFERRED; no tests planned). This section is therefore not a current-scope budget. It is kept unchanged as the evidence for REQ-ENC-002 and applies only if the owner re-activates it. OQ-104, OQ-105 and RISK-022 stay OPEN but are not in current scope.

H.265 is required (owner, 2026-10-07; REQ-ENC-001). Which outputs use it is OQ-103. *(Superseded 2026-10-08, later — see the scope note above.)* Bring-up evaluates CM4 and CM5 side by side (ADR-004 OPEN). This section collects the H.265 inputs per board. **It contains no budget value, because no source gives one** (Rules 8 and 22: never guess).

| Item | CM4 (BCM2711; also Pi 4 Model B, reasoning) | CM5 (BCM2712; also Pi 5, reasoning) | Sources |
|---|---|---|---|
| Hardware HEVC encoder | None | None | [D-24], [D-31] |
| Encoder | x265 4.1-2 from Debian, through `x265enc` or `libx265` | same | [H-01], [H-02], [H-09], [H-11] |
| CPU core and x265 SIMD | Cortex-A72: no DotProd, I8MM, SVE or SVE2 | Cortex-A76: Neon DotProd kernels can apply; I8MM, SVE and SVE2 cannot. Active on the image: OQ-105 | [H-04], [H-05] |
| Input conversion before x265 | UYVY → planar (I420/Y42B); ≈ 249 MB/s read + ≈ 187 MB/s written at 1080p60 (reasoning) | same | [H-10], [H-13], [H-43] |
| Low-latency settings | `tune=zerolatency`: B-frames 0, lookahead 0, one frame thread, only WPP row parallelism | same | [H-16] |
| Only figure found (community; not a 1080p60 live measurement) | 4.33 FPS `libx265` "Live" on a Pi 400 (Cortex-A72 @ 1.80 GHz) | 10.00 FPS `libx265` "Live" on Pi 5 (Cortex-A76 @ 2.40 GHz); `libx264` 66.17 FPS in the same test | [H-20], [H-21], [H-22] |
| Relative cost, same harness (reasoning) | — | `libx265` ≈ 6.6 × slower than `libx264` | [H-23] |
| Raspberry Pi statement (community) | — | 6by9 (forum, 2024): "too intensive an operation to perform at any significant resolution" | [H-19] |
| Official figure | None found | None found | [H-19] |
| Concurrent H.264 | On the hardware encoder; it does not use the CPU budget for x265 (reasoning; driving cost UNKNOWN, §5.2) | Software, on the same CPU (reasoning; §5.1) | [D-10], [G-22], [D-31] |
| **H.265 CPU budget at 1080p30 / 50 / 60** | **UNKNOWN — HARDWARE TEST REQUIRED** | **UNKNOWN — HARDWARE TEST REQUIRED** | OQ-104, RISK-022 |
| Thermal / throttling under sustained all-core load | UNKNOWN | UNKNOWN | OQ-104, §8 |

Rules for using these inputs (Claude's reasoning):

- **Do not derive an H.265 CPU budget by multiplying [G-22] ("H264 1080p30 encode (from ISP) ~30–40% CPU") by [H-23] (≈ 6.6; a reasoning-tier entry).** The ratio comes from a different harness (vbench clips, `-threads 1`, a 2022 x265 snapshot), as reported for the community benchmark [H-22]. Whether the percentage means all cores or one core is unknown (OQ-059). Research notes that the two readings give opposite feasibility conclusions (research open question, topic H).
- Do not carry the Pi 400 figure over to CM4. The community benchmark reports its clock as 1.80 GHz [H-21], and the CM4's clock is not in the source register.
- The community figures were measured with an x265 snapshot that predates x265 4.0's Arm optimisations [H-22], [H-06], while trixie ships 4.1 [H-01]. The x265 4.2 and 4.3 AArch64 speed-ups are not in trixie [H-07]. A measured value therefore holds only for the x265 version recorded with it (§9 "SW version").
- What the evidence suggests (reasoning, not a measurement; same reading as RISK-022): real-time 1080p60 H.265 in software is doubtful on CM5 and more so on CM4. It stays UNKNOWN until TEST-ENC-001 runs with H.265 (§10.3). *(2026-10-08, later: those H.265 runs are deferred, not run in current scope — REQ-ENC-002.)*

---

## 6. Memory and CMA budget

### 6.1 Frame and buffer sizes

| Item | Size | Source | Notes |
|---|---|---|---|
| One 1920×1080 UYVY frame | 4,147,200 bytes | [C-53] | |
| One 1920×1080 RGB888 / BGR3 frame | 6,220,800 bytes | [C-53] | |
| Four UYVY capture buffers | ≈ 16.6 MB (16,588,800 bytes) | [C-53] (CORRECTED verdict) | Four is the example count in [C-53]. The PACSCORDER count is UNKNOWN (set by the application, ADR-007) |
| Four RGB888 capture buffers | ≈ 24.9 MB (24,883,200 bytes) | [C-53] | as above |
| Pi 4/CM4 encoded (CAPTURE) buffer | 768 KiB each above 720p; a larger `sizeimage` can be requested | [D-23] | Buffer count UNKNOWN. *(2026-10-08, two encodes; reasoning: one set per encode, recording and live — see [DMA.md](DMA.md) §4.3)* |
| Pi 4/CM4 encoder raw (OUTPUT) buffers | No extra buffers when capture buffers are imported as DMABUF (reasoning from [D-19], [D-34]); otherwise frame-sized MMAP buffers allocated through `videobuf2-dma-contig` [D-19] | [D-19] | Count and pool: NEEDS VERIFICATION |
| Pi 5/CM5 conversion output buffers (planar format) | UNKNOWN | — | Depend on the format and framework (OQ-060, ADR-007). *(2026-10-08, two encodes; reasoning: one set if one conversion feeds both encodes, two if each encode converts or scales separately, §5.1)* |
| One 1920×1080 I420 (planar 4:2:0) frame, the conversion output for x265 — added 2026-10-08 | 3,110,400 bytes (1920 × 1080 × 1.5) | [H-43] (reasoning) | Needed for H.265 on **every** platform, Pi 4/CM4 included [H-10], [H-13]. Buffer count and allocator (CMA or not) UNKNOWN (ADR-007; OQ-061). *(2026-10-08, later: H.265 is deferred — REQ-ENC-002; not in current scope. Reasoning: the frame size depends only on the format, so it also applies on Pi 5/CM5 if the conversion for software H.264 outputs I420, which `x264enc`, `libx264` and `openh264enc` accept [D-40], [D-41], [D-43]; see the row above.)* |
| `vc-sm-cma` shared memory, pulled in by the codec drivers | UNKNOWN | [D-03] | OQ-061 |

### 6.2 CMA pool

| Setting | Value | Source |
|---|---|---|
| Kernel defconfig | `CONFIG_CMA=y`, `CONFIG_DMA_CMA=y`, `CONFIG_CMA_SIZE_MBYTES=5` | [E-47], [G-19] |
| Device-tree CMA pool (`linux,cma`, used instead of the defconfig size) | 64 MB = `0x4000000` bytes | [E-47] (CORRECTED verdict) |
| Allocation range | Pi 4/CM4: lower 768 MB (`bcm2711-rpi-ds.dtsi` overrides the 1 GB of `bcm2711.dtsi`). Pi 5/CM5: lower 1 GB | [E-47] |
| With `vc4-kms-v3d-pi4` | (512 − 4) MB | [C-40] |
| With `vc4-kms-v3d-pi5` | 64 MB | [C-40] |
| Generic `cma` overlay default | 256 MB | [C-40] |
| Size parameters | `cma-64` … `cma-512` and `cma-size` (bytes, 4 MB aligned); `cma-192` and above need 1 GB | [C-40], [E-47] |

**Reasoning (inputs: [C-53] buffer sizes; 64 MB = 67,108,864 bytes [E-47]):**

- Four UYVY capture buffers take 24.7% of a 64 MB pool.
- Four RGB888 capture buffers take 37.1%.
- On Pi 4/CM4 this memory comes from CMA, because Unicam uses `videobuf2-dma-contig` and has no IOMMU [C-53].

UNKNOWN — VERIFICATION REQUIRED:

- **Whether Pi 5/CM5 CFE capture buffers count against CMA at all.** The RP1 CSI nodes sit behind `iommu5` [C-53]. KERNEL SOURCE INSPECTION REQUIRED and HARDWARE TEST REQUIRED (OQ-053).
- Which overlay sets CMA on the PACSCORDER image, and whether the firmware changes it at boot. The verifier of [E-47] did not establish this. HARDWARE TEST REQUIRED (OQ-061). The product runs its own project-built OS image (REQ-BLD-002); the build tool is `rpi-image-gen` (ADR-003, ACCEPTED).
- The total CMA need at full load (capture + encoder + display, if any). HARDWARE TEST REQUIRED (OQ-061, RISK-020). *(2026-10-08: "encoder" now means both H.264 encodes, recording and live (OQ-005); capture buffers shared by two encodes may also stay in flight longer, [DMA.md](DMA.md) §2.)*
- On Pi 4/CM4, the `gpu_mem` value the codec needs. Note that `gpu_mem=16` selects the cut-down firmware, which has no codecs [D-48]. VENDOR CONFIRMATION REQUIRED and HARDWARE TEST REQUIRED (OQ-048).
- The DMA-BUF heap names on the chosen kernel [E-48], [G-19]. HARDWARE TEST REQUIRED (OQ-062). See [DMA.md](DMA.md).

Measurement method: read the `CmaFree` field of `/proc/meminfo` while streaming at maximum load [C-53] (TEST-PERF-001, §10.4). *(2026-10-08: maximum load includes both encodes running.)*

---

## 7. Latency budget

**End-to-end latency target: UNDEFINED — OWNER DECISION REQUIRED (OQ-005, OQ-008).** Without a target, the stages below cannot be judged; they can only be listed and measured.

*(Added 2026-10-08, two encodes; reasoning.)* With a recording encode and a live encode (owner, OQ-005), the glass-to-glass target concerns the live encode (RTMP and WebRTC), and the capture-to-file target the recording encode. The encode stages below are measured per encode.

| Stage | Known value or bound | Source | Status |
|---|---|---|---|
| HDMI signal-change detection, no interrupt wired | Without an IRQ, the driver polls the TC358743 interrupt status over I2C every 1000 ms (every 10 ms if a CEC adapter is registered). The stock Raspberry Pi overlay has no `interrupts` property | [A-30], [B-19], [C-20] | Up to about 1 s (RISK-013) |
| CEC fast polling | Not available by default: the Raspberry Pi defconfigs do not enable `CONFIG_VIDEO_TC358743_CEC` | [C-20], [E-39] | — |
| Signal-change detection, interrupt wired | INT is active-high, level-triggered [A-29]; the driver uses a threaded IRQ when the I2C client has one [B-19] | [A-29], [B-19] | PACSCORDER INT wiring: UNKNOWN — VERIFICATION REQUIRED (OQ-020) |
| Event delivery on Pi 5/CM5 | Reported by a Raspberry Pi engineer: subscribe to source-change events on the TC358743 sub-device, not on the video node [C-42] (community) | [C-42] | OQ-051 |
| Pipeline reconfiguration after a change | The driver sends `V4L2_EVENT_SOURCE_CHANGE` but does not apply new timings itself [B-39] | [B-39] | Application time UNKNOWN |
| One frame period (reasoning: 1 / frame rate, rates from [C-46]) | 16.7 ms at 1080p60; 20.0 ms at 1080p50; 33.3 ms at 1080p30 | [C-46] | — |
| Capture queue depth | UNKNOWN (number of buffers in flight, ADR-007) | — | HARDWARE TEST REQUIRED |
| Encode, Pi 4/CM4 hardware | No source figure. No B-frame reordering, because the encoder produces no B-frames [D-14]. *(2026-10-08, two encodes: whether two concurrent encode sessions change the live encode's per-frame latency is UNKNOWN, OQ-115)* | [D-14] | UNKNOWN — HARDWARE TEST REQUIRED |
| Encode, Pi 5/CM5 software | Official: software encoders "generally output frames with a longer latency than the old hardware encoders". Low-latency mode drops B-frames and arithmetic coding. *(2026-10-08, two encodes: the live encode carries no B-frames, because the MediaMTX project reports that browsers do not accept them in WebRTC [F-45] (community source); the two software encodes share the CPU, and their effect on the live encode's latency is UNKNOWN, OQ-059)* | [D-32], [G-22] | UNKNOWN — HARDWARE TEST REQUIRED (OQ-059) |
| `rpicam-apps` low-latency `libx264` reference settings | `ultrafast`, `zerolatency`, 4 slices, `refs=1`, `rc-lookahead 0` | [D-35] | Reference only |
| Encode, H.265 software (x265), all platforms — added 2026-10-08; deferred — REQ-ENC-002; not in current scope | `tune=zerolatency` sets B-frames 0, lookahead 0 and one frame thread [H-16]. Without it, x265 4.1 `ultrafast` still uses 3 B-frames and a 5-frame lookahead [H-17]. GStreamer 1.26.2 `x265enc` *reports* a hard-coded 5-frame latency unless `tune=zerolatency` (then 0) [H-15]. Reasoning: 5 frames are 83.3 ms at 1080p60, 100 ms at 1080p50 and 166.7 ms at 1080p30, using the frame periods above. In FFmpeg the thread count replaces zerolatency's single frame thread unless it is set explicitly [H-10] (CORRECTED), [H-16] | [H-10], [H-15], [H-16], [H-17] | UNKNOWN — HARDWARE TEST REQUIRED (OQ-104) |
| UYVY → planar conversion before software encode — added 2026-10-08 | A further CPU stage before x265 on every platform, and before x264 on Pi 5/CM5 [H-10], [H-13], [D-43]. Its time per frame is not in the source register. *(2026-10-08, later: the x265 case is deferred — REQ-ENC-002; in the current scope this stage applies before software H.264 on Pi 5/CM5.)* | [H-43] (traffic, reasoning) | UNKNOWN — HARDWARE TEST REQUIRED (OQ-060, OQ-104) |
| Capture and audio timestamps (A/V alignment) — added 2026-10-08 | Every Raspberry Pi CSI receiver driver stamps buffers with `CLOCK_MONOTONIC` at frame start [I-33]. alsa-lib 1.2.14 switches newly opened `hw` PCMs to monotonic timestamps on kernel PCM protocol 2.0.9 or later [I-34]. In GStreamer 1.26.2, `alsasrc` normally becomes the pipeline clock in a `v4l2src` + `alsasrc` pipeline and then does not use ALSA driver timestamps [I-35], [I-36]. `v4l2src` maps buffer timestamps through a measured delay [I-37]. In FFmpeg 7.1, the ALSA input uses wall-clock time and the V4L2 input uses monotonic time by default [I-38] | [I-33] to [I-38] | A/V offset and drift: UNKNOWN — HARDWARE TEST REQUIRED (OQ-112, RISK-024); tolerance UNDEFINED (OQ-112) |
| Muxing, network, server, player / browser | UNKNOWN | — | See [STREAMING.md](STREAMING.md), [RECORDING.md](RECORDING.md) |
| **End-to-end** | **UNDEFINED target; UNKNOWN value** | — | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |

---

## 8. Thermal

| Item | Value | Source | Status |
|---|---|---|---|
| TC358743XBG operating temperature | Ta −30 to +70 °C (ambient, voltage applied). TC9590XBG, a different part: −40 to +85 °C. Storage: −40 to +125 °C | [A-41] | Datasheet value |
| TC358743 typical power | 480.5 mW at 720p60; 543.2 mW at 1080p60 | [A-38] | Datasheet value |
| Raspberry Pi SoC temperature limits, throttling behaviour, cooling requirements (Pi 4 Model B, CM4, Pi 5, CM5) | **UNKNOWN — VERIFICATION REQUIRED.** No fact in the source register covers them | — | VENDOR CONFIRMATION REQUIRED (official Raspberry Pi documentation to be researched); HARDWARE TEST REQUIRED |
| Thermal effect of Pi 5/CM5 software encoding | UNKNOWN. RISK-003 lists CPU and thermal load at 1080p60 as an impact; software encoding is CPU work [G-22]. *(2026-10-08: two concurrent software encodes, recording and live (OQ-005), so roughly double the encode CPU work, reasoning from [G-22])* | [G-22] | HARDWARE TEST REQUIRED (OQ-059) |
| Thermal effect of software H.265 encoding, CM4 and CM5 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | UNKNOWN. H.265 is CPU work on every platform, because there is no hardware HEVC encoder [D-24], [D-31] (reasoning). OQ-104 asks whether CM5 throttles under sustained all-core load. A Raspberry Pi engineer's statement that software H.265 encode is "too intensive" [H-19] is community evidence, not a thermal figure | [D-24], [D-31], [H-19] | HARDWARE TEST REQUIRED (OQ-104) |
| Product ambient range, enclosure, airflow | UNDEFINED | — | OWNER DECISION REQUIRED (OQ-010) |
| Product power input and budget | UNKNOWN — VERIFICATION REQUIRED | — | OQ-023 |
| D-PHY timing and FIFO-level validity across temperature | UNKNOWN | [B-12], [C-43] | HARDWARE TEST REQUIRED (OQ-035) |

The command used to read SoC temperature and throttling state is not attested in the source register: NEEDS VERIFICATION (OQ-101). It must be chosen and documented in [TESTING.md](TESTING.md) before TEST-PERF-001 runs.

---

## 9. Measurements

**No measurements exist.** As of 2026-10-06 there is no PACSCORDER hardware, no code, and no test has been run. Every value in §3–§8 is a calculation or a source statement, not a measurement. *(Re-checked 2026-10-08: still no measurements. The H.265 inputs added in §5.3 are source statements and community reports, not measurements.)*

Add one row per measured value. Never delete or rewrite a row (Rule 21); add a correcting row instead.

| Date | Platform | HW rev | SW version | Test ID | Metric | Value | Notes |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | No measurements exist (no hardware as of 2026-10-06). |

Column rules:

- **Platform**: board and connector or carrier, and the lane configuration of REQ-CAP-007 (2-lane or 4-lane), for example "CM4 on <carrier>, CAM1, 4-lane".
- **HW rev**: PACSCORDER hardware revision. None exists yet (OQ-018).
- **SW version**: image, kernel version, firmware version, and the versions of the framework and encoder packages. *(Added 2026-10-08.)* For H.265 runs, also the x265 version and the CPU capabilities x265 reports (DotProd on CM5; OQ-105). *(H.265 runs are deferred — REQ-ENC-002; this applies only if it is re-activated.)*
- **Test ID**: one of the canonical IDs in [README.md](README.md).
- **Notes**: include at least the input mode, pixel format, active lanes, link frequency, encoder and settings, duration, ambient temperature and cooling. *(Added 2026-10-08.)* Also the codec (H.264 or H.265), preset, tune and thread settings, the conversion path (CPU or hardware) and target format, any concurrent encodes, and whether audio was being captured and encoded. *(Added 2026-10-08, two encodes.)* For two-encode runs, record each encode (recording, live) on its own row: its mode, settings, bitrate and output rate.

---

## 10. Measurement procedures — NOT YET RUN ON PACSCORDER HARDWARE

These procedures define *what* to measure for the budgets above. The authoritative test steps, pass criteria and results belong in [TESTING.md](TESTING.md).

### 10.1 General rules

- Only commands attested in the source register are written out. Where no command is attested, the step says **NEEDS VERIFICATION**. The tool must be chosen and documented in [TESTING.md](TESTING.md) before the run. This applies to:
  - frame-rate counting (streaming and frame counting are listed in OQ-101);
  - CPU sampling (OQ-101);
  - SoC temperature and throttling readout (OQ-101);
  - device enumeration (OQ-101);
  - pointing `v4l2-ctl` at a sub-device node with `-d <sub-device path>` (OQ-101; only `-d 11` [D-17] and running the step "on /dev/v4l-subdevN" [C-33] are attested);
  - reading the kernel, firmware and EEPROM versions for the §9 "SW version" column (OQ-101).
- Measure each platform and each lane configuration under evaluation separately (ADR-004, OQ-011; REQ-CAP-007 requires both a 2-lane and a 4-lane configuration). Never assume that results carry over between the 2-lane and the 4-lane configuration, or between Pi 4 Model B and CM4, or between Pi 5 and CM5, or between OS images, for example 16K-page and 4K-page kernels (OQ-055). *(Added 2026-10-08.)* The owner's bring-up pair is CM4 and CM5, side by side (ADR-004 OPEN until measured), so each procedure below is run on both boards. Also never assume that H.265 results carry over between x265 versions [H-07], or between GStreamer `x265enc` and FFmpeg `libx265`, whose thread and latency handling differ [H-10], [H-15]. *(2026-10-08, later: H.265 runs are deferred — REQ-ENC-002; this rule applies only if it is re-activated.)*
- Record every result in §9 and in [TESTING.md](TESTING.md). Then update the affected OQ, RISK and REQ entries as their own rules require.

### 10.2 TEST-CAP-002 — capture rate and link budget (REQ-CAP-001, REQ-CAP-007)

NOT YET RUN ON PACSCORDER HARDWARE. Status: BLOCKED — HARDWARE REQUIRED.

Scope (aligned with [TESTING.md](TESTING.md) TEST-CAP-002): this test covers the 4-lane configuration of REQ-CAP-007, where 1080p60 is required. The 2-lane configuration cannot carry 1080p60 [C-37], [C-48]; its rates (up to 1080p50 UYVY / 1080p30 RGB888) are measured in the TEST-CAP-004 supported-mode matrix with the same metrics as below.

**Preconditions**

1. A 4-lane connection for 1080p60 (CM4 CAM1 [C-02], Pi 5 [C-04] or CM5 [C-05]). This is necessary, not shown sufficient: at 972 Mbit/s 1080p60 UYVY runs on 3 of the 4 lanes, which this test must establish (OQ-038; §3.2). The lanes routed by the PACSCORDER board are UNKNOWN (OQ-021).
2. EDID written to the TC358743 with `v4l2-ctl --set-edid` [C-37]. The option syntax is `--set-edid pad=<pad>[,type=<type>|file=<file>][,format=<fmt>]` [B-24]. The EDID content is an owner decision (OQ-002); it may need to differ per lane configuration (reasoning, §3.2).
3. Timings applied with `v4l2-ctl --set-dv-bt-timings query` [C-37]. In Media Controller mode, the EDID and DV-timings ioctls are not available on the video node, so both steps 2 and 3 must be issued on the TC358743 sub-device node (`/dev/v4l-subdevN`) [B-25], [C-36] (CORRECTED verdicts). Media Controller mode is the only mode on Pi 5/CM5 [C-11] and is PROPOSED for Pi 4/CM4 (ADR-006). For Pi 5, Raspberry Pi engineer 6by9 reported the same sub-device step [C-33] (community). The sub-device number on PACSCORDER is UNKNOWN (OQ-043). How `v4l2-ctl` is pointed at the sub-device node (the `-d <sub-device path>` form) is not attested in the register: NEEDS VERIFICATION (OQ-101). Only `-d 11` [D-17] and running the step "on /dev/v4l-subdevN" [C-33] are attested.
4. Pi 5/CM5: configure media links and pad formats with `media-ctl`. As reported by Raspberry Pi engineer 6by9 for Pi 5 on kernel 6.18.39 [C-33] (community):

   ```bash
   media-ctl -d 2 -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'
   media-ctl -d 2 -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
   ```

   - 6.18.39 is only the kernel of that report. The 2026-10-06 Raspberry Pi OS image ships 6.18.50 [G-04], and the driver source was inspected at the `rpi-6.18.y` tip 6.18.55 [E-37]; whether the driver differs between those two is OQ-097.
   - Set matching formats on `"tc358743 1x-000f":0`, `"csi2":0` and `"csi2":4`. Leaving out `field:none` on the csi2 pads was reported to make STREAMON fail with `-EPIPE` [C-33].
   - The media device number (`-d 2`) and the entity names on PACSCORDER are UNKNOWN (OQ-043).
   - Pi 4/CM4 in Media Controller mode: the source register has no attested `media-ctl` sequence. Unicam registers only the `unicam-image` node, linked directly from the TC358743 pad with an IMMUTABLE|ENABLED link [C-36] (CORRECTED verdict). Which pad formats must be set is NEEDS VERIFICATION (OQ-044, OQ-046).
   - The full sequence belongs in [CSI_PIPELINE.md](CSI_PIPELINE.md) and [V4L2.md](V4L2.md).
5. Link frequency at the default 972 Mbit/s, as ADR-008 (PROPOSED; OQ-099) proposes. On Pi 5/CM5, do not use 594 Mbit/s (RISK-011, [C-52]).
6. HDMI source: each required source model that outputs the mode under test, ATEM output or camera (REQ-CAP-008; models OQ-102). *(Superseded in part, 2026-10-08: OQ-102 ANSWERED. There is no model list; sources are any HDMI camera plus ATEM outputs. Use representative cameras and an ATEM that output the mode under test.)*

**Runs**

- 1080p60, 1080p50 and 1080p30 in UYVY (ADR-005, PROPOSED).
- RGB888 only if ADR-005 is rejected.
- CM4 CAM1 4-lane only: 1080p60 UYVY also at `link-frequency=297000000` (594 Mbit/s, 4 active lanes), the evaluation that ADR-008 (PROPOSED) proposes (OQ-099). Not on Pi 5/CM5 (precondition 5).

**Metrics**

- Delivered frames per second compared with the source rate.
- Missing or dropped frames.
- Visible corruption (RISK-006).
- CSI-2 receiver errors, where the receiver reports them (method NEEDS VERIFICATION: RP1 CFE OQ-050; Unicam OQ-095).
- Configured `data-lanes`.

**Expected from sources (not results)**

- §3.2.
- On a 2-lane connection at 972 Mbit/s, 1080p60 UYVY should be rejected at STREAMON with "Device has requested 3 data lanes, which is >2 configured in DT", and 1080p60 RGB888 with "requested 4 data lanes" (reasoning from [C-16], [C-47]). That check belongs to the 2-lane configuration in TEST-CAP-004.

**Duration and drop threshold:** UNDEFINED (OQ-010).

### 10.3 TEST-ENC-001 — Sustained real-time H.264 encode (H.265 deferred) (REQ-ENC-001)

NOT YET RUN ON PACSCORDER HARDWARE. Status: BLOCKED — HARDWARE REQUIRED.

**Preconditions**

- TEST-CAP-002 has a recorded result of TESTED — PASS for the mode under test. TEST-CAP-002 cannot run on a 2-lane connector; there, as for TEST-CAP-003, a passing lower-rate capture of the mode under test is needed instead (same wording as [TESTING.md](TESTING.md) TEST-ENC-001).
- TEST-DMA-001 has been run, because [TESTING.md](TESTING.md) lists it as a dependency of TEST-ENC-001.
- The encoder settings are recorded: profile, level (set from the mode, see [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §6), bitrate and mode, GOP, repeat-header setting. *(Added 2026-10-08, two encodes.)* In two-encode runs they are recorded for each encode. The live encode uses the PROPOSED WebRTC settings of [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §7: Constrained Baseline [F-36], [F-39], a level set from its mode (Level 4 or above at 1080p [F-40]), no B-frames (reported by the MediaMTX project as a browser limitation [F-45], community source), SPS/PPS repeated in-band [D-15]. The recording encode's settings are UNDEFINED (OQ-005), so the values used are recorded as test choices.

**Runs**

- **Pi 4 / CM4, hardware encoder:**
  - Pi 4 Model B and CM4 CAM0 have 2 lanes, so 1080p60 cannot be captured from the TC358743 there [C-01], [C-02], [C-48]. On those connectors, candidates for the 2-lane configuration of REQ-CAP-007, the highest encode run is 1080p50 UYVY (reasoning). The 1080p60 run applies to CM4 CAM1 (4-lane configuration).
  - 1080p60 UYVY at Level 4.2 for at least 10 minutes at stock clocks. The "at least 10 minutes" duration comes from a research open question (topic D) and is repeated in RISK-002; it is not an owner-accepted criterion (OQ-010, OQ-017).
  - Repeat at 1080p50 and 1080p30.
  - An informational run with `gpu_freq=550`, the value suggested in the official 720p120 recipe [D-52], only if the owner allows overclocking to be evaluated. Whether an overclock is acceptable in the product is OWNER DECISION REQUIRED (OQ-096); the measurement itself belongs to OQ-056.
  - Before the runs, record the encoder's input-format list with `v4l2-ctl --list-formats-out -d 11` [D-17] (check E-1 in [VIDEO_ENCODER.md](VIDEO_ENCODER.md)).
- **Pi 5 / CM5, software encoder:**
  - 1080p60, 1080p50 and 1080p30, including the UYVY → planar conversion [D-43].
  - Use each ADR-007 candidate under evaluation. The official documentation names `x264enc speed-preset=1 threads=1` as the Pi 5 replacement [D-37]; also record runs with other thread and preset settings.
- **H.265, CM4 and CM5 (and any other platform kept)** *(added 2026-10-08)* — **deferred, not run in current scope** *(2026-10-08, later: the owner answered OQ-103 with "H.264 only for now"; H.265 is REQ-ENC-002, DEFERRED, with no tests planned. The runs below are kept as the reference procedure for when REQ-ENC-002 is re-activated.)* RISK-022 and OQ-104 ask TEST-ENC-001 to include these runs. Its canonical title was changed on 2026-10-08 to "Sustained real-time H.264 / H.265 encode" *(and later on 2026-10-08 to "Sustained real-time H.264 encode (H.265 deferred)")*; the authoritative steps belong in [TESTING.md](TESTING.md).
  - Modes: 1920×1080 at 30 and 60 fps with 8-bit planar input (OQ-104), plus 1080p50, because REQ-CAP-007 captures it. On a 2-lane connector use the highest mode it captures (§3.2).
  - Encoders: GStreamer `x265enc` and FFmpeg `libx265`, each with fast presets (`ultrafast`, `superfast`) and `tune=zerolatency` [H-14], [H-16], [H-17]. Also run one non-zerolatency setting for recording, if OQ-103 assigns H.265 to recording. *(OQ-103 ANSWERED 2026-10-08: no output uses H.265 for now.)*
  - FFmpeg: set and record the frame-thread count explicitly, because the wrapper's thread count replaces zerolatency's single frame thread [H-10] (CORRECTED), [H-16]. The exact option syntax is a research gap (topic H): BUILD TEST REQUIRED.
  - Include the UYVY → planar (I420) conversion in the measured load [H-43]. Record whether it runs on the CPU or on a hardware converter (OQ-057, OQ-060).
  - Repeat with a concurrent H.264 encode (hardware on CM4, software on CM5) and with audio capture and encode, as OQ-104 asks.
  - Before the runs, record the x265 version and the CPU capabilities x265 reports. Expected (reasoning from [H-04], [H-05]): DotProd on CM5 only. The command is NEEDS VERIFICATION (OQ-105).
- **Concurrency:** repeat with the number of simultaneous encodes that OQ-005 requires. *(Superseded 2026-10-08, two encodes: OQ-005 now gives that number — two, a recording encode and one live encode shared by RTMP and WebRTC (owner, "Separate record + live"). The runs are the next item.)*
- **Two encodes, recording + live, concurrently (added 2026-10-08; owner decision on OQ-005; REQ-ENC-001).** Both encodes run at the same time from one capture of the mode under test, on CM4 and on CM5 (ADR-004 bring-up pair), and on any other platform kept:
  - **CM4, both on the hardware encoder (OQ-115, RISK-002).** First record whether a second encode session on the encoder node starts and streams at all; the method is NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED for concurrent sessions in `bcm2835-codec`, OQ-115). Then run 1080p30 + 1080p30, the combined rate of one 1080p60 encode (§4.1). Then run the capture rate of the configuration with both encodes at that rate: up to 1080p50 on CAM0 (2-lane) and 1080p60 on CAM1 (4-lane). Stock clocks; a `gpu_freq=550` run only as for the single-encode runs above (OQ-096). If both encodes cannot be sustained, record the highest combination that is, and record any split tried (lower-resolution live encode, or one encode in software, §5.2) as informational; the acceptable split is OQ-115.
  - **CM5, both in software (OQ-059, RISK-003).** 1080p60, 1080p50 and 1080p30, with both encodes at the capture rate and also with a 30 fps live encode, including the UYVY → planar conversion [D-43]. Record whether one conversion feeds both encodes or each converts separately (OQ-060, ADR-007), and the encoder, preset and thread settings of each encode.
  - Record every metric below per encode, plus CPU and temperature for the whole run. Audio and streaming are added in TEST-PERF-001 (§10.4).
  - Duration: as for the single-encode runs (the "at least 10 minutes" from RISK-002 is not owner-accepted; OQ-010, OQ-017).

**Metrics**

| Metric | Why | How |
|---|---|---|
| Encoded frames per second compared with input | Real-time capacity (§4) | Tool NEEDS VERIFICATION |
| Dropped or late frames | REQ-PERF-001 | Tool NEEDS VERIFICATION |
| Encode latency per frame | §7 | Reasoning: the Pi 4/CM4 encoder copies input timestamps to output buffers [D-19], so latency = dequeue time − input timestamp, provided the application queues the capture timestamp and reads the dequeue time from the same clock. Which clock the timestamps use is NEEDS VERIFICATION. Pi 5/CM5 method NEEDS VERIFICATION. *(Superseded in part, 2026-10-08: on the capture side the clock is now in the register. Every Raspberry Pi CSI receiver driver stamps buffers with `CLOCK_MONOTONIC` at frame start [I-33]. The dequeue-side clock, and the method for software H.264/H.265 encoders, are still NEEDS VERIFICATION)* |
| x265 CPU capabilities (H.265 runs; added 2026-10-08; deferred — REQ-ENC-002; not recorded in current scope) | §5.3; DotProd only on CM5 [H-04], [H-05] | Method NEEDS VERIFICATION (OQ-105) |
| Largest encoded frame (bytes) | The default CAPTURE `sizeimage` is 768 KiB [D-23] (OQ-056) | Record the maximum `bytesused` over the run (method NEEDS VERIFICATION) |
| CPU, per core and total | §5 | Tool NEEDS VERIFICATION |
| SoC temperature and throttling | §8 | Tool NEEDS VERIFICATION |

*(Added 2026-10-08, two encodes.)* In two-encode runs, frame rate, dropped or late frames, latency and largest encoded frame are recorded separately for the recording encode and the live encode; CPU and SoC temperature are recorded for the whole run.

### 10.4 TEST-PERF-001 — soak: thermal, CPU, CMA, frame drops (REQ-PERF-001)

NOT YET RUN ON PACSCORDER HARDWARE. Status: BLOCKED — HARDWARE REQUIRED.

**Preconditions**

- TEST-CAP-002 and TEST-ENC-001 have recorded results of TESTED — PASS on the platform.
- Duration, ambient temperature range, enclosure and drop threshold are defined. Today they are UNDEFINED — OWNER DECISION REQUIRED (OQ-010).

**Load:** the full product load required by the owner: capture, encode, recording, RTMP, WebRTC and audio, as applicable (OQ-004, OQ-005), on each lane configuration under evaluation (REQ-CAP-007). The ATEM integration is HDMI capture only (REQ-ATEM-001, REQ-CAP-008; OQ-009 ANSWERED 2026-10-07), so it is covered by the capture load; network ATEM functions are not in current scope. *(Added 2026-10-08.)* Audio is required (OQ-004 ANSWERED), so the load always includes audio capture and encoding. The video load includes H.264 and H.265 on the outputs that OQ-103 assigns. *(Superseded 2026-10-08, later: OQ-103 ANSWERED — the video load is H.264 on every output; H.265 is deferred, REQ-ENC-002, not in current scope.)* Run on CM4 and on CM5 (ADR-004). *(Added 2026-10-08, two encodes.)* The combined load is: one capture; two concurrent H.264 encodes, recording and live (owner, "Separate record + live", OQ-005); the recorder writing the recording encode; RTMP and WebRTC both sending the one live encode; and two audio encodes, AAC for recording and RTMP and Opus for WebRTC [F-31], [F-41] (OQ-063). On CM4 both video encodes are on the hardware encoder (OQ-115, RISK-002); on CM5 both are software encodes with the UYVY → planar conversion (OQ-059, RISK-003).

**Sampled metrics** (at a regular interval, recorded with timestamps):

- CPU per core and total;
- SoC temperature and throttling state;
- `CmaFree` from `/proc/meminfo` [C-53];
- frames captured, encoded and dropped *(2026-10-08: encoded and dropped per encode, recording and live)*;
- encoder output rate *(2026-10-08: per encode)*;
- recording file growth and stream health;
- ambient temperature next to the TC358743, compared with its −30 to +70 °C rating [A-41];
- *(added 2026-10-08)* A/V offset over the run, for the multi-hour drift check that RISK-024 names (method NEEDS VERIFICATION; tolerance OQ-112).

**Pass criteria:** UNDEFINED (OQ-010).

### 10.5 TEST-CAP-003 — signal-change detection latency (REQ-CAP-004)

NOT YET RUN ON PACSCORDER HARDWARE. Status: BLOCKED — HARDWARE REQUIRED.

**Measure** the time from these events to the `V4L2_EVENT_SOURCE_CHANGE` event:

- HDMI disconnect;
- HDMI connect;
- an input mode change.

**Expected from sources:** up to about 1 s with polling [A-30]. On Pi 5/CM5, subscribe on the sub-device [C-42] (community report).

The command that waits for the event is NEEDS VERIFICATION (OQ-101). The result decides whether RISK-013 is accepted or whether INT must be wired (OQ-020).

---

## 11. Verification status

### Verified from sources (fact IDs)

"Verified from sources" means that the cited source says this (see [REFERENCES.md](REFERENCES.md), "What a verdict means"). It does not mean it has been observed on PACSCORDER hardware.

- CSI-2 bandwidth, lanes and feasibility (reasoning facts and their official cross-checks): [C-46], [C-47], [C-48], [C-49], [C-50], [B-33], [A-26], [C-37], [C-16]; community report [C-43].
- HDMI sources (REQ-CAP-008): [F-23].
- Kernel versions named in the procedures: [G-04], [E-37].
- Connectors and receivers: [C-01], [C-02], [C-04], [C-05], [C-07], [C-30], [C-31], [C-38], [C-52], [A-05], [A-07], [A-08].
- Encoder macroblock rate and levels: [D-10], [D-12], [D-36], [D-52], [F-40]; community report [D-50].
- Bitrate and buffers: [D-13], [D-19], [D-23], [D-34], [D-44].
- CPU: [G-22], [D-30], [D-08], [D-25], [D-40], [D-41], [D-43]; community reports [D-18], [D-50], [C-41].
- Memory/CMA: [C-53], [E-47], [C-40], [G-19], [E-48], [D-03], [D-48].
- Latency: [A-30], [A-29], [B-19], [B-39], [C-20], [E-39], [D-14], [D-32], [D-35]; community report [C-42].
- Thermal and power: [A-41], [A-38], [B-12].
- Procedure commands and node choice: [C-37], [B-24], [B-25], [C-11], [C-36], [D-17], [D-37]; community report [C-33].
- *(Added 2026-10-08.)* H.265 budget inputs (§4, §5.1–§5.3, §6.1, §7, §8, §10; deferred — REQ-ENC-002; kept as its evidence):
  - no hardware HEVC encoder: [D-24], [D-31];
  - encoders and versions: [H-01], [H-02], [H-06], [H-07], [H-09], [H-11];
  - input formats and conversion: [H-10] (CORRECTED), [H-13]; reasoning [H-43];
  - settings and latency: [H-14], [H-15], [H-16], [H-17];
  - SIMD: [H-04], [H-05];
  - cost evidence: community reports [H-19], [H-20], [H-21], [H-22]; reasoning [H-23];
  - bitrate scale: [H-29].
- *(Added 2026-10-08.)* Audio load and A/V timestamps (§5.1, §7):
  - encoders: [I-39], [I-41], [I-44], [I-45], [I-46], [I-47], with [H-26], [F-41];
  - timestamps: [I-33], [I-34], [I-35], [I-36], [I-37], [I-38].
- *(Added 2026-10-08.)* Two H.264 encodes (owner decision on OQ-005; §2, §4.1–§4.3, §5.1, §5.2, §6, §7, §8, §10.3, §10.4): [D-06], [D-07], [D-08], [D-10], [D-13], [D-15], [D-25], [D-36], [D-40], [D-41], [D-43], [D-52], [F-31], [F-36], [F-39], [F-40], [F-41], [G-22]; community report [F-45]. The two-encode macroblock table, the 720p30-equals-Level-3.1 observation, the doubling of the [G-22] figure and the per-encode storage and network reading are Claude's reasoning, not register facts. The number of concurrent encode sessions on the CM4 encoder is not in the register (OQ-115).

### Verified on PACSCORDER hardware

**Nothing** (no hardware exists as of 2026-10-07). No budget in this document has been measured. TEST-CAP-002, TEST-ENC-001, TEST-PERF-001 and TEST-CAP-003 are BLOCKED — HARDWARE REQUIRED. *(Re-checked 2026-10-08: still nothing. The H.265 budget line in §5.3 has no value, only UNKNOWN.)* *(Re-checked 2026-10-08, two encodes: no two-encode budget has been measured; the §4.1 combinations are calculations.)*

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against REFERENCES.md. Changes: the units rule now separates decimal [C-53] sizes from binary DT CMA sizes; 6by9's spreadsheet figures and the [C-43] attribution worded as community reports and completed; uncited "quad-core" and "Cortex-A72" removed; a Pi 5/CM5 bitrate-range gap added (NEEDS VERIFICATION); TEST-CAP-002 now issues EDID/DV-timings on the sub-device in Media Controller mode [B-25], [C-36] and notes that the Pi 4/CM4 `media-ctl` sequence is not attested; expected lane-rejection message given per format; TEST-ENC-001 adds the TEST-DMA-001 dependency from TESTING.md, a note that Pi 4 Model B / CM4 CAM0 cannot run 1080p60, and a clock-domain caveat on the latency method; "has passed" preconditions reworded to Rule 10 status words | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: the ~675 ns LP↔HS figure now carries the caveat that it was reported only for 2-lane 1080p50 UYVY at 972 Mbit/s ([C-50], community input); §3.2/§3.3 and TEST-CAP-002 say a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes, OQ-038); link-frequency statements reference ADR-008 / OQ-099, and TEST-CAP-002 adds the ADR-008 297 MHz CM4 CAM1 evaluation run; `gpu_freq=550` acceptability now OQ-096 (§4.2, §10.3), with OQ-056 kept for the measurement; 2-lane 1080p50 UYVY load-class note added from CSI_PIPELINE.md §10.6; CSI-2 error-counter method linked to OQ-050 / OQ-095; `-d <sub-device path>`, frame counting, event waiting and version-recording commands linked to OQ-101; 6.18.39 labelled as the kernel of the [C-33] report only (shipped 6.18.50 [G-04], inspected 6.18.55 [E-37]); RISK-002 10-minute duration labelled as a research open question Final verification pass (same date): §10.3 precondition aligned with TESTING.md TEST-ENC-001 (on 2-lane connectors a passing lower-rate capture replaces the 1080p60 TEST-CAP-002 result); CPU sampling, SoC temperature/throttling read-out and device enumeration in §8 and §10.1 linked to OQ-101, whose scope note now names them. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: §2 row "Whether 1080p60 is mandatory — UNDEFINED (OQ-001)" replaced by the REQ-CAP-007 capture scope (OQ-001 ANSWERED; mode list OQ-002, fractional rates OQ-040); new §3.2 per-lane-configuration budget table with candidate connectors and supported-mode limits [C-37], [C-48], [C-49], [B-33], HDMI-source note (REQ-CAP-008, OQ-102, [F-23]) and per-configuration EDID note (OQ-002, reasoning); §3.3 marks 2-lane / 4-lane candidates and the one-board-for-both question (OQ-021); §4.2 2-lane encode ceiling tied to REQ-CAP-007; §5.1/§5.2 ATEM load row: HDMI capture only, network functions not in current scope (OQ-009 ANSWERED, REQ-ATEM-001); §9 platform column, §10.1, §10.2 (scope aligned with TESTING.md TEST-CAP-002, "Verifies" REQ-CAP-001 and REQ-CAP-007; HDMI-source precondition 6), §10.3 and §10.4 now name the lane configuration; §6.2 OS-basis note references REQ-BLD-002 (ADR-003 still PROPOSED). Added citations B-33, F-23. No platform chosen; no ADR status changed; no measurement added. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §6.2 CMA unknowns: "the build tool is ADR-003 (PROPOSED)" → "the build tool is `rpi-image-gen` (ADR-003, ACCEPTED)". No budget, evidence, other ADR status or implementation status changed. | Claude (session 2026-10-07) |
| 2026-10-08 | H.265 budget inputs (research topic H), audio load and A/V timestamps (research topic I), and the owner decisions of 2026-10-07 (second set). Changes: <br>• Header: "Applies to" and "Verification" updated. <br>• §2: rows for codecs (H.264 + H.265; OQ-103), HDMI audio (OQ-004 ANSWERED; tolerance OQ-112) and the CM4 + CM5 side-by-side bring-up (ADR-004 OPEN). <br>• §3.2: HDMI-source note marked superseded in part (OQ-102 ANSWERED: any HDMI camera plus ATEM, no model list). <br>• §4.1/§4.2: H.265 levels not in the register; H.265 is a CPU load on every platform; H.264 + H.265 concurrency. §4.3: `x265enc` bitrate range [H-14]; YouTube 1080p60 bitrates with storage reasoning [H-29]. <br>• §5.1: new H.265 row; conversion row (planar only, [H-43] traffic); concurrency row; audio row marked superseded in part (audio required; encoders [I-39]–[I-47]). §5.2: H.265 and conversion rows; "core type not in register" marked superseded in part ([H-05]). <br>• New §5.3 H.265 budget line for CM4 and CM5. It holds inputs only, and the budget is UNKNOWN; it warns against multiplying [G-22] by [H-23] and against carrying the Pi 400 figure over to CM4. <br>• §6.1: I420 frame-size row [H-43]. §7: H.265 latency, conversion-stage and A/V timestamp rows. §8: H.265 thermal row. §9: re-check note; SW version and Notes rules extended. <br>• §10.1: CM4 + CM5 and x265 version rules. §10.2: precondition 6 marked superseded in part (OQ-102). §10.3: H.265 runs, the latency-clock note marked superseded in part ([I-33]), and an x265 CPU-capability metric. §10.4: audio always in the load; A/V drift metric. §11: fact lists. <br>New citations: D-24, D-31, H-01, H-02, H-04 to H-07, H-09 to H-11, H-13 to H-17, H-19 to H-23, H-26, H-29, H-43, I-33 to I-39, I-41, I-44 to I-47, F-41. No measurement added; no REQ or ADR status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic H and I additions: §5.3 usage rules — the harness description [H-22] and the Pi 400 clock [H-21] now worded as community reports, and [H-23] marked as a reasoning-tier entry. All other [H-xx] and [I-xx] citations and the storage arithmetic (12 and 17 Mbit/s for one hour) checked; no change needed. No measurement added; no status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (owner chose H.264 + H.265 on 2026-10-07); ID unchanged. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header "Applies to" (REQ-ENC-001 H.264 only; H.265 = REQ-ENC-002, DEFERRED), TEST-ENC-001 status row and §10.3 heading use the canonical title "Sustained real-time H.264 encode (H.265 deferred)"; §2 codecs row rewritten to the 2026-10-08 answer (earlier entry kept as superseded; OQ-103 ANSWERED); §4.1, §4.2, §4.3 H.265 notes labelled, §4.2 concurrency note superseded, YouTube H.265 figures marked deferred; §5.1 and §5.2 H.265 encode and conversion rows labelled, §5.1 concurrency row superseded (several H.264 encodes still OQ-005, OQ-059); §5.3 heading labelled "(deferred — REQ-ENC-002; not in current scope)" with a scope note — not a current-scope budget, inputs kept as evidence; §6.1 I420 row note (same frame size applies if software H.264 on Pi 5/CM5 is fed I420, reasoning [D-40], [D-41], [D-43]); §7 H.265 latency and conversion rows, §8 H.265 thermal row labelled; §9 SW-version rule and §10.1 x265 rule apply only if REQ-ENC-002 is re-activated; §10.3 H.265 runs deferred, not run in current scope (kept as reference procedure; OQ-103 answer noted; x265 metric labelled); §10.4 load superseded (H.264 on every output); §11 H.265 fact list labelled. Budget values unchanged; no other decision or status changed; no ID added; no fact ID new to this document. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): header "Applies to"; §2 "Bitrate(s) and number of simultaneous encodes" row — number of encodes set (recording + one live encode shared by RTMP and WebRTC), bitrate and rate control still UNDEFINED (earlier entry kept as superseded); §4.1 new two-encode macroblock table (2 × 1080p30 = 2.0×, up to 2 × 1080p60 = 4.0× the CM4 specification; reasoning), per-bitstream level note, 720p30 = Level 3.1 limits (reasoning [F-40]; OQ-073, RISK-019), scaling via `/dev/video12` UNKNOWN (OQ-057, OQ-115); §4.2 CM4 two-encode budget bullet (sum on one encoder; OQ-115, RISK-002), CM5 note, simultaneous-encodes bullet superseded in part; §4.3 separate recording/live bitrates (storage vs network); §5.1 per-encode note on the [G-22] row, shared-conversion note, simultaneous-encodes row superseded in part (≈ double, about 60–80 % at 1080p30 if linear — not established; OQ-059, RISK-003), audio row (still AAC + Opus), headroom row; §5.2 encode, raw-copy and software-fallback rows and headroom row (OQ-115); §6.1 encoded-buffer and conversion-buffer rows, §6.2 CMA unknown and measurement note; §7 latency note per encode and two encode rows; §8 CM5 thermal row; §9 Notes rule (one row per encode); §10.3 per-encode settings precondition (live encode: Constrained Baseline, level from mode, no B-frames, repeated SPS/PPS), concurrency bullet superseded, new two-encode run on CM4 (OQ-115) and CM5 (OQ-059), per-encode metrics note; §10.4 combined load (one capture, two video encodes, recorder, RTMP and WebRTC from the live encode, AAC + Opus) and per-encode sampled metrics; §11 fact list and re-check note. New citations in this document: D-06, D-07, D-15, F-31, F-36, F-39, F-45. No status changed; no ID added; no measurement added. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — §4.2 CM4 bullet "run on this one encoder" → "would run" and the 2 × 1080p30 sentence labelled "Reasoning"; §5.1 [G-22] row "PACSCORDER runs two" → "requires two"; §5.2 encode row "run on this encoder" → "would run"; §7 Pi 5/CM5 latency row and §10.3 live-encode settings: no-B-frames worded as a MediaMTX report [F-45] (community source). No status changed; no ID added. | Claude (session 2026-10-08) |
