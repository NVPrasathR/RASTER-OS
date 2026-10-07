# PACSCORDER Requirements

| | |
|---|---|
| Document status | DRAFT — no requirement has been accepted with acceptance criteria yet (OQ-017) |
| Last updated | 2026-10-07 |
| Applies to | The PACSCORDER product on all four candidate platforms: Pi 4 Model B, CM4, Pi 5, CM5 |
| Verification | Requirements only. Supporting facts are from source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)). Nothing has been tested on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Owner | Project owner |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 11, 12, 22 |

This document lists every product requirement with a stable ID (Rule 11). Requirements are never deleted. A requirement that is dropped is marked `WITHDRAWN`, and the reason is written here (Rule 11, Rule 14).

Traceability from each requirement to design, implementation, test and result is kept in [TRACEABILITY.md](TRACEABILITY.md) (Rule 12).

## How to read this document

Each requirement has two independent status fields.

**Acceptance** — where the requirement came from and whether the owner has agreed to it:

| Value | Meaning |
|---|---|
| `ACCEPTED` | The owner has confirmed the wording and acceptance criteria. |
| `DRAFT` | The requirement comes from the owner's engineering rules ([ENGINEERING_RULES.md](ENGINEERING_RULES.md)) or from a dated owner statement quoted in the requirement. The intent is the owner's, but the wording (written by Claude), parameters or acceptance criteria are not yet confirmed. |
| `PROPOSED` | Claude derived it from verified research (cited by fact ID from [REFERENCES.md](REFERENCES.md)). The owner has not agreed to it. |
| `WITHDRAWN` | No longer required. The reason is recorded in the requirement. |

**Implementation status** — Rule 10 vocabulary only:

`NOT STARTED` · `IMPLEMENTED — NOT TESTED` · `TESTED — PASS` · `TESTED — FAIL` · `PARTIALLY VERIFIED` · `BLOCKED — HARDWARE REQUIRED`

As of 2026-10-06 **no hardware exists** (owner, 2026-10-06), so every hardware-dependent requirement is `NOT STARTED` and its test is `BLOCKED — HARDWARE REQUIRED`.

Fact references such as `[C-37]` point to [REFERENCES.md](REFERENCES.md). Open questions such as `OQ-012` point to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Decisions such as `ADR-004` point to [DECISIONS.md](DECISIONS.md).

## Requirement summary

| ID | Title | Acceptance | Implementation | Test |
|---|---|---|---|---|
| REQ-ARCH-001 | Video pipeline architecture | DRAFT | NOT STARTED | TEST-PLT-001, TEST-DMA-001 |
| REQ-PLT-001 | Raspberry Pi platform coverage | DRAFT | NOT STARTED | TEST-PLT-001 |
| REQ-DRV-001 | TC358743 controlled by a Linux kernel driver | DRAFT | NOT STARTED | TEST-HW-001, TEST-DRV-001 |
| REQ-CAP-001 | 1920x1080@60 HDMI capture | DRAFT | NOT STARTED | TEST-CAP-002 |
| REQ-CAP-002 | Capture through V4L2 and Media Controller | DRAFT | NOT STARTED | TEST-CAP-001 |
| REQ-CAP-003 | EDID provisioning at every start | PROPOSED | NOT STARTED | TEST-DRV-002 |
| REQ-CAP-004 | Source change handling | PROPOSED | NOT STARTED | TEST-CAP-003 |
| REQ-CAP-005 | Unsupported input mode handling | PROPOSED | NOT STARTED | TEST-CAP-004 |
| REQ-CAP-006 | HDMI audio capture | DRAFT | NOT STARTED | TEST-AUD-001 |
| REQ-CAP-007 | 2-lane and 4-lane CSI-2 configurations, all frame rates each link carries | DRAFT | NOT STARTED | TEST-CAP-004, TEST-CAP-002 |
| REQ-CAP-008 | HDMI sources: ATEM switchers and cameras | DRAFT | NOT STARTED | TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 |
| REQ-DMA-001 | DMABUF buffer sharing | DRAFT | NOT STARTED | TEST-DMA-001 |
| REQ-ENC-001 | Video encoding | DRAFT | NOT STARTED | TEST-ENC-001 |
| REQ-REC-001 | Recording | DRAFT | NOT STARTED | TEST-REC-001 |
| REQ-STR-001 | RTMP streaming | DRAFT | NOT STARTED | TEST-STR-001 |
| REQ-STR-002 | WebRTC streaming | DRAFT | NOT STARTED | TEST-STR-002 |
| REQ-ATEM-001 | Blackmagic ATEM integration | DRAFT | NOT STARTED | TEST-ATEM-001 |
| REQ-BLD-001 | Reproducible product image build | DRAFT | NOT STARTED | TEST-BLD-001 |
| REQ-BLD-002 | Project-owned product OS image | DRAFT | NOT STARTED | TEST-BLD-001 |
| REQ-PERF-001 | Sustained operation | PROPOSED | NOT STARTED | TEST-PERF-001 |

