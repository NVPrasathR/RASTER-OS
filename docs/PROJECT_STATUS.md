# PACSCORDER Project Status

| | |
|---|---|
| Document status | Active — update after every major milestone (Rule 18) |
| Last updated | 2026-10-09 |

## Current Phase

PHASE 1 — Decisions and hardware (IN PROGRESS). PHASE 0, the documentation baseline, is complete. Owner decisions so far: 5 open questions answered and ADR-003 accepted on 2026-10-07; on 2026-10-08, OQ-103 (H.264 only for now), the number of encodes (OQ-005), the live-latency target (OQ-116) and the recording design (ADR-009) were decided; on 2026-10-09, how the latency target is judged and the viewer reach (OQ-008) and the recording duration (OQ-006) were decided; later on 2026-10-09 the recording encode (H.264 High profile, Level 4.2, no B-frames), the recording capture-to-file latency (under 1 s glass-to-disk) and the WebRTC measurement run (a 30-minute run at 1 sample/second, about 1800 samples) were decided, which fully answered OQ-005 and OQ-008 and raised OQ-130 (against which drive and statistic the recording capture-to-file < 1 s target is judged), which the owner then answered the same day: the primary recording target (the NVMe SSD) at the 95th percentile, with the HDD a lagging mirror.

Product work cannot start until the hardware and remaining decisions listed under **Blocked** exist.

**Proposed phase plan.** This is a PROPOSED roadmap by Claude, not accepted by the owner. Each phase ends only when Rule 24 is met: implementation, build, functional test and documentation.

| Phase | Content | Exit evidence | Status |
|---|---|---|---|
| PHASE 0 — Bootstrap | Git repository, rules, documentation set, source research | Documentation check passes ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)) | Documentation baseline complete; owner review of DRAFT/PROPOSED items continues |
| PHASE 1 — Decisions and hardware | Owner answers the owner-decision OQs; ADR-004 decided from bring-up measurements; bring-up hardware obtained and recorded in [HARDWARE.md](HARDWARE.md) | Accepted requirements; HW REV recorded | IN PROGRESS — ADR-003 and ADR-009 ACCEPTED; OQ-001, OQ-004, OQ-005, OQ-006, OQ-008, OQ-009, OQ-012, OQ-102, OQ-103, OQ-116, OQ-129 answered; no hardware yet |
| PHASE 2 — Bring-up | Stock Raspberry Pi OS on CM4 and CM5: I2C detection, driver probe, EDID / hot-plug, overlay and media graph, audio card | TEST-HW-001, TEST-DRV-001, TEST-DRV-002, TEST-PLT-001, TEST-CAP-001, TEST-AUD-001 | NOT STARTED |
| PHASE 3 — Capture pipeline | 2-lane and 4-lane configurations, source changes, unsupported modes, DMABUF | TEST-CAP-002, TEST-CAP-003, TEST-CAP-004, TEST-DMA-001 | NOT STARTED |
| PHASE 4 — Encode | Two real-time H.264 encodes on CM4 and CM5 (H.265 deferred); performance budget; ADR-004 decision | TEST-ENC-001, TEST-PERF-001 | NOT STARTED |
| PHASE 5 — Record and stream | Fragmented-MP4 recording mirrored to NVMe SSD and HDD (ADR-009); RTMP (best-effort latency); WebRTC under 1 s camera-to-viewer; with audio | TEST-REC-001, TEST-STR-001, TEST-STR-002 | NOT STARTED |
| PHASE 6 — ATEM | HDMI capture of ATEM output (OQ-009 scope) | TEST-ATEM-001 | NOT STARTED |
| PHASE 7 — Product image and release | Own OS image built with `rpi-image-gen`, update mechanism, release v0.1.0 | TEST-BLD-001, [RELEASE.md](RELEASE.md) checklist | NOT STARTED |

## Current Objective

