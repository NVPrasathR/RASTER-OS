# PACSCORDER Software Architecture (userspace)

| | |
|---|---|
| Document status | DRAFT. Lists responsibilities only. **Nothing is implemented.** The userspace media framework is OPEN (ADR-007). |
| Last updated | 2026-10-09 |
| Applies to | All PACSCORDER userspace software (everything above the kernel drivers) on the four candidate platforms: Raspberry Pi 4 Model B, CM4, Raspberry Pi 5, CM5. Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07, second answer; ADR-004 still OPEN). |
| Verification | Verified from sources only: the source research of 2026-10-06 and of 2026-10-08 (topic H, H.265/HEVC; topic I, HDMI audio; topic J, recording storage and power loss; topic K, live latency) in [REFERENCES.md](REFERENCES.md). Nothing has been verified on PACSCORDER hardware; no hardware and no code exist as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 5, Rule 10, Rule 12, Rule 13, Rule 22, Rule 24, Rule 25 |

This document lists the responsibilities that **any** PACSCORDER userspace implementation must cover. Each one is traced to its requirements in [REQUIREMENTS.md](REQUIREMENTS.md) and to source facts in [REFERENCES.md](REFERENCES.md).

It is **not a design.** None of the following has been chosen:

- process layout (OQ-090);
- thread model (OQ-090);
- IPC mechanism (OQ-090);
- programming language;
- library or framework (ADR-007, OQ-015).

Tools and libraries named below come from the sources and are labelled **CANDIDATE**. They are evidence for open decisions, not decisions.

The kernel-side pipeline these responsibilities sit on is described in [ARCHITECTURE.md](ARCHITECTURE.md).

> **Current state (owner, 2026-10-06):**
> - No hardware exists.
> - No code exists.
> - Nothing has been tested.
> - The target platform is undecided (ADR-004).
> - The media framework is undecided (ADR-007).
>
> Every responsibility below is `NOT STARTED`; every hardware-dependent test is `BLOCKED — HARDWARE REQUIRED`, and TEST-BLD-001 is `NOT STARTED` ([TESTING.md](TESTING.md)).

> **Owner decisions of 2026-10-07** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md)) that change this document:
> - **ATEM scope reduced** (OQ-009 ANSWERED). The ATEM integration is HDMI capture of the ATEM output only, and cameras may be connected directly (REQ-CAP-008, DRAFT; models OQ-102). There is **no UDP 9910 service** and no RTMP server for the ATEM in the current scope (section 5.7). *(Models: superseded by the second set below — OQ-102 ANSWERED, no model list.)*
> - **Two lane configurations** (OQ-001 ANSWERED). Both 2-lane and 4-lane CSI-2 configurations are required, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT). Provisioning, capture control and configuration must therefore know which lane configuration a unit has (sections 5.1, 5.2, 5.9).
> - **Own OS image.** The product runs its own project-built OS image (REQ-BLD-002, DRAFT). The build tool is ADR-003 (ACCEPTED 2026-10-07: `rpi-image-gen`; OQ-012 ANSWERED) (section 5.11).

> **Owner decisions of 2026-10-07, second set** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) ADR-004, [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) and [RISKS.md](RISKS.md); propagated here on 2026-10-08):
> - **Bring-up boards: CM4 and CM5 side by side.** ADR-004 stays OPEN until TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 have been run on both (OQ-011). Every responsibility below must therefore work on both a hardware-H.264 board (CM4) and a software-only board (CM5).
> - **HDMI audio is required** (REQ-CAP-006, DRAFT; OQ-004 ANSWERED). Audio capture (5.8) is no longer conditional. It adds a rate-tracking responsibility (RISK-023, OQ-111) and an A/V synchronisation responsibility (RISK-024, OQ-112).
> - **Codecs: H.264 and H.265** for recording and streaming (REQ-ENC-001). Which output uses which codec is OQ-103. H.265 is software-encoded on every candidate (RISK-022); HEVC over RTMP is not possible with the distribution's GStreamer `flvmux` (RISK-025, OQ-107). Sections 5.3–5.6. *(Superseded by the owner decision of 2026-10-08 below: H.264 only; H.265 deferred, REQ-ENC-002.)*
> - **Sources: any HDMI camera, no model list**, plus ATEM outputs (REQ-CAP-008; OQ-102 ANSWERED). Capture control (5.2) accepts what the supported-mode matrix and EDID allow (OQ-002).

> **Owner decision of 2026-10-08** (answer to OQ-103; recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md), [RISKS.md](RISKS.md) and [DECISIONS.md](DECISIONS.md) ADR-007): **"H.264 only for now."**
> - The recorder (5.4), RTMP publisher (5.5) and WebRTC server (5.6) all take H.264 from 5.3 (REQ-ENC-001). No responsibility encodes H.265 in the current scope.
> - H.265 is deferred as REQ-ENC-002 (acceptance DEFERRED; no planned tests). The H.265 material in sections 5.3–5.6, 5.9, 5.10, 6 and 7 is kept as its evidence and labelled "deferred — REQ-ENC-002; not in current scope".
> - RISK-022, RISK-025 and OQ-104 to OQ-109 stay OPEN but are not in current scope. ADR-007 stays OPEN; its scope note says the HEVC-over-RTMP constraint does not drive the framework choice for the current scope.
> - Unchanged: CM4 + CM5 side-by-side bring-up, HDMI audio required, any HDMI camera, ADR-003 ACCEPTED.

