# PACSCORDER Video Encoder

| | |
|---|---|
| Document status | DRAFT — design reference built from source research. No encoder code exists. |
| Last updated | 2026-10-07 |
| Applies to | Pi 4 Model B, CM4, Pi 5, CM5 — the "Encoder" stage of REQ-ARCH-001; REQ-ENC-001 |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)). Nothing in this document has been tested on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 7, 8, 10, 22, 23, 25 |

| Item | Status |
|---|---|
| REQ-ENC-001 (video encoding) implementation | NOT STARTED |
| TEST-ENC-001 (sustained real-time H.264 encode) | BLOCKED — HARDWARE REQUIRED |
| TEST-DMA-001 (DMABUF capture → encoder buffer sharing) | BLOCKED — HARDWARE REQUIRED |
| ADR-004 product platform | OPEN |
| ADR-005 capture pixel format | PROPOSED (UYVY) — not accepted |
| ADR-007 userspace media framework | OPEN |

Fact references such as `[D-10]` point to [REFERENCES.md](REFERENCES.md). Facts of tier `community` are written as reports ("reported by …"). Calculations are marked **reasoning** and name their inputs.

---

## 1. What this document answers

This document answers the Rule 25 question "How is video encoded?" for each candidate platform.

Short answer: the encoder is **different on each platform pair**, and nothing about it is decided yet.

- **Pi 4 Model B / CM4 (BCM2711).** A hardware H.264 encoder inside the VideoCore firmware is exposed as a V4L2 memory-to-memory device by the downstream-only `bcm2835-codec` driver [D-02], [D-05], [D-08]. Its official specification is 1080p30 encode [D-10].
- **Pi 5 / CM5 (BCM2712).** There is no hardware video encoder. H.264 is encoded in software on the Arm CPU [D-31], [G-22].
- **Codec, bitrate, rate control, latency target and number of simultaneous encodes** are UNDEFINED — OWNER DECISION REQUIRED (OQ-005). The platform is OPEN (ADR-004, OQ-011). The framework is OPEN (ADR-007, OQ-015).

Position of the encoder in the mandated pipeline (REQ-ARCH-001):

```text
TC358743 → CSI-2 → Unicam (Pi 4/CM4) or RP1 CFE (Pi 5/CM5) → V4L2 capture node
        → DMABUF → Encoder → H.264 bitstream → Recorder / RTMP / WebRTC
```

Receivers: Unicam on Pi 4/CM4 [C-09]; RP1 CFE on Pi 5/CM5 [C-29], which is always used in Media Controller mode with this bridge [C-11].

Capture is described in [CSI_PIPELINE.md](CSI_PIPELINE.md) and [V4L2.md](V4L2.md). Buffer sharing is described in [DMA.md](DMA.md). Budgets and measurement plans are in [PERFORMANCE.md](PERFORMANCE.md). Consumers of the bitstream are described in [RECORDING.md](RECORDING.md) and [STREAMING.md](STREAMING.md).

---

## 2. Platform comparison: Pi 4 / CM4 versus Pi 5 / CM5

