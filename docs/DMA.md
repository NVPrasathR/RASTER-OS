# DMA, Buffers and Memory

| | |
|---|---|
| Document status | DRAFT — derived from source research only |
| Last updated | 2026-10-08 |
| Applies to | Capture buffers after the CSI-2 receiver, CMA, DMA-BUF heaps and the hand-off to the encoder (H.264 only, owner 2026-10-08: "H.264 only for now", OQ-103; REQ-ENC-001. H.265 was added on 2026-10-07 and is now deferred as REQ-ENC-002 — not in current scope; its buffer path is kept in §8A and §9.5 as evidence) on Raspberry Pi 4 Model B, CM4, Pi 5 and CM5; kernel sources as read on `rpi-6.18.y` [D-01], [E-37] |
| Implementation status | NOT STARTED |
| Verification | Source research of 2026-10-06, plus research topics H (H.265) and I (timestamps) of 2026-10-08 ([REFERENCES.md](REFERENCES.md)). **Nothing in this document has been run or measured on PACSCORDER hardware. No hardware exists as of 2026-10-08.** |

This document covers what happens to a frame after the CSI-2 receiver has written it to memory:

- how large the buffers are;
- which allocator the capture driver uses on each platform;
- how CMA and DMA-BUF heaps are configured;
- what the Pi 4/CM4 hardware encoder requires of an imported buffer;
- which userspace paths avoid a CPU copy;
- what "zero-copy" can mean on Pi 5/CM5, where encoding runs in software;
- *(added 2026-10-08)* what H.265 requires of the frames on every platform, where it is always software-encoded (§8A) — *deferred — REQ-ENC-002; not in current scope (2026-10-08, OQ-103); kept as evidence*.

HDMI audio is required (owner, 2026-10-07; REQ-CAP-006), but it does not use these buffers. The `tc358743-audio` overlay routes audio from the TC358743 over I2S to an ALSA card [I-03], [I-04]. That path is described elsewhere ([TC358743_DRIVER.md](TC358743_DRIVER.md), [HARDWARE.md](HARDWARE.md)); only the timestamps that align it with video are noted here (§6).

Upstream: [CSI_PIPELINE.md](CSI_PIPELINE.md). Downstream: [VIDEO_ENCODER.md](VIDEO_ENCODER.md), [RECORDING.md](RECORDING.md), [STREAMING.md](STREAMING.md). Ioctl sequences: [V4L2.md](V4L2.md). Budgets and measurements: [PERFORMANCE.md](PERFORMANCE.md).

The governing requirement is **REQ-DMA-001 (DRAFT):** "Video frames shall pass from capture to encoder as DMABUF buffers without a CPU copy, where the platform encoder supports DMABUF import." The framework that will implement it is **ADR-007 (OPEN)**. The memory risk is **RISK-020**.

Conventions follow [README.md](README.md):

- `[D-19]` cites [REFERENCES.md](REFERENCES.md).
- Facts from `community` sources are worded as reports.
- Every calculation is marked **Reasoning** and names its inputs.
- `OQ-NNN` entries are in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).

---

## 1. Summary

| | Pi 4 Model B / CM4 | Pi 5 / CM5 |
|---|---|---|
| Capture driver memory | Unicam uses `videobuf2-dma-contig` with no IOMMU and allocates from CMA [C-53] | The RP1 CSI nodes sit behind `iommu5`. Whether CFE buffers come from CMA is not established [C-53] (OQ-053). |
| Encoder | Hardware `bcm2835-codec` encoder, `/dev/video11` [D-06]. Its input queue accepts DMABUF [D-19]. | No hardware video encoder [G-22], [D-31]. `/dev/video11` does not exist [D-29]. Encoding runs in software [G-22]; candidate encoders are `libx264` [D-43] and `x264enc` [D-40]. |
| Encoder accepts TC358743 UYVY? | Reported yes by a Raspberry Pi engineer [D-18] (community). The firmware decides the list [D-16] (OQ-057). | No: `libx264` and `x264enc` do not accept packed UYVY [D-43], [D-40] |
| Zero-copy capture → encoder | Possible by mechanism, subject to the constraints in §6. Unproven (OQ-058). | Does not apply to the encoder input. **Reasoning:** the encoder runs on the CPU [G-22], so the CPU reads every frame; packed UYVY must also be converted first [D-43], on the CPU unless a hardware converter is found (OQ-060). |
| Paths that copy every frame | Upstream FFmpeg `h264_v4l2m2m` (MMAP only) [D-44] | **Reasoning:** every software-encode path reads each frame with the CPU [G-22] |
| H.265 encoder (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | No hardware HEVC encoder [D-24]. Software x265 (`x265enc` [H-11], `libx265` [H-09]) on the CPU | No hardware HEVC encoder [D-31]. Software x265, as Pi 4 / CM4 [H-09], [H-11] |
| H.265 encoder accepts TC358743 UYVY? (added 2026-10-08; deferred — REQ-ENC-002) | **No.** Planar only, not UYVY or NV12 [H-10] (CORRECTED), [H-13] | **No**, as Pi 4 / CM4 [H-10], [H-13] |
| Zero-copy for H.265 (added 2026-10-08; deferred — REQ-ENC-002) | Does not apply. **Reasoning:** x265 runs on the CPU and needs a planar copy of every frame. At 1080p60 the conversion reads ≈ 249 MB/s and writes ≈ 187 MB/s [H-43] (§8A) | as Pi 4 / CM4 (§8A) |
| Status | NOT STARTED; TEST-DMA-001 BLOCKED — HARDWARE REQUIRED | NOT STARTED; TEST-DMA-001 BLOCKED — HARDWARE REQUIRED |

---

## 2. Capture buffer sizes

**Reasoning fact [C-53] (CORRECTED).**

- One 1920×1080 frame is 4,147,200 bytes in UYVY and 6,220,800 bytes in RGB888 (BGR3).
- Four capture buffers therefore take about 16.6 MB (UYVY) or 24.9 MB (RGB888).

The number of capture buffers is a design parameter of the capture application, which does not exist yet: **UNKNOWN — VERIFICATION REQUIRED (OWNER DECISION REQUIRED, together with ADR-007; size it from TEST-PERF-001 measurements; the CMA budget it drives is OQ-061).**

**Reasoning** (inputs: frame sizes [C-53]; 64 MiB DT CMA pool [E-47]). The percentages are of the base Device Tree pool only. Overlays can change the pool size (§4.2), and on Pi 5/CM5 it is not established that CFE buffers come from CMA at all (§3.2, OQ-053).

| Buffers | UYVY bytes | % of 64 MiB | RGB888 bytes | % of 64 MiB |
|---|---|---|---|---|
| 4 | 16,588,800 | 24.7 % | 24,883,200 | 37.1 % |
| 6 | 24,883,200 | 37.1 % | 37,324,800 | 55.6 % |
| 8 | 33,177,600 | 49.4 % | 49,766,400 | 74.2 % |

**Reasoning, line size** (inputs [C-53], [D-22]):

- [C-53]'s frame sizes imply 2 bytes per pixel for UYVY and 3 for RGB888.
- An unpadded 1920-pixel line is therefore 3,840 bytes (UYVY) or 5,760 bytes (RGB888).
- Unicam's actual `bytesperline` (any padding) is not in the source register. **NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED; OQ-058).**

**Reasoning, cross-check** (inputs [C-53], [C-46]):

- 4,147,200 bytes × 60 frames/s × 8 = 1.990656 Gbit/s. That is exactly the 1080p60 UYVY CSI-2 payload.
- So at 1080p60 UYVY the receiver writes about 248.8 MB/s into memory.

---

## 3. Where the capture buffers come from

### 3.1 Pi 4 Model B / CM4 — Unicam

- Unicam uses `videobuf2-dma-contig`. There is no IOMMU, so its buffers are allocated from CMA [C-53].
- **Reasoning** (inputs [C-53], [D-20]): a `videobuf2-dma-contig` buffer allocated without an IOMMU is one DMA-contiguous region. That is the property the encoder's import check requires (§6). **HARDWARE TEST REQUIRED (TEST-DMA-001).**

### 3.2 Pi 5 / CM5 — RP1 CFE

- The Device Tree gives the RP1 CSI nodes `iommus = <&iommu5>`, with `CONFIG_BCM2712_IOMMU=y` [C-53].
- The HVS (display) uses `iommu4` [C-53].
- It is therefore **not established** that CFE capture buffers, or display buffers, count against the 64 MB `vc4-kms-v3d-pi5` CMA default [C-53] (CORRECTED).
- Which `videobuf2` memory operations CFE uses is not in the source register.

**UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED: read `CmaFree` in `/proc/meminfo` while streaming [C-53]; OQ-053).**

