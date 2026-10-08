# PACSCORDER Recording

| | |
|---|---|
| Document status | Active — source research only. Recording design and implementation: NOT STARTED |
| Last updated | 2026-10-08 |
| Applies to | REQ-REC-001; REQ-ENC-001 (H.264 and H.265 required, owner 2026-10-07; which outputs use H.265 is OQ-103); REQ-CAP-006 (HDMI audio required in recordings, owner 2026-10-07; DRAFT); ADR-004 (`OPEN`; bring-up evaluates CM4 and CM5 side by side); RISK-022, RISK-023, RISK-024; all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5); the project's own product OS image (REQ-BLD-002, DRAFT), built per ADR-003 (ACCEPTED: Raspberry Pi OS with `rpi-image-gen`; Buildroot as the documented alternative) |
| Verification | Source research of 2026-10-06, plus research topics H (H.265/HEVC) and I (HDMI audio) of 2026-10-08 ([REFERENCES.md](REFERENCES.md)). Nothing has been tested. No PACSCORDER hardware or code exists as of 2026-10-08. |

This document answers the Rule 25 question "How is recording performed?". As of 2026-10-06 the honest answer is: **recording is not designed yet.** REQ-REC-001 is `DRAFT`, and the parameters that would drive a design (container, storage medium, duration, power-loss behaviour) are undefined.

What this document does record:

- what is undefined and who must decide it;
- what the sources say about the building blocks (muxers, encoder behaviour, storage);
- which design questions are open.

Fact IDs such as `[G-26]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Facts of tier `community` are worded as reports. Calculations are marked as reasoning and list their inputs.

> **Owner decisions of 2026-10-07 that affect recording** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md); added here 2026-10-08):
>
> - **Codecs.** H.264 **and** H.265 (HEVC) for recording and streaming (REQ-ENC-001). Whether recordings are H.264, H.265 or both, at which resolutions and frame rates, is open: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). HEVC in the candidate containers is in Section 3; HEVC encoder facts are in Section 4.
> - **Audio.** HDMI audio is required in recordings (REQ-CAP-006, DRAFT; OQ-004 ANSWERED). Channels, sample rates and the A/V tolerance are still open (OQ-110, OQ-111, OQ-112). See Section 7.
> - **Platforms.** Bring-up evaluates CM4 and CM5 side by side. The product platform (ADR-004) stays `OPEN` until measured.
> - **Sources.** Any HDMI camera (no model list) plus ATEM switcher outputs (OQ-102 ANSWERED; REQ-CAP-008).
>
> Container, storage medium, duration and power-loss behaviour are still undefined (OQ-006).

## 1. Status at a glance

| Item | Status |
|---|---|
| REQ-REC-001 Recording | Acceptance `DRAFT`. Implementation `NOT STARTED`. |
| TEST-REC-001 Recording integrity and duration | `BLOCKED — HARDWARE REQUIRED` |
| Container format | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). |
| Storage medium | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| Minimum continuous recording duration | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). |
| Behaviour on power loss | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). Not researched; no source fact exists. |
| Audio in recordings | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-004). *(Superseded 2026-10-08: audio is required — owner answer of 2026-10-07, "Yes, audio required"; REQ-CAP-006 DRAFT; OQ-004 ANSWERED. Still open: channels and formats (OQ-110), sample-rate policy (OQ-111), A/V tolerance (OQ-112), encoder choice and cost (OQ-063). See Section 7.)* |
| Codec, bitrate, shared or separate encode | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-005). *(Superseded in part 2026-10-08: the codecs are decided — H.264 and H.265, owner 2026-10-07, REQ-ENC-001. Which codec recordings use: OQ-103. Bitrate and shared or separate encode: still OQ-005.)* |
| HEVC in the container (added 2026-10-08) | From sources: MP4 and Matroska muxers in GStreamer 1.26.2 and FFmpeg 7.1.5 accept HEVC [H-37], [H-38] (Section 3). Not tested. |
| Software H.265 encode capacity on CM4 and CM5 (added 2026-10-08) | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104, OQ-105). RISK-022. |
| Recorded audio sample rate (added 2026-10-08) | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED; BUILD TEST REQUIRED (OQ-111). RISK-023. |
| A/V synchronisation in recordings (added 2026-10-08) | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED (OQ-112). RISK-024. |
| HEVC and AAC patent licensing (added 2026-10-08) | UNKNOWN — VERIFICATION REQUIRED. LEGAL CLARIFICATION REQUIRED (OQ-109, OQ-113). RISK-015. |
| Userspace framework (GStreamer, FFmpeg, direct V4L2) | ADR-007 `OPEN` (OQ-015). |
| Partition layout and update scheme | OQ-069; depends on ADR-003 (`ACCEPTED`). |
| Software updates or reboots while a recording is active | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-094). |

## 2. Where recording sits in the pipeline

```text
HDMI → TC358743 → CSI-2 → CSI-2 receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder (this document)
                                                                                     ├→ RTMP    (STREAMING.md)
                                                                                     └→ WebRTC  (STREAMING.md)
