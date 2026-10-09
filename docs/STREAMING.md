# PACSCORDER Streaming (RTMP and WebRTC)

| | |
|---|---|
| Document status | Active — source research only. Streaming design and implementation: NOT STARTED |
| Last updated | 2026-10-09 |
| Applies to | *(Added 2026-10-09.)* Live latency (owner, 2026-10-08; OQ-116 ANSWERED): under 1 s camera-to-viewer for WebRTC viewers only (REQ-STR-002); RTMP outputs best-effort, latency set by the receiving platform (REQ-STR-001); bitrate and rate control still open (OQ-005). *(Superseded 2026-10-09: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005); RTMP and WebRTC both carry the 17 Mbit/s live encode — §3.8, §4.7.)* Recording mirrored as fragmented MP4 (ADR-009, `ACCEPTED`), relevant here only as a load that must not stall the live path (OQ-117, RISK-028). OQ-125 to OQ-128; RISK-031 to RISK-034. *(2026-10-09: owner decisions — the < 1 s target is judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running); WebRTC viewers are on the LAN only (OQ-008), so OQ-128 and RISK-033 (internet viewers) are not in current scope and stay OPEN.)* — REQ-STR-001 (RTMP), REQ-STR-002 (WebRTC); REQ-ENC-001 (H.264 only for all outputs: owner, 2026-10-08, "H.264 only for now"; OQ-103 ANSWERED. Two simultaneous H.264 encodes, a recording encode and one live encode shared by RTMP and WebRTC: owner, 2026-10-08, "Separate record + live"; OQ-005, OQ-115, OQ-059); REQ-ENC-002 (H.265, `DEFERRED`: not in current scope); REQ-CAP-006 (HDMI audio required, owner 2026-10-07; DRAFT); REQ-ATEM-001 (scope notes only: receiving ATEM RTMP and ATEM network control are not in current scope, owner 2026-10-07); ADR-007 (`OPEN`); ADR-004 (`OPEN`; bring-up evaluates CM4 and CM5 side by side); RISK-002, RISK-003, RISK-019, RISK-023, RISK-024; RISK-022 and RISK-025 (not in current scope: H.265 deferred); all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5); Raspberry Pi OS (ADR-003, `ACCEPTED`) and Buildroot (documented alternative) |
| Verification | Source research of 2026-10-06, plus research topics H (H.265/HEVC) and I (HDMI audio) of 2026-10-08, plus research topics J (recording storage and power loss) and K (live latency) of 2026-10-08, added here 2026-10-09 ([REFERENCES.md](REFERENCES.md)). Nothing has been tested. No PACSCORDER hardware or code exists as of 2026-10-08. *(Still none as of 2026-10-09.)* |

This document answers the Rule 25 question "How is streaming performed?". As of 2026-10-06, no streaming design has been chosen. The userspace framework is ADR-007 (`OPEN`). It records:

- the protocol and codec constraints that any design must meet;
- the software components available in Raspberry Pi OS (ADR-003, `ACCEPTED`) and in the documented Buildroot alternative;
- the network ports involved;
- the gaps that remain open.

Fact IDs such as `[F-31]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Facts of tier `community` are worded as reports. Calculations are marked as reasoning and list their inputs. Pipeline sketches are design proposals, not tested commands.

> **Owner decisions of 2026-10-07 that affect streaming** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md); added here 2026-10-08):
>
> - **Codecs.** H.264 **and** H.265 (HEVC) for recording and streaming (REQ-ENC-001). Which outputs use which codec is open: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). HEVC over RTMP is in [§3.6](#36-h265-hevc-over-rtmp-and-srt-added-2026-10-08); H.265 in WebRTC is in [§4.5](#45-h265-in-webrtc-added-2026-10-08). *(Superseded 2026-10-08 — see the owner decision of 2026-10-08 below.)*
> - **Audio.** HDMI audio is required in recordings and streams (REQ-CAP-006, DRAFT; OQ-004 ANSWERED). The audio path and the audio encoder for each output are in [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08).
> - **Platforms.** Bring-up evaluates CM4 and CM5 side by side; the product platform (ADR-004) stays `OPEN` until measured.
> - **Sources.** Any HDMI camera (no model list) plus ATEM switcher outputs (OQ-102 ANSWERED; REQ-CAP-008). Reasoning: the audio sample rate and format of the source are therefore not known in advance (RISK-023, OQ-110, OQ-111).
>
> **Owner decision of 2026-10-08 (OQ-103 ANSWERED): "H.264 only for now".** RTMP and WebRTC, like recording, carry H.264 only (REQ-ENC-001). H.265 (HEVC) is deferred: REQ-ENC-002 is `DEFERRED`, not in current scope, with no planned tests. OQ-104 to OQ-109, RISK-022 and RISK-025 stay OPEN but are not in current scope. The H.265 research in this document is kept as the evidence for REQ-ENC-002 and is labelled "deferred — REQ-ENC-002; not in current scope": the H.265 table in §2, the H.265 rows in §1, §2.1 and §3.4, the HEVC notes in §3.1 to §3.3, all of §3.6 and all of §4.5.
>
> **Owner decision of 2026-10-08 (answer to OQ-005): "Separate record + live".** Recorded in [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-001, [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005 and OQ-115, and [RISKS.md](RISKS.md) RISK-002 and RISK-003.
>
> - **Two simultaneous H.264 encodes.** One recording encode ([RECORDING.md](RECORDING.md)), and **one live encode shared by RTMP and WebRTC**. RTMP and WebRTC therefore take the same encoded video; neither has its own encode ([§2](#2-position-in-the-pipeline)).
> - **Still open (OQ-005):** bitrate, rate control and latency. RTMP destinations and their bitrate remain OQ-007. *(Superseded in part 2026-10-09: the owner set the latency target on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers only; RTMP is best-effort (OQ-116 ANSWERED; see the note below). Bitrate and rate control remain OQ-005, which stays OPEN.)* *(Superseded 2026-10-09, later: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005); see the 2026-10-09 note below. OQ-005 stays OPEN for the recording encode's profile, level and B-frames and for a capture-to-file latency target for recordings. RTMP destinations remain OQ-007.)* *(Superseded 2026-10-09, latest: owner decisions — recording encode **High profile, Level 4.2, no B-frames** and a recording **capture-to-file latency target of under 1 s glass-to-disk** (OQ-005, now ANSWERED; which drive and statistic: OQ-130). RTMP destinations remain OQ-007.)*
> - **The live encode must be WebRTC-receivable** (reasoning from sources; [§4.1](#41-codec-requirements-from-the-standards-and-from-libwebrtc), [§4.2](#42-candidate-encoders-against-these-requirements)):
>   - RFC 7742 requires H.264 Constrained Baseline support [F-36].
>   - libwebrtc assumes Constrained Baseline Level 3.1 when `profile-level-id` is absent [F-39], while 1080p needs Level 4.0 or above (reasoning [F-40]; OQ-073, RISK-019).
>   - Browsers do not accept B-frames in WebRTC, as reported by the MediaMTX project [F-45].
>   - The Pi 4/CM4 encoder produces no B-frames [D-14] and offers Constrained Baseline [D-11]. Its SPS/PPS repetition is off by default [D-15], so it must be switched on (reasoning from [F-36]: SPS/PPS in-band).
>   - Reasoning: RTMP shares this encode, so RTMP carries the same Constrained Baseline stream without B-frames.
> - **CM4.** Whether the hardware encoder runs both encodes at once is UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED (OQ-115; RISK-002). Reasoning: two 1080p30 encodes need the macroblock rate of one 1080p60 encode, about 2.0× the 1080p30 specification [D-10], [D-52].
> - **CM5.** Both encodes run in software (OQ-059; RISK-003). Reasoning from [G-22]: that roughly doubles the encode CPU load.
> - **Audio is still two encodes:** AAC for RTMP and recording, Opus for WebRTC ([§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08)).
> - **H.265 stays deferred** (REQ-ENC-002).
>
> **Owner decisions of 2026-10-08 on latency and recording (added 2026-10-09).** Recorded in [REQUIREMENTS.md](REQUIREMENTS.md) REQ-STR-001 and REQ-STR-002, [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005 and OQ-116, and [DECISIONS.md](DECISIONS.md) ADR-009.
>
> - **WebRTC: under 1 s camera-to-viewer** ("Under 1 second"; "WebRTC viewers only"; REQ-STR-002; OQ-116 ANSWERED). No source shows the PACSCORDER path meeting it: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-125; RISK-031). Details: [§4.6](#46-live-latency-the--1-s-webrtc-target-added-2026-10-09).
> - **RTMP: best-effort.** Its latency is set by the receiving platform (REQ-STR-001; OQ-116 ANSWERED). Details: [§3.7](#37-rtmp-latency-best-effort-set-by-the-receiving-platform-added-2026-10-09).
> - **Still open:** bitrate and rate control (OQ-005, OPEN); reach, browsers and viewer count (OQ-008); how the < 1 s target is judged — statistic, number of samples, conditions: OWNER DECISION REQUIRED (OQ-008). *(Superseded in part 2026-10-09: owner decisions — the target is judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running); WebRTC viewers are on the LAN only, and internet viewers are not in current scope (OQ-128, RISK-033). Still open under OQ-008: browsers, viewer count, and the sample count and run length.)* *(Superseded in part 2026-10-09, later: bitrate and rate control decided — see the note below.)* *(Superseded 2026-10-09, latest: browsers, viewer count and the measurement run decided — **Chrome, Safari and Firefox**, up to **5 LAN viewers**, 95th percentile over a **30-minute run at 1 sample/second (~1800 samples)** with the recording running. **OQ-008 is ANSWERED.**)*
> - **Recording (ADR-009, `ACCEPTED`):** fragmented MP4, every recording mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure (owner's words: "Self-powered enclosure") ([RECORDING.md](RECORDING.md)). For streaming, ADR-009's consequence applies: an HDD stall must not stall the live path (OQ-117; RISK-028). See [§2](#2-position-in-the-pipeline).
>
> **Owner decisions of 2026-10-09 on bitrate, rate control and drive failure (added 2026-10-09, later).** Recorded in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005 and OQ-129, [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-001 and REQ-REC-001, and [RISKS.md](RISKS.md) RISK-028.
>
> - **Live encode — shared by RTMP and WebRTC: CBR at 17 Mbit/s** ("CBR 17 Mbit/s"). **Recording encode: 25 Mbit/s, VBR** ([RECORDING.md](RECORDING.md)). The options were based on the CM4 encoder's range of 25 kbit/s to 25 Mbit/s, default 10 Mbit/s, VBR or CBR [D-13], and on YouTube's H.264 1080p60 recommendation of 17 Mbit/s (minimum 6) with CBR [H-29], [K-17]. YouTube is one example destination; the RTMP destinations are still OQ-007. Details: [§3.8](#38-rtmp-bitrate-and-rate-control-added-2026-10-09) and [§4.7](#47-webrtc-bitrate-and-per-viewer-load-added-2026-10-09).
> - No per-mode values were given; whether 17 Mbit/s applies unchanged at, for example, 1080p30 or 720p is not stated (OQ-005). OQ-005 stays OPEN for the recording encode's profile, level and B-frames and for a capture-to-file latency target for recordings.
> - **Consequences to check:** whether 17 Mbit/s fits the maximum bitrate of the H.264 profile and level the live encode signals is not in the register (OQ-073; NEEDS VERIFICATION); two encodes at these rates on the CM4 hardware encoder: OQ-115; CPU load of two software encodes on CM5: OQ-059.
> - **Drive failure (OQ-129, still OPEN):** if one mirrored drive fills, is absent or fails during a recording, recording continues on the drive that still works and the operator is alerted. Still open: file splitting, the alert method (OQ-091), and whether a returning drive is used again. For streaming, see [§2](#2-position-in-the-pipeline). *(Superseded in part later still on 2026-10-09: the owner decided that a recording started with one drive missing runs on the available drive with an operator alert, and that recordings are split into a new file every 30 minutes (OQ-129; [RECORDING.md](RECORDING.md)). OQ-129 stays OPEN only for whether a returning drive is used again; the alert method is OQ-091.)*

## 1. Status at a glance

| Item | Status |
|---|---|
| REQ-STR-001 RTMP streaming | Acceptance `DRAFT`. Implementation `NOT STARTED`. TEST-STR-001: `BLOCKED — HARDWARE REQUIRED`. |
| REQ-STR-002 WebRTC streaming | Acceptance `DRAFT`. Implementation `NOT STARTED`. TEST-STR-002: `BLOCKED — HARDWARE REQUIRED`. |
| RTMP destinations, RTMPS, bitrate, audio | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-007). *(Superseded in part 2026-10-08: "audio" is decided — HDMI audio is required in streams (owner, 2026-10-07; REQ-CAP-006 DRAFT; OQ-004 ANSWERED). Destinations, RTMPS and bitrate remain OQ-007.)* *(Superseded in part 2026-10-09: reasoning from the owner decisions on OQ-005 — RTMP carries the shared live encode, now CBR at 17 Mbit/s, so its video bitrate follows from that decision; the register still lists the RTMP video bitrate under OQ-007. Destinations and RTMPS remain OQ-007. Whether each destination accepts 17 Mbit/s CBR is not in the register beyond YouTube's H.264 1080p60 recommendation [H-29]: VENDOR CONFIRMATION REQUIRED (OQ-007). [§3.8](#38-rtmp-bitrate-and-rate-control-added-2026-10-09).)* |
| Video codec per output (H.264, H.265) | Both codecs are required (owner, 2026-10-07; REQ-ENC-001). Which of RTMP and WebRTC carry H.265, at which resolutions and frame rates: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). RISK-022. *(Superseded 2026-10-08: decided. RTMP and WebRTC carry H.264 only (owner: "H.264 only for now"; OQ-103 ANSWERED; REQ-ENC-001). H.265 is deferred (REQ-ENC-002, `DEFERRED`); RISK-022 is not in current scope.)* |
| Software H.265 encode capacity on CM4 and CM5 (deferred — REQ-ENC-002; not in current scope) | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104, OQ-105). No official figure was found [H-19]; see [§2](#2-position-in-the-pipeline). *(2026-10-08: OQ-104 and OQ-105 stay OPEN, not in current scope, until REQ-ENC-002 is re-activated.)* |
| HEVC over RTMP: muxing path with GStreamer 1.26.2 (deferred — REQ-ENC-002; not in current scope) | GStreamer 1.26.2 `flvmux` cannot carry H.265 [H-27]; FFmpeg 7.1.5 can [H-26]. Path: UNKNOWN — VERIFICATION REQUIRED. BUILD TEST REQUIRED; OWNER DECISION REQUIRED (OQ-107). RISK-025. *(2026-10-08: OQ-107 and RISK-025 are not in current scope.)* |
| HEVC over RTMP: acceptance at each destination (deferred — REQ-ENC-002; not in current scope) | UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-106). *(2026-10-08: OQ-106 is not in current scope.)* |
| H.265 in WebRTC: which viewer browsers and devices receive it (deferred — REQ-ENC-002; not in current scope) | UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-108). RISK-019. *(2026-10-08: OQ-108 is not in current scope. RISK-019 still applies to the H.264 WebRTC track, for example the 1080p level question OQ-073.)* |
| Audio sample-rate detection and output sample rate | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED; BUILD TEST REQUIRED (OQ-111). RISK-023. |
| Audio/video synchronisation and tolerance | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED (OQ-112). RISK-024. |
| HEVC and AAC patent licensing | UNKNOWN — VERIFICATION REQUIRED. LEGAL CLARIFICATION REQUIRED (OQ-109, OQ-113). RISK-015. *(2026-10-08: the HEVC part, OQ-109, is not in current scope because H.265 is deferred (REQ-ENC-002). AAC licensing, OQ-113, is unchanged.)* |
| WebRTC reach (LAN or internet), browsers, viewer count, latency | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-008). *(Superseded in part 2026-10-09: latency is decided — under 1 s camera-to-viewer for WebRTC viewers (owner, 2026-10-08; REQ-STR-002; OQ-116 ANSWERED). Reach, browsers and viewer count remain OQ-008, as does how the < 1 s target is judged — statistic, number of samples, conditions (OWNER DECISION REQUIRED).)* *(Superseded in part 2026-10-09: owner decisions — reach is LAN only, internet viewers not in current scope (OQ-128, RISK-033); the target is judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running). Browsers, viewer count, and the sample count and run length remain OQ-008.)* |
| Video encodes behind the streams (row added 2026-10-08) | Decided by the owner, 2026-10-08 ("Separate record + live", answer to OQ-005): one live H.264 encode shared by RTMP and WebRTC, running beside a separate recording encode. Bitrate, rate control and latency: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-005). Two concurrent encodes on the CM4 hardware encoder: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED (OQ-115; RISK-002). On CM5 both are software encodes: HARDWARE TEST REQUIRED (OQ-059; RISK-003). *(Superseded in part 2026-10-09: the latency target is set — < 1 s camera-to-viewer for WebRTC viewers, RTMP best-effort (OQ-116 ANSWERED). Bitrate and rate control remain OQ-005.)* *(Superseded 2026-10-09, later: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005). OQ-005 stays OPEN for the recording encode's profile, level and B-frames and for a capture-to-file latency target. Level fit OQ-073; CM4 OQ-115; CM5 OQ-059.)* *(Superseded 2026-10-09, latest: owner decision — recording encode **High profile, Level 4.2, no B-frames**; recording **capture-to-file latency target under 1 s glass-to-disk** (which drive and statistic: OQ-130). **OQ-005 is ANSWERED.** This row concerns the live encode; the recording encode is detailed in [RECORDING.md](RECORDING.md) and [VIDEO_ENCODER.md](VIDEO_ENCODER.md).)* |
| Live latency per output (row added 2026-10-09) | **WebRTC:** target under 1 s camera-to-viewer (owner, 2026-10-08; REQ-STR-002; OQ-116 ANSWERED). Whether PACSCORDER meets it: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-125; RISK-031); [§4.6](#46-live-latency-the--1-s-webrtc-target-added-2026-10-09). **RTMP:** best-effort; latency set by the receiving platform (REQ-STR-001; OQ-116 ANSWERED); [§3.7](#37-rtmp-latency-best-effort-set-by-the-receiving-platform-added-2026-10-09). |
| Live-path element latencies and queue policy (row added 2026-10-09) | Several GStreamer elements buffer by default [K-07], [K-08], [K-09]. Settings and queue policy: UNKNOWN — VERIFICATION REQUIRED. BUILD TEST REQUIRED; HARDWARE TEST REQUIRED (OQ-126; RISK-032). |
| WebRTC publishing route (row added 2026-10-09) | Candidate only: RTSP publishing to MediaMTX, which serves WebRTC/WHEP readers (reasoning recorded under ADR-007, `OPEN`; [K-05], [F-45]). `webrtcsink` and `whipclientsink` are not packaged in Debian trixie or the Raspberry Pi archive [K-06]; WHEP is not an RFC as of 2026-10-08, still an Internet-Draft [K-02] (RISK-034). Choice: OWNER DECISION REQUIRED (OQ-074, OQ-015). |
| Live keyframe interval, on-demand keyframes, B-frames (row added 2026-10-09) | UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED (OQ-127; RISK-019). See [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §7.1. |
| Internet WebRTC viewers: ICE, STUN, TURN (row added 2026-10-09) | Needed only if OQ-008 puts internet viewers in scope. UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (OQ-128; RISK-033). *(Not in current scope since 2026-10-09 — LAN-only viewers (owner, 2026-10-09; OQ-008); kept as reference. OQ-128 and RISK-033 stay OPEN.)* |
| Live path isolated from the mirrored recording's HDD branch (row added 2026-10-09) | Required by ADR-009 (Consequences). Design: UNKNOWN — VERIFICATION REQUIRED. BUILD TEST REQUIRED; HARDWARE TEST REQUIRED (OQ-117; RISK-028). |
| H.264 level signalling for 1080p in browsers | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-073). *(2026-10-08: this concerns the live encode, which RTMP shares (OQ-005).)* *(2026-10-09: OQ-073 now also covers whether the live encode's 17 Mbit/s fits the maximum bitrate of the signalled profile and level — not in the register; DATASHEET REQUIRED, the H.264 specification; [§4.7](#47-webrtc-bitrate-and-per-viewer-load-added-2026-10-09).)* |
| WebRTC signalling and NAT traversal | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-074). *(2026-10-09: the NAT-traversal part is not in current scope — LAN-only viewers (owner, 2026-10-09; OQ-008; OQ-128, RISK-033). The signalling choice remains open (OQ-074).)* |
| RTMP server on PACSCORDER; ports to open | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-075). Receiving an ATEM's RTMP stream is not in current ATEM scope (REQ-ATEM-001, owner 2026-10-07), so the ATEM integration does not need an RTMP server unless the owner adds it ([§3.5](#35-receiving-rtmp-pacscorder-as-a-server)). |
| SRT | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-076). *(2026-10-08: the Raspberry Pi OS stacks contain the components for HEVC over SRT in MPEG-TS [H-30]; whether SRT is required is still OQ-076.)* *(2026-10-08, later: HEVC over SRT is deferred — REQ-ENC-002; not in current scope. OQ-076 itself is unchanged.)* |
| Audio encoders (AAC for RTMP, Opus for WebRTC) | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063). *(Superseded in part 2026-10-08: which AAC and Opus encoders are available in Raspberry Pi OS is now sourced [I-39], [I-41], [I-44], [I-45], [I-46]; see [§2.1](#21-audio-path-and-audio-encoder-per-output-added-2026-10-08). The choice and the CPU cost remain UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063).)* |
| PACSCORDER network interface (Ethernet, Wi-Fi) per platform and carrier | UNKNOWN — VERIFICATION REQUIRED. No register fact covers the network interfaces of the candidate boards: DATASHEET REQUIRED (OQ-098). Choice of interface: OWNER DECISION REQUIRED (OQ-018). |
| Userspace framework | ADR-007 `OPEN` (OQ-015). |

## 2. Position in the pipeline

```text
… → V4L2 → DMABUF ─┬→ live H.264 encoder ─┬→ (parser) → FLV mux → RTMP publisher   → RTMP server / service
                   │                       └→ RTP packetiser → WebRTC stack         → browser
                   └→ recording H.264 encoder → recorder (RECORDING.md)