> **Owner decision of 2026-10-08, second** (answer to OQ-005; recorded in [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-001, [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005 and OQ-115, and [RISKS.md](RISKS.md) RISK-002 and RISK-003): **"Separate record + live."**
> - **Two H.264 encodes at the same time.** The capture → encode pipeline (5.3) produces them from one capture: a recording encode for the recorder (5.4), and one live encode shared by the RTMP publisher (5.5) and the WebRTC server (5.6). Bitrate, rate control and latency are still open (OQ-005). *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — the latency target is under 1 s camera-to-viewer for WebRTC viewers only, and RTMP is best-effort (OQ-116 ANSWERED). Bitrate and rate control are still open; OQ-005 stays OPEN.)*
> - **CM4.** Whether the hardware encoder runs both at once is UNKNOWN — VERIFICATION REQUIRED (OQ-115; RISK-002). Reasoning: two 1080p30 encodes need the macroblock rate of one 1080p60 encode, about 2.0× the 1080p30 specification [D-10], [D-52].
> - **CM5.** Both encodes run in software (OQ-059; RISK-003). Reasoning from [G-22]: that roughly doubles the encode CPU load.
> - **The live encode carries the WebRTC constraints of 5.6:** Constrained Baseline [F-36], a level that covers 1080p (reasoning [F-40]; libwebrtc default Constrained Baseline Level 3.1 [F-39]), no B-frames (reported by the MediaMTX project [F-45]), SPS/PPS in-band [F-36] (off by default on the CM4 encoder [D-15]). RTMP receives the same stream.
> - **Audio is still two encodes:** AAC for 5.4 and 5.5, Opus for 5.6 (5.8.1 item 5).
> - **H.265 stays deferred** (REQ-ENC-002). No ADR status changed.
> - **Sections changed:** 2, 3, 5.3–5.6, 5.8.1, 5.9, 5.10, 6, 7, 8.

> **Source research of 2026-10-08** (topics H and I in [REFERENCES.md](REFERENCES.md); research gaps in [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json)) is added to sections 1, 4, 5.2–5.10, 6 and 7. New registers: OQ-104 to OQ-114, RISK-023 to RISK-025. Its H.265 parts (topic H) are deferred with REQ-ENC-002 since the owner decision of 2026-10-08 above.

> **Owner decisions of 2026-10-08, third** (recording storage and live latency; recorded in [DECISIONS.md](DECISIONS.md) ADR-009, [REQUIREMENTS.md](REQUIREMENTS.md) REQ-REC-001, REQ-STR-001 and REQ-STR-002, and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005, OQ-006 and OQ-116; propagated here 2026-10-09):
> - **Live latency.** Under 1 s camera-to-viewer for **WebRTC viewers only** (REQ-STR-002; OQ-116 ANSWERED). RTMP outputs are best-effort; their latency is set by the receiving platform (REQ-STR-001). Bitrate and rate control are still open (OQ-005 stays OPEN). Sections 5.3, 5.5, 5.6.1.
> - **Recording (ADR-009, ACCEPTED 2026-10-08).** MP4 written **fragmented** for power-loss safety; every recording **mirrored** to a PCIe NVMe SSD and a USB-to-SATA HDD; the HDD in a **self-powered** enclosure. ext4 on the recording volumes is Claude's proposal inside ADR-009, **not an owner decision** (OQ-120). The maximum recording duration is still open (OQ-006). *(Superseded 2026-10-09: until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED); behaviour when one mirrored drive fills or fails first, and file splitting: OQ-129.)* Section 5.4.1.
> - **Consequence for the components.** The recorder (5.4) runs two file writers per recording encode, and the HDD writer is decoupled behind its own queue, so that an HDD stall cannot reach the NVMe writer, the shared encoder (5.3) or the live path (5.5, 5.6) (ADR-009 Consequences; OQ-117, RISK-028).
> - **Unchanged:** two H.264 encodes (OQ-115 open for CM4); H.265 deferred (REQ-ENC-002); audio required; CM4 and CM5 side by side (ADR-004 OPEN); ADR-003 ACCEPTED; ADR-007 OPEN, so every GStreamer, FFmpeg or MediaMTX route below stays a CANDIDATE.
> - **Sections changed:** 2, 3, 4, 5.3, 5.4 (new 5.4.1), 5.5, 5.6 (new 5.6.1), 5.9, 5.10, 6, 7, 8.

> **Source research of 2026-10-08, later** (topics J and K in [REFERENCES.md](REFERENCES.md); research gaps, open questions and design risks in [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json)) is added to the sections listed above (2026-10-09). New registers: OQ-117 to OQ-128, RISK-026 to RISK-034.

> **Owner decisions of 2026-10-09** (recorded in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-006, OQ-008, OQ-074, OQ-128 and OQ-129, [REQUIREMENTS.md](REQUIREMENTS.md) REQ-STR-002 and REQ-REC-001, [RISKS.md](RISKS.md) RISK-033 and [DECISIONS.md](DECISIONS.md) ADR-009 Consequences; propagated here the same day):
> - **How the < 1 s WebRTC target is judged.** At the **95th percentile**: 95 % of camera-to-viewer latency samples under 1 s, over a sustained run with the recording running ("95th percentile < 1 s"). The number of samples and the run length are still open (OQ-008 stays OPEN), as are target browsers and the number of viewers. Sections 5.6.1, 8.
> - **Reach.** WebRTC viewers are on the **LAN only** ("LAN only"; OQ-008). Internet viewers are **not in current scope**: OQ-128 and RISK-033 stay OPEN with scope notes; the NAT-traversal, STUN/TURN and public-address material in sections 4, 5.6, 5.6.1 and 5.9 is kept as reference and labelled not in current scope. The signalling choice stays open (OQ-074).
> - **Recording duration.** No fixed limit: recordings run **until stopped or the disk is full** ("Until stopped / disk full"; OQ-006 ANSWERED). New OQ-129 (OPEN): what the recorder does when one mirrored drive fills, is absent or fails before the other, how the operator is told, and whether long recordings are split into several files. Sections 5.4, 5.4.1, 5.9.
> - **Unchanged:** ADR-009 ACCEPTED; < 1 s for WebRTC viewers only, RTMP best-effort; two H.264 encodes; H.265 deferred (REQ-ENC-002); ADR-004 and ADR-007 OPEN, so every GStreamer, FFmpeg or MediaMTX route stays a CANDIDATE.
> - **Sections changed:** 3, 4, 5.4, 5.4.1, 5.6, 5.6.1, 5.8, 5.9, 8.

**Conventions** (from [README.md](README.md)):

- `[X-NN]` cites the source register.
- `community` facts are worded "reported by …".
- `reasoning` facts and calculations are labelled **reasoning**.
- `CORRECTED` facts are used in their corrected wording.
- Unknowns are `UNKNOWN — VERIFICATION REQUIRED` or `UNDEFINED`, with a marker and an `OQ-NNN` from [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) where one exists.
- Design decisions that this document marks **decision needed** are tracked as OQ-090 (process model, threading, IPC), OQ-091 (operator interface), OQ-092 (configuration format and store; logging transport), OQ-093 (EDID provisioning trigger and ordering) and OQ-094 (updates versus active recordings). Where no OQ covers a point, the text says so.

---

## 1. Scope and boundary

**Kernel/userspace boundary.** ADR-001 (ACCEPTED) and REQ-DRV-001 put the CSI-2 receiver, DMA and TC358743 register access in the kernel. Userspace software therefore:

- talks to the hardware **only** through device nodes, ioctls and events;
- never accesses TC358743 or receiver registers directly.

**Kernel components the userspace relies on** (all from sources; none observed on PACSCORDER hardware):

| Kernel component | Platforms | Userspace sees it as | Facts |
|---|---|---|---|
| `tc358743` driver (module `tc358743`) | All four | V4L2 sub-device `/dev/v4l-subdevN`, one source pad, events enabled | [G-16], [B-18] |
| Unicam receiver (`bcm2835-unicam-legacy` module) | Pi 4, CM4 | Media graph plus capture node `unicam-image` | [C-09], [C-36] |
| RP1 CFE receiver (`rp1-cfe-downstream` module) | Pi 5, CM5 | Media graph with a `csi2` sub-device plus capture node `rp1-cfe-csi2_ch0` | [C-29], [C-32] |
| `bcm2835-codec` hardware H.264 encoder | Pi 4, CM4 only | V4L2 M2M node `/dev/video11` | [D-06], [D-08] |
| `bcm2835-codec` ISP converter | Pi 4, CM4 only | V4L2 M2M node `/dev/video12` | [D-25], [D-07] |
| No hardware encoder | Pi 5, CM5 | — (encoding in userspace software) | [D-31], [G-22] |
| DMA-BUF heaps | All four | `/dev/dma_heap/*`; heap names differ between kernel series | [G-19], [E-48] |
| I2S audio via the `tc358743-audio` overlay | Pi 4/CM4; Pi 5/CM5 unconfirmed (OQ-054) | ALSA card `tc358743` | [A-47], [G-14] |
| *(Added 2026-10-08.)* Audio capture path behind that card: `linux,spdif-dir` stub codec (no controls, no `hw_params`) plus the CPU I2S as clock consumer | CM4: `bcm2835-i2s` [I-10], [I-13]. CM5: RP1 I2S1; labels resolve, capture unconfirmed (OQ-054) [I-07], [I-09] | ALSA card id `tc358743`, device `hw:CARD=tc358743,DEV=0` [I-15] | [I-02], [I-11], [I-15] |
| *(Added 2026-10-08.)* TC358743 audio status controls | All four (same driver; topic I checked CM4 and CM5) | "Audio sampling rate" (0x00981980) and "Audio present" (0x00981981), with `V4L2_EVENT_CTRL` change events; CM4 legacy mode also on `/dev/videoN`, CM5 only on the sub-device | [I-20], [I-21], [I-23] |
| *(Added 2026-10-08.)* Buffer timestamps of the CSI receiver drivers | All four (every Unicam and RP1 CFE driver variant in `rpi-6.18.y`) | `CLOCK_MONOTONIC`, stamped at frame start | [I-33] |

**Out of scope here:**

- kernel driver behaviour: [TC358743_DRIVER.md](TC358743_DRIVER.md);
- Device Tree: [DEVICE_TREE.md](DEVICE_TREE.md);
- image build: [BUILD_SYSTEM.md](BUILD_SYSTEM.md) (the product runs its own project-built OS image, REQ-BLD-002; build tool ADR-003, ACCEPTED);
- release procedure: [RELEASE.md](RELEASE.md).

---

## 2. Responsibility summary

| § | Responsibility | Requirements | Implementation | Tests (status) |
|---|---|---|---|---|
| 5.1 | Boot-time provisioning (overlay, modules, EDID) | REQ-CAP-003, REQ-PLT-001, REQ-DRV-001, REQ-CAP-007 | NOT STARTED | TEST-DRV-002, TEST-PLT-001 (BLOCKED — HARDWARE REQUIRED) |
| 5.2 | Capture control service | REQ-CAP-002, REQ-CAP-004, REQ-CAP-005, REQ-CAP-001, REQ-CAP-007, REQ-CAP-008 | NOT STARTED | TEST-CAP-001, TEST-CAP-003, TEST-CAP-004 (BLOCKED — HARDWARE REQUIRED) |
| 5.3 | Capture → encode pipeline (H.264; H.265 from 2026-10-07 until deferred on 2026-10-08 as REQ-ENC-002). Since 2026-10-08, two H.264 encodes, recording and shared live (owner answer to OQ-005) | REQ-DMA-001, REQ-ENC-001, REQ-ARCH-001, REQ-CAP-007 | NOT STARTED | TEST-DMA-001, TEST-ENC-001, TEST-CAP-002 (BLOCKED — HARDWARE REQUIRED) |
| 5.4 | Recorder. *(Since 2026-10-08, ADR-009 ACCEPTED: fragmented MP4, two file writers — NVMe SSD and a decoupled HDD writer; section 5.4.1, added 2026-10-09.)* | REQ-REC-001 | NOT STARTED | TEST-REC-001 (BLOCKED — HARDWARE REQUIRED); added 2026-10-09: TEST-PERF-001 (HDD-stall soak, RISK-028) |
| 5.5 | RTMP publisher (and RTMP server, if needed). *(Since 2026-10-08: latency best-effort, OQ-116 ANSWERED.)* | REQ-STR-001 | NOT STARTED | TEST-STR-001 (BLOCKED — HARDWARE REQUIRED) |
| 5.6 | WebRTC server and signalling. *(Since 2026-10-08: under 1 s camera-to-viewer, OQ-116 ANSWERED; live publisher in section 5.6.1, added 2026-10-09.)* | REQ-STR-002 | NOT STARTED | TEST-STR-002 (BLOCKED — HARDWARE REQUIRED) |
| 5.7 | ATEM integration (HDMI capture only; no component of its own in the current scope) | REQ-ATEM-001 (scope recorded 2026-10-07, OQ-009 ANSWERED), REQ-CAP-008 | NOT STARTED | TEST-ATEM-001 (BLOCKED — HARDWARE REQUIRED) |
| 5.8 | Audio capture (only if OQ-004 says audio is required). *(Superseded 2026-10-08: required — OQ-004 ANSWERED 2026-10-07; includes sample-rate tracking and A/V alignment.)* | REQ-CAP-006 | NOT STARTED | TEST-AUD-001 (BLOCKED — HARDWARE REQUIRED) |
| 5.9 | Configuration (including the unit's lane configuration, REQ-CAP-007) | All | NOT STARTED | — |
| 5.10 | Logging and diagnostics (since 2026-10-08 including per-encode and combined load for the two video encodes) | REQ-PERF-001; Rule 25 | NOT STARTED | TEST-PERF-001 (BLOCKED — HARDWARE REQUIRED) |
| 5.11 | Update mechanism | REQ-BLD-001, REQ-BLD-002; ADR-003 | NOT STARTED | TEST-BLD-001 (NOT STARTED) |

---

## 3. Component diagram

The boxes are **responsibilities, not processes**. How they map to processes, threads and IPC is UNDEFINED (section 6; OQ-090).

```text
                         +-----------------------------------+
                         | Configuration (5.9)               |
                         | format/store UNDEFINED (OQ-092)   |
                         +-----------------+-----------------+
                                           | settings
       boot, driver load                   v
 +------------------------+   +-----------------------------------+   +-----------------------+
 | Boot-time provisioning |   | Capture control service (5.2)     |   | ATEM integration (5.7)|
 | (5.1)                  |-->| discover nodes, media links,      |   | HDMI capture only     |
 | overlay, modules,      |   | pad formats, DV timings, events,  |   | (OQ-009). No component|
 | EDID write             |   | unsupported-mode policy           |   | of its own: the ATEM  |
 +-----------+------------+   +-----------+-------------+---------+   | is an HDMI source for |
             |                            | start/stop, | ioctls      | 5.1-5.3. No UDP 9910  |
             |                            | timings,    |             | or ATEM RTMP service  |
             |                            | colour info |             +-----------------------+
             |                            v             |
             |               +-----------------------------------+   +-----------------------+
             |               | Capture -> encode pipeline (5.3)  |<--| Audio capture (5.8)   |
             |               | V4L2 capture, DMABUF, convert     |   | required (OQ-004)     |
             |               | (Pi 5/CM5), two H.264 encodes     |   +-----------------------+
             |               +------+-----------+----------------+
             |                      | recording | live H.264 (one encode,
             |                      | H.264     | shared by 5.5 and 5.6; OQ-005)
             |                      |           +-----------+
             |                      v           v           v
             |               +-----------+ +-----------+ +-----------------------+
             |               | Recorder  | | RTMP      | | WebRTC server and     |
             |               | (5.4)     | | publisher | | signalling (5.6)      |
             |               | frag. MP4 | | (5.5)     | | candidate: RTSP to    |
             |               | (ADR-009) | |           | | MediaMTX (5.6.1)      |
             |               +--+-----+--+ +-----+-----+ +-----------+-----------+
             |                  |     |          |                   |
             |                  v     v          v                   v
             |                NVMe   HDD     RTMP server        browsers,
             |                SSD    queue   (OQ-007);          < 1 s (OQ-116);
             |                writer + HDD   best-effort        reach OQ-008
             |                       writer  (OQ-116)
             |                       (OQ-117)
             v
 ============================ kernel boundary (ADR-001, ACCEPTED) ============================
   /dev/v4l-subdevN (TC358743)   /dev/mediaN   /dev/videoN (capture)   /dev/dma_heap/*
   /dev/video11 encoder, /dev/video12 ISP (Pi 4/CM4 only)   ALSA card "tc358743" (audio)
   ^
   |  HDMI -> TC358743 -> CSI-2, 2-lane or 4-lane configuration (REQ-CAP-007)
 HDMI sources: ATEM switcher output or camera connected directly (REQ-CAP-008; OQ-102)

 Cross-cutting, touching every box: Logging and diagnostics (5.10), Update mechanism (5.11)
```

*Diagram updated 2026-10-08.* The audio box read "only if OQ-004 = yes" and the pipeline box "H.264 encode" until the owner decisions of 2026-10-07 (second set). The arrows labelled "H.264" then carried "H.264 or H.265, per output (OQ-103)", and the pipeline box read "H.264/H.265 encode". *(Updated again 2026-10-08, after the owner answered OQ-103 with "H.264 only for now": the pipeline box reads "H.264 encode" and every arrow carries H.264; H.265 is deferred, REQ-ENC-002.)* *(Updated a third time 2026-10-08, after the owner answered OQ-005 with "Separate record + live". The pipeline box read "H.264 encode", with three arrows labelled "H.264" and "(one shared encode or several: OQ-005)". It now reads "two H.264 encodes": a recording encode to 5.4, and one live encode split to 5.5 and 5.6.)* Audio does not pass through the video pipeline: it is captured from ALSA, encoded (AAC and/or Opus) and handed to the recorder and stream muxers next to the video, and the capture control service passes it the source's audio sample rate (section 5.8; reasoning from [I-18], [I-21]). *(Updated a fourth time 2026-10-09, after the owner decisions of 2026-10-08 (ADR-009; OQ-116 ANSWERED). The recorder and WebRTC boxes had two text lines and the RTMP box three, and the labels under them read "storage (OQ-006)", "RTMP server (OQ-007)" and "browsers (OQ-008)". The recorder box now carries "frag. MP4 (ADR-009)" and two arrows, to the NVMe SSD writer and to the HDD queue and HDD writer (OQ-117; section 5.4.1). The WebRTC box carries the candidate live-publisher route, RTSP to MediaMTX (ADR-007 OPEN; section 5.6.1). The labels show the latency scope: RTMP best-effort, browsers < 1 s.)* *(2026-10-09, later: the diagram text is unchanged, but its label "reach OQ-008" is superseded — browsers are on the LAN only (owner, 2026-10-09; OQ-008); internet viewers are not in current scope (OQ-128, RISK-033). The < 1 s is judged at the 95th percentile (OQ-008).)*

---

## 4. Interfaces used by userspace

| Interface | Used by | Operations | Pi 4 / CM4 | Pi 5 / CM5 | Facts |
|---|---|---|---|---|---|
| `/dev/v4l-subdevN` (TC358743) | 5.1, 5.2, 5.8 | `S_EDID` / `G_EDID`; DV timings query, set, get, enumerate and capability; `set_fmt` / `get_fmt` / `enum_mbus_code`; `SUBSCRIBE_EVENT` (`V4L2_EVENT_SOURCE_CHANGE`, `V4L2_EVENT_CTRL`); controls (power present, audio present, audio sampling rate) | Read-write only in Media Controller mode; read-only in legacy mode, where the video node forwards EDID and DV-timings ioctls [C-36] (CORRECTED), [B-25] (CORRECTED) | The only place for EDID and DV timings [B-25] (CORRECTED); reasoning: it must therefore accept them, as in the Pi 5 sequence reported by a Raspberry Pi engineer [C-33] (reported; community source) | [B-18], [B-38], [B-16] |
| `/dev/mediaN` | 5.2 | Enable links; set pad formats | MC mode: `tc358743 → unicam-image` link is `IMMUTABLE\|ENABLED` [C-36] (CORRECTED) | Must enable `csi2:4 → rp1-cfe-csi2_ch0`; pad formats must match [C-32], [C-33] (reported; community source) | [C-32], [C-36] |
| `/dev/videoN` (capture) | 5.3 | `S_FMT` (pixel format only), buffer streaming | `unicam-image` [C-36] | `rp1-cfe-csi2_ch0` [C-32] | [C-37], [C-34] |
| `/dev/video11` (H.264 encoder) | 5.3 | V4L2 M2M multiplanar; DMABUF or MMAP on both queues; H.264 controls | Present [D-06], [D-19] | Does not exist. Reasoning: `bcm2835-codec` cannot probe without a VCHIQ node [D-29], [D-28]. | [D-08] |
| `/dev/video12` (ISP converter) | 5.3 | M2M format conversion and scaling | Present [D-25] | Not available. Reasoning: same driver as `/dev/video11` [D-29]. | [D-07] |
| `/dev/dma_heap/*` | 5.3 (if buffers are allocated from a heap) | DMABUF allocation | Heaps enabled; names per kernel [G-19], [E-48] | Same [G-19], [E-48] | OQ-062 |
| ALSA card `tc358743` | 5.8 | I2S capture, 2 channels | Via the `tc358743-audio` overlay [A-47], [A-13] | Unconfirmed (OQ-054) | [G-14] |
| *(Added 2026-10-08.)* ALSA card `tc358743`, details | 5.8 | Open by card id: `hw:CARD=tc358743,DEV=0` (or `plughw:`/`sysdefault:`), whatever card index is assigned [I-15]; the index was reported to vary (community source) [I-16]. Capture formats and rates below. | CM4 `bcm2835-i2s`: exactly 2 channels, 8–384 kHz, S16_LE/S24_LE/S32_LE; PCM name `bcm2835-i2s-dir-hifi dir-hifi-0` [I-13], [I-15] | RP1 I2S1: channels and formats from hardware registers [I-14]; reasoning from source: PCM name `1f000a4000.i2s-dir-hifi dir-hifi-0`, so select by card id [I-17]. Capture unconfirmed (OQ-054). | [I-13], [I-14], [I-15], [I-16], [I-17] |
| *(Added 2026-10-08.)* TC358743 audio controls | 5.2 (owner of the node), 5.8 (consumer) | Read "Audio sampling rate" (0x00981980; 0 without TMDS) and "Audio present" (0x00981981); subscribe `V4L2_EVENT_CTRL` [I-19], [I-20], [I-21] | `/dev/videoN` in legacy Unicam mode [I-23]; reasoning from [I-23]: in Media Controller mode (ADR-006) the sub-device node | TC358743 sub-device node only [I-23] | [I-19], [I-20], [I-21], [I-23] |
| *(Added 2026-10-08.)* Timestamp clocks | 5.3, 5.8 | V4L2 buffers carry `CLOCK_MONOTONIC` frame-start timestamps [I-33]; ALSA status timestamps use the clock the application picks, and alsa-lib 1.2.14 (the trixie version) switches newly opened `hw` PCMs to monotonic timestamps when the kernel PCM protocol is 2.0.9 or later [I-34] | Same | Same | [I-33], [I-34] |
| Network: RTMP | 5.5 (5.7 only if ATEM RTMP exchange is added; not in current scope) | TCP; default port 1935 | Same | Same | [F-32] |
| Network: WebRTC | 5.6 | RTP media with ICE connectivity: `webrtcbin` takes and produces `application/x-rtp` [F-42] and its ICE transport needs the libnice plugin [G-28]. Ports depend on the implementation. The MediaMTX project reports default settings `webrtcAddress :8889` and `webrtcLocalUDPAddress :8189` [F-44]; reasoning, not stated by [F-44]: :8189 is the UDP port for WebRTC (ICE/RTP) media. *(2026-10-09: now stated by MediaMTX v1.21.1's configuration — `webrtcLocalUDPAddress :8189` is the UDP/ICE listener, and the TCP/ICE listener is disabled by default because TCP adds a progressive delay under congestion [K-41]; internet viewers: [K-42], [K-43], OQ-128.)* *(Internet viewers: not in current scope since 2026-10-09 — LAN-only viewers (OQ-008); kept as reference.)* | Same | Same | [F-42], [G-28], [F-44] (reported; community source); added 2026-10-09: [K-41], [K-42], [K-43] |
| *(Added 2026-10-09; CANDIDATE, ADR-007 OPEN.)* Network: RTSP publish of the live encode to MediaMTX | 5.6.1 | RTSP client publishing (GStreamer `rtspclientsink`), which MediaMTX documents as the recommended way for GStreamer to publish [K-05]. `rtspclientsink` `latency` defaults to 2000 ms ("Amount of ms to buffer") [K-09]; whether that adds delay on the sending side is a research open question (topic K; OQ-126). The MediaMTX project reports its default RTSP address as `:8554` [F-44] (community source). Whether MediaMTX runs on the unit, and as which process, is not decided (OQ-074, OQ-090) | Same | Same | [K-05], [K-09]; [F-44] (community) |
| Network: ATEM control — **not in current scope** (OQ-009 ANSWERED 2026-10-07; kept as reference) | None in the current scope (5.7 only if the owner adds network control) | UDP port 9910, as reported by the OpenSwitcher project | Same | Same | [F-11] (reported; community source) |
| Storage | 5.4 | File writes | Medium UNDEFINED (OQ-006); SD card or CM eMMC per board [G-71] (CORRECTED; reasoning); per-board storage facts DATASHEET REQUIRED (OQ-098). *(Superseded in part 2026-10-09: owner decisions of 2026-10-08, ADR-009 — two recording volumes, a PCIe NVMe SSD and a USB-to-SATA HDD, each written with the same fragmented MP4 (section 5.4.1). CM4 on the CM4 IO Board: the SSD in the PCIe Gen 2 x1 socket, seen as `/dev/nvme0n1` [J-03], [J-11]; the HDD at USB 2.0 through the board's hub [J-06]. Pi 4 Model B: not researched in topic J (NEEDS VERIFICATION, OQ-098).)* | Same. *(Superseded in part 2026-10-09: CM5 on the CM5 IO Board — the SSD in the M.2 slot at PCIe Gen 2 x1, a link the `rpi-6.18.y` device tree leaves disabled unless `dtparam=pciex1` is set [J-13], [J-15], [J-17] (OQ-124); the HDD on USB 3.0 [J-18]. Pi 5: not researched in topic J (NEEDS VERIFICATION, OQ-098).)* | OQ-006 (ANSWERED 2026-10-09), OQ-098; added 2026-10-09: ADR-009, OQ-117 to OQ-124; OQ-129 (added later on 2026-10-09: one drive full, absent or failed; file splitting) |
| `/proc/meminfo` (`CmaFree`) | 5.10 | CMA headroom while streaming | Same | Same | [C-53] (CORRECTED; reasoning from kernel source) |

---

## 5. Responsibilities

Each subsection uses the same headings: purpose, traces, inputs, outputs, interfaces, required behaviour (from sources), platform differences, candidate implementations, open items and status.

### 5.1 Boot-time provisioning

**Purpose.** Bring the kernel capture path into a usable state at every boot **and after every TC358743 driver load**, before any capture is expected.

**Traces.** REQ-CAP-003 (PROPOSED), REQ-PLT-001, REQ-DRV-001, REQ-CAP-007 (DRAFT). ADR-003 (ACCEPTED), ADR-006 (PROPOSED). RISK-010. TEST-DRV-002, TEST-PLT-001.

**Inputs.**

- The unit's lane configuration: 2-lane or 4-lane. Both are required (REQ-CAP-007, DRAFT; owner, 2026-10-07), so provisioning must know which one it is setting up (section 5.9).
- Platform and connector for each lane configuration: UNKNOWN — VERIFICATION REQUIRED; OWNER DECISION REQUIRED (OQ-011; ADR-004, OPEN, a choice per configuration).
- Lanes wired on the PACSCORDER board, for each configuration, and whether one board design serves both: UNKNOWN — VERIFICATION REQUIRED; VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-021).
- The EDID file: content UNDEFINED, OWNER DECISION REQUIRED (OQ-002). Reasoning: the 2-lane and 4-lane configurations may need different EDIDs, because they carry different modes [C-48], [C-49].

**Outputs.**

- The TC358743 is bound to its driver.
- An EDID is loaded, so the driver raises HPD once source +5V is present, after a delay of HZ/7 jiffies [B-21] (CORRECTED).
- The media graph exists for the capture control service.

**Required behaviour (from sources).**

1. **Device Tree overlay**, loaded through a `dtoverlay=` line in `config.txt` [G-12]. Raspberry Pi OS reads `config.txt` from the boot partition mounted at `/boot/firmware/` [G-11] (CORRECTED).

   | | Pi 4 / CM4 | Pi 5 / CM5 |
   |---|---|---|
   | Overlay | `tc358743` [G-12] | `tc358743`, which the overlay map redirects to `tc358743-pi5` [C-11], [E-43] |
   | Control model | Add `media-controller` for Media Controller mode (ADR-006, PROPOSED); the default is legacy video-node mode [C-10] | Always Media Controller [C-11] |
   | Lanes | `4lane` switches both endpoints to 4 lanes; documented for the CM CAM1 connector [G-12], [B-42] | `4lane` is available [G-13]. Reasoning: the shared `tc358743.dtsi` defaults to 2 data lanes [C-12], so 4-lane operation on Pi 5/CM5 needs this parameter (OQ-049). Whether `4lane` together with `cam0` gives 4 lanes on `csi0`: NEEDS VERIFICATION, because `4lane` is defined on the TC358743 endpoint and `csi1_ep` [B-42], and the README text for `tc358743-pi5` is stale [C-13]. |
   | Connector | `cam0` retargets I2C, CSI and clock to connector 0 [C-10] | `cam0` selects connector 0; otherwise connector 1 [C-39] (CORRECTED), [G-13] |

   - **Lane configuration (REQ-CAP-007).** The shared `tc358743.dtsi` defaults to 2 data lanes, and `4lane` switches both endpoints to 4 [C-12]. Reasoning: `4lane` must therefore be set for the 4-lane configuration and left out for the 2-lane configuration. A wrong setting is not caught at probe on Pi 4/CM4: if the endpoint lists more lanes than the Unicam instance supports, the driver only logs a message and adopts the endpoint count [C-17] (see [DEVICE_TREE.md](DEVICE_TREE.md)).
   - **`config.txt` syntax.** Only `dtoverlay=tc358743,<param>=<val>` [G-12] and appending `,cam0` [C-39] are attested. Giving a boolean parameter by name alone (`,media-controller`, `,4lane`) and combining several parameters on one line are NEEDS VERIFICATION (OQ-100); see [DEVICE_TREE.md](DEVICE_TREE.md) §6.0.
   - Whether `camera_auto_detect=0` is needed is unresolved (OQ-072) [C-39] (CORRECTED).
   - Reasoning: `config.txt` model filters let one file carry per-model settings [G-71] (CORRECTED). Whether `[pi4]` also matches a CM4 is NEEDS VERIFICATION (OQ-100).
   - *(Added 2026-10-08; audio is required, REQ-CAP-006.)* **Audio overlay.** Load `tc358743-audio` in addition to `tc358743` [C-37]; its only parameter is `card-name` [G-14]. On CM5 `overlay_map` has no entry for it, so the firmware loads it under its own name [I-05], [I-06]; whether it captures there is OQ-054. It claims GPIO 18–21 [I-30], which conflicts with overlays that default to GPIO 18 [I-31] (OQ-114). The README's "tc358743-fast" is a stale name; only `tc358743`, `tc358743-audio` and `tc358743-pi5` are built [I-04].

2. **Kernel modules.**
   - The bridge, receiver and codec drivers are kernel modules [G-16], [G-17], [G-18], [E-49]. The 2026-10-06 Raspberry Pi OS image ships them for both kernels [G-71] (CORRECTED; reasoning from register inputs).
   - Whether they load automatically on Raspberry Pi OS: NEEDS VERIFICATION — HARDWARE TEST REQUIRED (TEST-PLT-001).
   - Under Buildroot, the default `/dev` management is devtmpfs only; the Buildroot manual names devtmpfs + mdev as the option that can load kernel modules automatically, and with systemd udev handles `/dev` [E-50]. Reasoning: the default configuration therefore needs explicit module loading or a different `/dev` option (OQ-065).
3. **Pi 4/CM4 firmware and memory.**
   - The hardware codec needs non-cut-down GPU firmware; `gpu_mem=16` selects the cut-down firmware, which removes codec support [D-48] (OQ-048).
   - The CMA size is set by `cma-*` overlay parameters [C-40] (OQ-061).
4. **EDID write.** Write the PACSCORDER EDID after every boot and after every driver probe. Why:
   - `edid_blocks_written` is 0 after probe, and HPD is never raised until an EDID is written [B-21] (CORRECTED), [A-33].
   - Writing 0 blocks clears the EDID [B-22].

   Constraints:

   | Constraint | Facts |
   |---|---|
   | Pad 0, start block 0, at most 8 blocks of 128 bytes | [B-22], [A-32] |
   | The chip's EDID support is a 128-byte EDID 1.3 base block plus one CEA-861-D extension | [A-31] |
   | Advertise only modes the wired lanes can carry (REQ-CAP-003). Reasoning: `v4l2-ctl`'s built-in `hdmi` EDID advertises up to 1080p60 [B-24], more than a 2-lane link carries [C-48]. Since both 2-lane and 4-lane configurations are required (REQ-CAP-007), provisioning must load the EDID that matches the unit's configuration (OQ-002). | [B-24], [C-48] |
   | Advertise no interlaced modes (reasoning: they are rejected [B-27]) and no pixel clock above 165 MHz | [B-27], [B-26] |
   | Where the ioctl goes: video node in Pi 4/CM4 legacy mode; sub-device node in Media Controller mode and on Pi 5/CM5 | [B-25] (CORRECTED) |
   | *(Added 2026-10-08.)* Audio descriptors. The driver outputs 2-channel I2S only [I-24], and CM4's `bcm2835-i2s` captures exactly 2 channels [I-13]. Research proposes advertising 2-channel LPCM only, possibly at a single sample rate (research design risk, topic I — not a register fact). Whether sources honour this is unknown (OQ-002, OQ-110, OQ-111). | [I-13], [I-24] |

   **Trigger.** What runs the EDID write after a driver reload is UNDEFINED: a boot unit, a device-event rule, or the capture control service itself. Decision needed — OQ-093 (EDID provisioning trigger and ordering).

**Reference command forms.** **NOT YET RUN ON PACSCORDER HARDWARE**; procedures belong in [TESTING.md](TESTING.md).

- EDID: `v4l2-ctl --set-edid pad=0,file=<edid-file>`. The option syntax is from [B-24]; `pad=0` is from [B-22]; official documentation names `v4l2-ctl --set-edid` [C-37].
- Device selection: `-d` appears, with a node number, in a listing posted by a Raspberry Pi engineer [D-17] (reported; community source).
- NEEDS VERIFICATION: passing a sub-device path to `-d` (the `v4l2-ctl -d /dev/v4l-subdevN` form). The source register attests only `-d 11` [D-17] and running the EDID and timings steps "on /dev/v4l-subdevN" [C-33] (OQ-101).
- Clearing an EDID: the attested form is `--clear-edid <pad>` [B-24]; any concrete argument form such as `--clear-edid 0` is NEEDS VERIFICATION (OQ-101).

**Candidate implementations (CANDIDATE — not decided).**

- `v4l2-ctl` invoked at boot. It is preinstalled in Raspberry Pi OS Lite [G-24]; Buildroot provides it through `BR2_PACKAGE_LIBV4L_UTILS` [E-29].
- A direct `VIDIOC_SUBDEV_S_EDID` call from the capture control service [B-22], [A-32].

**Open.** OQ-002 (EDID content), OQ-032 (HPD state before the first EDID write), OQ-093 (EDID trigger and ordering), OQ-072, OQ-065 (Buildroot only), OQ-048, OQ-061, OQ-100 (`config.txt` syntax), OQ-101 (command syntax). Added 2026-10-08: OQ-054 (audio overlay on CM5), OQ-114 (GPIO 18–21).

**Status.** NOT STARTED.

### 5.2 Capture control service

**Purpose.** Own the control plane at runtime:

- configure the media graph;
- lock to the HDMI input's timings;
- handle source changes without a reboot;
- refuse modes the pipeline cannot carry;
- tell the pipeline when to start and stop.

**Traces.** REQ-CAP-002, REQ-CAP-004 (PROPOSED), REQ-CAP-005 (PROPOSED), REQ-CAP-001, REQ-CAP-007 (DRAFT), REQ-CAP-008 (DRAFT). ADR-001 (ACCEPTED), ADR-005 (PROPOSED), ADR-006 (PROPOSED). RISK-006, RISK-008, RISK-009, RISK-012, RISK-013. TEST-PLT-001, TEST-CAP-001, TEST-CAP-003, TEST-CAP-004. *(Added 2026-10-08: REQ-CAP-006 (DRAFT) for the audio controls; RISK-023; TEST-AUD-001.)*

**Inputs.**

- Events and DV timings from the TC358743 sub-device. The HDMI source is an ATEM switcher's HDMI output or a camera connected directly (REQ-CAP-008; models OQ-102 — ANSWERED 2026-10-07: any HDMI camera, no model list).
- Configuration:
  - pixel format (ADR-005);
  - supported-mode list, per lane configuration (OQ-002; REQ-CAP-007);
  - lane configuration of the unit, 2-lane or 4-lane (REQ-CAP-007). Reasoning: the service needs it to choose the supported-mode list and to predict lane-count failures before streaming [B-31], [C-16]. The lanes actually wired on the PACSCORDER board: UNKNOWN — VERIFICATION REQUIRED; VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-021);
  - link frequency (overlay default 486 MHz, i.e. 972 Mbps per lane [A-45]; ADR-008, PROPOSED, proposes keeping it; OQ-099).

**Outputs.**

- A configured media graph and capture node.
- Signal state, timings and colour metadata, passed to the capture → encode pipeline (5.3) and to diagnostics (5.10).
- *(Added 2026-10-08.)* The HDMI audio sampling rate and audio-present state, and every change to them, passed to audio capture (5.8). Reasoning from [I-18], [I-21], [I-23]: the kernel does not apply the rate to ALSA, and the controls live on the node this service owns.

**Interfaces.** `/dev/v4l-subdevN`, `/dev/mediaN`, `/dev/videoN` (section 4).

**Required behaviour (from sources).**

1. **Discover devices at runtime.** Entity names and node numbers depend on platform, connector and kernel:
   - the TC358743 entity name contains the I2C bus number [C-28] (CORRECTED; reported by a Raspberry Pi engineer and corrected against the 6.18 DT);
   - capture node names differ per receiver [C-32], [C-36].

   Do not hard-code them (OQ-043).
2. **Bring up capture in this order.** The order is EDID → wait for a signal → query and set DV timings → enable links → pad formats → video-node format → STREAMON. It follows the Pi 5 sequence reported by a Raspberry Pi engineer [C-33] (reported; community source) and [B-35], and is the same order as [V4L2.md](V4L2.md) section 6.
   1. **EDID.** An EDID must be loaded on the sub-device (section 5.1, item 4); until then the driver never raises HPD [B-21] (CORRECTED), so no signal can arrive. Subscribe to `V4L2_EVENT_SOURCE_CHANGE` on the sub-device (step 3 below) before waiting.
   2. **Wait for a signal.** Use the source-change event [B-38], the `V4L2_CID_DV_RX_POWER_PRESENT` control [B-16], or repeat the DV-timings query until it no longer returns `-ENOLINK` or `-ENOLCK` [B-29]. Without a wired INT pin, detection relies on the 1000 ms poll [A-30], [B-19].
   3. **Query, then set, the DV timings** (`QUERY_DV_TIMINGS`, then `S_DV_TIMINGS`). This is the `--set-dv-bt-timings query` pattern in official documentation [C-37]. `S_DV_TIMINGS` recomputes the lane count [B-30].
   4. **Enable the media links.**
      - Pi 5/CM5: enable `csi2:4 → rp1-cfe-csi2_ch0`; it starts disabled [C-32].
      - Pi 4/CM4 in Media Controller mode: nothing to enable; the link to `unicam-image` is immutable and enabled [C-36] (CORRECTED).
   5. **Set the pad formats**, with matching media-bus code, `field:none` and a colorspace: the TC358743 pad, and on Pi 5/CM5 also `csi2` pads 0 and 4 [C-33] (reported; community source). The TC358743 pad width and height always come from the current DV timings, and a pad-format set changes only the media-bus code [B-35]. Without `field:none` on the `csi2` pads, STREAMON was reported to fail with `-EPIPE` [C-33].
   6. **Set the video-node format.** `VIDIOC_S_FMT` changes only the pixel format [C-37]: `UYVY` for `UYVY8_1X16` [C-34]; on Pi 5, `BGR3` for `RGB888_1X24` [C-33] (reported; community source).
   7. **Allocate buffers and STREAMON.** The receiver checks the bridge's lane request against its DT endpoint only at this point [C-16], [B-32] (step 4 below). The TC358743 `s_stream` does not check for a signal and always returns 0 [B-36]; reasoning: a successful timings query (sub-step 3) must come first.

   Why this order, and what is not yet known:
   - Reasoning from [B-35]: pad formats set before the timings are applied would carry the wrong frame size, so the timings come first.
   - The reported Pi 5 sequence sets the EDID and timings, then enables the link, then sets the pad formats, then the video-node format [C-33] (reported; community source).
   - Whether the order of sub-steps 4 and 5 matters is NEEDS VERIFICATION — HARDWARE TEST REQUIRED (TEST-PLT-001).
   - The register has no tested Pi 4/CM4 Media Controller sequence; sub-steps 5 and 6 there are reasoning by analogy with [C-33]: NEEDS VERIFICATION — HARDWARE TEST REQUIRED (TEST-PLT-001). Do not rely on filtered format enumeration on Pi 4/CM4 (OQ-046).
   - Which component runs these steps, and in which process, is OQ-090; the EDID trigger and its ordering relative to module load and graph setup is OQ-093.
3. **Handle source changes (REQ-CAP-004).**
   - **Subscribe on the sub-device.** Subscribe to `V4L2_EVENT_SOURCE_CHANGE` on the TC358743 sub-device node [B-38]. On Pi 5, an open issue reports that `rp1-cfe` video nodes do not deliver the event, and a Raspberry Pi engineer advised subscribing on the source sub-device [C-42] (community; OQ-051).
   - **The driver does not re-lock.** It mutes the stream and sends the event, but does not apply new timings [B-39]. Loss of source +5V drops HPD and zeroes the stored timings [B-23].
   - **Detection latency.** Without a wired INT pin, changes are detected by a 1000 ms poll [A-30], [B-19] (RISK-013; INT wiring OQ-020).
   - **Restart streaming after a timing change.** Reasoning from [B-30] and [C-16]:
     - `S_DV_TIMINGS` recomputes the lane count;
     - the receiver checks the lane count only at stream start;
     - and on Pi 5 the `csi2` pad formats must match the new size [C-33] (reported; community source).

     So the service must stop streaming and repeat step 2 from sub-step 2: wait for a signal, query and set the new timings, set the pad formats and the video-node format again, and restart streaming. NEEDS VERIFICATION — HARDWARE TEST REQUIRED (TEST-CAP-003).
   - **Recovery by module reload.** Driver removal does not drop HPD, assert reset or stop the reference clock [B-40]. Whether a module reload is a usable recovery action is open (OQ-039).
4. **Refuse unsupported modes (REQ-CAP-005).** Report them as unsupported; never stream corrupted video.
   - **Out of capability → `-ERANGE`:** interlaced input, and anything outside 640–1920 × 350–1200 or 13–165 MHz [B-26], [C-18]; interlaced rejection is reasoning from driver source [B-27].
   - **Too many lanes → `-EINVAL` at stream start.** The driver does not compare its lane count with the DT `data-lanes` [B-30]. The receiver fails stream start with `-EINVAL` when the bridge asks for more lanes than configured [C-16], [B-32]. Reasoning: the service can predict this before streaming with the driver's formula, `DIV_ROUND_UP(width × height × fps × bpp, lane rate)` with 16 bpp for UYVY [B-31], or treat the `-EINVAL` at stream start as "unsupported". On the 2-lane configuration this applies to 1080p60 in either format and to 1080p50 RGB888 (reasoning) [C-48]; REQ-CAP-007 requires every rate up to the 2-lane limit, not beyond it.
   - **Source models.** The ATEM Mini Pro's documented output standards are all progressive 1080p [F-23]. Camera output modes are UNKNOWN — VERIFICATION REQUIRED (OQ-102); a camera that outputs an interlaced mode is rejected like any interlaced source (reasoning) [B-27] (RISK-009). *(2026-10-08: OQ-102 ANSWERED 2026-10-07 — any HDMI camera, no model list. The service must therefore decide from the detected timings alone, never from a model list; per-camera output modes remain unknown until tested.)*
   - **Do not trust bandwidth alone.** Corruption near lane-count limits has been reported [C-43] (community; RISK-006). The supported-mode matrix must therefore come from TEST-CAP-004, not from the bandwidth formula alone.
   - **HDCP sources.** The driver disables HDCP authentication [A-04], so HDCP-protected sources are expected to give no video (RISK-008, OQ-028).
5. **Format and metadata.**
   - Use UYVY by default (ADR-005, PROPOSED).
   - UYVY is BT.601 limited range, reported as `SMPTE170M` [B-34]. This must reach the encoder's colour signalling (OQ-041).
   - Frame rates are reported as integers, so 59.94 Hz and 60 Hz look alike [B-28] (OQ-040).
6. **Status.** Read `V4L2_CID_DV_RX_POWER_PRESENT` and the "Audio present" and "Audio sampling rate" controls [B-16]. `V4L2_EVENT_CTRL` can be subscribed [B-38].
7. **Audio rate and presence (added 2026-10-08; audio is required, REQ-CAP-006).**
   - Read "Audio sampling rate" (0x00981980) and "Audio present" (0x00981981) after the signal locks and before audio capture opens ALSA [I-20]. A rate of 0 means no TMDS signal [I-19].
   - Subscribe to `V4L2_EVENT_CTRL` for both controls; the driver updates the rate on the CBIT `FS` interrupt and the present flag on audio lock or unlock [I-21]. Forward every change to audio capture (5.8), which must reopen ALSA at the new rate (RISK-023, OQ-111).
   - Node: on CM5 the controls exist only on the TC358743 sub-device node; on CM4 in legacy Unicam mode they are also copied to `/dev/videoN` [I-23]. Reasoning from [I-23]: under ADR-006 (PROPOSED; Media Controller mode everywhere) Unicam does not copy them, so the sub-device node serves both boards.
   - Latency: without a wired INT pin a change takes up to about 1 s, plus I2C time, to reach the control [I-22] (OQ-020).

**Conditions the service must distinguish.** These are derived from driver return codes; this is not a state-machine design.

| Condition | How it is observed | Facts |
|---|---|---|
| No source connected, or no source +5V | `V4L2_CID_DV_RX_POWER_PRESENT` = 0; DV-timings query returns `-ENOLINK` | [B-16], [B-29], [B-23] |
| No EDID loaded, so HPD is low | DV-timings query returns `-ENOLINK`; `G_EDID` returns `-ENODATA` | [B-29], [B-22] |
| Signal present but not locked | DV-timings query returns `-ENOLCK` | [B-29] |
| Locked, outside capability (including interlaced) | DV-timings query or set returns `-ERANGE` | [B-29], [B-27] (reasoning from source) |
| Locked, but needs more lanes than the receiver's DT endpoint configures (the check is against the DT value, not the physical wiring) | Stream start fails with `-EINVAL` and "Device has requested %u data lanes, which is >%u configured in DT" | [C-16] |
| Timing change while streaming | `V4L2_EVENT_SOURCE_CHANGE` on the sub-device | [B-39] |

**Platform differences.**

| | Pi 4 / CM4 | Pi 5 / CM5 |
|---|---|---|
| EDID / DV-timings node | Sub-device in MC mode (ADR-006); video node in legacy mode [B-25] (CORRECTED) | Sub-device only [B-25] (CORRECTED) |
| Graph steps | None for the link (immutable) [C-36] (CORRECTED) | Enable link; set three pad formats [C-32], [C-33] (reported; community source) |
| Source-change subscription | Sub-device (MC mode) [B-38] | Sub-device; video nodes reported not to deliver it [C-42] (reported; community source) |

**Reference command forms.** **NOT YET RUN ON PACSCORDER HARDWARE.**

| Purpose | Command form | Facts |
|---|---|---|
| Apply detected timings | `v4l2-ctl --set-dv-bt-timings query` | [C-37], [C-33] (Pi 5 sequence reported; community source) |
| Enable the Pi 5 capture link | `media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'` | [C-33] (reported; community source) |
| Set a Pi 5 pad format (repeated for the TC358743 pad and `"csi2":4`) | `media-ctl -d <N> -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'` | [C-33] (reported; community source) |

`<N>` and the TC358743 entity name are found at runtime: NEEDS VERIFICATION (OQ-043). How `v4l2-ctl` is pointed at the sub-device node (`-d` with a sub-device path) is NEEDS VERIFICATION (OQ-101).

**Candidate implementations (CANDIDATE — not decided).**

- Shell scripts around `v4l2-ctl` and `media-ctl`, for bring-up: official documentation [C-37] and a Pi 5 sequence reported by a Raspberry Pi engineer [C-33] (reported; community source).
- A program that issues the V4L2 and Media Controller ioctls directly.
- Whether GStreamer's `v4l2src` or FFmpeg handle DV timings, EDID and sub-device events themselves was not researched: NEEDS VERIFICATION (bears on ADR-007, OQ-015).

**Open.** OQ-014, OQ-043, OQ-046, OQ-051, OQ-020, OQ-038, OQ-039, OQ-040, OQ-041, OQ-002, OQ-021 (lanes wired per configuration), OQ-090 (process model), OQ-093 (EDID ordering), OQ-099 (link frequency, ADR-008), OQ-101 (command syntax), OQ-102 (ATEM and camera models; ANSWERED 2026-10-07). Added 2026-10-08: OQ-111 (audio sample-rate policy).

**Status.** NOT STARTED.

### 5.3 Capture → encode pipeline

**Purpose.** Move frames from the capture node into an H.264 encoder without a CPU copy where the platform allows it (REQ-DMA-001), encode them (REQ-ENC-001), and hand the bitstream to the consumers (5.4–5.6). *(Added 2026-10-08.)* Since the owner decision of 2026-10-07 this also covers H.265, which is software-encoded on every candidate (RISK-022); see "Required behaviour: H.265" below. *(Superseded 2026-10-08: the owner deferred H.265 — "H.264 only for now", OQ-103 ANSWERED; REQ-ENC-002 DEFERRED — so this responsibility encodes H.264 only. The H.265 material below is kept as evidence and is not in current scope.)* *(Added 2026-10-08, later; owner answer to OQ-005, "Separate record + live".)* It runs **two H.264 encodes at the same time** from one capture: a recording encode for 5.4, and one live encode shared by 5.5 and 5.6.

**Traces.** REQ-DMA-001, REQ-ENC-001, REQ-ARCH-001, REQ-CAP-001, REQ-CAP-007 (DRAFT). ADR-004 (OPEN), ADR-005 (PROPOSED), ADR-007 (OPEN). RISK-002, RISK-003, RISK-015, RISK-020. TEST-DMA-001, TEST-ENC-001, TEST-CAP-002. *(Added 2026-10-08: RISK-022 — not in current scope since H.265 was deferred the same day; REQ-ENC-002, DEFERRED, no planned test.)* *(Added 2026-10-08, later: RISK-002 and RISK-003 now include the two concurrent encodes; TEST-PERF-001 covers the combined load.)*

**Inputs.**

- Capture buffers: reasoning (calculation) gives 4,147,200 bytes for a 1920x1080 UYVY frame [C-53] (CORRECTED). *(2026-10-08: each buffer now has two consumers, the recording and live encodes; see "Fan-out" below.)*
- Timings and colour metadata from 5.2.
- Encoding parameters: codec, bitrate, latency and number of encodes are UNDEFINED, OWNER DECISION REQUIRED (OQ-005). *(Superseded in part, owner 2026-10-07: the codecs are H.264 and H.265; which output uses which is OQ-103. Bitrate, latency and number of encodes remain OQ-005.)* *(Superseded again 2026-10-08: the codec is H.264 only, for every output — OQ-103 ANSWERED; H.265 deferred, REQ-ENC-002. Bitrate, latency and number of encodes remain OQ-005.)* *(Superseded in part 2026-10-08, later: the number of encodes is two, recording and shared live — owner answer to OQ-005, "Separate record + live". Bitrate, rate control and latency remain OQ-005.)* *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — the live encode serves a target of under 1 s camera-to-viewer for WebRTC viewers; RTMP is best-effort (OQ-116 ANSWERED). Bitrate and rate control remain OQ-005; the live keyframe interval is OQ-127.)*

**Outputs.**

- An H.264 elementary stream with timestamps. On Pi 4/CM4 the encoder copies input timestamps to output buffers [D-19]. *(2026-10-08: two streams — the recording stream for 5.4 and the live stream for 5.5 and 5.6.)*
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* An H.265 elementary stream where OQ-103 requires one. *(OQ-103 ANSWERED 2026-10-08: no output uses H.265 for now.)* GStreamer `x265enc` outputs `video/x-h265` byte-stream, alignment=au [H-13].
- *(Added 2026-10-08.)* Video timestamps on the same clock base as the audio from 5.8, or a documented mapping between them (RISK-024, OQ-112). V4L2 capture buffers are stamped with `CLOCK_MONOTONIC` [I-33].

**Required behaviour: Pi 4 / CM4 (hardware encoder).**

- **DMABUF import** into the `/dev/video11` OUTPUT queue [D-19]. Requirements:
  - the buffer must be one DMA-contiguous region [D-20];
  - it must hold a single plane [D-21];
  - UYVY `bytesperline` is rounded up to 64 bytes [D-22].

  Capture-side buffers are `videobuf2-dma-contig` from CMA [C-53] (CORRECTED). Reasoning, with inputs 2 bytes per UYVY pixel [C-53] and 64-byte alignment [D-22]: a 1920-pixel line (3840 bytes) is a multiple of 64, but a 720-pixel line (1440 bytes) is not. Zero-copy for every supported width is therefore open (OQ-058).
- **Two encode sessions** (added 2026-10-08; owner answer to OQ-005). The recording and live encodes would both run on `/dev/video11` [D-06], each importing the same capture buffer (reasoning).
  - Whether `bcm2835-codec` runs two encode sessions at once is not in the source register: UNKNOWN — VERIFICATION REQUIRED. KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED (OQ-115; RISK-002).
  - Reasoning: two 1080p30 encodes need 2 × 244,800 = 489,600 macroblocks/s, the rate of one 1080p60 encode and about 2.0× the 1080p30 specification [D-10], [D-52].
  - Which split is acceptable if the encoder cannot run both (for example a lower-resolution live encode, or one encode in software) is part of OQ-115.
- **Optional conversion** through the ISP node `/dev/video12` [D-25] (OQ-057).
- **Encoder controls that matter for consumers:**

  | Control | Behaviour | Facts |
  |---|---|---|
  | Profile | Baseline, Constrained Baseline, Main, High | [D-11] |
  | Level | Menu 1.0–5.1; level 4.0 is the hardware specification | [D-12] |
  | Bitrate | 25 kbit/s – 25 Mbit/s, VBR or CBR | [D-13] |
  | B-frames | None; every I-frame is IDR | [D-14] |
  | SPS/PPS repetition | Off by default. WebRTC requires SPS/PPS in-band [F-36]; the official GStreamer streaming example sets `repeat_sequence_header=1` [D-37]. | [D-15] |

  *(Added 2026-10-08; owner answer to OQ-005.)* Reasoning: the live encode, which feeds 5.5 and 5.6, takes the WebRTC values of these controls: Constrained Baseline [D-11], [F-36], SPS/PPS repetition on [D-15], [D-37], and a level per 5.6 [F-40]. B-frames are never produced [D-14]. The recording encode's values are UNDEFINED (OQ-005).

- **Level for 1080p60.** Reasoning: 1080p60 needs level 4.2 [D-36], [F-40], above the hardware specification. Older Raspberry Pi TC358743 instructions were reported to use `h264_level=10`, which is level 3.2 [D-54] (reported; community source).
- **Encoded buffer size.** It defaults to 768 KiB above 720p, and some 1080p frames exceed 512 KiB [D-23].
- **1080p60 sustained encode** is unproven (RISK-002, OQ-056). *(2026-10-08: so are two concurrent encodes (RISK-002, OQ-115).)*

**Required behaviour: Pi 5 / CM5 (software encoder).**

- **No hardware encoder.** `bcm2835-codec` is absent (reasoning) [D-29], [D-31].
- **Format conversion.** UYVY must be converted to a planar format before `libx264` / `x264enc` [D-43], [D-40]. Whether a hardware block can do it is open (OQ-060). *(2026-10-08; reasoning.)* With two encodes, one converted frame can feed both, because `x264enc` and `libx264` both accept I420 and NV12 [D-40], [D-43]. Whether the design converts once or once per encode is ADR-007 (OPEN).
- **CPU and latency.**
  - Official figure: 1080p30 encode costs ~30–40 % CPU [G-22].
  - Software encoders usually output frames with more latency than the old hardware encoders [D-32].
  - A Raspberry Pi engineer (6by9) reported that software 1080p60 encode from camera capture is "easily achievable" [D-50] (reported; community source). Not measured with TC358743 input.
  - RISK-003, OQ-059.
  - *(Added 2026-10-08; owner answer to OQ-005.)* Two software encodes are required at once, recording and live; whether CM5 sustains them is UNKNOWN — HARDWARE TEST REQUIRED. Reasoning from [G-22]: that roughly doubles the encode CPU load. Whether the "~30–40 % CPU" figure means all cores or one core is part of OQ-059 (RISK-003).
- **Encoder settings.**
  - Official documentation replaces `v4l2h264enc` with `x264enc speed-preset=1 threads=1` on Pi 5 [D-37].
  - `rpicam-apps` low-latency mode uses x264 `ultrafast` / `zerolatency` with slice threading [D-35]; its normal mode allows one B-frame [D-35].
  - Reasoning: an encode that feeds WebRTC must disable B-frames, because browsers were reported not to accept them [F-45] (reported; community source). *(2026-10-08: that is the live encode (OQ-005). The recording encode is not bound by it; its settings are UNDEFINED, OQ-005.)*

**Required behaviour: live-encode latency settings (added 2026-10-09; OQ-116, research topic K).** From sources; nothing has been run on PACSCORDER hardware. The < 1 s target applies to WebRTC viewers (5.6.1).

- **CM4 (`bcm2835-codec`).**
  - B-frames are limited to 0, so the encoder never emits them; its defaults are profile High, level 4.0 and a GOP of 60 [K-30]. Reasoning from [K-30] and [F-36]: the default profile is not Constrained Baseline, so the live encode must set its profile explicitly.
  - Force-keyframe is implemented, and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe, so an IDR can be requested on demand, for example when a viewer joins [K-31]. Interval and trigger: OQ-127.
  - `v4l2h264enc` on BCM2711 reports 0 latency to the pipeline [K-32]. Reasoning (research design risk, topic K): GStreamer's latency query, and A/V-sync calculations that rely on it, then leave out the real encode time (RISK-024, OQ-112).
  - A Raspberry Pi engineer stated that the encoder holds no extra buffers and that its latency depends on the macroblock count, about 10 ms for 720p on a Pi 4 (community source) [K-33]. The register gives no 1080p figure for BCM2711, and the recording encode would share the encoder (OQ-115).
- **CM5 (x264).**
  - x264's `zerolatency` tune sets lookahead, sync-lookahead and B-frames to 0 and uses sliced threads [K-27]; x264 then holds no frames back, and GStreamer `x264enc` reports 0 frames of latency [K-28]. Its per-frame encode time on BCM2712, with the recording encode running, is unknown (OQ-059).
  - **Defaults trap.** GStreamer 1.26 `x264enc` applies its property-default string only when no speed preset and no tune are set; since `speed-preset` defaults to medium, it actually runs x264's medium preset (3 B-frames, rc-lookahead 40). Its `bframes=0` property default therefore does not guarantee a B-frame-free stream; set `tune=zerolatency`, set `bframes=0` explicitly, or force `profile=baseline` in caps (CORRECTED) [K-29] (OQ-127).
  - Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39].