---

## REQ-ARCH-001 — Video pipeline architecture

**Requirement.** The video path shall follow this architecture:

```text
HDMI source → TC358743 → CSI-2 → Raspberry Pi CSI-2 receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder / RTMP / WebRTC
```

- **Source:** ENGINEERING_RULES.md Rule 5.
- **Acceptance:** DRAFT — the architecture is mandated by the rules; acceptance criteria are not defined.
- **Implementation:** NOT STARTED.
- **Notes:** The receiver and Media Controller layer differ per platform. Pi 4/CM4 use Unicam [C-09]; Pi 5/CM5 use the RP1 CFE, which supports Media Controller only [C-11], [C-29]. See [ARCHITECTURE.md](ARCHITECTURE.md).

## REQ-PLT-001 — Raspberry Pi platform coverage

**Requirement.** The project shall document and support configuration for Raspberry Pi 4 Model B, Compute Module 4, Raspberry Pi 5 and Compute Module 5.

- **Source:** ENGINEERING_RULES.md Rule 6 (Pi 4 / CM4 / Pi 5 / CM5 configurations). Owner, 2026-10-06: the product target platform is **undecided — keep all four** (OQ-011).
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:** The platforms are not equivalent. Pi 4 Model B has a 2-lane camera connector [C-01], which cannot carry 1080p60 from the TC358743 (official documentation [C-37]; reasoning-tier lane calculation [C-48]). It therefore cannot serve the 4-lane configuration that 1080p60 (REQ-CAP-001) needs; under the owner decision of 2026-10-07 it remains a candidate for the 2-lane configuration (REQ-CAP-007). The product platform choice is an open decision, made per lane configuration: ADR-004.

## REQ-DRV-001 — TC358743 controlled by a Linux kernel driver

**Requirement.** The TC358743 shall be configured and monitored by a Linux kernel driver that exposes it as a V4L2 sub-device in the Media Controller graph. Userspace shall not access the TC358743 or the CSI-2 receiver registers directly.

- **Source:** ENGINEERING_RULES.md Rule 5 (V4L2 / Media Controller in the pipeline), Rule 7 (driver documentation), Rule 13 example decision (use V4L2 instead of direct userspace access). Recorded as ADR-001.
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:** An in-tree driver exists (`drivers/media/i2c/tc358743.c`). It is identical in mainline Linux and in the Raspberry Pi kernel branch `rpi-6.18.y` [A-48], [B-02]. Whether PACSCORDER uses it as-is, patches it, or replaces it is ADR-002 (PROPOSED: use it as-is).

## REQ-CAP-001 — 1920x1080@60 HDMI capture

**Requirement.** The system shall capture HDMI input through the TC358743 at 1920x1080@60Hz.