---

## 4. CMA configuration

### 4.1 Kernel configuration versus Device Tree

- Both Raspberry Pi arm64 defconfigs set `CONFIG_CMA=y`, `CONFIG_DMA_CMA=y` and `CONFIG_CMA_SIZE_MBYTES=5` [E-47], [G-19].
- The CMA region actually used comes from the Device Tree `cma: linux,cma` node: `shared-dma-pool`, reusable, `linux,cma-default`, size 0x4000000 = 64 MB [E-47] (CORRECTED).
- Placement:
  - On Pi 4 Model B and CM4, `bcm2711-rpi-ds.dtsi` overrides the allocation range to the lower 768 MB [E-47].
  - On Pi 5/CM5, `bcm2712.dtsi` limits it to the lower 1 GB [E-47], [C-40].

### 4.2 Overlay parameters and defaults

- The `vc4-kms-v3d` overlay and the `cma` overlay take the CMA size parameters `cma-64`, `cma-96`, `cma-128`, `cma-192` … `cma-512`, and `cma-size` (bytes, 4 MB aligned). Sizes of `cma-192` and above need 1 GB [C-40].
- The `cma` overlay also offers `cma-default` [E-47].

| Source of the CMA size | Pi 4 Model B / CM4 | Pi 5 / CM5 | Source |
|---|---|---|---|
| Base Device Tree pool | 64 MB, lower 768 MB | 64 MB, lower 1 GB | [E-47], [C-40] |
| `vc4-kms-v3d` overlay default | (512 − 4) MB (`vc4-kms-v3d-pi4`) | 64 MB (`vc4-kms-v3d-pi5`) | [C-40] |
| `cma` overlay default | 256 MB | 256 MB | [C-40] |

**Reasoning** (inputs: the table above): the effective CMA size on a PACSCORDER image depends on which of these overlays its `config.txt` loads, and whether the product drives a display at all. The product `config.txt` does not exist yet ([BUILD_SYSTEM.md](BUILD_SYSTEM.md); ADR-003 ACCEPTED). **Effective CMA size: UNKNOWN — VERIFICATION REQUIRED (OWNER DECISION REQUIRED, HARDWARE TEST REQUIRED; OQ-061).**

### 4.3 Other CMA users

- On Pi 4/CM4 the codec and ISP drivers select `BCM2835_VCHIQ_MMAL`, which selects `BCM_VC_SM_CMA` (the `vc-sm-cma` shared-memory driver) [D-03], [E-46]. How much CMA it consumes is not in the source register.
- The encoder's queues also use `videobuf2-dma-contig` [D-19]. That its MMAP buffers come from CMA, as Unicam's do, is expected but **NEEDS VERIFICATION**.
- The encoded (CAPTURE) buffers default to 768 KiB each above 720p [D-23]. **Reasoning:** four of them would take 3,145,728 bytes. The buffer count is a design choice.

### 4.4 Budget and measurement

**Recommendation — PROPOSED, Claude's recommendation; no ADR records it. OWNER DECISION REQUIRED.**

- Set the CMA size explicitly in the product `config.txt` with an attested `cma-*` parameter [C-40].
- Size it from a measurement of `CmaFree` in `/proc/meminfo` while capturing and encoding at the highest supported mode [C-53].
- Do not rely on whichever default the loaded overlays happen to give.

The measurement is part of TEST-PERF-001, which is **BLOCKED — HARDWARE REQUIRED**. Allocation failure at STREAMON is the RISK-020 failure mode. The per-platform budget is OQ-061.

---

## 5. DMA-BUF heaps

| Item | Fact | Source |
|---|---|---|
| Kernel support | `CONFIG_DMABUF_HEAPS=y`, `CONFIG_DMABUF_HEAPS_SYSTEM=y`, `CONFIG_DMABUF_HEAPS_CMA=y` in both Raspberry Pi arm64 defconfigs | [E-48], [G-19] |
| CMA heap names, `rpi-6.18.y` | The default CMA heap is registered as `default_cma_region`. When `DMABUF_HEAPS_CMA_LEGACY` is set (default y), a second heap named after the CMA area is also registered. | [E-48] |
| CMA heap name, 6.12.61 | Only one heap, named `cma_get_name(cma)` | [E-48] |
| Kernel per OS option | Raspberry Pi OS 2026-10-06 ships 6.18.50. Buildroot 2026.08 pins 6.12.61. | [G-04], [E-06] |

