# PACSCORDER Performance: Budgets and Measurement Plan

| | |
|---|---|
| Document status | DRAFT — budgets calculated from source research; **no measurement exists** |
| Last updated | 2026-10-07 |
| Applies to | Pi 4 Model B, CM4, Pi 5, CM5; the 2-lane and the 4-lane configuration — REQ-PERF-001, REQ-CAP-001, REQ-CAP-007, REQ-CAP-008, REQ-ENC-001, REQ-DMA-001 |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)). Nothing has been measured or tested on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 8, 10, 22, 23, 25 |

| Item | Status |
|---|---|
| REQ-PERF-001 (sustained operation) implementation | NOT STARTED |
| TEST-CAP-002 (1080p60 capture: frame rate and frame integrity) | BLOCKED — HARDWARE REQUIRED |
| TEST-ENC-001 (sustained real-time H.264 encode) | BLOCKED — HARDWARE REQUIRED |
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
| Bitrate(s) and number of simultaneous encodes | UNDEFINED | OWNER DECISION REQUIRED (OQ-005) |
| Boot-to-first-frame time | UNDEFINED | OWNER DECISION REQUIRED (OQ-068) |
| Capture rates per lane configuration | Owner, 2026-10-07 (OQ-001 ANSWERED): both a 2-lane and a 4-lane configuration, each capturing every frame rate its link carries (REQ-CAP-007, DRAFT). Recorded interpretation: 1080p60 required on the 4-lane configuration; the 2-lane configuration is limited to 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48] (§3.2). The exact mode list is UNDEFINED | OWNER DECISION REQUIRED (OQ-002; fractional rates OQ-040) |

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
- **HDMI sources (REQ-CAP-008).** Sources are ATEM switcher outputs and cameras connected directly (models OQ-102). The ATEM Mini Pro outputs 1080p23.98 to 1080p60 with no 720p or 1080i [F-23]. Reasoning (inputs [F-23], [C-48]): on the 2-lane configuration its 1080p59.94 and 1080p60 standards exceed the link budget, and 1080p50 fits only in UYVY. Camera output modes are UNKNOWN (OQ-102).
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

### 4.2 Budget per platform

- **Pi 4 / CM4 (hardware).**
  - The specified capacity is 1080p30 [D-10], and the driver calls level 4.0 the hardware specification [D-12].
  - 1080p60 needs 2.0× the specified rate, and about 1.13× the officially documented 720p120 recipe [D-52].
  - A Raspberry Pi engineer reported 1080p60 as an "edge case" on the hardware encoder [D-50] (community).
  - **The available margin is UNKNOWN — HARDWARE TEST REQUIRED** (RISK-002, OQ-056, TEST-ENC-001).
  - Whether a GPU overclock such as `gpu_freq=550` (suggested in the official 720p120 recipe [D-52]) would be acceptable in the product is OWNER DECISION REQUIRED (OQ-096).
  - A Pi 4 Model B never presents more than 1080p50 UYVY to the encoder, because of its 2 lanes [C-01], [C-48]; that is 1.67× the specification (reasoning). The same ceiling applies to any 2-lane configuration of REQ-CAP-007. On a 4-lane configuration (CM4 CAM1) 1080p60 is captured; whether it must also be encoded at that rate is REQ-ENC-001 / OQ-005.
- **Pi 5 / CM5 (software).** There is no hardware encoder, so no macroblock-rate specification exists; encoding runs in software [G-22], so the limit is CPU time (§5) (reasoning).
- **Simultaneous encodes.** If recording, RTMP and WebRTC need separate encodes (OQ-005), the demand is the sum of (macroblocks per frame × frames/s) over all encodes (reasoning). Whether the Pi 4/CM4 hardware encoder can run several encode sessions at once, and at what total rate, is UNKNOWN — HARDWARE TEST REQUIRED.

### 4.3 Bitrate, storage and network