- **Source:** ENGINEERING_RULES.md Rule 11 (example requirement text, used here as the owner's stated intent).
- **Acceptance:** DRAFT. The owner must confirm the following:
  - whether 1080p60 is mandatory at all (OQ-001) — **answered by the owner on 2026-10-07:** "i need 2 lane and 4 lane with all frame rate". Recorded interpretation: 1080p60 is required on 4-lane configurations; 2-lane configurations capture every rate their link can carry (REQ-CAP-007);
  - whether 60 Hz is mandatory or 59.94 Hz is also required (OQ-002, OQ-040);
  - whether 50 Hz is required (OQ-002);
  - which pixel format is acceptable (OQ-003, ADR-005);
  - whether audio is part of this requirement (see REQ-CAP-006, OQ-004).
- **Implementation:** NOT STARTED. Test status: BLOCKED — HARDWARE REQUIRED.
- **Verified constraints (from sources, not hardware):**
  - Reasoning from the driver's lane formula (reasoning-tier entries; A-26 CORRECTED): 1080p60 needs more than 2 CSI-2 lanes in both formats the driver supports. UYVY needs 3 lanes and RGB888 needs 4 at the overlay default of 972 Mbps per lane [A-26], [C-47], [C-48].
  - Official Raspberry Pi documentation states that 2 lanes give at most 1080p30 RGB888 or 1080p50 YUV422, and that 4 lanes on a Compute Module give 1080p60 [C-37].
  - 4-lane connectors exist on CM4 CAM1 [C-02], on both Pi 5 ports [C-04] and on CM5 MIPI0/1 [C-05]. **Pi 4 Model B cannot meet this requirement** [C-01]; under the owner decision of 2026-10-07 it remains a candidate for the 2-lane configuration (REQ-CAP-007).
  - A 4-lane port is necessary but not shown to be sufficient. Reasoning from the driver's lane formula at 972 Mbps: 1080p60 UYVY activates 3 of the 4 configured lanes [C-47]. The receivers accept 3 of 4 lanes at stream start [C-16], but capture with 3 of 4 lanes is unproven (OQ-038), and corruption near lane-count limits has been reported in a community issue (RISK-006) [C-43]. The bridge board's own lane count is UNKNOWN (OQ-021). The link frequency is ADR-008 (PROPOSED; OQ-099), which proposes keeping the 486 MHz default and evaluating 297 MHz on a CM4 CAM1 4-lane link, where 1080p60 UYVY uses all 4 lanes, in TEST-CAP-002.
  - The Pi 4/CM4 hardware H.264 encoder is officially specified for 1080p30 encode [D-10], so 1080p60 *encode* is a separate unresolved risk (REQ-ENC-001, RISK-002).

## REQ-CAP-002 — Capture through V4L2 and Media Controller

**Requirement.** Captured frames shall be obtained from a V4L2 video capture device fed by the platform CSI-2 receiver driver. Formats and links shall be configured through the V4L2 and Media Controller APIs.

- **Source:** ENGINEERING_RULES.md Rule 5.
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:**
  - libcamera cannot be used for the TC358743: the bridge is not a raw camera sensor and exposes no link-frequency or pixel-rate control [B-16]. Raspberry Pi engineers reported (community source) that libcamera does not support it [C-41].
  - Pi 4/CM4 can run Unicam in legacy video-node mode (the overlay default) or in Media Controller mode [B-43], [C-10]. Pi 5/CM5 support Media Controller mode only [C-11]. The mode choice is ADR-006.

## REQ-CAP-003 — EDID provisioning at every start

**Requirement (PROPOSED).** The system shall write a project-defined EDID to the TC358743 after every boot and every driver load, before capture is expected. The EDID shall advertise only modes the configured CSI-2 link can carry.

- **Source:** Derived by Claude from verified facts.
  - The driver never asserts HDMI hot-plug until an EDID has been written [A-33], [B-21] (B-21 CORRECTED).
  - After every probe the driver has no EDID stored (`edid_blocks_written == 0`) [B-21], and the EDID is held in the chip's embedded SRAM [A-31]. Reasoning: an EDID must therefore be written after every boot and every driver load. Research also found no default EDID and no Device Tree property for one in the driver (research gap, topic B — not a register fact).
  - The TMDS clock is limited to 165 MHz [A-07], [B-26].
- **Acceptance:** PROPOSED. Without this, no HDMI source will output video to the device.
- **Implementation:** NOT STARTED.
- **Open:** EDID contents are an open decision: OQ-002 (OWNER DECISION REQUIRED). The HPD state between power-on and the first EDID write is OQ-032. What triggers the EDID write and in what order relative to overlay, module and media-graph setup is OQ-093.

## REQ-CAP-004 — Source change handling

**Requirement (PROPOSED).** The system shall detect the following and reconfigure the capture pipeline without a reboot:

- an HDMI source being connected or disconnected;
- a loss of signal;
- a change in input timing.

- **Source:** Derived by Claude from verified facts.
  - The driver emits `V4L2_EVENT_SOURCE_CHANGE` but does not apply new timings itself [B-39].
  - Stored timings are cleared when +5V is lost [B-23].
  - On Pi 5 an open issue reports that the CFE video nodes do not deliver the event, and a Raspberry Pi engineer advised subscribing on the TC358743 sub-device node instead (community source) [C-42]; OQ-051.
  - Without a wired interrupt, the driver polls every 1000 ms [A-30], [B-19].
- **Acceptance:** PROPOSED.
- **Implementation:** NOT STARTED.

## REQ-CAP-005 — Unsupported input mode handling

**Requirement (PROPOSED).** When the HDMI source sends a mode the pipeline cannot carry, the system shall report it as unsupported and shall not stream corrupted video. Such modes include:

- interlaced video;
- a pixel clock above 165 MHz;
- a mode that needs more CSI-2 lanes than are wired.

- **Source:** Derived by Claude from verified facts.
  - Interlaced input is rejected with `-ERANGE` (reasoning from driver source) [B-27].
  - The DV timings capability is 640–1920 × 350–1200 and 13–165 MHz [A-08].
  - Receivers fail STREAMON when the bridge requests more lanes than are configured in the Device Tree [B-32], [C-16]. Note: the receivers compare against the Device Tree `data-lanes` value, not the physical wiring. The DT value must therefore match the lanes the board actually routes (OQ-021); a DT value larger than the wiring is not caught by this check ([CSI_PIPELINE.md](CSI_PIPELINE.md) §5).
  - Image corruption has been reported near lane-count boundaries in a community issue [C-43].
- **Acceptance:** PROPOSED.
- **Implementation:** NOT STARTED.

## REQ-CAP-006 — HDMI audio capture

**Requirement.** The system shall capture the embedded HDMI audio of the source from the TC358743 and include it in recordings and streams.

- **Owner decision, 2026-10-07:** audio is required ("Yes, audio required", answer to OQ-004). Acceptance changed from PROPOSED to DRAFT. The original PROPOSED wording (2026-10-06) was: "If the product records or streams audio, embedded HDMI audio shall be captured from the TC358743."

- **Source:** Derived by Claude.
  - RTMP/FLV carries AAC audio [F-31].
  - ATEM program audio is available only embedded in HDMI/USB [F-24] (CORRECTED).
  - The TC358743 silicon can send audio over CSI-2 [A-05] or on I2S/TDM output pins [A-11]. The Linux driver always configures 2-channel I2S [A-13], and the `tc358743-audio` overlay expects that I2S on GPIO 18/19/20 [A-47]. Reasoning: with the stock driver, audio therefore needs I2S wiring that is separate from the CSI-2 camera cable (OQ-025). Whether audio over CSI-2 works with the Raspberry Pi receivers is OQ-033.
- **Acceptance:** DRAFT. Channels, sample rates and the audio/video synchronisation tolerance are not yet specified (OQ-004 answered for "required"; parameters remain open there and in OQ-025, OQ-054, OQ-063).
- **Implementation:** NOT STARTED.

## REQ-CAP-007 — 2-lane and 4-lane CSI-2 configurations, all frame rates each link carries

**Requirement.** The system shall support TC358743 capture over both a 2-lane and a 4-lane CSI-2 link. On each configuration it shall capture every HDMI input frame rate that the configured link can carry.

- **Source:** Owner statement, 2026-10-07: "i need 2 lane and 4 lane with all frame rate" (answer to OQ-001). Wording by Claude.
- **Acceptance:** DRAFT. Acceptance criteria are the supported-mode matrix for each lane count, which is not yet defined (OQ-002). Fractional rates (23.98 / 29.97 / 59.94 Hz) are part of "all frame rates" unless the owner says otherwise; the driver reports integer frame rates [B-28] (OQ-040).
- **Implementation:** NOT STARTED. Test status: BLOCKED — HARDWARE REQUIRED.
- **Verified constraints (from sources, not hardware):**
  - Physical limit: a 2-lane link cannot carry 1080p60 in either supported format. Official Raspberry Pi documentation gives at most 1080p30 RGB888 or 1080p50 YUV422 on 2 lanes [C-37]. Reasoning at the default 972 Mbit/s per lane: 1080p30 UYVY and RGB888 and 1080p50 UYVY fit; 1080p50 RGB888 and 1080p60 in either format do not [C-48]. "All frame rates" on a 2-lane configuration therefore means all rates up to that limit.
  - On 4 lanes, all 1080p30/50/60 × UYVY/RGB888 combinations fit by bandwidth (reasoning) [C-49]. 1080p60 UYVY then uses 3 of the 4 lanes (reasoning) [C-47], which is unproven (OQ-038; ADR-008).
  - Lower resolutions need fewer lanes. Reasoning at 972 Mbit/s: 720p60 needs 1 lane in UYVY and 2 in RGB888 [B-33].
  - 2-lane configurations: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port (reasoning [C-48]; board lane count OQ-021). 4-lane configurations: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05].
  - The `4lane` overlay parameter switches `data-lanes` to `<1 2 3 4>` [B-42]. On the 2-lane ports (Pi 4 Model B, CM4 CAM0; both Unicam) that does not fail the probe: Unicam logs a message and adopts the endpoint's lane count [C-17]. The configuration must therefore match the wiring ([DEVICE_TREE.md](DEVICE_TREE.md)).