```

The recorder receives the encoder's output. The encoder differs per platform, which affects what can be recorded:

| Platform | Encoder (facts) | Consequence for recording (reasoning) |
|---|---|---|
| Pi 4 Model B, CM4 | Hardware H.264 encoder, officially specified for 1080p30 encode [D-10] | 1080p60 recording depends on unproven 1080p60 hardware encode (RISK-002, OQ-056). |
| Pi 5, CM5 | No hardware video encoder [D-31]; software encoders; official figure "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22] | If recording has its own encode, it competes for CPU with the RTMP and WebRTC encodes (RISK-003, OQ-005, OQ-059). |

No candidate platform has a hardware HEVC encoder [D-24], [D-31].

**H.265 recordings (added 2026-10-08; H.265 is required by REQ-ENC-001).** Reasoning from [D-24] and [D-31]: an H.265 recording is a software encode on every candidate, CM4 included. CM4's hardware H.264 encoder [D-10] does not help it. Research topic H covered CM4 and CM5; the community benchmarks below name Pi 5 and a Pi 400.

- **Encoders.**
  - The encoder is Debian's x265 4.1-2, which Raspberry Pi does not override [H-01], [H-02].
  - FFmpeg uses it through `libx265` [H-08], [H-09]; GStreamer through `x265enc` [H-11], [H-12] (CORRECTED).
  - Both accept planar input only, not the TC358743's packed UYVY [H-10] (CORRECTED), [H-13]. Reasoning: the conversion at 1080p60 reads about 249 MB/s and writes about 187 MB/s on the CPU [H-43].
- **CM4 versus CM5.** x265's Neon DotProd kernels can apply only on CM5's Cortex-A76 [H-04], [H-05].
- **Cost evidence.**
  - A Raspberry Pi engineer stated on the official forum that software H.265 encode "is too intensive an operation to perform at any significant resolution". This is a community source; no raspberrypi.com figure was found [H-19].
  - Community benchmarks report `libx265` "Live" results of 10.00 FPS on Pi 5 and 4.33 FPS on a Pi 400 (Cortex-A72). The test is not a 1080p60 live measurement (community sources) [H-20], [H-21], [H-22].
  - Reasoning: in that harness `libx265` was about 6.6 times slower than `libx264` [H-23].
- **Throughput** on CM4 and CM5: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104; TEST-ENC-001). RISK-022.

Whether the recorder shares one encoded stream with RTMP and WebRTC, or has its own encode, is part of OQ-005. Reasoning: browser WebRTC interoperability requires H.264 Constrained Baseline [F-36], and the MediaMTX project reports that browsers do not accept H.264 B-frames [F-45]. A single shared encode would therefore carry those restrictions into the recording as well. See [STREAMING.md](STREAMING.md) and [VIDEO_ENCODER.md](VIDEO_ENCODER.md).

*(Added 2026-10-08.)* Reasoning from [H-33], [H-34], [H-35] and the community report [H-36], as in RISK-019: H.265 in WebRTC reaches only some browsers, so an H.264 WebRTC track has to remain for browser reach. If recordings are H.265 (OQ-103), the recording therefore cannot share the WebRTC encode. On CM5 that is a second concurrent software video encode (OQ-104, OQ-108).

## 3. Container muxers available in Raspberry Pi OS and the Buildroot alternative

| Container | Raspberry Pi OS (trixie) | Buildroot 2026.08 | Notes |
|---|---|---|---|
| MP4 (ISO BMFF) | `libgstisomp4` ships in `gstreamer1.0-plugins-good` 1.26.2 [G-26] | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_ISOMP4` (MP4 muxing) [E-34] | ATEM switchers record H.264 + AAC in MP4 [F-07], [F-26]. *HEVC (added 2026-10-08):* GStreamer 1.26.2 `qtmux` and `mp4mux` accept `video/x-h265` with stream-format `hvc1` or `hev1`, `alignment=au`, and write an `hvc1` or `hev1` sample entry with an `hvcC` box [H-37]. FFmpeg 7.1.5's MP4 muxer tags HEVC as `hev1`, `hvc1` or `dvh1`; `hev1` means parameter sets may be in the elementary stream, `hvc1` means they shall not be [H-38]. |
| Matroska | `libgstmatroska` ships in `gstreamer1.0-plugins-good` [G-26] | NEEDS VERIFICATION — no register fact | *HEVC (added 2026-10-08):* GStreamer 1.26.2 `matroskamux` accepts `video/x-h265` `hvc1` or `hev1`, but warns that `hev1` "is not officially supported, only use this format for smart encoding" [H-37]. FFmpeg's Matroska muxer handles HEVC, writing `hvcC` CodecPrivate [H-38]. |
| FLV | `libgstflv` ships in `gstreamer1.0-plugins-good` [G-26] | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV` [E-34], [F-34] | FLV is the RTMP container. In legacy FLV, H.264 (AVC) is the only modern video codec, and AAC is audio SoundFormat 10 [F-31]. `flvmux` caps are in [F-34]. *HEVC (added 2026-10-08):* GStreamer 1.26.2 `flvmux` has no H.265 [H-27]; FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV [H-26]. |
| MPEG-TS | NEEDS VERIFICATION — no register fact *(Superseded in part 2026-10-08: GStreamer 1.26.2 `mpegtsmux` accepts `video/x-h265,stream-format=byte-stream`, and Debian trixie's `gstreamer1.0-plugins-bad` ships `libgstmpegtsmux.so` [H-30]. Whether the Raspberry Pi build of plugins-bad contains it, and the FFmpeg MPEG-TS muxer: NEEDS VERIFICATION.)* | NEEDS VERIFICATION — no register fact | Named as a candidate in OQ-006 only. |
| FFmpeg muxers | FFmpeg 7.1.5 (Raspberry Pi build `+rpt2`) is in the archive [G-29]. Its muxer list is not in the register: NEEDS VERIFICATION. *(Superseded in part 2026-10-08: its MP4 and Matroska muxers handle HEVC [H-38], and its FLV muxer handles HEVC + AAC [H-26]. The complete muxer list is still NEEDS VERIFICATION.)* | FFmpeg 6.1.5 [E-36], [G-64]. Muxers: NEEDS VERIFICATION. | — |

Further constraints:

- **GStreamer must be installed.** The Raspberry Pi OS Lite image of 2026-10-06 has no GStreamer packages installed [G-32].
- **Versions differ between the OS options.** Raspberry Pi OS trixie ships GStreamer 1.26.2 [G-26], [G-31]. Buildroot 2026.08 ships 1.24.13 [E-31], [G-64]. Differences are tracked in OQ-066.
- **Element names and properties are not sourced.** The register names the isomp4 and matroska *plugins* only. Their element names, and any properties for fragmenting, segmenting or finalising files, are NEEDS VERIFICATION before a design is written. *(Superseded in part 2026-10-08: the element names `qtmux`, `mp4mux`, `matroskamux` and `h265parse` are now in the register [H-37]. Properties for fragmenting, segmenting or finalising files, and the audio caps of these muxers, are still NEEDS VERIFICATION.)*
- **A parser is needed for HEVC** (added 2026-10-08). `x265enc` outputs `video/x-h265,stream-format=byte-stream,alignment=au` [H-13], so `h265parse` is needed before `qtmux`, `mp4mux` or `matroskamux` [H-37].

## 4. Encoder facts that affect recording

| Fact | Effect on recording (reasoning unless cited) |
|---|---|
| The Pi 4/CM4 hardware encoder produces no B-frames. GOP size defaults to 60. Every I-frame is an IDR frame [D-14]. | Random-access points in the recording are the IDR frames. Reasoning with inputs GOP = 60 [D-14] and frame rates 30 or 60: one IDR every 2 s at 30 fps, every 1 s at 60 fps. The GOP length bounds how finely a recording can be cut or split into segments without re-encoding. GOP size is a control (`V4L2_CID_MPEG_VIDEO_GOP_SIZE`) [D-14]. |
| On the Pi 4/CM4 encoder, `V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER` (SPS/PPS inline with every IDR) defaults to off [D-15]. Raspberry Pi's official GStreamer streaming pipeline sets `repeat_sequence_header=1` [D-37]. | If a recording is split into segments that must each decode on their own, each segment needs SPS/PPS. Whether the chosen muxer stores the parameter sets itself is NEEDS VERIFICATION. |
| The Pi 4/CM4 encoder queues copy input timestamps to encoded buffers (`V4L2_BUF_FLAG_TIMESTAMP_COPY`) [D-19]. | Capture timestamps can flow through the hardware encoder into the container. |
| The TC358743 driver derives the reported timings from an integer frame rate, so fractional rates such as 59.94 Hz are reported as integer-fps pixel clocks [B-28]. The ATEM Mini Pro outputs both 1080p59.94 and 1080p60 [F-23]. | Reasoning: container frame-rate metadata and timestamps cannot rely on the reported DV timings alone, because 59.94 and 60 Hz sources are not distinguished there. How the true rate is found is OQ-040. |
| UYVY output is BT.601 limited range, reported as `SMPTE170M` [B-34]. ATEM output is Rec 709 [F-23]. | The colour metadata written into the file is undecided (OQ-041). |
| Pi 4/CM4 encoder bitrate range is 25 kbit/s to 25 Mbit/s, default 10 Mbit/s, VBR (default) or CBR [D-13]. | Input to storage sizing (Section 5.3). |
| *(Added 2026-10-08; H.265 software encoder, every platform.)* `x265enc` exposes `speed-preset`, `tune`, `bitrate` (kbit/s, default 2048) and `key-int-max` (default 0 = x265 default) [H-14]. | Bitrate and keyframe interval are settable, which bounds storage (Section 5.3) and segment boundaries. The x265 default keyframe interval is not in the register: NEEDS VERIFICATION. |
| *(Added 2026-10-08.)* `tune=zerolatency` sets B-frames 0, lookahead 0 and one frame thread [H-16]. x265 4.1's `ultrafast` preset uses 3 B-frames and lookahead 5 [H-17]. | Reasoning: a recording-only H.265 encode is not bound by live latency, so it could keep B-frames; a recording that shares a live encode inherits zerolatency's settings (OQ-005, OQ-103). |
| *(Added 2026-10-08.)* In MP4, `hvc1` means parameter sets shall not be in the elementary stream; `hev1` means they may be [H-38]. `matroskamux` warns that `hev1` is not officially supported [H-37]. | Reasoning: the `hvc1`/`hev1` choice decides where the HEVC parameter sets live, which matters for segments that must decode on their own (Section 6, approach 1). Which form `h265parse` produces for each muxer: NEEDS VERIFICATION. |

**Pi 5 / CM5.** The encoder rows above ([D-13], [D-14], [D-15], [D-19]) describe the Pi 4/CM4 hardware encoder only. For the Pi 5/CM5 software encoders, the register states only that `rpicam-apps` uses `max_b_frames=1` for `libx264` in normal mode [D-35]. Their GOP length, IDR behaviour, SPS/PPS repetition and timestamp handling are NEEDS VERIFICATION. See [VIDEO_ENCODER.md](VIDEO_ENCODER.md). *(2026-10-08: the H.265 rows above apply to x265 on all platforms, including CM4; x265's GOP length, IDR behaviour and timestamp handling on PACSCORDER are still NEEDS VERIFICATION.)*

## 5. Storage

### 5.1 Storage per platform (what the source register says)

| Platform | Storage facts in the register | PACSCORDER recording storage |
|---|---|---|
| Pi 4 Model B | For A/B boot updates, Raspberry Pi Connect Remote Update needs a Raspberry Pi 4 or later with a storage device (typically microSD) of at least 16 GB [G-43] (CORRECTED). This is an update-mechanism requirement, not a recording requirement. Storage and provisioning differ between boards (SD vs eMMC with `rpiboot`) [G-71] (reasoning, CORRECTED). | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| CM4 | Compute Module eMMC is flashed by fitting nRPI_BOOT (J2) and running `rpiboot` [G-47]. | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| Pi 5 | Same Connect statement as Pi 4 [G-43]. | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| CM5 | eMMC flashing as for CM4 [G-47]. The Raspberry Pi OS image includes CM5 Lite device trees [G-71]. | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |

**Not in the source register** (each is UNKNOWN — VERIFICATION REQUIRED):

- USB mass storage and NVMe/PCIe as recording media. DATASHEET REQUIRED (OQ-098).
- eMMC capacities of the Compute Module variants, and the storage of Lite (no-eMMC) variants. DATASHEET REQUIRED (OQ-098).
- Sustained write throughput of any medium. HARDWARE TEST REQUIRED, as part of TEST-REC-001 and TEST-PERF-001.

### 5.2 Filesystem and partition constraints

- **Read-only root.** On Raspberry Pi OS, raspi-config's overlay file system option prepends `overlayroot=tmpfs` to `cmdline.txt` [G-48]. Reasoning: if that overlay is enabled, recordings written under it would be lost at reboot and would consume RAM, so recordings would need a separate persistent partition.
- **rpi-image-gen A/B layout.** The `image-rota` layout provides immutable A/B system slots and a single shared persistent data partition [G-40], [G-41]. This is a candidate location for recordings, subject to OQ-069. Whether an update or a reboot into a new slot may happen while a recording is active, and what happens to that recording, is OQ-094 (OWNER DECISION REQUIRED).
- **Buildroot Raspberry Pi sample images.** Buildroot 2026.08 `board/raspberrypi/genimage.cfg.in` defines a 32 MB boot partition, and the defconfigs set a 120 MB ext4 root filesystem [E-19]. Reasoning: there is no recording space in that layout, so a project partition layout would be needed.
- **OS footprint.** The Raspberry Pi OS Lite image of 2026-10-06 expands to 3,078,619,136 bytes [G-33]. Reasoning: on a stock-image installation, that much of the medium is used before any recording space is allocated. The stock image is for bring-up only; the product runs its own project-built OS image (REQ-BLD-002, owner 2026-10-07). An `rpi-image-gen` image may differ (OQ-068).
- **Recording filesystem.** UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). For reference only: ATEM Mini Pro records to ExFAT or HFS+ media [F-26].

### 5.3 Storage sizing (reasoning)

Formula: bytes per hour = video bitrate (bit/s) × 3600 / 8. GB here means 10^9 bytes. Audio and container overhead are excluded. In VBR mode the actual bitrate varies around the target, so measured sizes will differ (reasoning).

| Video bitrate (input) | Bytes per hour, video only |
|---|---|
| 10 Mbit/s — Pi 4/CM4 encoder default [D-13] | 4.5 GB |
| 25 Mbit/s — Pi 4/CM4 encoder maximum [D-13] | 11.25 GB |
| 12 Mbit/s — YouTube Live's recommended 1080p60 H.265 *streaming* bitrate [H-29] (added 2026-10-08; a streaming figure, used here only as an example) | 5.4 GB |
| 17 Mbit/s — YouTube Live's recommended 1080p60 H.264 *streaming* bitrate [H-29] (added 2026-10-08; same caveat) | 7.65 GB |

The product bitrate is undecided (OQ-005), so these are examples, not requirements.

*Audio (added 2026-10-08; reasoning).* Audio is required (REQ-CAP-006) but is excluded from the table. Input: FFmpeg's native `aac` defaults to 128 kb/s for stereo when no bitrate is given [I-40] (CORRECTED). At that rate, audio adds 128,000 × 3600 / 8 = 57.6 MB per hour. The product audio bitrate is undecided (OQ-063).

## 6. Power-loss behaviour — OPEN

REQ-REC-001 leaves power-loss behaviour undefined. **It was not researched:** no fact in [REFERENCES.md](REFERENCES.md) describes how any container, muxer or filesystem behaves when power is lost during recording.

**Owner decisions required (OQ-006):**

- How much recorded material may be lost when power is cut?
- Must the file being written remain playable?
- Is a hardware hold-up (time to close files after input power fails) acceptable? This depends on the product power design, which is UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-023).

**Candidate approaches to evaluate.** These are Claude's reasoning, not sourced facts and not decisions. Each one is NEEDS VERIFICATION through TEST-REC-001 with deliberate power cuts.

1. **Segmented recording.** Write the recording as a sequence of shorter files that are closed regularly, so that a power cut affects at most the file that is open. Segment boundaries would fall on IDR frames (Section 4, [D-14]).
2. **Progressive index.** Use a container or muxer mode that writes index data during recording rather than only at the end. Which of the available muxers ([G-26], [E-34]) offer such a mode is NEEDS VERIFICATION.
3. **Power-fail detection.** Detect input power loss in hardware and close files during a hold-up interval. This requires hardware support that is UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-018, OQ-023).
4. **Filesystem choice and mount options** for the recording partition. Behaviour under power loss is NEEDS VERIFICATION.

After OQ-006 is answered, the chosen design is recorded in a new ADR in [DECISIONS.md](DECISIONS.md). No such ADR exists yet.

## 7. Audio in recordings

Whether recordings include audio is undecided (OQ-004). If audio is required:

> **Superseded 2026-10-08.** Audio is required: the owner answered OQ-004 on 2026-10-07 ("Yes, audio required"), and REQ-CAP-006 moved to DRAFT. The bullets below therefore apply. Subsections 7.1 to 7.4 add the research of 2026-10-08 (topic I).

- The TC358743 datasheet allows audio to be sent over CSI-2 [A-05] or on the multiplexed I2S/TDM pins [A-11]. The Linux driver always configures 2-channel I2S output [A-13], so with that driver the audio leaves on I2S, not on CSI-2.
- The `tc358743-audio` overlay routes that I2S to Pi GPIO 18/19/20 [A-47]. Wiring on PACSCORDER hardware is UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED (OQ-025). Pi 5/CM5 support is UNKNOWN — VERIFICATION REQUIRED. KERNEL SOURCE INSPECTION REQUIRED (OQ-054). See RISK-014. *(2026-10-08: the CM5 labels now resolve in kernel source, but operation is still unconfirmed; see Section 7.1.)*
- The audio encoder choice and its CPU cost are open (OQ-063). For reference, ATEM recordings use AAC in MP4 [F-07]. *(2026-10-08: the available AAC encoders are now sourced; see Section 7.3. Choice and CPU cost are still OQ-063.)*

### 7.1 Capture path per platform (added 2026-10-08)

Research topic I covered CM4 and CM5, the two boards evaluated side by side in bring-up (ADR-004). Pi 4 Model B and Pi 5 were not covered: NEEDS VERIFICATION (OQ-054 for Pi 5).

| Item | CM4 | CM5 |
|---|---|---|
| Overlay | `tc358743-audio` enables `i2s_clk_consumer`, adds a `linux,spdif-dir` stub codec as bit-clock and frame master, and creates the ALSA card `tc358743` [I-01], [I-02], [I-03], [I-15] | Same overlay. `overlay_map` has no entry for it, so the firmware does not block it [I-05], [I-06]. |
| CPU I2S | `bcm2835-i2s` on GPIO 18–21 [I-10]; captures exactly 2 channels at 8–384 kHz, S16_LE, S24_LE or S32_LE [I-13] | RP1 I2S1 on GPIO 18–21, the clock-consumer instance that a codec-master link needs [I-07], [I-08], [I-09]. Its capture channel count and formats come from hardware registers not visible in source [I-14]. |
| Status | Path documented in kernel source [I-10], [I-13]. Implementation `NOT STARTED`; TEST-AUD-001 `BLOCKED — HARDWARE REQUIRED`. | No source shows audio captured through this path: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-054). Implementation `NOT STARTED`; TEST-AUD-001 `BLOCKED — HARDWARE REQUIRED`. |

- **Device selection.** Reasoning from source: the PCM name differs between CM4 and CM5, so software should select the device by card id `tc358743`, not by PCM name [I-15], [I-17].
- **Channels.** The driver hard-codes 2-channel I2S at probe [I-24]. Reasoning: with the stock driver, recordings carry stereo audio only. What the TC358743 outputs for compressed or multichannel HDMI audio is OQ-110.
- **Pins.**
  - The four TC358743 audio pins are outputs powered from VDDIO2, rated 1.8–3.3 V [I-27].
  - The CM4 and CM5 IO Boards have a selectable 1.8 V or 3.3 V GPIO voltage, which VDDIO2 should match, or the lines need level shifting [I-29].
  - The overlay also claims GPIO 21, which the audio path does not use [I-30]. Other overlays default to GPIO 18 [I-31] (OQ-114).
  - Details: [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md).

### 7.2 Sample rate and A/V synchronisation (added 2026-10-08)

- **The recorded rate must follow the source.** Reasoning from source [I-18]:
  - No kernel path carries the HDMI sample rate into ALSA.
  - If the card is opened at 48000 Hz while the source sends 44100 Hz, the frames are labelled 48 kHz. They play 8.84 % fast and the audio timeline is 8.1 % short, so A/V drift accumulates, with no ALSA error.
  - Reasoning: a recording made that way has wrong-speed, wrong-pitch audio for its whole length.
  - RISK-023, OQ-111.
- **The driver exposes the rate.**
  - "Audio sampling rate" (ID 0x00981980, read-only) and "Audio present" (ID 0x00981981) [I-19], [I-20].
  - It sends `V4L2_EVENT_CTRL` change events [I-21].
  - Without a wired interrupt, a change takes up to about 1 s, plus I2C time, to reach the control [I-22].
  - On CM4 with legacy Unicam the controls are on `/dev/videoN`; on CM5 they are only on the TC358743 sub-device node [I-23].
- **Clock domains.**
  - The TC358743's audio PLL tracks the source's audio clock [I-26].
  - The CSI receiver drivers stamp video buffers with `CLOCK_MONOTONIC` [I-33].
  - GStreamer 1.26.2 and FFmpeg 7.1 align the two differently by default [I-35], [I-36], [I-37], [I-38].
  - Reasoning: offset at start and drift over long recordings are possible on CM4 and CM5.
  - The A/V tolerance is not set: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (OQ-112, RISK-024).

### 7.3 AAC encoders for recordings (added 2026-10-08)

ATEM switchers record AAC in MP4 [F-07], [F-26]. AAC encoders available in Raspberry Pi OS:

| Encoder | Package | Input | Notes |
|---|---|---|---|
| FFmpeg native `aac` | Raspberry Pi FFmpeg 7.1.5 [I-39] | FLTP at the MPEG-4 audio rates [I-40] | FFmpeg's default AAC encoder, not flagged experimental. Without `-b` it defaults to 128 kb/s for stereo; an explicit `-b` selects CBR [I-40] (CORRECTED). |
| GStreamer `avenc_aac` | Debian's `gstreamer1.0-libav`, which wraps FFmpeg's native encoder and is not rebuilt by Raspberry Pi [I-46] | F32LE, 7.35–96 kHz including 44.1 and 48 kHz, 1–16 channels [I-47] | — |
| GStreamer `voaacenc` | Raspberry Pi `gstreamer1.0-plugins-bad` [I-44] | S16LE, 8–96 kHz, 1 or 2 channels; rank secondary [I-47] | — |
| `fdk-aac` / `fdkaacenc` | Not shipped in the Raspberry Pi FFmpeg or GStreamer builds [I-39], [I-44] | — | Debian non-free, under a licence Debian calls incompatible with every GPL version; grants no patent licence [I-43]. A GPL FFmpeg build can enable it only with `--enable-nonfree`, which makes the result unredistributable [I-42]. |

- **Still open.**
  - Encoder choice and CPU cost alongside the video encode on CM4 and CM5: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063).
  - AAC patent licensing: LEGAL CLARIFICATION REQUIRED (OQ-113; RISK-015).
  - Other audio codecs in recordings (for example Opus or PCM) were not researched: NEEDS VERIFICATION.
- **Reasoning:** if a recording shares the RTMP encode chain, its audio is the same AAC stream. A WebRTC output needs Opus [F-41], which is a separate audio encode ([STREAMING.md](STREAMING.md) §2.1).

### 7.4 Audio tests (added 2026-10-08)

TEST-AUD-001 (HDMI audio capture over I2S, REQ-CAP-006) is `BLOCKED — HARDWARE REQUIRED`. For recordings, it and TEST-REC-001 would need to cover:

- audio presence in the file;
- correct speed with sources at 44.1 kHz and 48 kHz, and a rate change during recording (OQ-111);
- A/V offset (OQ-112);
- runs on both CM4 and CM5 (ADR-004; OQ-054).

TEST-PERF-001 covers multi-hour drift (RISK-024). This list is reasoning from the registers, not an accepted test specification.

## 8. Reference: how an ATEM records

ATEM switchers record on their own. This is listed as a reference for PACSCORDER's recorder design. Controlling or reading ATEM recording over the network is not in current scope: OQ-009 was answered on 2026-10-07 with HDMI capture of the ATEM output only (REQ-ATEM-001, REQ-CAP-008).

- The ATEM SDK Recording API records H.264 video and AAC audio as MP4 to an externally connected disk [F-07].
  - Its states are Idle, Recording and Stopping.
  - Its errors include `NoMedia`, `MediaFull` and `DroppingFrames` [F-07].
- ATEM Mini Pro records to USB-C media as `.mp4` H.264 with AAC audio, on ExFAT or HFS+ [F-26].
- The atem-connection project reports recording commands `RcTM` (set) and `RTMS` (status) over the network protocol [F-16] (reference only; network control is not in current scope). See [ATEM.md](ATEM.md).

Reasoning: the ATEM's error states (no media, media full, dropping frames) are a useful checklist for the status that PACSCORDER's own recorder must report. That is a design input, not a requirement.

## 9. Open design decisions

| Decision | Options | Status | Resolved by |
|---|---|---|---|
| Container | MP4, Matroska, MPEG-TS, other. *(2026-10-08: MP4 and Matroska both accept HEVC in GStreamer 1.26.2 and FFmpeg 7.1.5 [H-37], [H-38]; MPEG-TS accepts H.265 in GStreamer 1.26.2 `mpegtsmux` [H-30].)* | `OPEN` | OQ-006 (OWNER DECISION REQUIRED) |
| Recording video codec (added 2026-10-08) | H.264, H.265 or both (both are required for "recording and streaming", REQ-ENC-001) | `OPEN` | OQ-103 (OWNER DECISION REQUIRED), then OQ-104 (H.265 capacity on CM4 and CM5) |
| Audio encoder for recordings (added 2026-10-08) | FFmpeg native `aac`, `avenc_aac`, `voaacenc` (Section 7.3) | `OPEN` | OQ-063 (HARDWARE TEST REQUIRED), OQ-113 (LEGAL CLARIFICATION REQUIRED) |
| Audio sample-rate policy (added 2026-10-08) | Read the driver's rate control and reopen ALSA on change; force one rate through the EDID; or both (options from OQ-111) | `OPEN` | OQ-111 |
| A/V clock model (added 2026-10-08) | Monotonic system clock with driver timestamps, or the audio clock as pipeline clock (options from OQ-112) | `OPEN` | OQ-112, ADR-007 |
| Storage medium and interface | SD, eMMC, USB, NVMe, network | `OPEN` | OQ-006, OQ-018; board storage facts OQ-098 |
| Shared or separate encode for recording | One encode for all outputs, or a separate recording encode | `OPEN` | OQ-005, OQ-059 (Pi 5/CM5 CPU budget) |
| Userspace framework | GStreamer, FFmpeg, direct V4L2 application | `OPEN` | ADR-007, OQ-015 |
| Power-loss strategy | Section 6 candidates | `OPEN` | OQ-006, then TEST-REC-001 |
| Recordings on a persistent partition, separate from the root filesystem | — | `PROPOSED` as a recommendation only (Claude's reasoning from [G-48], [G-40]; not accepted; no recording ADR exists yet, so it is not a decision record). Depends on ADR-003 (`ACCEPTED`). | OQ-006 (storage and recording requirements), OQ-069 (partition and update layout), OQ-094 (updates versus active recordings), then owner acceptance |

## 10. Test

TEST-REC-001 "Recording integrity and duration" (canonical ID, [README.md](README.md)) is `BLOCKED — HARDWARE REQUIRED`. Its acceptance criteria cannot be written until OQ-006 is answered. Once they exist, the test must at least cover:

- the minimum duration;
- file integrity and playability;
- power-cut behaviour;
- sustained write throughput.

*(Added 2026-10-08; reasoning from the owner decisions of 2026-10-07, not accepted criteria.)* The test must also cover:

- recorded audio and A/V synchronisation (Section 7.4; TEST-AUD-001);
- H.265 recordings, if OQ-103 assigns H.265 to recording;
- runs on both CM4 and CM5, which bring-up evaluates side by side (ADR-004).

The procedure will be written in [TESTING.md](TESTING.md). No recording command is given here, because no recording command has been run on PACSCORDER hardware.

## Verification status

### Verified from sources (fact IDs)

This document cites 107 register entries, all with verdict `CONFIRMED` or `CORRECTED`:

A-05, A-11, A-13, A-47, B-28, B-34, D-10, D-13, D-14, D-15, D-19, D-24, D-31, D-35, D-37, E-19, E-31, E-34, E-36, F-07, F-16, F-23, F-26, F-31, F-34, F-36, F-41, F-45, G-22, G-26, G-29, G-31, G-32, G-33, G-40, G-41, G-43, G-47, G-48, G-64, G-71, H-01, H-02, H-04, H-05, H-08, H-09, H-10, H-11, H-12, H-13, H-14, H-16, H-17, H-19, H-20, H-21, H-22, H-23, H-26, H-27, H-29, H-30, H-33, H-34, H-35, H-36, H-37, H-38, H-43, I-01, I-02, I-03, I-05, I-06, I-07, I-08, I-09, I-10, I-13, I-14, I-15, I-17, I-18, I-19, I-20, I-21, I-22, I-23, I-24, I-26, I-27, I-29, I-30, I-31, I-33, I-35, I-36, I-37, I-38, I-39, I-40, I-42, I-43, I-44, I-46, I-47.

- `CORRECTED` entries, used in their corrected wording only: F-34, G-43, G-71, H-10, H-12, I-40.
- `community` entries, worded as reports: F-16, F-45, H-19, H-20, H-21, H-22, H-36.
- `reasoning` entries, labelled as reasoning: G-71, H-23, H-43, I-17, I-18.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). *(Still nothing as of 2026-10-08: no hardware, no code, no test run.)*

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against the source register. Corrected the TC358743 audio statement: the datasheet also allows audio over CSI-2 [A-05], and the driver selects I2S [A-13]. Reworded the fractional-frame-rate row to match [B-28]. Labelled the platform consequences and the read-only-root conclusion as reasoning. Added a Pi 5/CM5 note to the encoder facts [D-35]. Stated the Buildroot partition source [E-19] precisely. Marked the Connect storage figure as an update requirement [G-43]. Added the unit and VBR caveat to the sizing calculation. Normalised the UNKNOWN markers to the README convention. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: the persistent-partition recommendation (Section 9) now references OQ-006, OQ-069 and OQ-094 and stays labelled as a recommendation (no recording ADR exists); updates versus active recordings (OQ-094) added to the status table and Section 5.2; storage-media gaps linked to OQ-098 | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: REQ-BLD-002 (own product OS image) in the header "Applies to" row and the §5 OS-footprint note (stock image is bring-up only); §8 no longer lists the ATEM recording reference as input to an open "ATEM integration scope (OQ-009)" — OQ-009 is answered (HDMI capture only; REQ-ATEM-001, REQ-CAP-008), and the network recording commands are marked reference only. No citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); header "Applies to" row, §1 "Partition layout and update scheme" row and §9 persistent-partition row: ADR-003 (`PROPOSED`) → (`ACCEPTED`); §3 heading "…in the candidate OS builds" → "…in Raspberry Pi OS and the Buildroot alternative". The persistent-partition recommendation itself stays `PROPOSED`; OQ-069 stays open. No evidence, other ADR status or implementation status changed. | Claude (session 2026-10-07) |
| 2026-10-08 | Owner decisions of 2026-10-07 (second set: H.264 + H.265, REQ-ENC-001 / OQ-103; HDMI audio required, REQ-CAP-006 / OQ-004 ANSWERED; CM4 and CM5 side by side, ADR-004 OPEN; any HDMI camera plus ATEM, OQ-102 ANSWERED) and research topics H and I propagated. Header rows and an owner-decision note added. §1: "Audio in recordings" marked superseded (required) and "Codec…" superseded in part (codecs decided; which codec per output is OQ-103); rows added for HEVC in containers, H.265 capacity (OQ-104/105), recorded sample rate (OQ-111), A/V sync (OQ-112), HEVC/AAC licensing (OQ-109/113). §2: H.265 recordings are software on every candidate; x265/`libx265`/`x265enc`, planar-only input, DotProd only on CM5, community cost evidence [H-01]–[H-23], [H-43]; reasoning that an H.265 recording cannot share the H.264 WebRTC encode. §3: HEVC in MP4 and Matroska [H-37], [H-38], FLV [H-26], [H-27], MPEG-TS [H-30] (MPEG-TS and FFmpeg-muxer NEEDS VERIFICATION entries superseded in part); element-name constraint superseded in part; `h265parse` needed [H-13], [H-37]. §4: three H.265 encoder rows [H-14], [H-16], [H-17], [H-37], [H-38]. §5.3: two example bitrates from YouTube's streaming guidance [H-29] and an AAC audio-size note [I-40] (reasoning). §7: "undecided (OQ-004)" marked superseded; new §7.1 capture path on CM4 and CM5, §7.2 sample rate and A/V sync, §7.3 AAC encoders, §7.4 audio tests [I-01]–[I-47]. §9: container note and four OPEN decision rows (recording codec, audio encoder, rate policy, A/V clock model). §10: added coverage (audio, H.265, CM4 and CM5). Verification status: 40 → 107 entries. No requirement, decision, risk or test status changed. | Claude (session 2026-10-08) |