1. **Decided for the current scope** (owner decisions of 2026-10-08, plus 2026-10-09 where marked):
   - **Encoding:** two simultaneous H.264 encodes, one for recording and one live encode shared by RTMP and WebRTC ("Separate record + live", OQ-005). Whether CM4's hardware encoder can run both is open (OQ-115); on CM5 both run in software (OQ-059).
   - **Codec:** "H.264 only for now" (OQ-103). H.265 is deferred as REQ-ENC-002 (DEFERRED); its evidence and risks (RISK-022, RISK-025, OQ-104 to OQ-109) are kept for later.
   - **Live latency:** under 1 s camera-to-viewer for **WebRTC viewers only**; RTMP outputs are best-effort (OQ-116 ANSWERED; REQ-STR-002). Nothing shows that either module meets it (RISK-031).
   - **Latency criterion (2026-10-09):** judged at the **95th percentile** — 95 % of camera-to-viewer samples under 1 s, over a sustained run with the recording running (OQ-008). Sample count and run length are still open.
   - **Viewer reach (2026-10-09):** WebRTC viewers on the **LAN only**; internet viewers are not in current scope (OQ-128, RISK-033 kept as reference).
   - **Recording (ADR-009, ACCEPTED):** MP4 written **fragmented**, every recording **mirrored** to a PCIe NVMe SSD and a USB-to-SATA HDD, the HDD in a **self-powered** enclosure. ext4 on the recording volumes is Claude's proposal inside ADR-009, not an owner decision (OQ-120).
   - **Recording duration (2026-10-09):** no fixed limit — until stopped or the disk is full (OQ-006 ANSWERED).
   - **Bitrate and rate control (2026-10-09, later):** live encode (RTMP and WebRTC) CBR 17 Mbit/s; recording encode 25 Mbit/s VBR (OQ-005). Whether these fit the signalled H.264 profile and level is not in the register (OQ-073).
   - **Drive failure (2026-10-09, later):** if one mirrored drive fills, is absent or fails, recording continues on the other drive and the operator is alerted (OQ-129). A recording started with one drive missing runs on the available drive with an alert. A drive that returns is used again from the next 30-minute file (OQ-129 ANSWERED).
   - **Viewers and browsers (2026-10-09, later):** up to 5 simultaneous WebRTC viewers on the LAN; Chrome (and Chromium-based browsers), Safari (macOS and iOS) and Firefox (OQ-008).
   - **File splitting (2026-10-09, later):** recordings are split into a new file every 30 minutes, about 5.67 GB per file per drive at 25 Mbit/s (reasoning, [J-36]). The mechanism depends on ADR-007; fragmented-mode compatibility is OQ-118.
   - **Recording encode (2026-10-09, latest):** H.264 **High profile, Level 4.2, no B-frames**, the same on CM4 and CM5 (OQ-005). Whether 25 Mbit/s fits Level 4.2's maximum bitrate is OQ-073.
   - **Recording latency (2026-10-09, latest):** a **capture-to-file latency target of under 1 s glass-to-disk** (OQ-005). Judged against the **primary recording target (the NVMe SSD) at the 95th percentile**, with the HDD a **lagging mirror** not bound by the target (OQ-130 ANSWERED, 2026-10-09; it refines ADR-009's mirror). With these, **OQ-005 and OQ-008 are fully ANSWERED.**
2. **Owner decisions still open** ([OPEN_QUESTIONS.md](OPEN_QUESTIONS.md)):
   - OQ-091: the operator interface, including how drive alerts are shown.
   - OQ-118 to OQ-121 (in part): fragment duration and the acceptable loss on a power cut, recording filesystem, NVMe SSD and adapter choice.
   - OQ-007 (RTMP destinations), OQ-010 (sustained-operation envelope), OQ-113 (AAC licensing).
   - OQ-018 to OQ-021: bridge board(s), wiring of INT/RESET and audio I2S.
3. **Obtain bring-up hardware** (owner decision: evaluate **CM4 and CM5 side by side**):
   - CM4 and CM5 with their IO boards. The CM4 IO Board's PCIe slot is powered only from its 12 V barrel input [J-05] (RISK-026).
   - TC358743 bridge board(s) for both lane configurations, with a 27 MHz reference clock [A-45], [B-10] and the I2S audio pins wired to GPIO 18–20 [I-04].
   - TC358743 I/O voltage matched to the IO board's selected GPIO voltage (1.8 V or 3.3 V) [I-27], [I-29].
   - Recording storage per ADR-009: an NVMe SSD for each board (with a PCIe adaptor for the CM4 IO Board socket [J-03], [J-11]; M.2 on the CM5 IO Board [J-13], [J-14]) and a USB-to-SATA HDD in a self-powered enclosure. No part is chosen; the bridge chipset has to be qualified (OQ-121, OQ-122, RISK-027).

## Completed

- [x] Git: `main` is on `origin` (github.com/NVPrasathR/RASTER-OS). Commits:
  - `7107a39` docs: bootstrap PACSCORDER rules, source register and documentation baseline (squash of the owner's two local commits, Rule 15);
  - `df3591d` docs: record owner decisions, accept ADR-003, add H.265 and audio research;
  - `d2d217e` docs: defer H.265 encoding, H.264 only for now (OQ-103);
  - `6efadce` docs: two H.264 encodes - recording and shared live (OQ-005);
  - `54269bf` "update 9oct" (2026-10-09, committed under the owner's git identity without a Rule 15 proposal): research topics J and K, ADR-009, OQ-116 and the new OQs and risks, registers only. Its message does not follow Rule 15; it was left unchanged because it is already on `origin` (see [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md), 2026-10-08 fourth entry).
  - "docs: propagate ADR-009 and live-latency decision, verify topics J and K" (2026-10-09, approved by the owner): the 2026-10-09 documentation catch-up. *(Pushed 2026-10-09 at the owner's request: `54269bf..f39e661`.)*
  - `6ad83d4` docs: record latency criterion, LAN-only viewers and recording duration (2026-10-09; owner: "Commit and push"; pushed `f39e661..6ad83d4`).
  - `b142871` docs: record live and recording bitrates and drive-failure policy (2026-10-09; owner: "Commit and push"; pushed `6ad83d4..b142871`).
  - The OQ-129 decisions (start, file splitting, drive return) and the viewer and browser decisions (OQ-008) are uncommitted; the owner said "Not yet" on 2026-10-09. See Next Step.
- [x] Owner's engineering rules stored verbatim in [ENGINEERING_RULES.md](ENGINEERING_RULES.md), loaded every session through `CLAUDE.md`.
- [x] Source research: **557 facts in 11 topics (A–K)**, each independently fact-checked: 512 CONFIRMED, 45 CORRECTED, 0 UNVERIFIABLE, 0 REFUTED ([REFERENCES.md](REFERENCES.md)). Topics J (recording storage and power loss) and K (live latency) were added on 2026-10-08.
- [x] Requirements: 21 (16 DRAFT, 4 PROPOSED, 1 DEFERRED — REQ-ENC-002 H.265) — [REQUIREMENTS.md](REQUIREMENTS.md).
- [x] Decisions: 9 ADRs (ACCEPTED: ADR-001, ADR-003, ADR-009; PROPOSED: ADR-002, ADR-005, ADR-006, ADR-008; OPEN: ADR-004, ADR-007) — [DECISIONS.md](DECISIONS.md).
- [x] Risks: 34, all OPEN — [RISKS.md](RISKS.md).
- [x] Open questions: 130 (118 OPEN; 12 ANSWERED by owner statements; OQ-129 added and answered 2026-10-09; OQ-130 added and answered 2026-10-09) — [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).
- [x] Full Rule 2 documentation set, written from the source register and kept consistent with every owner decision up to 2026-10-08 (catch-up of 2026-10-09); the documentation check passes ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)).

None of the above is product functionality. **Nothing in the product works yet, because nothing has been built or tested.**

## In Progress

- [ ] Owner review of DRAFT/PROPOSED requirements and PROPOSED ADRs (OQ-017).

## Blocked

- [ ] Every hardware test (TEST-HW-001 … TEST-PERF-001): **BLOCKED — HARDWARE REQUIRED**. No hardware exists.
- [ ] Device Tree / overlay for PACSCORDER: blocked on the bridge-board facts (OQ-018 to OQ-020).
- [ ] Product image build (TEST-BLD-001): ADR-003 is ACCEPTED, but the build is NOT STARTED. No `rpi-image-gen` configuration exists, and the image cannot be boot-tested without hardware.
- [ ] Recording design (ADR-009) beyond the documented decisions: HDD-branch buffering (OQ-117), fragment settings (OQ-118) and storage qualification (OQ-121, OQ-122) need hardware or a build.

## Known Problems

These are known from sources; none has been observed on PACSCORDER hardware. See [RISKS.md](RISKS.md).

- **RISK-001:** 1080p60 needs a 4-lane CSI-2 link. The 2-lane configuration (REQ-CAP-007) is physically limited to 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48].
- **RISK-002:** 1080p60 hardware H.264 encode on CM4 is unproven; the official specification is 1080p30 [D-10]. Two concurrent encodes on it are unknown (OQ-115).
- **RISK-003:** CM5 has no hardware video encoder; all encoding is in software [G-22].
- **RISK-031:** the < 1 s WebRTC target is unproven. A labelled budget (reasoning, not a measurement) accounts for about 56–75 ms of documented terms on CM4; the rest of the path is undocumented [K-45] (OQ-125).
- **RISK-026:** on CM4 the NVMe SSD takes the only PCIe lane, the HDD shares the IO Board's USB 2.0 hub, and the PCIe slot needs the 12 V input [J-01], [J-05], [J-06].
- **RISK-028:** an HDD stall (one example drive needs up to 3.0 s from standby to ready [J-30]) could back-pressure the mirrored recording into the shared encoder and the live path unless the HDD writer is decoupled (OQ-117).
- **Network egress (reasoning, OQ-098):** 5 WebRTC viewers at about 17 Mbit/s each plus one RTMP destination is about 102 Mbit/s of video leaving the board, above a 100 Mbit/s Fast Ethernet link. The Ethernet link speed of each board has not been researched.
- **RISK-034:** the GStreamer WebRTC publishing elements (gst-plugins-rs) are not packaged for Raspberry Pi OS trixie [K-06], and WHEP is still an Internet-Draft [K-02].
- **RISK-023:** HDMI audio at a sample rate other than the one ALSA opens is not detected by the kernel. The application must read the TC358743 sampling-rate control (reasoning from kernel source) [I-18].
- **RISK-014:** HDMI audio on CM5 is unconfirmed. The overlay labels resolve on CM5, but no source shows it working [I-05], [I-06], [I-07].
- **RISK-010:** HDMI sources see no display until userspace loads an EDID [A-33], [B-21].
- **RISK-022:** not in current scope — H.265 is deferred (REQ-ENC-002). It is kept OPEN because real-time 1080p H.265 is doubtful on CM4 and CM5.

## Last Verified

2026-10-09. Only the **documentation consistency check** has been run: 0 problems, 557 of 557 facts cited ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md), 2026-10-09). No hardware or software verification has ever been performed.