- The Pi 4/CM4 encoder bitrate range is 25 kbit/s – 25 Mbit/s, with a default of 10 Mbit/s [D-13].
- **Reasoning (input [D-13]):** video only, without audio or container overhead:
  - at 25 Mbit/s: 25 × 10⁶ × 3600 / 8 = 11.25 GB (decimal) per hour;
  - at 10 Mbit/s: 4.5 GB per hour.
- Pi 5/CM5: the bitrate ranges of `x264enc`, `libx264` and `openh264enc` were not researched — NEEDS VERIFICATION. The storage arithmetic above depends only on the bitrate, so it applies to any encoder at the same bitrate (reasoning).
- The PACSCORDER bitrate, storage medium and its sustained write rate, and network uplink capacity are UNKNOWN — OWNER DECISION REQUIRED / HARDWARE TEST REQUIRED (OQ-005, OQ-006, OQ-007, OQ-008).

---

## 5. CPU budget

### 5.1 Pi 5 / CM5

The **only official figure** is: **"H264 1080p30 encode (from ISP) ~30–40% CPU"** on BCM2712 [G-22], [D-30].

Everything else is UNKNOWN — HARDWARE TEST REQUIRED.

| Load component | Budget | Status / resolution |
|---|---|---|
| H.264 encode, 1080p30 | ~30–40% CPU (official, "from ISP") [G-22] | Whether this means all CPU cores together or one core: UNKNOWN — VENDOR CONFIRMATION REQUIRED (OQ-059). The BCM2712 core count is not in the source register. Whether it applies to TC358743 input: UNKNOWN — HARDWARE TEST REQUIRED. Reasoning: that input is not expected to come "from ISP", because Raspberry Pi engineers reported that libcamera does not support the bridge [C-41] |
| H.264 encode, 1080p50 / 1080p60 | UNKNOWN | HARDWARE TEST REQUIRED (OQ-059). A Raspberry Pi engineer reported 1080p60 software encode from camera capture as "easily achievable" [D-50] (community); this is not a PACSCORDER budget |
| UYVY → I420/NV12 (or other planar) conversion per frame | UNKNOWN | Required because `x264enc`, `libx264` and `openh264enc` do not accept packed UYVY [D-40], [D-41], [D-43]. Hardware offload: UNKNOWN (OQ-060) |
| V4L2 capture handling | UNKNOWN | HARDWARE TEST REQUIRED |
| Additional simultaneous encodes (recording + RTMP + WebRTC) | UNKNOWN | HARDWARE TEST REQUIRED (OQ-005, OQ-059) |
| Muxing, RTMP, WebRTC (RTP, ICE, DTLS/SRTP) | UNKNOWN | HARDWARE TEST REQUIRED; see [STREAMING.md](STREAMING.md) |
| Audio capture and encode | UNKNOWN | OQ-004, OQ-063 |
| ATEM integration | No separate load expected (reasoning): the ATEM integration is HDMI capture of the ATEM output only (REQ-ATEM-001, REQ-CAP-008; owner 2026-10-07), which is the capture load above | OQ-009 ANSWERED. Network tally/control and RTMP exchange are not in current scope; they would add load only if the owner adds them |
| **Total and remaining headroom** | **UNKNOWN** | TEST-ENC-001, TEST-PERF-001 |

### 5.2 Pi 4 / CM4

| Load component | Budget | Status / resolution |
|---|---|---|
| H.264 encode | Done by the VideoCore firmware through `ril.video_encode` [D-08]. The Arm CPU cost of driving it is UNKNOWN | HARDWARE TEST REQUIRED |
| UYVY input | A Raspberry Pi engineer reported it as accepted directly by the encoder [D-18] (community). Otherwise the `/dev/video12` ISP M2M device can convert [D-25]. CPU cost UNKNOWN | HARDWARE TEST REQUIRED (OQ-057) |
| Raw-frame copy into the encoder | None if capture buffers are imported as DMABUF [D-19], [D-34]. One copy per frame if upstream FFmpeg `h264_v4l2m2m` is used, because it is MMAP-only [D-44]. CPU cost UNKNOWN | HARDWARE TEST REQUIRED (OQ-058, ADR-007) |
| Software x264 fallback on the BCM2711 Arm CPU (if hardware cannot reach the required rate) | No official figure (research gap, topic D). The CPU core type and count are not in the source register | UNKNOWN — HARDWARE TEST REQUIRED (OQ-056) |
| Streaming, audio | UNKNOWN | as for Pi 5 / CM5 |
| ATEM integration | No separate load expected (reasoning; HDMI capture only, §5.1) | OQ-009 ANSWERED |
| **Total and remaining headroom** | **UNKNOWN** | TEST-ENC-001, TEST-PERF-001 |

