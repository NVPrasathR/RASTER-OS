# PACSCORDER Changelog

| | |
|---|---|
| Document status | Active |
| Last updated | 2026-10-08 |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 4 |

Every meaningful change is recorded here. Versioning is semantic-style (`Unreleased`, `v0.1.0`, `v0.2.0`, `v1.0.0`; see [RELEASE.md](RELEASE.md)). History is never rewritten: a correction is a new entry.

## Unreleased

### Added

**Repository and process**
- Git repository initialised on branch `main` (2026-10-06).
- `.gitignore` for macOS and editor files and for future build outputs.
- `CLAUDE.md`: loads the engineering rules into every Claude Code session.

**Process documents**
- `docs/ENGINEERING_RULES.md`: the owner's engineering and documentation rules, verbatim. The only formatting change is wider outer code fences where code blocks nest.
- `docs/README.md`: documentation index, the Rule 25 question map, documentation conventions and the canonical test IDs.
- `docs/REQUIREMENTS.md`: 17 requirements (11 DRAFT from the rules, 6 PROPOSED from research).
- `docs/DECISIONS.md`: ADR-001 to ADR-008. ADR-001 is ACCEPTED (from the rules); ADR-002, -003, -005, -006 and -008 are PROPOSED; ADR-004 and -007 are OPEN.
- `docs/RISKS.md`: 21 technical risks.
- `docs/OPEN_QUESTIONS.md`: 101 open questions and owner decisions. OQ-001 to OQ-089 were compiled from the research; OQ-090 to OQ-101 were added after the per-document reviews.
- `docs/TRACEABILITY.md`: requirement → design → implementation → test → result.
- `docs/ARCHIVED_APPROACHES.md`: template only; no entries.
- `docs/PROJECT_STATUS.md`, `docs/DEVELOPMENT_LOG.md`, `docs/CHANGELOG.md`.

**Source research**
- `docs/REFERENCES.md`: source register of 377 facts (348 CONFIRMED, 29 CORRECTED) from research topics A–G.
- `docs/research/2026-10-06-source-research.json`: the raw research and verification data behind the register.