- **Both.** MediaMTX documents that browsers deliberately do not support H.264 B-frames over WebRTC and recommends Baseline [K-04]; YouTube recommends 2 s keyframes (not over 4 s), CBR and 2 B-frames for RTMP [K-17]. Reasoning (REQ-ENC-001 notes): the shared live encode has to be B-frame-free, and its keyframe interval is bounded by YouTube's advice and by viewer join time unless keyframes are requested on demand (OQ-127, RISK-019).
- Pi 4 Model B uses the same encoder driver as CM4, and Pi 5 the same SoC as CM5 (reasoning).

**Required behaviour: H.265, all platforms (added 2026-10-08; deferred — REQ-ENC-002; not in current scope).** Kept as evidence for REQ-ENC-002; none of it is required while H.264 is the only codec (owner, 2026-10-08; OQ-103). H.265 is software-only on every candidate: `bcm2835-codec` has no HEVC encoder [D-24] and Pi 5/CM5 have no hardware video encoder [D-31]. Nothing below has been measured on PACSCORDER hardware.

- **Encoder.** Debian's x265 4.1-2 (`libx265-215`); Raspberry Pi does not override it [H-01], [H-02]. Debian's build enables arm64 assembly and links Main, Main10 and Main12 into one library [H-03]. Trixie also packages kvazaar and HM, but neither is wired into the distribution's FFmpeg or GStreamer, HM is not real-time, and SVT-HEVC is not packaged [H-18] (CORRECTED).
- **Planar input only, on every board.** FFmpeg `libx265` accepts only planar (or gray) formats, never `uyvy422`, `yuyv422` or `nv12` [H-10] (CORRECTED); GStreamer `x265enc` accepts only Y444, Y42B, I420 and their 10/12-bit variants [H-13]. So on CM4 too the H.265 path needs a UYVY-to-planar conversion. Reasoning: on the CPU at 1080p60 it reads about 249 MB/s and writes about 187 MB/s before x265 starts [H-43]. Reasoning from [D-40], [H-10], [H-13]: I420 is accepted by both x264 and x265, NV12 only by x264. Hardware conversion: unknown (OQ-057 on CM4, OQ-060 on CM5).
- **Throughput.** UNKNOWN — VERIFICATION REQUIRED; HARDWARE TEST REQUIRED (OQ-104, RISK-022). Evidence so far:
  - research found no official H.265 figure; a Raspberry Pi engineer (6by9) stated on the official forum that software H.265 encode "is too intensive an operation to perform at any significant resolution" [H-19] (reported; community source);
  - a community benchmark reports `libx265` "Live" at 10.00 FPS on Pi 5 [H-20] and 4.33 FPS on a Cortex-A72 Pi 400 [H-21] (community sources), in a test that is not a 1080p60 live measurement (vbench clips, `-threads 1`, a 2022 x265 snapshot) [H-22] (community source);
  - reasoning: in that harness `libx265` was about 6.6 times slower than `libx264` on Pi 5 [H-23].
- **SIMD per board.** x265 enables its Neon DotProd kernels only when `AT_HWCAP` reports ASIMDDP [H-04]; they can apply on CM5's Cortex-A76 but not on CM4's Cortex-A72, and I8MM, SVE and SVE2 apply on neither [H-05]. Trixie's 4.1-2 lacks the 4.2 and 4.3 AArch64 speed-ups [H-07] (OQ-105).
- **Latency settings.**
  - `tune=zerolatency` sets B-frames 0, lookahead 0 and one frame thread; WPP row parallelism stays on [H-16]. The `ultrafast` preset alone keeps 3 B-frames [H-17].
  - GStreamer 1.26.2 `x265enc`: properties `speed-preset`, `tune`, `bitrate` (kbit/s), `key-int-max`, `option-string` [H-14]; it reports a hard-coded 5-frame latency unless `tune=zerolatency` (then 0); the fix arrived in 1.26.8 and is not backported [H-15].
  - FFmpeg `libx265`: the wrapper copies its thread count into x265's frame threads after applying preset and tune [H-10] (CORRECTED). Reasoning from [H-10] and [H-16]: `-threads` replaces zerolatency's single frame thread unless the frame-thread count is set explicitly (research gap, topic H; BUILD TEST REQUIRED, OQ-104).
- **Concurrency (reasoning).** If WebRTC keeps an H.264 track (5.6; OQ-108) while another output uses H.265, CM5 runs two software video encodes plus the conversion on the same CPU [D-31], [G-22] (the BCM2712 core count is not in the source register); CM4 can keep H.264 on `/dev/video11` [D-06] while H.265 runs on its Cortex-A72 cores. CPU left for audio encoding, muxing and WebRTC: OQ-104.
- **Licensing.** x265 is GPL version 2 or later, or commercially licensed; neither covers HEVC patents [H-39]. HEVC patent pools charge per unit [H-40], [H-41], [H-42] (RISK-015, OQ-087, OQ-109; [RELEASE.md](RELEASE.md)).

**Required behaviour: common.**

- **Memory.**
  - The CMA pool and its limits are set in the DT [E-47] (CORRECTED); overlay defaults are in [C-40].
  - Reasoning: four 1080p UYVY buffers take about 16.6 MB [C-53] (CORRECTED).
  - RISK-020, OQ-061; heap names OQ-062.
  - *(Added 2026-10-08; reasoning.)* With two encodes, a capture buffer is held until both have released it, so the slower encode sets the hold time. The buffer count and CMA budget stay open (OQ-061, RISK-020).
