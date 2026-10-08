# PACSCORDER Streaming (RTMP and WebRTC)

| | |
|---|---|
| Document status | Active — source research only. Streaming design and implementation: NOT STARTED |
| Last updated | 2026-10-08 |
| Applies to | REQ-STR-001 (RTMP), REQ-STR-002 (WebRTC); REQ-ENC-001 (H.264 and H.265 required, owner 2026-10-07; which outputs use H.265 is OQ-103); REQ-CAP-006 (HDMI audio required, owner 2026-10-07; DRAFT); REQ-ATEM-001 (scope notes only: receiving ATEM RTMP and ATEM network control are not in current scope, owner 2026-10-07); ADR-007 (`OPEN`); ADR-004 (`OPEN`; bring-up evaluates CM4 and CM5 side by side); RISK-003, RISK-019, RISK-022, RISK-023, RISK-024, RISK-025; all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5); Raspberry Pi OS (ADR-003, `ACCEPTED`) and Buildroot (documented alternative) |
| Verification | Source research of 2026-10-06, plus research topics H (H.265/HEVC) and I (HDMI audio) of 2026-10-08 ([REFERENCES.md](REFERENCES.md)). Nothing has been tested. No PACSCORDER hardware or code exists as of 2026-10-08. |

This document answers the Rule 25 question "How is streaming performed?". As of 2026-10-06, no streaming design has been chosen. The userspace framework is ADR-007 (`OPEN`). It records:

- the protocol and codec constraints that any design must meet;
- the software components available in Raspberry Pi OS (ADR-003, `ACCEPTED`) and in the documented Buildroot alternative;
- the network ports involved;
- the gaps that remain open.

Fact IDs such as `[F-31]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Facts of tier `community` are worded as reports. Calculations are marked as reasoning and list their inputs. Pipeline sketches are design proposals, not tested commands.

> **Owner decisions of 2026-10-07 that affect streaming** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md); added here 2026-10-08):
>
> - **Codecs.** H.264 **and** H.265 (HEVC) for recording and streaming (REQ-ENC-001). Which outputs use which codec is open: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). HEVC over RTMP is in [§3.6](#36-h265-hevc-over-rtmp-and-srt-added-2026-10-08); H.265 in WebRTC is in [§4.5](#45-h265-in-webrtc-added-2026-10-08).
> - **Audio.** HDMI audio is required in recordings and streams (REQ-CAP-006, DRAFT; OQ-004 ANSWERED). The audio path and the audio encoder for each output are in [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08).
> - **Platforms.** Bring-up evaluates CM4 and CM5 side by side; the product platform (ADR-004) stays `OPEN` until measured.
> - **Sources.** Any HDMI camera (no model list) plus ATEM switcher outputs (OQ-102 ANSWERED; REQ-CAP-008). Reasoning: the audio sample rate and format of the source are therefore not known in advance (RISK-023, OQ-110, OQ-111).

## 1. Status at a glance

| Item | Status |
|---|---|
| REQ-STR-001 RTMP streaming | Acceptance `DRAFT`. Implementation `NOT STARTED`. TEST-STR-001: `BLOCKED — HARDWARE REQUIRED`. |
| REQ-STR-002 WebRTC streaming | Acceptance `DRAFT`. Implementation `NOT STARTED`. TEST-STR-002: `BLOCKED — HARDWARE REQUIRED`. |
| RTMP destinations, RTMPS, bitrate, audio | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-007). *(Superseded in part 2026-10-08: "audio" is decided — HDMI audio is required in streams (owner, 2026-10-07; REQ-CAP-006 DRAFT; OQ-004 ANSWERED). Destinations, RTMPS and bitrate remain OQ-007.)* |
| Video codec per output (H.264, H.265) | Both codecs are required (owner, 2026-10-07; REQ-ENC-001). Which of RTMP and WebRTC carry H.265, at which resolutions and frame rates: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). RISK-022. |
| Software H.265 encode capacity on CM4 and CM5 | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104, OQ-105). No official figure was found [H-19]; see [§2](#2-position-in-the-pipeline). |
| HEVC over RTMP: muxing path with GStreamer 1.26.2 | GStreamer 1.26.2 `flvmux` cannot carry H.265 [H-27]; FFmpeg 7.1.5 can [H-26]. Path: UNKNOWN — VERIFICATION REQUIRED. BUILD TEST REQUIRED; OWNER DECISION REQUIRED (OQ-107). RISK-025. |
| HEVC over RTMP: acceptance at each destination | UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-106). |
| H.265 in WebRTC: which viewer browsers and devices receive it | UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-108). RISK-019. |
| Audio sample-rate detection and output sample rate | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED; BUILD TEST REQUIRED (OQ-111). RISK-023. |
| Audio/video synchronisation and tolerance | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED (OQ-112). RISK-024. |
| HEVC and AAC patent licensing | UNKNOWN — VERIFICATION REQUIRED. LEGAL CLARIFICATION REQUIRED (OQ-109, OQ-113). RISK-015. |
| WebRTC reach (LAN or internet), browsers, viewer count, latency | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-008). |
| H.264 level signalling for 1080p in browsers | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-073). |
| WebRTC signalling and NAT traversal | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-074). |
| RTMP server on PACSCORDER; ports to open | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-075). Receiving an ATEM's RTMP stream is not in current ATEM scope (REQ-ATEM-001, owner 2026-10-07), so the ATEM integration does not need an RTMP server unless the owner adds it ([§3.5](#35-receiving-rtmp-pacscorder-as-a-server)). |
| SRT | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-076). *(2026-10-08: the Raspberry Pi OS stacks contain the components for HEVC over SRT in MPEG-TS [H-30]; whether SRT is required is still OQ-076.)* |
| Audio encoders (AAC for RTMP, Opus for WebRTC) | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063). *(Superseded in part 2026-10-08: which AAC and Opus encoders are available in Raspberry Pi OS is now sourced [I-39], [I-41], [I-44], [I-45], [I-46]; see [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08). The choice and the CPU cost remain UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063).)* |
| PACSCORDER network interface (Ethernet, Wi-Fi) per platform and carrier | UNKNOWN — VERIFICATION REQUIRED. No register fact covers the network interfaces of the candidate boards: DATASHEET REQUIRED (OQ-098). Choice of interface: OWNER DECISION REQUIRED (OQ-018). |
| Userspace framework | ADR-007 `OPEN` (OQ-015). |

## 2. Position in the pipeline

```text
… → V4L2 → DMABUF → Encoder ─┬→ (parser) → FLV mux → RTMP publisher   → RTMP server / service
                             └→ RTP packetiser → WebRTC stack         → browser