## REQ-CAP-008 — HDMI sources: ATEM switchers and cameras

**Requirement.** The system shall capture HDMI video both from the HDMI output of Blackmagic ATEM switchers and directly from cameras.

- **Source:** Owner statement, 2026-10-07: "it can be atem and direct video from camera" (answer to OQ-009). Wording by Claude.
- **Acceptance:** DRAFT. Sources (owner decision, 2026-10-07; OQ-102 ANSWERED): **any HDMI camera, no model list** ("Any HDMI camera (generic)"), plus ATEM switcher outputs (OQ-009). What is accepted is defined by the supported-mode matrix and the EDID (OQ-002), not by a model list.
- **Implementation:** NOT STARTED. Test status: BLOCKED — HARDWARE REQUIRED.
- **Notes (from sources, not hardware):**
  - ATEM Mini Pro HDMI output standards are 1080p23.98 to 1080p60, with no 720p or 1080i; video is 4:2:2 YUV, 10-bit, Rec 709 [F-23]. Its HDMI output defaults to multiview, not program [F-25].
  - Camera HDMI output modes are UNKNOWN — VERIFICATION REQUIRED per camera model (OQ-102). Reasoning from driver source: the driver accepts progressive timings only; interlaced input returns `-ERANGE` [B-27] (RISK-009). The TMDS clock limit is 165 MHz [A-07]. The driver disables HDCP [A-04] (RISK-008).