```

The stages after the encoder are a generic sketch (reasoning from [F-33], [F-35] and [F-42]). The actual elements are not chosen: ADR-007 (`OPEN`).

*(Diagram updated 2026-10-08 after the owner answered OQ-005 with "Separate record + live". It read `… → V4L2 → DMABUF → Encoder ─┬→ …`, with one unnamed encoder feeding both streams.)* Reasoning from that decision:

- One capture feeds two H.264 encoders. Each capture buffer therefore has two consumers, the live encode and the recording encode. Zero-copy into the CM4 encoder is OQ-058; two concurrent encode sessions on it are OQ-115.
- One live bitstream feeds two outputs. It is split after the encoder: one branch goes through a parser to the FLV muxer [F-35], the other to the RTP packetiser. How the split is done is ADR-007 (`OPEN`).

*(Added 2026-10-09; ADR-009 ACCEPTED by the owner on 2026-10-08.)* The recorder writes every recording as fragmented MP4 to both an NVMe SSD and a USB-to-SATA HDD ([RECORDING.md](RECORDING.md)). One Seagate BarraCuda 2.5-inch HDD family (an example, not a general figure) takes 2.5 s typical, 3.0 s maximum from standby to ready (CORRECTED) [J-30]. ADR-009 requires that an HDD stall must not stall the NVMe copy or the live path. Reasoning (as in RISK-028): on CM4 the recording encode would share the hardware encoder with the live encode (whether it can run both is OQ-115), and both encodes share the capture, so back-pressure from the HDD branch could reach the live stream; each frame waiting in a V4L2 queue adds one frame period (reasoning part of [K-36]). Buffering design: OQ-117; live-path queues: OQ-126.

*(Added 2026-10-09, later; owner decision on OQ-129.)* If one mirrored drive fills, is absent or fails during a recording, recording continues on the drive that still works and the operator is alerted (OQ-129, still OPEN for file splitting, the alert method — OQ-091 — and drive return). *(Superseded in part later still on 2026-10-09: file splitting is decided — a new file every 30 minutes — and so is a drive missing at the start — start on the available drive with an operator alert (owner; OQ-129). OQ-129 stays OPEN only for drive return; the alert method is OQ-091.)* Reasoning, as recorded in RISK-028: a failed HDD therefore must not end the recording; this does not change the stall concern above, which is about a slow HDD, not a failed one. Reasoning: like a stall, a drive failure must not back-pressure the shared capture or, on CM4, the shared encoder, so the same branch isolation applies (OQ-117).

The encoder differs per platform. That affects both outputs:

| Platform | Encoder (facts) | Consequence for streaming (reasoning) |
|---|---|---|
| Pi 4 Model B, CM4 | Hardware H.264 encoder, officially specified for 1080p30 encode [D-10] | 1080p60 streaming depends on unproven 1080p60 hardware encode (RISK-002, OQ-056). *(2026-10-08: the live encode would run beside a separate recording encode on the same hardware encoder (OQ-005). Reasoning: two 1080p30 encodes need 2 × 244,800 = 489,600 macroblocks/s, the rate of one 1080p60 encode and about 2.0× the 1080p30 specification [D-10], [D-52]. Whether the encoder sustains both: OQ-115, RISK-002.)* |
| Pi 5, CM5 | No hardware video encoder [D-31]. "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22]. | Every stream is a CPU software encode (RISK-003). How many simultaneous encodes fit is OQ-059. *(2026-10-08: two software encodes are required at once, the live encode for both streams and the recording encode (OQ-005). Reasoning from [G-22]: that roughly doubles the encode CPU load. Whether it fits: OQ-059, RISK-003.)* |

No candidate platform has a hardware HEVC encoder [D-24], [D-31]. Whether recording, RTMP and WebRTC share one encode or use separate encodes is OQ-005. *(Superseded 2026-10-08: decided by the owner, "Separate record + live" (answer to OQ-005). RTMP and WebRTC share one live encode; recording has its own. Bitrate, rate control and latency remain OQ-005.)* *(Superseded in part 2026-10-09: latency is decided — < 1 s camera-to-viewer for WebRTC viewers, RTMP best-effort (owner, 2026-10-08; OQ-116 ANSWERED). Bitrate and rate control remain OQ-005.)* *(Superseded 2026-10-09, later: owner decision — live CBR 17 Mbit/s, recording 25 Mbit/s VBR (OQ-005). On Pi 4/CM4 both values lie inside the encoder's 25 kbit/s – 25 Mbit/s range [D-13]; whether the encoder sustains both encodes at these rates is OQ-115. On Pi 5/CM5 the CPU load of two software encodes at these rates is UNKNOWN (OQ-059).)*

**H.265 per platform (added 2026-10-08, when H.265 was required by REQ-ENC-001; deferred — REQ-ENC-002; not in current scope).** Since the owner's answer to OQ-103 on 2026-10-08 ("H.264 only for now"), no stream carries H.265. This table and the notes under it are kept as the evidence for REQ-ENC-002. Bring-up evaluates CM4 and CM5 side by side (ADR-004, `OPEN`). Research topic H covered CM4 and CM5 only. For Pi 4 Model B and Pi 5, only the community benchmarks below name those boards (Pi 5 and a Pi 400); everything else is NEEDS VERIFICATION.

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
- Real-time H.265 throughput on CM4 and CM5: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104; TEST-ENC-001). RISK-022. *(2026-10-08: not in current scope. TEST-ENC-001 is now "Sustained real-time H.264 encode (H.265 deferred)"; its H.265 runs are deferred, not run in current scope, and are needed only if REQ-ENC-002 is re-activated. OQ-104 and RISK-022 stay OPEN.)*

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
| RTMP, enhanced FLV (H.265, FFmpeg 7.1.5; deferred — REQ-ENC-002; not in current scope) | FFmpeg's FLV audio table includes AAC and MP3 and has no Opus [H-26]. YouTube Live lists AAC or MP3 audio [H-29]. | FFmpeg native `aac` [I-39], [I-40] (CORRECTED) | MPEG-4 audio rates [I-40] |
| SRT / MPEG-TS (only if OQ-076 adds SRT) | Audio caps of `mpegtsmux` are not in the register: NEEDS VERIFICATION | As for RTMP (reasoning) | NEEDS VERIFICATION |
| WebRTC | Opus or G.711 required; AAC is not a required WebRTC codec [F-41] | FFmpeg `libopus` [I-39], [I-41]; GStreamer `opusenc` (plugins-base) [I-45]. FFmpeg's native `opus` encoder is experimental and CELT-only [I-41]. | 48, 24, 16, 12 or 8 kHz only, so a 44.1 kHz source needs resampling first [I-41], [I-47] |

- **FFmpeg native `aac`** (CORRECTED wording [I-40]):
  - It is FFmpeg 7.1's default AAC encoder and is not flagged experimental.
  - Without `-b` it defaults to 128 kb/s for stereo.
  - An explicit `-b` selects CBR.
- **`opusenc`** defaults to 64000 bit/s, constrained VBR [I-47].
  - *(Added 2026-10-09; research topic K.)* Opus frames can be 2.5, 5, 10, 20, 40 or 60 ms (RFC 6716). `opusenc` defaults to `frame-size=20` ms and `audio-type=generic`, and also offers a `restricted-lowdelay` audio type [K-40]. Audio-path latency and browser A/V sync are unmeasured (research gap, topic K; OQ-125, OQ-112).
- **`fdk-aac` is not an option.**
  - It is not shipped in the Raspberry Pi FFmpeg or GStreamer builds [I-39], [I-44].
  - Debian ships it in non-free, under a licence that Debian calls incompatible with every GPL version [I-43].
  - A GPL FFmpeg build can enable it only with `--enable-nonfree`, which makes the result unredistributable [I-42].
- **Two audio encodes.** Reasoning from [H-26] and [F-41] (as in OQ-063 and RISK-019): when RTMP and WebRTC with audio run at the same time, two audio encodes are needed, AAC and Opus. *(2026-10-08: unchanged by the owner's answer to OQ-005, which concerns video. Audio is still two encodes: AAC for RTMP and recording, Opus for WebRTC.)*
- **Still open.**
  - CPU cost of each encoder alongside the video encodes on CM4 and CM5: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-063). *(2026-10-08: alongside two video encodes, recording and live (OQ-005).)*
  - AAC patent licensing: LEGAL CLARIFICATION REQUIRED (OQ-113).

## 3. RTMP (REQ-STR-001)

### 3.1 Protocol and codec constraints

- **Codecs in legacy FLV/RTMP.** Video CodecID 7 = AVC (H.264) is the only modern video codec. Audio SoundFormat 10 = AAC. HEVC, AV1, VP9, VP8 and Opus need Enhanced RTMP (E-RTMP) FourCC signalling [F-31].
- **Reasoning:** H.264 video with AAC audio can be carried without Enhanced RTMP; HEVC cannot [F-31]. No candidate platform can encode HEVC in hardware anyway [D-24], [D-31]. *(Superseded 2026-10-08: "anyway" no longer applies. H.265 is required (owner, 2026-10-07; REQ-ENC-001), so if RTMP carries H.265 (OQ-103), it needs Enhanced RTMP and a software encoder; see [§3.6](#36-h265-hevc-over-rtmp-and-srt-added-2026-10-08).)* *(Superseded 2026-10-08, later: OQ-103 is answered — RTMP carries H.264 only (owner: "H.264 only for now"; REQ-ENC-001). Reasoning from [F-31]: H.264 video with AAC audio fits legacy FLV/RTMP, so the current scope needs no Enhanced RTMP. HEVC over RTMP is deferred — REQ-ENC-002; not in current scope.)*
- **Enhanced RTMP and HEVC** (added 2026-10-08; deferred — REQ-ENC-002; not in current scope). The Enhanced RTMP specification (document version v2-2026-01-31-r2) defines the HEVC FourCC `hvc1` and lists it among the `fourCcList` connect-command values [H-24].
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
- *(Added 2026-10-08; matters only for H.265, which is deferred — REQ-ENC-002; not in current scope.)* In GStreamer 1.26.2, its video sink pad has no H.265. It accepts only `video/x-flash-video`, `video/x-flash-screen`, `video/x-vp6-flash`, `video/x-vp6-alpha` and `video/x-h264,stream-format=avc`. The distribution's GStreamer therefore cannot put HEVC into FLV/RTMP [H-27].

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
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope: no output carries H.265, so this CM4 conversion is not needed in the current scope.)* The H.265 encoders accept planar input only: `x265enc` accepts Y444, Y42B and I420 at 8-bit [H-13], and FFmpeg `libx265` accepts planar or gray formats only [H-10] (CORRECTED). Reasoning: because H.265 is software-encoded on every candidate [D-24], [D-31], an H.265 output needs this conversion on CM4 as well as on CM5 [H-43].
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
H.265      :  deferred — REQ-ENC-002; not in current scope (2026-10-08). Not possible with flvmux in GStreamer 1.26.2 [H-27]; see §3.6
```

