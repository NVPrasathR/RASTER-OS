# PACSCORDER Streaming (RTMP and WebRTC)

| | |
|---|---|
| Document status | Active — source research only. Streaming design and implementation: NOT STARTED |
| Last updated | 2026-10-07 |
| Applies to | REQ-STR-001 (RTMP), REQ-STR-002 (WebRTC); REQ-ATEM-001 (scope notes only: receiving ATEM RTMP and ATEM network control are not in current scope, owner 2026-10-07); ADR-007 (`OPEN`); RISK-003, RISK-019; all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5); Raspberry Pi OS (ADR-003, `ACCEPTED`) and Buildroot (documented alternative) |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)). Nothing has been tested. No PACSCORDER hardware or code exists as of 2026-10-06. |

This document answers the Rule 25 question "How is streaming performed?". As of 2026-10-06, no streaming design has been chosen. The userspace framework is ADR-007 (`OPEN`). It records:

- the protocol and codec constraints that any design must meet;
- the software components available in Raspberry Pi OS (ADR-003, `ACCEPTED`) and in the documented Buildroot alternative;
- the network ports involved;
- the gaps that remain open.

Fact IDs such as `[F-31]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Facts of tier `community` are worded as reports. Calculations are marked as reasoning and list their inputs. Pipeline sketches are design proposals, not tested commands.

## 1. Status at a glance

| Item | Status |
|---|---|
| REQ-STR-001 RTMP streaming | Acceptance `DRAFT`. Implementation `NOT STARTED`. TEST-STR-001: `BLOCKED — HARDWARE REQUIRED`. |
| REQ-STR-002 WebRTC streaming | Acceptance `DRAFT`. Implementation `NOT STARTED`. TEST-STR-002: `BLOCKED — HARDWARE REQUIRED`. |
| RTMP destinations, RTMPS, bitrate, audio | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-007). |
| WebRTC reach (LAN or internet), browsers, viewer count, latency | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-008). |
| H.264 level signalling for 1080p in browsers | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-073). |
| WebRTC signalling and NAT traversal | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-074). |
| RTMP server on PACSCORDER; ports to open | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-075). Receiving an ATEM's RTMP stream is not in current ATEM scope (REQ-ATEM-001, owner 2026-10-07), so the ATEM integration does not need an RTMP server unless the owner adds it ([§3.5](#35-receiving-rtmp-pacscorder-as-a-server)). |
| SRT | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-076). |
| Audio encoders (AAC for RTMP, Opus for WebRTC) | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063). |
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

## 3. RTMP (REQ-STR-001)

### 3.1 Protocol and codec constraints

- **Codecs in legacy FLV/RTMP.** Video CodecID 7 = AVC (H.264) is the only modern video codec. Audio SoundFormat 10 = AAC. HEVC, AV1, VP9, VP8 and Opus need Enhanced RTMP (E-RTMP) FourCC signalling [F-31].
- **Reasoning:** H.264 video with AAC audio can be carried without Enhanced RTMP; HEVC cannot [F-31]. No candidate platform can encode HEVC in hardware anyway [D-24], [D-31].
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
```

### 3.3 Publishing with FFmpeg

- **Documented publish example** (NOT YET RUN ON PACSCORDER HARDWARE) [F-32]:

  ```bash
  ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream
  ```