```

The stages after the encoder are a generic sketch (reasoning from [F-33], [F-35] and [F-42]). The actual elements are not chosen: ADR-007 (`OPEN`).

The encoder differs per platform. That affects both outputs:

| Platform | Encoder (facts) | Consequence for streaming (reasoning) |
|---|---|---|
| Pi 4 Model B, CM4 | Hardware H.264 encoder, officially specified for 1080p30 encode [D-10] | 1080p60 streaming depends on unproven 1080p60 hardware encode (RISK-002, OQ-056). |
| Pi 5, CM5 | No hardware video encoder [D-31]. "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22]. | Every stream is a CPU software encode (RISK-003). How many simultaneous encodes fit is OQ-059. |

No candidate platform has a hardware HEVC encoder [D-24], [D-31]. Whether recording, RTMP and WebRTC share one encode or use separate encodes is OQ-005.

**H.265 per platform (added 2026-10-08; H.265 is required by REQ-ENC-001).** Bring-up evaluates CM4 and CM5 side by side (ADR-004, `OPEN`). Research topic H covered CM4 and CM5 only. For Pi 4 Model B and Pi 5, only the community benchmarks below name those boards (Pi 5 and a Pi 400); everything else is NEEDS VERIFICATION.

| Item | CM4 | CM5 |
|---|---|---|
| H.265 encoder | Software only [D-24]: x265, as Debian's 4.1-2 package, which Raspberry Pi does not override [H-01], [H-02]. Reasoning: the hardware H.264 encoder [D-10] does not help H.265. | Software only [D-31]: the same x265 4.1-2 [H-01], [H-02]. |
| Framework wrappers | FFmpeg 7.1.5 `libx265` in the Raspberry Pi build [H-08], [H-09]; GStreamer `x265enc` in `gstreamer1.0-plugins-bad` [H-11], [H-12] (CORRECTED) | Same |
| Input format | Planar only; packed UYVY is not accepted by `libx265` [H-10] (CORRECTED) or `x265enc` [H-13]. Reasoning: a CPU conversion from UYVY at 1080p60 reads about 249 MB/s and writes about 187 MB/s before x265 starts [H-43]. | Same |
| SIMD | Cortex-A72 has no DotProd, so x265's Neon DotProd kernels cannot apply; I8MM, SVE and SVE2 paths apply on neither board [H-04], [H-05] | x265's Neon DotProd kernels can apply on Cortex-A76 [H-04], [H-05]; whether the Debian binary contains them and the CM5 kernel enables them: OQ-105 |
| Cost evidence | Community benchmarks report `libx265` "Live" at 4.33 FPS on a Pi 400 (Cortex-A72 @ 1.8 GHz) [H-21] (community source) | Community benchmarks report `libx265` "Live" at 10.00 FPS on Pi 5 [H-20] (community source) |

- A Raspberry Pi engineer stated on the official forum that software H.265 encode "is too intensive an operation to perform at any significant resolution" (community source). No raspberrypi.com figure was found in a site search [H-19].
- The benchmark test behind [H-20] and [H-21] is not a 1080p60 live measurement. It encodes vbench clips with `-threads 1`, using a 2022 x265 snapshot (community source) [H-22].
- Reasoning: in that harness `libx265` was about 6.6 times slower than `libx264` on Pi 5. This is a relative cost only, not a PACSCORDER prediction [H-23].
- Real-time H.265 throughput on CM4 and CM5: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104; TEST-ENC-001). RISK-022.

### 2.1 Audio path and audio encoder per output (added 2026-10-08)

HDMI audio is required in streams (owner, 2026-10-07; REQ-CAP-006 DRAFT; OQ-004 ANSWERED).

```text
HDMI source → TC358743 I2S (2-channel, set at probe [I-24]) → Pi I2S (clock consumer [I-03]) → ALSA card "tc358743" [I-15]
            → (resample if the encoder needs it [I-47]) → AAC encoder → FLV mux → RTMP
                                                       └→ Opus encoder → RTP → WebRTC
```

The sketch is reasoning from the cited facts. The elements are not chosen: ADR-007 (`OPEN`), OQ-063.

**Capture side** (details in [TC358743_DRIVER.md](TC358743_DRIVER.md), [DEVICE_TREE.md](DEVICE_TREE.md) and [HARDWARE.md](HARDWARE.md)):

- **CM4.** The CPU side is `bcm2835-i2s` on GPIO 18–21 [I-10]. It captures exactly 2 channels at 8–384 kHz [I-13].
- **CM5.** The overlay's labels resolve to RP1 I2S1 on GPIO 18–21 [I-07], [I-08], and the firmware does not block the overlay [I-05], [I-06]. No source shows audio being captured that way: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-054; TEST-AUD-001).
- **Channels.** The driver hard-codes 2-channel I2S [I-24]. Reasoning: with the stock driver, streams carry stereo only. Compressed or multichannel input is OQ-110.
- **Sample rate.** Reasoning from source [I-18]:
  - The kernel does not carry the HDMI sample rate into ALSA.
  - If the card is opened at 48 kHz while the source sends 44.1 kHz, the audio runs 8.84 % fast with no error.
  - The driver exposes the rate as a read-only V4L2 control with a change event [I-20], [I-21].
  - Without a wired interrupt, a change takes up to about 1 s to appear [I-22].
  - The rate policy is OQ-111 (RISK-023).
- **A/V clocks.** The TC358743's audio PLL follows the source's audio clock [I-26]. Video buffers are stamped with `CLOCK_MONOTONIC` [I-33]. GStreamer and FFmpeg align the two differently by default [I-35], [I-36], [I-37], [I-38] (OQ-112, RISK-024).

**Audio encoder per output**

| Output | Audio codec the transport carries | Encoders available in Raspberry Pi OS | Rate constraint |
|---|---|---|---|
| RTMP, legacy FLV (H.264) | AAC (SoundFormat 10) [F-31]; `flvmux` needs raw AAC [F-34] (CORRECTED) | GStreamer `voaacenc` (plugins-bad) [I-44]; GStreamer `avenc_aac` (Debian's `gstreamer1.0-libav`, wraps FFmpeg's native encoder) [I-46]; FFmpeg native `aac` [I-39], [I-40] (CORRECTED) | `voaacenc`: S16LE, 8–96 kHz, 1 or 2 channels; `avenc_aac`: F32LE, 7.35–96 kHz including 44.1 and 48 kHz [I-47] |
| RTMP, enhanced FLV (H.265, FFmpeg 7.1.5) | FFmpeg's FLV audio table includes AAC and MP3 and has no Opus [H-26]. YouTube Live lists AAC or MP3 audio [H-29]. | FFmpeg native `aac` [I-39], [I-40] (CORRECTED) | MPEG-4 audio rates [I-40] |
| SRT / MPEG-TS (only if OQ-076 adds SRT) | Audio caps of `mpegtsmux` are not in the register: NEEDS VERIFICATION | As for RTMP (reasoning) | NEEDS VERIFICATION |
| WebRTC | Opus or G.711 required; AAC is not a required WebRTC codec [F-41] | FFmpeg `libopus` [I-39], [I-41]; GStreamer `opusenc` (plugins-base) [I-45]. FFmpeg's native `opus` encoder is experimental and CELT-only [I-41]. | 48, 24, 16, 12 or 8 kHz only, so a 44.1 kHz source needs resampling first [I-41], [I-47] |

- **FFmpeg native `aac`** (CORRECTED wording [I-40]):
  - It is FFmpeg 7.1's default AAC encoder and is not flagged experimental.
  - Without `-b` it defaults to 128 kb/s for stereo.
  - An explicit `-b` selects CBR.
- **`opusenc`** defaults to 64000 bit/s, constrained VBR [I-47].
- **`fdk-aac` is not an option.**
  - It is not shipped in the Raspberry Pi FFmpeg or GStreamer builds [I-39], [I-44].
  - Debian ships it in non-free, under a licence that Debian calls incompatible with every GPL version [I-43].
  - A GPL FFmpeg build can enable it only with `--enable-nonfree`, which makes the result unredistributable [I-42].
- **Two audio encodes.** Reasoning from [H-26] and [F-41] (as in OQ-063 and RISK-019): when RTMP and WebRTC with audio run at the same time, two audio encodes are needed, AAC and Opus.
- **Still open.**
  - CPU cost of each encoder alongside the video encodes on CM4 and CM5: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063).
  - AAC patent licensing: LEGAL CLARIFICATION REQUIRED (OQ-113).

## 3. RTMP (REQ-STR-001)

### 3.1 Protocol and codec constraints

- **Codecs in legacy FLV/RTMP.** Video CodecID 7 = AVC (H.264) is the only modern video codec. Audio SoundFormat 10 = AAC. HEVC, AV1, VP9, VP8 and Opus need Enhanced RTMP (E-RTMP) FourCC signalling [F-31].
- **Reasoning:** H.264 video with AAC audio can be carried without Enhanced RTMP; HEVC cannot [F-31]. No candidate platform can encode HEVC in hardware anyway [D-24], [D-31]. *(Superseded 2026-10-08: "anyway" no longer applies. H.265 is required (owner, 2026-10-07; REQ-ENC-001), so if RTMP carries H.265 (OQ-103), it needs Enhanced RTMP and a software encoder; see [§3.6](#36-h265-hevc-over-rtmp-and-srt-added-2026-10-08).)*
- **Enhanced RTMP and HEVC** (added 2026-10-08). The Enhanced RTMP specification (document version v2-2026-01-31-r2) defines the HEVC FourCC `hvc1` and lists it among the `fourCcList` connect-command values [H-24].
- **URL form and port.** In FFmpeg, RTMP URLs take the form `rtmp://[username:password@]server[:port][/app][/instance][/playpath]`. The default TCP port is 1935 [F-32].
- **RTMPS.** GStreamer `rtmp2sink` supports RTMP and RTMPS [F-33]. Whether any destination requires RTMPS is OQ-007.