*(Added 2026-10-08; owner answer to OQ-005, "Separate record + live".)* The encoder in the Pi 4/CM4 and Pi 5/CM5 shapes above is the **live** encode. Reasoning from that decision:

- Its output also feeds the WebRTC branch ([§4](#4-webrtc-req-str-002)), so the bitstream is split after the encoder. The RTMP branch still needs `h264parse` or an equivalent to reach `stream-format=avc` [F-34], [F-35]. The input format of the WebRTC branch's RTP packetiser is not in the register: NEEDS VERIFICATION.
- A separate recording encode runs from the same capture ([RECORDING.md](RECORDING.md)).
- The live encode carries the WebRTC settings of [§4.2](#42-candidate-encoders-against-these-requirements):
  - Constrained Baseline: available on Pi 4/CM4 [D-11]; how to select it in `x264enc` is NEEDS VERIFICATION.
  - SPS/PPS repeated: `repeat_sequence_header=1` [D-15], [D-37].
  - No B-frames: the Pi 4/CM4 encoder produces none [D-14]; browsers do not accept them in WebRTC, as reported by the MediaMTX project [F-45] (community source).
- The splitting element is not chosen: ADR-007 (`OPEN`).
- *(Added 2026-10-09; owner decision on OQ-005.)* The live encode in these shapes runs at **CBR 17 Mbit/s**: on Pi 4/CM4 through `V4L2_CID_MPEG_VIDEO_BITRATE` and `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` [D-13]; on Pi 5/CM5 the `x264enc` property names and units are NEEDS VERIFICATION. In `v4l2h264enc` `extra-controls` form, the name `video_bitrate` is reported in a Raspberry Pi engineer's forum instructions (community source) [D-54]; the name for the bitrate mode is not in the register: NEEDS VERIFICATION ([VIDEO_ENCODER.md](VIDEO_ENCODER.md) §3.6). See [§3.8](#38-rtmp-bitrate-and-rate-control-added-2026-10-09).

### 3.3 Publishing with FFmpeg

- **Documented publish example** (NOT YET RUN ON PACSCORDER HARDWARE) [F-32]:

  ```bash
  ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream
  ```

- **Options.** The `listen` option makes FFmpeg act as an RTMP server. `rtmp_enhanced_codecs` advertises E-RTMP FourCCs such as `hvc1,av01,vp09` [F-32].
  - *(Added 2026-10-08.)* In FFmpeg 7.1.5, `rtmp_enhanced_codecs` writes a `fourCcList` in the connect command but accepts only `hvc1`, `av01` and `vp09`; any other FourCC fails [H-26].
- **HEVC in FLV** (added 2026-10-08; deferred — REQ-ENC-002; not in current scope).
  - FFmpeg 6.1 was the first release to mux HEVC into FLV and signal it over RTMP [H-25].
  - FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP publishing. Its FLV muxer maps HEVC to FourCC `hvc1` [H-26].
  - Its FLV audio table has no Opus [H-26].
- **Encoders.**
  - `h264_v4l2m2m` is the V4L2 mem2mem hardware wrapper.
  - `libx264` requires FFmpeg built with `--enable-gpl` [D-42].
  - *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope: the `libx265` encoder is used only if REQ-ENC-002 is re-activated.)* `libx265`: the Raspberry Pi FFmpeg 7.1.5 is configured with `--enable-libx265` [H-08], and its `libavcodec61` links `libx265-215` [H-09]. Reasoning from [H-09]: `libavcodec61` declares a dependency on `libx265-215`, so installing the Raspberry Pi FFmpeg still pulls in `libx265` even though no output uses H.265.
    - It accepts planar or gray input only, never packed `uyvy422` [H-10] (CORRECTED).
    - The wrapper copies FFmpeg's thread count into x265's frame threads after applying preset and tune [H-10]. `tune=zerolatency` sets one frame thread [H-16]. Reasoning from [H-10] and [H-16]: a `-threads` value therefore replaces zerolatency's single frame thread unless it is set explicitly. Latency and throughput for both settings: HARDWARE TEST REQUIRED; BUILD TEST REQUIRED (OQ-104; not in current scope).
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
| x265 library (H.265; rows added 2026-10-08; deferred — REQ-ENC-002: the x265 rows matter only if REQ-ENC-002 is re-activated) | Debian's x265 4.1-2 (`libx265-215`); the Raspberry Pi archive does not override it [H-01], [H-02]. *(2026-10-08: still installed as a dependency of the Raspberry Pi FFmpeg's `libavcodec61` [H-09], although no output uses H.265.)* | NEEDS VERIFICATION — no register fact |
| `x265enc` (GStreamer; only if REQ-ENC-002 is re-activated) | `libgstx265.so` in Debian's `gstreamer1.0-plugins-bad` 1.26.2-3+deb13u3 [H-11]; the Raspberry Pi build `1.26.2-3+rpt4+deb13u3` still installs it [H-12] (CORRECTED) | NEEDS VERIFICATION — no register fact |
| FFmpeg `libx265` (encoder used only if REQ-ENC-002 is re-activated) | `--enable-libx265` in the Raspberry Pi FFmpeg; `libavcodec61` depends on `libx265-215` [H-08], [H-09], so `libx265` is pulled in with FFmpeg regardless (reasoning from [H-09]) | NEEDS VERIFICATION — no register fact |
| `rtph265pay` (WebRTC H.265; only if REQ-ENC-002 is re-activated) | In GStreamer 1.26.2; profile-id, tier-flag and level-id in its output caps arrived only in 1.26.4 [H-31] | NEEDS VERIFICATION — no register fact |
| SRT and MPEG-TS (HEVC over SRT; the HEVC use is deferred — REQ-ENC-002; SRT itself is OQ-076) | Debian trixie's `gstreamer1.0-plugins-bad` ships `libgstsrt.so` and `libgstmpegtsmux.so`; 1.26.2 `mpegtsmux` accepts H.265; the Raspberry Pi FFmpeg has `--enable-libsrt` [H-30]. Whether the Raspberry Pi build of plugins-bad contains the same files: NEEDS VERIFICATION. | NEEDS VERIFICATION — no register fact |
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

> **Deferred — REQ-ENC-002; not in current scope (2026-10-08).** The owner answered OQ-103 with "H.264 only for now": RTMP carries H.264 only (REQ-ENC-001), and H.265 is deferred (REQ-ENC-002, `DEFERRED`). This whole section is kept as the evidence for REQ-ENC-002. Its H.265 open questions (OQ-104, OQ-105, OQ-106, OQ-107) and RISK-022 and RISK-025 stay OPEN, not in current scope. No H.265 RTMP or SRT run is planned.

H.264 and H.265 are both required (owner, 2026-10-07; REQ-ENC-001). Whether RTMP carries H.265, H.264 or both is open: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-103). *(Superseded 2026-10-08 by the note above.)* This section records what the sources say about each way of carrying H.265.

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
  - 1080p60 bitrate: 12 Mbps recommended (4 Mbps minimum) for H.265 and AV1, against 17 Mbps (6 Mbps minimum) for H.264. *(2026-10-09: for H.264 the owner has since chosen CBR at 17 Mbit/s for the live encode, an option based on this figure (OQ-005; [§3.8](#38-rtmp-bitrate-and-rate-control-added-2026-10-09)). The H.265 figures stay deferred — REQ-ENC-002.)*
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

**Capacity.** Running an H.265 RTMP encode beside an H.264 encode (for WebRTC, [§4.5](#45-h265-in-webrtc-added-2026-10-08)) is two concurrent video encodes. On CM5 both are software encodes [D-31]; on CM4 the H.265 one is (reasoning from [D-10], [D-24]). Whether this fits: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-104, OQ-059; TEST-ENC-001, TEST-PERF-001). RISK-022. *(2026-10-08: not in current scope. These H.265 runs are deferred, not run in current scope; they would be added to TEST-ENC-001 and TEST-PERF-001 only if REQ-ENC-002 is re-activated. OQ-059 still applies to the H.264 encodes.)*

### 3.7 RTMP latency: best-effort, set by the receiving platform (added 2026-10-09)

**Owner decision of 2026-10-08 (OQ-116 ANSWERED; REQ-STR-001).** The < 1 s target applies to WebRTC viewers only. RTMP outputs are best-effort, and their latency is set by the receiving platform. Reasoning: the < 1 s target is therefore not a TEST-STR-001 criterion.

**YouTube Live** (the only RTMP platform research topic K checked):

| Item | Fact | Source |
|---|---|---|
| Latency modes | Normal: no stated figure; supports all resolutions and features. Low: "Most viewers … will experience latency less than 10 seconds"; no 4K. Ultra-low: "Most viewers … will experience latency less than 5 seconds"; no 4K; more viewer buffering. | [K-15] |
| API setting | The Live Streaming API's `contentDetails.latencyPreference` accepts `normal`, `low` and `ultraLow`. `ultraLow` supports neither closed captions nor resolutions above 1080p. | [K-16] |
| No sub-second mode | Reasoning (CORRECTED): YouTube documents no sub-second mode. Ultra-low's "under 5 s for most viewers" is an upper bound for most viewers, not a minimum. YouTube names the player's read-ahead buffer as the main source of stream latency. The RTMP-to-YouTube output therefore cannot be planned or claimed to meet the < 1 s target, and PACSCORDER's encoder settings cannot remove the player-side buffer. | [K-18] |
| Encoder settings | 2 s keyframe frequency recommended ("Do not exceed 4 seconds"); CBR. Under advanced settings: "2 B-Frames", "1 Reference Frame", "CABAC". The B-frame recommendation conflicts with the B-frame-free stream WebRTC requires (MediaMTX documents that browsers do not support H.264 B-frames over WebRTC [K-04]). | [K-17], [K-04] |

Consequences (reasoning):

- Reasoning from [K-15] and [K-16]: the latency mode is chosen on the YouTube side, in the broadcast's settings or through the API. It is not a property of the RTMP stream PACSCORDER sends.
- RTMP shares the live encode with WebRTC (OQ-005), so YouTube receives the B-frame-free stream that WebRTC needs (as MediaMTX documents [K-04]), not the 2 B-frames it recommends [K-17]. Whether YouTube and other destinations accept a B-frame-free Baseline or Constrained Baseline stream with acceptable quality is not stated by YouTube, which only "recommends" its settings (research open question, topic K): UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED (OQ-127; RISK-019).
- The keyframe interval of the shared live encode has to fit both YouTube's 2 s recommendation [K-17] and WebRTC viewer join time ([§4.6](#46-live-latency-the--1-s-webrtc-target-added-2026-10-09); OQ-127).
- Other RTMP platforms (for example Twitch, Facebook) were not checked for documented latency modes (research gap, topic K). The destinations themselves are OQ-007.
- *(Added 2026-10-09, later.)* The live encode is now CBR at 17 Mbit/s (owner, OQ-005), which follows YouTube's CBR recommendation [K-17] and its H.264 1080p60 bitrate [H-29]. The B-frame conflict above is unchanged. See [§3.8](#38-rtmp-bitrate-and-rate-control-added-2026-10-09).

### 3.8 RTMP bitrate and rate control (added 2026-10-09)

**Owner decision of 2026-10-09 (OQ-005).** The live encode, which RTMP shares with WebRTC, is **CBR at 17 Mbit/s** ("CBR 17 Mbit/s"). The recording encode (25 Mbit/s VBR) does not feed RTMP ([§2](#2-position-in-the-pipeline)). Nothing below has been implemented or run on PACSCORDER hardware.

| Item | Fact or decision | Source |
|---|---|---|
| Basis of the option | YouTube Live recommends 17 Mbps (6 Mbps minimum) for H.264 at 1080p60, and CBR | [H-29], [K-17] |
| Pi 4 Model B / CM4 encoder | Bitrate 25 kbit/s – 25 Mbit/s, step 25 kbit/s; VBR (default) or CBR, through `V4L2_CID_MPEG_VIDEO_BITRATE` and `V4L2_CID_MPEG_VIDEO_BITRATE_MODE` | [D-13] |
| Pi 5 / CM5 encoder | `x264enc` bitrate and rate-control property names and units are not in the register: NEEDS VERIFICATION. BUILD TEST REQUIRED | — |
| Other modes | The register holds YouTube's 1080p60 figures only, and the owner gave no per-mode values: whether 17 Mbit/s applies unchanged at 1080p30 or 720p is not stated (OQ-005) | [H-29] |

Consequences:

- **YouTube is an example, not a decided destination.** The RTMP destinations, RTMPS and audio are still OQ-007. Whether each destination accepts 17 Mbit/s CBR from a B-frame-free Constrained Baseline stream is not in the register: VENDOR CONFIRMATION REQUIRED (OQ-007, OQ-127; RISK-019). Other ingest services' bitrate limits were not researched.
- **Upstream load (reasoning).** Each RTMP destination receives the one live encode, so each takes about 17 Mbit/s of video plus the AAC audio and protocol overhead (neither budgeted here). N destinations ≈ N × 17 Mbit/s of video upstream, if each is published separately; the number of destinations is OQ-007. Ethernet on the candidate boards is not researched (OQ-098), and the uplink capacity is UNKNOWN, as in [PERFORMANCE.md](PERFORMANCE.md) §4.3.
- **Level.** Whether 17 Mbit/s fits the maximum bitrate of the H.264 profile and level the live encode signals is not in the register — [F-40] covers frame size and macroblock rate only: NEEDS VERIFICATION. DATASHEET REQUIRED (the H.264 specification; OQ-073).
- **Encoder load.** On CM4, both encodes at 17 and 25 Mbit/s on the one hardware encoder: OQ-115 (RISK-002). On CM5, two software encodes at these rates: OQ-059 (RISK-003).
- **TEST-STR-001** runs with the live encode at 17 Mbit/s CBR (scope note, not an accepted criterion; [§6](#6-tests)).

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

*(Added 2026-10-09; research topic K.)* The same statement is in the register from MediaMTX's current documentation, at tier `vendor-other`: browsers deliberately do not support H.264 with B-frames over WebRTC; for broad browser compatibility MediaMTX recommends H.264 Baseline profile (no B-frames) with Opus audio; its WebRTC audio codecs are Opus, G722 and G711 only, and AAC is not listed [K-04]. If GStreamer publishes to MediaMTX over WebRTC (`whipclientsink`), MediaMTX requires GStreamer 1.22 or later and, for H.264, the Baseline profile [K-05].

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
| No B-frames | Produces no B-frames [D-14]. FFmpeg `h264_v4l2m2m` forces B-frames to 0 [D-44]. *(Added 2026-10-09.)* Re-confirmed in `rpi-6.18.y`: B-frames limited to min 0, max 0; defaults profile High, level 4.0, GOP 60 [K-30]. | `rpicam-apps` normal-mode `libx264` settings use `max_b_frames=1`; its `--low-latency` mode uses `tune zerolatency` [D-35] and drops B-frames [D-32]. Reasoning, from the MediaMTX project's report that browsers do not support H.264 B-frames [F-45]: B-frames must be disabled for browser WebRTC. *(Added 2026-10-09.)* GStreamer 1.26 `x264enc` runs x264's medium preset by default (3 B-frames), so its `bframes=0` property default does not guarantee a B-frame-free stream; set `tune=zerolatency`, set `bframes=0` explicitly, or force `profile=baseline` in caps (CORRECTED) [K-29]. `tune=zerolatency` sets B-frames to 0 [K-27]. See [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §7.1. |
| SPS/PPS in-band | `REPEAT_SEQ_HEADER` defaults to off [D-15]. The official pipeline sets `repeat_sequence_header=1` [D-37]. | NEEDS VERIFICATION |
| Level that covers 1080p | Level menu 1.0–5.1, default 4.0. The driver states the hardware spec is Level 4.0 and higher levels "may not be able to keep up with real-time" [D-12]. Reasoning: 1080p30 fits Level 4; 1080p60 needs Level 4.2 [F-40], beyond the hardware spec (RISK-002). | `rpicam-apps` forces Level 4.2 when the MB rate exceeds 245,760 MB/s, i.e. at 1080p60 [D-36] (reasoning). |

As reported in a Raspberry Pi engineer's 2020 TC358743 instructions (community source), the GStreamer example sets `h264_profile=4` (High) and `h264_level=10` (Level 3.2) [D-54]. Reasoning: High is not Constrained Baseline [F-36], so those settings are not suitable as-is for browser WebRTC.

*(Added 2026-10-08; owner answer to OQ-005, "Separate record + live".)* Reasoning from that decision and the table above:

- **The live encode carries these requirements.** RTMP shares it, so RTMP also receives Constrained Baseline H.264 without B-frames, at the level chosen for WebRTC (OQ-073).
- **RTMP acceptance is not known.** Legacy FLV/RTMP carries H.264 [F-31]. Whether each RTMP destination accepts that profile and level is not in the register: UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED (OQ-007).
- **The recording encode is not bound by them.** Its profile, level, bitrate and rate control are UNDEFINED (OQ-005). *(Superseded in part 2026-10-09: owner decision — the recording encode is 25 Mbit/s VBR, the live encode CBR 17 Mbit/s (OQ-005). The recording encode's profile and level were not part of that decision and remain UNDEFINED (OQ-005).)*
- **Platforms.** On Pi 4/CM4 both encodes would use the same hardware encoder [D-06]; whether it runs two at once is OQ-115 (RISK-002). On Pi 5/CM5 both are software encodes (OQ-059, RISK-003).

### 4.3 Implementation options

| Option | Facts | Gaps |
|---|---|---|
| **`webrtcbin`** (GStreamer) | Plugin `webrtc` in gst-plugins-bad, licence "LGPL". It implements most of the W3C RTCPeerConnection API. It takes and produces `application/x-rtp`. It has **no built-in signalling**. It needs libnice to build [F-42]. **Raspberry Pi OS:** Debian's `gstreamer1.0-plugins-bad` `1.26.2-3+deb13u3` ships `libgstwebrtc`, `libgstwebrtcdsp`, `libgstsrtp`, `libgstdtls` and `libgstsctp`; Raspberry Pi OS installs the Raspberry Pi build `1.26.2-3+rpt4+deb13u3` instead [G-27], whose file list NEEDS VERIFICATION. `gstreamer1.0-nice` (0.1.22) provides the `nicesrc`/`nicesink` elements that `webrtcbin` needs at runtime [G-28]. **Buildroot:** `BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_WEBRTC` depends on `!BR2_STATIC_LIBS`. It selects plugins-base, libnice, and the DTLS (OpenSSL), SCTP and SRTP (libsrtp) plugins. libnice builds its GStreamer elements only when plugins-base is enabled [E-35], [F-42]. | Signalling and ICE design (OQ-074) |
| **`webrtcsink`** (gst-plugins-rs, plugin `rswebrtc`) | MPL-2.0. It includes a simple signalling server. It encodes internally (VP8, H.264, VP9, H.265, AV1; Opus audio). It offers Google Congestion Control. Buildroot master has no gst1-plugins-rs package [F-43]. *(Added 2026-10-09.)* Its documentation gives no latency figure. Documented defaults: `congestion-control=gcc`, `do-fec=true`, `do-retransmission=true`, `enable-mitigation-modes=downsampled+downscaled`, `min-bitrate=1000`, `start-bitrate=2048000`, `max-bitrate=8192000` bps [K-13]. | Raspberry Pi OS / Debian package: NEEDS VERIFICATION *(Superseded 2026-10-09: no Debian trixie package contains `libgstrswebrtc.so`, which provides `webrtcsink` and `whipclientsink`; no package named `gstreamer1.0-plugins-rs` exists in any Debian suite; the Raspberry Pi archive (trixie main arm64) has no gst-plugins-rs package as of 2026-10-08 [K-06]. Using it needs a self-built plugin matching GStreamer 1.26.2 (research gap, topic K; RISK-034). The Raspberry Pi archive part of [K-06] rests on the register verifier's check (REFERENCES.md, Register notes, 2026-10-09); whether the plugin is available on the PACSCORDER image: BUILD TEST REQUIRED (research open question, topic K).)* |
| **MediaMTX** (community-reported) | As reported by the MediaMTX project: MIT-licensed, zero-dependency single executable for Linux, Windows and macOS. It converts between RTMP, WebRTC, SRT, RTSP and HLS. Buildroot master has no `package/mediamtx` [F-44] (community-tier entry). WebRTC readers get AV1, VP9, VP8, H265 or H264 video, and only Opus, G722 or G711 audio. Access is through a browser page at `:8889/<path>` or WHEP at `/<path>/whep` [F-45]. | Raspberry Pi OS package: NEEDS VERIFICATION. Reasoning: an RTMP ingest with AAC audio would need audio transcoding for WebRTC readers [F-41], [F-45]. |

Choosing among these is part of ADR-007 (`OPEN`) and OQ-074. Licences: `webrtcbin` LGPL [F-42], `webrtcsink` MPL-2.0 [F-43], MediaMTX MIT (as reported [F-44]). x264 is GPL [D-47]. See OQ-087 and RISK-015.

*(Added 2026-10-09; research topic K.)* MediaMTX documents RTSP-client publishing as the recommended way for GStreamer to publish to it [K-05]. Reasoning, as recorded under ADR-007 (`OPEN`) from [F-45], [K-05] and [K-06]: with the packages in Debian trixie and the Raspberry Pi archive, the GStreamer route to browser viewers is `rtspclientsink` publishing over RTSP to MediaMTX, which serves WebRTC readers, including over WHEP. *(Verifier note, 2026-10-09: read this as "a GStreamer route that needs no gst-plugins-rs", not "the only route" — `webrtcbin` with PACSCORDER's own signalling remains an option in the table above [F-42], [G-27]. Which Raspberry Pi OS package provides `rtspclientsink` is not in the register: NEEDS VERIFICATION. BUILD TEST REQUIRED.)* This is a **candidate, not a decision**; the latency it adds is in [§4.6](#46-live-latency-the--1-s-webrtc-target-added-2026-10-09).

### 4.4 Gaps the sources do not cover

| Topic | State | OQ |
|---|---|---|
| WHIP (ingest) and WHEP (egress) standards | Not sourced. The MediaMTX project reports a WHEP endpoint [F-45]. *(Superseded 2026-10-09: now sourced. WHIP is RFC 9725 and covers ingest only [K-01]; WHEP is not an RFC as of 2026-10-08 — draft-ietf-wish-whep-04, "Waiting for WG Chair Go-Ahead" [K-02] (RISK-034). See [§4.6](#46-live-latency-the--1-s-webrtc-target-added-2026-10-09).)* | OQ-074 |
| ICE, STUN and TURN for viewers behind NAT | Not sourced. The MediaMTX project reports a `webrtcLocalUDPAddress` default of `:8189` [F-44]; [F-44] gives only the setting name and port. Treating it as an ICE or RTP media port would be reasoning from the setting name, not a sourced fact (NEEDS VERIFICATION). *(Superseded in part 2026-10-09: MediaMTX v1.21.1's configuration describes `webrtcLocalUDPAddress` `:8189` as a "UDP/ICE listener" and leaves the TCP/ICE listener disabled [K-41]; its documentation covers advertised addresses, STUN and TURN [K-42] and four connection methods [K-43]. See [§4.6](#46-live-latency-the--1-s-webrtc-target-added-2026-10-09). Latency and stability over STUN or TURN are untested (research open question, topic K; OQ-128, RISK-033).)* *(Not in current scope since 2026-10-09 — LAN-only viewers (owner, 2026-10-09; OQ-008); kept as reference.)* | OQ-074, OQ-008; OQ-128 (added 2026-10-09) |
| SRT | ATEM streams over SRT [F-06], [F-26]. The MediaMTX project reports SRT conversion [F-44]. GStreamer and Buildroot SRT support were not researched. *(Superseded in part 2026-10-08: the Raspberry Pi OS GStreamer and FFmpeg SRT components are now sourced [H-30] — see [§3.6](#36-h265-hevc-over-rtmp-and-srt-added-2026-10-08). Buildroot SRT support is still not researched.)* | OQ-076 |
| Browser behaviour with a 1080p stream above the negotiated level | Not sourced; needs an interoperability test in Chrome, Firefox and Safari | OQ-073 |
| Audio encoders (AAC, Opus): element choice, licence, CPU cost | Not sourced *(Superseded in part 2026-10-08: availability and the `fdk-aac` licence are now sourced [I-39], [I-41], [I-42], [I-43], [I-44], [I-45], [I-46], [I-47]; element choice and CPU cost are still open, and AAC patent licensing is OQ-113.)* | OQ-063, OQ-113 |
| H.265 receive per browser and device (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | Partly sourced ([§4.5](#45-h265-in-webrtc-added-2026-10-08)); per-device behaviour not tested | OQ-108 |
| HDMI audio sample-rate changes during a stream (added 2026-10-08) | Driver control and event sourced [I-20], [I-21]; behaviour of the TC358743 output during a change unknown | OQ-111 |
| A/V synchronisation in live outputs (added 2026-10-08) | Clock handling sourced [I-33], [I-35], [I-36], [I-37], [I-38]; no measurement | OQ-112 |

### 4.5 H.265 in WebRTC (added 2026-10-08)

> **Deferred — REQ-ENC-002; not in current scope (2026-10-08).** The owner answered OQ-103 with "H.264 only for now": WebRTC carries H.264 only (REQ-ENC-001), and H.265 is deferred (REQ-ENC-002, `DEFERRED`). This whole section is kept as the evidence for REQ-ENC-002. OQ-108 stays OPEN, not in current scope. No H.265 WebRTC run is planned.

H.265 is required for streaming (REQ-ENC-001). Whether WebRTC carries it is OQ-103. *(Superseded 2026-10-08 by the note above.)*

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

### 4.6 Live latency: the < 1 s WebRTC target (added 2026-10-09)

**Target.** Under 1 s camera-to-viewer for WebRTC viewers (owner, 2026-10-08: "Under 1 second", "WebRTC viewers only"; REQ-STR-002; OQ-116 ANSWERED). Whether PACSCORDER meets it: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-125; RISK-031; TEST-STR-002). How the target is judged — which statistic (for example median, 95th percentile or maximum), over how many samples, under which conditions — is not defined: OWNER DECISION REQUIRED (OQ-008). *(Superseded in part 2026-10-09: owner decision — judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running), with WebRTC viewers on the LAN only; sample count and run length still open (OQ-008).)* Nothing in this section has been run on PACSCORDER hardware. Research topic K covered CM4 and CM5; Pi 4 Model B and Pi 5 are covered below only where stated.

**Protocols**

- **WHIP** is RFC 9725 (Proposed Standard, March 2025). It covers ingest only — unidirectional WebRTC media into a streaming service or CDN — and not playback [K-01].
- **WHEP** is not an RFC as of 2026-10-08. The latest version is draft-ietf-wish-whep-04 (22 June 2026); its WG state is "Waiting for WG Chair Go-Ahead" (since 26 August 2026), and a shepherd write-up was submitted on 6 October 2026 [K-02]. MediaMTX serves WebRTC readers over WHEP at `/<path>/whep`, as reported by the project [F-45] (community source). Research expects MediaMTX's `/whep` behaviour may change when WHEP becomes an RFC (research design risk, topic K — not a register fact; RISK-034).

**MediaMTX, as documented by the project** (the register tiers these MediaMTX sources `vendor-other`, while [F-44] and [F-45] are tiered `community`; see the note under [§5](#5-network-ports) and REFERENCES.md, Register notes, 2026-10-09)

- Its current documentation gives no numeric WebRTC latency figure. It says only that HLS has higher latency than WebRTC, with fewer server-client connectivity problems [K-03]. Its relay latency from RTSP ingest to WebRTC egress is undocumented (research gap, topic K; OQ-125).
- It favours real-time delivery over reliability: most protocols run over UDP so that late packets can be dropped, and outgoing packets pass through a circular buffer (`writeQueueSize`, default 512) that drops packets when full and logs "reader is too slow" [K-44].
- Codec, publishing and packaging facts: [§4.1](#41-codec-requirements-from-the-standards-and-from-libwebrtc) and [§4.3](#43-implementation-options) ([K-04], [K-05], [K-06]).

**Candidate publishing route.** Reasoning recorded under ADR-007 (`OPEN`) from [F-45], [K-05] and [K-06] ([§4.3](#43-implementation-options)). A candidate, not a decision; NOT YET RUN ON PACSCORDER HARDWARE. Element properties beyond those cited are NEEDS VERIFICATION.

```text
Pi 4 / CM4 : capture → v4l2h264enc (live encode; settings in VIDEO_ENCODER.md §7.1) ─┐
Pi 5 / CM5 : capture → UYVY-to-I420/NV12 conversion [D-40] → x264enc (tune=zerolatency [K-27], [K-28]) ─┤
                                                                                        └→ rtspclientsink (latency set explicitly; default 2000 ms [K-09])
                                                                                           → MediaMTX RTSP ingest [K-05] → WebRTC / WHEP readers [F-45]
Audio      : alsasrc [I-45] → opusenc (frame-size default 20 ms [K-40]) → RTSP publish to MediaMTX
             (Reasoning: MediaMTX WebRTC readers take Opus, G722 or G711 only [K-04]. How audio and video share the RTSP publish: NEEDS VERIFICATION.)
RTMP       : the AAC branch and FLV/RTMP publish of §3.2 stay separate (OQ-005; §2.1).
```

**Default buffering in the live path.** Each value must be set explicitly and recorded (OQ-126; RISK-032).

| Element | Default (fact) | Relevance to PACSCORDER's live path | Source |
|---|---|---|---|
| `webrtcbin` | `latency` 200 ms ("Default duration to buffer in the jitterbuffers") | Reasoning: a jitter-buffer size for received RTP. With a browser viewer, the viewer's buffer is the browser's own, so this default matters only where a GStreamer element receives RTP inside the live path. | [K-07], [K-10] |
| `rtpbin`, `rtpjitterbuffer` | `latency` 200 ms; `rtpjitterbuffer` holds packets for at most this time and adds that much latency | As for `webrtcbin` (reasoning) | [K-08], [K-10] |
| `rtspclientsink` | `latency` 2000 ms ("Amount of ms to buffer") | Whether it adds delay on the sending side is not documented (research open question, topic K): UNKNOWN — VERIFICATION REQUIRED. BUILD TEST REQUIRED (OQ-126). | [K-09], [K-10] |
| `rtspsrc` | `latency` 2000 ms; MediaMTX's own GStreamer reader examples set `rtspsrc latency=0` | Only if a GStreamer element reads RTSP inside the live path (reasoning from [K-10]) | [K-09] |
| `v4l2src` | From GStreamer 1.26 source code (`gstv4l2src.c`; the register tiers it `vendor-other`): always live; minimum latency one frame duration; maximum buffer-pool depth × frame duration; pool minimum 2 (4 for alternate-field interlace) | Capture on every platform | [K-37] |
| V4L2 capture queue | Kernel documentation (`mmap.rst`): incoming and outgoing queues are FIFOs; `VIDIOC_DQBUF` returns the oldest filled buffer. Reasoning: each filled buffer still waiting adds one frame period, 33.3 ms at 30p. | Capture on every platform | [K-36] |
| `x264enc` (Pi 5 / CM5) | Medium preset by default: 3 B-frames, rc-lookahead 40 (CORRECTED). With `tune=zerolatency`, x264 holds no frames back. | Live encode | [K-27], [K-28], [K-29] |
| `v4l2h264enc` (Pi 4 / CM4) | Reports 0 latency to the pipeline | Reasoning: the pipeline's latency query leaves out the real encode time (RISK-024) | [K-32] |
| `opusenc` | `frame-size` 20 ms | WebRTC audio | [K-40] |

**Capture, bridge and encode terms**

- **CM4.** The `bcm2835-unicam` driver timestamps each buffer at Frame Start and completes it only at Frame End, so a frame reaches userspace at least one frame's readout time after its first line. If no buffer is queued, the frame goes to a dummy buffer and is dropped, not queued [K-34].
- **CM5.** The RP1 CFE driver also timestamps at Frame Start and completes the buffer at end of frame, with `min_queued_buffers=1`, so capture also costs about one frame's readout time [K-35].
- **Pi 4 Model B and Pi 5.** Reasoning: Pi 4 Model B uses the same downstream Unicam driver as CM4 [C-09], and Pi 5 the same RP1 CFE driver as CM5 [C-29], so the capture terms are expected to carry over. *(Verifier pass, 2026-10-09: [C-09] does not name the boards; the Pi 4 Model B and CM4 device trees both use Unicam nodes compatible `brcm,bcm2835-unicam` [C-08], which the downstream driver binds [C-09]. [C-29] names Pi 5 and CM5.)* NEEDS VERIFICATION (TEST-STR-002 on any kept platform).
- **TC358743.** The driver sets the FIFOCTL trigger level to 374 and states no latency figure; no public source checked documents the bridge's internal buffering [K-38]. DATASHEET REQUIRED (OQ-125).
- **Encode.** CM4: a Raspberry Pi engineer stated that the hardware encoder holds no extra buffers and that its latency is about 10 ms for 720p on a Pi 4 (community source) [K-33]. CM5: Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39]; the per-frame x264 time on BCM2712 is undocumented (OQ-059). Details: [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §3.14 and §7.1.

**Budget (summary; the full budget is in [PERFORMANCE.md](PERFORMANCE.md) §7).** Reasoning (a labelled budget, not a measurement; CORRECTED) [K-45]:

- For a CM4 1080p30 WebRTC/WHEP viewer on a LAN, the documented or extrapolated terms come to about 56 ms typical and about 75 ms worst case: capture readout of about 33.3 ms [K-34]; hardware encode of about 23 ms (the 720p Pi 4 figure of about 10 ms, a community report [K-33], scaled by macroblocks, 8160/3600 = 2.27); and zero frames waiting in V4L2 or GStreamer queues, where each waiting frame would add 33.3 ms (reasoning part of [K-36]). The worst case scales the issue reporter's 720p maximum of 18.5 ms the same way, to about 42 ms: 33.3 + 41.9 ≈ 75 ms [K-33].
- That leaves about 925–945 ms of the 1 s target for undocumented terms: HDMI source or ATEM, TC358743, any pixel-format conversion, MediaMTX relay, LAN, browser jitter buffer, decode and render.
- The encode estimate assumes the live encode has the CM4 hardware encoder to itself, but PACSCORDER's recording encode would share it (whether the encoder runs both is OQ-115). On CM5 the encode term is unknown until per-frame x264 time on BCM2712 is measured (OQ-059).
- Research design risk (topic K — not a register fact): on CM5, if x264 cannot finish each frame within 33.3 ms, queues grow by one frame period per queued frame without bound, so the live branch would need leaky queues or a lower live resolution or frame rate (RISK-032; OQ-126, OQ-059).

**Viewer side**

- Browsers expose `RTCRtpReceiver.jitterBufferTarget`, a hint in ms (at most 4000) that influences but does not set the receiver's jitter-buffer target; MDN marks it Baseline 2026 [K-11].
- The W3C WebRTC Statistics API defines inbound-rtp metrics a viewer can read to measure its own buffering and decode delay: `jitterBufferDelay`, `jitterBufferEmittedCount`, `jitterBufferTargetDelay`, `jitterBufferMinimumDelay` and `totalProcessingDelay`. `roundTripTime` on remote-inbound-rtp is a sender-side metric (available on MediaMTX's side, not in a receive-only browser); a viewer can read RTT from candidate-pair `currentRoundTripTime` or remote-outbound-rtp `roundTripTime` (CORRECTED) [K-12].
- Reference figure only: Cloudflare Stream, a third-party CDN, documents WHEP playback "with less than 500 milliseconds of latency". This is not the PACSCORDER stack [K-14].
- Browsers' minimum jitter-buffer, decode and render times are undocumented (research gap, topic K; OQ-125).

**Viewer join time and keyframes**

- The CM4 encoder defaults to a GOP of 60 [K-30] and can insert an IDR on request through GStreamer 1.26 `v4l2videoenc` [K-31]. Reasoning (as in OQ-127): at 30 fps, without an on-demand keyframe, a new viewer may wait up to about 2 s for a decodable frame.
- Whether MediaMTX passes a WebRTC viewer's keyframe request (PLI/FIR) back to an RTSP publisher is undocumented (research open question, topic K): UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED (OQ-127). Encoder settings: [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §7.1.

**HLS and LL-HLS: not a sub-second path.** HLS is not a required output (OQ-008). MediaMTX's HLS port is in [§5](#5-network-ports).

| Item | Fact | Source |
|---|---|---|
| Apple LL-HLS | No numeric latency. LL-HLS "lowers video latencies over public networks into the range of standard television broadcasts"; the example uses 200 ms Partial Segments with 6 s parent segments. | [K-19] |
| HLS 2nd Edition draft (draft-pantos-hls-rfc8216bis-22) | PART-HOLD-BACK MUST be at least 2× the Part Target Duration and SHOULD be at least 3×. HOLD-BACK MUST be at least 3× the Target Duration. | [K-20] |
| MediaMTX v1.21.1 defaults | `hlsVariant: lowLatency`, `hlsSegmentDuration: 1s`, `hlsPartDuration: 200ms`, `hlsSegmentCount: 7`. The configuration notes that segment count does not influence latency, that segments stretch to include at least one IDR frame, and that "A player usually puts 3 parts in a buffer". | [K-21] |
| gohlslib v2.4.5 (MediaMTX v1.21.1's HLS library) | In Low-Latency mode it advertises PART-HOLD-BACK = 2.5 × the Part Target Duration, which it sets to the longest actual part. `hlsPartDuration` is a minimum, so PART-HOLD-BACK is at least 500 ms. 2.5× meets the draft's 2× MUST but not its 3× SHOULD. It emits no HOLD-BACK, so the spec's implied 3× Target Duration applies (CORRECTED). | [K-22] |
| hls.js (used in MediaMTX's docs for browser HLS) | `lowLatencyMode` defaults to true and starts live streams at PART-HOLD-BACK instead of HOLD-BACK; `liveSyncDurationCount` defaults to 3 target durations. | [K-23] |
| Apple devices | MediaMTX's configuration says HTTPS (`hlsEncryption`) is required for Low-Latency HLS to work correctly on Apple devices. | [K-24] |
| Historical figures | The pre-rename documentation (rtsp-simple-server v0.21.6, March 2023) put HLS latency at 1–15 s depending on segment duration and at 500 ms–3 s with the Low-Latency variant. Current MediaMTX documentation no longer states these figures. | [K-25] |

Reasoning (CORRECTED) [K-26]: LL-HLS through MediaMTX defaults is unlikely to reliably reach under 1 s glass-to-glass, and regular HLS cannot.

- LL-HLS: a player that stays at least PART-HOLD-BACK (at least 0.5 s) behind the playlist end runs at least PART-HOLD-BACK plus one part (at least 0.2 s) behind capture. With about 33 ms of capture readout, that is about 0.73 s before encode, HTTP round trips, player buffer, decode and render.
- Regular HLS: segments are at least 1 s and stretch to the IDR interval (2 s with the CM4 encoder's default GOP of 60 at 30 fps). HOLD-BACK of at least 3× the Target Duration then gives at least 6 s; even a 1 s GOP gives at least 3 s.
- Not checked (research gaps, topic K): the hls.js latency controller's live-edge estimate, Safari's native LL-HLS player, and MediaMTX's MoQ (Media over QUIC) support.

**Internet viewers** (only if OQ-008 puts them in scope; OQ-128, RISK-033) *(Not in current scope since 2026-10-09 — LAN-only viewers (owner, 2026-10-09; OQ-008); kept as reference. OQ-128 and RISK-033 stay OPEN.)*

- MediaMTX v1.21.1 defaults to `webrtcLocalUDPAddress` `:8189` (a UDP/ICE listener) and leaves `webrtcLocalTCPAddress` disabled, because TCP "is less efficient than UDP and introduces a progressive delay when network is congested" [K-41].
- By default it advertises the IPs of its network interfaces (`webrtcIPsFromInterfaces: true`). Its docs say to put the server's LAN address in `webrtcAdditionalHosts` for LAN clients, and its public IP or DNS name for internet clients. STUN/TURN (`webrtcICEServers2`) is "Needed only when local listeners can't be reached by clients" [K-42].
- It lists four connection methods: static UDP port (default), static TCP port, random UDP port with STUN hole punching, and a TURN relay. For coturn it recommends TCP transport only, and notes that the TURN server can be configured as client-only [K-43].
- Reasoning from [K-41] and [K-43] (as in RISK-033): viewers forced onto TCP or a TCP TURN relay may see delay that grows under congestion, which threatens the < 1 s target. Latency and stability over STUN and TURN are untested (research open question, topic K): HARDWARE TEST REQUIRED (OQ-128).

### 4.7 WebRTC bitrate and per-viewer load (added 2026-10-09)

**Owner decision of 2026-10-09 (OQ-005).** WebRTC viewers receive the shared live encode, **CBR at 17 Mbit/s**. Viewers are on the LAN only (owner, 2026-10-09; OQ-008). Nothing below has been implemented or run on PACSCORDER hardware.

- **Per-viewer load (reasoning, not a measurement).** Each WebRTC viewer on the LAN receives about 17 Mbit/s of video, plus Opus audio and RTP/SRTP overhead, which are not budgeted here (`opusenc` defaults to 64000 bit/s [I-47]; the audio settings are OQ-063). If each viewer has its own WebRTC session, the sender transmits N × 17 Mbit/s of video for N viewers — for example about 85 Mbit/s for 5 viewers and about 170 Mbit/s for 10. The viewer count is still OQ-008; where the WebRTC sender runs (MediaMTX or PACSCORDER's own stack) is ADR-007 (OPEN); Ethernet on the candidate boards is not researched (OQ-098). Whether the network carries a given number of viewers: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (TEST-STR-002).
- **One bitrate for every output (reasoning from the two-encode decision).** One CBR live encode serves RTMP and every WebRTC viewer, so every viewer receives the same 17 Mbit/s. A lower rate for one viewer or one network would need another encode, which the owner's two-encode decision does not include (OQ-005).
- **`webrtcsink`.** It is documented as encoding internally [F-43], with congestion control and a `max-bitrate` default of 8,192,000 bit/s [K-13], below 17 Mbit/s. Whether it can pass the shared 17 Mbit/s live encode through unchanged is not in the register: NEEDS VERIFICATION (ADR-007, OQ-074). It is also not packaged in Debian trixie or the Raspberry Pi archive [K-06].
- **MediaMTX.** It drops packets when a reader's outgoing circular buffer (`writeQueueSize`, default 512) is full [K-44]. Whether that default is enough at 17 Mbit/s is not in the register: UNKNOWN — BUILD TEST REQUIRED; HARDWARE TEST REQUIRED (OQ-126).
- **Level and browsers.** Whether 17 Mbit/s fits the maximum bitrate of the profile and level the live encode signals and browsers negotiate is not in the register — [F-40] covers frame size and macroblock rate only: NEEDS VERIFICATION. DATASHEET REQUIRED (the H.264 specification), then TEST-STR-002 (OQ-073; RISK-019). Browser limits on received bitrate are not in the register either.
- **Frame size on the wire (reasoning).** At 17 Mbit/s the average 1080p30 live frame is about 70,833 bytes (17,000,000 / 30 / 8). How long it takes on the LAN depends on the link speed, which is not researched (OQ-098). Latency budget: [PERFORMANCE.md](PERFORMANCE.md) §7.1.
- **Encoder load.** Both encodes at 17 and 25 Mbit/s: CM4 OQ-115 (RISK-002); CM5 OQ-059 (RISK-003).

## 5. Network ports

Defaults from the sources. The ports PACSCORDER actually opens are UNKNOWN — VERIFICATION REQUIRED, OWNER DECISION REQUIRED (OQ-075).

| Port | Transport | Service | Relevance to PACSCORDER | Source |
|---|---|---|---|---|
| 1935 | TCP | RTMP default port | Outbound when publishing (REQ-STR-001). Inbound only if PACSCORDER hosts an RTMP server (OQ-075); receiving ATEM RTMP is not in current ATEM scope (REQ-ATEM-001). | [F-32], [F-44] |
| 1935 | TCP | Forwarded to an ATEM Streaming Bridge, or to an ATEM Mini Extreme ISO G2, for internet links | ATEM-side network setup (see [ATEM.md](ATEM.md)). Reference only: sending PACSCORDER video into an ATEM setup is not in current ATEM scope (REQ-ATEM-001, owner 2026-10-07). | [F-28], [F-29] |
| 8889 | Not stated in the register (serves the browser page and the WHEP endpoint [F-45]) | MediaMTX `webrtcAddress` default | Inbound, only if MediaMTX is used for WebRTC | [F-44], [F-45] |
| 8189 | UDP (reasoning from the setting name `webrtcLocalUDPAddress`) *(Superseded 2026-10-09: UDP is now sourced [K-41].)* | MediaMTX `webrtcLocalUDPAddress` default. [F-44] gives only the setting name and `:8189`; calling it a WebRTC local UDP listener (for example for ICE/RTP) is reasoning from the name, and its exact role is NEEDS VERIFICATION *(Superseded 2026-10-09: MediaMTX v1.21.1's configuration describes it as "Address of a UDP/ICE listener that will receive connections" [K-41]. Reasoning from MediaMTX's documentation [K-42]: internet viewers need this port reachable and the public address advertised, or STUN/TURN (OQ-128).)* *(The internet-viewer sentence is not in current scope since 2026-10-09 — LAN-only viewers (owner, 2026-10-09; OQ-008); kept as reference.)* | Inbound, only if MediaMTX is used for WebRTC | [F-44], [K-41] |
| — (row added 2026-10-09) | TCP | MediaMTX `webrtcLocalTCPAddress` (TCP/ICE listener): disabled by default, because TCP "introduces a progressive delay when network is congested" [K-41] | Only if enabled; not recommended for the < 1 s target (reasoning from [K-41]; RISK-033) *(2026-10-09: RISK-033 is not in current scope — LAN-only viewers (owner; OQ-008).)* | [K-41] |
| 8554 | Not stated in the register | MediaMTX RTSP default | Only if enabled | [F-44] |
| 8890 | Not stated in the register | MediaMTX SRT default | Only if SRT is required (OQ-076) | [F-44] |
| 8888 | Not stated in the register | MediaMTX HLS default | Only if enabled | [F-44] |
| 9997 | Not stated in the register | MediaMTX API (`api: false` by default) | Only if enabled | [F-44] |
| 9910 | UDP | ATEM control protocol (reverse-engineered), as reported by the OpenSwitcher project [F-11]. The atem-connection and PyATEMMax projects report this port as their default [F-14], [F-19]. | Outbound to the ATEM (reasoning: PACSCORDER would be the client), only for control or tally integration. That integration is not in current scope (REQ-ATEM-001; OQ-009 ANSWERED 2026-10-07; RISK-018). | [F-11], [F-14], [F-19] |

All MediaMTX entries come from a community source (the MediaMTX project's own documentation and configuration file). *(Superseded in part 2026-10-09: the topic K MediaMTX entries [K-03], [K-04], [K-05], [K-21], [K-24], [K-41], [K-42], [K-43] and [K-44] cite the same kind of source — MediaMTX's own documentation and configuration — but carry tier `vendor-other` in the register, while [F-44] and [F-45] carry tier `community`. Both are stated here as what MediaMTX documents, as REFERENCES.md "Register notes" (2026-10-09) directs.)* Conflicts when PACSCORDER both receives RTMP from an ATEM and publishes RTMP onward are part of OQ-075; that case arises only if the owner adds receiving ATEM RTMP to scope.

## 6. Tests

| Test | Verifies | Status |
|---|---|---|
| TEST-STR-001 RTMP publish and playback | REQ-STR-001 | `BLOCKED — HARDWARE REQUIRED` |
| TEST-STR-002 WebRTC browser playback | REQ-STR-002 (also resolves OQ-073) | `BLOCKED — HARDWARE REQUIRED` |
| TEST-ENC-001 Sustained real-time H.264 encode (H.265 deferred) | Encoder capacity behind both streams *(2026-10-08: the live encode shared by both streams, running at the same time as the recording encode; OQ-005, OQ-115, OQ-059)* | `BLOCKED — HARDWARE REQUIRED` |
| TEST-PERF-001 Soak: thermal, CPU, CMA, frame drops | Concurrent recording + RTMP + WebRTC *(2026-10-08: two video encodes, recording and live, plus the AAC and Opus audio encodes)* | `BLOCKED — HARDWARE REQUIRED` |
| TEST-AUD-001 HDMI audio capture over I2S (added 2026-10-08) | REQ-CAP-006, the audio source for both streams | `BLOCKED — HARDWARE REQUIRED` |

*(Added 2026-10-08; scope notes from the registers, no test ID or status changed. Updated the same day after the owner answered OQ-103 with "H.264 only for now": both streams are H.264 only (REQ-ENC-001) and H.265 is deferred (REQ-ENC-002).)* With H.264 only, audio required and CM4 and CM5 evaluated side by side (ADR-004):

- **TEST-ENC-001** ("Sustained real-time H.264 encode (H.265 deferred)"): its H.265 runs on CM4 and CM5, with and without a concurrent H.264 encode and audio (OQ-104, OQ-105; RISK-022), are deferred, not run in current scope.
- **TEST-STR-001** publishes H.264 with AAC audio. H.265 publishing to each destination (OQ-106) is deferred, not run in current scope.
- **TEST-STR-002**:
  - the Edge and per-device H.265 runs (OQ-108) are deferred, not run in current scope, because no H.265 track is offered;
  - checks Opus audio.
- **TEST-AUD-001** includes:
  - sources at 44.1 kHz and 48 kHz, and a rate change during capture (OQ-111, RISK-023);
  - an A/V offset measurement (OQ-112, RISK-024);
  - runs on both CM4 and CM5 (OQ-054).
- **TEST-PERF-001** includes a multi-hour A/V drift check (RISK-024).

*(Added 2026-10-08 after the owner answered OQ-005 with "Separate record + live"; scope notes, not accepted criteria; no test ID or status changed.)*

- **TEST-ENC-001** must include a **two-encode run** on CM4 and on CM5: the recording encode and the live encode at the same time.
  - CM4: OQ-115, RISK-002.
  - CM5: OQ-059, RISK-003.
  - The live encode's profile, level, B-frame and SPS/PPS settings are checked against [§4.1](#41-codec-requirements-from-the-standards-and-from-libwebrtc).
- **TEST-STR-001** and **TEST-STR-002** take their video from the same live encode. Reasoning: the RTMP run therefore uses the WebRTC-receivable settings (OQ-007 for destination acceptance).
- **TEST-PERF-001** must cover the **combined load** on CM4 and CM5:
  - two H.264 encodes (recording and live);
  - the AAC and Opus audio encodes;
  - recording, RTMP publishing and WebRTC serving at once.

  Related: OQ-115, OQ-059, OQ-063; RISK-002, RISK-003.

*(Added 2026-10-09 after the owner decisions of 2026-10-08 on latency (OQ-116 ANSWERED) and recording (ADR-009); scope notes taken from the "Retire by" lines of RISK-019, RISK-028, RISK-031 to RISK-033 and from OQ-125 to OQ-128; not accepted criteria; no test ID or status changed.)*

- **TEST-STR-002** measures camera-to-viewer latency against the < 1 s target on CM4 and CM5, in each target browser, with the recording encode running (OQ-125; RISK-031). Its pass criterion — which statistic, over how many samples, under which conditions — is not defined: OWNER DECISION REQUIRED (OQ-008). *(Superseded in part 2026-10-09: owner decision — judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running), with LAN viewers; sample count and run length still open (OQ-008).)* The method named in OQ-125 (an on-screen millisecond clock filmed next to the viewer's display, plus the browser statistics of [K-12]) is NOT YET RUN ON PACSCORDER HARDWARE. It also:
  - records the latency setting of every live-path element and includes a run with an injected slow branch (OQ-126; RISK-032);
  - records viewer join time with the chosen keyframe policy (OQ-127; RISK-019);
  - checks that an injected HDD stall in the mirrored recording does not change live latency or frame rate (OQ-117; RISK-028);
  - runs from outside the LAN through NAT, over UDP and over TURN, only if OQ-008 puts internet viewers in scope (OQ-128; RISK-033). *(Not in current scope since 2026-10-09 — LAN-only viewers (owner, 2026-10-09; OQ-008); these runs are not run in current scope; kept as reference.)*
- **TEST-STR-001**: RTMP latency is best-effort (OQ-116 ANSWERED), so it is not a pass criterion (reasoning). The run confirms that the destinations accept the B-frame-free live encode (OQ-127; RISK-019).

*(Added 2026-10-09, later, after the owner decisions on OQ-005 (live CBR 17 Mbit/s, recording 25 Mbit/s VBR) and OQ-129 (continue on the remaining drive); scope notes, not accepted criteria; no test ID or status changed.)*

- **TEST-STR-001** and **TEST-STR-002** run with the live encode at CBR 17 Mbit/s and the recording encode at 25 Mbit/s VBR. TEST-STR-001 records whether each destination accepts 17 Mbit/s CBR (OQ-007); TEST-STR-002 records the number of LAN viewers and the sender's outgoing rate, against the N × 17 Mbit/s reasoning of [§4.7](#47-webrtc-bitrate-and-per-viewer-load-added-2026-10-09) (viewer count OQ-008), and covers the level question for 17 Mbit/s (OQ-073).
- **TEST-ENC-001** two-encode runs use the same rates on CM4 (OQ-115) and CM5 (OQ-059).
- The drive-failure runs belong to TEST-REC-001 (OQ-129). Reasoning: a TEST-STR-002 run in which one mirrored drive is removed or fills would show whether the live path stays unaffected (OQ-117; RISK-028).

Procedures will be written in [TESTING.md](TESTING.md). Every command in this document is NOT YET RUN ON PACSCORDER HARDWARE.

## Verification status

### Verified from sources (fact IDs)

This document cites 122 register entries, all with verdict `CONFIRMED` or `CORRECTED` (120 until 2026-10-08; D-06 and D-52 added with the two-encode decision):

D-06, D-10, D-11, D-12, D-14, D-15, D-24, D-31, D-32, D-35, D-36, D-37, D-38, D-39, D-40, D-41, D-42, D-43, D-44, D-45, D-46, D-47, D-52, D-54, E-31, E-34, E-35, E-36, F-06, F-11, F-14, F-19, F-26, F-28, F-29, F-31, F-32, F-33, F-34, F-35, F-36, F-37, F-38, F-39, F-40, F-41, F-42, F-43, F-44, F-45, F-46, G-22, G-26, G-27, G-28, G-29, G-30, G-31, G-32, G-64, G-65, H-01, H-02, H-04, H-05, H-07, H-08, H-09, H-10, H-11, H-12, H-13, H-14, H-15, H-16, H-19, H-20, H-21, H-22, H-23, H-24, H-25, H-26, H-27, H-28, H-29, H-30, H-31, H-32, H-33, H-34, H-35, H-36, H-43, I-03, I-05, I-06, I-07, I-08, I-10, I-13, I-15, I-18, I-20, I-21, I-22, I-24, I-26, I-33, I-35, I-36, I-37, I-38, I-39, I-40, I-41, I-42, I-43, I-44, I-45, I-46, I-47.

- `CORRECTED` entries, used in their corrected wording only: D-46, F-28, F-34, F-37, H-10, H-12, I-40.
- `community` entries, worded as reports: D-54, F-11, F-14, F-19, F-44, F-45, H-19, H-20, H-21, H-22, H-36.
- `reasoning` entries, labelled as reasoning: D-36, D-52, F-35, F-38, F-40, F-46, H-23, H-43, I-18.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.

*(Added 2026-10-09; research topics J and K of 2026-10-08.)* 48 entries added, so the document cites 170 register entries, all with verdict `CONFIRMED` or `CORRECTED` *(verifier pass, same date: C-08 added, so 49 entries added and 171 cited)*:

C-08 (added in the verifier pass), C-09, C-29 (reasoning inputs for Pi 4 Model B and Pi 5 capture terms, §4.6); J-30; K-01, K-02, K-03, K-04, K-05, K-06, K-07, K-08, K-09, K-10, K-11, K-12, K-13, K-14, K-15, K-16, K-17, K-18, K-19, K-20, K-21, K-22, K-23, K-24, K-25, K-26, K-27, K-28, K-29, K-30, K-31, K-32, K-33, K-34, K-35, K-36, K-37, K-38, K-39, K-40, K-41, K-42, K-43, K-44, K-45.

- `CORRECTED` entries, used in their corrected wording only: J-30, K-12, K-18, K-22, K-26, K-29, K-45.
- `community` entries, worded as reports: K-33.
- `reasoning` entries, labelled as reasoning: K-10, K-18, K-26, K-45; also the reasoning sentence of the `kernel-source` entry K-36.
- Statements marked *research gap*, *research open question* or *research design risk* come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json) and are not register facts.

*(Added 2026-10-09, later; owner decisions on OQ-005 and OQ-129.)* One entry added, so the document cites 172 register entries, all with verdict `CONFIRMED` or `CORRECTED`: D-13 (`kernel-source`; CM4 encoder bitrate range and modes, §3.8 and the 2026-10-09 notes). The bitrate notes also cite D-54 (community, worded as a report), F-40, F-43, H-29, I-47, K-06, K-13, K-17 and K-44, already listed above. The per-viewer and per-destination network figures (N × 17 Mbit/s; about 85 and 170 Mbit/s for 5 and 10 viewers) and the average frame size at 17 Mbit/s are Claude's reasoning, not register facts. No register fact gives the maximum bitrate per H.264 profile and level (OQ-073) or `x264enc` bitrate property names.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). *(Still nothing as of 2026-10-08: no hardware, no code, no test run.)* *(Still nothing as of 2026-10-09: no latency, no RTMP or WebRTC run, no measurement of any kind.)* *(Still nothing later on 2026-10-09: no stream has run at the decided 17 Mbit/s.)*

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
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header "Applies to" (REQ-ENC-001 H.264 only, REQ-ENC-002 DEFERRED, RISK-022/RISK-025 not in current scope); owner-decision box (Codecs bullet superseded, new 2026-10-08 note); §1 codec-per-output row superseded, H.265 capacity, HEVC-RTMP muxing and acceptance, WebRTC H.265 rows labelled deferred, HEVC licensing (OQ-109) and HEVC-over-SRT notes; §2 H.265-per-platform table labelled deferred, throughput line (TEST-ENC-001 H.265 runs not run in current scope); §2.1 enhanced-FLV audio row labelled; §3.1 superseded note (H.264 fits legacy FLV [F-31]) and E-RTMP HEVC label; §3.2 `flvmux` H.265, planar-input note and H.265 pipeline line labelled; §3.3 HEVC-in-FLV and `libx265` labelled, `libx265` still pulled in by `libavcodec61` (reasoning [H-09]); §3.4 x265, `x265enc`, `libx265`, `rtph265pay`, SRT/MPEG-TS rows "only if REQ-ENC-002 is re-activated"; §3.6 and §4.5 deferred notes (opening sentences superseded), §3.6 capacity note; §4.4 H.265 gap row labelled; §6 TEST-ENC-001 title as in README.md and H.265 test runs deferred, not run in current scope. H.265 research kept as evidence; no citation added or removed; no other decision or status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): header "Applies to" (two encodes; OQ-005, OQ-115, OQ-059; RISK-002 added); new owner-decision note (one live encode shared by RTMP and WebRTC plus a recording encode; bitrate, rate control and latency still OQ-005; live encode WebRTC-receivable [F-36], [F-39], [F-40], [F-45], [D-11], [D-14], [D-15]; CM4 concurrency OQ-115 with the 2.0× reasoning [D-10], [D-52]; CM5 two software encodes OQ-059; audio still AAC + Opus; H.265 still deferred); §1 new "Video encodes behind the streams" row and live-encode note on the OQ-073 row; §2 diagram redrawn (DMABUF to live and recording encoders; old form recorded) with topology reasoning (two consumers per capture buffer, OQ-058, OQ-115; live bitstream split), platform-table notes (CM4 OQ-115/RISK-002, CM5 OQ-059/RISK-003) and the "share one encode or separate" sentence marked superseded; §2.1 two-audio-encode and CPU-cost notes; §3.2 note that the shapes show the live encode, with its WebRTC settings and split; §4.2 note (RTMP receives the Constrained Baseline stream, destination acceptance OQ-007; recording encode not bound; per-platform concurrency); §6 TEST-ENC-001 and TEST-PERF-001 rows annotated, scope notes for the two-encode run on CM4 and CM5, the shared live encode in TEST-STR-001/002 and the combined load in TEST-PERF-001; Verification status 120 → 122 entries (D-06, D-52; D-52 reasoning). No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — §2 platform table: CM4 "the live encode runs beside" → "would run beside" (OQ-115); CM5 "two software encodes run at once" → "are required at once" (OQ-059); §3.2 live-encode "No B-frames [D-14], [F-45]" split into the [D-14] fact and the MediaMTX report [F-45] (community source). No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header "Last updated", "Applies to" (WebRTC < 1 s camera-to-viewer, RTMP best-effort, ADR-009, OQ-125 to OQ-128, RISK-031 to RISK-034) and "Verification" (topics J and K); owner-decision box — OQ-005 "still open: latency" superseded in part, new note on the 2026-10-08 latency and recording decisions; §1 OQ-008 and "Video encodes" rows superseded in part (latency decided), six rows added (latency per output, element latencies OQ-126, publishing route RISK-034, keyframes OQ-127, internet viewers OQ-128, HDD-branch isolation OQ-117); §2 latency sentence superseded in part, ADR-009 mirrored-recording note [J-30], [K-36] (OQ-117, RISK-028); §2.1 Opus frame size [K-40]; new §3.7 RTMP latency (YouTube modes and API [K-15], [K-16], no sub-second mode [K-18], encoder recommendations and B-frame conflict [K-17], [K-04], OQ-127); §4.1 MediaMTX B-frame/Baseline/Opus documentation [K-04], `whipclientsink` needs [K-05]; §4.2 CM4 encoder re-confirmed [K-30], `x264enc` default trap [K-29], [K-27]; §4.3 `webrtcsink` defaults [K-13], package NEEDS VERIFICATION superseded [K-06], candidate RTSP-to-MediaMTX route (reasoning under ADR-007, OPEN); §4.4 WHIP/WHEP and ICE gap rows superseded [K-01], [K-02], [K-41]–[K-43]; new §4.6 live latency (protocols, MediaMTX [K-03], [K-44], candidate route sketch NOT YET RUN, default element latencies [K-07]–[K-10], [K-36], [K-37], capture/bridge/encode terms [K-33]–[K-35], [K-38], [K-39], [C-09], [C-29], budget summary [K-45] pointing to PERFORMANCE.md §7, viewer side [K-11], [K-12], [K-14], keyframes [K-30], [K-31], HLS/LL-HLS not sub-second [K-19]–[K-26], internet viewers [K-41]–[K-43]); §5 port 8189 role superseded [K-41], TCP/ICE row added, MediaMTX tier note; §6 scope notes for TEST-STR-002 and TEST-STR-001; Verification status 122 → 170 entries. No requirement, ADR, risk, OQ or test status changed. Verifier pass (same date): owner-decision box and §1 OQ-008 row — how the < 1 s target is judged (statistic, samples, conditions) added as still open (OQ-008); HDD "self-powered enclosure or hub" → "self-powered enclosure" (the owner's words); §1 publishing-route row — "not packaged [K-06]" → "not packaged in Debian trixie or the Raspberry Pi archive", WHEP dated "as of 2026-10-08"; §2 — "the register's example 2.5-inch HDD" → one Seagate BarraCuda 2.5-inch family, an example [J-30]; CM4 "shares the hardware encoder" → "would share" (OQ-115); [K-36] per-frame cost labelled as its reasoning part; §3.7 — B-frame conflict attributed to MediaMTX's documentation [K-04] (table and consequences); §4.3 — `webrtcsink` row: [K-06] Raspberry Pi archive provenance (register note) and BUILD TEST REQUIRED for the image; candidate-route sentence kept, with a note that it is a route needing no gst-plugins-rs, not the only one (`webrtcbin` remains an option [F-42], [G-27]), and that the package providing `rtspclientsink` is NEEDS VERIFICATION; §4.6 — target-judging OQ-008 sentence; MediaMTX tier pointer (§5, register note); `v4l2src` row labelled GStreamer source code [K-37]; V4L2 queue row labelled kernel documentation [K-36]; Pi 4 Model B capture reasoning re-based on [C-08] with [C-09] ([C-09] does not name the boards); budget — 10 ms labelled a community report [K-33], worst-case inputs shown (18.5 ms × 2.27 ≈ 42 ms; 33.3 + 41.9 ≈ 75 ms), recording encode "shares it" → "would share it"; §5 — 8189 internet-viewer sentence labelled reasoning from MediaMTX's documentation [K-42]; tier note points to the register note; §6 — TEST-STR-002 pass criterion open (OQ-008); Verification status 170 → 171 entries (C-08). | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 (latency judged at the 95th percentile; LAN-only WebRTC viewers; recording until stopped or disk full — OQ-006 ANSWERED, OQ-008, OQ-128/RISK-033 not in current scope, new OQ-129): header "Applies to" — dated note (95th-percentile criterion; LAN only; OQ-128 and RISK-033 not in current scope, still OPEN); owner-decision box "Still open" bullet superseded in part (criterion and reach decided; browsers, viewer count, sample count and run length still OQ-008); §1 — OQ-008 reach/browsers/latency row superseded in part, "Internet WebRTC viewers" row scope note, "WebRTC signalling and NAT traversal" row: NAT-traversal part not in current scope, signalling still OQ-074; §4.4 ICE/STUN/TURN gap row scope note; §4.6 target sentence superseded in part, "Internet viewers" block scope note (kept as reference); §5 port 8189 internet-viewer sentence and TCP/ICE row (RISK-033) scope notes; §6 TEST-STR-002 pass criterion superseded in part, outside-the-LAN runs not run in current scope. This document states no recording duration, so OQ-006 and OQ-129 needed no change here. Original text kept; no citation added or removed; no requirement, ADR, risk, OQ or test status changed. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09, later (live CBR 17 Mbit/s and recording 25 Mbit/s VBR — OQ-005; continue on the remaining drive if one fails — OQ-129): header "Applies to" — "bitrate and rate control still open" superseded; owner-decision box — OQ-005 "still open" note and the 2026-10-08 "Still open" bullet superseded, new box "Owner decisions of 2026-10-09 on bitrate, rate control and drive failure" (basis [D-13], [H-29], [K-17]; YouTube an example, OQ-007; no per-mode values; level fit OQ-073, CM4 OQ-115, CM5 OQ-059; OQ-129 policy with OQ-091); §1 — RTMP destinations row (RTMP carries the 17 Mbit/s live encode; the register still lists the RTMP bitrate under OQ-007) and "Video encodes behind the streams" row superseded, OQ-073 row note (bitrate part, DATASHEET REQUIRED); §2 — "Bitrate and rate control remain OQ-005" superseded (CM4 range [D-13], OQ-115; CM5 OQ-059), new OQ-129 note after the ADR-009 paragraph (a failed drive must not end the recording; stall concern unchanged, RISK-028, OQ-117); §3.2 live-encode note (CBR 17 Mbit/s; CM4 controls [D-13], `video_bitrate` in `extra-controls` a community report [D-54], bitrate-mode name NEEDS VERIFICATION); §3.6 YouTube bullet note; §3.7 new consequence (CBR 17 Mbit/s follows [K-17], [H-29]); new §3.8 RTMP bitrate and rate control (table [H-29], [K-17], [D-13]; CM5 `x264enc` properties NEEDS VERIFICATION; per-destination upstream reasoning N × 17 Mbit/s; Ethernet not researched, OQ-098; level NEEDS VERIFICATION, OQ-073); §4.2 recording-encode bullet superseded in part (25 Mbit/s VBR; profile and level still UNDEFINED, no longer under OQ-005); new §4.7 WebRTC bitrate and per-viewer load (reasoning N × 17 Mbit/s, about 85 / 170 Mbit/s for 5 / 10 viewers, viewer count OQ-008; one bitrate for every output; `webrtcsink` [F-43], [K-13], [K-06]; MediaMTX `writeQueueSize` [K-44], OQ-126; level OQ-073; average frame ≈ 70,833 bytes at 1080p30); §6 scope notes (TEST-STR-001, TEST-STR-002, TEST-ENC-001 at the decided rates; drive-failure runs in TEST-REC-001); Verification status 171 → 172 entries (D-13). Original text kept; no requirement, ADR, risk, OQ or test status changed. Main session, same date: §4.2 note aligned with the register — OQ-005 keeps the recording encode's profile and level open. Main session, same date: OQ-005 remainder wording aligned with the register — it also keeps the recording encode's profile, level and B-frames open. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 on OQ-129 (start on the available drive when one is missing; split recordings every 30 minutes): owner-decision box — "Drive failure (OQ-129, still OPEN)" bullet ("Still open: file splitting …") superseded in part; §2 OQ-129 note ("still OPEN for file splitting …") superseded in part. OQ-129 stays OPEN only for drive return; alert method OQ-091. No streaming statement, citation, requirement, ADR, risk, OQ or test status changed. | Claude (session 2026-10-09) |
