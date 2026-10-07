# PACSCORDER Project Status

| | |
|---|---|
| Document status | Active — update after every major milestone (Rule 18) |
| Last updated | 2026-10-07 |

## Current Phase

PHASE 0 — Bootstrap.

The repository, engineering rules, documentation baseline and verified source research are in place. Product work cannot start until the owner decisions and hardware listed under **Blocked** exist.

**Proposed phase plan.** This is a PROPOSED roadmap by Claude, not accepted by the owner. Each phase ends only when Rule 24 is met: implementation, build, functional test and documentation.

| Phase | Content | Exit evidence | Status |
|---|---|---|---|
| PHASE 0 — Bootstrap | Git repository, rules, documentation set, source research | Documentation check passes (see [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)) | IN PROGRESS — documentation baseline complete; owner review pending |
| PHASE 1 — Decisions and hardware | Owner answers OQ-001…OQ-017 (OQ-001, OQ-004, OQ-009, OQ-012 answered 2026-10-07); ADR-003 decided (ACCEPTED 2026-10-07) and ADR-004 decided; bring-up hardware obtained and recorded in [HARDWARE.md](HARDWARE.md) | Accepted requirements; HW REV recorded | NOT STARTED |
| PHASE 2 — Bring-up | Stock Raspberry Pi OS: I2C detection, driver probe, EDID / hot-plug, overlay and media graph | TEST-HW-001, TEST-DRV-001, TEST-DRV-002, TEST-PLT-001, TEST-CAP-001 | NOT STARTED |
| PHASE 3 — Capture pipeline | Target mode capture, source changes, unsupported modes, DMABUF | TEST-CAP-002, TEST-CAP-003, TEST-CAP-004, TEST-DMA-001 | NOT STARTED |
| PHASE 4 — Encode | Real-time encode on the chosen platform; performance budget | TEST-ENC-001, TEST-PERF-001 | NOT STARTED |
| PHASE 5 — Record and stream | Recording, RTMP, WebRTC; audio if required | TEST-REC-001, TEST-STR-001, TEST-STR-002, TEST-AUD-001 | NOT STARTED |
| PHASE 6 — ATEM | Integration as scoped by OQ-009 | TEST-ATEM-001 | NOT STARTED |
| PHASE 7 — Product image and release | Reproducible image, update mechanism, release v0.1.0 | TEST-BLD-001, [RELEASE.md](RELEASE.md) checklist | NOT STARTED |

## Current Objective

1. **Owner decisions recorded on 2026-10-07** (see [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)):
   - OQ-001 is answered. Both 2-lane and 4-lane configurations are required, each with every frame rate its link carries (REQ-CAP-007). 1080p60 is required on 4-lane.
   - OQ-009 is answered. Sources are ATEM HDMI output and cameras directly (REQ-CAP-008). ATEM network integration is out of scope.
   - The product runs its own OS image (REQ-BLD-002).
   - ADR-003 is ACCEPTED (owner: "accept ADR-003"). Bring-up uses stock Raspberry Pi OS Lite 64-bit; the product image is built with `rpi-image-gen`; Buildroot is the documented alternative. OQ-012 is answered.
   - Second set of answers (2026-10-07): bring-up evaluates **CM4 and CM5 side by side** (ADR-004 stays OPEN until measured); **audio is required** (REQ-CAP-006, OQ-004); codecs **H.264 and H.265** (REQ-ENC-001; H.265 is software-only on every candidate, RISK-022); sources are **any HDMI camera** plus ATEM outputs (OQ-102).
2. **Remaining owner decisions** in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). The most decisive:
   - OQ-103: which outputs use H.265, and is software-only H.265 acceptable?
   - OQ-005 to OQ-008: bitrate, latency, recording and streaming parameters.
   - OQ-010: sustained-operation envelope (temperature, soak duration, drop threshold).
   - OQ-018 to OQ-021: which TC358743 bridge board(s), wiring of INT/RESET and audio I2S.
3. **Obtain bring-up hardware**: CM4 and CM5 with IO boards, and TC358743 bridge board(s) for both lane configurations with I2S audio wired (OQ-018 to OQ-021, OQ-025).

## Completed