**Heap names on the target**

- The research expected the legacy heap name to come from the DT node name `linux,cma`. That expectation is a research open question (topic E), not a register fact.
- The heap device names userspace opens (`/dev/dma_heap/*`, a path taken from that research open question, not from a register fact) on the chosen kernel: **UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED; OQ-062).**

**Toolchain header caveat**

- The Bootlin toolchain used by Buildroot 2026.08's Pi defconfigs declares kernel headers of at least 5.4 [E-12].
- The research found that `linux/dma-heap.h` first appears in Linux v5.6. That is a research gap (topic E), not a register fact.
- **Reasoning:** if both hold, an application built with that toolchain that uses DMA-heap ioctls must carry its own copy of the uapi header, or use a toolchain with headers of 5.6 or later.
- The Raspberry Pi OS native toolchain was not researched.

**UNKNOWN — VERIFICATION REQUIRED (BUILD TEST REQUIRED against the chosen toolchain; OQ-062, OQ-064).**

**Contiguity**

- The Pi 4/CM4 encoder imports only buffers that are one DMA-contiguous region [D-20]. **Reasoning:** on Pi 4/CM4 the CMA heap is therefore the candidate heap for any buffer that must reach the encoder. That CMA-heap buffers are contiguous is not itself a register fact (NEEDS VERIFICATION).
- Whether system-heap buffers are contiguous is not in the source register. Treat them as unsuitable for `bcm2835-codec` import until TEST-DMA-001 shows otherwise. **NEEDS VERIFICATION.**

**PROPOSED** (Claude's reasoning from [E-48]; to be decided with ADR-007): if PACSCORDER allocates from a heap, the application discovers the heap name at runtime instead of hard-coding a path. The name differs between kernel series.

Heaps are needed only if the application allocates the buffers itself (Model B in §10).

---

## 6. Pi 4 / CM4 hardware encoder: what an imported buffer must satisfy

| Constraint | Source | 1920×1080 UYVY capture buffer | Status |
|---|---|---|---|
| Encoder node is `/dev/video11` (`encode_video_nr=11`), card `bcm2835-codec-encode`, a multiplanar M2M device backed by VideoCore firmware over VCHIQ | [D-06], [D-07], [D-08] | — | — |
| Both queues support `VB2_MMAP` and `VB2_DMABUF` with `vb2_dma_contig_memops`. Input timestamps are copied to the encoded buffers (`V4L2_BUF_FLAG_TIMESTAMP_COPY`). | [D-19] | The capture timestamp follows the frame into the bitstream buffer. *(Added 2026-10-08.)* That timestamp is `CLOCK_MONOTONIC`, taken at frame start by Unicam (and by CFE on Pi 5/CM5) [I-33]. Reasoning: the encoded H.264 buffers therefore carry a monotonic capture time, which the application can use to align video with audio (RISK-024, OQ-112) | NOT STARTED |
| An imported DMABUF must be one DMA-contiguous region at least as large as the plane. Otherwise import fails with `contiguous chunk is too small`. | [D-20] | Unicam buffers are `dma-contig` from CMA with no IOMMU [C-53]; **Reasoning:** expected to satisfy this | HARDWARE TEST REQUIRED (OQ-058) |
| Always one memory plane (`num_planes = 1`), even for YUV420/NV12. One fd holds all planes contiguously. | [D-21] | One fd per frame is needed; Unicam's plane layout: NEEDS VERIFICATION | KERNEL SOURCE INSPECTION REQUIRED |
| ENCODE role rounds `bytesperline` up to a 64-byte multiple for YUV420/YVU420 and YUYV/UYVY/YVYU/VYUY, and to 32 bytes for NV12/NV21/RGB24/BGR24 | [D-22] | **Reasoning:** 3,840 = 60 × 64, so no padding is needed *if* Unicam's stride is 3,840 | NEEDS VERIFICATION (OQ-058) |
| ENCODE role does not align height to 16 | [D-22] | 1,080 lines are used as-is | — |
| Encode frame size is clamped to 1920 × 1920 | [D-09] | 1080p is within the limit | — |
| Input formats are read from the firmware at probe and filtered by a static table that includes UYVY | [D-16] | A 2021 listing posted by a Raspberry Pi engineer showed UYVY [D-17]. A Raspberry Pi engineer stated that both TC358743 formats are accepted directly [D-18]. (Both community.) | HARDWARE TEST REQUIRED (OQ-057) |
| Encoded output buffers: default 768 KiB above 720p; clients may request larger. Some 1080p frames exceed 512 KiB. | [D-23] | Whether the default is enough at the chosen bitrate | HARDWARE TEST REQUIRED (OQ-056) |
| The cut-down firmware (`gpu_mem=16`) removes codec support | [D-48] | The product `config.txt` must keep codec-capable firmware | OQ-048 |

**Reasoning — stride at other widths** (inputs: 2 bytes per UYVY pixel [C-53]; 64-byte encoder alignment [D-22]; driver minimum width 640 [A-08]):

An unpadded UYVY line is 2 × width bytes. It is a multiple of 64 only when the width is a multiple of 32 pixels.

| Width | Unpadded UYVY line | Multiple of 64? |
|---|---|---|
| 1920 | 3,840 B | Yes |
| 1280 | 2,560 B | Yes |
| 720 | 1,440 B | **No** |
| 640 | 1,280 B | Yes |

Which input modes PACSCORDER supports is OQ-002. If capture and encoder strides differ, how the mismatch would be handled (capture-side padding, an ISP conversion step or a copy): **UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED; OQ-058).**

**Alternative input path: the ISP converter**

- `/dev/video12` is a simple M2M converter/scaler with MMAP and DMABUF on both queues. It is a candidate for UYVY → YUV420/NV12 conversion between capture and encoder [D-25], [D-07].
- `rpicam-apps` feeds the encoder YUV420 [D-34].
- Whether direct UYVY input costs encoder throughput, compared with routing through the ISP: **UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, VENDOR CONFIRMATION REQUIRED; OQ-057).**

---

## 7. Userspace paths: which avoid a CPU copy

"No copy" below means no CPU copy of raw frames between capture and the encoder input. It is a property of the mechanism as described by the sources. **None has been run on PACSCORDER hardware.**

