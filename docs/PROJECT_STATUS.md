# PACSCORDER Project Status

| | |
|---|---|
| Document status | Active — update after every major milestone (Rule 18) |
| Last updated | 2026-10-08 |

## Current Phase

PHASE 0 — Bootstrap (documentation baseline complete). PHASE 1 (decisions and hardware) has started: the owner answered 5 open questions on 2026-10-07, accepted ADR-003, and answered OQ-103 on 2026-10-08 (H.264 only for now).

Product work cannot start until the hardware and remaining decisions listed under **Blocked** exist.

**Proposed phase plan.** This is a PROPOSED roadmap by Claude, not accepted by the owner. Each phase ends only when Rule 24 is met: implementation, build, functional test and documentation.

| Phase | Content | Exit evidence | Status |
|---|---|---|---|
| PHASE 0 — Bootstrap | Git repository, rules, documentation set, source research | Documentation check passes ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)) | Documentation baseline complete; owner review of DRAFT/PROPOSED items continues |
| PHASE 1 — Decisions and hardware | Owner answers the owner-decision OQs; ADR-004 decided from bring-up measurements; bring-up hardware obtained and recorded in [HARDWARE.md](HARDWARE.md) | Accepted requirements; HW REV recorded | IN PROGRESS — ADR-003 ACCEPTED; OQ-001, OQ-004, OQ-009, OQ-012, OQ-102, OQ-103 answered; no hardware yet |
| PHASE 2 — Bring-up | Stock Raspberry Pi OS on CM4 and CM5: I2C detection, driver probe, EDID / hot-plug, overlay and media graph, audio card | TEST-HW-001, TEST-DRV-001, TEST-DRV-002, TEST-PLT-001, TEST-CAP-001, TEST-AUD-001 | NOT STARTED |
| PHASE 3 — Capture pipeline | 2-lane and 4-lane configurations, source changes, unsupported modes, DMABUF | TEST-CAP-002, TEST-CAP-003, TEST-CAP-004, TEST-DMA-001 | NOT STARTED |
| PHASE 4 — Encode | Real-time H.264 encode on CM4 and CM5 (H.265 deferred); performance budget; ADR-004 decision | TEST-ENC-001, TEST-PERF-001 | NOT STARTED |
| PHASE 5 — Record and stream | Recording, RTMP, WebRTC, with audio | TEST-REC-001, TEST-STR-001, TEST-STR-002 | NOT STARTED |
| PHASE 6 — ATEM | HDMI capture of ATEM output (OQ-009 scope) | TEST-ATEM-001 | NOT STARTED |
| PHASE 7 — Product image and release | Own OS image built with `rpi-image-gen`, update mechanism, release v0.1.0 | TEST-BLD-001, [RELEASE.md](RELEASE.md) checklist | NOT STARTED |

## Current Objective

1. **OQ-103 decided (owner, 2026-10-08): "H.264 only for now".** All outputs (recording, RTMP, WebRTC) use H.264. H.265 is deferred as REQ-ENC-002 (DEFERRED), and its evidence and risks (RISK-022, RISK-025, OQ-104 to OQ-109) are kept for later.
2. **Other owner decisions** in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md):
   - OQ-005 to OQ-008: bitrate, latency, recording and streaming parameters.
   - OQ-010: sustained-operation envelope.
   - OQ-109 / OQ-113: HEVC and AAC patent licensing.
   - OQ-018 to OQ-021: bridge board(s), wiring of INT/RESET and audio I2S.
3. **Obtain bring-up hardware** (owner decision: evaluate **CM4 and CM5 side by side**):
   - CM4 and CM5 with their IO boards;
   - TC358743 bridge board(s) for both lane configurations, with a 27 MHz reference clock [A-45], [B-10] and the I2S audio pins wired to GPIO 18–20 [I-04];
   - TC358743 I/O voltage matched to the IO board's selected GPIO voltage (1.8 V or 3.3 V) [I-27], [I-29].

## Completed

- [x] Git: `main` is pushed to `origin` (github.com/NVPrasathR/RASTER-OS). Commits:
  - `7107a39` docs: bootstrap PACSCORDER rules, source register and documentation baseline (squash of the owner's two local commits, Rule 15);
  - `df3591d` docs: record owner decisions, accept ADR-003, add H.265 and audio research (pushed 2026-10-08 at the owner's request).
  - The H.265 deferral is uncommitted; see Next Step.
- [x] Owner's engineering rules stored verbatim in [ENGINEERING_RULES.md](ENGINEERING_RULES.md), loaded every session through `CLAUDE.md`.
- [x] Source research: **467 facts in 9 topics (A–I)**, each independently fact-checked: 434 CONFIRMED, 33 CORRECTED, 0 UNVERIFIABLE, 0 REFUTED ([REFERENCES.md](REFERENCES.md)). Topics H (H.265) and I (HDMI audio) were added on 2026-10-08.
- [x] Requirements: 21 (16 DRAFT, 4 PROPOSED, 1 DEFERRED — REQ-ENC-002 H.265) — [REQUIREMENTS.md](REQUIREMENTS.md).
- [x] Decisions: 8 ADRs (ACCEPTED: ADR-001, ADR-003; PROPOSED: ADR-002, ADR-005, ADR-006, ADR-008; OPEN: ADR-004, ADR-007) — [DECISIONS.md](DECISIONS.md).
- [x] Risks: 25, all OPEN — [RISKS.md](RISKS.md).
- [x] Open questions: 114 (108 OPEN; 6 ANSWERED by owner statements) — [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).
- [x] Full Rule 2 documentation set, written from the source register, reviewed and kept consistent; the documentation check passes ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)).