### 3.2 Publishing with GStreamer

**`rtmp2sink`**

- Plugin `rtmp2`, GStreamer Bad Plug-ins.
- It accepts `video/x-flv` on its sink pad.
- The documented pipeline is `x264enc ! flvmux ! rtmp2sink location=rtmp://...` [F-33].

**`flvmux`** (GStreamer Good Plug-ins)

- It needs H.264 as `video/x-h264,stream-format=avc`.
- It needs AAC as `audio/mpeg,mpegversion={4,2},stream-format=raw`.
- It outputs `video/x-flv`.
- Its `streamable` property drops indexes and duration for live streaming [F-34] (CORRECTED).
- *(Added 2026-10-08.)* In GStreamer 1.26.2, its video sink pad has no H.265. It accepts only `video/x-flash-video`, `video/x-flash-screen`, `video/x-vp6-flash`, `video/x-vp6-alpha` and `video/x-h264,stream-format=avc`. The distribution's GStreamer therefore cannot put HEVC into FLV/RTMP [H-27].

**Parser requirement (reasoning)**

- `v4l2h264enc` outputs `stream-format=byte-stream, alignment=au`. Because `flvmux` needs `stream-format=avc`, an `h264parse` element or an equivalent conversion is needed between them [F-35].
- Which package provides `h264parse`: NEEDS VERIFICATION (not in the register).

**Pi 4/CM4 hardware encoder in GStreamer**

- `v4l2h264enc` is not a static element. It is registered at plugin load by probing M2M devices, and only when V4L2 probing is compiled in [D-38].
- Raspberry Pi OS's `gstreamer1.0-plugins-good` ships `libgstvideo4linux2.so` with "the probed v4l2 M2M elements" [G-26].
- Buildroot needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y`; without it `v4l2h264enc` is not registered [D-39], [G-65].

**Official Raspberry Pi pipeline elements**

- The official streaming pipeline uses `v4l2h264enc extra-controls="controls,repeat_sequence_header=1"` with caps `video/x-h264,level=(string)4`.
- On Raspberry Pi 5 the documentation says to replace that encoder with `x264enc speed-preset=1 threads=1` [D-37].

**Input formats for the software encoders**

- `x264enc` does not accept packed UYVY [D-40]. FFmpeg `libx264` does not either [D-43].
- The TC358743 UYVY output (ADR-005, `PROPOSED`) therefore needs a conversion on Pi 5/CM5.
- *(Added 2026-10-08.)* The H.265 encoders accept planar input only: `x265enc` accepts Y444, Y42B and I420 at 8-bit [H-13], and FFmpeg `libx265` accepts planar or gray formats only [H-10] (CORRECTED). Reasoning: because H.265 is software-encoded on every candidate [D-24], [D-31], an H.265 output needs this conversion on CM4 as well as on CM5 [H-43].
- See [VIDEO_ENCODER.md](VIDEO_ENCODER.md).

**`eflvmux`** (E-RTMP v2 FourCC muxing for H.265 `hvc1` and AV1)

- It first appears in the GStreamer 1.28 branch and is absent from 1.24 and 1.26.
- Buildroot master ships GStreamer 1.24.13, so it is not available in a stock Buildroot build [F-34] (CORRECTED).
- Reasoning, with inputs [F-34] and [G-26]: `flvmux` and `eflvmux` live in GStreamer Good Plug-ins [F-34], and Raspberry Pi OS trixie uses Debian's `gstreamer1.0-plugins-good` 1.26.2 without a Raspberry Pi override [G-26], so `eflvmux` is not available there either.

**PROPOSED pipeline shapes.** These are Claude's reasoning, linked to ADR-007 (`OPEN`). They are not commands and are NOT YET RUN ON PACSCORDER HARDWARE. Element properties beyond those cited are NEEDS VERIFICATION.

```text
Pi 4 / CM4 :  capture → v4l2h264enc (repeat_sequence_header=1 [D-37]) → h264parse [F-35] → flvmux (streamable [F-34]) → rtmp2sink [F-33]
Pi 5 / CM5 :  capture → UYVY-to-I420/NV12 conversion [D-40] → x264enc [D-37] → flvmux [F-33] → rtmp2sink [F-33]
Audio      :  audio capture (if required, OQ-004) → AAC encoder (OQ-063) → flvmux (raw AAC [F-34])
Audio (2026-10-08; audio is required, REQ-CAP-006 — the "if required" above is superseded):
              alsasrc [I-45] on card tc358743 [I-15] → voaacenc [I-44] or avenc_aac [I-46] (OQ-063) → flvmux (raw AAC [F-34])