| Path | Platform | How raw frames reach the encoder | CPU copy of raw frames? | Evidence | Caveats |
|---|---|---|---|---|---|
| **GStreamer** `v4l2h264enc` with `output-io-mode=dmabuf-import` | Pi 4/CM4 | The encoder's OUTPUT queue imports DMABUFs supplied by the upstream element | No, *if* the upstream buffers are DMABUFs that satisfy §6 | [D-53], [D-19], [D-20] | Whether `v4l2src` on Unicam supplies such buffers: NEEDS VERIFICATION (OQ-058). `v4l2h264enc` exists only when V4L2 M2M probing is compiled in [D-38]. Debian trixie's plugins-good 1.26.2, which the Raspberry Pi archive does not override, ships the probed M2M elements [G-26]. Buildroot needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y` [D-39]. |
| **Direct V4L2 application** (the pattern `rpicam-apps` uses) | Pi 4/CM4 | Capture buffers are queued on the `/dev/video11` OUTPUT queue as `V4L2_MEMORY_DMABUF`. The H.264 bitstream is read from MMAP CAPTURE buffers. | No | [D-34] | PACSCORDER can reuse the pattern, not the program: whether `rpicam-apps` can capture from the TC358743 at all is not established, and Raspberry Pi engineers have stated that libcamera does not support the bridge [C-41] (community). `rpicam-apps` feeds YUV420 [D-34], not UYVY. |
| **FFmpeg, upstream** `h264_v4l2m2m` | Pi 4/CM4 | Buffers are allocated with `V4L2_MEMORY_MMAP` only | **Yes — every frame is copied** into driver buffers | [D-44] (read from FFmpeg master) | Buildroot master packages upstream FFmpeg 6.1.5 with patches 0001–0007 only, none of which add Raspberry Pi V4L2/DRM_PRIME support [D-46] (CORRECTED). Whether 6.1.5 is also MMAP-only is a research open question (topic D): NEEDS VERIFICATION (source inspection of the 6.1.5 tarball). The encoder wrapper forces B-frames to 0 [D-44]. |
| **FFmpeg, Raspberry Pi patched** (`DRM_PRIME`) | Pi 4/CM4 | When `pix_fmt` is `AV_PIX_FMT_DRM_PRIME`, the OUTPUT queue uses `V4L2_MEMORY_DMABUF`. `rpicam-apps` sets `DRM_PRIME` for `h264_v4l2m2m`. | No at the encoder input, *if* frames arrive as `DRM_PRIME` | [D-45] | Raspberry Pi OS installs Raspberry Pi's FFmpeg 7.1.5 build in preference to Debian's [G-29]. That the shipped binary contains the [D-45] patch: NEEDS VERIFICATION. How FFmpeg would obtain `DRM_PRIME` frames from a TC358743 capture node: not researched (NEEDS VERIFICATION). Whether FFmpeg can output WebRTC is not covered by any register fact (NEEDS VERIFICATION); ADR-007 marks its "no WebRTC" point the same way. |
| **Software encode** (`libx264` via FFmpeg, `x264enc` via GStreamer) | Pi 5/CM5 (on Pi 4/CM4 only a candidate fallback, not researched; OQ-056) | The CPU reads each frame through a mapping, converts UYVY to a planar/semi-planar format, then encodes | **Yes — the CPU reads every frame** (conversion and encode) | [D-43], [D-40], [G-22] | See §8 |
| **Software H.265 encode** (`libx265` via FFmpeg, `x265enc` via GStreamer) — added 2026-10-08; deferred — REQ-ENC-002; not in current scope | **All four platforms**: no hardware HEVC encoder [D-24], [D-31] | The CPU reads each frame through a mapping, converts UYVY to a **planar** format (I420 or Y42B; NV12 is not accepted), then encodes | **Yes — the CPU reads every frame** and writes a planar copy (reasoning [H-43]) | [H-09], [H-10] (CORRECTED), [H-11], [H-13], [H-43] | See §8A. On Pi 4/CM4 it could run beside the DMABUF hardware H.264 path, from the same capture buffer (candidate; §8A) |

The framework choice is **ADR-007 (OPEN)**, OQ-015. Encoder settings are in [VIDEO_ENCODER.md](VIDEO_ENCODER.md).

---

## 8. Pi 5 / CM5: software encoding from CPU-mapped buffers

**No hardware encoder.**

- Official documentation: "Raspberry Pi 5 uses software video encoders". On BCM2712 "Other CODECs run in software", with only HEVC decode in hardware [G-22].
- The Pi 5 and CM5 product briefs list no video encoder [D-31].
- The VCHIQ driver that hosts `bcm2835-codec` has no node in the BCM2712 Device Trees, so `bcm2835-codec` cannot probe and `/dev/video11` does not exist [D-28], [D-29].
- The BCM2712 DT comment mentions "(unused) H264 accelerators" behind `iommu2`, but no H.264 node or driver is instantiated [D-51].
- A Raspberry Pi engineer (jamesh) stated on the forum that BCM2712 has no H.264 hardware block for encode or decode [D-50] (community).

**Input formats force a conversion.**

| Encoder | Accepted 8-bit raw input | Packed UYVY? | Source |
|---|---|---|---|
| FFmpeg `libx264` | YUV420P, YUVJ420P, YUV422P, YUVJ422P, YUV444P, YUVJ444P, NV12, NV16, NV21 | No | [D-43] |
| GStreamer `x264enc` | Y444, Y42B, I420, YV12, NV12, GRAY8 (plus 10-bit variants) | No | [D-40] |
| GStreamer `openh264enc` | I420 only | No | [D-41] |
| FFmpeg `libx265` (H.265, deferred — REQ-ENC-002; added 2026-10-08) | yuv420p, yuvj420p, yuv422p, yuvj422p, yuv444p, yuvj444p, gbrp, gray8; no NV12 (plus 10/12-bit planar variants with Debian's library; CORRECTED) | No | [H-10] |
| GStreamer `x265enc` (H.265, deferred — REQ-ENC-002; added 2026-10-08) | Y444, Y42B, I420 (plus 10/12-bit planar variants); no NV12 | No | [H-13] |

TC358743 UYVY frames must therefore be converted first, for example with FFmpeg's swscale [D-43].

*(Added 2026-10-08.)* Reasoning (inputs: the table above): I420 is the one format that all five encoders accept. A conversion that outputs NV12 can feed `libx264` and `x264enc` but not x265. If H.264 and H.265 run together from one conversion, that conversion must output a planar format such as I420 (ADR-007; OQ-060). H.265 applies on Pi 4/CM4 too; see §8A. *(2026-10-08, later: H.265 is deferred — REQ-ENC-002; not in current scope. In the current scope the conversion feeds software H.264 only, so NV12 and I420 are both candidate outputs for `libx264` and `x264enc` [D-43], [D-40]; `openh264enc` needs I420 [D-41].)*

**Hardware conversion.** The PiSP back end is in the DT behind `iommu2` [D-51] and is built as a module [G-17]. Raspberry Pi engineers have stated that libcamera does not support the TC358743 [C-41] (community). Whether any BCM2712 block can do this conversion outside libcamera: **UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED; OQ-060).**

**Cost**

- The only official figure is "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22], [D-30].
- Whether that means the whole CPU or one core is OQ-059.
- The source register holds no official figure for 1080p60 encode or for the UYVY conversion.
- One Raspberry Pi engineer (6by9) reported that software 1080p60 encode from camera capture is "easily achievable (which was edge case on the hardware encode)" [D-50] (community). This is a single community report, not an official figure or a PACSCORDER measurement.

**Reasoning — conversion read traffic** (input: UYVY frame size 4,147,200 bytes [C-53]). The conversion step alone reads:

| Mode | Read traffic |
|---|---|
| 1080p60 | 248,832,000 B/s |
| 1080p50 | 207,360,000 B/s |
| 1080p30 | 124,416,000 B/s |

The converter's writes and the encoder's reads come on top. Their sizes are not in the source register. CPU load, temperature and latency: **UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED; OQ-059; TEST-ENC-001, TEST-PERF-001).** *(Superseded in part, 2026-10-08: for a conversion to I420, the write size is now in the register as reasoning fact [H-43]: 3,110,400 bytes per 1920×1080 frame, about 187 MB/s at 1080p60. The write rates for all three modes are in §8A. The encoder's own reads are still not in the register.)*

**What "zero-copy" can mean on Pi 5/CM5** (Reasoning; candidates, not decisions):

1. The converter reads directly from the mapped capture buffer, with no intermediate `memcpy` before conversion.
2. One capture buffer is shared by several consumers (recording, RTMP and WebRTC encoders) instead of being copied per consumer. How many encodes are needed is OQ-005.
3. If OQ-060 finds a usable hardware converter and a test shows it handling TC358743 frames, the capture buffer is handed to it as a DMABUF.

REQ-DMA-001 applies "where the platform encoder supports DMABUF import". **Reasoning:** no Pi 5/CM5 encoder does, so the requirement's no-copy clause does not bind the encoder input there. The owner must confirm this reading when accepting REQ-DMA-001 (OWNER DECISION REQUIRED; OQ-017).

**Page size.**

- Raspberry Pi's `bcm2712_defconfig` builds `kernel_2712.img` with 16K pages (`CONFIG_ARM64_16K_PAGES=y`), and Raspberry Pi OS's default BCM2712 kernel uses 16K pages [E-51], [G-20], [G-60]. Raspberry Pi documents that the 4K-page `kernel8.img` also runs on BCM2712 [G-20].
- Buildroot's Pi 5/CM5 defconfigs force 4K pages [E-09], [G-60].
- Whether this changes DMABUF behaviour or performance is OQ-055.

---

## 8A. H.265 (HEVC) on every platform: conversion to planar input (deferred — REQ-ENC-002; not in current scope)

*(Section added 2026-10-08 from research topic H. The owner requires H.264 **and** H.265 (REQ-ENC-001, 2026-10-07). Which outputs use H.265 is OQ-103. Nothing here has been run.)*

**Scope (2026-10-08, later): deferred — REQ-ENC-002; not in current scope.** The owner answered OQ-103 with "H.264 only for now": every output uses H.264 (REQ-ENC-001), and H.265 is recorded as REQ-ENC-002 (DEFERRED; no tests planned). This section is kept unchanged as the evidence for REQ-ENC-002 and applies only if the owner re-activates it. In the current scope the x265 conversion and the CM4 two-consumer case below (hardware H.264 plus software H.265 from one capture buffer) do not arise. RISK-022 and OQ-104 stay OPEN but are not in current scope.

**No hardware HEVC encoder anywhere.** Pi 4/CM4 has none [D-24], and Pi 5/CM5 has none [D-31]. Reasoning: H.265 is encoded by x265 on the CPU on all four platforms, through GStreamer `x265enc` [H-11] or FFmpeg `libx265` [H-09]. No V4L2 encoder device exists for H.265, so nothing imports a DMABUF for it.

**Planar input only.** `x265enc` accepts Y444, Y42B and I420 (plus 10/12-bit planar variants), not UYVY, YUY2 or NV12 [H-13]. `libx265` accepts planar or gray formats only, never `uyvy422`, `yuyv422` or `nv12` [H-10] (CORRECTED). Every TC358743 UYVY frame must therefore be converted before x265 sees it. On Pi 4/CM4 this applies even though the hardware H.264 encoder takes UYVY directly (reported by a Raspberry Pi engineer [D-18], community).

**Reasoning — conversion memory traffic for H.265** (inputs: UYVY 1920×1080 × 2 bytes = 4,147,200 bytes per frame [C-53], [H-43]; I420 1920×1080 × 1.5 bytes = 3,110,400 bytes per frame [H-43]; frame rates from [C-46]). The 1080p60 row is [H-43]; the 1080p50 and 1080p30 rows use the same arithmetic:

| Mode | UYVY read | I420 written | Read + write |
|---|---|---|---|
| 1080p60 | 248,832,000 B/s (≈ 249 MB/s) | 186,624,000 B/s (≈ 187 MB/s) | 435,456,000 B/s |
| 1080p50 | 207,360,000 B/s | 155,520,000 B/s | 362,880,000 B/s |
| 1080p30 | 124,416,000 B/s | 93,312,000 B/s | 217,728,000 B/s |

- This traffic comes before x265 starts [H-43]. x265's own memory traffic is not in the source register.
- If the conversion targets Y42B (planar 4:2:2) instead, the write size differs. It is not in the source register, so no row is given.
- The CPU time of the conversion, and whether it competes with x265 for memory bandwidth, are UNKNOWN — HARDWARE TEST REQUIRED (OQ-104, OQ-060).

**Buffers.** One I420 1920×1080 frame is 3,110,400 bytes (reasoning [H-43]). The number of conversion buffers is a framework choice (ADR-007). Whether they are allocated from CMA (§4) is NEEDS VERIFICATION. Reasoning: a CPU-only consumer does not need contiguous memory, but this is not a register fact. The total memory budget is OQ-061.

**Hardware conversion candidates.**

| Platform | Candidate | Status |
|---|---|---|
| Pi 4 Model B / CM4 | `/dev/video12` ISP M2M converter, DMABUF on both queues [D-25], [D-07] | Whether it converts TC358743 UYVY to I420 at 1080p60 for x265: UNKNOWN — HARDWARE TEST REQUIRED (OQ-057) |
| Pi 5 / CM5 | PiSP back end [D-51], [G-17] | UNKNOWN — KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED (OQ-060) |

**Pi 4 / CM4: two consumers of one capture buffer** (reasoning; candidate, not decided). If H.264 runs on the hardware encoder and H.265 in software, one Unicam capture buffer could be:

1. imported as a DMABUF by `/dev/video11` for H.264 (§6, §9.1); and
2. mapped by the CPU (or passed to `/dev/video12`) for the H.265 conversion.

Whether the frameworks under ADR-007 can hand one capture buffer to both consumers at once, and how that affects the number of capture buffers in flight and the CMA budget, is UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED; OQ-058, OQ-061).

**REQ-DMA-001 reading.** REQ-DMA-001 applies "where the platform encoder supports DMABUF import". Reasoning: x265 is a CPU library on every platform, so the no-copy clause does not bind the H.265 encoder input anywhere. It still binds the H.264 hardware path on Pi 4/CM4. The owner must confirm this reading when accepting REQ-DMA-001 (OWNER DECISION REQUIRED; OQ-017), as for Pi 5/CM5 in §8.

---

## 9. DMABUF flow per platform

These are **candidate** flows. They depend on ADR-004 (platform, OPEN), ADR-005 (UYVY, PROPOSED) and ADR-007 (framework, OPEN). None is decided and none has been run.

### 9.1 Pi 4 Model B / CM4 — candidate A: UYVY straight into the hardware encoder

```text
TC358743 ──CSI-2──► Unicam (csi0 / csi1)                                   [C-07], [C-08]
                      │ videobuf2-dma-contig, no IOMMU                       [C-53]
                      ▼
               CMA pool (DT linux,cma 64 MB; overlay may change size)      [E-47], [C-40]
               capture buffer: UYVY 1920x1080 = 4,147,200 B               [C-53]
                      │ DMABUF fd
                      │ (how the fd is obtained from the capture queue:
                      │  NEEDS VERIFICATION — see §10)
                      ▼
               /dev/video11  bcm2835-codec-encode, OUTPUT queue            [D-06], [D-08]
               V4L2_MEMORY_DMABUF, one contiguous plane,                   [D-19], [D-20], [D-21]
               bytesperline multiple of 64 (UYVY)                          [D-22]
               UYVY accepted? reported yes (community); OQ-057             [D-16], [D-18]
                      │ VideoCore firmware H.264 encode
                      ▼
               CAPTURE queue: H.264, MMAP, 768 KiB default per buffer      [D-23], [D-34]
               (input timestamp copied to the output buffer)               [D-19]
                      ▼
               recorder / RTMP / WebRTC  (RECORDING.md, STREAMING.md)