- **Fan-out.** One shared encode or several separate encodes: UNDEFINED (OQ-005). See [ARCHITECTURE.md](ARCHITECTURE.md) section 3.9 for the "strictest consumer" reasoning. *(Superseded 2026-10-08: decided by the owner, "Separate record + live" (answer to OQ-005).)* Topology, as reasoning and not a design:
  - **One capture feeds two encoders.** Each capture buffer goes to both: as a DMABUF on Pi 4/CM4 (OQ-058, OQ-115), or as the converted frame on Pi 5/CM5 (OQ-060).
  - **Recording stream.** The recording encode's stream goes to 5.4.
  - **Live stream.** The live encode's stream is split after the encoder. One branch goes to 5.5, through a parser to `stream-format=avc` [F-34], [F-35]. The other goes to 5.6 (RTP).
  - **Open.** How the split and the buffer sharing are done is ADR-007 and OQ-090. The "strictest consumer" reasoning now applies to the live encode only ([ARCHITECTURE.md](ARCHITECTURE.md) section 3.9).
  - *(Added 2026-10-09; ADR-009 / OQ-116.)* **Recording stream to two writers.** The recorder (5.4.1) writes the recording stream to the NVMe SSD and, through a decoupled queue, to the HDD. The HDD branch must not back-pressure this pipeline: V4L2 buffer queues are FIFOs, and reasoning: each filled capture buffer still waiting adds one frame period, 33.3 ms at 30p [K-36]; on CM4, when no buffer is queued, Unicam drops the frame into a dummy buffer [K-34]. A stall that reaches capture therefore becomes live latency or dropped frames (reasoning; OQ-117, RISK-028). Queue depth and policy on the live branch: OQ-126 (RISK-032).
- **Licensing.**
  - x264 is GPL [D-47].
  - FFmpeg must be built with `--enable-gpl` to use `libx264` [D-42].
  - RISK-015, OQ-086, OQ-087.

**Candidate implementations (CANDIDATE — not decided; ADR-007).**

| Candidate | Pi 4 / CM4 | Pi 5 / CM5 | Notes |
|---|---|---|---|
| GStreamer | `v4l2src` → `v4l2h264enc` with `output-io-mode=dmabuf-import` [D-53]; official pipeline [D-37]; UYVY fed straight in, as reported [D-54] (reported; community source) | `x264enc` [D-37], which does not accept packed UYVY [D-40] | `v4l2h264enc` exists only if V4L2 probing is compiled in [D-38]. The Debian package used by Raspberry Pi OS ships the probed M2M elements [G-26]; Buildroot needs an option [D-39], [E-32]. `openh264enc` accepts only I420 [D-41]. |
| FFmpeg | `h264_v4l2m2m`. The Raspberry Pi OS build adds DMABUF input [D-45]; upstream is MMAP-only and copies every frame [D-44]. | `libx264` (GPL build) [D-42]; no UYVY input [D-43] | Raspberry Pi OS archive: FFmpeg 7.1.5 `+rpt2` [G-29] |
| Direct V4L2 application (the `rpicam-apps` model) | DMABUF import into `/dev/video11` [D-34] | `libx264` through libav [D-33] | Everything downstream (muxing, RTMP, WebRTC) must be built or linked separately |

*(Added 2026-10-09; research topic K; CANDIDATE facts, not decisions.)* Latency-relevant facts per candidate:

- **GStreamer.** `v4l2src` always reports itself as live, with a minimum latency of one frame and a maximum of buffer-pool depth × frame duration; the pool minimum is 2 [K-37]. `v4l2h264enc` issues force-keyframe on request [K-31] and reports 0 latency on BCM2711 [K-32]. `x264enc` defaults trap: [K-29] (CORRECTED); `tune=zerolatency`: [K-27], [K-28].
- **FFmpeg.** Reasoning: x264's `zerolatency` tune [K-27] is a property of the x264 library, which FFmpeg's `libx264` encoder also uses. No topic K register fact covers FFmpeg's own input, muxing or output latency: NEEDS VERIFICATION (BUILD TEST REQUIRED; OQ-126).
- **Direct V4L2 application.** Reasoning from [K-30], [K-31] and [K-36] (as recorded under ADR-007): the B-frame limit, force-keyframe and the capture queues are V4L2 interfaces that the application drives itself, so it controls queue depth directly.

**H.265 candidate implementations (added 2026-10-08; CANDIDATE — not decided; ADR-007; deferred — REQ-ENC-002; not in current scope).** The same on CM4 and CM5, because both encode H.265 in software:

| Candidate | Element / encoder | Packaging in Raspberry Pi OS | Notes |
|---|---|---|---|
| GStreamer | `x265enc` (plugin `x265`), then `h265parse` before the MP4/Matroska muxers | `gstreamer1.0-plugins-bad` 1.26.2-3+rpt4+deb13u3 (Raspberry Pi build) installs `libgstx265.so` [H-11], [H-12] (CORRECTED) | Planar input only [H-13]; latency reporting [H-15]; `h265parse` after `x265enc` [H-37]. Cannot publish HEVC over RTMP with 1.26.2 `flvmux` [H-27] (5.5). |
| FFmpeg | `libx265` | Raspberry Pi FFmpeg 8:7.1.5-0+deb13u1+rpt2, configured with `--enable-libx265`; `libavcodec61` depends on `libx265-215` [H-08], [H-09] | Planar input only; thread-count behaviour [H-10] (CORRECTED). Can publish HEVC over Enhanced RTMP [H-26] (5.5). |
| Direct application | x265 library or libav | Reasoning from [H-09], [H-13]: encoding, muxing and transport still come from x265 or FFmpeg libraries | Reasoning from [I-33], [I-34]: one `CLOCK_MONOTONIC` timestamp clock is available for both the video buffers and ALSA audio (5.8), although the audio samples are still clocked by the source [I-26] |

**Open.** OQ-005, OQ-015, OQ-041, OQ-056, OQ-057, OQ-058, OQ-059, OQ-060, OQ-061, OQ-062. Added 2026-10-08: OQ-103 (codec per output; ANSWERED 2026-10-08: H.264 only), OQ-104 (H.265 throughput and headroom), OQ-105 (x265 SIMD and version), OQ-108 (H.264 WebRTC track alongside H.265) — OQ-104, OQ-105 and OQ-108 not in current scope (H.265 deferred, REQ-ENC-002) — and OQ-112 (A/V timestamps). Added 2026-10-08, later (two encodes): OQ-115 (two concurrent encodes on the CM4 hardware encoder); OQ-005 stays OPEN for bitrate, rate control and latency. Added 2026-10-09: OQ-116 (ANSWERED 2026-10-08: < 1 s for WebRTC viewers only; RTMP best-effort; OQ-005 stays OPEN for bitrate and rate control), OQ-117 (HDD-branch back-pressure), OQ-125 (measured latency), OQ-126 (element latencies and queues), OQ-127 (keyframes and B-frames); RISK-024, RISK-028, RISK-031, RISK-032.

**Status.** NOT STARTED.

### 5.4 Recorder

**Purpose.** Record encoded video, and audio if required, to local storage (REQ-REC-001).

**Traces.** REQ-REC-001, REQ-PERF-001. ADR-007 (OPEN). TEST-REC-001. *(Added 2026-10-09: ADR-009 (ACCEPTED 2026-10-08); RISK-026 to RISK-030; TEST-PERF-001 for the HDD-stall soak.)*

**Inputs.** The H.264 stream and timestamps from 5.3; audio from 5.8 if OQ-004 requires it. *(Superseded 2026-10-08: audio is required — OQ-004 ANSWERED 2026-10-07 — and the video may be H.264 or H.265, per OQ-103.)* *(Superseded again 2026-10-08: the video is H.264 only — OQ-103 ANSWERED; H.265 deferred, REQ-ENC-002. Audio is still required.)* *(2026-10-08, later: the video is the recording encode's stream, separate from the live encode used by 5.5 and 5.6 — owner answer to OQ-005, "Separate record + live". Its parameters are OQ-005.)*

**Outputs.** Files on storage. *(Superseded in part 2026-10-09: owner decisions of 2026-10-08, ADR-009 — two copies of every recording, both fragmented MP4, one on the NVMe SSD and one on the USB-to-SATA HDD; section 5.4.1.)*

**Required behaviour.**

- **UNDEFINED, OWNER DECISION REQUIRED (OQ-006):**
  - container;
  - storage medium;
  - minimum recording duration;
  - behaviour on power loss.

  *(Superseded in part 2026-10-09: owner decisions of 2026-10-08, ADR-009 ACCEPTED — container MP4; storage a PCIe NVMe SSD and a USB-to-SATA HDD, mirrored; power-loss strategy fragmented MP4, with the amount that may be lost measured under OQ-119. The maximum recording duration is still UNDEFINED, OWNER DECISION REQUIRED (OQ-006). Section 5.4.1.)* *(Superseded 2026-10-09, later: duration — no fixed limit; recordings run until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED). Behaviour when one mirrored drive fills, is absent or fails before the other, and file splitting: OQ-129, OPEN.)*
- **Persistence (reasoning):** recordings must go to a writable location that persists across reboot and update.
  - Raspberry Pi OS's read-only option uses `overlayroot=tmpfs` [G-48], so root-filesystem writes do not persist.
  - `rpi-image-gen`'s `image-rota` layout provides a shared persistent data partition [G-40].
- **Timestamps:** fractional source rates are reported as integers by the driver [B-28] (OQ-040).
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* **HEVC in the container.** GStreamer 1.26.2 `qtmux`/`mp4mux` and `matroskamux` accept H.265 in `hvc1` or `hev1` form; `x265enc` outputs byte-stream, so `h265parse` is needed first; `matroskamux` warns that `hev1` "is not officially supported" [H-37]. FFmpeg 7.1.5's MP4 muxer tags HEVC as `hev1`, `hvc1` or `dvh1` (`hvc1` meaning parameter sets shall not be in the stream), and its Matroska muxer handles HEVC [H-38]. The container is still OQ-006. *(Superseded in part 2026-10-09: the owner decided the container on 2026-10-08 — MP4, written fragmented (ADR-009, ACCEPTED); OQ-006 stays OPEN for the maximum recording duration. HEVC is still deferred.)* *(Superseded 2026-10-09, later: OQ-006 ANSWERED — until stopped or the disk is full (owner, 2026-10-09); follow-up OQ-129.)*
- *(Added 2026-10-08.)* **Audio track.** AAC from FFmpeg's native `aac` (the default AAC encoder, not experimental; 128 kb/s stereo by default) [I-40] (CORRECTED), GStreamer `voaacenc` [I-44] or `avenc_aac` [I-46]. The recorded rate must be the rate the source actually sends (RISK-023, OQ-111), and audio and video timestamps must be aligned (RISK-024, OQ-112).

**Platform differences.**

- Storage options differ, for example SD card versus Compute Module eMMC [G-71] (CORRECTED; reasoning). The per-board storage facts are DATASHEET REQUIRED (OQ-098). *(Superseded in part 2026-10-09: research topic J gave the USB and storage facts for CM4, CM5 and their IO Boards; the recording drives per board are in section 5.4.1. Pi 4 Model B and Pi 5 storage stays DATASHEET REQUIRED, OQ-098.)*
- On Pi 5/CM5 encoding runs in software [G-22]. Reasoning: recording, streaming and encoding therefore compete for the same CPU. *(2026-10-08: two video encodes, recording and live, run on that CPU (OQ-059, RISK-003). On Pi 4/CM4 both would share the hardware encoder (OQ-115, RISK-002).)*

**Candidate implementations (CANDIDATE).**

- GStreamer `isomp4` (MP4) and `matroska` plugins, shipped by Debian trixie's `gstreamer1.0-plugins-good` [G-26]. No GStreamer package is installed in the Lite image by default [G-32].
- FFmpeg container muxers: not covered by the source register — NEEDS VERIFICATION. *(Superseded in part 2026-10-08: FFmpeg 7.1.5's MP4 and Matroska muxers handle HEVC [H-38] — HEVC deferred, REQ-ENC-002; not in current scope. The other FFmpeg muxer behaviour the recorder needs, such as power-loss safety, is still NEEDS VERIFICATION.)* *(Superseded in part 2026-10-09: FFmpeg 7.1's MP4 muxer documentation covers fragmented output that stays decodable if writing is interrupted, `+frag_keyframe` and `hybrid_fragmented` [J-45]; whether the Raspberry Pi 7.1.5 build accepts them is a research gap, topic J (OQ-118). Section 5.4.1.)*
- Reference point only: ATEM switchers record H.264 + AAC as MP4 [F-07], [F-26].

**Open.** OQ-006, OQ-040, OQ-004 (ANSWERED 2026-10-07: audio required), OQ-094 (updates versus active recordings), OQ-098 (storage facts). Added 2026-10-08: OQ-103 (ANSWERED 2026-10-08: H.264 only), OQ-111, OQ-112; later the same day OQ-005 (recording-encode bitrate, rate control and latency) and OQ-115. Added 2026-10-09: OQ-117 to OQ-124 (section 5.4.1). *(Added later on 2026-10-09: OQ-006 ANSWERED — until stopped or the disk is full; OQ-129 (one drive full, absent or failed; file splitting).)*

**Status.** NOT STARTED.

#### 5.4.1 Recorder as decided by ADR-009 (added 2026-10-09)

**Purpose.** Write every recording, as fragmented MP4, to both recording drives, so that a power cut leaves decodable files and a stall on the HDD cannot reach the NVMe copy, the encoders or the live path (ADR-009, ACCEPTED 2026-10-08). Nothing here has been built or run on PACSCORDER hardware. The system view is [ARCHITECTURE.md](ARCHITECTURE.md) section 3.9.3; detail is in [RECORDING.md](RECORDING.md).

**Traces.** REQ-REC-001, REQ-PERF-001, REQ-STR-002 (the live path must not be affected). ADR-009 (ACCEPTED), ADR-007 (OPEN). RISK-026, RISK-027, RISK-028, RISK-029, RISK-030. TEST-REC-001, TEST-PERF-001; TEST-STR-002 for the effect on the live path (OQ-117).

**Decided (ADR-009).** Points 1 to 3 are owner decisions; point 4 is not.

1. MP4, written fragmented.
2. Every recording written to both drives (mirror).
3. The HDD in a self-powered enclosure or hub, never on the board's VBUS. *(The owner's words were "Self-powered enclosure"; "or hub" in ADR-009 decision 3 is Claude's addition, not an owner decision.)*
4. ext4 on the recording volumes: **Claude's proposal inside ADR-009, not an owner decision** (OQ-120).

**Component view (responsibilities, not processes; process model OQ-090).**

```text
recording encode (5.3, H.264) ---+
AAC audio (5.8.1 item 5) --------+--> fragmented MP4 muxer (fragment duration: OQ-118)
                                        |
                                        +--> NVMe writer --> NVMe SSD volume (ext4 proposed, OQ-120)
                                        |
                                        +--> HDD queue (decoupled; size and overflow policy: OQ-117)
                                               --> HDD writer --> USB-to-SATA HDD volume, self-powered
                                                                  (ext4 proposed, OQ-120)
```

*(Diagram added 2026-10-09.)* One muxer is drawn for brevity. Whether one muxer feeds both writers or each writer has its own muxer (for example two file sinks behind one tee, or two `splitmuxsink`s) is open under ADR-007; concurrent writing to two file sinks has not been tested (research gap, topic J; OQ-117).

**Required behaviour.**

1. **Fragmented MP4.** A plain moov-at-end `mp4mux` file is unplayable after an unclean stop [J-38]; a fragmented file stays decodable if writing is interrupted, at the cost of lower compatibility [J-45] (RISK-029). The fragment duration is not set yet (OQ-118).
2. **Two writers, every recording.** Both copies come from the same recording encode (5.3).
3. **HDD writer decoupled (ADR-009 Consequences; OQ-117, RISK-028).** Stall sources named in OQ-117: standby-to-ready after a spin-down, 2.5 s typical and 3.0 s maximum for the one Seagate BarraCuda 2.5-inch family in the register, an example and not a general HDD figure (CORRECTED) [J-30]; a UAS reset (RISK-027); a periodic `fsync()`; and on CM4 the shared USB 2.0 path (RISK-026). None of them may back-pressure the shared recording encode, the NVMe writer or the live path (5.5, 5.6).
   - Reasoning from [J-30] (CORRECTED) and [J-36] (OQ-117; not a design figure): riding out a 3.0 s stall at about 3.15 MB/s (25 Mbit/s video plus AAC) needs about 9.5 MB of HDD-queue capacity, before any margin.
   - Queue size and overflow policy (drop and mark the HDD copy incomplete, or stop the HDD copy): UNDEFINED; BUILD TEST REQUIRED; HARDWARE TEST REQUIRED (OQ-117).
   - Research design risks (topic J — not register facts): a large or leaky queue in front of the HDD branch; spin-down disabled or extended where the bridge allows. Whether `hdparm -B/-S` works through the bridge is unknown (research gap, topic J; OQ-117).
   - Reasoning: whichever overflow policy is chosen, the state of each copy (complete or not) has to be visible to the operator and in the logs (5.10; OQ-091).
4. **Filesystem and mounts (OQ-120).** The CM4 and CM5 defconfigs build ext4, VFAT, NVMe, USB storage and UAS into the kernel, and exFAT as a module [J-29]. ext4 defaults: `data=ordered`, `barrier=1`, `commit=5`; a power loss loses at most the last 5 s of metadata changes without damaging the filesystem, but delayed allocation can lose older data (CORRECTED) [J-35]. Raspberry Pi's storage guide documents `nofail`; adding `x-systemd.device-timeout=30` shortens the 90 s boot wait for an absent disk to 30 s rather than removing it; Raspberry Pi OS Lite does not automount (CORRECTED) [J-33]. Which mount options keep a missing HDD from delaying boot or blocking the NVMe copy: OQ-120.
5. **Power loss (OQ-119, RISK-030).** Whether fragmented mode forces each fragment to storage is not in the register: UNKNOWN — VERIFICATION REQUIRED; HARDWARE TEST REQUIRED (OQ-119). Research design risk (topic J — not a register fact): the last seconds still in the page cache or drive cache are lost even with fragmented MP4.
6. **Long recordings.** The maximum duration is OQ-006; file splitting, if used, is part of OQ-118. *(Superseded in part 2026-10-09: no fixed duration — recordings run until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED). What the recorder does when one drive fills, is absent or fails while the other still works, how the operator is told, and whether long recordings are split into several files: OQ-129, OPEN (file splitting is also asked in OQ-118).)*

**Configuration parameters that are still open** (section 5.9):

| Parameter | Open item |
|---|---|
| Fragment duration or rule; finalising a cleanly stopped file as a normal MP4; file splitting *(2026-10-09, later: file splitting for long recordings is also asked in OQ-129)* | OQ-118 |
| HDD-queue size and overflow policy; HDD spin-down | OQ-117 |
| Filesystem and mount options (ext4 proposed, not owner-decided) | OQ-120 |
| Maximum recording duration *(ANSWERED 2026-10-09: no fixed limit — until stopped or the disk is full; no longer a parameter to set)* | OQ-006 (ANSWERED 2026-10-09) |
| *(Added 2026-10-09.)* Behaviour when one mirrored drive fills, is absent or fails before the other; operator notification; splitting long recordings into several files | OQ-129 |
| NVMe SSD and CM4 PCIe adaptor; HDD enclosure and USB-to-SATA bridge; `usb-storage.quirks` entry if needed [J-26] | OQ-121, OQ-122 |
| CM5: whether the image sets `dtparam=pciex1` (section 5.1, `config.txt`) [J-15], [J-17] | OQ-124 |

**Platform differences (from sources; what the software has to handle).**

| | CM4 on the CM4 IO Board | CM5 on the CM5 IO Board |
|---|---|---|
| NVMe SSD | PCIe Gen 2 x1 socket through a passive adaptor; `/dev/nvme0n1` [J-03], [J-11]. No 64-bit accesses from the ARM; MSI-X disputed between the datasheets [J-02] (OQ-123) | M.2 M-key at PCIe Gen 2 x1, 2230–2280 [J-13], [J-14]; the link is disabled by default in the `rpi-6.18.y` device tree and `dtparam=pciex1` controls it [J-15], [J-17] (OQ-124) |
| HDD | USB 2.0 through the IO Board hub, whose ports share one VBUS switch of about 1.2 A [J-06]; reasoning (CORRECTED): with the SSD in the PCIe socket, the HDD shares that USB 2.0 path with every other USB device, USB 3 would need a PCIe switch, and booting through a switch is not supported [J-09], [J-04] (RISK-026) | USB 3.0 [J-18]; the two IO Board ports share about 1.2 A of VBUS [J-19]; the NVMe link and RP1 are on separate PCIe controllers [J-17] |
| USB-to-SATA bridge driver | `uas` cannot bind behind `dwc2`, but can under Raspberry Pi OS's default `otg_mode=1` controller [J-24], [J-07] | RP1 xHCI controllers [J-21]; the bound driver is to be recorded on hardware (OQ-122) |
| Bridge faults (both boards) | A Raspberry Pi engineer's forum post reports that some UAS devices stop responding or, rarely, throw write data away, with `usb-storage.quirks=…:u` as the workaround (community source, CORRECTED) [J-27]; Raspberry Pi documents that USB SATA adapters can fail if Linux selects UAS [J-28]; the kernel applies built-in bridge quirks first [J-25] (RISK-027, OQ-122) | Same |

Pi 4 Model B and Pi 5 storage were not researched in topic J: NEEDS VERIFICATION (OQ-098).

**Candidate implementations (CANDIDATE — not decided; ADR-007).**

| Candidate | Fragmented MP4 | File splitting | Writers and sync | Facts |
|---|---|---|---|---|
| GStreamer 1.26.2 | `mp4mux`/`qtmux` `fragment-duration` (ms; default 0) gives a fragmented file when > 0, and then silently disables robust muxing; `fragment-mode` is `dash-or-mss` (default) or `first-moov-then-finalise` (since 1.20) [J-39]. A plain `mp4mux` file is unplayable after power loss [J-38]. Robust muxing, not chosen by ADR-009: [J-40], [J-41]. Whether the Raspberry Pi OS build exposes these properties is a research open question (topic J; BUILD TEST REQUIRED, OQ-118) | `splitmuxsink` wraps a muxer (`mp4mux` by default) and a sink, and starts a new file at a keyframe before `max-size-time` or `max-size-bytes` is crossed; the minimum file is one GOP; `max-files` defaults to 0 (unlimited) [J-44] | `filesink` calls `fsync()` after a buffer flagged sync-after and has an `o-sync` property, default FALSE [J-41] | [J-38], [J-39], [J-40], [J-41], [J-44] |
| FFmpeg 7.1 | A fragmented file stays decodable if writing is interrupted, at lower compatibility; `+frag_keyframe` starts a fragment at each video keyframe; `hybrid_fragmented` writes a fragmented file and converts it to a normal one at the end, and an aborted file can be remuxed by hand; `empty_moov` exists in the 7.1 source but is not described in its muxer documentation [J-45]. Reasoning from [J-45] (OQ-118): with `frag_keyframe`, the recording encode's keyframe interval sets the fragment length. The Raspberry Pi 7.1.5 build was not checked for these flags (research gap, topic J; OQ-118) | Not in the register: NEEDS VERIFICATION | Not in the register: NEEDS VERIFICATION | [J-45] |
| Direct application | Muxing and the two writers have to be written or linked (reasoning, as recorded under ADR-007) | — | — | — |

**Open.** OQ-006, OQ-117, OQ-118, OQ-119, OQ-120, OQ-121, OQ-122, OQ-123, OQ-124, OQ-090, OQ-091. *(2026-10-09, later: OQ-006 ANSWERED — until stopped or the disk is full; OQ-129 added.)*

**Status.** NOT STARTED. TEST-REC-001 and TEST-PERF-001: BLOCKED — HARDWARE REQUIRED.