None of the above is product functionality. **Nothing in the product works yet, because nothing has been built or tested.**

## In Progress

- [ ] Owner review of DRAFT/PROPOSED requirements and PROPOSED ADRs (OQ-017).

## Blocked

- [ ] Every hardware test (TEST-HW-001 … TEST-PERF-001): **BLOCKED — HARDWARE REQUIRED**. No hardware exists.
- [ ] Device Tree / overlay for PACSCORDER: blocked on the bridge-board facts (OQ-018 to OQ-020).
- [ ] Product image build (TEST-BLD-001): ADR-003 is ACCEPTED, but the build is NOT STARTED. No `rpi-image-gen` configuration exists, and the image cannot be boot-tested without hardware.

## Known Problems

These are known from sources; none has been observed on PACSCORDER hardware. See [RISKS.md](RISKS.md).

- **RISK-001:** 1080p60 needs a 4-lane CSI-2 link. The 2-lane configuration (REQ-CAP-007) is physically limited to 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48].
- **RISK-002:** 1080p60 hardware H.264 encode on CM4 is unproven; the official specification is 1080p30 [D-10].
- **RISK-003:** CM5 has no hardware video encoder; all encoding is in software [G-22].
- **RISK-022:** not in current scope — H.265 is deferred (REQ-ENC-002). It is kept OPEN because real-time 1080p H.265 is doubtful on CM4 and CM5.
- **RISK-023:** HDMI audio at a sample rate other than the one ALSA opens is not detected by the kernel. The application must read the TC358743 sampling-rate control (reasoning from kernel source) [I-18].
- **RISK-014:** HDMI audio on CM5 is unconfirmed. The overlay labels resolve on CM5, but no source shows it working [I-05], [I-06], [I-07].
- **RISK-010:** HDMI sources see no display until userspace loads an EDID [A-33], [B-21].

## Last Verified

2026-10-08. Only the **documentation consistency check** has been run: 0 problems ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)). No hardware or software verification has ever been performed.

## Hardware

None. Bring-up will evaluate CM4 and CM5 side by side (owner, 2026-10-07); the product platform stays OPEN (ADR-004). The bridge board is not selected (OQ-018). See [HARDWARE.md](HARDWARE.md).

## Kernel

None selected or built.

- **Bring-up kernel (ADR-003, ACCEPTED):** the Raspberry Pi OS 2026-10-06 kernel, 6.18.50 [G-04].
- **Raspberry Pi default branch:** `rpi-6.18.y` [B-01], [E-37].

## Buildroot

Not used. ADR-003 (ACCEPTED 2026-10-07) chooses Raspberry Pi OS with `rpi-image-gen` and keeps Buildroot as the documented alternative. See [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

## Next Step

1. The owner decides the remaining owner-decision OQs listed under Current Objective, starting with OQ-005: bitrate, latency and the number of simultaneous H.264 encodes.
2. The owner obtains the CM4 + CM5 bring-up hardware listed under Current Objective, and records it in [HARDWARE.md](HARDWARE.md) as HW REV A.
3. Commit and push the H.265 deferral once the owner approves. The proposed commit is in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) (2026-10-08, second entry).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created: Phase 0 status after repository bootstrap and source research. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 recorded (OQ-001, OQ-009 answered; REQ-CAP-007, REQ-CAP-008, REQ-BLD-002 added); objective, counts, known problems and next step updated. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED and OQ-012 ANSWERED recorded. Requirement counts corrected to 15 DRAFT / 5 PROPOSED (the earlier "14 DRAFT and 6 PROPOSED" carried forward a miscount from 2026-10-06; see DEVELOPMENT_LOG.md 2026-10-07). | Claude (session 2026-10-07) |
| 2026-10-07 | Second set of owner answers recorded (CM4 + CM5 evaluation, audio required, H.264 + H.265, any HDMI camera); counts updated (16 DRAFT / 4 PROPOSED, 103 OQs, 22 risks); stale ADR-003 lines in Blocked and the phase plan updated. | Claude (session 2026-10-07) |
| 2026-10-08 | Brought up to date after topics H and I: 467 facts, 25 risks, 114 OQs; OQ-103 (H.265 scope) made the most urgent owner decision; known problems extended with RISK-022, RISK-023 and RISK-014; commit `7107a39` recorded; phase plan updated for CM4 + CM5, audio and H.265; Last Verified 2026-10-08. | Claude (session 2026-10-08) |
| 2026-10-08 | OQ-103 answered ("H.264 only for now"): H.265 deferred (REQ-ENC-002); counts 21 requirements / 108 OPEN + 6 ANSWERED OQs; git state (commits `7107a39`, `df3591d` pushed); next step updated. | Claude (session 2026-10-08) |