## REQ-DMA-001 — DMABUF buffer sharing

**Requirement.** Video frames shall pass from capture to encoder as DMABUF buffers without a CPU copy, where the platform encoder supports DMABUF import.

- **Source:** ENGINEERING_RULES.md Rule 5 ("DMABUF" stage).
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:** (The cited facts are `CONFIRMED` in [REFERENCES.md](REFERENCES.md).)
  - The Pi 4/CM4 encoder queues support DMABUF import [D-19]; an imported buffer must be a single DMA-contiguous region at least as large as the plane [D-20]. Whether Unicam capture buffers meet that without a copy is OQ-058.
  - On Pi 5/CM5 encoding runs in software, so "zero-copy into the encoder" has a different meaning there [D-30], [G-22] (OQ-060).
  - See [DMA.md](DMA.md).

## REQ-ENC-001 — Video encoding

**Requirement.** Captured video shall be compressed by an encoder before recording and streaming.

- **Source:** ENGINEERING_RULES.md Rule 5 ("Encoder" stage).
- **Codecs (owner decision, 2026-10-07):** H.264 **and** H.265 (HEVC) for recording and streaming ("H.264 + H.265 (HEVC)", answer to OQ-005). Which outputs use which codec is OQ-103.
- **Acceptance:** DRAFT. The following are still **UNDEFINED — owner to specify** (OQ-005):
  - bitrate;
  - latency target;
  - how many simultaneous encodes are needed.
- **Implementation:** NOT STARTED.
- **Notes:**
  - Pi 4/CM4 have a hardware H.264 encoder, officially specified for 1080p30 [D-10].
  - Pi 5/CM5 have no hardware video encoder. Official figure: "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22].
  - No candidate platform has a hardware HEVC encoder [D-24], [D-31], so H.265 is software-encoded on every candidate (reasoning; RISK-022). Legacy RTMP/FLV carries only H.264 video; HEVC needs Enhanced RTMP [F-31]. WebRTC mandates only VP8 and H.264 [F-36].
  - See [VIDEO_ENCODER.md](VIDEO_ENCODER.md).

## REQ-REC-001 — Recording

**Requirement.** The system shall record encoded video to local storage.

- **Source:** ENGINEERING_RULES.md Rule 5 ("Recorder").
- **Acceptance:** DRAFT. The following are **UNDEFINED — owner to specify** (OQ-006):
  - container format;
  - storage medium;
  - minimum recording duration;
  - behaviour on power loss.