| Aspect | Pi 4 Model B / CM4 (BCM2711) | Pi 5 / CM5 (BCM2712) |
|---|---|---|
| Hardware H.264 encoder | Yes. `bcm2835-codec`, a V4L2 M2M driver over VCHIQ to the firmware component `ril.video_encode` [D-02], [D-08] | **No** [D-31], [G-22]. The `bcm2835-codec` module is built in `bcm2712_defconfig` [D-04] but cannot probe (reasoning) [D-28], [D-29] |
| Official encode figure | H.264 1080p30 encode [D-10] | "H264 1080p30 encode (from ISP) ~30–40% CPU", in software [G-22], [D-30] |
| 1080p60 encode | Unproven. Needs 2.0× the specified macroblock rate (reasoning) [D-52]. Raspberry Pi engineer 6by9 reported it as an "edge case" on the hardware encoder [D-50] (community). RISK-002, OQ-056 | No official figure. Raspberry Pi engineer 6by9 reported (forum, 2023-10-17) 1080p60 software encode from camera capture as "easily achievable" [D-50]. Not measured for TC358743 input. RISK-003, OQ-059 |
| Frame-size limit | 32×32 to 1920×1920; no 4K encode [D-09] | No hardware block. Raspberry Pi engineer 6by9 reported 4K software encode at "at least 20fps" [D-50] (community) |
| HEVC (H.265) encode | No [D-24] | No; only HEVC *decode* is in hardware [D-30], [D-31] |
| H.264 profiles | Baseline, Constrained Baseline, Main, High (default High) [D-11] | Depends on the software encoder. `openh264enc`: constrained-baseline, baseline, main, constrained-high, high [D-41]. x264 profile options: NEEDS VERIFICATION |
| B-frames | Never produced [D-14] | Encoder setting. `rpicam-apps` uses `max_b_frames=1` in normal mode [D-35]; its low-latency mode drops B-frames [D-32] |
| Bitrate range | 25 kbit/s – 25 Mbit/s, VBR or CBR [D-13] | Not researched for x264/openh264 — NEEDS VERIFICATION |
| Raw input formats | Read from firmware at probe; the driver table includes UYVY [D-16]. UYVY was in a 2021 listing posted by a Raspberry Pi engineer [D-17] | Planar and semi-planar YUV (plus GRAY8 for `x264enc`): `x264enc` [D-40], FFmpeg `libx264` [D-43], `openh264enc` (I420 only) [D-41]. **Packed UYVY is not accepted** by any of them [D-40], [D-41], [D-43] |
| UYVY from TC358743 | Reported by a Raspberry Pi engineer as accepted directly by the encoder [D-18] | Must be converted per frame before encoding [D-40], [D-43]. Offload path UNKNOWN (OQ-060) |
| Hardware format converter | `/dev/video12` simple ISP M2M device [D-25], [D-07] | No `bcm2835-codec` ISP, because `bcm2835-codec` cannot probe (reasoning) [D-29]. PiSP back end exists [D-51], [G-17]; standalone use UNKNOWN (OQ-060) |
| DMABUF into encoder | Yes, both queues, `videobuf2-dma-contig`, single plane [D-19], [D-20], [D-21] | No V4L2 encoder device. A software encoder reads frames from CPU-accessible memory (reasoning from [G-22]); see [DMA.md](DMA.md) |
| Latency | No source figure | Software encoders "generally output frames with a longer latency than the old hardware encoders" [D-32], [G-22] |
| GStreamer element | `v4l2h264enc` [D-37], [D-38] | `x264enc` (official replacement) [D-37], [D-40]; `openh264enc` [D-41] |
| FFmpeg encoder | `h264_v4l2m2m` [D-42] | `libx264` (requires `--enable-gpl`) [D-42] |
| Licensing notes | Encoder runs in the proprietary GPU firmware [D-08], [G-69] | x264 is GPL [D-47] (RISK-015) |
| CSI-2 lanes feeding the encoder (capture side) | Pi 4 Model B: 2 [C-01]. CM4: CAM0 2, CAM1 4 [C-02] | 4 per port [C-04], [C-05] |

Pi 4 Model B and CM4 share the BCM2711 encoder specification [D-10]. The researched sources show no encoder difference between them; the difference that matters for encoding is the number of capture lanes [C-01], [C-02] (§3.13). Pi 5 and CM5 share BCM2712; neither product brief nor the CM5 datasheet lists an encoder [D-31].

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

### 3.6 Encoder controls

All controls below are read from the driver source [D-11]–[D-15]. The "PACSCORDER value" column is not decided unless stated.

| Control | Range / values | Driver default | Source | PACSCORDER value |
|---|---|---|---|---|
| `V4L2_CID_MPEG_VIDEO_H264_PROFILE` | Baseline, Constrained Baseline, Main, High | High | [D-11] | UNDEFINED (OQ-005). Constrained Baseline for WebRTC: PROPOSED (§7) |
| `V4L2_CID_MPEG_VIDEO_H264_LEVEL` | 1.0 – 5.1 | 4.0 | [D-12] | Must match the encoded mode (§6) |
| `V4L2_CID_MPEG_VIDEO_BITRATE` | 25,000 – 25,000,000 bit/s, step 25,000 | 10,000,000 | [D-13] | UNDEFINED (OQ-005) |
| `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` | VBR, CBR | VBR | [D-13] | UNDEFINED (OQ-005) |
| `V4L2_CID_MPEG_VIDEO_B_FRAMES` | 0 only (min 0, max 0) | 0 | [D-14] | — (always 0) |
| `V4L2_CID_MPEG_VIDEO_GOP_SIZE` | 0 – 0x7FFFFFFF | 60 | [D-14] | UNDEFINED (OQ-005) |
| `V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER` | 0 / 1 (SPS/PPS inline with every IDR) | 0 (off) | [D-15] | 1 for streaming: PROPOSED (§7) |
| `H264_MIN_QP` | 0 – 51 | 20 | [D-15] | UNDEFINED |
| `H264_MAX_QP` | 0 – 51 | 51 | [D-15] | UNDEFINED |
| `FORCE_KEY_FRAME` | Control type and range not recorded in [D-15] — NEEDS VERIFICATION | — | [D-15] | Use is a design decision (ADR-007) |
| `HEADER_MODE` | joined with first frame | — | [D-15] | — |