### 5.5 RTMP publisher (and RTMP server, if required)

**Purpose.** Publish encoded video, and audio if required, to an RTMP server (REQ-STR-001).

**Traces.** REQ-STR-001. ADR-007 (OPEN). TEST-STR-001. REQ-ATEM-001 would apply only if the owner adds receiving ATEM RTMP, which is not in current scope (OQ-009 ANSWERED 2026-10-07). *(Added 2026-10-08: REQ-ENC-001, REQ-CAP-006; RISK-022, RISK-025 — both not in current scope since H.265 was deferred the same day, REQ-ENC-002.)*

**Inputs.**

- H.264 in AVC stream format (`stream-format=avc`) [F-34]. *(2026-10-08: from the live encode, shared with 5.6 — owner answer to OQ-005, "Separate record + live".)* Reasoning from that decision:
  - RTMP carries the WebRTC-receivable settings of 5.6: Constrained Baseline, no B-frames.
  - Whether each destination accepts that profile and level is not in the register: UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED (OQ-007).
  - Bitrate is the live encode's, shared with WebRTC (OQ-005, OQ-007).
- AAC audio, if required. *(Superseded 2026-10-08: audio is required, OQ-004 ANSWERED 2026-10-07.)*
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* H.265, if OQ-103 assigns it to RTMP. *(OQ-103 ANSWERED 2026-10-08: RTMP carries H.264 only.)*

Legacy RTMP/FLV carries exactly these [F-31] (the first two; H.265 needs Enhanced RTMP, below).

**Outputs.** An RTMP or RTMPS session over TCP; port 1935 by default [F-32], [F-33].

**Required behaviour (from sources).**

- **Codecs.** HEVC, AV1, VP9 and Opus need Enhanced RTMP [F-31].
- **`flvmux` input.** It needs `video/x-h264,stream-format=avc` and raw AAC [F-34] (CORRECTED). Reasoning: `v4l2h264enc` outputs byte-stream, so an `h264parse` (or equivalent) is needed between them [F-35].
- **E-RTMP on Raspberry Pi OS.** `eflvmux` (E-RTMP muxing) first appears in the GStreamer 1.28 branch and is absent from 1.24 and 1.26 [F-34] (CORRECTED). Reasoning: it is therefore not in the GStreamer 1.26.2 that Debian trixie provides [G-26], [G-27].
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* **HEVC over RTMP — the framework constraint.** OQ-103 is ANSWERED (RTMP carries H.264 only), so this constraint does not apply in the current scope and, per the ADR-007 scope note, does not drive the framework choice. Kept as evidence for REQ-ENC-002:
  - The Enhanced RTMP specification defines the HEVC FourCC `hvc1` and lists it among the `fourCcList` connect-command values [H-24]. FFmpeg 6.1 was the first release to mux HEVC into FLV and signal it over RTMP [H-25].
  - **FFmpeg 7.1.5 can** mux HEVC + AAC into enhanced FLV for RTMP publishing. Its `rtmp_enhanced_codecs` option writes a `fourCcList` but accepts only `hvc1`, `av01` and `vp09`. Its FLV audio table has AAC, MP3 and others but **no Opus** [H-26].
  - **GStreamer 1.26.2 `flvmux` cannot**: its video sink pad has no H.265 [H-27].
  - Reasoning from [H-26], [H-27], [F-34]: if RTMP carries H.265 (OQ-103) and GStreamer is used elsewhere (ADR-007), this responsibility needs FFmpeg 7.1.5 for HEVC RTMP, a backported or newer GStreamer carried outside the distribution, or SRT/MPEG-TS instead (RISK-025, OQ-107). A split GStreamer/FFmpeg design also has to reconcile the two frameworks' A/V timestamp handling [I-35], [I-38] (5.8).
  - **Alternative transport.** The distribution stacks contain the components for HEVC over SRT in MPEG-TS: GStreamer 1.26.2 `mpegtsmux` accepts H.265, plugins-bad ships the SRT and MPEG-TS plugins, and the Raspberry Pi FFmpeg is built with `--enable-libsrt` [H-30]. Whether SRT is required is OQ-076.
  - **Destinations.** YouTube Live lists H.264, H.265 and AV1 over RTMP/RTMPS with AAC or MP3 audio, and recommends 12 Mbps for 1080p60 H.265 versus 17 Mbps for H.264; the page does not use the term "Enhanced RTMP" [H-29]. MediaMTX's documentation lists H265 among RTMP publish and read codecs [H-28]. Whether each destination accepts FFmpeg's signalling, and whether `-rtmp_enhanced_codecs hvc1` is required, is OQ-106.
- *(Added 2026-10-08.)* **Audio for RTMP.** AAC (legacy and Enhanced RTMP; FFmpeg's FLV has no Opus [H-26]). Encoders: FFmpeg native `aac` [I-40] (CORRECTED), GStreamer `voaacenc` [I-44] and `avenc_aac` [I-46]. `fdk-aac` is not an option: in a GPL FFmpeg build `libfdk_aac` needs `--enable-nonfree`, which makes the result unredistributable [I-42], and Debian calls its licence incompatible with every GPL version [I-43] (OQ-063, OQ-113).
- *(Added 2026-10-09; OQ-116 ANSWERED.)* **Latency: best-effort.** The owner decided on 2026-10-08 that the < 1 s target applies to WebRTC viewers only; RTMP latency is set by the receiving platform (REQ-STR-001).
  - YouTube Live offers Normal (no figure), Low ("less than 10 seconds" for most viewers) and Ultra-low ("less than 5 seconds" for most viewers, no 4K) [K-15]; the API's `latencyPreference` takes `normal`, `low` or `ultraLow`, and `ultraLow` supports neither closed captions nor resolutions above 1080p [K-16].
  - Reasoning (CORRECTED): YouTube documents no sub-second mode and names the player's read-ahead buffer as the main source of latency, so this responsibility's encoder settings cannot make YouTube meet < 1 s [K-18].
  - YouTube recommends a 2 s keyframe frequency (not over 4 s), CBR, 2 B-frames, 1 reference frame and CABAC; the B-frame advice conflicts with the B-frame-free live encode that WebRTC needs [K-17]. Whether YouTube accepts a B-frame-free or Baseline stream with acceptable quality is a research open question (topic K; OQ-127, RISK-019).
  - Other RTMP platforms were not checked for latency modes (research gap, topic K; OQ-007).
- **UNDEFINED, OWNER DECISION REQUIRED:**
  - destinations, RTMPS, bitrate and whether audio is included (OQ-007);
  - SRT (OQ-076).
- **Reconnect and back-off behaviour:** not researched; UNDEFINED.
- **Receiving an ATEM stream** (ATEM option 3 in 5.7; **not in current scope** since the owner decision of 2026-10-07, OQ-009 ANSWERED; kept as reference). If the owner adds it, reasoning: PACSCORDER must run a *listening* RTMP server [F-46]:
  - GStreamer `rtmp2src` is client-only [F-33];
  - FFmpeg has a `listen` option [F-32];
  - the MediaMTX project reports an RTMP server on :1935 [F-44] (reported; community source).

  Ports and conflicts: OQ-075.

**Candidate implementations (CANDIDATE).**

- GStreamer `flvmux` + `rtmp2sink` [F-33], [F-34]; `libgstrtmp2` is in Debian trixie's plugins-bad [G-27].
- FFmpeg `-f flv rtmp://…` [F-32].
- MediaMTX as a relay or server, reported by its project [F-44] (reported; community source).
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* For HEVC: FFmpeg with its `libx265` encoder and enhanced-FLV muxer [H-09], [H-26] (command form NEEDS VERIFICATION, OQ-101); or SRT/MPEG-TS with GStreamer `mpegtsmux` + SRT plugin or FFmpeg with libsrt [H-30].

**Open.** OQ-007, OQ-075, OQ-076, OQ-063 (audio encoder). Added 2026-10-08: OQ-103 (ANSWERED 2026-10-08: H.264 only), OQ-106 and OQ-107 (not in current scope — H.265 deferred, REQ-ENC-002), OQ-111, OQ-113; later the same day OQ-005 (live-encode bitrate, rate control and latency). Added 2026-10-09: OQ-116 (ANSWERED 2026-10-08: RTMP best-effort), OQ-127 (keyframe interval and B-frames shared with WebRTC); OQ-005 stays OPEN for bitrate and rate control.

**Status.** NOT STARTED.

### 5.6 WebRTC server and signalling

**Purpose.** Make live video, and audio if required, available to WebRTC clients (REQ-STR-002).

**Traces.** REQ-STR-002. ADR-007 (OPEN). RISK-019. TEST-STR-002.

**Inputs.**

- An H.264 stream that meets the browser constraints below. *(2026-10-08: the live encode, shared with 5.5 — owner answer to OQ-005, "Separate record + live". The constraints below therefore bind the live encode, and RTMP receives the same stream. The recording encode is not bound by them.)*
- Opus or G.711 audio, if required. *(Superseded 2026-10-08: audio is required, OQ-004 ANSWERED 2026-10-07.)*
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* Optionally an H.265 stream, if OQ-103 assigns H.265 to WebRTC; it does not replace the H.264 track (below). *(OQ-103 ANSWERED 2026-10-08: WebRTC carries H.264 only.)*

**Outputs.** RTP media over ICE to browsers. GStreamer's `webrtcbin` takes and produces `application/x-rtp` [F-42].

**Required behaviour (from sources).**

| Constraint | Detail | Facts |
|---|---|---|
| Profile | WebRTC browsers must implement H.264 Constrained Baseline. `packetization-mode=1` is required, `profile-level-id` must appear in SDP, and SPS/PPS must be sent in-band. | [F-36] |
| Level | Reasoning: 1080p needs Level 4 (`0x28`) or higher, and 1080p60 needs Level 4.2 [F-40]. `42e01f` is Constrained Baseline Level 3.1 [F-38] (reasoning) and is libwebrtc's default [F-39]. Level-asymmetry rules are in [F-37] (CORRECTED). | [F-40], [F-38], [F-39], [F-37] |
| B-frames | Browsers do not accept H.264 B-frames in WebRTC, as reported by the MediaMTX project | [F-45] (reported; community source) |
| Audio | WebRTC endpoints must implement Opus and G.711. AAC is not a required WebRTC codec, so AAC audio has to be transcoded (typically to Opus) for browser playback. | [F-41] |
| Signalling | `webrtcbin` has none built in [F-42], and needs the libnice GStreamer plugin at runtime [G-28] | [F-42], [G-28] |
| H.265 support *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | RFC 7742 mentions H.265 only for SEI orientation messages, not as a required codec [H-32]. Chrome 136+ enables H.265 only where the platform decodes it in hardware, with no software fallback [H-33]. Safari 18.0 supports the standard HEVC RTP payload [H-34]. No evidence of Firefox support was found [H-35]. A Microsoft Q&A answer reports that Edge 147 does not enable it by default [H-36] (reported; community source). | [H-32], [H-33], [H-34], [H-35], [H-36] |
| H.265 payloader *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | GStreamer's `rtph265pay` implements RFC 7798; profile-id, tier-flag and level-id in its output caps arrived in 1.26.4, after the distribution's 1.26.2 | [H-31] |
| Opus encoders *(added 2026-10-08)* | FFmpeg `libopus` (FFmpeg's native `opus` is experimental and CELT-only) and GStreamer `opusenc`, which defaults to 64 kbit/s constrained VBR. Both accept only 48, 24, 16, 12 or 8 kHz, so a 44.1 kHz source needs `audioresample` before `opusenc` | [I-41], [I-45], [I-47] |

- **UNDEFINED, OWNER DECISION REQUIRED (OQ-008):**
  - viewers on the LAN only or over the internet (NAT traversal);
  - browsers;
  - number of viewers;
  - latency target.

  *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — the latency target is under 1 s camera-to-viewer for WebRTC viewers (REQ-STR-002; OQ-116 ANSWERED); section 5.6.1. Reach, browsers and the number of viewers remain UNDEFINED (OQ-008).)* *(Superseded in part 2026-10-09, later: reach — LAN only (owner, 2026-10-09; OQ-008); internet viewers are not in current scope (OQ-128, RISK-033). The latency target is judged at the 95th percentile (95 % of samples under 1 s, sustained run, recording running). Still UNDEFINED (OQ-008): browsers, the number of viewers, and the sample count and run length of the measurement.)*
- Signalling and NAT design: OQ-074. Level signalling: OQ-073. *(Added 2026-10-09: internet viewers through MediaMTX, ICE, STUN and TURN: OQ-128.)* *(2026-10-09, later: the NAT-traversal part and OQ-128 are not in current scope — LAN-only viewers (OQ-008); kept as reference. The signalling choice stays open (OQ-074).)*

**Platform differences.**

- Pi 4/CM4: the hardware encoder offers Constrained Baseline [D-11] and never produces B-frames [D-14]. *(2026-10-08: SPS/PPS repetition, off by default [D-15], must be switched on for the live encode (reasoning from [F-36]). The same encoder would also run the recording encode; whether it runs both at once is OQ-115 (RISK-002).)*
- Pi 5/CM5: the software encoder must be configured with B-frames off, since `rpicam-apps`' normal mode allows one [D-35] (reasoning). *(2026-10-08: this applies to the live encode. It runs beside the software recording encode (OQ-059, RISK-003).)*
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* Reasoning from [H-33] to [H-36] ([H-36] is a community report): an H.265-only WebRTC stream would not reach Firefox, Chrome without hardware HEVC decode, or Edge by default, so an H.264 track has to remain (RISK-019, OQ-108). If H.265 is offered as well, CM5 runs a second concurrent software video encode (5.3; OQ-104), while CM4 can keep the H.264 track on its hardware encoder.

**Candidate implementations (CANDIDATE).**

| Candidate | Evidence | Packaging / licence |
|---|---|---|
| GStreamer `webrtcbin` + own signalling | [F-42] | LGPL [F-42]; in Debian trixie plugins-bad [G-27] |
| GStreamer `webrtcsink` (gst-plugins-rs): includes a simple signalling server and encodes internally | [F-43] | MPL-2.0 [F-43]; no Buildroot package [F-43]; Raspberry Pi OS packaging NEEDS VERIFICATION. *(Superseded 2026-10-09: neither Debian trixie nor the Raspberry Pi archive packages the gst-plugins-rs WebRTC plugin (`webrtcsink`, `whipclientsink`) as of 2026-10-08 [K-06] (RISK-034); using it needs a self-built plugin matching GStreamer 1.26.2 (research gap, topic K). Its documentation gives no latency figure [K-13].)* |
| MediaMTX: browser page and WHEP endpoint, as reported by the project | [F-45] (reported; community source) | MIT, single executable [F-44] (reported; community source); no Buildroot package [F-44] |
| *(Added 2026-10-09.)* GStreamer `rtspclientsink` publishing the live encode over RTSP to MediaMTX, which serves WebRTC readers (section 5.6.1) | [K-05], [K-09]; [F-45] (community) | `rtspclientsink` is a GStreamer element [K-09]; MediaMTX as above. The packaging of `rtspclientsink` in the Raspberry Pi OS image is not in the register: NEEDS VERIFICATION (BUILD TEST REQUIRED) |
| *(Added 2026-10-09.)* GStreamer `whipclientsink` publishing to MediaMTX over WHIP | MediaMTX requires GStreamer 1.22 or later and, for H.264, the Baseline profile [K-05]; WHIP is RFC 9725 and covers ingest only [K-01] | Not packaged in Debian trixie or the Raspberry Pi archive [K-06] (RISK-034) |

**Open.** OQ-008, OQ-073, OQ-074, OQ-063. Added 2026-10-08: OQ-103 (ANSWERED 2026-10-08: H.264 only), OQ-108 (not in current scope — H.265 deferred, REQ-ENC-002), OQ-111; later the same day OQ-005 (live-encode bitrate, rate control and latency) and OQ-115. Added 2026-10-09: OQ-116 (ANSWERED 2026-10-08), OQ-125, OQ-126, OQ-127, OQ-128 (section 5.6.1). *(2026-10-09, later: OQ-128 not in current scope — LAN-only viewers, OQ-008.)*

**Status.** NOT STARTED.

#### 5.6.1 Live publisher and the < 1 s WebRTC target (added 2026-10-09)

**Purpose.** Carry the live encode (5.3) and the Opus audio (5.8.1) to WebRTC viewers within the owner's target of under 1 s camera-to-viewer (REQ-STR-002; OQ-116 ANSWERED 2026-10-08), without being slowed by the recorder (5.4.1). *(Added later on 2026-10-09; owner decisions, OQ-008.)* The target is judged at the 95th percentile — 95 % of camera-to-viewer samples under 1 s, over a sustained run with the recording running; sample count and run length are still open (OQ-008). Viewers are on the LAN only; internet viewers are not in current scope (OQ-128, RISK-033). Nothing here has been built or measured on PACSCORDER hardware. The system view and the budget summary are in [ARCHITECTURE.md](ARCHITECTURE.md) section 3.9.4; the full budget is in [PERFORMANCE.md](PERFORMANCE.md); detail is in [STREAMING.md](STREAMING.md).

**Traces.** REQ-STR-002, REQ-ENC-001, REQ-CAP-006. ADR-007 (OPEN). RISK-019, RISK-024, RISK-031, RISK-032, RISK-033, RISK-034. TEST-STR-002. *(2026-10-09: RISK-033 is not in current scope — LAN-only viewers, OQ-008.)*

**Component view (CANDIDATE route — reasoning recorded under ADR-007, OPEN; not a decision).**

```text
live encode (5.3; H.264, B-frame-free) + Opus (5.8.1)
     |
     v
live publisher: RTSP client (GStreamer rtspclientsink)      [K-05]; latency default 2000 ms [K-09] (OQ-126)
     |
     v
MediaMTX: RTSP in, WebRTC readers out (WHEP reported [F-45])  relay latency undocumented [K-03] (OQ-125)
     |
     v
browser viewers (reach: LAN or internet, OQ-008, OQ-128)
```

*(Diagram added 2026-10-09.)* Reasoning from [F-45], [K-05] and [K-06], as recorded under ADR-007: with the packages in Debian trixie and the Raspberry Pi archive, this is the candidate GStreamer route to browser viewers, because MediaMTX documents RTSP-client publishing as the recommended way for GStreamer to publish [K-05] and the gst-plugins-rs WebRTC elements are not packaged [K-06] (RISK-034). It is not the only option: ADR-007 also names a self-built gst-plugins-rs for WHIP, and `webrtcbin` with PACSCORDER's own signalling stays a candidate (5.6 candidate table) [F-42]. The RTMP publisher (5.5) takes the same live stream separately. For FFmpeg and a direct application, the WebRTC route has no register fact (NEEDS VERIFICATION, as in ADR-007). *(2026-10-09, later: the diagram text is unchanged, but its label "reach: LAN or internet, OQ-008, OQ-128" is superseded — viewers are on the LAN only (owner, 2026-10-09; OQ-008); internet viewers are not in current scope (OQ-128, RISK-033).)*

**Required behaviour (from sources; reasoning where labelled).**

1. **Set every buffering element explicitly; never keep a default** (OQ-126, RISK-032).

   | Element (GStreamer candidate) | Default | Facts |
   |---|---|---|
   | `webrtcbin` `latency` | 200 ms ("Default duration to buffer in the jitterbuffers") | [K-07] |
   | `rtpbin` and `rtpjitterbuffer` `latency` | 200 ms | [K-08] |
   | `rtspclientsink` and `rtspsrc` `latency` | 2000 ms ("Amount of ms to buffer"); MediaMTX's own reader examples set `rtspsrc latency=0` | [K-09] |
   | `v4l2src` | Reports itself live: minimum latency one frame, maximum buffer-pool depth × frame duration; pool minimum 2 | [K-37] |
   | `x264enc` (CM5) | x264 medium preset (3 B-frames, rc-lookahead 40) unless `tune`, `bframes` or a profile is set (CORRECTED); with `tune=zerolatency` no frames are held back | [K-29], [K-27], [K-28] |
   | `v4l2h264enc` (CM4) | Reports 0 latency, although the encode takes real time | [K-32] |

   Reasoning [K-10]: the `webrtcbin`, `rtpbin` and `rtpjitterbuffer` values are jitter-buffer sizes for received RTP; with a browser viewer, the viewer's buffer is the browser's own, so they matter only where a GStreamer element receives RTP inside the live path. Whether `rtspclientsink`'s 2000 ms adds delay on the sending side, and whether a send-only `rtpbin` adds any, are research open questions (topic K; OQ-126).
2. **Keep queues short and keep other branches out** (OQ-126, OQ-117). V4L2 queues are FIFOs, and reasoning: each waiting filled buffer adds one frame period [K-36]. Whether the queues in front of the live encoder and live payloader must be leaky or limited to one buffer is a research open question (topic K; OQ-126). The HDD branch of the recorder must not back-pressure the live path (5.4.1; RISK-028). Research design risk (topic K — not a register fact): on CM5, if x264 cannot finish a frame within 33.3 ms, queues grow without bound unless the live branch has leaky queues or a lower resolution or frame rate (RISK-032; OQ-059).
3. **No B-frames; keyframes on demand** (OQ-127, RISK-019). MediaMTX documents that browsers deliberately do not support H.264 B-frames over WebRTC, and recommends Baseline with Opus [K-04]. CM4 never emits B-frames and can insert an IDR on request [K-30], [K-31]; CM5 settings: 5.3 [K-27], [K-29] (CORRECTED). Claude's reasoning from [K-30] (OQ-127; not a measurement): with the CM4 encoder's default 60-frame GOP at 30 fps, a new viewer may wait up to about 2 s for a decodable frame without an on-demand keyframe. Whether MediaMTX passes a viewer's keyframe request (PLI/FIR) back to an RTSP publisher is undocumented (research open question, topic K; OQ-127).
4. **Audio.** MediaMTX's WebRTC audio codecs are Opus, G722 and G711 [K-04]. Opus frames are 2.5–60 ms, and GStreamer `opusenc` defaults to 20 ms [K-40]. Audio-path latency and browser A/V sync are unmeasured (research gap, topic K; OQ-112, OQ-125).
5. **Do not take the pipeline's own latency figure as the measurement.** `v4l2h264enc` on BCM2711 reports 0 latency [K-32] (RISK-024). End-to-end latency is measured (OQ-125, TEST-STR-002), and viewers can report their own buffering through the W3C statistics API: `jitterBufferDelay`, `jitterBufferEmittedCount`, `jitterBufferTargetDelay`, `jitterBufferMinimumDelay` and `totalProcessingDelay` on the inbound stream (CORRECTED) [K-12]. `RTCRtpReceiver.jitterBufferTarget` is a hint of at most 4000 ms that influences but does not set the browser's target [K-11].
6. **MediaMTX behaviour to expect.** MediaMTX documents no numeric WebRTC latency [K-03]. It documents that it favours real-time delivery: most protocols run over UDP so late packets can be dropped, and a full outgoing circular buffer (`writeQueueSize`, default 512) drops packets and logs "reader is too slow" [K-44]. WHEP, the playback protocol, is still an Internet-Draft as of 2026-10-08 [K-02]; WHIP is RFC 9725 and covers ingest only [K-01] (RISK-034).
7. **Internet viewers, if required** (OQ-008, OQ-128, RISK-033). MediaMTX v1.21.1's configuration listens on UDP `:8189` and leaves TCP disabled, because, it says, TCP adds a progressive delay under congestion [K-41]; its documentation says internet clients need the public IP or DNS name in `webrtcAdditionalHosts`, or STUN/TURN in `webrtcICEServers2` when the listeners cannot be reached [K-42]; four connection methods are documented, and TCP transport is recommended only for a coturn relay [K-43]. *(Not in current scope since 2026-10-09 — LAN-only viewers (OQ-008); kept as reference.)*

**Budget (reasoning, CORRECTED; a labelled budget, not a measurement) [K-45].** For a CM4 1080p30 WebRTC/WHEP viewer on a LAN the documented or extrapolated terms are about 56 ms typical and about 75 ms worst case (capture readout [K-34], CM4 hardware encode extrapolated from a Raspberry Pi engineer's 720p statement (community source) [K-33], no waiting frames [K-36]), leaving about 925–945 ms for undocumented terms. The encode term assumes the live encode has the CM4 encoder to itself, but the recording encode would share it (OQ-115); on CM5 it is unknown until measured (OQ-059). RISK-031, OQ-125; full budget in [PERFORMANCE.md](PERFORMANCE.md).

**Configuration parameters that are still open** (section 5.9): live keyframe interval and on-demand keyframes (OQ-127); the latency value of every live-path element and the queue policy (OQ-126); WebRTC reach, ICE servers and advertised addresses (OQ-008, OQ-128); bitrate and rate control (OQ-005). *(Superseded in part 2026-10-09: reach decided — LAN only (owner, 2026-10-09; OQ-008); STUN/TURN servers and public advertised addresses for internet viewers are not in current scope (OQ-128, RISK-033). The sample count and run length of the 95th-percentile latency measurement are still open (OQ-008).)*

**Platform differences.**

| | CM4 | CM5 |
|---|---|---|
| Live encode B-frames | Never emitted [K-30] | Must be configured off: `tune=zerolatency`, explicit `bframes=0`, or `profile=baseline` in caps (CORRECTED) [K-29] |
| On-demand keyframe | Force-keyframe through `v4l2videoenc` [K-31] | Not in the register for `x264enc`: NEEDS VERIFICATION (OQ-127) |
| Encode latency evidence | About 10 ms at 720p on a Pi 4, stated by a Raspberry Pi engineer (community source) [K-33]; reported as 0 to GStreamer [K-32] | x264 holds no frames back with `zerolatency` [K-28]; per-frame time unknown (OQ-059); Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39] |
| Capture | Buffer completed at frame end; with no buffer queued, the frame is dropped into a dummy buffer [K-34] | Buffer completed at frame end; `min_queued_buffers=1` [K-35] |

**Open.** OQ-125, OQ-126, OQ-127, OQ-128, OQ-008, OQ-074, OQ-059, OQ-115, OQ-112. *(2026-10-09, later: OQ-128 not in current scope — LAN-only viewers; OQ-008 stays OPEN for browsers, viewer count, sample count and run length.)*

**Status.** NOT STARTED. TEST-STR-002: BLOCKED — HARDWARE REQUIRED.

### 5.7 ATEM integration

**Purpose.** Integrate with Blackmagic ATEM switchers (REQ-ATEM-001).

**Scope (owner, 2026-10-07; OQ-009 ANSWERED): option 1 only — capture the ATEM's HDMI output through the TC358743.** The owner said: "it can be atem and direct video from camera" (REQ-CAP-008, DRAFT). Consequences for the software (reasoning):

- In the current scope this responsibility has **no software component of its own**. The ATEM is an HDMI source, handled by provisioning, capture control and the pipeline (5.1–5.3), exactly like a camera connected directly (REQ-CAP-008).
- There is **no UDP 9910 service** and **no RTMP server for the ATEM** in the current scope.
- Options 2 to 4 were offered and not selected; option 5 lies outside the HDMI scope. They are kept in the table below as reference, marked "not in current scope", unless the owner adds them.
- Which ATEM models and which cameras must be supported is OQ-102. *(ANSWERED 2026-10-07, second answer: any HDMI camera, no model list, plus ATEM outputs; acceptance is set by the supported-mode matrix and EDID, OQ-002.)*
- *(Added 2026-10-08.)* ATEM program audio is embedded on the HDMI output [F-24] (CORRECTED), so with audio required (REQ-CAP-006) it is captured by 5.8 like any HDMI source's audio.

| Option | Scope (2026-10-07) | Software responsibility | Facts | Open |
|---|---|---|---|---|
| 1. Capture the ATEM HDMI output through the TC358743 | **In scope** | No new component: provisioning, capture control and pipeline (5.1–5.3). The ATEM output is 1080p only (23.98–60), 4:2:2 YUV 10-bit Rec 709 [F-23]. It defaults to multiview, not program [F-25]. Program audio is embedded on HDMI [F-24] (CORRECTED). On the 2-lane configuration the ATEM must be set to 1080p50 or lower in UYVY, or 1080p30 or lower in RGB888 (reasoning from [C-48]; REQ-CAP-007). | [F-23], [F-25], [F-24], [C-48] | OQ-078, OQ-083, OQ-102 |
| 2. Read tally / recording / streaming state, or control the switcher, over the network | Not in current scope | A UDP 9910 client. The protocol is reverse-engineered, as reported by the OpenSwitcher project [F-11] (reported; community source). The official SDK does not document it [F-10] and supports Windows and macOS only [F-01]. The atem-connection project reports tally (`TlSr`), aux routing (`CAuS`) [F-17] and recording/streaming status commands (`RTMS`, `StRS`) [F-16] (reported; community source). Its protocol enum stops at version 9.6, although ATEM 10.x exists [F-18] (reported; community source). | [F-11], [F-10], [F-01], [F-16], [F-17], [F-18] | OQ-077, OQ-084, OQ-089 |
| 3. Receive the ATEM's RTMP stream | Not in current scope | The RTMP server in 5.5. Reasoning: the ATEM publishes as an RTMP client, so PACSCORDER must listen [F-46]. The ATEM streams H.264 or H.265 with AAC [F-06]. AAC is not a required WebRTC codec, so for browser WebRTC playback it has to be transcoded, typically to Opus [F-41]. | [F-06], [F-46], [F-41] | OQ-075, OQ-079, OQ-080 |
| 4. Send PACSCORDER video into an ATEM setup | Not in current scope | RTMP publisher (5.5). An ATEM Streaming Bridge documents RTMP input only from Blackmagic sources [F-28] (CORRECTED). | [F-28] | OQ-081 |
| 5. Use the ATEM USB-C webcam output | Not in current scope | A USB capture path, bypassing the TC358743. Blackmagic documents only Mac and Windows use [F-27]. | [F-27] | OQ-082 |

Reacting to ATEM state (for example, starting a recording when the ATEM records) would need option 2, which is not in current scope (OQ-009 ANSWERED).

**Candidate UDP 9910 libraries (reference only — not in current scope).** CANDIDATE only if the owner adds option 2; all community tier, as reported by each project:

| Library | Language / runtime | Licence | Reported limits | Facts |
|---|---|---|---|---|
| atem-connection | TypeScript, Node.js | MIT | Protocol enum up to 9.6; no USB control | [F-14], [F-15], [F-18] (reported by the project) |
| PyATEMMax | Python 3 | GPL-3.0 | No recording/streaming status commands | [F-19], [F-20] (reported by the project) |
| pyatem (OpenSwitcher) | Python | LGPL-3.0-only | Network and USB | [F-21] (reported by the project) |
| LibAtem | C#, .NET | LGPL-3.0 | Self-described as incomplete | [F-22] (reported by the project) |

If option 2 is added, the library choice brings a language runtime into the image. Licence and runtime would then be open (OQ-084, OQ-087, OQ-089).

**Traces.** REQ-ATEM-001, REQ-CAP-008. RISK-008; RISK-018 applies only if option 2 is added. TEST-ATEM-001 (verifies REQ-ATEM-001 and REQ-CAP-008); the camera sources are covered by TEST-CAP-001 and TEST-CAP-004. Models: OQ-102.

**Status.** NOT STARTED.

### 5.8 Audio capture (required since the owner decision of 2026-10-07; heading read "conditional" until 2026-10-08)

**Applies only if the owner decides that audio is required (OQ-004; REQ-CAP-006, PROPOSED).** *(Superseded: the owner decided on 2026-10-07 that audio is required — OQ-004 ANSWERED, REQ-CAP-006 DRAFT. This responsibility is mandatory. The original inputs, outputs and open items are kept below; the responsibility as extended on 2026-10-08 follows them.)*

**Inputs.**

- 2-channel I2S from the TC358743, which is the audio output the Linux driver always configures [A-13], through the `tc358743-audio` overlay and ALSA card `tc358743` [A-47], [G-14]. The silicon can also send audio over CSI-2 [A-05], but the driver does not configure that path. Reasoning: the I2S signals are wiring separate from the CSI-2 cable (OQ-025).
- Status controls "Audio present" and "Audio sampling rate" on the sub-device [B-16].

**Outputs.**

- AAC for RTMP/FLV [F-31]. The recording audio codec depends on the container, which is UNDEFINED (OQ-006). *(Superseded in part 2026-10-09: the container is MP4, decided by the owner on 2026-10-08 (ADR-009, ACCEPTED); the recording audio track is AAC as in section 5.4. OQ-006 stays OPEN for the maximum recording duration.)* *(Superseded 2026-10-09, later: OQ-006 ANSWERED — until stopped or the disk is full (owner, 2026-10-09).)*
- Opus or G.711 for WebRTC [F-41].

**Open.**

- Encoder choice and cost (OQ-063).
- Pi 5/CM5 overlay behaviour (OQ-054).
- Wiring: UNKNOWN — VERIFICATION REQUIRED; VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED (OQ-025).
- RISK-014.

#### 5.8.1 Responsibility as extended on 2026-10-08 (research topic I)

**Purpose.** Capture the HDMI source's embedded audio at the rate the source actually sends, keep it aligned with the video, encode it for each output, and hand it to the recorder (5.4), RTMP publisher (5.5) and WebRTC server (5.6). The kernel-side path is in [ARCHITECTURE.md](ARCHITECTURE.md) section 3.11.

**Traces.** REQ-CAP-006 (DRAFT), REQ-REC-001, REQ-STR-001, REQ-STR-002. ADR-004 (OPEN; CM5 audio is a bring-up gate in its analysis), ADR-007 (OPEN). RISK-014, RISK-019, RISK-023, RISK-024. TEST-AUD-001.

**Data path.**

```text
TC358743 sub-device (5.2)                    ALSA card "tc358743" [I-15]
  "Audio sampling rate" 0x00981980            hw:CARD=tc358743,DEV=0
  "Audio present"       0x00981981 [I-20]       CM4: bcm2835-i2s, 2 ch [I-13]
  V4L2_EVENT_CTRL on change [I-21]              CM5: RP1 I2S1, unconfirmed [I-14] (OQ-054)
          |                                             |
          |  rate / present / change                    |  PCM frames, labelled with the
          v                                             v  rate the application opened
  +--------------------------------------------------------------------+
  | Audio capture (5.8): open ALSA at the source's rate; reopen on     |
  | change; timestamp; resample where an encoder needs it [I-47]       |
  +-------------+------------------------------+-----------------------+
                | AAC (recording, RTMP)        | Opus (WebRTC)
                v                              v
         5.4 Recorder, 5.5 RTMP          5.6 WebRTC
```

**Required behaviour (from sources).**

1. **Open the right device.** Open the card by id (`hw:CARD=tc358743,DEV=0`, or `plughw:` / `sysdefault:`) rather than by index or PCM name [I-15]. The card index was reported to differ between systems [I-16] (reported; community source), and the PCM name differs on CM5 (reasoning from source) [I-17]. The `tc358743-audio` overlay must be loaded together with `tc358743`: official documentation says audio needs it in addition [C-37], and a Raspberry Pi engineer stated that it "*requires* dtoverlay=tc358743 to be loaded too" [I-16] (reported; community source).
2. **Track the source sample rate — this is PACSCORDER's job, not the kernel's.**
   - Reasoning from source: no kernel path carries HDMI sample-rate changes into ALSA. The stub codec has no controls and no `hw_params`, the CPU I2S runs as clock consumer and ignores the requested rate, and nothing outside `tc358743.c` uses the rate control [I-18]. On CM4 the rate requested through ALSA never reaches the hardware in this mode [I-13].
   - Consequence (reasoning, [I-18]): opening at 48000 Hz while the source sends 44100 Hz labels the frames 48 kHz; they play 8.84 % fast and A/V drift accumulates, with no ALSA error (RISK-023).
   - Therefore: get the rate from the capture control service (5.2, item 7) before opening ALSA; open ALSA at that rate; on every `V4L2_EVENT_CTRL` rate change, close and reopen at the new rate [I-20], [I-21]. Treat rate 0 as "no signal" [I-19].
   - Detection latency is up to about 1 s, plus I2C time, without a wired INT pin [I-22]. Reasoning from [I-18], [I-22]: audio captured in that window carries the old rate label; how to handle it is part of OQ-111.
   - Alternative or complement: restrict the EDID to one rate (research design risk, topic I — not a register fact; OQ-002, OQ-111). Policy: OWNER DECISION REQUIRED (OQ-111).
3. **Channels and sample format.** Stereo only with the stock driver: it hard-codes 2-channel I2S at probe [I-24]. On CM4 capture is exactly 2 channels in S16_LE, S24_LE or S32_LE [I-13]; on CM5 the channel count and formats come from RP1 hardware registers [I-14] (OQ-054). How 24-bit HDMI samples sit in the 32-bit slots, and what arrives with compressed or multichannel input, is unknown: DATASHEET REQUIRED; HARDWARE TEST REQUIRED (OQ-110).
4. **Keep audio and video aligned (A/V synchronisation design inputs).**

   | Clock or timestamp | Behaviour | Facts |
   |---|---|---|
   | Audio sample clock | Follows the HDMI source's audio clock (TC358743 audio PLL tracks N/CTS) | [I-26] |
   | Video buffer timestamps | `CLOCK_MONOTONIC`, frame start, on every Raspberry Pi CSI receiver driver | [I-33] |
   | ALSA status timestamps | Clock chosen by the application; alsa-lib 1.2.14 (trixie) switches newly opened `hw` PCMs to monotonic when the kernel PCM protocol is 2.0.9 or later | [I-34] |
   | GStreamer 1.26.2 `alsasrc` | Uses ALSA driver timestamps only when the element clock is a monotonic `GstSystemClock`; defaults provide-clock=TRUE, slave-method=skew, buffer-time 200 ms, latency-time 10 ms | [I-35] |
   | GStreamer pipeline clock | In a `v4l2src` + `alsasrc` pipeline, `alsasrc`'s audio clock normally becomes the pipeline clock, so ALSA driver timestamps are then not used | [I-36] |
   | GStreamer 1.26.2 `v4l2src` | PTS = pipeline clock time minus the measured age of the V4L2 buffer; after a bad timestamp it assumes a one-frame delay for the rest of the session | [I-37] |
   | FFmpeg 7.1 inputs | ALSA input stamps packets with wall-clock time; V4L2 input passes monotonic timestamps through by default; without `-ts abs` or `mono2abs` the bases are mixed, and the CLI's default start shift discards the real offset between inputs | [I-38] |

   Required of whichever implementation is chosen (reasoning from the inputs above): put audio and video on one documented clock base, or record the mapping between them; do not rely on a framework default without measuring it. Which arrangement holds over hours, and the A/V tolerance, are OQ-112 (HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED for the tolerance). A sample-rate mismatch (item 2) adds drift (RISK-023, RISK-024).
5. **Encode for each output.**

   | Output | Codec | Encoders available in Raspberry Pi OS | Facts |
   |---|---|---|---|
   | Recording (container OQ-006; MP4 since the owner decision of 2026-10-08, ADR-009 — noted 2026-10-09), RTMP | AAC | FFmpeg native `aac` (default AAC encoder, not experimental; 128 kb/s stereo without `-b`; explicit `-b` selects CBR) (CORRECTED); GStreamer `voaacenc` (S16LE, 8–96 kHz, 1 or 2 channels) and `avenc_aac` (F32LE, 7.35–96 kHz), which wraps FFmpeg's native encoder | [I-39], [I-40], [I-44], [I-46], [I-47] |
   | WebRTC | Opus | FFmpeg `libopus`; GStreamer `opusenc`. Both accept only 48, 24, 16, 12 or 8 kHz; resample a 44.1 kHz source first | [I-41], [I-45], [I-47] |
   | Not usable | `fdk-aac` | Not in the Raspberry Pi FFmpeg or GStreamer packages; Debian ships it in non-free under a licence it calls incompatible with every GPL version; in a GPL FFmpeg build it needs `--enable-nonfree`, which makes the result unredistributable | [I-39], [I-42], [I-43], [I-44] |

   Reasoning from [H-26] and [F-41]: FFmpeg's FLV muxer has no Opus and WebRTC needs Opus or G.711, so simultaneous RTMP and WebRTC with audio need two audio encodes, AAC and Opus. Their CPU cost next to software video encoding is unmeasured (research gap, topic I; OQ-063). *(2026-10-08: unchanged by the owner's answer to OQ-005, which separates the video encodes only. Audio is still two encodes: one AAC encode for 5.4 and 5.5, one Opus encode for 5.6. Their cost next to two video encodes is unmeasured (OQ-063).)*