- **Options.** The `listen` option makes FFmpeg act as an RTMP server. `rtmp_enhanced_codecs` advertises E-RTMP FourCCs such as `hvc1,av01,vp09` [F-32].
- **Encoders.**
  - `h264_v4l2m2m` is the V4L2 mem2mem hardware wrapper.
  - `libx264` requires FFmpeg built with `--enable-gpl` [D-42].
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
| `flvmux` | `libgstflv` in `gstreamer1.0-plugins-good` [G-26] | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV` [E-34], [F-34] |
| `eflvmux` | Not available (1.26.2; reasoning from [F-34], [G-26]) | Not available (1.24.13) [F-34] |
| `v4l2h264enc` (Pi 4/CM4) | `libgstvideo4linux2.so`, probed elements [G-26] | Needs `..._PLUGIN_V4L2_PROBE=y` [D-39], [G-65] |
| `x264enc` (Pi 5/CM5) | Debian trixie has `x264` `2:0.164.3108+git31e19f9-2+b1`; the Raspberry Pi archive has no `x264` or `libx264-164` package [G-30]. The package that provides the GStreamer `x264enc` plugin on Raspberry Pi OS: NEEDS VERIFICATION. | `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264`, which selects x264 [D-47], [E-36] |
| FFmpeg | 7.1.5 `+rpt2` [G-29] | 6.1.5 [E-36], [G-64] |
| MediaMTX | Debian / Raspberry Pi OS package: NEEDS VERIFICATION | No `package/mediamtx` in Buildroot master, as reported in [F-44] (community tier) |

### 3.5 Receiving RTMP (PACSCORDER as a server)

> **Not in current ATEM scope (owner decision of 2026-10-07; OQ-009 ANSWERED).** REQ-ATEM-001 now covers HDMI capture of the ATEM output only (REQ-CAP-008). Receiving the ATEM's RTMP stream was offered and not selected, so it is out of scope unless the owner adds it. This section is kept as reference only. The RTMP and WebRTC *output* requirements, REQ-STR-001 and REQ-STR-002, are unchanged.

- **A server would be required.** Reasoning [F-46]: an ATEM streaming over RTMP is an RTMP *client* that publishes to a configured URL and key. To receive it, PACSCORDER must run a listening RTMP server, such as MediaMTX on :1935 or FFmpeg in `listen` mode [F-32].
- **`rtmp2src` cannot do this.** It only connects to an RTMP server as a client and cannot accept incoming publish connections [F-33].
- **MediaMTX codecs.** The MediaMTX project reports that it accepts RTMP publishers using AV1, VP9, H265 or H264 video (Enhanced RTMP), and Opus, FLAC, AAC, MP3, AC-3, G711 or LPCM audio [F-45].
- **Open questions.** Whether PACSCORDER hosts an RTMP server at all is OQ-075. Whether an ATEM will publish to a LAN-only PACSCORDER URL is OQ-080, which matters only if receiving ATEM RTMP is added to scope. See [ATEM.md](ATEM.md).

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
| SRT | ATEM streams over SRT [F-06], [F-26]. The MediaMTX project reports SRT conversion [F-44]. GStreamer and Buildroot SRT support were not researched. | OQ-076 |
| Browser behaviour with a 1080p stream above the negotiated level | Not sourced; needs an interoperability test in Chrome, Firefox and Safari | OQ-073 |
| Audio encoders (AAC, Opus): element choice, licence, CPU cost | Not sourced | OQ-063 |

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
| TEST-ENC-001 Sustained real-time H.264 encode | Encoder capacity behind both streams | `BLOCKED — HARDWARE REQUIRED` |
| TEST-PERF-001 Soak: thermal, CPU, CMA, frame drops | Concurrent recording + RTMP + WebRTC | `BLOCKED — HARDWARE REQUIRED` |

Procedures will be written in [TESTING.md](TESTING.md). Every command in this document is NOT YET RUN ON PACSCORDER HARDWARE.

## Verification status

### Verified from sources (fact IDs)

This document cites 59 register entries, all with verdict `CONFIRMED` or `CORRECTED`:

D-10, D-11, D-12, D-14, D-15, D-24, D-31, D-32, D-35, D-36, D-37, D-38, D-39, D-40, D-41, D-42, D-43, D-44, D-45, D-46, D-47, D-54, E-31, E-34, E-35, E-36, F-06, F-11, F-14, F-19, F-26, F-28, F-29, F-31, F-32, F-33, F-34, F-35, F-36, F-37, F-38, F-39, F-40, F-41, F-42, F-43, F-44, F-45, F-46, G-22, G-26, G-27, G-28, G-29, G-30, G-31, G-32, G-64, G-65.

- `CORRECTED` entries, used in their corrected wording only: D-46, F-28, F-34, F-37.
- `community` entries, worded as reports: D-54, F-11, F-14, F-19, F-44, F-45.
- `reasoning` entries, labelled as reasoning: D-36, F-35, F-38, F-40, F-46.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against the source register. The `eflvmux` reasoning now cites the plugins-good package [G-26] instead of plugins-bad [G-27]. Package rows now separate Debian's file lists from the Raspberry Pi builds [G-27]. Corrected the x264 package wording [G-30]. The MediaMTX `:8189` port is no longer called an ICE port, which [F-44] does not state. Community wording added for the port 9910 sources and the B-frame reasoning. Added a network-interface UNKNOWN row (OQ-018). Normalised the UNKNOWN markers to the README convention. Labelled the pipeline sketch and the platform consequences as reasoning. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: MediaMTX `:8189` wording (Section 4.4 and the Section 5 port table) now says [F-44] gives only `webrtcLocalUDPAddress :8189` and that any ICE/RTP role is reasoning from the setting name; network-interface row now links the board Ethernet facts to OQ-098; WebRTC audio wording checked against [F-41] (AAC is not a *required* WebRTC codec; no change needed) | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: receiving ATEM RTMP marked not in current ATEM scope (REQ-ATEM-001, OQ-009 ANSWERED) in §1, §3.5 (kept as reference; OQ-080 relevant only if added) and the §5 port table (1935 inbound, 1935 Streaming Bridge forwarding, 9910 control/tally; RISK-018 linked); REQ-STR-001 / REQ-STR-002 output requirements stated unchanged; header Applies-to row updated. No citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); header "Applies to" row: Raspberry Pi OS (ADR-003, `PROPOSED`) → (ADR-003, `ACCEPTED`); intro list item "the candidate OS builds" → "Raspberry Pi OS (ADR-003, `ACCEPTED`) and in the documented Buildroot alternative". No evidence, other ADR status (ADR-005 `PROPOSED`, ADR-007 `OPEN`) or implementation status changed. | Claude (session 2026-10-07) |