---

## 6. Memory and CMA budget

### 6.1 Frame and buffer sizes

| Item | Size | Source | Notes |
|---|---|---|---|
| One 1920×1080 UYVY frame | 4,147,200 bytes | [C-53] | |
| One 1920×1080 RGB888 / BGR3 frame | 6,220,800 bytes | [C-53] | |
| Four UYVY capture buffers | ≈ 16.6 MB (16,588,800 bytes) | [C-53] (CORRECTED verdict) | Four is the example count in [C-53]. The PACSCORDER count is UNKNOWN (set by the application, ADR-007) |
| Four RGB888 capture buffers | ≈ 24.9 MB (24,883,200 bytes) | [C-53] | as above |
| Pi 4/CM4 encoded (CAPTURE) buffer | 768 KiB each above 720p; a larger `sizeimage` can be requested | [D-23] | Buffer count UNKNOWN |
| Pi 4/CM4 encoder raw (OUTPUT) buffers | No extra buffers when capture buffers are imported as DMABUF (reasoning from [D-19], [D-34]); otherwise frame-sized MMAP buffers allocated through `videobuf2-dma-contig` [D-19] | [D-19] | Count and pool: NEEDS VERIFICATION |
| Pi 5/CM5 conversion output buffers (planar format) | UNKNOWN | — | Depend on the format and framework (OQ-060, ADR-007) |
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
- The total CMA need at full load (capture + encoder + display, if any). HARDWARE TEST REQUIRED (OQ-061, RISK-020).
- On Pi 4/CM4, the `gpu_mem` value the codec needs. Note that `gpu_mem=16` selects the cut-down firmware, which has no codecs [D-48]. VENDOR CONFIRMATION REQUIRED and HARDWARE TEST REQUIRED (OQ-048).
- The DMA-BUF heap names on the chosen kernel [E-48], [G-19]. HARDWARE TEST REQUIRED (OQ-062). See [DMA.md](DMA.md).

Measurement method: read the `CmaFree` field of `/proc/meminfo` while streaming at maximum load [C-53] (TEST-PERF-001, §10.4).

---

## 7. Latency budget

**End-to-end latency target: UNDEFINED — OWNER DECISION REQUIRED (OQ-005, OQ-008).** Without a target, the stages below cannot be judged; they can only be listed and measured.