```

### 9.2 Pi 4 Model B / CM4 — candidate B: through the ISP converter

```text
Unicam capture buffer (UYVY, CMA)                                  [C-53]
   │ DMABUF
   ▼
/dev/video12  ISP converter, DMABUF on both queues                 [D-25]
   │ UYVY → YUV420 / NV12  (extra buffers; sizes NEEDS VERIFICATION)
   ▼ DMABUF
/dev/video11  encoder OUTPUT queue (YUV420, as rpicam-apps feeds)  [D-34]
   ▼
H.264 CAPTURE (MMAP)
```

### 9.3 Pi 4 Model B / CM4 — for contrast: upstream FFmpeg

```text
Unicam capture buffer ──CPU copy every frame──► h264_v4l2m2m MMAP OUTPUT buffer   [D-44]
```

This path does not satisfy REQ-DMA-001.

### 9.4 Pi 5 / CM5

```text
TC358743 ──CSI-2──► RP1 CFE "csi2" ──► rp1-cfe-csi2_ch0               [C-32]
                      (D-PHY programmed for 999 Mbps)                    [C-31]
                      │ RP1 CSI behind iommu5                             [C-53]
                      ▼
               capture buffer: UYVY (from CMA or not: UNKNOWN, OQ-053)    [C-53]
                      │ CPU mapping (V4L2 MMAP or DMABUF mmap: NEEDS VERIFICATION)
                      ▼
               CPU conversion UYVY → I420 / NV12 (e.g. swscale)          [D-43], [D-40]
               (hardware offload, e.g. PiSP back end: UNKNOWN, OQ-060)   [D-51]
                      ▼
               libx264 / x264enc software H.264                   [G-22], [D-43], [D-40]
               (official figure for 1080p30 from ISP: ~30–40 % CPU)      [G-22]
                      ▼
               recorder / RTMP / WebRTC