- **Implementation:** NOT STARTED.

## REQ-STR-001 — RTMP streaming

**Requirement.** The system shall publish encoded video, and audio if required, to an RTMP server.

- **Source:** ENGINEERING_RULES.md Rule 5 ("RTMP").
- **Acceptance:** DRAFT. Server targets and bitrate are **UNDEFINED** (OQ-007).
- **Implementation:** NOT STARTED.
- **Notes:** Legacy RTMP/FLV carries H.264 video and AAC audio [F-31]. GStreamer `rtmp2sink` is a client (publisher) [F-33].

## REQ-STR-002 — WebRTC streaming

**Requirement.** The system shall make live video available to WebRTC clients.

- **Source:** ENGINEERING_RULES.md Rule 5 ("WebRTC").
- **Acceptance:** DRAFT. The following are **UNDEFINED** (OQ-008):
  - target browsers;
  - latency target;
  - whether viewers are on the LAN only or over the internet (NAT traversal).
- **Implementation:** NOT STARTED.
- **Notes:**
  - Browser interoperability requires H.264 Constrained Baseline per RFC 7742 [F-36]. RFC 7874 requires WebRTC endpoints to implement Opus and G.711 audio; AAC is not a required WebRTC codec, so AAC audio has to be transcoded (typically to Opus) for browser playback [F-41].
  - Reasoning from H.264 Table A-1 as encoded in FFmpeg: a 1080p H.264 stream needs Level 4.0 or above (1080p60 needs Level 4.2), which a strict `42e01f` (Level 3.1) negotiation does not cover [F-40] (OQ-073).

## REQ-ATEM-001 — Blackmagic ATEM integration

**Requirement.** The system shall integrate with Blackmagic Design ATEM switchers.

- **Source:** ENGINEERING_RULES.md Rule 2 (`ATEM.md`), Rule 11 (`REQ-ATEM-001` listed).
- **Scope (owner, 2026-10-07; OQ-009 ANSWERED):** "it can be atem and direct video from camera". Recorded interpretation: the integration is **option 1 below** — PACSCORDER captures the ATEM's HDMI output (REQ-CAP-008). Options 2 and 3 were offered and not selected, so they are **not in scope** unless the owner adds them. Supported ATEM models: OQ-102.
- **Acceptance:** DRAFT. The researched options were:
  1. capture the ATEM HDMI output through the TC358743 [F-23], [F-25];
  2. read tally and record/stream state, or control the switcher, over the UDP 9910 protocol, which the official SDK manual does not document [F-10] and which the OpenSwitcher project reports as reverse-engineered (community source) [F-11];
  3. exchange RTMP streams [F-26]; reasoning: to receive an ATEM's RTMP push, PACSCORDER must run a listening RTMP server [F-46].
- **Implementation:** NOT STARTED.
- **Notes:** The official ATEM SDK supports Windows and macOS only, not Linux [F-01], [F-02].

## REQ-BLD-001 — Reproducible product image build

**Requirement.** The product image shall be built from this repository by a documented, repeatable procedure that pins kernel, firmware and package versions.

- **Source:** ENGINEERING_RULES.md Rule 17 ("Build"), Rule 25 ("How is the system built?").
- **Acceptance:** DRAFT. The OS / build basis is decided: ADR-003 ACCEPTED on 2026-10-07 (OQ-012 ANSWERED) — own OS image built with `rpi-image-gen`, with a package mirror for reproducibility. Acceptance criteria for "reproducible" are not yet defined.
- **Implementation:** NOT STARTED.

## REQ-BLD-002 — Project-owned product OS image

**Requirement.** The product shall run a project-built OS image of its own, not an unmodified stock distribution image.

- **Source:** Owner statement, 2026-10-07: "which is best i need by own one" (answer to the OS/build question, OQ-012). Wording by Claude.
- **Acceptance:** DRAFT. The build tool is decided: ADR-003 ACCEPTED on 2026-10-07 (owner: "accept ADR-003") — the image is built with `rpi-image-gen` from Raspberry Pi OS packages; Buildroot remains the documented alternative.
- **Implementation:** NOT STARTED.

## REQ-PERF-001 — Sustained operation

**Requirement (PROPOSED).** The system shall capture, encode and record or stream continuously for a duration to be specified by the owner, without frame loss beyond a threshold also to be specified, across the product's operating temperature range.