## Hardware

None. Bring-up will evaluate CM4 and CM5 side by side (owner, 2026-10-07); the product platform stays OPEN (ADR-004). The bridge board is not selected (OQ-018). Recording storage is decided by type (NVMe SSD and USB-to-SATA HDD, ADR-009) but no part is chosen (OQ-121, OQ-122). See [HARDWARE.md](HARDWARE.md).

## Kernel

None selected or built.

- **Bring-up kernel (ADR-003, ACCEPTED):** the Raspberry Pi OS 2026-10-06 kernel, 6.18.50 [G-04].
- **Raspberry Pi default branch:** `rpi-6.18.y` [B-01], [E-37].

## Buildroot

Not used. ADR-003 (ACCEPTED 2026-10-07) chooses Raspberry Pi OS with `rpi-image-gen` and keeps Buildroot as the documented alternative. See [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

## Next Step

1. *(Done: the catch-up commit `f39e661` was pushed on 2026-10-09, and the owner-decision commit was committed and pushed on 2026-10-09.)*
2. *(Done: the bitrate and drive-failure decisions were committed and pushed on 2026-10-09.)*
3. Commit the uncommitted 2026-10-09 decisions once the owner approves. This now includes the fourth-entry decisions (OQ-129; viewers and browsers) and the fifth-entry decisions (OQ-005 recording encode and capture-to-file latency; OQ-008 measurement run; new OQ-130); the proposed commits are in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) (2026-10-09, fourth and fifth entries).
4. The owner decides the remaining recording-design items (OQ-118 to OQ-121) and the operator interface (OQ-091).
5. Proposed (Claude): research the Ethernet facts of CM4, CM5 and their IO Boards (OQ-098), because the decided viewer count and bitrate put video egress near 100 Mbit/s.
6. The owner obtains the CM4 + CM5 bring-up hardware, including the recording storage, and records it in [HARDWARE.md](HARDWARE.md) as HW REV A.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created: Phase 0 status after repository bootstrap and source research. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 recorded (OQ-001, OQ-009 answered; REQ-CAP-007, REQ-CAP-008, REQ-BLD-002 added); objective, counts, known problems and next step updated. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED and OQ-012 ANSWERED recorded. Requirement counts corrected to 15 DRAFT / 5 PROPOSED (the earlier "14 DRAFT and 6 PROPOSED" carried forward a miscount from 2026-10-06; see DEVELOPMENT_LOG.md 2026-10-07). | Claude (session 2026-10-07) |
| 2026-10-07 | Second set of owner answers recorded (CM4 + CM5 evaluation, audio required, H.264 + H.265, any HDMI camera); counts updated (16 DRAFT / 4 PROPOSED, 103 OQs, 22 risks); stale ADR-003 lines in Blocked and the phase plan updated. | Claude (session 2026-10-07) |
| 2026-10-08 | Brought up to date after topics H and I: 467 facts, 25 risks, 114 OQs; OQ-103 (H.265 scope) made the most urgent owner decision; known problems extended with RISK-022, RISK-023 and RISK-014; commit `7107a39` recorded; phase plan updated for CM4 + CM5, audio and H.265; Last Verified 2026-10-08. | Claude (session 2026-10-08) |
| 2026-10-08 | OQ-103 answered ("H.264 only for now"): H.265 deferred (REQ-ENC-002); counts 21 requirements / 108 OPEN + 6 ANSWERED OQs; git state (commits `7107a39`, `df3591d` pushed); next step updated. | Claude (session 2026-10-08) |
| 2026-10-08 | Two-encode decision recorded (OQ-005: recording + shared live; OQ-115 added); commit `d2d217e` pushed; next step updated. | Claude (session 2026-10-08) |
| 2026-10-09 | Brought up to date with commit `54269bf` (research topics J and K, ADR-009 ACCEPTED, OQ-116 ANSWERED) and the 2026-10-09 documentation catch-up: Current Phase reworded (PHASE 1 in progress); phase plan rows for PHASE 1, 4 and 5; Current Objective restructured into decided items, open owner decisions (OQ-008 now includes how the < 1 s target is judged) and hardware including recording storage; Completed (commits `6efadce` and `54269bf`, 557 facts, 9 ADRs, 34 risks, 128 OQs); Blocked (recording design items); Known Problems (RISK-026, RISK-028, RISK-031, RISK-034 added); Last Verified, Hardware and Next Step. The 2026-10-08 statement that the two-encode decision was uncommitted was stale: it was committed as `6efadce`. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 recorded: latency judged at the 95th percentile and LAN-only WebRTC viewers (OQ-008), recording until stopped or disk full (OQ-006 ANSWERED), new OQ-129; Current Phase, phase plan, Current Objective, Completed (push of `f39e661`; 129 OQs) and Next Step updated. | Claude (session 2026-10-09) |
| 2026-10-09 | Later owner decisions of 2026-10-09 recorded: live encode CBR 17 Mbit/s and recording 25 Mbit/s VBR (OQ-005), drive-failure policy (OQ-129); commit `6ad83d4` and its push recorded; Current Objective and Next Step updated. | Claude (session 2026-10-09) |
| 2026-10-09 | Commit `b142871` and its push recorded; OQ-129 decisions (start on the available drive; split every 30 minutes) added to Current Objective; Next Step renumbered. | Claude (session 2026-10-09) |
| 2026-10-09 | Third set of 2026-10-09 decisions recorded (OQ-129 ANSWERED: drive return; OQ-008: up to 5 viewers, Chrome, Safari, Firefox); network-egress known problem (reasoning, OQ-098); counts 120 OPEN / 9 ANSWERED; owner's "Not yet" on the commit; Next Step renumbered with the proposed Ethernet research. | Claude (session 2026-10-09) |
| 2026-10-09 | Fifth set of 2026-10-09 decisions recorded (OQ-005 ANSWERED: recording encode H.264 High profile, Level 4.2, no B-frames, and a capture-to-file latency target under 1 s glass-to-disk; OQ-008 ANSWERED: 95th-percentile measured over a 30-minute run at 1 sample/second, ~1800 samples); new OQ-130 (which drive and statistic the recording latency is judged against); counts 130 OQs (119 OPEN / 11 ANSWERED); Current Phase, phase plan, Current Objective and Next Step updated. Changes uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-09 | OQ-130 ANSWERED: the recording capture-to-file < 1 s target is judged against the primary recording target (the NVMe SSD) at the 95th percentile, the HDD a lagging mirror (refines ADR-009's mirror). Counts 130 OQs (118 OPEN / 12 ANSWERED); Current Objective, counts and Next Step updated; ADR-009 Consequences refined. Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
