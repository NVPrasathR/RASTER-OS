# PACSCORDER Blackmagic ATEM Integration

| | |
|---|---|
| Document status | Active — source research only. ATEM integration design and implementation: NOT STARTED. No code exists. |
| Last updated | 2026-10-07 |
| Applies to | REQ-ATEM-001, REQ-CAP-008 (ATEM and camera HDMI sources), REQ-CAP-007 (2-lane and 4-lane configurations; Section 7.1); RISK-008, RISK-009, RISK-018 (only if network control is added); OQ-102; ATEM Mini Pro (primary source of specifications), with ATEM Mini Pro ISO, Mini Extreme, Mini Extreme ISO G2 and ATEM Streaming Bridge where stated; all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5) |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)), plus the owner's scope decision of 2026-10-07 (OQ-009). Nothing has been tested. No PACSCORDER hardware and no code exist as of 2026-10-07. Whether an ATEM test unit is available to the project is not recorded. |

REQ-ATEM-001 says that PACSCORDER "shall integrate with Blackmagic Design ATEM switchers".

> **Scope decision (owner, 2026-10-07; OQ-009 ANSWERED).** The owner said: "it can be atem and direct video from camera". Recorded interpretation (Claude; the owner may correct it), as in [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ATEM-001 and REQ-CAP-008:
>
> - **In scope — mode 1, HDMI capture.** PACSCORDER captures the ATEM's HDMI output through the TC358743, like any other HDMI source (Sections 6.1, 6.2 and 7).
> - **In scope — cameras connected directly.** HDMI cameras may also be connected to PACSCORDER without an ATEM (REQ-CAP-008, DRAFT; Section 7.4).
> - **Not in current scope — modes 2, 3a and 3b.** Network tally/control over UDP 9910 (mode 2), receiving the ATEM's RTMP stream (mode 3a) and sending PACSCORDER's RTMP stream into an ATEM setup (mode 3b) were offered and not selected. They are out of scope unless the owner adds them. Their research is kept in this document as reference only and is labelled "not in current scope".
> - **Still open.** Which ATEM models and firmware versions, and which camera models, must be supported: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (OQ-102).

This document records the integration surfaces that the sources describe. The surface in scope (mode 1) is the HDMI capture path. The surfaces not selected are kept, with their prerequisites and open questions, so that the owner can add them later if needed.

Fact IDs such as `[F-23]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Facts of tier `community` are worded as reports. Calculations are marked as reasoning and list their inputs. Specifications of ATEM models other than those named above were not researched: UNKNOWN — VERIFICATION REQUIRED.

## 1. Status at a glance

| Item | Status |
|---|---|
| REQ-ATEM-001 | Acceptance `DRAFT`. Implementation `NOT STARTED`. Scope recorded 2026-10-07: mode 1 (HDMI capture) only (OQ-009 ANSWERED). |
| REQ-CAP-008 (HDMI sources: ATEM switchers and cameras) | Acceptance `DRAFT`. Implementation `NOT STARTED`. |
| TEST-ATEM-001 ATEM HDMI output capture (scope per OQ-009) | `BLOCKED — HARDWARE REQUIRED` (no PACSCORDER hardware exists; availability of an ATEM test unit is UNKNOWN — VERIFICATION REQUIRED, OWNER DECISION REQUIRED). Its scope is mode 1, HDMI capture of the ATEM output (OQ-009 ANSWERED). It verifies REQ-ATEM-001 and REQ-CAP-008. |
| Integration scope | Mode 1 (HDMI capture) only. Modes 2, 3a and 3b are not in current scope. Owner decision of 2026-10-07 (OQ-009 ANSWERED). |
| ATEM models and firmware to support | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-102). |
| Camera models to support (directly connected sources) | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (OQ-102). |
| Lane configurations | Both 2-lane and 4-lane CSI-2 configurations are required, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; OQ-001 ANSWERED 2026-10-07). Which ATEM output standards each configuration carries: Section 7.1. |
| Network control library | Not needed in current scope (mode 2 not selected). If the owner adds mode 2: UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-084, which also needs legal clarification). |
| Blackmagic SDK licence terms | Relevant only if the owner adds mode 2. Then: UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED (OQ-089, which also needs legal clarification). |
| PACSCORDER network interface (Ethernet, Wi-Fi) for modes 2 and 3 | Modes 2 and 3 are not in current scope. The network interface still matters for RTMP and WebRTC streaming ([STREAMING.md](STREAMING.md)). UNKNOWN — VERIFICATION REQUIRED. No register fact covers the network interfaces of the candidate boards: DATASHEET REQUIRED (OQ-098). Choice of interface: OWNER DECISION REQUIRED (OQ-018). |
| Risks | In scope: RISK-008 (HDCP), RISK-009 (interlaced inputs; Section 7.4), RISK-014 (audio path), RISK-001 (lane count). RISK-018 (reverse-engineered control protocol) applies only if the owner adds mode 2. |

## 2. Candidate integration modes

The three modes researched (REQ-ATEM-001 options 1–3) are below. Mode 3 is split by direction, because the two directions have different prerequisites. **On 2026-10-07 the owner chose mode 1 (OQ-009 ANSWERED).** Modes 2, 3a and 3b are kept as reference and are **not in current scope**.

| Mode | What PACSCORDER would do | Prerequisites from sources | Open questions and risks |
|---|---|---|---|
| **1. HDMI capture** — **in scope** (owner, 2026-10-07) | Capture the ATEM's HDMI output through the TC358743, like any other HDMI source. Cameras connected directly use the same path (REQ-CAP-008; Section 7.4). | The ATEM Mini Pro HDMI output is 1080p only (23.98–60), 4:2:2 YUV 10-bit, Rec 709 [F-23]. On the Mini Pro it defaults to **multiview**, not program [F-25]. The capture path must carry the ATEM's video standard; 1080p59.94/60 need more than 2 CSI-2 lanes (reasoning from [C-37], [C-48]; Section 7). Both 2-lane and 4-lane configurations are required (REQ-CAP-007), so on a 2-lane configuration the ATEM must be set to a standard that link carries (Section 7.1). An EDID must be loaded before the ATEM sees a sink [A-33]. Program audio arrives embedded in HDMI [F-24]. The TC358743 can send audio over CSI-2 [A-05] or I2S/TDM [A-11]; the Linux driver always configures 2-channel I2S output [A-13]. | OQ-102, OQ-078, OQ-083, OQ-002, OQ-004, OQ-025, OQ-040, OQ-041, OQ-028. RISK-008, RISK-014, RISK-001. |
| **2. Network control / tally (UDP 9910)** — not in current scope | Read tally, recording and streaming state. Optionally control the switcher (aux routing, macros, start/stop record or stream). | The official ATEM Switchers SDK does not support Linux [F-01]. The wire protocol is not in the official SDK manual [F-10]. The OpenSwitcher project reports it as a reverse-engineered protocol on UDP 9910 [F-11]. Reasoning: PACSCORDER therefore needs a third-party library or its own implementation; the libraries reported by their projects are in Section 5 ([F-14]–[F-22], community sources). PACSCORDER also needs IP reachability to the ATEM. | OQ-077, OQ-084, OQ-089. RISK-018. |
| **3a. Receive the ATEM's RTMP stream** — not in current scope | The ATEM publishes its program stream to PACSCORDER, which records or relays it. | The ATEM streams H.264 or H.265 video with AAC audio over RTMP or SRT [F-06], [F-26]. The ATEM is an RTMP client, so PACSCORDER must run a listening RTMP server; GStreamer `rtmp2src` cannot do this (reasoning [F-46], [F-33]). H.265 over RTMP needs Enhanced RTMP [F-31]. AAC is not a required WebRTC codec (RFC 7874 requires Opus and G.711), so relaying to browsers over WebRTC needs the AAC audio transcoded, typically to Opus [F-41]. | OQ-075, OQ-079, OQ-080, OQ-076. See [STREAMING.md](STREAMING.md). |
| **3b. Send PACSCORDER's RTMP stream into an ATEM setup** — not in current scope | PACSCORDER publishes to an ATEM Streaming Bridge or to an ATEM Mini Extreme ISO G2. | The Streaming Bridge documents RTMP input only from Web Presenter, ATEM Mini and ATEM SDI, and SRT only from Web Presenter. Third-party encoders are not documented [F-28] (CORRECTED). Extreme ISO G2 streaming sources are compatible Blackmagic cameras [F-29]. | OQ-081 (VENDOR CONFIRMATION REQUIRED). |

**A fourth path not in REQ-ATEM-001 (not in current scope):** the ATEM Mini's USB-C webcam output [F-27]. The recorded scope is HDMI capture through the TC358743 (REQ-CAP-008; OQ-009 ANSWERED), and this path bypasses the TC358743. Blackmagic documents it for Mac and Windows only, with no statement about Linux or UVC [F-27]. USB capture from switchers goes through the separate DeckLink SDK, which lists Linux [F-09]. Whether it can be used from the candidate platforms is OQ-082.

## 3. Official ATEM Switchers SDK

> **Reference — not in current scope.** The SDK concerns network control, recording and streaming (modes 2 and 3), which the owner did not select on 2026-10-07 (OQ-009 ANSWERED).

| Topic | Fact |
|---|---|
| Host platforms | Windows and macOS only; the manual names no Linux host [F-01]. |
| Latest release | "ATEM Switchers 10.4.1 SDK", 03 Sep 2026, for Mac OS X and Windows only. Download requires registration and acceptance of the terms "bmd-standard-sdk" [F-02]. |
| API model | Modelled on Microsoft COM. On Windows: `CoCreateInstance(CLSID_CBMDSwitcherDiscovery, ..., IID_IBMDSwitcherDiscovery)`. On macOS: `CreateBMDSwitcherDiscoveryInstance()`. Headers: `BMDSwitcherAPI.idl` (Windows), `BMDSwitcherAPI.h` (macOS) [F-03]. |
| Runtime | The runtime libraries ship inside the ATEM product installers, which exist for Mac and Windows only. Applications link dynamically against the library on the user's system [F-04]. |
| Connection | `IBMDSwitcherDiscovery::ConnectTo(deviceAddress, switcherDevice, failReason)` makes a synchronous network connection to a hostname or IP address. If no network connection can be made, it tries USB, but only if the switcher supports USB. An empty `deviceAddress` connects over USB only [F-05] (CORRECTED). |
| Streaming API | `IBMDSwitcherStreamRTMP` streams H.264 or H.265 video with AAC audio over RTMP or SRT. States: Idle, Connecting, Streaming, Stopping. Methods include `SetUrl`, `SetKey`, `CanStreamSRT` and `SetProfileXml` [F-06]. |
| Recording API | `IBMDSwitcherRecordAV` records H.264 + AAC as MP4 to an externally connected disk. States: Idle, Recording, Stopping. Errors include `NoMedia`, `MediaFull` and `DroppingFrames` [F-07]. |
| Tally | `IBMDSwitcherInput::IsProgramTallied(bool*)` and `IsPreviewTallied(bool*)`, with change events [F-08]. |
| USB video | Uncompressed USB 3 capture and H.264 streaming over USB use the separate DeckLink SDK. "Desktop Video 16.0 SDK" (08 Apr 2026) lists Linux [F-09]. |
| Wire protocol | Not documented: a full-text search of the December 2025 manual finds no "9910" or "UDP" [F-10]. |

**Consequences (reasoning):**

- The SDK cannot run on PACSCORDER: it supports neither Linux as a host [F-01] nor a Linux runtime [F-04].
- Its API is still useful as the authoritative list of what a switcher exposes (tally, recording, streaming states and errors). That makes it a reference if the owner later extends REQ-ATEM-001 beyond HDMI capture.
- Whether the "bmd-standard-sdk" terms affect the use of third-party protocol libraries is OQ-089 (relevant only if mode 2 is added).

## 4. Network protocol: UDP 9910 (reverse-engineered)

> **Reference — not in current scope.** This protocol is mode 2, which the owner did not select on 2026-10-07 (OQ-009 ANSWERED). PACSCORDER has no UDP 9910 client in the current scope.

**Apart from [F-10], every fact in this section comes from a community source.** The official ATEM SDK manual does not document this protocol [F-10].

As reported by the OpenSwitcher project:

- ATEM Software Control talks to the switcher over a custom UDP protocol on port 9910.
- The protocol has sequence numbers, retransmissions, acknowledgements and a TCP-like 3-way handshake.
- The project documents it as reverse-engineered, not as an official specification [F-11].

### 4.1 Packet header (as reported by OpenSwitcher [F-12], CORRECTED)

The header is always 12 bytes:

| Order | Field | Width |
|---|---|---|
| 1 | Flags | 5 bits |
| 2 | Packet length, including the 12-byte header | 11 bits |
| 3 | Session ID | 16 bits |
| 4 | Acknowledgement number | 16 bits |
| 5 | Unknown | 16 bits |
| 6 | Remote sequence number | 16 bits |
| 7 | Local sequence number | 16 bits |

Flag bits: 0 = Reliable, 1 = SYN, 2 = Retransmission, 3 = Request retransmission, 4 = ACK [F-12].

The payload is a series of commands. Each command has:

- a 16-bit length, header included;
- 2 padding/unknown bytes;
- a 4-character ASCII name;
- then (length − 8) bytes of data [F-12].

Byte order (endianness) and the bit-numbering convention of the flags field are not stated in the register: NEEDS VERIFICATION before any implementation.

### 4.2 Connection handshake (as reported by OpenSwitcher [F-13])

1. The client sends SYN with payload `01 00 00 00 00 00 00 00`.
2. The switcher replies with SYN and a status byte: `0x02` on success, `0x04` meaning the client must restart.
3. The client sends ACK.
4. The switcher sends its full state, ending with an empty packet.
5. From then on, reliable packets must be ACKed.

Not in the register: session timeouts, keep-alive behaviour, and the maximum number of simultaneous clients (PACSCORDER plus ATEM Software Control plus others). See OQ-077.

### 4.3 Command names reported by the libraries

| Purpose | Command(s) | Reported by |
|---|---|---|
| Tally by source | `TlSr` | atem-connection [F-17]; PyATEMMax [F-20] |
| Tally by index | `TlIn` | PyATEMMax [F-20] |
| Macro action | `MAct` (Run = 0, Stop = 1, StopRecord = 2, InsertUserWait = 3, Continue = 4, Delete = 5) | atem-connection [F-17] |
| Macro recording status | `MRcS` | PyATEMMax [F-20] |
| Aux output routing (set / read) | `CAuS` / `AuxS` | atem-connection [F-17] |
| Recording (set / status) | `RcTM` / `RTMS` | atem-connection [F-16] |
| Streaming (set / status) | `StrR` / `StRS` | atem-connection [F-16] |
| Streaming service (set / read) | `CRSS` (serviceName 64 bytes, url 512 bytes, key 512 bytes) / `SRSU` | atem-connection [F-16] |

As further reported by atem-connection:

- The four recording and streaming status commands require minimum protocol version `V8_1_1` (0x0002001e, protocol 2.30) [F-16].
- The program input appears at state path `video.mixEffects.0.programInput` [F-17].

### 4.4 Protocol-version gap (RISK-018)

- As reported from the atem-connection source, its `ProtocolVersion` enum ends at `V9_6 = 0x00020020` (protocol 2.32). It has no 10.x entry, although Blackmagic has shipped ATEM 10.x software (10.4.1 on 03 Sep 2026) [F-18].
- The project states that it officially targets ATEM firmware v8.0 to latest. It also warns that new firmware will likely require library updates, and it does not support USB control [F-15].
- **Consequence (reasoning):** whether tally, `RTMS` and `StRS` decode correctly on ATEM 10.x firmware is UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-077). This would be part of TEST-ATEM-001 only if the owner adds mode 2; the current TEST-ATEM-001 scope is HDMI capture.
- **Mitigations to consider if mode 2 is added** (Claude's reasoning; not decided):
  - state the supported ATEM firmware versions in the product documentation;
  - re-run TEST-ATEM-001 against every new ATEM firmware release.

## 5. Open-source protocol libraries

> **Reference — not in current scope.** These libraries implement mode 2 (UDP 9910), which the owner did not select on 2026-10-07 (OQ-009 ANSWERED).

All entries are community sources, as reported by each project.

| Library | Language / runtime | Licence | Capabilities (as reported) | Notes |
|---|---|---|---|---|
| **atem-connection** (Sofie, originally NRK) | TypeScript. Requires Node `^14.18 \|\| ^16.14 \|\| >=18.0`. Uses a `udp4` socket with `DEFAULT_PORT = 9910`. | MIT | Recording and streaming status and control [F-16]; tally, macros, aux routing [F-17] | Latest npm release 3.10.3, 2026-10-01; repository `github.com/Sofie-Automation/sofie-atem-connection` [F-14]. Firmware v8.0 to latest; no USB control [F-15]. Protocol enum ends at 2.32 [F-18]. |
| **PyATEMMax** | Python 3 | GPL-3.0 | Tally (`TlIn`, `TlSr`) and macro status (`MRcS`); no recording-status (`RTMS`) or streaming-status (`StRS`) commands found [F-20] | Port of Kasper Skårhøj's ATEMmax Arduino library, `UDPPort = 9910`. Latest release 1.0b9, uploaded 2022-09-16 [F-19]. |
| **pyatem** (OpenSwitcher) | Not stated in the register; its USB support uses pyusb [F-21] | Library LGPL-3.0-only. The `gtk_switcher`, `openswitcher_proxy` and `bmd_setup` programs are GPL-3.0-only. | Network control and USB. OpenSwitcher offers livestream and recording controls for the ATEM Mini series [F-21]. | — |
| **LibAtem** | C#, .NET Core 3.0 | LGPL-3.0 | Its README reports testing on Windows and Linux, including Raspberry Pi; primary support for firmware 8.0+. The README calls it incomplete: some macro operations, much of the audio mixer and camera control are missing [F-22]. | — |

- **Selection** is OQ-084: needed only if the owner adds mode 2. Then OWNER DECISION REQUIRED, and the OQ also needs legal clarification.
- **Packaging.** How each runtime (Node.js, Python, .NET) is packaged in Raspberry Pi OS or Buildroot was not researched: NEEDS VERIFICATION.
- **Licence obligations** are part of OQ-087.

## 6. ATEM video, audio and streaming I/O relevant to PACSCORDER

Sections 6.1 (HDMI output) and 6.2 (audio) belong to mode 1, which is in scope. Sections 6.3 to 6.6 belong to modes 3a and 3b and to the USB-C path; they are reference only and **not in current scope** (owner, 2026-10-07; OQ-009 ANSWERED).

### 6.1 HDMI output

- **ATEM Mini Pro output** [F-23]
  - One HDMI program output.
  - HD output standards: 1080p23.98, 1080p24, 1080p25, 1080p29.97, 1080p30, 1080p50, 1080p59.94 and 1080p60.
  - No 720p, 1080i or Ultra HD output standards.
  - Video sampling: 4:2:2 YUV, 10-bit, Rec 709.
- **Default output source** [F-25]
  - ATEM Mini Pro: **multiview**.
  - ATEM Mini Extreme: program on HDMI out 1, multiview on out 2.
  - The source is changed with the front-panel VIDEO OUT buttons or the "output" menu in ATEM Software Control. Selectable sources include inputs, program, preview and camera 1 direct.
- **Changing the source over the network** (mode 2; not in current scope). atem-connection reports aux routing with `CAuS` [F-17]. No source maps the Mini Pro HDMI output to an aux index, so network control of this setting is UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-078). In the current scope the output source is set by the operator on the ATEM (Section 7.3, step 1).

### 6.2 Audio [F-24] (CORRECTED)

- ATEM Mini Pro and Mini Pro ISO have no dedicated audio output connector ("Total Audio Outputs: None, embedded audio only").
- Mixed program audio is output as embedded digital audio on the HDMI output and the USB-C webcam output. It is also carried in the RTMP/SRT stream and in USB recordings.
- The HDMI *inputs* carry 2-channel embedded audio.
- The channel count and sample rate of the HDMI *output* are not specified for the Mini Pro [F-24]: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-083).

### 6.3 Streaming and recording (reference — not in current scope)

- **ATEM Mini Pro** [F-26]
  - Streams directly over RTMP and SRT, through its Ethernet port (10/100/1000 BaseT) or a shared internet connection over USB-C.
  - Records to USB-C media as `.mp4` H.264 with AAC audio, at the ATEM video standard, on ExFAT or HFS+ media.
- **XML streaming profiles** [F-30]
  - Extra streaming services and low-level encoder settings are configured through an XML file.
  - `IBMDSwitcherStreamRTMP::SetProfileXml` takes a `<profile>` with `<config resolution fps codec>` elements holding `<bitrate>`, `<audio-bitrate>` and `<keyframe-interval>`. The SDK example shows `keyframe-interval` 2 and codec `H264`.
- **Not sourced:** the ATEM's actual H.264 profile and level, B-frame use, SPS/PPS cadence and AAC parameters. These decide whether an ATEM stream can be relayed to browsers without re-encoding (OQ-079, RISK-019).

### 6.4 USB-C webcam output (reference — not in current scope)

- The USB-C port also acts as a webcam output; a connected computer recognises the ATEM Mini as a webcam [F-27].
- Blackmagic documents Mac and Windows use only (Teams, Zoom, OBS) and makes no statement about Linux or UVC [F-27].
- See OQ-082.

### 6.5 ATEM Streaming Bridge [F-28] (CORRECTED) (reference — not in current scope)

- **Function.** Receives an H.264 stream over Ethernet and outputs it on HDMI (1080p23.98–1080p60, 2-channel embedded audio) and 2 × 3G-SDI.
- **Documented input protocols.** RTMP from Web Presenter, ATEM Mini and ATEM SDI; SRT from Web Presenter.
- **Configuration.**
  - The bridge is configured with the ATEM Setup utility over USB, or with its mini switches.
  - The utility exports an XML settings file. Loading that file on the remote ATEM Mini Pro/Extreme makes the bridge appear in that switcher's streaming "platform" menu.
- **Internet links** need TCP port 1935 forwarded to the bridge.
- **Third-party RTMP encoders:** not documented (OQ-081).

### 6.6 ATEM Mini Extreme ISO G2 streaming sources [F-29] (reference — not in current scope)

- It accepts up to 8 streaming sources: RTMP/SRT video with audio from compatible Blackmagic Design cameras set to HyperDeck High/Medium/Low or Streaming High/Medium/Low quality.
- Receiving remote sources over the internet requires forwarding TCP port 1935 to the switcher.
- PACSCORDER as a source: not documented (OQ-081).

## 7. ATEM-specific capture concerns (mode 1 — in scope)

### 7.1 Which ATEM output standards each platform can capture

**Requirement (REQ-CAP-007, DRAFT; owner, 2026-10-07: "i need 2 lane and 4 lane with all frame rate"; OQ-001 ANSWERED).** PACSCORDER must support both a 2-lane and a 4-lane CSI-2 configuration, each capturing every frame rate its link can carry. Recorded interpretation (Claude; the owner may correct it): 1080p60 is required on 4-lane configurations; on a 2-lane configuration the physical limit is 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48]. ADR-004 (OPEN) is now a platform choice per configuration.

Inputs:

- the ATEM Mini Pro output standards (1080p23.98–1080p60) [F-23];
- official limits: 2 lanes give at most 1080p30 RGB888 or 1080p50 YUV422, and 4 lanes on a Compute Module give 1080p60 [C-37];
- 2-lane feasibility at the default 486 MHz link frequency: 1080p50 UYVY needs 85.3% and 1080p60 UYVY needs 102.4% of 2-lane capacity [C-48] (reasoning);
- 4-lane feasibility at the default 486 MHz link frequency: all 1080p30/50/60 UYVY and RGB888 combinations fit on CM4 CAM1, Pi 5 and CM5 by bandwidth [C-49] (reasoning). A 4-lane port is necessary for 1080p60 but not shown sufficient for 1080p60 UYVY: at 972 Mbit/s per lane the driver activates only 3 of the 4 lanes for it [C-47] (reasoning), and capture on 3 of 4 lanes is unproven (OQ-038). The link-frequency choice is ADR-008 (PROPOSED; OQ-099);
- lane counts per platform [C-01], [C-02], [C-04], [C-05].

| Platform (connector) | Lanes | Configuration under REQ-CAP-007 | ATEM 1080p23.98–1080p50 (UYVY) | ATEM 1080p59.94 / 1080p60 |
|---|---|---|---|---|
| Pi 4 Model B | 2 [C-01] | 2-lane candidate (ADR-004, OPEN) | Within the official 2-lane YUV422 limit of 1080p50 [C-37] (reasoning: lower frame rates of the same frame size need less bandwidth) | **Not capturable.** The official 2-lane maximum is 1080p50 YUV422 [C-37]; 1080p60 needs 102.4% of 2-lane capacity [C-48]. Reasoning: 1080p59.94 needs 102.4% × 59.94/60 ≈ 102.3%, which also exceeds it. Under REQ-CAP-007 this is the accepted 2-lane limit, not a reason to exclude the board. |
| CM4 CAM0 | 2 [C-02] | 2-lane candidate | As Pi 4 Model B | As Pi 4 Model B: not capturable |
| Any port with a 2-lane bridge board | 2 (reasoning; the board's lane count is OQ-021) | 2-lane candidate | As Pi 4 Model B [C-48] | As Pi 4 Model B: not capturable [C-48] |
| CM4 CAM1 | 4 [C-02] | 4-lane candidate | Fits [C-49] | Official documentation: 1080p60 in either format on 4 lanes [C-37]; fits by bandwidth [C-49]. In UYVY at 972 Mbit/s only 3 of 4 lanes are active [C-47], and capture on 3 of 4 lanes is unproven (OQ-038) |
| Pi 5 | 4 per port [C-04] | 4-lane candidate | Fits by bandwidth [C-49] | Fits by bandwidth [C-49]; UYVY uses 3 of 4 lanes at 972 Mbit/s [C-47], unproven (OQ-038). The official Raspberry Pi documentation has no Pi 5-specific TC358743 instructions [C-38] (RISK-012). |
| CM5 | 4 per MIPI interface [C-05] | 4-lane candidate | Fits by bandwidth [C-49] | As Pi 5 |

The candidates per configuration are those listed in ADR-004 and REQ-CAP-007; which platform serves which configuration is still OPEN (ADR-004, OQ-011). The PACSCORDER board's actual lane wiring is UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-021). Whether one bridge-board design can serve both configurations is also part of OQ-021.

Reasoning from the table above: the ATEM's 1080p59.94 and 1080p60 standards are captured only on the 4-lane configuration. On a 2-lane configuration, "every frame rate its link can carry" (REQ-CAP-007) covers the ATEM standards 1080p23.98 to 1080p50 in UYVY, so an operator would have to set the ATEM video standard to 1080p50 or lower.

The table assumes UYVY capture (ADR-005, `PROPOSED`; OQ-003). With RGB888 the official 2-lane maximum is 1080p30 [C-37], and 1080p50 RGB888 needs 128% of 2-lane capacity [C-48] (reasoning). On 2 lanes, RGB888 capture would therefore exclude the ATEM's 1080p50 standard as well, leaving 1080p23.98 to 1080p30 (reasoning).

### 7.2 Other capture concerns

| Concern | Facts | Status |
|---|---|---|
| **HDCP** | The driver always disables automatic HDCP authentication on device-tree platforms [A-04]. The public datasheet does not state the chip's HDCP version [A-03]. No register fact states whether the ATEM's HDMI output ever uses HDCP. | UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED. RISK-008; OQ-083, OQ-028. |
| **EDID and hot-plug** | The driver does not assert hot-plug until an EDID has been written [A-33], [B-21]. Userspace must supply the EDID with `VIDIOC_S_EDID`, for example `v4l2-ctl --set-edid` [C-37]. The set-EDID call requires pad 0 [B-22]. `v4l2-ctl` has a built-in EDID type `hdmi`: "CTA-861 with HDMI support up to 1080p60" [B-24]. Whether the ATEM honours the sink EDID, or always outputs its own video standard, is not sourced. | EDID content: OQ-002. ATEM behaviour: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-083). |
| **Interlace** | The ATEM Mini Pro has no 1080i output standard [F-23]. The driver rejects interlaced input [B-27] (reasoning). | Reasoning: RISK-009 does not affect the ATEM Mini Pro's documented output standards [F-23]. Other ATEM models: UNKNOWN — VERIFICATION REQUIRED (OQ-102). Cameras: Section 7.4. |
| **Fractional frame rates** | The ATEM Mini Pro outputs both 1080p59.94 and 1080p60 [F-23]. The driver derives the reported timings from an integer frame rate, so fractional rates such as 59.94 Hz are reported as integer-fps pixel clocks [B-28]. | OQ-040 |
| **Pixel encoding and colour** | The ATEM Mini Pro samples 4:2:2 YUV, 10-bit, Rec 709 [F-23]. The TC358743 accepts RGB and YCbCr 4:4:4 at 24 bpp and YCbCr 4:2:2 at 24 bpp, up to 1080p60 [A-07]. The driver's UYVY output is BT.601 limited range, reported as `SMPTE170M` [B-34]. | The encoding actually received, and the colour metadata to signal: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-083, OQ-041). |
| **TMDS clock** | 1080p60 uses a 148.5 MHz pixel clock [C-46] (reasoning). The TC358743 maximum TMDS clock is 165 MHz [A-07]. | Reasoning: within the bridge's input limit if the ATEM's TMDS clock equals the pixel clock. The relation between the two for the ATEM's 10-bit 4:2:2 output is not in the register: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED (OQ-083). |
| **Multiview default** | The ATEM Mini Pro HDMI output defaults to multiview [F-25]. | Operator step, or network routing (OQ-078). |
| **Audio** | Program audio is embedded only [F-24]. The TC358743 can send audio over CSI-2 [A-05] or I2S/TDM [A-11]. The Linux driver always configures 2-channel I2S output [A-13]. The `tc358743-audio` overlay uses GPIO 18/19/20 [A-47], [G-14]. [A-47] applies to Pi 4/CM4; [G-14] is not read here as confirming Pi 5/CM5 operation (OQ-054). | OQ-004, OQ-025, OQ-054. RISK-014. |

### 7.3 Bring-up check outline for ATEM HDMI capture

This is NOT YET RUN ON PACSCORDER HARDWARE. It is an outline; the full procedure belongs in [TESTING.md](TESTING.md) under TEST-CAP-001 and TEST-ATEM-001.

1. **ATEM output source.** On the ATEM, set the HDMI output source to Program with the VIDEO OUT buttons or the ATEM Software Control "output" menu [F-25].
2. **ATEM video standard.** Set a standard that the lane configuration under test can carry: up to 1080p60 on the 4-lane configuration (1080p60 UYVY on 3 of 4 lanes is unproven, OQ-038), up to 1080p50 (UYVY) on the 2-lane configuration (REQ-CAP-007; Section 7.1) [F-23], [C-37].
3. **EDID.** Load an EDID on TC358743 pad 0, for example `v4l2-ctl --set-edid pad=0,type=hdmi` [B-22], [B-24].
   - On Pi 4/CM4 in Media Controller mode, and always on Pi 5, the call goes to the TC358743 sub-device node [B-25] (CORRECTED). Reasoning: the same applies to CM5, which also uses the RP1 CFE with Media Controller only [G-71] (CORRECTED, reasoning tier).
   - On Pi 4/CM4 in legacy Unicam mode, the video node forwards the EDID and DV-timings ioctls to the TC358743 [B-25]. See ADR-006 (`PROPOSED`).
   - The `v4l2-ctl` option that selects the device node is not quoted in the register: NEEDS VERIFICATION (OQ-101). In particular, the `-d /dev/v4l-subdevN` (sub-device path) form is not attested. The register attests only `-d 11` for the encoder video node [D-17] (community) and a Raspberry Pi engineer's report that the EDID and timings step is run "on /dev/v4l-subdevN" [C-33] (community).
   - Whether the built-in `hdmi` EDID is acceptable for the product is OQ-002.
4. **Timings.** Query and apply the detected timings with `v4l2-ctl --set-dv-bt-timings query` [C-33], [C-37], on the same node as the EDID (step 3) [B-25]. Compare the result with the standard set on the ATEM.
5. **Links and formats.** Configure media links and pad formats as described in [V4L2.md](V4L2.md) and [CSI_PIPELINE.md](CSI_PIPELINE.md). Raspberry Pi engineer 6by9 gave a Pi 5 capture sequence and reported that it captured on kernel 6.18.39 [C-33] (community source). 6.18.39 is only the kernel of that report; the 2026-10-06 Raspberry Pi OS image ships kernel 6.18.50 [G-04]. In the same thread, the reporter found that leaving out `field:none` on the csi2 pads makes `STREAMON` fail with `-EPIPE` [C-33] (community source).

Expected and actual results go in [TESTING.md](TESTING.md).

### 7.4 Cameras connected directly (REQ-CAP-008)

REQ-CAP-008 (DRAFT; owner, 2026-10-07) makes cameras connected directly a second kind of HDMI source, beside the ATEM's HDMI output.

- **Same capture path.** Reasoning: a camera is captured through the same TC358743 path as mode 1 (EDID, DV timings, media graph, capture). No ATEM-specific step applies; in particular, the multiview default and Section 7.3 steps 1 and 2 are ATEM settings [F-25], [F-23].
- **Camera output behaviour is not in the source register.** HDMI output modes, colour formats and HDCP behaviour per camera model are UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (model list); HARDWARE TEST REQUIRED (each listed camera) (OQ-102).
- **Driver limits that apply to any source:**
  - progressive timings only: interlaced input returns `-ERANGE` (reasoning from driver source) [B-27] (RISK-009);
  - DV-timings capability of 640–1920 × 350–1200 and a 13–165 MHz pixel clock [B-26];
  - TMDS clock up to 165 MHz [A-07];
  - HDCP authentication always disabled [A-04] (RISK-008);
  - no sink is seen until an EDID is loaded [A-33];
  - fractional rates are reported as integer-fps pixel clocks [B-28] (OQ-040).
- **Lane configuration.** The 2-lane and 4-lane limits of Section 7.1 apply to cameras in the same way (REQ-CAP-007; reasoning from [C-37], [C-48], [C-49]).
- **Tests.** TEST-CAP-001 and TEST-CAP-004 verify REQ-CAP-008 for each listed source (Section 9).

## 8. Network requirements

> **Not in current scope.** Every ATEM port below belongs to mode 2 or 3, which the owner did not select on 2026-10-07 (OQ-009 ANSWERED). The rows are kept as reference. Reasoning: mode 1 (HDMI capture) uses no network connection to the ATEM. The ports PACSCORDER needs for its own RTMP and WebRTC streaming are in [STREAMING.md](STREAMING.md).

| Port | Transport | Purpose | Source |
|---|---|---|---|
| 9910 | UDP | ATEM control protocol (mode 2; not in current scope), as reported by the OpenSwitcher project [F-11]. The atem-connection and PyATEMMax projects report this port as their default [F-14], [F-19]. | [F-11], [F-14], [F-19] |
| 1935 | TCP | RTMP default port (mode 3a: PACSCORDER as server; mode 3b: PACSCORDER as publisher; both not in current scope) | [F-32] |
| 1935 | TCP | Forwarded to a Streaming Bridge or an Extreme ISO G2 for internet links (ATEM side; mode 3b, not in current scope) | [F-28], [F-29] |

Whether an ATEM will publish to a LAN-only RTMP server without internet access is OQ-080 (mode 3a; not in current scope). The complete port list, including WebRTC, is in [STREAMING.md](STREAMING.md) Section 5 (OQ-075).

## 9. Tests

| Test | Verifies | Status | Depends on |
|---|---|---|---|
| TEST-ATEM-001 ATEM HDMI output capture (scope per OQ-009: mode 1; OQ-009 ANSWERED 2026-10-07) | REQ-ATEM-001, REQ-CAP-008 | `BLOCKED — HARDWARE REQUIRED` | An ATEM test unit (availability UNKNOWN — VERIFICATION REQUIRED, OWNER DECISION REQUIRED; models OQ-102); PACSCORDER hardware |
| TEST-CAP-001 DV timings detection for each source mode (with the ATEM and with directly connected cameras as sources) | REQ-CAP-002, REQ-CAP-008 | `BLOCKED — HARDWARE REQUIRED` | PACSCORDER hardware; source models (OQ-102) |
| TEST-CAP-004 Unsupported-mode rejection and supported-mode matrix (per lane configuration; for each listed ATEM and camera) | REQ-CAP-005, REQ-CAP-007, REQ-CAP-008 | `BLOCKED — HARDWARE REQUIRED` | PACSCORDER hardware; mode list (OQ-002); source models (OQ-102) |
| TEST-AUD-001 HDMI audio capture over I2S (ATEM embedded audio) | REQ-CAP-006 | `BLOCKED — HARDWARE REQUIRED` | OQ-004, OQ-025 |

## Verification status

### Verified from sources (fact IDs)

This document cites 66 register entries, all with verdict `CONFIRMED` or `CORRECTED`:

A-03, A-04, A-05, A-07, A-11, A-13, A-33, A-47, B-21, B-22, B-24, B-25, B-26, B-27, B-28, B-34, C-01, C-02, C-04, C-05, C-33, C-37, C-38, C-46, C-47, C-48, C-49, D-17, F-01, F-02, F-03, F-04, F-05, F-06, F-07, F-08, F-09, F-10, F-11, F-12, F-13, F-14, F-15, F-16, F-17, F-18, F-19, F-20, F-21, F-22, F-23, F-24, F-25, F-26, F-27, F-28, F-29, F-30, F-31, F-32, F-33, F-41, F-46, G-04, G-14, G-71.

- `CORRECTED` entries, used in their corrected wording only: B-21, B-25, F-05, F-12, F-24, F-28, G-71.
- `community` entries, worded as reports: C-33, D-17, F-11, F-12, F-13, F-14, F-15, F-16, F-17, F-18, F-19, F-20, F-21, F-22.
- `reasoning` entries, labelled as reasoning: B-27, C-46, C-47, C-48, C-49, F-46, G-71.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.
- The scope decision (mode 1 in scope; modes 2, 3a and 3b not in current scope) rests on the owner statement of 2026-10-07 recorded in OQ-009, not on a source fact.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-07). No ATEM switcher and no camera has been connected to any PACSCORDER prototype.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against the source register. Corrected the TC358743 audio statements: the datasheet also allows audio over CSI-2 [A-05], and the driver selects I2S [A-13]. Removed the claim that no ATEM test unit exists, which is not recorded. The HDCP and "Blackmagic does not publish" statements now say only what [F-10] and the register support. The TMDS-clock conclusion is now reasoning with its assumption stated. Section 7.1 now states the 486 MHz basis of [C-48]/[C-49] and adds the RGB888 2-lane limit. The CM5 node selection now cites [G-71]. Added the `field:none` report from [C-33]. pyatem's language is no longer asserted. Added a network-interface UNKNOWN row (OQ-018). Normalised the UNKNOWN markers to the README convention. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: Section 7.3 step 3 marks the `-d /dev/v4l-subdevN` device-selection form NEEDS VERIFICATION (OQ-101) and states the attested forms ([D-17], [C-33]); step 5 attributes the Pi 5 sequence to 6by9, replaces "reported it successful" with "reported that it captured", and labels 6.18.39 as the kernel of that report only (shipped 6.18.50 [G-04]); Section 7.1 says a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes [C-47], OQ-038; link frequency ADR-008 / OQ-099); mode 3a WebRTC audio worded as "AAC is not a required WebRTC codec" [F-41]; [G-14] no longer read as confirming Pi 5/CM5 audio (OQ-054); network-interface row linked to OQ-098; verification list updated (65 entries) | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: scope decision recorded prominently (OQ-009 ANSWERED: mode 1 HDMI capture in scope; modes 2 UDP 9910, 3a and 3b RTMP and the USB-C path kept as reference, labelled "not in current scope", in Sections 2–6 and 8); camera sources added (REQ-CAP-008; new Section 7.4); ATEM and camera models moved to OQ-102; Section 7.1 now records REQ-CAP-007 (2-lane and 4-lane configurations, OQ-001 ANSWERED) with a configuration column and a 2-lane-board row instead of "whether 1080p60 is mandatory is OQ-001"; status table, risk row and Section 9 test table (with "Verifies" column, TEST-CAP-004 added) updated; B-26 cited (66 entries). No requirement, decision, risk or test status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording): this document has no ADR-003 or OQ-012 status statement, so none changed; TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)" (ID unchanged, as in README.md) in the §1 status table and the §9 test table. No scope, evidence, ADR status or implementation status changed. | Claude (session 2026-10-07) |