Further facts:

- Every I-frame the encoder produces is an IDR frame [D-14].
- GStreamer `v4l2h264enc` sets these controls through its `extra-controls` property [D-53]. Control names attested in sources for that property are `repeat_sequence_header` [D-37], `h264_profile`, `h264_level` and `video_bitrate` (reported in a Raspberry Pi engineer's forum instructions, community source) [D-54]. The names of the other controls in `extra-controls` form are NEEDS VERIFICATION.

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

Status: zero-copy capture → encoder on PACSCORDER is NOT STARTED; TEST-DMA-001 is BLOCKED — HARDWARE REQUIRED. See [DMA.md](DMA.md) and REQ-DMA-001.

### 3.9 Encoded (CAPTURE) buffer size

- Default `sizeimage` of an encoded buffer: 768 KiB when width × height > 1280 × 720, otherwise 512 KiB. Clients may request a larger `sizeimage` [D-23].
- The driver notes that some frames of the 1080p "Big Buck Bunny" test sequence exceed 512 KiB [D-23].
- **Reasoning (inputs: maximum bitrate 25,000,000 bit/s [D-13]; 60 frames/s):** the *average* encoded frame at the maximum bitrate is 25,000,000 / 60 / 8 ≈ 52,083 bytes (≈ 51 KiB). The average does not bound individual frames.
- The worst-case IDR frame size at PACSCORDER's bitrate is UNKNOWN — HARDWARE TEST REQUIRED (OQ-056). Whether 768 KiB is enough is part of TEST-ENC-001.

### 3.10 No HEVC encoder

`bcm2835-codec`'s compressed formats are H264, JPEG, MJPEG, MPEG4, H263, MPEG2 and VC1_ANNEX_G. There is no HEVC. The separate Raspberry Pi HEVC driver (`VIDEO_RPI_HEVC_DEC`, module `rpi-hevc-dec`) is a stateless *decoder* only [D-24]. No candidate platform can encode HEVC in hardware [D-24], [D-31].

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

- Whether "~30–40% CPU" means all CPU cores together or one core. VENDOR CONFIRMATION REQUIRED (OQ-059). (The BCM2712 core count and core type are not in the source register.)
- Whether the figure applies to the TC358743 path. Reasoning: that path is not expected to come "from ISP", because Raspberry Pi engineers reported that libcamera does not support the bridge [C-41]. Its UYVY input must also be converted first [D-43].
- Cost at 1080p50 and 1080p60.
- Cost of several simultaneous encodes (recording + RTMP + WebRTC) (OQ-005).

Reported, not measured on PACSCORDER: Raspberry Pi engineer 6by9 wrote that software 1080p60 encode from camera capture "is easily achievable" [D-50] (community). The official `rpicam-vid` documentation says its low-latency mode "will still easily achieve 1080p30" [D-32].

The CPU budget and measurement plan are in [PERFORMANCE.md](PERFORMANCE.md).

### 4.3 Latency

- Official: Raspberry Pi 5's software encoders "generally output frames with a longer latency than the old hardware encoders" [D-32], [G-22]. No figure is given.
- Official: `rpicam-vid --low-latency` reduces latency by no longer using B-frames and arithmetic coding [D-32].
- `rpicam-apps` implements low-latency mode with `libx264` preset `ultrafast`, tune `zerolatency`, slice threading with 4 slices and `refs=1`. Both modes use motion estimation `dia` and `rc-lookahead 0` [D-35].
- Measured encode latency on PACSCORDER: UNKNOWN — HARDWARE TEST REQUIRED (OQ-059). The end-to-end latency target is UNDEFINED — OWNER DECISION REQUIRED (OQ-005, OQ-008). See [PERFORMANCE.md](PERFORMANCE.md).

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

### 4.6 Packages

- Raspberry Pi OS (trixie): x264 comes from Debian trixie (`2:0.164.3108+git31e19f9-2+b1`); `archive.raspberrypi.com` has no x264 package [G-30]. FFmpeg is the Raspberry Pi build `8:7.1.5-0+deb13u1+rpt2` [G-29]. Whether `x264enc` is packaged and installed on the PACSCORDER image is NEEDS VERIFICATION (BUILD TEST REQUIRED).
- Buildroot: `x264enc` comes from `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264`, which selects `BR2_PACKAGE_X264` [D-47], [E-36]. FFmpeg is built with `--enable-libx264` only when both `BR2_PACKAGE_X264=y` and `BR2_PACKAGE_FFMPEG_GPL=y` [D-46], [E-36].

### 4.7 Pi 5 versus CM5

Both use BCM2712 device trees (`bcm2712-rpi-5-b.dts`, `bcm2712-rpi-cm5.dtsi` [D-28]), and neither lists a hardware encoder [D-31]. Reasoning: the encoder design is therefore the same on both. Differences in cooling and sustained CPU throughput between the two boards are UNKNOWN — HARDWARE TEST REQUIRED (see [PERFORMANCE.md](PERFORMANCE.md), OQ-010, OQ-059).

---

## 5. Framework elements and encoder names

The userspace framework is OPEN (ADR-007, OQ-015). The names below are what each option would use.

| Framework | Pi 4 / CM4 (hardware) | Pi 5 / CM5 (software) |
|---|---|---|
| GStreamer | `v4l2h264enc`. Not a static element: it is registered at plugin load by probing M2M devices, with rank `GST_RANK_PRIMARY + 1` and klass `Codec/Encoder/Video/Hardware`. If the name is taken, it registers as `v4l2<basename>h264enc` [D-38], [G-23]. Probing is compiled in only when the meson option `v4l2-probe` is true; upstream defaults to true [D-38], [E-33]. In Buildroot this needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y`; without it, `v4l2h264enc` and `v4l2convert` are not registered [D-39], [E-32]. Outputs `stream-format=byte-stream`, `alignment=au` [F-35] | `x264enc` [D-40]; `openh264enc` [D-41]. Buildroot: `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264` [D-47] |
| FFmpeg | `h264_v4l2m2m` ("V4L2 mem2mem H.264 encoder wrapper"; built when `v4l2_m2m` is enabled) [D-42]. Upstream it uses MMAP only and forces B-frames to 0 [D-44]. Raspberry Pi OS's build adds DMABUF input [D-45] | `libx264`. It is in FFmpeg's `EXTERNAL_LIBRARY_GPL_LIST`, so FFmpeg must be built with `--enable-gpl` [D-42] |
| `rpicam-apps` (reference only, camera stack) | Detects VC4 from a V4L2 card named `bcm2835-isp` and uses its hardware `H264Encoder` [D-33] on `/dev/video11` with DMABUF input [D-34] | Detects PiSP from card `pispbe` and switches to libav with `libav_video_codec = "libx264"` [D-33], [D-35] |
| Direct V4L2 application | Opens the encoder M2M device itself (§3.3) | Links a software encoder library. The library APIs were not researched — NEEDS VERIFICATION |

**Official streaming example (fragments only)** [D-37]. The official Raspberry Pi documentation gives a GStreamer pipeline that uses:

- in the base pipeline (the hardware-encoder case, which applies to Pi 4/CM4 — reasoning): `v4l2h264enc extra-controls="controls,repeat_sequence_header=1"` with caps `video/x-h264,level=(string)4`;
- on Raspberry Pi 5: replace that encoder with `x264enc speed-preset=1 threads=1`, and replace the decoder `v4l2h264dec` with `avdec_h264`.

Notes on these fragments (reasoning):

- `level=(string)4` is enough for 1080p30, but not for 1080p50 or 1080p60 (§6, [F-40]).
- `speed-preset=1` is `ultrafast` [D-40]; the exact effect of the `threads` property is not in the source register (NEEDS VERIFICATION). Whether `x264enc speed-preset=1 threads=1` sustains 1080p50 or 1080p60 from TC358743 input is UNKNOWN — HARDWARE TEST REQUIRED (OQ-059).

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

### 6.1 Pitfall in the published TC358743 example

As reported in Raspberry Pi engineer 6by9's TC358743 install instructions (Pi 0–4, forum, 2020-08-05; community source) [D-54]:

- the example pipeline feeds UYVY from `v4l2src` straight into `v4l2h264enc`, with `extra-controls="controls,h264_profile=4,h264_level=10,video_bitrate=256000;"` and caps `framerate=30/1,format=UYVY`;
- `h264_level=10` is `V4L2_MPEG_VIDEO_H264_LEVEL_3_2` (**Level 3.2**), not level 4.x;
- `h264_profile=4` is **High** [D-54].

Reasoning: Table A-1 says 1080p30 needs at least Level 4 [F-40], so this example under-signals the level for any 1080p input. Its 256 kbit/s bitrate is the example's own value, not a PACSCORDER requirement. **PROPOSED (Claude's reasoning): PACSCORDER does not copy these values.** Level follows §6, and profile follows §7 and OQ-005.

### 6.2 WebRTC level negotiation

- `profile-level-id` `42e01f` decodes to Constrained Baseline Level 3.1 (reasoning) [F-38].
- libwebrtc defines `42e01f` as its Constrained Baseline profile-level-id, assumes Constrained Baseline Level 3.1 when `profile-level-id` is absent, and advertises Baseline, Constrained Baseline and Main at Level 3.1 [F-39].
- A strict `42e01f` negotiation does not cover 1080p [F-40].

Whether browsers decode a 1080p stream after a Level 3.1 negotiation, or PACSCORDER must signal Level 4.0 or 4.2, is UNKNOWN — HARDWARE TEST REQUIRED (OQ-073, RISK-019, TEST-STR-002). Details are in [STREAMING.md](STREAMING.md).

---

## 7. Streaming-relevant encoder settings

The settings below are **PROPOSED**: Claude's reasoning from the cited facts. Parameter values belong to OQ-005, and the framework is ADR-007 (OPEN). None has been implemented or tested.

| Setting | Why (source) | Pi 4 / CM4 hardware encoder | Pi 5 / CM5 software encoder | Status |
|---|---|---|---|---|
| Repeat SPS/PPS in-band with every IDR | RFC 7742: WebRTC SPS/PPS MUST be sent in-band, and `sprop-parameter-sets` MUST NOT be in SDP [F-36]. The GStreamer streaming example in the official Raspberry Pi documentation enables `repeat_sequence_header=1` [D-37] | `V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER` defaults to **0 (off)**, so it must be set to 1 [D-15] | x264/openh264 option name: NEEDS VERIFICATION | PROPOSED |
| No B-frames | The MediaMTX project reports that browsers deliberately do not support H.264 B-frames in WebRTC [F-45] (community). Official: dropping B-frames lowers latency [D-32] | Never produced [D-14]. Upstream FFmpeg `h264_v4l2m2m` also forces 0 [D-44] | `rpicam-apps` normal mode uses `max_b_frames=1` [D-35]; its low-latency mode drops them [D-32]. `x264enc` property name: NEEDS VERIFICATION | PROPOSED (RISK-019) |
| Constrained Baseline profile for the WebRTC output | RFC 7742 requires WebRTC browsers to implement H.264 Constrained Baseline [F-36]. libwebrtc's built-in encoder encodes only Constrained Baseline [F-39] | Available; driver default is High [D-11] | `openh264enc` offers constrained-baseline [D-41]. `x264enc` profile setting: NEEDS VERIFICATION | PROPOSED for WebRTC. Profile for recording/RTMP: UNDEFINED (OQ-005) |
| Signalled level matches the mode | Table A-1 [F-40]; §6 | Level control, default 4.0 [D-12] | `rpicam-apps` rule: 4.2 above 245,760 MB/s [D-36] | PROPOSED |
| Keyframe (IDR) interval and on-demand keyframes | Needed by viewers joining a stream (reasoning). Value UNDEFINED (OQ-005) | GOP default 60; `FORCE_KEY_FRAME` exists; every I-frame is IDR [D-14], [D-15] | NEEDS VERIFICATION | UNDEFINED |
| H.264 bitstream format for FLV/RTMP | `flvmux` needs `stream-format=avc` [F-34] (CORRECTED verdict); `v4l2h264enc` outputs `byte-stream`, so an `h264parse` (or equivalent) is needed between them [F-35] | applies | applies to any byte-stream encoder (reasoning) | Design input for ADR-007 |
| Colour description | TC358743 UYVY output is BT.601 limited range, `SMPTE170M` [B-34] | How to set the bitstream colour description: NEEDS VERIFICATION | NEEDS VERIFICATION | UNKNOWN (OQ-041) |
| Timestamps and frame rate | Input timestamps are copied to encoded buffers [D-19]. The TC358743 driver reports fractional rates such as 59.94 as integer-rate pixel clocks [B-28] | applies | applies | UNKNOWN (OQ-040) |

Codec choice context: legacy RTMP/FLV carries H.264 (AVC) as its only modern video codec; HEVC needs Enhanced RTMP [F-31]. No candidate platform encodes HEVC in hardware [D-24], [D-31]. Audio encoding (AAC for RTMP, Opus for WebRTC [F-31], [F-41]) is outside this document; see OQ-063.

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

On Pi 5/CM5, software encoding is unavoidable [G-22], [D-31], so the licence of whichever software encoder is chosen applies on those platforms (reasoning): x264 is GPL [D-47]; the openh264 licence was not researched (OQ-087). H.264 patent questions apply to hardware and software encoding alike (OQ-086). This is RISK-015, which is retired only by legal review (LEGAL CLARIFICATION REQUIRED).

---

## 9. Open items that block the encoder design

| OQ | Question (short) | Resolution marker |
|---|---|---|
| OQ-005 | Codec, bitrate, rate control, latency target, number of simultaneous encodes | OWNER DECISION REQUIRED |
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
| OQ-059 | Pi 5/CM5 software encode: CPU, thermal, latency, concurrency | HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED |
| OQ-060 | Pi 5/CM5 UYVY-to-planar conversion offload | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-061 | CMA budget per platform (encoder buffers included) | HARDWARE TEST REQUIRED |
| OQ-063 | Audio encoder choice and cost | HARDWARE TEST REQUIRED; LEGAL CLARIFICATION REQUIRED |
| OQ-073 | H.264 level signalling for 1080p WebRTC | HARDWARE TEST REQUIRED |
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

Risks: RISK-002 (Pi 4/CM4 1080p60 encode unproven), RISK-003 (no hardware encoder on Pi 5/CM5), RISK-015 (GPL and patent licensing), RISK-019 (WebRTC profile/level constraints), RISK-020 (CMA sizing).

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

**Check E-3 (all platforms): sustained encode.** This is TEST-ENC-001. See [PERFORMANCE.md](PERFORMANCE.md) §10.3 (procedure for TEST-ENC-001) for the metrics to record.

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

### Verified on PACSCORDER hardware

**Nothing** (no hardware exists as of 2026-10-06). Every statement above about PACSCORDER behaviour is a design input, not a result. The checks in §11 and TEST-ENC-001 / TEST-DMA-001 are BLOCKED — HARDWARE REQUIRED.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against REFERENCES.md. Changes: citations added ([D-01], [D-04], [D-05], [C-09], [C-11], [C-29]); reasoning and CORRECTED-verdict labels added; uncited claims removed or marked (quad-core CPU, `FORCE_KEY_FRAME` control type, `threads=1` meaning); Pi 4 vs CM4 and CM4 CAM1 1080p60 wording narrowed to what sources say; "decided with ADR-007" changed to OPEN wording; firmware-licence and Pi 5/CM5 GPL statements narrowed to the evidence; the libwebrtc `42e01f` statement aligned with [F-39]; cross-reference corrected to PERFORMANCE.md §10.3 | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: `gpu_freq=550` acceptability now points to OQ-096 (OQ-056 kept for the measurement) and OQ-096 added to the §9 table; [D-50] 1080p60/4K statements attributed to 6by9 with community label; the RISK-002 "at least 10 minutes" duration labelled as a research open question (OQ-010, OQ-017); §3.13 now says a 4-lane CM4 CAM1 port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s [C-47], OQ-038; link frequency ADR-008 / OQ-099); version-recording commands in check E-1 marked NEEDS VERIFICATION (OQ-101) Final verification pass (same date): the device-enumeration command in checks E-1 and E-2 linked to OQ-101, whose scope note now names it. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: §3.13 adds that Pi 4 Model B and CM4 CAM0 remain 2-lane candidates under REQ-CAP-007 (OQ-001 ANSWERED; ADR-004 OPEN), and that the 2-lane maximum of 1080p50 UYVY is also above the encoder's official 1080p30 specification (reasoning [D-10], [C-37], [C-48]; full-rate encode not specified, OQ-005). No new fact ID cited. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §3.1 consequence: "ADR-003 (OS/build basis, PROPOSED)" → "ADR-003 (OS/build basis, ACCEPTED: Raspberry Pi OS with `rpi-image-gen`)". No evidence, other ADR status (ADR-004 OPEN, ADR-005 PROPOSED, ADR-007 OPEN) or implementation status changed. | Claude (session 2026-10-07) |