6. **Pins (for configuration and diagnostics).** The overlay's pin group claims GPIO 18–21, including GPIO 21, which the audio path does not use [I-30]; overlays such as `pwm`, `pwm-2chan` and `gpio-ir` default to GPIO 18 [I-31] (OQ-114). The carrier's GPIO voltage must match VDDIO2, or the lines need level shifting [I-27], [I-29] (OQ-024).

**Platform differences.**

| | CM4 | CM5 |
|---|---|---|
| CPU I2S | `bcm2835-i2s`, GPIO 18–21 ALT0 [I-10] | RP1 I2S1, GPIO 18–21, function `i2s1` [I-07], [I-08] |
| Status | Documented path [I-10], [I-13] | Labels resolve and the firmware does not block the overlay [I-05], [I-06], [I-09]; no source shows capture (research gap, topic I; OQ-054). Bring-up gate for ADR-004. |
| Audio controls on | `/dev/videoN` (legacy Unicam) or the sub-device (Media Controller mode; reasoning from [I-23]) | Sub-device only [I-23] |
| Expected PCM name | `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15] | `1f000a4000.i2s-dir-hifi dir-hifi-0` (reasoning from source) [I-17] |

Pi 4 Model B and Pi 5 were not researched separately in topic I: NEEDS VERIFICATION.

**Candidate implementations (CANDIDATE — not decided; ADR-007).**

| Candidate | Capture | Encoders | Notes |
|---|---|---|---|
| GStreamer | `alsasrc` from `gstreamer1.0-alsa` 1.26.2-1+rpt3+deb13u2 [I-45] | `voaacenc` (plugins-bad) [I-44], `avenc_aac` (`gstreamer1.0-libav`) [I-46], `opusenc` (plugins-base) [I-45]; `audioresample` before `opusenc` for 44.1 kHz [I-47] | Clock model per item 4 [I-35], [I-36], [I-37] |
| FFmpeg | ALSA input device [I-38] (whether the Raspberry Pi build includes it: NEEDS VERIFICATION — BUILD TEST REQUIRED) | Native `aac` [I-40] (CORRECTED), `libopus` [I-41] | Timestamp bases per item 4 [I-38]; its FLV has no Opus [H-26] |
| Direct ALSA application | alsa-lib 1.2.14 [I-34] | Linked libraries (encoder choice as above) | Reasoning from [I-33], [I-34]: one monotonic timestamp clock for audio and video |

**Open.** OQ-025, OQ-054, OQ-063, OQ-110, OQ-111, OQ-112, OQ-113, OQ-114, OQ-024, OQ-020 (INT would shorten rate detection), OQ-090 (which process owns the sub-device and the ALSA device).

**Status.** NOT STARTED. TEST-AUD-001: BLOCKED — HARDWARE REQUIRED.

### 5.9 Configuration

**Purpose.** Hold every product setting, and keep it across reboot and update.

**Settings that must be either configurable or set by an owner decision:**

| Setting | Decided by | OQ / ADR |
|---|---|---|
| Lane configuration of the unit: 2-lane or 4-lane (REQ-CAP-007) | Owner / hardware | OQ-011 (ADR-004, per configuration), OQ-021 |
| EDID file and supported-mode list, per lane configuration (REQ-CAP-007) | Owner | OQ-002 |
| Capture pixel format | ADR-005 (PROPOSED) | OQ-003 |
| Platform, connector, lanes (overlay parameters [G-12], [G-13]) | Owner / hardware | OQ-011, OQ-021 |
| Encoder codec, bitrate, latency, number of encodes | Owner | OQ-005. *(Codec decided 2026-10-07: H.264 and H.265; the codec per output is OQ-103.)* *(2026-10-08: OQ-103 ANSWERED — H.264 on every output, so the codec is decided for the current scope; H.265 deferred, REQ-ENC-002. Bitrate, latency and number of encodes remain OQ-005.)* *(2026-10-08, later: number of encodes decided — two, recording and shared live ("Separate record + live", answer to OQ-005). Bitrate, rate control and latency remain OQ-005; reasoning: they are set for each of the two encodes.)* *(2026-10-09: latency target decided by the owner on 2026-10-08 — under 1 s for WebRTC viewers, RTMP best-effort, OQ-116 ANSWERED; bitrate and rate control remain OQ-005.)* |
| *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* H.265 encoder settings (preset, tune, threads) per framework | Design, after measurement | OQ-104 |
| *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* HEVC-over-RTMP path (FFmpeg, newer GStreamer, or SRT) | ADR-007 | OQ-107, OQ-076 |
| Recording container, storage, duration | Owner | OQ-006. *(2026-10-09: container and storage decided by the owner on 2026-10-08 — fragmented MP4 mirrored to an NVMe SSD and a self-powered USB-to-SATA HDD, ADR-009 ACCEPTED; the maximum duration remains OQ-006.)* *(2026-10-09, later: duration decided by the owner — until stopped or the disk is full, no fixed limit; OQ-006 ANSWERED. Follow-up: OQ-129.)* |
| *(Added 2026-10-09; ADR-009.)* Fragment duration or rule; finalising a cleanly stopped file as a normal MP4; file splitting *(2026-10-09, later: file splitting for long recordings is also asked in OQ-129)* | Design, after TEST-REC-001; owner for the players and editors that must open the files | OQ-118 |
| *(Added 2026-10-09; ADR-009.)* HDD-queue size and overflow policy; HDD spin-down | Design, after BUILD and HARDWARE tests | OQ-117 |
| *(Added 2026-10-09; ADR-009.)* Recording-volume filesystem and mount options (ext4 is Claude's proposal inside ADR-009, not an owner decision) | Owner (ext4, or exFAT for exchange) / design | OQ-120 |
| *(Added 2026-10-09; ADR-009.)* NVMe SSD, CM4 PCIe adaptor, HDD enclosure and USB-to-SATA bridge; `usb-storage.quirks` entry if the bridge needs one [J-26] | Owner / hardware | OQ-121, OQ-122 |
| *(Added 2026-10-09; ADR-009.)* CM5: `dtparam=pciex1` in `config.txt` for the M.2 link [J-15], [J-17] | Image content (REQ-BLD-002), after hardware test | OQ-124 |
| *(Added later on 2026-10-09; after the owner's duration answer to OQ-006.)* Behaviour when one mirrored drive fills, is absent or fails before the other; operator notification; splitting long recordings into several files | Owner / design | OQ-129 |
| RTMP destinations, keys, RTMPS | Owner | OQ-007 |
| WebRTC reach, signalling, ICE servers | Owner | OQ-008, OQ-074. *(2026-10-09: internet path, advertised addresses and STUN/TURN: OQ-128.)* *(2026-10-09, later: reach decided by the owner — LAN only (OQ-008); the internet path, OQ-128, is not in current scope. Signalling stays open (OQ-074).)* |
| *(Added 2026-10-09.)* Live latency target | Owner — decided 2026-10-08: under 1 s camera-to-viewer for WebRTC viewers; RTMP best-effort. *(2026-10-09, later: judged at the 95th percentile — 95 % of samples under 1 s, sustained run, recording running; sample count and run length still open, OQ-008.)* | OQ-116 (ANSWERED); OQ-008 (judging criterion; added 2026-10-09) |
| *(Added 2026-10-09; OQ-116.)* Live keyframe interval and on-demand keyframes | Design, after test | OQ-127 |
| *(Added 2026-10-09; OQ-116.)* Latency value of every live-path element, and the live-path queue policy | Design, after test | OQ-126 |
| ATEM integration scope | Owner — decided 2026-10-07: HDMI capture only, so no ATEM network address is needed in the current scope | OQ-009 (ANSWERED) |
| Supported ATEM and camera models (REQ-CAP-008) | Owner — decided 2026-10-07: no model list (any HDMI camera, plus ATEM outputs), so there is no model setting | OQ-102 (ANSWERED) |
| Audio on/off | Owner — decided 2026-10-07: audio is required, so there is no on/off setting for the product | OQ-004 (ANSWERED) |
| *(Added 2026-10-08.)* Audio sample-rate policy (follow the source and reopen ALSA, or a single-rate EDID) and output sample rates | Owner / design | OQ-111 |
| *(Added 2026-10-08.)* A/V synchronisation tolerance | Owner | OQ-112 |
| *(Added 2026-10-08.)* Audio encoders and bitrates per output | Owner / design | OQ-063 |
| *(Added 2026-10-08.)* Use of GPIO 18–21 when audio is enabled | Owner / hardware | OQ-114 |

**Known from sources.**

- **Boot-time configuration** is `config.txt` on the boot partition at `/boot/firmware/` [G-11] (CORRECTED). Model filters can hold per-board settings in one file [G-71] (CORRECTED; reasoning).
- **Lane configuration (REQ-CAP-007).** Both 2-lane and 4-lane configurations are required, so the configuration must record which one a unit has. Reasoning: the overlay parameters (5.1), the EDID and supported-mode list (OQ-002) and the capture control service's mode policy (5.2) all depend on it. Model filters select by board model [G-71], so they cannot tell two lane configurations on the same model apart (for example CM4 CAM0 and CAM1). How a unit's lane configuration is recorded is UNDEFINED — decision needed: part of the configuration design (OQ-092); no separate OQ. The wiring itself is OQ-021.
- **Persistence options:**
  - read-only root with `overlayroot=tmpfs` [G-48];
  - the `image-rota` persistent partition [G-40] and its slot-shared paths [G-41].

**UNDEFINED — decision needed:**

- runtime configuration format and store (OQ-092);
- validation, and how secrets (RTMP stream keys) are stored: part of the configuration design, which OQ-092 covers; no separate OQ;
- how an operator changes settings (OQ-091). **No requirement defines an operator interface** (web UI, API or physical controls).

**Status.** NOT STARTED.

### 5.10 Logging and diagnostics

**Purpose.**

- Make failures diagnosable (Rule 25; [TROUBLESHOOTING.md](TROUBLESHOOTING.md)).
- Provide the measurements that REQ-PERF-001 and TEST-PERF-001 need.

**Kernel message signatures** the diagnostics should surface. These come from driver and receiver source; none has been seen on PACSCORDER hardware.

| Message | Meaning | Facts |
|---|---|---|
| `not a TC358743 on address 0x%x` (8-bit form, so 0x1e for 0x0f) | Chip-ID check failed; probe returns `-ENODEV` | [A-19], [A-16] |
| `failed to get refclk` / `unsupported refclk rate: %u Hz` | Reference-clock problem in the DT. An unsupported rate is followed by a kernel BUG if the chip answers. | [A-22] (CORRECTED) |
| `untested bps per lane` | Link frequency is not 297 or 486 MHz | [A-24] |
| `Device has requested %u data lanes, which is >%u configured in DT` | Mode needs more lanes than configured; stream start fails | [C-16] |
| `subdevice requires %u data lanes when %u are supported` | Unicam endpoint lists more lanes than the instance supports | [C-17] |
| `Unable to determine sensor link rate, using 999 Mbps` | Expected on Pi 5/CM5 with this bridge | [C-31] |
| STREAMON fails with `-EPIPE` on Pi 5 | `csi2` pad formats mismatched, for example without `field:none` (reported) | [C-33] (reported; community source) |

**Tools.**

- `v4l2-ctl`, `media-ctl`, `v4l2-compliance` and `cec-ctl` are installed in Raspberry Pi OS Lite [G-24].
- `i2c-tools` is in the archive but not installed [G-25].
- Buildroot equivalents: [E-29], [E-30].
- The driver implements a `log_status` operation [B-38] and creates debugfs InfoFrame entries [B-15] (OQ-042). How to invoke them: NEEDS VERIFICATION.

**Metrics for REQ-PERF-001.**

- CMA headroom: `CmaFree` in `/proc/meminfo`, the measurement named in [C-53] (CORRECTED; reasoning from kernel source).
- Frame drops, CPU load, temperature and CSI-2 error counters: the method is UNDEFINED, and where CSI-2 errors can be read is NEEDS VERIFICATION (RISK-011; OQ-050 for RP1 CFE on Pi 5/CM5, OQ-095 for Unicam on Pi 4/CM4).
- *(Added 2026-10-08.)* Audio: the source sampling rate and audio-present state read from the TC358743 controls [I-20], the rate at which ALSA was opened, and each reopen after a rate change (RISK-023). A/V offset and drift over long runs (RISK-024): the measurement method is part of OQ-112 (HARDWARE TEST REQUIRED).
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* CPU load per encode when H.264 and H.265 run together (OQ-104, RISK-022).
- *(Added 2026-10-08; owner answer to OQ-005.)* For the two H.264 encodes, recording and live:
  - frame rate achieved, frame drops and latency of each encode, and of both together;
  - CPU load on Pi 5/CM5 (OQ-059, RISK-003);
  - encoder throughput on Pi 4/CM4 (OQ-115, RISK-002).

  TEST-PERF-001 is to measure the combined load. The measurement method is UNDEFINED.
- *(Added 2026-10-09; ADR-009 / OQ-116; candidates to measure, method UNDEFINED.)* Recorder (5.4.1):
  - per writer, write latency and errors; the HDD-queue fill level; data dropped and whether each copy is complete (RISK-028, OQ-117);
  - kernel-log resets and I/O errors on the USB-to-SATA bridge, and the bound driver (`uas` or `usb-storage`) per board (RISK-027, OQ-122);
  - on CM4, the NVMe interrupt mode (OQ-123).
- *(Added 2026-10-09; OQ-116.)* Live path (5.6.1):
  - measured camera-to-viewer latency on CM4 and CM5 with the recording encode running (OQ-125, RISK-031); GStreamer's own latency report is not a measurement, because `v4l2h264enc` on BCM2711 reports 0 [K-32];
  - viewer-side buffering from the W3C statistics API, `jitterBufferDelay`, `jitterBufferEmittedCount`, `jitterBufferTargetDelay`, `jitterBufferMinimumDelay` and `totalProcessingDelay` (CORRECTED) [K-12];
  - MediaMTX's "reader is too slow" log line, written when its outgoing write queue is full and drops packets [K-44].

**Audio tools (added 2026-10-08).** A 2019 forum thread shows the card listed as `card 0: tc358743 [tc358743], device 0: bcm2835-i2s-dir-hifi dir-hifi-0` in a Raspberry Pi engineer's test, and a capture run with `arecord -vv -d 20 -r 48000 -c 2 -f dat -t wav -D sysdefault:CARD=tc358743 out.wav` [I-16] (reported; community source). **NOT YET RUN ON PACSCORDER HARDWARE.** Which package provides `arecord` on Raspberry Pi OS, and whether it is preinstalled in the Lite image, is not in the register: NEEDS VERIFICATION.

**UNDEFINED — decision needed:**

- log transport and retention (OQ-092);
- remote access to logs (OQ-092);
- health reporting to the operator (OQ-091).

**Status.** NOT STARTED.

### 5.11 Update mechanism

**Purpose.** Update units in the field (REQ-BLD-001, REQ-BLD-002; [RELEASE.md](RELEASE.md)). The product runs its own project-built OS image (REQ-BLD-002, DRAFT; owner, 2026-10-07), so updates deliver that image, not a stock distribution image. The build tool is decided by ADR-003, ACCEPTED 2026-10-07 (Raspberry Pi OS Lite for bring-up, `rpi-image-gen` for the product image, Buildroot as the alternative; OQ-012 ANSWERED). The update mechanism is undecided (OQ-069).

**Options from sources (none chosen):**

| Option | Facts | Notes |
|---|---|---|
| `rpi-image-gen` `image-rota` A/B slots | [G-40], [G-41] | Immutable A/B slots and a persistent partition; EROFS, dm-verity and LUKS2 options |
| Firmware `autoboot.txt` + tryboot | [G-50], [G-51] (CORRECTED) | Firmware-level A/B partition selection only; needs redundant partitions and update tooling |
| EEPROM A/B | [G-52] | Pi 5 / CM5 only |
| Raspberry Pi Connect Remote Update | [G-43] (CORRECTED), [G-42] | A/B boot updates need a Raspberry Pi 4 or later that is connected to the internet, opted in and signed in to Connect, with storage of at least 16 GB. Personal-account deployments need the device online; Connect for Organisations devices update the next time they sign in. Compute Module support unconfirmed (OQ-069) |
| RAUC / SWUpdate / Mender | [G-49] | Packaged in Debian trixie |
| Buildroot whole-image upgrade | [G-67] | Only if Buildroot is chosen (ADR-003 alternative) |

**Constraints.**

- `apt full-upgrade` also updates kernel and firmware [G-08] (RISK-017).
- `rpi-update` installs pre-release firmware that can leave a unit unbootable [G-09], and it is preinstalled [G-10] (OQ-071).
- Secure-boot provisioning writes OTP [G-45], [G-46] (OQ-071).
- How an update interacts with an active recording or stream is UNDEFINED — decision needed (OQ-094).

**Status.** NOT STARTED.

---

## 6. Undefined cross-cutting design decisions

Rule 22 (do not guess) forbids inventing these. Each stays open until an ADR in [DECISIONS.md](DECISIONS.md) records it.

| Topic | Status | Constraints known from sources | OQ |
|---|---|---|---|
| Media framework (GStreamer, FFmpeg, direct V4L2, or a combination) | OPEN — ADR-007 | Sections 5.3–5.6. ADR-007 says to decide after TEST-CAP-002 and TEST-ENC-001 on the candidate platforms. *(Added 2026-10-09.)* Per the ADR-007 Decision note in [DECISIONS.md](DECISIONS.md), the decision is also to record the live publishing route (RTSP to MediaMTX [K-05], or a self-built gst-plugins-rs for WHIP [K-06]; RISK-034), the explicit latency of every live-path element (OQ-126, RISK-032), the fragmented-MP4 settings (OQ-118) and the HDD-branch decoupling (OQ-117), measured with TEST-STR-002 and TEST-REC-001. *(Added 2026-10-08.)* According to sources, FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP [H-26], while GStreamer 1.26.2 `flvmux` cannot carry H.265 [H-27] (neither tested on PACSCORDER), so a GStreamer-based design may need FFmpeg for that output (RISK-025). *(Scope note 2026-10-08: H.265 is deferred — REQ-ENC-002 — so this constraint does not drive ADR-007 for the current scope (ADR-007 scope note); RISK-025 stays OPEN, not in current scope.)* | OQ-015; OQ-107 (not in current scope) |
| *(Added 2026-10-08.)* A/V clock model (which clock is the master; how audio and video timestamps are put on one base) | UNDEFINED | Section 5.8 item 4: [I-26], [I-33], [I-34], [I-35], [I-36], [I-37], [I-38] (RISK-024) | OQ-112 |
| *(Added 2026-10-08.)* Audio sample-rate policy | UNDEFINED | Section 5.8 item 2: [I-18] (reasoning), [I-20], [I-21], [I-22] (RISK-023) | OQ-111 |
| *(Added 2026-10-09; ADR-009.)* HDD-branch decoupling: queue size, overflow policy, and whether one muxer or two feed the writers | UNDEFINED | Section 5.4.1: [J-30] (CORRECTED); reasoning [J-36], [K-36] (RISK-028) | OQ-117 |
| *(Added 2026-10-09; OQ-116.)* Live-path buffering: the latency of every element and the queue policy | UNDEFINED | Section 5.6.1: [K-07], [K-08], [K-09], [K-37]; reasoning [K-10], [K-36] (RISK-032) | OQ-126 |
| Process model (one process or several services) | UNDEFINED | None from sources | OQ-090 |
| Threading model | UNDEFINED | `rpicam-apps` uses x264 frame threading in normal mode and slice threading in low-latency mode [D-35]; reasoning: the threading choice is tied to latency | OQ-090 |
| IPC between responsibilities (bitstream fan-out, control and status) | UNDEFINED | DMABUF file descriptors are the kernel-level frame-sharing mechanism (REQ-DMA-001). *(2026-10-08: fan-out is now one capture to two encoders, and one live bitstream to 5.5 and 5.6 (owner answer to OQ-005; section 5.3 "Fan-out").)* *(2026-10-09: and the recording stream to two file writers, the HDD writer decoupled (ADR-009; section 5.4.1).)* | OQ-090; OQ-115 (two encode sessions on CM4); added 2026-10-09: OQ-117 |
| Implementation language(s) | UNDEFINED | Only if ATEM network control (option 2 in 5.7, not in current scope) is added would the ATEM libraries imply Node.js, Python or .NET runtimes [F-14], [F-19], [F-21], [F-22] (reported; community source) | OQ-084 (ATEM part, only if option 2 is added); otherwise no dedicated OQ — to be decided with OQ-090 and ADR-007 (OQ-015) |
| Service supervision and start order (EDID before capture; capture before outputs) | UNDEFINED | Raspberry Pi OS Lite includes systemd [G-32]; Buildroot `/dev` management options [E-50] | OQ-090 (process model); OQ-093 (EDID trigger and ordering) |
| Configuration format and store; operator interface | UNDEFINED; no requirement exists for an operator interface | [G-40], [G-48] (persistence only) | OQ-092 (configuration); OQ-091 (operator interface) |
| Logging transport | UNDEFINED | — | OQ-092 |
| Software updates while recording or streaming | UNDEFINED | `apt full-upgrade` updates kernel and firmware [G-08]; A/B options in section 5.11 | OQ-094 |
| Licensing limits on the choices | OPEN | x264 GPL [D-47]; `webrtcbin` LGPL [F-42]; `webrtcsink` MPL-2.0 [F-43]; MediaMTX MIT (reported) [F-44]; ATEM libraries MIT / GPL-3.0 / LGPL-3.0 (reported; relevant only if ATEM option 2 is added) [F-14], [F-19], [F-21], [F-22]. *(Added 2026-10-08.)* x265 GPLv2-or-later or commercial, neither covering HEVC patents [H-39]; HEVC patent pools per unit [H-40], [H-41], [H-42]; the Raspberry Pi FFmpeg is a GPL build because it links `libx264` and `libx265` [I-39]; `fdk-aac` is GPL-incompatible and would make a GPL FFmpeg unredistributable [I-42], [I-43]; AAC patent licensing not researched. *(2026-10-08, H.265 deferred — REQ-ENC-002: the HEVC patent items are not in current scope; x265's own licence still applies wherever the Raspberry Pi FFmpeg ships, because `libavcodec61` depends on `libx265-215` [H-09].)* | OQ-087, OQ-084; added 2026-10-08: OQ-109 (not in current scope), OQ-113 |

---

## 7. Platform differences (userspace view)

| Concern | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Lane configuration candidate (REQ-CAP-007; platform per configuration: ADR-004, OPEN) | 2-lane | CAM1 4-lane; CAM0 2-lane | 4-lane | 4-lane |
| Overlay | `tc358743` [G-12] | `tc358743` (+`4lane` on CAM1) [G-12] | `tc358743-pi5`, auto-selected [C-11] | `tc358743-pi5`, auto-selected [C-11] |
| Control node for EDID / DV timings | Sub-device if MC mode (ADR-006); video node in legacy mode [B-25] (CORRECTED) | Same | Sub-device only [B-25] (CORRECTED) | Same as Pi 5 |
| Graph setup by userspace | None (link immutable) [C-36] (CORRECTED) | None [C-36] (CORRECTED) | Enable link; set pad formats [C-32], [C-33] (reported; community source) | Same as Pi 5 |
| 1080p60 capture possible (bandwidth reasoning; a 4-lane port is necessary but not shown sufficient for UYVY, which uses 3 of 4 lanes at 972 Mbit/s: OQ-038, ADR-008) | No [C-48]; the 2-lane configuration carries up to 1080p50 UYVY / 1080p30 RGB888 [C-37] | CAM1 by bandwidth [C-49] | By bandwidth [C-49]; 4 lanes on CAM/DISP0 (`cam0`) NEEDS VERIFICATION (OQ-049) | By bandwidth [C-49]; carrier-dependent (OQ-052) |
| Encoder | Hardware `/dev/video11` [D-06] | Hardware `/dev/video11` [D-06] | Software [G-22] | Software [D-31] |
| Two H.264 encodes, recording + live (owner, 2026-10-08; OQ-005; row added 2026-10-08) | Both on `/dev/video11`; two at once unproven (OQ-115; RISK-002) | Same | Both in software; roughly double the encode CPU load (reasoning from [G-22]; OQ-059; RISK-003) | Same as Pi 5 |
| UYVY into the encoder | Accepted directly, as reported [D-18] (reported; community source) | Same | Conversion needed [D-43] | Conversion needed [D-43] |
| DMABUF meaning | Zero-copy import into the encoder is the target (OQ-058). *(2026-10-08: into two encode sessions, OQ-115.)* | Same | Sharing with the CPU converter/encoder. *(2026-10-08: two software encoders, OQ-059.)* | Same as Pi 5 |
| Audio overlay | `tc358743-audio` [A-47] | Same | Unconfirmed (OQ-054) | Unconfirmed (OQ-054) |
| Firmware A/B for EEPROM updates | Not supported [G-52] | Not supported [G-52] | Supported [G-52] | Supported [G-52] |
| Bring-up board (owner, 2026-10-07, second answer; added 2026-10-08) | No (documented candidate) | **Yes** | No (documented candidate) | **Yes** |
| H.265 encoder (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | Software (x265; Cortex-A72, no DotProd) [D-24], [H-05] | Same | Software (x265; DotProd can apply — reasoning, same BCM2712 core as CM5) [D-31], [H-05] | Software (x265; DotProd can apply) [D-31], [H-05] |
| UYVY into the H.265 encoder (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | Conversion needed [H-10] (CORRECTED), [H-13] | Same | Same | Same |
| CPU I2S behind the audio overlay (added 2026-10-08) | Not researched in topic I (NEEDS VERIFICATION) | `bcm2835-i2s`, 2 channels [I-10], [I-13] | Not researched in topic I (NEEDS VERIFICATION) | RP1 I2S1; capture unconfirmed [I-07], [I-09] (OQ-054) |
| Node with the audio controls (added 2026-10-08) | As CM4 (reasoning: same Unicam driver) | `/dev/videoN` in legacy Unicam mode; sub-device in Media Controller mode (reasoning) [I-23] | As CM5 (reasoning: same RP1 CFE driver) | Sub-device only [I-23] |
| Expected ALSA PCM name (added 2026-10-08) | NEEDS VERIFICATION | `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15] | NEEDS VERIFICATION | `1f000a4000.i2s-dir-hifi dir-hifi-0` (reasoning) [I-17] |
| Recording NVMe SSD (ADR-009; added 2026-10-09) | Not researched in topic J (NEEDS VERIFICATION; OQ-098) | CM4 IO Board PCIe Gen 2 x1 socket through a passive adaptor; `/dev/nvme0n1` [J-03], [J-11]; slot needs the 12 V input [J-05] (RISK-026; OQ-121, OQ-123) | Not researched in topic J beyond NVMe boot and Gen 3 [J-10], [J-16] (NEEDS VERIFICATION; OQ-098) | CM5 IO Board M.2 at PCIe Gen 2 x1; link disabled by default, `dtparam=pciex1` [J-13], [J-15], [J-17] (OQ-121, OQ-124) |
| Recording HDD (ADR-009; added 2026-10-09) | Not researched in topic J (NEEDS VERIFICATION; OQ-098) | USB 2.0 through the IO Board hub, about 1.2 A of VBUS shared by its ports [J-06]; the USB 2.0 path is shared with every other USB device (CORRECTED; reasoning) [J-09]; `uas` can bind under `otg_mode=1` but not behind `dwc2` [J-24], [J-07] (RISK-026, RISK-027; OQ-122) | Not researched in topic J (NEEDS VERIFICATION; OQ-098) | USB 3.0 [J-18]; about 1.2 A of VBUS shared by the two IO Board ports [J-19] (RISK-027; OQ-122) |
| Live-encode latency handling (OQ-116; added 2026-10-09) | As CM4 (reasoning: same BCM2711 encoder driver) | No B-frames [K-30]; force-keyframe [K-31]; reports 0 latency to GStreamer [K-32] | As CM5 (reasoning: same BCM2712) | B-frames off and `zerolatency` set explicitly [K-27], [K-29] (CORRECTED); per-frame time unknown (OQ-059) |