| Stage | Known value or bound | Source | Status |
|---|---|---|---|
| HDMI signal-change detection, no interrupt wired | Without an IRQ, the driver polls the TC358743 interrupt status over I2C every 1000 ms (every 10 ms if a CEC adapter is registered). The stock Raspberry Pi overlay has no `interrupts` property | [A-30], [B-19], [C-20] | Up to about 1 s (RISK-013) |
| CEC fast polling | Not available by default: the Raspberry Pi defconfigs do not enable `CONFIG_VIDEO_TC358743_CEC` | [C-20], [E-39] | — |
| Signal-change detection, interrupt wired | INT is active-high, level-triggered [A-29]; the driver uses a threaded IRQ when the I2C client has one [B-19] | [A-29], [B-19] | PACSCORDER INT wiring: UNKNOWN — VERIFICATION REQUIRED (OQ-020) |
| Event delivery on Pi 5/CM5 | Reported by a Raspberry Pi engineer: subscribe to source-change events on the TC358743 sub-device, not on the video node [C-42] (community) | [C-42] | OQ-051 |
| Pipeline reconfiguration after a change | The driver sends `V4L2_EVENT_SOURCE_CHANGE` but does not apply new timings itself [B-39] | [B-39] | Application time UNKNOWN |
| One frame period (reasoning: 1 / frame rate, rates from [C-46]) | 16.7 ms at 1080p60; 20.0 ms at 1080p50; 33.3 ms at 1080p30 | [C-46] | — |
| Capture queue depth | UNKNOWN (number of buffers in flight, ADR-007) | — | HARDWARE TEST REQUIRED |
| Encode, Pi 4/CM4 hardware | No source figure. No B-frame reordering, because the encoder produces no B-frames [D-14] | [D-14] | UNKNOWN — HARDWARE TEST REQUIRED |
| Encode, Pi 5/CM5 software | Official: software encoders "generally output frames with a longer latency than the old hardware encoders". Low-latency mode drops B-frames and arithmetic coding | [D-32], [G-22] | UNKNOWN — HARDWARE TEST REQUIRED (OQ-059) |
| `rpicam-apps` low-latency `libx264` reference settings | `ultrafast`, `zerolatency`, 4 slices, `refs=1`, `rc-lookahead 0` | [D-35] | Reference only |
| Muxing, network, server, player / browser | UNKNOWN | — | See [STREAMING.md](STREAMING.md), [RECORDING.md](RECORDING.md) |
| **End-to-end** | **UNDEFINED target; UNKNOWN value** | — | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |

---

## 8. Thermal

| Item | Value | Source | Status |
|---|---|---|---|
| TC358743XBG operating temperature | Ta −30 to +70 °C (ambient, voltage applied). TC9590XBG, a different part: −40 to +85 °C. Storage: −40 to +125 °C | [A-41] | Datasheet value |
| TC358743 typical power | 480.5 mW at 720p60; 543.2 mW at 1080p60 | [A-38] | Datasheet value |
| Raspberry Pi SoC temperature limits, throttling behaviour, cooling requirements (Pi 4 Model B, CM4, Pi 5, CM5) | **UNKNOWN — VERIFICATION REQUIRED.** No fact in the source register covers them | — | VENDOR CONFIRMATION REQUIRED (official Raspberry Pi documentation to be researched); HARDWARE TEST REQUIRED |
| Thermal effect of Pi 5/CM5 software encoding | UNKNOWN. RISK-003 lists CPU and thermal load at 1080p60 as an impact; software encoding is CPU work [G-22] | [G-22] | HARDWARE TEST REQUIRED (OQ-059) |
| Product ambient range, enclosure, airflow | UNDEFINED | — | OWNER DECISION REQUIRED (OQ-010) |
| Product power input and budget | UNKNOWN — VERIFICATION REQUIRED | — | OQ-023 |
| D-PHY timing and FIFO-level validity across temperature | UNKNOWN | [B-12], [C-43] | HARDWARE TEST REQUIRED (OQ-035) |

The command used to read SoC temperature and throttling state is not attested in the source register: NEEDS VERIFICATION (OQ-101). It must be chosen and documented in [TESTING.md](TESTING.md) before TEST-PERF-001 runs.

---

## 9. Measurements

**No measurements exist.** As of 2026-10-06 there is no PACSCORDER hardware, no code, and no test has been run. Every value in §3–§8 is a calculation or a source statement, not a measurement.

Add one row per measured value. Never delete or rewrite a row (Rule 21); add a correcting row instead.

| Date | Platform | HW rev | SW version | Test ID | Metric | Value | Notes |
|---|---|---|---|---|---|---|---|
| — | — | — | — | — | — | — | No measurements exist (no hardware as of 2026-10-06). |

Column rules:

- **Platform**: board and connector or carrier, and the lane configuration of REQ-CAP-007 (2-lane or 4-lane), for example "CM4 on <carrier>, CAM1, 4-lane".
- **HW rev**: PACSCORDER hardware revision. None exists yet (OQ-018).
- **SW version**: image, kernel version, firmware version, and the versions of the framework and encoder packages.
- **Test ID**: one of the canonical IDs in [README.md](README.md).
- **Notes**: include at least the input mode, pixel format, active lanes, link frequency, encoder and settings, duration, ambient temperature and cooling.

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
- Measure each platform and each lane configuration under evaluation separately (ADR-004, OQ-011; REQ-CAP-007 requires both a 2-lane and a 4-lane configuration). Never assume that results carry over between the 2-lane and the 4-lane configuration, or between Pi 4 Model B and CM4, or between Pi 5 and CM5, or between OS images, for example 16K-page and 4K-page kernels (OQ-055).
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
6. HDMI source: each required source model that outputs the mode under test, ATEM output or camera (REQ-CAP-008; models OQ-102).

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

### 10.3 TEST-ENC-001 — sustained real-time encode (REQ-ENC-001)

NOT YET RUN ON PACSCORDER HARDWARE. Status: BLOCKED — HARDWARE REQUIRED.

**Preconditions**