**Technical documents**
- Written from the source register; none describes implemented or tested functionality: `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `HARDWARE.md`, `DEVICE_TREE.md`, `TC358743_DRIVER.md`, `V4L2.md`, `CSI_PIPELINE.md`, `DMA.md`, `VIDEO_ENCODER.md`, `PERFORMANCE.md`, `RECORDING.md`, `STREAMING.md`, `ATEM.md`, `BUILD_SYSTEM.md`, `RELEASE.md`, `TESTING.md`, `TROUBLESHOOTING.md`.

### Changed

- 2026-10-08: **Two H.264 encodes.** The owner answered OQ-005 with "Separate record + live": one recording encode, and one live encode shared by RTMP and WebRTC. REQ-ENC-001 now records this together with the WebRTC constraints on the live encode. OQ-115 was added (two concurrent encodes on the CM4 hardware encoder), and RISK-002 and RISK-003 were annotated. Technical documents updated.
- 2026-10-08: Pushed `d2d217e` (H.265 deferral) to `origin/main`.

- 2026-10-08: **H.265 deferred.** The owner answered OQ-103 with "H.264 only for now". REQ-ENC-001 is now H.264 only, and REQ-ENC-002 (H.265, new acceptance value DEFERRED) was added. OQ-104 to OQ-109, RISK-022 and RISK-025 are marked not in current scope. TEST-ENC-001 was retitled "Sustained real-time H.264 encode (H.265 deferred)". The technical documents were updated, with the H.265 evidence kept and labelled deferred.
- 2026-10-08: Pushed `main` to `origin` (github.com/NVPrasathR/RASTER-OS) at the owner's request: `7107a39`, `df3591d`. The local backup branch was not pushed.

- 2026-10-08: **Source register extended** with topic H (H.265/HEVC software encoding and transport, 43 facts) and topic I (HDMI audio path, 47 facts): 90 facts, 86 CONFIRMED and 4 CORRECTED. The register now holds 467 facts; existing entries are unchanged. Raw data: `docs/research/2026-10-08-hevc-audio-research.json`.
- 2026-10-08: Owner decisions of 2026-10-07 (second set) and the topic H/I facts propagated to the registers and technical documents.
  - OQ-104 to OQ-114 added: H.265 throughput, SIMD, RTMP, WebRTC and licensing; audio formats, sample rate, A/V sync, AAC licensing and GPIO allocation.
  - RISK-023 (audio sample-rate mismatch), RISK-024 (A/V sync across clock domains) and RISK-025 (HEVC over RTMP may force a split GStreamer/FFmpeg design) added.
  - RISK-014, RISK-015, RISK-019 and RISK-022 extended.
  - TEST-AUD-001 written out in full; TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (ID unchanged).
- 2026-10-07: Git history: the owner's local commits "update doc" and "update" (18:23, never pushed) were squashed, at the owner's request, into `7107a39` "docs: bootstrap PACSCORDER rules, source register and documentation baseline", to meet Rule 15. File contents are identical (same tree). The old history is kept on local branch `backup/pre-squash-2026-10-07`.

- 2026-10-07: Owner decisions recorded.
  - OQ-001 and OQ-009 are ANSWERED.
  - REQ-CAP-001 and REQ-ATEM-001 are updated: 1080p60 is required on 4-lane; ATEM scope is HDMI capture only.
  - New DRAFT requirements: REQ-CAP-007 (2-lane and 4-lane, all frame rates each link carries), REQ-CAP-008 (ATEM and camera HDMI sources), REQ-BLD-002 (own OS image).
  - Owner input added to ADR-003 and ADR-004; no decision status changed.
  - OQ-102 added (which ATEM models and cameras).
  - The affected technical documents were updated to match.
- 2026-10-07: **ADR-003 ACCEPTED** by the owner. The OS/image basis is decided: Raspberry Pi OS Lite for bring-up, own product image built with `rpi-image-gen`, Buildroot as the documented alternative. OQ-012 is ANSWERED. All documents were updated to the accepted status.
- 2026-10-07: **Correction.** The 2026-10-06 entry under Added says "17 requirements (11 DRAFT from the rules, 6 PROPOSED from research)". The correct split was 12 DRAFT and 5 PROPOSED. The original line is left as written (Rule 21). As of 2026-10-07 there are 20 requirements: 15 DRAFT, 5 PROPOSED.
- 2026-10-07: Second set of owner answers recorded.
  - Bring-up evaluates CM4 and CM5 side by side (ADR-004 input; still OPEN).
  - Audio is required: REQ-CAP-006 moved from PROPOSED to DRAFT; OQ-004 is ANSWERED.
  - Codecs are H.264 and H.265 (REQ-ENC-001). OQ-103 added; RISK-022 added because H.265 is software-only on every candidate.
  - Sources are any HDMI camera with no model list; OQ-102 is ANSWERED.
- 2026-10-07: TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)"; the ID is unchanged.

### Fixed

- Nothing (first entry).

### Known Issues

- Procedure steps whose command syntax is not in the source register are marked NEEDS VERIFICATION (OQ-101).

- No hardware exists. Every hardware test is BLOCKED — HARDWARE REQUIRED.
- No code exists. Every requirement is NOT STARTED.
- No requirement is ACCEPTED (21 requirements: 16 DRAFT, 4 PROPOSED, 1 DEFERRED). Four PROPOSED ADRs (ADR-002, -005, -006, -008) and two OPEN ADRs (ADR-004, -007) await owner decision; ADR-001 and ADR-003 are ACCEPTED.
- H.265 is deferred (REQ-ENC-002). If re-activated, it is software-only on every candidate, and real-time 1080p H.265 is doubtful (RISK-022, OQ-104).
- HDMI audio on CM5 is unconfirmed (RISK-014). Audio sample-rate changes are not tracked by the kernel (RISK-023).
- 1080p60 capture is not possible on a 2-lane link (RISK-001). Since 2026-10-07, 1080p60 is required on 4-lane configurations only, and Pi 4 Model B remains a 2-lane candidate (REQ-CAP-007). 1080p60 encode is unproven on every candidate platform (RISK-002, RISK-003).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created with the Phase 0 bootstrap entry. | Claude (session 2026-10-06) |
| 2026-10-07 | Unreleased/Changed: owner decisions of 2026-10-07. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 acceptance, requirement-count correction, TEST-ATEM-001 retitle; RISK-001 known-issue line updated for REQ-CAP-007. | Claude (session 2026-10-07) |
| 2026-10-07 | Second set of owner answers; Known Issues updated (ADR counts, RISK-022). | Claude (session 2026-10-07) |
| 2026-10-08 | Topics H and I; propagation of the second owner decisions; commit squash recorded; Known Issues extended. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferral and push recorded. | Claude (session 2026-10-08) |
| 2026-10-08 | Two-encode decision; push of `d2d217e`. | Claude (session 2026-10-08) |