---

## 8. Traceability (Rule 12)

| Requirement | Responsibility | Test | Implementation | Test status |
|---|---|---|---|---|
| REQ-ARCH-001 | 5.2, 5.3 | TEST-PLT-001, TEST-DMA-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-PLT-001 | 5.1 | TEST-PLT-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-DRV-001 | 5.1 (module load; no userspace register access) | TEST-HW-001, TEST-DRV-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-001 | 5.2, 5.3 | TEST-CAP-002 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-002 | 5.2 | TEST-CAP-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-003 | 5.1 | TEST-DRV-002 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-004 | 5.2 | TEST-CAP-003 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-005 | 5.2 | TEST-CAP-004 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-006 | 5.8; since 2026-10-08 also 5.2 (audio controls), 5.4–5.6 (audio tracks) | TEST-AUD-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-007 | 5.1, 5.2, 5.3, 5.9 | TEST-CAP-002, TEST-CAP-004 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-008 | 5.2, 5.7 | TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-DMA-001 | 5.3 | TEST-DMA-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-ENC-001 | 5.3 (H.264 only since 2026-10-08, when the owner deferred H.265; the H.265 parts of 5.3 and the HEVC-over-RTMP item of 5.5 now trace to REQ-ENC-002). *(2026-10-08, later: two encodes, recording and shared live (owner answer to OQ-005). TEST-ENC-001 is to include a two-encode run on CM4 and CM5, and TEST-PERF-001 the combined load.)* | TEST-ENC-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-ENC-002 (DEFERRED, 2026-10-08) | 5.3 and 5.5 H.265 material, kept as evidence; not in current scope | — (deferred; no planned tests) | NOT STARTED | — (deferred) |
| REQ-REC-001 | 5.4. *(2026-10-09: and 5.4.1, ADR-009 — two writers, HDD decoupled. TEST-REC-001 is to cover both drives, power cuts and injected HDD stalls, and TEST-PERF-001 the HDD-stall soak (RISK-028, RISK-030); scope notes, not accepted criteria.)* *(2026-10-09, later: duration — until stopped or the disk is full (owner; OQ-006 ANSWERED); behaviour when one drive fills, is absent or fails first, and file splitting: OQ-129, resolving test TEST-REC-001.)* | TEST-REC-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-STR-001 | 5.5. *(2026-10-09: latency best-effort, OQ-116 ANSWERED.)* | TEST-STR-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-STR-002 | 5.6. *(2026-10-09: and 5.6.1 — under 1 s camera-to-viewer, OQ-116 ANSWERED. TEST-STR-002 is to measure it on CM4 and CM5 with the recording encode running (OQ-125); scope note, not an accepted criterion.)* *(2026-10-09, later: owner decisions — judged at the 95th percentile, 95 % of samples under 1 s over a sustained run with the recording running; LAN viewers only; sample count and run length still open (OQ-008). Acceptance of REQ-STR-002 stays DRAFT.)* | TEST-STR-002 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-ATEM-001 (HDMI capture only; OQ-009 ANSWERED) | 5.7 (through 5.1–5.3) | TEST-ATEM-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |
| REQ-BLD-001 | 5.11 | TEST-BLD-001 | NOT STARTED | NOT STARTED |
| REQ-BLD-002 | 5.11 | TEST-BLD-001 | NOT STARTED | NOT STARTED |
| REQ-PERF-001 | 5.10 (measurements), all | TEST-PERF-001 | NOT STARTED | BLOCKED — HARDWARE REQUIRED |