- [x] Git repository initialised (`main`, no commits yet).
- [x] Owner's engineering rules stored verbatim: [ENGINEERING_RULES.md](ENGINEERING_RULES.md), loaded every session through `CLAUDE.md`.
- [x] Source research: 377 facts from 7 topics, each independently fact-checked (348 CONFIRMED, 29 CORRECTED, 0 UNVERIFIABLE, 0 REFUTED): [REFERENCES.md](REFERENCES.md).
- [x] Requirements baseline: 20 requirements, 16 DRAFT and 4 PROPOSED: [REQUIREMENTS.md](REQUIREMENTS.md). REQ-CAP-007, REQ-CAP-008 and REQ-BLD-002 were added from owner statements on 2026-10-07.
- [x] Decision log: 8 ADRs (2 ACCEPTED, 4 PROPOSED, 2 OPEN): [DECISIONS.md](DECISIONS.md). ADR-003 (OS and image basis) was accepted by the owner on 2026-10-07.
- [x] Risk register (22 risks; RISK-022 added 2026-10-07): [RISKS.md](RISKS.md). Open-question register (103 questions; OQ-001, OQ-004, OQ-009, OQ-012 and OQ-102 ANSWERED on 2026-10-07): [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).
- [x] Full Rule 2 documentation set written from the source register, reviewed per document cluster and checked across documents. The documentation check passes; see [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md).

None of the above is product functionality. **Nothing in the product works yet, because nothing has been built or tested.**

## In Progress

- [ ] Owner review of DRAFT/PROPOSED requirements and PROPOSED ADRs (OQ-017).

## Blocked

- [ ] Every hardware test (TEST-HW-001 … TEST-PERF-001): **BLOCKED — HARDWARE REQUIRED**. No hardware exists (owner, 2026-10-06).
- [ ] Device Tree / overlay for PACSCORDER: blocked on the bridge-board facts (OQ-018 to OQ-020) and on ADR-004.
- [ ] Product image build (TEST-BLD-001): ADR-003 is ACCEPTED; NOT STARTED — no `rpi-image-gen` configuration exists yet, and the image cannot be boot-tested without hardware.

## Known Problems

These are known from sources; none has been observed on PACSCORDER hardware. See [RISKS.md](RISKS.md).

- **RISK-001:** 1080p60 needs a 4-lane CSI-2 link. The 2-lane configuration required by REQ-CAP-007 is physically limited to 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48].
- **RISK-002:** 1080p60 hardware H.264 encode on Pi 4/CM4 is unproven; the official specification is 1080p30.
- **RISK-003:** Pi 5/CM5 have no hardware video encoder; all encoding is in software.
- **RISK-004:** TC358743 lifecycle is uncertain.
- **RISK-010:** HDMI sources see no display until userspace loads an EDID.

## Last Verified

2026-10-06. Only a **documentation consistency check** has been run (see [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)). No hardware or software verification has ever been performed.

## Hardware

None. Target platform OPEN (ADR-004). Bridge board not selected (OQ-018). See [HARDWARE.md](HARDWARE.md).

## Kernel

None selected or built.

- **Bring-up kernel (ADR-003, ACCEPTED):** the Raspberry Pi OS 2026-10-06 kernel 6.18.50 [G-04].
- **Raspberry Pi default branch:** `rpi-6.18.y` [B-01], [E-37].

## Buildroot

Not used. ADR-003 (ACCEPTED 2026-10-07) chooses Raspberry Pi OS with `rpi-image-gen` and keeps Buildroot as the documented alternative. If Buildroot is ever re-evaluated, the reference versions are Buildroot 2026.08 or LTS 2025.02.18 [E-01], [E-02]. See [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

## Next Step

1. The owner answers the remaining owner-decision questions, listed under Current Objective.
2. The owner chooses bring-up hardware for **both** lane configurations. Criteria from sources:
   - 2-lane: Pi 4 Model B or CM4 CAM0 [C-01], [C-02].
   - 4-lane: CM4 CAM1, Pi 5 or CM5 [C-02], [C-04], [C-05].
   - A TC358743 bridge board with a 27 MHz reference clock, which the stock overlay assumes [A-45], [B-10].
   - Connector and cable orientation matched to the Pi [C-45].
3. Commit the baseline when the owner approves; the owner said "not yet" on 2026-10-07. The proposed commit is in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created: Phase 0 status after repository bootstrap and source research. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 recorded (OQ-001, OQ-009 answered; REQ-CAP-007, REQ-CAP-008, REQ-BLD-002 added); objective, counts, known problems and next step updated. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED and OQ-012 ANSWERED recorded. Requirement counts corrected to 15 DRAFT / 5 PROPOSED (the earlier "14 DRAFT and 6 PROPOSED" carried forward a miscount from 2026-10-06; see DEVELOPMENT_LOG.md 2026-10-07). | Claude (session 2026-10-07) |
| 2026-10-07 | Second set of owner answers recorded (CM4 + CM5 evaluation, audio required, H.264 + H.265, any HDMI camera); counts updated (16 DRAFT / 4 PROPOSED, 103 OQs, 22 risks); stale ADR-003 lines in Blocked and the phase plan updated. | Claude (session 2026-10-07) |