H.265      :  not possible with flvmux in GStreamer 1.26.2 [H-27]; see §3.6
```

### 3.3 Publishing with FFmpeg

- **Documented publish example** (NOT YET RUN ON PACSCORDER HARDWARE) [F-32]:

  ```bash
  ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream
  ```

- **Options.** The `listen` option makes FFmpeg act as an RTMP server. `rtmp_enhanced_codecs` advertises E-RTMP FourCCs such as `hvc1,av01,vp09` [F-32].
  - *(Added 2026-10-08.)* In FFmpeg 7.1.5, `rtmp_enhanced_codecs` writes a `fourCcList` in the connect command but accepts only `hvc1`, `av01` and `vp09`; any other FourCC fails [H-26].
- **HEVC in FLV** (added 2026-10-08).
  - FFmpeg 6.1 was the first release to mux HEVC into FLV and signal it over RTMP [H-25].
  - FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP publishing. Its FLV muxer maps HEVC to FourCC `hvc1` [H-26].
  - Its FLV audio table has no Opus [H-26].
- **Encoders.**
  - `h264_v4l2m2m` is the V4L2 mem2mem hardware wrapper.
  - `libx264` requires FFmpeg built with `--enable-gpl` [D-42].
  - *(Added 2026-10-08.)* `libx265`: the Raspberry Pi FFmpeg 7.1.5 is configured with `--enable-libx265` [H-08], and its `libavcodec61` links `libx265-215` [H-09].
    - It accepts planar or gray input only, never packed `uyvy422` [H-10] (CORRECTED).
    - The wrapper copies FFmpeg's thread count into x265's frame threads after applying preset and tune [H-10]. `tune=zerolatency` sets one frame thread [H-16]. Reasoning from [H-10] and [H-16]: a `-threads` value therefore replaces zerolatency's single frame thread unless it is set explicitly. Latency and throughput for both settings: HARDWARE TEST REQUIRED; BUILD TEST REQUIRED (OQ-104).
  - *(Added 2026-10-08.)* Audio: the Raspberry Pi build has the native `aac` encoder and the `libopus` wrapper, and no `libfdk_aac` [I-39]. Linking `libx264` and `libx265` makes it a GPL build [I-39]. See [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08).
- **SRT** (added 2026-10-08): the full Raspberry Pi FFmpeg build has `--enable-libsrt` [H-08], [H-30].
- **Capture timestamps** (added 2026-10-08). In FFmpeg 7.1 the ALSA input stamps packets with wall-clock time, while the V4L2 input passes monotonic timestamps through by default. Mixing them without `-ts abs` or `mono2abs` mixes clock bases, and the CLI's default per-input start shift discards the real offset between the inputs [I-38] (OQ-112, RISK-024).
- **Upstream V4L2 M2M behaviour.** Upstream FFmpeg's V4L2 M2M code uses MMAP buffers only, so each raw frame is copied. Its encoder forces B-frames to 0 [D-44].
- **Raspberry Pi OS.**
  - FFmpeg is 7.1.5 `+rpt2` [G-29].
  - Its patch adds DMABUF input to the V4L2 M2M encoder [D-45].
- **Buildroot.**
  - FFmpeg is upstream 6.1.5 without Raspberry Pi V4L2/DRM_PRIME patches [D-46] (CORRECTED).
  - It enables `--enable-libx264` only when both `BR2_PACKAGE_X264` and `BR2_PACKAGE_FFMPEG_GPL` are set [E-36].
- **WebRTC output from FFmpeg** was not researched: NEEDS VERIFICATION.

### 3.4 Package availability

| Component | Raspberry Pi OS (trixie) | Buildroot 2026.08 |
|---|---|---|
| GStreamer version | 1.26.2 [G-26], [G-27], [G-31] | 1.24.13 [E-31], [G-64] |
| Installed in the stock Lite image | No GStreamer packages installed [G-32] | — |
| `rtmp2sink` / `rtmp2src` | `libgstrtmp2` is in Debian's `gstreamer1.0-plugins-bad` `1.26.2-3+deb13u3`. The Raspberry Pi archive carries its own build, `1.26.2-3+rpt4+deb13u3`, which sorts higher [G-27]. The file list of the Raspberry Pi build is not in the register: NEEDS VERIFICATION. | `BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2`, no external library [E-34], [F-33]. The older `..._PLUGIN_RTMP` selects rtmpdump (librtmp) [E-34]. |
| `flvmux` | `libgstflv` in `gstreamer1.0-plugins-good` [G-26]. No H.265 on its video sink pad in 1.26.2 [H-27] (added 2026-10-08). | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV` [E-34], [F-34] |
| `eflvmux` | Not available (1.26.2; reasoning from [F-34], [G-26]) | Not available (1.24.13) [F-34] |
| `v4l2h264enc` (Pi 4/CM4) | `libgstvideo4linux2.so`, probed elements [G-26] | Needs `..._PLUGIN_V4L2_PROBE=y` [D-39], [G-65] |
| `x264enc` (Pi 5/CM5) | Debian trixie has `x264` `2:0.164.3108+git31e19f9-2+b1`; the Raspberry Pi archive has no `x264` or `libx264-164` package [G-30]. The package that provides the GStreamer `x264enc` plugin on Raspberry Pi OS: NEEDS VERIFICATION. | `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264`, which selects x264 [D-47], [E-36] |
| FFmpeg | 7.1.5 `+rpt2` [G-29] | 6.1.5 [E-36], [G-64] |
| MediaMTX | Debian / Raspberry Pi OS package: NEEDS VERIFICATION | No `package/mediamtx` in Buildroot master, as reported in [F-44] (community tier) |
| x265 library (H.265; rows added 2026-10-08) | Debian's x265 4.1-2 (`libx265-215`); the Raspberry Pi archive does not override it [H-01], [H-02] | NEEDS VERIFICATION — no register fact |
| `x265enc` (GStreamer) | `libgstx265.so` in Debian's `gstreamer1.0-plugins-bad` 1.26.2-3+deb13u3 [H-11]; the Raspberry Pi build `1.26.2-3+rpt4+deb13u3` still installs it [H-12] (CORRECTED) | NEEDS VERIFICATION — no register fact |
| FFmpeg `libx265` | `--enable-libx265` in the Raspberry Pi FFmpeg; `libavcodec61` depends on `libx265-215` [H-08], [H-09] | NEEDS VERIFICATION — no register fact |
| `rtph265pay` (WebRTC H.265) | In GStreamer 1.26.2; profile-id, tier-flag and level-id in its output caps arrived only in 1.26.4 [H-31] | NEEDS VERIFICATION — no register fact |
| SRT and MPEG-TS (HEVC over SRT) | Debian trixie's `gstreamer1.0-plugins-bad` ships `libgstsrt.so` and `libgstmpegtsmux.so`; 1.26.2 `mpegtsmux` accepts H.265; the Raspberry Pi FFmpeg has `--enable-libsrt` [H-30]. Whether the Raspberry Pi build of plugins-bad contains the same files: NEEDS VERIFICATION. | NEEDS VERIFICATION — no register fact |
| `alsasrc` (audio capture) | `gstreamer1.0-alsa` 1.26.2-1+rpt3+deb13u2 [I-45] | NEEDS VERIFICATION — no register fact |
| AAC encoders | `voaacenc` in the Raspberry Pi `gstreamer1.0-plugins-bad`; `fdkaacenc` is not shipped [I-44]. `avenc_aac` in Debian's `gstreamer1.0-libav` [I-46]. FFmpeg native `aac`; no `libfdk_aac` [I-39]. | NEEDS VERIFICATION — no register fact |
| Opus encoders | `opusenc` in the Raspberry Pi `gstreamer1.0-plugins-base` 1.26.2-1+rpt3+deb13u2 [I-45]; FFmpeg `libopus` [I-39], [I-41] | NEEDS VERIFICATION — no register fact |

### 3.5 Receiving RTMP (PACSCORDER as a server)

> **Not in current ATEM scope (owner decision of 2026-10-07; OQ-009 ANSWERED).** REQ-ATEM-001 now covers HDMI capture of the ATEM output only (REQ-CAP-008). Receiving the ATEM's RTMP stream was offered and not selected, so it is out of scope unless the owner adds it. This section is kept as reference only. The RTMP and WebRTC *output* requirements, REQ-STR-001 and REQ-STR-002, are unchanged.

- **A server would be required.** Reasoning [F-46]: an ATEM streaming over RTMP is an RTMP *client* that publishes to a configured URL and key. To receive it, PACSCORDER must run a listening RTMP server, such as MediaMTX on :1935 or FFmpeg in `listen` mode [F-32].
- **`rtmp2src` cannot do this.** It only connects to an RTMP server as a client and cannot accept incoming publish connections [F-33].
- **MediaMTX codecs.** The MediaMTX project reports that it accepts RTMP publishers using AV1, VP9, H265 or H264 video (Enhanced RTMP), and Opus, FLAC, AAC, MP3, AC-3, G711 or LPCM audio [F-45].
- **Open questions.** Whether PACSCORDER hosts an RTMP server at all is OQ-075. Whether an ATEM will publish to a LAN-only PACSCORDER URL is OQ-080, which matters only if receiving ATEM RTMP is added to scope. See [ATEM.md](ATEM.md).