```

### 9.5 All platforms — H.265 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope)

```text
capture buffer: UYVY 1920x1080 = 4,147,200 B                             [C-53]
  (Unicam, CMA, Pi 4/CM4 · RP1 CFE, CMA or not: OQ-053, Pi 5/CM5)
                      │ CPU mapping (or /dev/video12 on Pi 4/CM4: OQ-057)
                      ▼
               UYVY → I420 (planar; NV12 not accepted by x265)            [H-10], [H-13]
               I420 1920x1080 = 3,110,400 B per frame                     [H-43] (reasoning)
               1080p60: ≈ 249 MB/s read + ≈ 187 MB/s write                [H-43] (reasoning)
               (hardware offload: Pi 4/CM4 OQ-057, Pi 5/CM5 OQ-060)
                      ▼
               x265 4.1 (x265enc / libx265), software H.265, Arm CPU      [H-01], [H-09], [H-11]
               (cost on CM4 and CM5: UNKNOWN, OQ-104)
                      ▼
               recorder / RTMP (Enhanced RTMP; FFmpeg) / SRT / WebRTC   (VIDEO_ENCODER.md §4A.7)
```

On Pi 4/CM4 this flow can run in parallel with §9.1 or §9.2 for H.264 (candidate; §8A). On Pi 5/CM5 it can share one I420 conversion with §9.4 (candidate; §8, OQ-060). *(2026-10-08, later: this flow is kept as evidence for REQ-ENC-002 and applies only if it is re-activated; the current-scope flows are §9.1 to §9.4, H.264 only.)*

---

## 10. Buffer ownership models (candidates for ADR-007)

| Model | Who allocates | What must be true | Open points |
|---|---|---|---|
| **A — capture-allocated** | The capture driver: Unicam from CMA [C-53]; CFE as in §3.2 | The application can obtain a DMABUF fd for each capture buffer and queue it on the encoder | The export mechanism for Unicam and CFE is not in the source register (KERNEL SOURCE INSPECTION REQUIRED; OQ-058) |
| **B — application-allocated from a heap** | The application, from a CMA DMA-BUF heap [E-48] | The capture queue accepts `V4L2_MEMORY_DMABUF` import. The heap name is known (OQ-062). The uapi header is available (§5). | Unicam and CFE capture-queue I/O modes are not in the source register (KERNEL SOURCE INSPECTION REQUIRED) |
| **C — framework-managed** | GStreamer negotiates buffer sharing between elements | The M2M element imports DMABUFs (`dmabuf-import` [D-53]) and the upstream element supplies suitable ones | As Model A, plus GStreamer behaviour on the shipped version |

None of these models is decided. The model is to be chosen together with ADR-007 (OPEN). ADR-007 is to be decided after bring-up proves capture (TEST-CAP-002) and encode (TEST-ENC-001); TEST-DMA-001 supplies the buffer-sharing evidence.

---

## 11. Verification plan

The full procedures belong in [TESTING.md](TESTING.md). Both tests are **BLOCKED — HARDWARE REQUIRED**.

**TEST-DMA-001** (REQ-DMA-001, REQ-ARCH-001) must show, per candidate platform:

- capture buffers imported into the encoder with no `contiguous chunk is too small` error [D-20];
- capture and encoder strides that agree (§6);
- frames that arrive intact;
- that the chosen userspace path does not copy raw frames on Pi 4/CM4.

**TEST-PERF-001** must record `CmaFree` from `/proc/meminfo` during sustained capture and encode [C-53] (RISK-020, OQ-061). On Pi 5/CM5 it must also record CPU load of conversion and encode (OQ-059). *(Added 2026-10-08.)* With H.265 required, the CPU load of the UYVY → planar conversion and of x265 must be recorded on CM4 as well as CM5, the owner's bring-up pair (ADR-004; OQ-104). On CM4, also record the effect on capture buffers in flight when one buffer feeds both the hardware H.264 path and the H.265 conversion (§8A; OQ-058, OQ-061). *(Superseded 2026-10-08, later: H.265 is deferred — REQ-ENC-002; these H.265 measurements are deferred, not run in current scope. `CmaFree` on every platform under evaluation, and the conversion and encode CPU load on Pi 5/CM5 (OQ-059), are still recorded as above.)*

> **NOT YET RUN ON PACSCORDER HARDWARE.** No command in this document has been run on PACSCORDER hardware. Exact command lines will be added to [TESTING.md](TESTING.md) only where a cited source attests them.

---

## 12. Traceability

| ID | Relation to this document | Status |
|---|---|---|
| REQ-DMA-001 (DRAFT) | Whole document; Pi 5/CM5 reading in §8 needs owner confirmation (OQ-017) | NOT STARTED; TEST-DMA-001 BLOCKED — HARDWARE REQUIRED |
| REQ-ARCH-001 (DRAFT) | §9: the DMABUF stage of the mandated path | NOT STARTED |
| REQ-ENC-001 (DRAFT) | §6, §8: encoder input constraints; *(added 2026-10-08)* §8A: H.265 planar input on every platform. *(2026-10-08, later: REQ-ENC-001 is H.264 only; §8A now traces to REQ-ENC-002)* | NOT STARTED |
| REQ-ENC-002 (DEFERRED; added to this table 2026-10-08) | §8A, §9.5: H.265 buffer path — evidence only, not in current scope | NOT STARTED; no tests planned (deferred) |
| REQ-PERF-001 (PROPOSED) | §4.4, §8: CMA and CPU measurements | NOT STARTED; TEST-PERF-001 BLOCKED — HARDWARE REQUIRED |
| ADR-003 (ACCEPTED) | §5: kernel series and toolchain differ between Raspberry Pi OS and Buildroot | — |
| ADR-004 (OPEN) | §1, §9: flows differ per platform. *(2026-10-08: bring-up evaluates CM4 and CM5 side by side, owner 2026-10-07; still OPEN)* | — |
| ADR-005 (PROPOSED) | §6, §8: UYVY at the encoder input; *(added 2026-10-08)* §8A: UYVY is never accepted by x265 (H.265 deferred — REQ-ENC-002) | — |
| ADR-007 (OPEN) | §7, §10: framework and buffer-ownership model; *(added 2026-10-08)* §8, §8A: one planar conversion for both codecs, and one capture buffer for two consumers on CM4 (both H.265 cases deferred — REQ-ENC-002; not in current scope) | — |
| RISK-002 | §6: Pi 4/CM4 encoder at 1080p60 (see [VIDEO_ENCODER.md](VIDEO_ENCODER.md)) | OPEN |
| RISK-003 | §8 | OPEN |
| RISK-020 | §2, §4 | OPEN |
| RISK-022 (added 2026-10-08) | §8A: H.265 is software-only; the conversion cost comes before x265 [H-43]. *(2026-10-08, later: not in current scope — H.265 deferred, REQ-ENC-002)* | OPEN |
| RISK-024 (added 2026-10-08) | §6: capture timestamps are `CLOCK_MONOTONIC` [I-33] (A/V alignment) | OPEN |

**Open questions referenced:** OQ-002, OQ-005, OQ-015, OQ-017, OQ-048, OQ-053, OQ-055, OQ-056, OQ-057, OQ-058, OQ-059, OQ-060, OQ-061, OQ-062, OQ-064; added 2026-10-08: OQ-103, OQ-104, OQ-112. *(2026-10-08, later: OQ-103 ANSWERED — "H.264 only for now"; OQ-104 OPEN but not in current scope.)*

---

## Verification status

**Verified from sources (fact IDs)**

These are statements found in the cited sources, or arithmetic built on them. They are not observations on PACSCORDER hardware.

- **Buffers and allocators:** [C-53] (CORRECTED), [C-46], [A-08].
- **CMA and heaps:** [C-40], [E-47] (CORRECTED), [E-48], [G-19], [E-12], [G-04], [E-06], [D-03], [E-46].
- **Pi 4/CM4 encoder:** [D-06], [D-07], [D-08], [D-09], [D-16], [D-17] (community), [D-18] (community), [D-19], [D-20], [D-21], [D-22], [D-23], [D-25], [D-48].
- **Userspace paths:** [D-34], [D-38], [D-39], [D-44], [D-45], [D-46] (CORRECTED), [D-53], [G-26], [G-29], [C-41] (community).
- **Pi 5/CM5:** [G-22], [D-28], [D-29], [D-30], [D-31], [D-40], [D-41], [D-43], [D-50] (community), [D-51], [G-17], [G-20], [G-60], [E-51], [E-09], [C-31], [C-32].
- **Receivers and kernel baseline:** [C-07], [C-08], [D-01], [E-37].
- **H.265 (added 2026-10-08; deferred — REQ-ENC-002; kept as its evidence):** [D-24], [D-31], [H-01], [H-09], [H-10] (CORRECTED), [H-11], [H-13]; reasoning [H-43].
- **Timestamps and audio path (added 2026-10-08):** [I-33], [I-03], [I-04].
- **Claude's own reasoning in this document (not register facts):**
  - *(added 2026-10-08)* the 1080p50/1080p30 conversion rows and read + write totals in §8A, the single-format (I420) reading in §8, the two-consumer and REQ-DMA-001 readings in §8A, and the CMA note for conversion buffers;
  - the buffer-count table and line sizes in §2;
  - the stride table in §6;
  - the conversion traffic in §8;
  - the "zero-copy" readings in §8;
  - the ownership models in §10;
  - the CMA-heap contiguity and toolchain-header inferences in §5, and the Pi 5/CM5 CPU-read statements in §1.

**Verified on PACSCORDER hardware:** nothing (no hardware exists as of 2026-10-06). *(Re-checked 2026-10-08: still nothing.)*

Every hardware-dependent item is **BLOCKED — HARDWARE REQUIRED**.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review: inferences labelled as reasoning; UNKNOWN markers put in the standard form; page-size citation corrected to [E-51]/[G-60]; Raspberry Pi OS GStreamer and Buildroot FFmpeg wording aligned with [G-26]/[D-46]; uncited "No WebRTC" marked NEEDS VERIFICATION; CMA-percentage caveat; ADR-007 decision point aligned with DECISIONS.md. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: [D-50] statements attributed per engineer (jamesh: no H.264 block in BCM2712; 6by9 alone: 1080p60 software encode "easily achievable", worded as a single community report) in §8; FFmpeg "no WebRTC" sentence aligned with the corrected ADR-007 (both NEEDS VERIFICATION) in §7; DMA-heap toolchain-header item now uses the README marker BUILD TEST REQUIRED (§5); capture-buffer count linked to the CMA budget question OQ-061 (§2). No stale "still being verified" hedge or "no OQ" text found. No status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §4.2 reasoning ("ADR-003 PROPOSED" → "ADR-003 ACCEPTED") and §12 traceability row (ADR-003 (PROPOSED) → (ACCEPTED)). The product `config.txt` still does not exist; no evidence, other ADR status or implementation status changed. | Claude (session 2026-10-07) |
| 2026-10-08 | H.265 buffer path (research topic H) and capture timestamps (research topic I), plus the owner decisions of 2026-10-07 (second set). Changes: <br>• Header: "Applies to" and "Verification" updated. <br>• Scope: H.265 bullet added; note that HDMI audio (required, REQ-CAP-006) travels over I2S to ALSA, not through these buffers [I-03], [I-04]. <br>• §1: three H.265 rows (no hardware HEVC encoder; planar-only input; no zero-copy for H.265, [H-43] traffic). §6: capture timestamps are `CLOCK_MONOTONIC` at frame start [I-33]. §7: software H.265 path row. <br>• §8: `libx265` and `x265enc` input rows; I420 is the one format all five encoders accept; "converter writes not in register" marked superseded in part ([H-43]). <br>• New §8A: H.265 on every platform — planar input; the UYVY → I420 conversion traffic table (1080p60 from [H-43]; 1080p50 and 1080p30 by the same reasoning); I420 buffer size; hardware conversion candidates (OQ-057, OQ-060); the CM4 two-consumer capture buffer (OQ-058, OQ-061); the REQ-DMA-001 reading for H.265 (OQ-017). <br>• New §9.5 H.265 candidate flow. §11: TEST-PERF-001 note for CM4 and CM5. §12: REQ/ADR rows annotated (ADR-004 still OPEN, CM4 + CM5 side by side); RISK-022 and RISK-024 rows; OQ-103, OQ-104 and OQ-112 added. Verification lists updated. <br>New citations: D-24, H-01, H-09, H-10, H-11, H-13, H-43, I-03, I-04, I-33. No REQ or ADR status changed; nothing run or measured. | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic H and I additions: §9.5 diagram — the two figures taken from the reasoning-tier entry [H-43] are now marked "(reasoning)". The §8A traffic table arithmetic (1080p60/50/30) was re-checked against the [H-43] inputs; all other [H-xx] and [I-xx] citations checked; no other change. No status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header "Applies to" (H.264 only; H.265 = REQ-ENC-002, not in current scope); scope bullet, §1 H.265 rows, §7 software-H.265 path row and §8 `libx265`/`x265enc` rows labelled deferred; §8 note that in the current scope the Pi 5/CM5 conversion feeds software H.264 only, so NV12 or I420 suit `libx264`/`x264enc` and `openh264enc` needs I420 [D-40], [D-41], [D-43]; §8A heading labelled "(deferred — REQ-ENC-002; not in current scope)" with a scope note (x265 conversion and CM4 two-consumer case do not arise now; RISK-022, OQ-104 OPEN, not in current scope); §9.5 heading labelled and note (current-scope flows are §9.1–§9.4, H.264 only); §11 TEST-PERF-001 H.265 measurements superseded (deferred, not run in current scope; `CmaFree` and Pi 5/CM5 CPU load still recorded); §12 REQ-ENC-001 row annotated, REQ-ENC-002 (DEFERRED) row added, ADR-005, ADR-007 and RISK-022 rows annotated (statuses unchanged), OQ-103 ANSWERED / OQ-104 not in current scope noted; Verification H.265 list labelled. H.265 research kept as REQ-ENC-002 evidence. No other decision or status changed; no ID added; no fact ID new to this document. | Claude (session 2026-10-08) |