---

## 9. Software vs implementation

- No userspace code exists as of 2026-10-06, so nothing can diverge from this document yet.
- When implementation starts, this document must be updated in the same change as the code (Rule 24), as must [ARCHITECTURE.md](ARCHITECTURE.md) (Rule 5). Each responsibility gets:
  - its chosen component, process and interface;
  - the ADR that chose it;
  - its status in Rule 10 words.

---

## Verification status

### Verified from sources (fact IDs)

The statements in this document rest on these entries of [REFERENCES.md](REFERENCES.md). Each has the verdict `CONFIRMED` or `CORRECTED`. "Verified from sources" means only that the cited source says so.

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-04, A-05, A-13, A-16, A-19, A-22, A-24, A-30, A-31, A-32, A-33, A-45, A-47 |
| B — tc358743 Linux driver | B-15, B-16, B-18, B-19, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-28, B-29, B-30, B-31, B-32, B-34, B-35, B-36, B-38, B-39, B-40, B-42 |
| C — Raspberry Pi CSI-2 receive path | C-09, C-10, C-11, C-12, C-13, C-16, C-17, C-18, C-28, C-29, C-31, C-32, C-33, C-34, C-36, C-37, C-39, C-40, C-42, C-43, C-48, C-49, C-53 |
| D — Encoders | D-06, D-07, D-08, D-11, D-12, D-13, D-14, D-15, D-17, D-18, D-19, D-20, D-21, D-22, D-23, D-25, D-28, D-29, D-31, D-32, D-33, D-34, D-35, D-36, D-37, D-38, D-39, D-40, D-41, D-42, D-43, D-44, D-45, D-47, D-48, D-50, D-53, D-54 |
| E — Buildroot and kernel configuration | E-29, E-30, E-32, E-43, E-47, E-48, E-49, E-50 |
| F — ATEM and streaming | F-01, F-06, F-07, F-10, F-11, F-14, F-15, F-16, F-17, F-18, F-19, F-20, F-21, F-22, F-23, F-24, F-25, F-26, F-27, F-28, F-31, F-32, F-33, F-34, F-35, F-36, F-37, F-38, F-39, F-40, F-41, F-42, F-43, F-44, F-45, F-46 |
| G — Raspberry Pi OS and image tooling | G-08, G-09, G-10, G-11, G-12, G-13, G-14, G-16, G-17, G-18, G-19, G-22, G-24, G-25, G-26, G-27, G-28, G-29, G-32, G-40, G-41, G-42, G-43, G-45, G-46, G-48, G-49, G-50, G-51, G-52, G-67, G-71 |
| D — added 2026-10-08 | D-24; later the same day, with the two-encode decision: D-10, D-52 |
| H — H.265/HEVC software encoding and transport (added 2026-10-08; since 2026-10-08 evidence for REQ-ENC-002, deferred, except [H-09] on the `libx265-215` dependency) | H-01, H-02, H-03, H-04, H-05, H-07, H-08, H-09, H-10, H-11, H-12, H-13, H-14, H-15, H-16, H-17, H-18, H-19, H-20, H-21, H-22, H-23, H-24, H-25, H-26, H-27, H-28, H-29, H-30, H-31, H-32, H-33, H-34, H-35, H-36, H-37, H-38, H-39, H-40, H-41, H-42, H-43 |
| I — HDMI audio path (added 2026-10-08) | I-02, I-04, I-05, I-06, I-07, I-08, I-09, I-10, I-11, I-13, I-14, I-15, I-16, I-17, I-18, I-19, I-20, I-21, I-22, I-23, I-24, I-26, I-27, I-29, I-30, I-31, I-33, I-34, I-35, I-36, I-37, I-38, I-39, I-40, I-41, I-42, I-43, I-44, I-45, I-46, I-47 |
| J — Recording storage and power loss (research of 2026-10-08; cited here from 2026-10-09) | J-02, J-03, J-04, J-05, J-06, J-07, J-09, J-10, J-11, J-13, J-14, J-15, J-16, J-17, J-18, J-19, J-21, J-24, J-25, J-26, J-27, J-28, J-29, J-30, J-33, J-35, J-36, J-38, J-39, J-40, J-41, J-44, J-45 |
| K — Live latency (research of 2026-10-08; cited here from 2026-10-09) | K-01, K-02, K-03, K-04, K-05, K-06, K-07, K-08, K-09, K-10, K-11, K-12, K-13, K-15, K-16, K-17, K-18, K-27, K-28, K-29, K-30, K-31, K-32, K-33, K-34, K-35, K-36, K-37, K-39, K-40, K-41, K-42, K-43, K-44, K-45 |

How each tier is used:

- **`CORRECTED` entries**, used in their corrected wording only: A-22, B-21, B-25, C-28, C-36, C-39, C-53, E-47, F-24, F-28, F-34, F-37, G-11, G-43, G-51, G-71. Added 2026-10-08: H-10, H-12, H-18, I-40. Added 2026-10-09: J-09, J-27, J-30, J-33, J-35, K-12, K-18, K-29, K-45.
- **`community` entries**, worded as reports: C-28, C-33, C-42, C-43, D-17, D-18, D-50, D-54, F-11, F-14, F-15, F-16, F-17, F-18, F-19, F-20, F-21, F-22, F-44, F-45. Added 2026-10-08: H-19, H-20, H-21, H-22, H-36, I-16. Added 2026-10-09: J-27, K-33.
- **`reasoning` entries**, labelled as reasoning: B-27, C-48, C-49, C-53, D-29, D-36, F-35, F-38, F-40, F-46, G-71. Added 2026-10-08: H-23, H-43, I-17, I-18; later the same day D-52. Added 2026-10-09: J-09, J-36, K-10, K-18, K-45, and the reasoning sentence of the `kernel-source` entry K-36.
- Statements marked *research gap*, *research open question* or *research design risk* for topics H and I come from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json); they are not register facts (added 2026-10-08). Since 2026-10-09, such statements for topics J and K come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json), on the same terms.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No userspace software exists. Every responsibility is `NOT STARTED`, and every hardware-dependent test is `BLOCKED — HARDWARE REQUIRED`. *(2026-10-09: unchanged. No recording has been written to either drive, and no latency has been measured.)*

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review against REFERENCES.md. Changes: aligned wording with the CORRECTED fact [G-43] and with [D-32], [D-50], [D-40], [F-36], [F-41], [F-34], [G-26], [E-50], [D-35]; labelled the Pi 5 sub-device read-write statement as reasoning; added the `4lane` + `cam0` NEEDS VERIFICATION note [B-42], [C-13]; used full unknown markers for OQ-011, OQ-021 and OQ-025; distinguished TEST-BLD-001 (`NOT STARTED`) from hardware-blocked tests. Added citation C-13. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: §5.2 required behaviour reordered into one explicit bring-up sequence (EDID → wait for signal → query/set DV timings → enable links → pad formats with `field:none` and colorspace → video-node format → STREAMON), per [C-33], [B-35] and [V4L2.md](V4L2.md) §6, with later steps renumbered and the source-change restart aligned to it; "decision needed — no OQ registered" replaced by OQ-090 (process model, threading, IPC), OQ-091 (operator interface), OQ-092 (configuration, logging), OQ-093 (EDID trigger) and OQ-094 (updates versus recordings); MediaMTX :8189 described as the `webrtcLocalUDPAddress` setting [F-44], with the ICE/RTP reading labelled reasoning; ATEM AAC wording aligned with [F-41] ("not a required WebRTC codec"); audio input notes that the driver configures I2S [A-13] while the silicon can also use CSI-2 [A-05], and the separate-wiring statement labelled reasoning; `v4l2-ctl -d <sub-device path>` and `--clear-edid` argument form marked NEEDS VERIFICATION (OQ-101); `config.txt` bare-boolean/combined-parameter syntax and `[pi4]`-matches-CM4 marked NEEDS VERIFICATION (OQ-100); link frequency linked to ADR-008 (PROPOSED, OQ-099); Unicam CSI-2 error counters linked to OQ-095; storage facts linked to OQ-098; §7 1080p60 row: 4-lane port necessary but not shown sufficient for UYVY (OQ-038) and Pi 5 CAM/DISP0 4-lane NEEDS VERIFICATION (OQ-049); lane-mismatch condition worded against the DT lane count. Added citations A-05, B-36. No requirement, decision, risk or test status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: summary block added under "Current state"; §5.7 ATEM integration reduced to HDMI capture (OQ-009 ANSWERED; REQ-CAP-008): no component of its own, no UDP 9910 service and no ATEM RTMP server in current scope, options 2–5 and the UDP 9910 libraries kept as reference marked "not in current scope", models OQ-102; component diagram redrawn (ATEM box, HDMI sources at the kernel boundary); §4 ATEM-control and RTMP interface rows, §5.5 ATEM-receive text and traces, §6 language and licensing rows conditioned on option 2; lane configuration (REQ-CAP-007) added as an input of §5.1 (`4lane` per configuration [C-12], [C-17]; EDID per configuration) and §5.2 (mode list and lane-count prediction; 2-lane limit [C-48]), as a §5.9 setting and as a §7 row; camera sources noted in §5.2 (OQ-102, RISK-009); own OS image (REQ-BLD-002) added to §1 and §5.11; §2 summary and §8 traceability extended with REQ-CAP-007, REQ-CAP-008, REQ-BLD-002. No requirement, decision, risk or test status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); "Owner decisions of 2026-10-07" summary block (ADR-003 ACCEPTED, OQ-012 ANSWERED); §1 scope list, §5.1 traces and §5.11 purpose now say ADR-003 ACCEPTED (§5.11: OQ-012 ANSWERED). Not changed: evidence and citations, the update-mechanism options (still none chosen, OQ-069), ADR-006 (PROPOSED), ADR-007 (OPEN), implementation status (NOT STARTED), Change-history rows. TEST-ATEM-001 appears here by ID only, so no title changed. | Claude (session 2026-10-07) |
| 2026-10-08 | Second set of owner decisions of 2026-10-07 and research topics H (H.265/HEVC) and I (HDMI audio) propagated. Header (Last updated, Applies to: CM4 + CM5 bring-up, Verification: topics H and I); new summary blocks for the second owner set and the 2026-10-08 research; OQ-102 "models" statements marked ANSWERED (summary block, §5.2, §5.7, §5.9); §1 three kernel-component rows (audio capture path [I-02], [I-11], [I-15]; audio controls [I-20], [I-21], [I-23]; `CLOCK_MONOTONIC` buffer timestamps [I-33]); §2 rows 5.3 and 5.8 (5.8 marked superseded: audio required); §3 diagram boxes updated (audio "required", "H.264/H.265 encode") with a note recording the old labels; §4 three interface rows (ALSA card details, audio controls, timestamp clocks); §5.1 audio-overlay bullet and EDID audio-descriptor row; §5.2 audio-rate output, new item 7 (read, subscribe and forward the audio rate) and traces; §5.3 codec input marked superseded, H.265 outputs, new "Required behaviour: H.265" (planar input, throughput evidence, SIMD, latency settings, concurrency, licensing) and H.265 candidate table; §5.4 HEVC containers [H-37], [H-38], audio track, FFmpeg-muxer NEEDS VERIFICATION superseded in part; §5.5 HEVC-over-RTMP framework constraint [H-24]–[H-30] (RISK-025, OQ-107), AAC encoders, `fdk-aac` excluded; §5.6 H.265 browser support [H-31]–[H-36], Opus encoders, H.264 track kept; §5.8 heading changed from "(conditional)" to "required" with the old applicability line kept and marked superseded, new §5.8.1 (data path, device selection, rate-tracking responsibility [I-18]–[I-22], channels, A/V synchronisation inputs [I-26], [I-33]–[I-38], encoders per output, pins, platform table, candidates); §5.9 nine settings rows added (six) or annotated (three); §5.10 audio and dual-encode metrics, `arecord` reference (community); §6 two rows added (A/V clock model, audio rate policy) and two extended (framework, licensing); §7 six platform rows; §8 REQ-CAP-006 and REQ-ENC-001 rows extended; Verification status gains D-24 and topics H and I. No requirement, decision, risk or test status changed; every responsibility is still NOT STARTED. | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic H and I additions: §6 media-framework row — "HEVC over RTMP works with FFmpeg 7.1.5" reworded to what [H-26] states (FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP), marked as not tested on PACSCORDER (Rule 10). All other [H-xx] and [I-xx] citations checked against the register; no change needed. No status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): second-set codec bullet marked superseded and new "Owner decision of 2026-10-08" summary block (all outputs take H.264 from 5.3; REQ-ENC-002 DEFERRED; RISK-022, RISK-025, OQ-104 to OQ-109 OPEN but not in current scope; ADR-007 OPEN with its scope note; CM4 + CM5, audio, any HDMI camera, ADR-003 unchanged); research block notes the deferral; §2 row 5.3 reworded; §3 pipeline box back to "H.264 encode" (alignment kept) and the diagram note records the "H.264/H.265 encode" label and the change; §5.3 purpose and codec input marked superseded (H.264 only), traces note RISK-022 not in current scope and REQ-ENC-002 DEFERRED, H.265 output, "Required behaviour: H.265" and H.265 candidate table labelled "deferred — REQ-ENC-002; not in current scope", Open list annotated (OQ-103 ANSWERED; OQ-104, OQ-105, OQ-108 not in current scope); §5.4 video input marked superseded (H.264 only), HEVC-in-container item and FFmpeg HEVC note labelled, Open list annotated; §5.5 traces, H.265 input, HEVC-over-RTMP item (with scope sentence), HEVC candidate and Open list labelled or annotated; §5.6 H.265 input, H.265 support and payloader rows, H.265 reasoning bullet and Open list labelled or annotated; §5.9 codec row annotated (codec fixed to H.264 in the current scope) and the two H.265 setting rows labelled; §5.10 dual-encode metric labelled; §6 framework row scope note (OQ-107 not in current scope) and licensing row note (HEVC patents not in current scope; x265 licence still applies via `libavcodec61` → `libx265-215` [H-09]; OQ-109 not in current scope); §7 two H.265 rows labelled; §8 REQ-ENC-001 row reworded (H.264 only) and a REQ-ENC-002 (DEFERRED) row added with no planned test; Verification status topic H row labelled. No other decision, status or citation changed. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): verifier pass — the 5.9 Configuration table note on the encoder codec, added earlier today, now says the codec is "decided" rather than "fixed" for the current scope (Rule 10 vocabulary; meaning unchanged). No other text, decision, status or citation changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): new "Owner decision of 2026-10-08, second" summary block (5.3 produces a recording encode for 5.4 and one live encode shared by 5.5 and 5.6; bitrate, rate control and latency still OQ-005; CM4 concurrency OQ-115, RISK-002, 2.0× reasoning [D-10], [D-52]; CM5 two software encodes OQ-059, RISK-003 [G-22]; live encode carries the WebRTC constraints [F-36], [F-39], [F-40], [F-45]; audio still AAC + Opus; H.265 still deferred; no ADR status changed); §2 rows 5.3 and 5.10 extended; §3 diagram: pipeline box "two H.264 encodes", recording arrow to 5.4 and one live arrow split to 5.5 and 5.6 (old labels recorded in the note); §5.3 purpose, traces note, input and output notes, new "Two encode sessions" item on Pi 4/CM4 (UNKNOWN, OQ-115; split options part of OQ-115), live-encode control values note [D-11], [D-14], [D-15], [D-37], [F-40], 1080p60 note, Pi 5/CM5 one-conversion-for-both reasoning [D-40], [D-43] and two-encode CPU bullet, B-frame note limited to the live encode, buffer hold-time reasoning (OQ-061, RISK-020), "Fan-out" marked superseded with the topology reasoning (two consumers per buffer, OQ-058, OQ-060, OQ-115; live split through a parser [F-34], [F-35]; ADR-007, OQ-090), Open list (OQ-115; OQ-005 stays OPEN); §5.4 input, platform-difference and Open notes; §5.5 input note (live encode; RTMP destination acceptance OQ-007) and Open list; §5.6 input note, platform-difference notes ([D-15]; OQ-115; OQ-059) and Open list; §5.8.1 item 5 audio note; §5.9 codec/encodes row note; §5.10 two-encode metrics (TEST-PERF-001 combined load); §6 IPC row note; §7 new two-encode row and DMABUF-meaning notes; §8 REQ-ENC-001 row note (TEST-ENC-001 two-encode run on CM4 and CM5; TEST-PERF-001 combined load); Verification status gains D-10 and D-52 (reasoning). No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — §5.3 Pi 5/CM5 CPU bullet "Two software encodes run at once" → "are required at once; sustaining them UNKNOWN"; §5.4 and §5.6 CM4 notes "both share" / "also runs" → "would share" / "would also run" (OQ-115); §5.3 H.265 concurrency bullet: "the same four cores" replaced — the BCM2712 core count is not in the source register. No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header (Last updated; Verification adds topics J and K); "Owner decision of 2026-10-08, second" latency line marked superseded in part; new "Owner decisions of 2026-10-08, third" block (< 1 s for WebRTC viewers only, RTMP best-effort, OQ-005 open for bitrate and rate control; ADR-009, ext4 marked Claude's proposal, not an owner decision, OQ-120; two writers with a decoupled HDD queue, OQ-117, RISK-028; unchanged decisions; sections changed) and topic J/K research block; §2 rows 5.4–5.6; §3 component diagram (recorder → NVMe writer + HDD queue/writer; candidate RTSP-to-MediaMTX live publisher; latency labels) with the old labels recorded; §4 Storage row superseded in part per board [J-03], [J-06], [J-11], [J-13], [J-15], [J-17], [J-18], WebRTC network row [K-41]–[K-43], new RTSP-publish row [K-05], [K-09]; §5.3 latency input superseded in part, new "live-encode latency settings" (CM4 [K-30]–[K-33]; CM5 [K-27], [K-28], `x264enc` defaults trap [K-29], [K-39]; [K-04], [K-17]), fan-out note on the recorder's two writers [K-34], [K-36], latency facts per candidate [K-37], Open list; §5.4 traces, outputs, OQ-006 items, platform and FFmpeg notes superseded in part, Open list, and new §5.4.1 (ADR-009 decisions, component diagram, required behaviour [J-29], [J-30], [J-33], [J-35], [J-36], [J-38], [J-45], open configuration parameters OQ-117/118/120/121/122/124, CM4/CM5 platform table [J-02]–[J-07], [J-09], [J-11], [J-13]–[J-19], [J-21], [J-24]–[J-28], candidate table GStreamer `mp4mux` `fragment-duration`/`fragment-mode` [J-39], plain `mp4mux` [J-38], robust muxing [J-40], [J-41], `splitmuxsink` [J-44], FFmpeg `+frag_keyframe`/`hybrid_fragmented` [J-45]); §5.5 best-effort latency bullet [K-15]–[K-18] and Open list; §5.6 latency item superseded in part, OQ-128, candidate table (`webrtcsink` not packaged [K-06], [K-13]; `rtspclientsink` and `whipclientsink` rows [K-01], [K-05], [K-09]), Open list, and new §5.6.1 (candidate live-publisher diagram, element-latency table [K-07]–[K-10], [K-27]–[K-29], [K-32], [K-37], queues [K-36], B-frames and keyframes [K-04], [K-30], [K-31], audio [K-40], measurement [K-11], [K-12], MediaMTX behaviour [K-01]–[K-03], [K-44], internet viewers [K-41]–[K-43], budget summary [K-45] pointing to PERFORMANCE.md, platform table [K-34], [K-35], [K-39]); §5.9 eleven settings rows added or annotated; §5.10 recorder and live-path metrics [K-12], [K-32], [K-44]; §6 framework row note and two rows (HDD-branch decoupling OQ-117; live-path buffering OQ-126), IPC row note; §7 three rows; §8 REQ-REC-001, REQ-STR-001, REQ-STR-002 rows annotated; Verification status gains topics J and K with CORRECTED, community and reasoning entries. No requirement, ADR, risk, OQ or test status changed; every responsibility is still NOT STARTED. Verifier pass (same date): §5.3 CM4 latency bullet "No 1080p figure exists for BCM2711" → "The register gives no 1080p figure for BCM2711", recording encode "would share" the encoder (OQ-115); §5.3 FFmpeg candidate bullet labelled as reasoning (x264 `zerolatency` [K-27] carried over to `libx264`); §5.4.1 item 3: [J-30] worded as one Seagate BarraCuda 2.5-inch family, an example and not a general HDD figure; §5.6.1 diagram note "this is the GStreamer route" → "the candidate GStreamer route", with the self-built gst-plugins-rs and `webrtcbin` [F-42] alternatives named (ADR-007 OPEN); §5.6.1 item 3: the 60-frame GOP is the CM4 encoder's default [K-30]; budget: the recording encode "would share" the CM4 encoder; three older statements that the recording container is still open (§5.4 HEVC note, §5.8 outputs, §5.8.1 item 5 table) marked superseded in part (MP4, ADR-009; OQ-006 open for the maximum duration). Main session, same date: "self-powered enclosure or hub" stated as the owner decision reworded to the owner's words, "self-powered enclosure"; where ADR-009 decision 3 is quoted, "or hub" is marked as Claude's addition (consistent with the DECISIONS.md correction of 2026-10-09). | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 (latency judged at the 95th percentile; LAN-only WebRTC viewers; recording until stopped or disk full — OQ-006 ANSWERED, OQ-008, OQ-128/RISK-033 not in current scope, new OQ-129): new "Owner decisions of 2026-10-09" summary block after the 2026-10-08 research block; "Owner decisions of 2026-10-08, third" recording bullet (duration still open) marked superseded; §3 diagram note: label "reach OQ-008" marked superseded (diagram text unchanged); §4 WebRTC network row: internet-viewer facts labelled not in current scope; Storage row OQ column: OQ-006 ANSWERED, OQ-129 added; §5.4 required behaviour (duration UNDEFINED), HEVC note and Open list marked superseded / OQ-006 ANSWERED, OQ-129 added; §5.4.1 item 6 (Long recordings) marked superseded in part, configuration table — duration row marked ANSWERED, OQ-118 row cross-references OQ-129, new OQ-129 row — and Open list updated; §5.6 UNDEFINED list (reach, latency) marked superseded in part (LAN only; 95th percentile; browsers, viewers, sample count and run length still OQ-008), NAT/OQ-128 line and Open list labelled not in current scope; §5.6.1 purpose gains the 95th-percentile and LAN-only note, Traces RISK-033 annotated, diagram label "reach: LAN or internet" marked superseded (diagram text unchanged), item 7 (internet viewers) labelled not in current scope (kept as reference), open configuration parameters (reach, ICE servers, advertised addresses) marked superseded in part, Open list annotated; §5.8 outputs (OQ-006 OPEN) marked superseded; §5.9 table — recording-duration row notes OQ-006 ANSWERED, OQ-118 row cross-references OQ-129, new OQ-129 row, WebRTC reach row (LAN only; OQ-128 not in current scope), live-latency row (95th percentile; OQ-008); §8 REQ-REC-001 and REQ-STR-002 rows gain dated notes. No text deleted; no decision, status or ID changed except as recorded in the registers. | Claude (session 2026-10-09) |