### 3.6 H.265 (HEVC) over RTMP and SRT (added 2026-10-08)

H.264 and H.265 are both required (owner, 2026-10-07; REQ-ENC-001). Whether RTMP carries H.265, H.264 or both is open: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). This section records what the sources say about each way of carrying H.265.

**Muxing HEVC into FLV with the distribution's packages**

| Framework | HEVC into FLV/RTMP | Source |
|---|---|---|
| GStreamer 1.26.2 (Raspberry Pi OS) | **No.** `flvmux` has no H.265 on its video sink pad. | [H-27] |
| GStreamer `eflvmux` | E-RTMP v2 FourCC muxing for H.265 (`hvc1`) and AV1. It first appears in the 1.28 branch and is absent from 1.24 and 1.26. | [F-34] (CORRECTED) |
| FFmpeg 7.1.5 (Raspberry Pi OS) | **Yes.** It muxes HEVC + AAC into enhanced FLV (FourCC `hvc1`) for RTMP publishing. | [H-26] |
| FFmpeg 6.1.5 (Buildroot) | Reasoning: FFmpeg 6.1 was the first release to mux HEVC into FLV [H-25], and Buildroot ships 6.1.5 [E-36]. Whether that build includes the HEVC encoder is NEEDS VERIFICATION. | [H-25], [E-36] |

Reasoning from [H-26], [H-27] and [F-34]: with Raspberry Pi OS packages, only FFmpeg can publish HEVC over RTMP. If GStreamer is chosen for the rest of the pipeline (ADR-007, `OPEN`), the options listed in OQ-107 are:

- hand the HEVC elementary stream to FFmpeg 7.1.5;
- backport or build GStreamer 1.28's `eflvmux` against 1.26.2;
- carry a newer GStreamer in the image (ADR-003);
- use SRT/MPEG-TS instead of RTMP for HEVC.

Path: UNKNOWN — VERIFICATION REQUIRED. BUILD TEST REQUIRED; OWNER DECISION REQUIRED (OQ-107). RISK-025.

**Destinations**

- **YouTube Live** [H-29]:
  - Codecs: its encoder-settings page lists H.264, H.265 (HEVC) and AV1 over RTMP/RTMPS, with AAC or MP3 audio.
  - Frame rate and keyframes: up to 60 fps; 2 s keyframes recommended, not more than 4 s; CBR.
  - 1080p60 bitrate: 12 Mbps recommended (4 Mbps minimum) for H.265 and AV1, against 17 Mbps (6 Mbps minimum) for H.264.
  - The page does not use the term "Enhanced RTMP".
- **Signalling.** Whether YouTube accepts the signalling FFmpeg 7.1.5 sends, and whether `-rtmp_enhanced_codecs hvc1` is needed: UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-106).
- **Other destinations.** Other ingest services and customer RTMP servers were not checked (research gap, topic H; OQ-106). The destinations themselves are OQ-007.
- **MediaMTX.** Its documentation lists H265 among RTMP publish and read codecs and describes its RTMP as expanded with Enhanced RTMP [H-28].

**Audio with HEVC over RTMP**

- FFmpeg 7.1.5's FLV muxer has no Opus [H-26]; YouTube lists AAC or MP3 [H-29].
- Reasoning: the HEVC RTMP output carries AAC, as the H.264 one does. See [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08).

**SRT / MPEG-TS** (whether SRT is required is OQ-076, OWNER DECISION REQUIRED)

- **Components exist** in the distribution stacks [H-30]:
  - GStreamer 1.26.2 `mpegtsmux` accepts `video/x-h265,stream-format=byte-stream`;
  - trixie's `gstreamer1.0-plugins-bad` ships the SRT and MPEG-TS plugins;
  - the Raspberry Pi FFmpeg has `--enable-libsrt`.
- **Encoder output matches.** `x265enc` outputs `video/x-h265,stream-format=byte-stream,alignment=au` [H-13]. Reasoning: that matches `mpegtsmux`'s sink caps [H-30].
- **MediaMTX.** Its documentation lists H265 and H264 among SRT publish video codecs [H-28].
- **Reasoning:** SRT/MPEG-TS is one way to carry HEVC without Enhanced RTMP (OQ-107).
- **Not in the register:** the SRT element names and properties: NEEDS VERIFICATION.

**Encoder settings that matter for live H.265**

- `x265enc` exposes `speed-preset`, `tune`, `bitrate` (kbit/s) and `key-int-max` [H-14].
- `tune=zerolatency` sets B-frames to 0, lookahead depth 0 and one frame thread; only wavefront row parallelism remains [H-16].
- GStreamer 1.26.2 `x265enc` reports a hard-coded latency of 5 frames unless `tune=zerolatency` (then 0). Latency computed from the real parameters arrived only in 1.26.8 and is not backported [H-15].
- In FFmpeg, `-threads` replaces zerolatency's single frame thread unless set explicitly (reasoning from [H-10], [H-16]; [§3.3](#33-publishing-with-ffmpeg)).
- Trixie's x265 4.1-2 lacks the AArch64 speed-ups of x265 4.2 and 4.3 [H-07]. Whether a newer x265 will be provided is OQ-105.

**PROPOSED pipeline shapes.** These are Claude's reasoning, linked to ADR-007 (`OPEN`) and OQ-107. They are not commands and are NOT YET RUN ON PACSCORDER HARDWARE. Element properties beyond those cited are NEEDS VERIFICATION.

```text
H.265 RTMP (FFmpeg) :  V4L2 capture [I-38] → UYVY-to-planar conversion [H-10], [H-43] → libx265 (tune=zerolatency [H-16]) → enhanced FLV (hvc1) [H-26] → RTMP [F-32]
                       ALSA capture [I-38] → native aac [I-40] → same enhanced FLV [H-26]   (timestamps: -ts abs or mono2abs, OQ-112 [I-38])
H.265 SRT (GStreamer, only if OQ-076 adds SRT) :
                       capture → UYVY-to-I420 conversion [H-13], [H-43] → x265enc [H-11] → mpegtsmux [H-30] → SRT sink (element NEEDS VERIFICATION) [H-30]
```