- TEST-CAP-002 has a recorded result of TESTED — PASS for the mode under test. TEST-CAP-002 cannot run on a 2-lane connector; there, as for TEST-CAP-003, a passing lower-rate capture of the mode under test is needed instead (same wording as [TESTING.md](TESTING.md) TEST-ENC-001).
- TEST-DMA-001 has been run, because [TESTING.md](TESTING.md) lists it as a dependency of TEST-ENC-001.
- The encoder settings are recorded: profile, level (set from the mode, see [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §6), bitrate and mode, GOP, repeat-header setting.

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
- **Concurrency:** repeat with the number of simultaneous encodes that OQ-005 requires.

**Metrics**

| Metric | Why | How |
|---|---|---|
| Encoded frames per second compared with input | Real-time capacity (§4) | Tool NEEDS VERIFICATION |
| Dropped or late frames | REQ-PERF-001 | Tool NEEDS VERIFICATION |
| Encode latency per frame | §7 | Reasoning: the Pi 4/CM4 encoder copies input timestamps to output buffers [D-19], so latency = dequeue time − input timestamp, provided the application queues the capture timestamp and reads the dequeue time from the same clock. Which clock the timestamps use is NEEDS VERIFICATION. Pi 5/CM5 method NEEDS VERIFICATION |
| Largest encoded frame (bytes) | The default CAPTURE `sizeimage` is 768 KiB [D-23] (OQ-056) | Record the maximum `bytesused` over the run (method NEEDS VERIFICATION) |
| CPU, per core and total | §5 | Tool NEEDS VERIFICATION |
| SoC temperature and throttling | §8 | Tool NEEDS VERIFICATION |

### 10.4 TEST-PERF-001 — soak: thermal, CPU, CMA, frame drops (REQ-PERF-001)

NOT YET RUN ON PACSCORDER HARDWARE. Status: BLOCKED — HARDWARE REQUIRED.

**Preconditions**

- TEST-CAP-002 and TEST-ENC-001 have recorded results of TESTED — PASS on the platform.
- Duration, ambient temperature range, enclosure and drop threshold are defined. Today they are UNDEFINED — OWNER DECISION REQUIRED (OQ-010).

**Load:** the full product load required by the owner: capture, encode, recording, RTMP, WebRTC and audio, as applicable (OQ-004, OQ-005), on each lane configuration under evaluation (REQ-CAP-007). The ATEM integration is HDMI capture only (REQ-ATEM-001, REQ-CAP-008; OQ-009 ANSWERED 2026-10-07), so it is covered by the capture load; network ATEM functions are not in current scope.

**Sampled metrics** (at a regular interval, recorded with timestamps):

- CPU per core and total;
- SoC temperature and throttling state;
- `CmaFree` from `/proc/meminfo` [C-53];
- frames captured, encoded and dropped;
- encoder output rate;
- recording file growth and stream health;
- ambient temperature next to the TC358743, compared with its −30 to +70 °C rating [A-41].

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

### Verified on PACSCORDER hardware

**Nothing** (no hardware exists as of 2026-10-07). No budget in this document has been measured. TEST-CAP-002, TEST-ENC-001, TEST-PERF-001 and TEST-CAP-003 are BLOCKED — HARDWARE REQUIRED.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against REFERENCES.md. Changes: the units rule now separates decimal [C-53] sizes from binary DT CMA sizes; 6by9's spreadsheet figures and the [C-43] attribution worded as community reports and completed; uncited "quad-core" and "Cortex-A72" removed; a Pi 5/CM5 bitrate-range gap added (NEEDS VERIFICATION); TEST-CAP-002 now issues EDID/DV-timings on the sub-device in Media Controller mode [B-25], [C-36] and notes that the Pi 4/CM4 `media-ctl` sequence is not attested; expected lane-rejection message given per format; TEST-ENC-001 adds the TEST-DMA-001 dependency from TESTING.md, a note that Pi 4 Model B / CM4 CAM0 cannot run 1080p60, and a clock-domain caveat on the latency method; "has passed" preconditions reworded to Rule 10 status words | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: the ~675 ns LP↔HS figure now carries the caveat that it was reported only for 2-lane 1080p50 UYVY at 972 Mbit/s ([C-50], community input); §3.2/§3.3 and TEST-CAP-002 say a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes, OQ-038); link-frequency statements reference ADR-008 / OQ-099, and TEST-CAP-002 adds the ADR-008 297 MHz CM4 CAM1 evaluation run; `gpu_freq=550` acceptability now OQ-096 (§4.2, §10.3), with OQ-056 kept for the measurement; 2-lane 1080p50 UYVY load-class note added from CSI_PIPELINE.md §10.6; CSI-2 error-counter method linked to OQ-050 / OQ-095; `-d <sub-device path>`, frame counting, event waiting and version-recording commands linked to OQ-101; 6.18.39 labelled as the kernel of the [C-33] report only (shipped 6.18.50 [G-04], inspected 6.18.55 [E-37]); RISK-002 10-minute duration labelled as a research open question Final verification pass (same date): §10.3 precondition aligned with TESTING.md TEST-ENC-001 (on 2-lane connectors a passing lower-rate capture replaces the 1080p60 TEST-CAP-002 result); CPU sampling, SoC temperature/throttling read-out and device enumeration in §8 and §10.1 linked to OQ-101, whose scope note now names them. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: §2 row "Whether 1080p60 is mandatory — UNDEFINED (OQ-001)" replaced by the REQ-CAP-007 capture scope (OQ-001 ANSWERED; mode list OQ-002, fractional rates OQ-040); new §3.2 per-lane-configuration budget table with candidate connectors and supported-mode limits [C-37], [C-48], [C-49], [B-33], HDMI-source note (REQ-CAP-008, OQ-102, [F-23]) and per-configuration EDID note (OQ-002, reasoning); §3.3 marks 2-lane / 4-lane candidates and the one-board-for-both question (OQ-021); §4.2 2-lane encode ceiling tied to REQ-CAP-007; §5.1/§5.2 ATEM load row: HDMI capture only, network functions not in current scope (OQ-009 ANSWERED, REQ-ATEM-001); §9 platform column, §10.1, §10.2 (scope aligned with TESTING.md TEST-CAP-002, "Verifies" REQ-CAP-001 and REQ-CAP-007; HDMI-source precondition 6), §10.3 and §10.4 now name the lane configuration; §6.2 OS-basis note references REQ-BLD-002 (ADR-003 still PROPOSED). Added citations B-33, F-23. No platform chosen; no ADR status changed; no measurement added. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §6.2 CMA unknowns: "the build tool is ADR-003 (PROPOSED)" → "the build tool is `rpi-image-gen` (ADR-003, ACCEPTED)". No budget, evidence, other ADR status or implementation status changed. | Claude (session 2026-10-07) |