- **Source:** Derived by Claude.
  - The TC358743 is rated −30 to +70 °C ambient [A-41].
  - Its FIFO trigger level (374) is hard-coded in the driver [B-12]; a Raspberry Pi engineer reported that the value is empirical (community source) [A-43]. The driver has D-PHY timing tables only for 594 and 972 Mbps [A-24], [B-09]. Their validity across modes and temperature is OQ-035.
  - Pi 5 has no hardware video encoder; the official figure for software H.264 1080p30 encode is ~30–40 % CPU [G-22] (OQ-059).
- **Acceptance:** PROPOSED. Duration, drop threshold and temperature range are **UNDEFINED** (OQ-010).
- **Implementation:** NOT STARTED.

---

## Verification status

### Verified from sources (fact IDs)

Every fact cited above is in [REFERENCES.md](REFERENCES.md) with verdict `CONFIRMED` or `CORRECTED`; `CORRECTED` entries (A-26, B-21, F-24) are used in their corrected wording. Community-tier entries (A-43, C-41, C-42, C-43, F-11) are worded as reports. Reasoning-tier entries (A-26, B-27, B-33, C-47, C-48, C-49, F-40, F-46) are labelled as reasoning. "Verified from sources" means only that the cited source says so; it says nothing about PACSCORDER hardware.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No requirement has a test result; every test listed above is `BLOCKED — HARDWARE REQUIRED` except TEST-BLD-001, which is `NOT STARTED`.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Document created. Rule-derived requirements entered as DRAFT; research-derived requirements entered as PROPOSED. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document review. No requirement wording, acceptance or status changed. Changes: header rows "Applies to" and "Verification" added; owner-to-specify items linked to their OQ IDs (OQ-001 to OQ-012, OQ-017, OQ-040); REQ-CAP-003 "EDID RAM is volatile [B-22]" replaced by what [B-21] and [A-31] support, and the vague "OQ list" replaced by OQ-002 / OQ-032; REQ-CAP-006 audio note corrected (the silicon can also send audio over CSI-2 [A-05]; I2S is the driver's choice [A-13]); REQ-STR-002 AAC wording aligned with [F-41] ("not a *required* WebRTC codec"); REQ-DMA-001 stale "still being verified" note removed; community-tier facts worded as reports; DT-versus-wiring lane note added to REQ-CAP-005; 3-of-4-lane caveat (OQ-038) added to REQ-CAP-001; REQ-PERF-001 FIFO/D-PHY sources corrected; "Verification status" section added. Cross-document consistency fixes (second pass, same date; no requirement wording, acceptance or status changed): REQ-CAP-001 4-lane note linked to ADR-008 / OQ-099; REQ-CAP-003 linked to OQ-093 (EDID provisioning trigger and ordering); REQ-PLT-001 [C-48] labelled as reasoning. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions recorded: OQ-001 answer added to REQ-CAP-001; REQ-ATEM-001 scope recorded (OQ-009); new DRAFT requirements REQ-CAP-007 (2-lane and 4-lane, all frame rates each link carries), REQ-CAP-008 (ATEM and camera HDMI sources) and REQ-BLD-002 (own OS image). DRAFT definition extended to dated owner statements. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated (verification pass): REQ-PLT-001 note no longer says Pi 4 Model B simply "conflicts with REQ-CAP-001" — it cannot serve the 4-lane configuration and remains a 2-lane candidate (REQ-CAP-007); REQ-CAP-007 citations corrected — the 3-of-4-lane statement now cites [C-47] (not only [C-49]), the "2-lane bridge board on any port" reasoning cites [C-48], and the `4lane` probe note is scoped to the Unicam 2-lane ports with [B-42] for what `4lane` sets; REQ-CAP-008 [B-27] labelled as reasoning; Verification status reasoning-tier list adds B-33 and C-49. No requirement status, acceptance or ID changed. | Claude (session 2026-10-07) |
| 2026-10-07 | REQ-BLD-001 and REQ-BLD-002 notes updated: ADR-003 ACCEPTED by the owner (OQ-012 ANSWERED). No requirement wording or acceptance status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions recorded: REQ-CAP-006 audio required (PROPOSED → DRAFT; original wording kept in the entry); REQ-ENC-001 codecs H.264 and H.265 (OQ-103, RISK-022); REQ-CAP-008 sources generic (OQ-102 answered). Counts now 16 DRAFT / 4 PROPOSED. | Claude (session 2026-10-07) |