**Capacity.** Running an H.265 RTMP encode beside an H.264 encode (for WebRTC, [§4.5](#45-h265-in-webrtc-added-2026-10-08)) is two concurrent video encodes. On CM5 both are software encodes [D-31]; on CM4 the H.265 one is (reasoning from [D-10], [D-24]). Whether this fits: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104, OQ-059; TEST-ENC-001, TEST-PERF-001). RISK-022.

## 4. WebRTC (REQ-STR-002)

### 4.1 Codec requirements from the standards and from libwebrtc

**RFC 7742** [F-36]

- WebRTC browsers MUST implement VP8 and H.264 Constrained Baseline.
- H.264 endpoints MUST support Constrained Baseline Level 1.2 and SHOULD support Constrained High Level 1.3.
- `packetization-mode` 1 MUST be supported.
- `profile-level-id` MUST appear in the SDP.
- SPS/PPS MUST be sent in-band; `sprop-parameter-sets` MUST NOT appear in the SDP.

**RFC 6184** [F-37] (CORRECTED)

- `profile-level-id` is the hex encoding of three SPS bytes: `profile_idc`, `profile-iop` and `level_idc`.
- Constrained Baseline can be signalled three ways: `42` with `x1xx0000`, `4D` with `1xxx0000`, or `58` with `11xx0000`.
- If `level-asymmetry-allowed` is absent, it is inferred as 0. Both directions must then use the same level: the lower of the two default levels.

**Decoding `42e01f`** (reasoning [F-38])

- `profile_idc` 0x42 = Baseline.
- `profile-iop` 0xE0 sets `constraint_set1_flag`, which makes it Constrained Baseline.
- `level_idc` 0x1F = 31, i.e. Level 3.1.

**libwebrtc (Chromium)** [F-39]

- It defines `kH264ProfileLevelConstrainedBaseline = "42e01f"` and `kH264ProfileLevelConstrainedHigh = "640c1f"`.
- It assumes Constrained Baseline Level 3.1 when `profile-level-id` is absent.
- Its built-in H.264 encoder encodes Constrained Baseline only.
- It advertises Baseline, Constrained Baseline and Main at Level 3.1.

**Level arithmetic for 1080p** (reasoning, from H.264 Table A-1 as encoded in FFmpeg [F-40]):

| Item | Frame size (MBs) | MB rate (MB/s) | Result |
|---|---|---|---|
| Level 3.1 limits | MaxFS 3600 | MaxMBPS 108,000 | 1080p does not fit |
| Level 4 limits | MaxFS 8192 | MaxMBPS 245,760 | — |
| Level 4.2 limits | MaxFS 8704 | MaxMBPS 522,240 | — |
| 1080p30 (120 × 68 MBs) | 8160 | 244,800 | Needs Level 4 (`level_idc` 0x28) or higher |
| 1080p60 (120 × 68 MBs) | 8160 | 489,600 | Needs Level 4.2 (`level_idc` 0x2A) |

**A strict `42e01f` negotiation does not cover 1080p** [F-40]. What browsers actually do with a higher-level stream is OQ-073 (RISK-019).

**B-frames.** The MediaMTX project reports that browsers deliberately do not support H.264 B-frames in WebRTC. It recommends re-encoding to H.264 Baseline plus Opus [F-45].

**Audio.** RFC 7874 requires WebRTC endpoints to implement Opus and G.711 (PCMA/PCMU). AAC is not a required WebRTC codec, so AAC audio must be transcoded, typically to Opus, for browser playback [F-41].

*(Added 2026-10-08; audio is required, REQ-CAP-006.)* Opus encoders available in Raspberry Pi OS:

- FFmpeg's `libopus` wrapper [I-39], [I-41]. FFmpeg's native `opus` encoder is experimental and CELT-only [I-41].
- GStreamer `opusenc` from plugins-base [I-45]. Its default is 64000 bit/s, constrained VBR [I-47].
- Both accept only 48, 24, 16, 12 or 8 kHz. A 44.1 kHz HDMI source therefore needs resampling before Opus [I-41], [I-47] (OQ-111).
- Reasoning: Opus is encoded from the captured PCM directly, not transcoded from AAC, if the audio is encoded on PACSCORDER. See [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08).

### 4.2 Candidate encoders against these requirements

| Requirement | Pi 4 / CM4 hardware encoder | Pi 5 / CM5 software encoder |
|---|---|---|
| Constrained Baseline profile | The profile menu includes Constrained Baseline; the default is High [D-11]. Reasoning: the profile must be set explicitly for WebRTC [F-36]. | `openh264enc` can output Constrained Baseline but accepts only I420 input [D-41]. How to select the profile in `x264enc` or `libx264`: NEEDS VERIFICATION. |
| No B-frames | Produces no B-frames [D-14]. FFmpeg `h264_v4l2m2m` forces B-frames to 0 [D-44]. | `rpicam-apps` normal-mode `libx264` settings use `max_b_frames=1`; its `--low-latency` mode uses `tune zerolatency` [D-35] and drops B-frames [D-32]. Reasoning, from the MediaMTX project's report that browsers do not support H.264 B-frames [F-45]: B-frames must be disabled for browser WebRTC. |
| SPS/PPS in-band | `REPEAT_SEQ_HEADER` defaults to off [D-15]. The official pipeline sets `repeat_sequence_header=1` [D-37]. | NEEDS VERIFICATION |
| Level that covers 1080p | Level menu 1.0–5.1, default 4.0. The driver states the hardware spec is Level 4.0 and higher levels "may not be able to keep up with real-time" [D-12]. Reasoning: 1080p30 fits Level 4; 1080p60 needs Level 4.2 [F-40], beyond the hardware spec (RISK-002). | `rpicam-apps` forces Level 4.2 when the MB rate exceeds 245,760 MB/s, i.e. at 1080p60 [D-36] (reasoning). |

As reported in a Raspberry Pi engineer's 2020 TC358743 instructions (community source), the GStreamer example sets `h264_profile=4` (High) and `h264_level=10` (Level 3.2) [D-54]. Reasoning: High is not Constrained Baseline [F-36], so those settings are not suitable as-is for browser WebRTC.

### 4.3 Implementation options

| Option | Facts | Gaps |
|---|---|---|
| **`webrtcbin`** (GStreamer) | Plugin `webrtc` in gst-plugins-bad, licence "LGPL". It implements most of the W3C RTCPeerConnection API. It takes and produces `application/x-rtp`. It has **no built-in signalling**. It needs libnice to build [F-42]. **Raspberry Pi OS:** Debian's `gstreamer1.0-plugins-bad` `1.26.2-3+deb13u3` ships `libgstwebrtc`, `libgstwebrtcdsp`, `libgstsrtp`, `libgstdtls` and `libgstsctp`; Raspberry Pi OS installs the Raspberry Pi build `1.26.2-3+rpt4+deb13u3` instead [G-27], whose file list NEEDS VERIFICATION. `gstreamer1.0-nice` (0.1.22) provides the `nicesrc`/`nicesink` elements that `webrtcbin` needs at runtime [G-28]. **Buildroot:** `BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_WEBRTC` depends on `!BR2_STATIC_LIBS`. It selects plugins-base, libnice, and the DTLS (OpenSSL), SCTP and SRTP (libsrtp) plugins. libnice builds its GStreamer elements only when plugins-base is enabled [E-35], [F-42]. | Signalling and ICE design (OQ-074) |
| **`webrtcsink`** (gst-plugins-rs, plugin `rswebrtc`) | MPL-2.0. It includes a simple signalling server. It encodes internally (VP8, H.264, VP9, H.265, AV1; Opus audio). It offers Google Congestion Control. Buildroot master has no gst1-plugins-rs package [F-43]. | Raspberry Pi OS / Debian package: NEEDS VERIFICATION |
| **MediaMTX** (community-reported) | As reported by the MediaMTX project: MIT-licensed, zero-dependency single executable for Linux, Windows and macOS. It converts between RTMP, WebRTC, SRT, RTSP and HLS. Buildroot master has no `package/mediamtx` [F-44] (community-tier entry). WebRTC readers get AV1, VP9, VP8, H265 or H264 video, and only Opus, G722 or G711 audio. Access is through a browser page at `:8889/<path>` or WHEP at `/<path>/whep` [F-45]. | Raspberry Pi OS package: NEEDS VERIFICATION. Reasoning: an RTMP ingest with AAC audio would need audio transcoding for WebRTC readers [F-41], [F-45]. |

Choosing among these is part of ADR-007 (`OPEN`) and OQ-074. Licences: `webrtcbin` LGPL [F-42], `webrtcsink` MPL-2.0 [F-43], MediaMTX MIT (as reported [F-44]). x264 is GPL [D-47]. See OQ-087 and RISK-015.

### 4.4 Gaps the sources do not cover

| Topic | State | OQ |
|---|---|---|
| WHIP (ingest) and WHEP (egress) standards | Not sourced. The MediaMTX project reports a WHEP endpoint [F-45]. | OQ-074 |
| ICE, STUN and TURN for viewers behind NAT | Not sourced. The MediaMTX project reports a `webrtcLocalUDPAddress` default of `:8189` [F-44]; [F-44] gives only the setting name and port. Treating it as an ICE or RTP media port would be reasoning from the setting name, not a sourced fact (NEEDS VERIFICATION). | OQ-074, OQ-008 |
| SRT | ATEM streams over SRT [F-06], [F-26]. The MediaMTX project reports SRT conversion [F-44]. GStreamer and Buildroot SRT support were not researched. *(Superseded in part 2026-10-08: the Raspberry Pi OS GStreamer and FFmpeg SRT components are now sourced [H-30] — see [§3.6](#36-h265-hevc-over-rtmp-and-srt-added-2026-10-08). Buildroot SRT support is still not researched.)* | OQ-076 |
| Browser behaviour with a 1080p stream above the negotiated level | Not sourced; needs an interoperability test in Chrome, Firefox and Safari | OQ-073 |
| Audio encoders (AAC, Opus): element choice, licence, CPU cost | Not sourced *(Superseded in part 2026-10-08: availability and the `fdk-aac` licence are now sourced [I-39], [I-41], [I-42], [I-43], [I-44], [I-45], [I-46], [I-47]; element choice and CPU cost are still open, and AAC patent licensing is OQ-113.)* | OQ-063, OQ-113 |
| H.265 receive per browser and device (added 2026-10-08) | Partly sourced ([§4.5](#45-h265-in-webrtc-added-2026-10-08)); per-device behaviour not tested | OQ-108 |
| HDMI audio sample-rate changes during a stream (added 2026-10-08) | Driver control and event sourced [I-20], [I-21]; behaviour of the TC358743 output during a change unknown | OQ-111 |
| A/V synchronisation in live outputs (added 2026-10-08) | Clock handling sourced [I-33], [I-35], [I-36], [I-37], [I-38]; no measurement | OQ-112 |

### 4.5 H.265 in WebRTC (added 2026-10-08)

H.265 is required for streaming (REQ-ENC-001). Whether WebRTC carries it is OQ-103.

**Standards**

- RFC 7742 requires VP8 and H.264 Constrained Baseline. It mentions H.265 only as a reference for SEI orientation messages, not as a required codec [H-32].
- RFC 7798 defines the HEVC RTP payload, and GStreamer's `rtph265pay` implements it. Profile-id, tier-flag and level-id in its output caps arrived only in 1.26.4, after the distribution's 1.26.2 [H-31].

**Browsers**

| Browser | H.265 in WebRTC | Source |
|---|---|---|
| Chrome 136+ | On by default on desktop, Android and WebView, **only where the platform provides H.265 in hardware**; no software fallback | [H-33] |
| Safari 18.0 | Added the standard HEVC RTP payload format for WebRTC | [H-34] |
| Firefox | No evidence of support was found; Mozilla's standards-position issue has no position, and Chromestatus records "No signal" | [H-35] |
| Edge 147 (Windows 11) | Reported not enabled by default as of May 2026, in a Microsoft Q&A answer by a moderator labelled "Microsoft External Staff" (community source) | [H-36] |

**Senders**

- `webrtcsink` encodes H.265 internally [F-43].
- MediaMTX's documentation lists H265 among WebRTC read codecs [H-28].

**Consequences** (reasoning from [H-32] to [H-36], as in RISK-019 and OQ-108):

- An H.265-only WebRTC stream would not play in Firefox.
- It would play in Chrome only on platforms with hardware HEVC decode, and not in Edge by default.
- An H.264 WebRTC track therefore has to remain for browser reach.
- If H.265 is also offered, CM5 needs a second concurrent software video encode (OQ-104). On CM4 the H.264 track can use the hardware encoder [D-10], while H.265 is software [D-24].
- Which viewer browsers and devices must receive H.265: UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-108; OQ-008 for the target browsers).

## 5. Network ports

Defaults from the sources. The ports PACSCORDER actually opens are UNKNOWN — VERIFICATION REQUIRED, OWNER DECISION REQUIRED (OQ-075).

| Port | Transport | Service | Relevance to PACSCORDER | Source |
|---|---|---|---|---|
| 1935 | TCP | RTMP default port | Outbound when publishing (REQ-STR-001). Inbound only if PACSCORDER hosts an RTMP server (OQ-075); receiving ATEM RTMP is not in current ATEM scope (REQ-ATEM-001). | [F-32], [F-44] |
| 1935 | TCP | Forwarded to an ATEM Streaming Bridge, or to an ATEM Mini Extreme ISO G2, for internet links | ATEM-side network setup (see [ATEM.md](ATEM.md)). Reference only: sending PACSCORDER video into an ATEM setup is not in current ATEM scope (REQ-ATEM-001, owner 2026-10-07). | [F-28], [F-29] |
| 8889 | Not stated in the register (serves the browser page and the WHEP endpoint [F-45]) | MediaMTX `webrtcAddress` default | Inbound, only if MediaMTX is used for WebRTC | [F-44], [F-45] |
| 8189 | UDP (reasoning from the setting name `webrtcLocalUDPAddress`) | MediaMTX `webrtcLocalUDPAddress` default. [F-44] gives only the setting name and `:8189`; calling it a WebRTC local UDP listener (for example for ICE/RTP) is reasoning from the name, and its exact role is NEEDS VERIFICATION | Inbound, only if MediaMTX is used for WebRTC | [F-44] |
| 8554 | Not stated in the register | MediaMTX RTSP default | Only if enabled | [F-44] |
| 8890 | Not stated in the register | MediaMTX SRT default | Only if SRT is required (OQ-076) | [F-44] |
| 8888 | Not stated in the register | MediaMTX HLS default | Only if enabled | [F-44] |
| 9997 | Not stated in the register | MediaMTX API (`api: false` by default) | Only if enabled | [F-44] |
| 9910 | UDP | ATEM control protocol (reverse-engineered), as reported by the OpenSwitcher project [F-11]. The atem-connection and PyATEMMax projects report this port as their default [F-14], [F-19]. | Outbound to the ATEM (reasoning: PACSCORDER would be the client), only for control or tally integration. That integration is not in current scope (REQ-ATEM-001; OQ-009 ANSWERED 2026-10-07; RISK-018). | [F-11], [F-14], [F-19] |

All MediaMTX entries come from a community source (the MediaMTX project's own documentation and configuration file). Conflicts when PACSCORDER both receives RTMP from an ATEM and publishes RTMP onward are part of OQ-075; that case arises only if the owner adds receiving ATEM RTMP to scope.

## 6. Tests

| Test | Verifies | Status |
|---|---|---|
| TEST-STR-001 RTMP publish and playback | REQ-STR-001 | `BLOCKED — HARDWARE REQUIRED` |
| TEST-STR-002 WebRTC browser playback | REQ-STR-002 (also resolves OQ-073) | `BLOCKED — HARDWARE REQUIRED` |
| TEST-ENC-001 Sustained real-time H.264 / H.265 encode | Encoder capacity behind both streams | `BLOCKED — HARDWARE REQUIRED` |
| TEST-PERF-001 Soak: thermal, CPU, CMA, frame drops | Concurrent recording + RTMP + WebRTC | `BLOCKED — HARDWARE REQUIRED` |
| TEST-AUD-001 HDMI audio capture over I2S (added 2026-10-08) | REQ-CAP-006, the audio source for both streams | `BLOCKED — HARDWARE REQUIRED` |

*(Added 2026-10-08; scope notes from the registers, no test ID or status changed.)* With H.265 and audio required and CM4 and CM5 evaluated side by side (ADR-004):

- **TEST-ENC-001** includes H.265 runs on CM4 and CM5, with and without a concurrent H.264 encode and audio (OQ-104, OQ-105; RISK-022).
- **TEST-STR-001** includes H.265 publishing to each required destination (OQ-106) if RTMP carries H.265 (OQ-103), and AAC audio.
- **TEST-STR-002**:
  - also runs in Edge and on each target viewer device if H.265 is offered (OQ-108);
  - checks Opus audio.
- **TEST-AUD-001** includes:
  - sources at 44.1 kHz and 48 kHz, and a rate change during capture (OQ-111, RISK-023);
  - an A/V offset measurement (OQ-112, RISK-024);
  - runs on both CM4 and CM5 (OQ-054).
- **TEST-PERF-001** includes a multi-hour A/V drift check (RISK-024).

Procedures will be written in [TESTING.md](TESTING.md). Every command in this document is NOT YET RUN ON PACSCORDER HARDWARE.

## Verification status

### Verified from sources (fact IDs)

This document cites 120 register entries, all with verdict `CONFIRMED` or `CORRECTED`:

D-10, D-11, D-12, D-14, D-15, D-24, D-31, D-32, D-35, D-36, D-37, D-38, D-39, D-40, D-41, D-42, D-43, D-44, D-45, D-46, D-47, D-54, E-31, E-34, E-35, E-36, F-06, F-11, F-14, F-19, F-26, F-28, F-29, F-31, F-32, F-33, F-34, F-35, F-36, F-37, F-38, F-39, F-40, F-41, F-42, F-43, F-44, F-45, F-46, G-22, G-26, G-27, G-28, G-29, G-30, G-31, G-32, G-64, G-65, H-01, H-02, H-04, H-05, H-07, H-08, H-09, H-10, H-11, H-12, H-13, H-14, H-15, H-16, H-19, H-20, H-21, H-22, H-23, H-24, H-25, H-26, H-27, H-28, H-29, H-30, H-31, H-32, H-33, H-34, H-35, H-36, H-43, I-03, I-05, I-06, I-07, I-08, I-10, I-13, I-15, I-18, I-20, I-21, I-22, I-24, I-26, I-33, I-35, I-36, I-37, I-38, I-39, I-40, I-41, I-42, I-43, I-44, I-45, I-46, I-47.

- `CORRECTED` entries, used in their corrected wording only: D-46, F-28, F-34, F-37, H-10, H-12, I-40.
- `community` entries, worded as reports: D-54, F-11, F-14, F-19, F-44, F-45, H-19, H-20, H-21, H-22, H-36.
- `reasoning` entries, labelled as reasoning: D-36, F-35, F-38, F-40, F-46, H-23, H-43, I-18.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). *(Still nothing as of 2026-10-08: no hardware, no code, no test run.)*

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against the source register. The `eflvmux` reasoning now cites the plugins-good package [G-26] instead of plugins-bad [G-27]. Package rows now separate Debian's file lists from the Raspberry Pi builds [G-27]. Corrected the x264 package wording [G-30]. The MediaMTX `:8189` port is no longer called an ICE port, which [F-44] does not state. Community wording added for the port 9910 sources and the B-frame reasoning. Added a network-interface UNKNOWN row (OQ-018). Normalised the UNKNOWN markers to the README convention. Labelled the pipeline sketch and the platform consequences as reasoning. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: MediaMTX `:8189` wording (Section 4.4 and the Section 5 port table) now says [F-44] gives only `webrtcLocalUDPAddress :8189` and that any ICE/RTP role is reasoning from the setting name; network-interface row now links the board Ethernet facts to OQ-098; WebRTC audio wording checked against [F-41] (AAC is not a *required* WebRTC codec; no change needed) | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: receiving ATEM RTMP marked not in current ATEM scope (REQ-ATEM-001, OQ-009 ANSWERED) in §1, §3.5 (kept as reference; OQ-080 relevant only if added) and the §5 port table (1935 inbound, 1935 Streaming Bridge forwarding, 9910 control/tally; RISK-018 linked); REQ-STR-001 / REQ-STR-002 output requirements stated unchanged; header Applies-to row updated. No citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); header "Applies to" row: Raspberry Pi OS (ADR-003, `PROPOSED`) → (ADR-003, `ACCEPTED`); intro list item "the candidate OS builds" → "Raspberry Pi OS (ADR-003, `ACCEPTED`) and in the documented Buildroot alternative". No evidence, other ADR status (ADR-005 `PROPOSED`, ADR-007 `OPEN`) or implementation status changed. | Claude (session 2026-10-07) |
| 2026-10-08 | Owner decisions of 2026-10-07 (second set: H.264 + H.265, REQ-ENC-001 / OQ-103; HDMI audio required, REQ-CAP-006 / OQ-004 ANSWERED; CM4 and CM5 side by side, ADR-004 OPEN; any HDMI camera plus ATEM, OQ-102 ANSWERED) and research topics H and I propagated. Header rows and an owner-decision note added. §1: "audio" in the RTMP row and the SRT and audio-encoder rows marked superseded in part; rows added for codec per output (OQ-103), H.265 capacity (OQ-104/105), HEVC RTMP muxing (OQ-107) and destinations (OQ-106), WebRTC H.265 (OQ-108), sample rate (OQ-111), A/V sync (OQ-112), HEVC/AAC licensing (OQ-109/113). §2: H.265-per-platform table (x265 4.1, `libx265`, `x265enc`, planar-only input, DotProd only on CM5, community cost evidence, 6.6x reasoning) [H-01]–[H-23], [H-43]; new §2.1 audio path and per-output audio-encoder table (AAC for RTMP, Opus for WebRTC, rate limits, `fdk-aac` excluded, two audio encodes) [I-03]–[I-47]. §3.1 "anyway" reasoning marked superseded; Enhanced RTMP `hvc1` [H-24]. §3.2: `flvmux` has no H.265 [H-27]; H.265 encoders planar-only; audio pipeline line updated ("if required" superseded). §3.3: FFmpeg HEVC-in-FLV [H-25], [H-26], `libx265` and thread note [H-08]–[H-10], [H-16], audio encoders [I-39], SRT, ALSA/V4L2 timestamps [I-38]. §3.4: rows for x265, `x265enc`, `libx265`, `rtph265pay`, SRT/MPEG-TS, `alsasrc`, AAC and Opus encoders. New §3.6 (HEVC over RTMP and SRT: framework table, YouTube [H-29], MediaMTX [H-28], SRT [H-30], live encoder settings [H-14]–[H-16], proposed shapes, capacity). §4.1 Opus encoders [I-41], [I-45], [I-47]. §4.4 SRT and audio-encoder gaps superseded in part; three gap rows added. New §4.5 H.265 in WebRTC [H-31]–[H-36]. §6: TEST-AUD-001 row and H.265/audio scope notes. Verification status: 59 → 120 entries. No requirement, decision, risk or test status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (owner chose H.264 + H.265 on 2026-10-07); ID unchanged. | Claude (session 2026-10-08) |
