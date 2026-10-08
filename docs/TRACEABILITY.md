# PACSCORDER Traceability Matrix

| | |
|---|---|
| Document status | Active — every requirement traced to design documents and tests (REQ-ENC-002 is DEFERRED and has no planned test); **nothing implemented, nothing tested** |
| Last updated | 2026-10-08 |
| Applies to | All 21 requirements in [REQUIREMENTS.md](REQUIREMENTS.md) (REQ-ENC-002 added 2026-10-08, DEFERRED); all 17 canonical tests in [README.md](README.md) and [TESTING.md](TESTING.md); all candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5). *(Added 2026-10-08.)* Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07; ADR-004 OPEN). |
| Verification | Built from the requirement, decision, risk and open-question registers of 2026-10-06 and the source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)); updated for the owner decisions of 2026-10-07 (REQ-CAP-007, REQ-CAP-008, REQ-BLD-002; OQ-001 and OQ-009 answered; OQ-102 added; ADR-003 ACCEPTED, OQ-012 answered). Updated on 2026-10-08 for the second set of owner decisions of 2026-10-07 (audio required, OQ-004 ANSWERED; H.264 + H.265, OQ-103 added; any HDMI camera plus ATEM, OQ-102 ANSWERED; CM4 + CM5 bring-up, ADR-004 OPEN) and for the source research of topics H and I (2026-10-08), with the register entries OQ-104 to OQ-114 and RISK-022 to RISK-025. Updated again on 2026-10-08 for the owner's answer to OQ-103 ("H.264 only for now"): REQ-ENC-001 is H.264 only; H.265 is REQ-ENC-002 (DEFERRED); OQ-104 to OQ-109, RISK-022 and RISK-025 stay OPEN but are not in the current scope. Updated again on 2026-10-08 for the owner's answer to OQ-005 ("Separate record + live"): two simultaneous H.264 encodes, a recording encode and one live encode shared by RTMP and WebRTC; OQ-115 added; RISK-002 and RISK-003 annotated. Nothing has been verified on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 12 (traceability), Rule 10 (status words), Rule 11 (requirements), Rule 24 (feature complete = implementation + build + test + documentation) |

Rule 12 requires every requirement to be traceable:

```text
Requirement → Design → Implementation → Test → Result
```

This document is that chain for PACSCORDER. The requirements come from [REQUIREMENTS.md](REQUIREMENTS.md), decisions from [DECISIONS.md](DECISIONS.md), risks from [RISKS.md](RISKS.md), open questions from [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md), and tests from the canonical table in [README.md](README.md), with procedures in [TESTING.md](TESTING.md).

> **State on 2026-10-06:**
> - Design exists only as documentation and ADRs. Most ADRs are PROPOSED or OPEN; only ADR-001 is ACCEPTED.
> - Implementation is `NOT STARTED` for every requirement.
> - No test has been run.
>
> No row of this matrix may be read as "done". Under Rule 24, a feature is complete only when implementation, build, functional test and documentation are all complete.
>
> **Since 2026-10-07:** ADR-003 is also ACCEPTED (owner: "accept ADR-003"), so two ADRs are ACCEPTED (ADR-001, ADR-003), four are PROPOSED and two are OPEN. Implementation and test status are unchanged.
>
> **Since 2026-10-08:** the owner's second set of decisions of 2026-10-07 is traced below:
> - HDMI audio is required (REQ-CAP-006 PROPOSED → DRAFT; OQ-004 ANSWERED);
> - codecs are H.264 and H.265 (REQ-ENC-001; which outputs use H.265 is OQ-103, RISK-022) *(superseded later on 2026-10-08: see the next note)*;
> - sources are any HDMI camera, with no model list, plus ATEM outputs (OQ-102 ANSWERED);
> - bring-up evaluates CM4 and CM5 side by side (ADR-004 stays OPEN until measured).
>
> Source research topics H (H.265) and I (HDMI audio) of 2026-10-08 added OQ-104 to OQ-114 and RISK-023 to RISK-025. No ADR status changed. Implementation and test status are unchanged.
>
> **Later on 2026-10-08:** the owner answered OQ-103: "H.264 only for now". REQ-ENC-001 is H.264 only, for recording, RTMP and WebRTC. H.265 is recorded as REQ-ENC-002 (acceptance DEFERRED; no tests planned). OQ-104 to OQ-109, RISK-022 and RISK-025 stay OPEN but are not in the current scope. ADR-007 carries a scope note; no ADR status changed. Implementation and test status are unchanged.
>
> **Also on 2026-10-08:** the owner answered OQ-005 with "Separate record + live". REQ-ENC-001 now requires two simultaneous H.264 encodes:
> - a recording encode;
> - one live encode shared by RTMP and WebRTC, which must therefore be receivable over WebRTC (constraints recorded in REQ-ENC-001).
>
> Bitrate, rate control and latency are still open, so OQ-005 stays OPEN. Whether the CM4 hardware encoder runs both encodes is the new OQ-115 (RISK-002). On CM5 both run in software (OQ-059, RISK-003). Audio stays two encodes, AAC and Opus (reasoning recorded in OQ-063). No requirement acceptance, ADR status, risk or OQ status, test ID or test mapping changed. Implementation and test status are unchanged.

## How to read the matrix

| Column | Content |
|---|---|
| Requirement | ID and short title from [REQUIREMENTS.md](REQUIREMENTS.md). |
| Acceptance | `DRAFT` (from the owner's rules or a dated owner statement; wording or criteria unconfirmed), `PROPOSED` (derived by Claude from research, not agreed), `ACCEPTED`, or, since 2026-10-08, `DEFERRED` (the owner wants it later, but it is not in the current scope; it has no planned tests). No requirement is `ACCEPTED` yet (OQ-017). The open questions that block acceptance criteria are given in brackets. |
| Design | The documents that describe the design, and the ADRs that bear on it, with each ADR's status. Only an ACCEPTED ADR records a decision; a PROPOSED ADR only proposes, and an OPEN ADR is undecided. |
| Implementation | Code, configuration or hardware that implements the requirement. Rule 10 vocabulary. |
| Test | Canonical TEST IDs. Procedures are in [TESTING.md](TESTING.md). |
| Result | The latest recorded result in the [TESTING.md](TESTING.md) result log, in Rule 10 vocabulary. `none recorded` means no entry exists; the current test status is in the reverse table below. |

ADR status abbreviations used below: **A** = ACCEPTED, **P** = PROPOSED, **O** = OPEN.

## Forward matrix: requirement → design → implementation → test → result

| Requirement | Acceptance | Design (docs + ADR IDs) | Implementation | Test | Result |
|---|---|---|---|---|---|
| REQ-ARCH-001 — Video pipeline architecture | DRAFT (OQ-017) | [ARCHITECTURE.md](ARCHITECTURE.md), [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md), [CSI_PIPELINE.md](CSI_PIPELINE.md), [DMA.md](DMA.md); ADR-001 (A), ADR-006 (P), ADR-007 (O). *Added 2026-10-08:* the encoder stage is two H.264 encodes fed from one capture, recording and live (owner, OQ-005) | none — NOT STARTED | TEST-PLT-001, TEST-DMA-001 | none recorded |
| REQ-PLT-001 — Raspberry Pi platform coverage | DRAFT (OQ-011, OQ-017). Owner 2026-10-07: bring-up evaluates CM4 and CM5 side by side; ADR-004 stays OPEN until measured | [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [CSI_PIPELINE.md](CSI_PIPELINE.md); ADR-004 (O; since 2026-10-08 its analysis also weighs H.265 on CM4 versus CM5 — deferred, REQ-ENC-002; not in current scope — and treats CM5 audio as a bring-up gate), ADR-006 (P) | none — NOT STARTED | TEST-PLT-001 | none recorded |
| REQ-DRV-001 — TC358743 controlled by a Linux kernel driver | DRAFT (OQ-013, OQ-017) | [TC358743_DRIVER.md](TC358743_DRIVER.md), [DEVICE_TREE.md](DEVICE_TREE.md), [HARDWARE.md](HARDWARE.md); ADR-001 (A), ADR-002 (P) | none — NOT STARTED | TEST-HW-001, TEST-DRV-001 | none recorded |
| REQ-CAP-001 — 1920x1080@60 HDMI capture | DRAFT (OQ-002, OQ-003, OQ-004, OQ-017). OQ-001 answered 2026-10-07: 1080p60 is required on the 4-lane configuration (REQ-CAP-007). OQ-004 also answered 2026-10-07: audio is required (REQ-CAP-006) | [CSI_PIPELINE.md](CSI_PIPELINE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md), [DEVICE_TREE.md](DEVICE_TREE.md), [V4L2.md](V4L2.md); ADR-002 (P), ADR-004 (O, now a choice per lane configuration), ADR-005 (P), ADR-006 (P), ADR-008 (P) | none — NOT STARTED | TEST-CAP-002 | none recorded |
| REQ-CAP-002 — Capture through V4L2 and Media Controller | DRAFT (OQ-014, OQ-017) | [V4L2.md](V4L2.md), [CSI_PIPELINE.md](CSI_PIPELINE.md); ADR-001 (A), ADR-006 (P) | none — NOT STARTED | TEST-CAP-001 | none recorded |
| REQ-CAP-003 — EDID provisioning at every start | PROPOSED (OQ-002, OQ-017) | [TC358743_DRIVER.md](TC358743_DRIVER.md), [V4L2.md](V4L2.md); ADR-006 (P) | none — NOT STARTED | TEST-DRV-002 | none recorded |
| REQ-CAP-004 — Source change handling | PROPOSED (OQ-017, OQ-020) | [V4L2.md](V4L2.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); ADR-006 (P) | none — NOT STARTED | TEST-CAP-003 | none recorded |
| REQ-CAP-005 — Unsupported input mode handling | PROPOSED (OQ-002, OQ-017) | [CSI_PIPELINE.md](CSI_PIPELINE.md), [V4L2.md](V4L2.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); ADR-005 (P) | none — NOT STARTED | TEST-CAP-004 | none recorded |
| REQ-CAP-006 — HDMI audio capture | PROPOSED (OQ-004, OQ-017). *Superseded 2026-10-08:* DRAFT since 2026-10-07 — owner: "Yes, audio required" (OQ-004 ANSWERED), as recorded in [REQUIREMENTS.md](REQUIREMENTS.md). Channels, sample rates and A/V tolerance still open (OQ-110, OQ-111, OQ-112, OQ-017) | [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); no ADR. *Added 2026-10-08:* no dedicated ADR, but ADR-004 (O) treats CM5 audio as a bring-up gate, and ADR-007 (O) is to record the A/V clock model (OQ-112) and, per OQ-111, the sample-rate policy | none — NOT STARTED | TEST-AUD-001 | none recorded |
| REQ-CAP-007 — 2-lane and 4-lane CSI-2 configurations, all frame rates each link carries | DRAFT (OQ-002, OQ-040, OQ-017) | [CSI_PIPELINE.md](CSI_PIPELINE.md), [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); ADR-004 (O, a choice per lane configuration), ADR-005 (P), ADR-008 (P) | none — NOT STARTED | TEST-CAP-002, TEST-CAP-004 | none recorded |
| REQ-CAP-008 — HDMI sources: ATEM switchers and cameras | DRAFT (OQ-102, OQ-017). OQ-102 answered 2026-10-07: any HDMI camera, no model list, plus ATEM outputs; the accepted sources are defined by the supported-mode matrix and the EDID (OQ-002) | [ATEM.md](ATEM.md), [V4L2.md](V4L2.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); no ADR | none — NOT STARTED | TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 | none recorded |
| REQ-DMA-001 — DMABUF buffer sharing | DRAFT (OQ-015, OQ-017) | [DMA.md](DMA.md), [VIDEO_ENCODER.md](VIDEO_ENCODER.md); ADR-004 (O), ADR-007 (O). *Added 2026-10-08:* one capture buffer feeds two encoders, recording and live (OQ-005); whether two CM4 encode sessions can import it is OQ-115 | none — NOT STARTED | TEST-DMA-001 | none recorded |
| REQ-ENC-001 — Video encoding (H.264) | DRAFT (OQ-005, OQ-017). Owner 2026-10-08: **H.264 only**, for all outputs ("H.264 only for now"; OQ-103 ANSWERED); H.265 is deferred as REQ-ENC-002. *(History: on 2026-10-07 the owner chose H.264 and H.265, and which outputs use H.265 was OQ-103.)* *Added later on 2026-10-08:* owner answer to OQ-005, "Separate record + live": two simultaneous H.264 encodes, a recording encode and one live encode shared by RTMP and WebRTC, with the WebRTC constraints on the live encode recorded in REQ-ENC-001. Bitrate, rate control and latency are still open (OQ-005) | [VIDEO_ENCODER.md](VIDEO_ENCODER.md), [PERFORMANCE.md](PERFORMANCE.md); ADR-004 (O), ADR-005 (P), ADR-007 (O) | none — NOT STARTED | TEST-ENC-001 (H.264 runs on CM4 and CM5; its H.265 runs are deferred — REQ-ENC-002; not run in current scope). *Added later on 2026-10-08:* includes a two-encode run (recording + live) on CM4 and CM5 (OQ-115, OQ-059) | none recorded |
| REQ-ENC-002 — H.265 (HEVC) encoding (deferred) | DEFERRED (2026-10-08; owner: "H.264 only for now", OQ-103 ANSWERED). Not in the current scope; to re-activate it, the owner changes the acceptance to DRAFT | Evidence kept for re-activation: [REQUIREMENTS.md](REQUIREMENTS.md) (REQ-ENC-002 and the REQ-ENC-001 notes), [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §4A, [STREAMING.md](STREAMING.md) §3.6 and §4.5, [PERFORMANCE.md](PERFORMANCE.md) §5.3; ADR-004 (O) and ADR-007 (O) hold its evidence (ADR-007 scope note: HEVC over RTMP does not drive the current choice) | none — NOT STARTED | none — deferred | none recorded |
| REQ-REC-001 — Recording | DRAFT (OQ-006, OQ-017) | [RECORDING.md](RECORDING.md); ADR-007 (O). *Added 2026-10-08:* fed by the recording encode (OQ-005) | none — NOT STARTED | TEST-REC-001 | none recorded |
| REQ-STR-001 — RTMP streaming | DRAFT (OQ-007, OQ-017) | [STREAMING.md](STREAMING.md); ADR-007 (O; since 2026-10-08 also the HEVC-over-RTMP path, OQ-107, RISK-025 — not in current scope per the ADR-007 scope note: H.265 deferred, REQ-ENC-002). *Added later on 2026-10-08:* fed by the live encode, shared with REQ-STR-002 (OQ-005) | none — NOT STARTED | TEST-STR-001 | none recorded |
| REQ-STR-002 — WebRTC streaming | DRAFT (OQ-008, OQ-017) | [STREAMING.md](STREAMING.md); ADR-007 (O). *Added 2026-10-08:* fed by the live encode, shared with REQ-STR-001, which must be WebRTC-receivable (REQ-ENC-001; OQ-005) | none — NOT STARTED | TEST-STR-002 | none recorded |
| REQ-ATEM-001 — Blackmagic ATEM integration | DRAFT (OQ-102, OQ-017). OQ-009 answered 2026-10-07: scope is HDMI capture of the ATEM output only; network tally/control (UDP 9910) and RTMP exchange are not in current scope. OQ-102 also answered 2026-10-07: no model list | [ATEM.md](ATEM.md); capture path as REQ-CAP-008; no ADR. [STREAMING.md](STREAMING.md) only for the RTMP option, which is not in current scope | none — NOT STARTED | TEST-ATEM-001 | none recorded |
| REQ-BLD-001 — Reproducible product image build | DRAFT (OQ-017). OQ-012 answered 2026-10-07: ADR-003 ACCEPTED (own image built with `rpi-image-gen`, package mirror for reproducibility) | [BUILD_SYSTEM.md](BUILD_SYSTEM.md), [RELEASE.md](RELEASE.md); ADR-003 (A) | none — NOT STARTED | TEST-BLD-001 | none recorded |
| REQ-BLD-002 — Project-owned product OS image | DRAFT (OQ-017). OQ-012 answered 2026-10-07: ADR-003 ACCEPTED (image built with `rpi-image-gen` from Raspberry Pi OS packages) | [BUILD_SYSTEM.md](BUILD_SYSTEM.md), [RELEASE.md](RELEASE.md); ADR-003 (A) | none — NOT STARTED | TEST-BLD-001 | none recorded |
| REQ-PERF-001 — Sustained operation | PROPOSED (OQ-010, OQ-017) | [PERFORMANCE.md](PERFORMANCE.md); ADR-004 (O) | none — NOT STARTED | TEST-PERF-001. *Added 2026-10-08:* includes the combined load of two H.264 encodes, two audio encodes and all three outputs (OQ-005) | none recorded |

Three requirements have no ADR:

- **REQ-CAP-006:** whether audio is needed at all is OQ-004. An ADR will be written once the owner answers. *(Superseded 2026-10-08: the owner answered on 2026-10-07 that audio is required. No audio ADR has been written yet. The open design points are the sample-rate policy (OQ-111) and the A/V clock model (OQ-112), both to be recorded with ADR-007 (OPEN), and the CM5 audio path, which ADR-004's analysis treats as a bring-up gate (OQ-054).)*
- **REQ-ATEM-001** and **REQ-CAP-008:** the owner answered the integration scope on 2026-10-07 (OQ-009: "it can be atem and direct video from camera"). The scope is recorded in the requirements (HDMI capture of the ATEM output and of cameras connected directly), not in an ADR; no ADR has been written for it. Which ATEM and camera models must be supported is still open (OQ-102). *(Superseded 2026-10-08: OQ-102 was answered on 2026-10-07 — "Any HDMI camera (generic)", no model list. What is accepted is defined by the supported-mode matrix and the EDID (OQ-002).)*

REQ-CAP-007 and REQ-BLD-002 (owner decisions of 2026-10-07) map to existing ADRs: ADR-004 (OPEN, status unchanged), now a platform choice per lane configuration, and ADR-003, whose recommendation of an own image built with `rpi-image-gen` the owner ACCEPTED on 2026-10-07 ("accept ADR-003"; OQ-012 ANSWERED).

## Risks and open questions per requirement

Each requirement is listed with the registered risks that could stop it being met ([RISKS.md](RISKS.md)) and the open questions that block its design or test ([OPEN_QUESTIONS.md](OPEN_QUESTIONS.md)). The OQ lists are the main ones, not every OQ that touches the area. No risk has been retired, because retiring a risk needs test evidence (Rule 10).

| Requirement | Risks | Main open questions |
|---|---|---|
| REQ-ARCH-001 | RISK-003, RISK-012 | OQ-014, OQ-015, OQ-090 |
| REQ-PLT-001 | RISK-001, RISK-012 | OQ-011, OQ-043, OQ-044, OQ-049, OQ-052, OQ-100 |
| REQ-DRV-001 | RISK-005, RISK-007, RISK-021 | OQ-013, OQ-018, OQ-019, OQ-020, OQ-026, OQ-029 |
| REQ-CAP-001 | RISK-001, RISK-006, RISK-008, RISK-011, RISK-016, RISK-020 | OQ-003, OQ-021, OQ-037, OQ-038, OQ-050, OQ-099 (OQ-001 ANSWERED 2026-10-07) |
| REQ-CAP-002 | RISK-012 | OQ-014, OQ-043, OQ-046 |
| REQ-CAP-003 | RISK-010 | OQ-002, OQ-024, OQ-032, OQ-093 |
| REQ-CAP-004 | RISK-013 | OQ-020, OQ-039, OQ-051 |
| REQ-CAP-005 | RISK-006, RISK-009 | OQ-002, OQ-030, OQ-035 |
| REQ-CAP-006 | RISK-014; added 2026-10-08: RISK-015 (AAC licensing), RISK-023, RISK-024 | OQ-004 (ANSWERED 2026-10-07), OQ-025, OQ-054, OQ-063; added 2026-10-08: OQ-024, OQ-110, OQ-111, OQ-112, OQ-113, OQ-114 |
| REQ-CAP-007 | RISK-001, RISK-006, RISK-011 | OQ-002, OQ-011, OQ-021, OQ-038, OQ-040, OQ-099 |
| REQ-CAP-008 | RISK-008, RISK-009 | OQ-102, OQ-002, OQ-028, OQ-078, OQ-083 (OQ-102 ANSWERED 2026-10-07) |
| REQ-DMA-001 | RISK-020 | OQ-058, OQ-060, OQ-062 |
| REQ-ENC-001 | RISK-002, RISK-003, RISK-015; added 2026-10-08: RISK-022, RISK-025 (both not in current scope since the OQ-103 answer — H.265 deferred; see REQ-ENC-002) | OQ-005, OQ-048, OQ-056, OQ-057, OQ-059, OQ-096; added 2026-10-08: OQ-060, OQ-103, OQ-104, OQ-105, OQ-109 (OQ-103 ANSWERED 2026-10-08; OQ-104, OQ-105 and OQ-109 not in current scope — see REQ-ENC-002); added later on 2026-10-08: OQ-115 (two concurrent encodes on CM4) |
| REQ-ENC-002 (DEFERRED) | RISK-022, RISK-025 and the H.265 licensing part of RISK-015 — OPEN, not in current scope | OQ-104, OQ-105, OQ-106, OQ-107, OQ-108, OQ-109 — OPEN, not in current scope (OQ-103 ANSWERED 2026-10-08) |
| REQ-REC-001 | none registered *(superseded 2026-10-08: RISK-022, RISK-023, RISK-024; RISK-022 not in current scope — H.265 deferred, REQ-ENC-002)* | OQ-006, OQ-069; added 2026-10-08: OQ-103, OQ-111, OQ-112, OQ-113 (OQ-103 ANSWERED 2026-10-08); added later on 2026-10-08: OQ-005 (recording encode) |
| REQ-STR-001 | RISK-003; added 2026-10-08: RISK-022, RISK-023, RISK-024, RISK-025 (RISK-022 and RISK-025 not in current scope — H.265 deferred, REQ-ENC-002) | OQ-007, OQ-063, OQ-075; added 2026-10-08: OQ-076, OQ-103, OQ-106, OQ-107, OQ-111, OQ-113 (OQ-103 ANSWERED 2026-10-08; OQ-106 and OQ-107 not in current scope); added later on 2026-10-08: OQ-005 (shared live encode) |
| REQ-STR-002 | RISK-003, RISK-019; added 2026-10-08: RISK-022, RISK-023, RISK-024 (RISK-022 not in current scope — H.265 deferred, REQ-ENC-002) | OQ-008, OQ-073, OQ-074; added 2026-10-08: OQ-103, OQ-108, OQ-111 (OQ-103 ANSWERED 2026-10-08; OQ-108 not in current scope); added later on 2026-10-08: OQ-005 (shared live encode) |
| REQ-ATEM-001 | RISK-008 (RISK-018 applies only to network control, not in current scope) | OQ-078, OQ-083, OQ-102 (OQ-009 ANSWERED 2026-10-07; OQ-102 ANSWERED 2026-10-07; OQ-077 and OQ-084 apply only to network control, not in current scope) |
| REQ-BLD-001 | RISK-015, RISK-017 | OQ-067, OQ-068, OQ-071 (OQ-012 ANSWERED 2026-10-07) |
| REQ-BLD-002 | RISK-017 | OQ-064, OQ-067, OQ-071 (OQ-012 ANSWERED 2026-10-07) |
| REQ-PERF-001 | RISK-003, RISK-006, RISK-020 | OQ-010, OQ-035, OQ-053, OQ-059, OQ-061; added 2026-10-08: OQ-104 (not in current scope — H.265 deferred, REQ-ENC-002) |

RISK-016 (RGB888 pixel-format label differs between receivers) affects ADR-005, so it is listed under REQ-CAP-001, whose pixel format ADR-005 proposes. RISK-001 is registered against REQ-CAP-001; it is also listed under REQ-CAP-007 because it sets the 2-lane configuration's limit. RISK-018 is kept against REQ-ATEM-001 only as a reference: it concerns the UDP 9910 network option, which the owner did not select on 2026-10-07 (OQ-009). RISK-004 (TC358743 supply and lifecycle) affects the product as a whole, not one requirement, so it appears in no row; it is tracked through OQ-085.

*(Added 2026-10-08.)* The risks and open questions added since 2026-10-07 are placed by the "Affects" column of [RISKS.md](RISKS.md) and the "Why it matters" field of [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md):

- **RISK-022** (H.265 required but software-only on every candidate) → REQ-ENC-001, REQ-STR-001, REQ-STR-002, REQ-REC-001. *(Not in current scope since the OQ-103 answer; see the last bullet.)*
- **RISK-023** (HDMI audio sample-rate mismatch not detected by ALSA) → REQ-CAP-006, REQ-REC-001, REQ-STR-001, REQ-STR-002.
- **RISK-024** (A/V synchronisation across separate clock domains) → the same four requirements, and ADR-007.
- **RISK-025** (HEVC over RTMP may force a split GStreamer/FFmpeg architecture) → REQ-STR-001, REQ-ENC-001, and ADR-007. *(Not in current scope since the OQ-103 answer; see the last bullet.)*
- **RISK-015** was extended on 2026-10-08 to H.265 and AAC licensing, so it now also appears under REQ-CAP-006.
- **OQ-104** (H.265 throughput) and **OQ-105** (x265 SIMD paths) also inform ADR-004 (CM4 versus CM5); OQ-104 is listed under REQ-PERF-001 as well. *(Both not in current scope since the OQ-103 answer; see the last bullet.)*
- **OQ-109** (HEVC patent licensing) and **OQ-113** (AAC patent licensing) have no resolving test; they are LEGAL CLARIFICATION REQUIRED and block product release, not a test. *(OQ-109: not in current scope since the OQ-103 answer; see the last bullet.)*
- **OQ-114** (GPIO 18–21 allocation) also concerns the carrier design (OQ-018).
- *(Added later on 2026-10-08.)* After the owner's answer to OQ-103 ("H.264 only for now"), RISK-022, RISK-025 and OQ-104 to OQ-109 are not in the current scope. They stay OPEN, are listed under REQ-ENC-002 (DEFERRED), and are marked "not in current scope" where they remain in other rows. OQ-103 is ANSWERED. The H.265 licensing part of RISK-015 therefore also applies only if REQ-ENC-002 is re-activated; its H.264 and AAC parts are unchanged. OQ-109 no longer blocks release in the current scope; OQ-113 still does.
- *(Added later on 2026-10-08, owner answer to OQ-005: two H.264 encodes.)* **OQ-115** (two concurrent H.264 encodes on the CM4 hardware encoder) is placed under REQ-ENC-001, per its "Why it matters". Its resolving tests are TEST-ENC-001 and TEST-PERF-001, so TEST-PERF-001 (REQ-PERF-001) also measures it. **OQ-005** is now also listed under REQ-REC-001, REQ-STR-001 and REQ-STR-002, which its "Why it matters" names. The parameters of the recording encode and of the shared live encode are still open there. **RISK-002** and **RISK-003** gained an owner-decision note in [RISKS.md](RISKS.md); their "Affects" columns are unchanged, so their rows here are unchanged. OQ-059 (CM5 concurrency) was already listed under REQ-ENC-001 and REQ-PERF-001.

## Source evidence added on 2026-10-08 (research topics H and I)

The source research of 2026-10-08 added topic H (H.265 software encoding and transport, H-01 to H-43) and topic I (HDMI audio path, I-01 to I-47) to [REFERENCES.md](REFERENCES.md). This table records, by ID only, where that evidence is attached. The facts are stated, with their tiers, in the linked documents. Evidence from sources is not a result: every Result cell above stays `none recorded`.

| Requirement or decision | Topic H / I facts cited | Where cited | Test that checks it on hardware |
|---|---|---|---|
| REQ-CAP-006 | I-01 to I-10, I-13 to I-15, I-18 to I-22, I-24 to I-27, I-29 to I-31, I-33, I-35 to I-39, I-41, I-43 to I-46 | [REQUIREMENTS.md](REQUIREMENTS.md) REQ-CAP-006; RISK-014, RISK-023, RISK-024; OQ-054, OQ-110 to OQ-112, OQ-114 | TEST-AUD-001 (CM4 and CM5) |
| REQ-ENC-001 | H-01, H-02, H-04, H-05, H-08 to H-13, H-15, H-16, H-19 to H-23, H-43 | [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-001; RISK-022; OQ-103 to OQ-105 | none in current scope: the TEST-ENC-001 H.265 runs (CM4 and CM5) are deferred — REQ-ENC-002; not run in current scope |
| REQ-ENC-002 (DEFERRED) | H-19, H-26, H-27, H-33, H-35; it also points to the REQ-ENC-001 notes, whose topic H facts are listed in the row above | [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-002; RISK-022, RISK-025; OQ-104 to OQ-109 | none — deferred |
| REQ-REC-001 | H-37, H-38, I-40, I-44, I-46 | [REQUIREMENTS.md](REQUIREMENTS.md) REQ-REC-001 | TEST-REC-001 |
| REQ-STR-001 | H-24 to H-27, H-29, H-30, I-40, I-42 to I-44, I-46 | [REQUIREMENTS.md](REQUIREMENTS.md) REQ-STR-001; RISK-025; OQ-106, OQ-107, OQ-113 | TEST-STR-001 |
| REQ-STR-002 | H-31 to H-36, I-41, I-45, I-47 | [REQUIREMENTS.md](REQUIREMENTS.md) REQ-STR-002; RISK-019; OQ-108 | TEST-STR-002 |
| ADR-004 (OPEN) — platform | H-04, H-05, H-19 to H-23, I-05 to I-07, I-10, I-13 | [DECISIONS.md](DECISIONS.md) ADR-004 Analysis; OQ-011 | TEST-ENC-001, TEST-AUD-001 |
| ADR-007 (OPEN) — media framework | H-08 to H-13, H-15, H-16, H-26, H-27, H-30, H-31, H-37, H-38, I-26, I-33 to I-39, I-41, I-44 to I-46 | [DECISIONS.md](DECISIONS.md) ADR-007; RISK-024, RISK-025; OQ-107, OQ-112 | TEST-STR-001, TEST-AUD-001 |

No requirement acceptance, ADR status or test mapping changed with this evidence. The ID lists were taken from [REQUIREMENTS.md](REQUIREMENTS.md) and [DECISIONS.md](DECISIONS.md) as they stood on 2026-10-08.

*(Added later on 2026-10-08.)* H.265 is deferred (owner: "H.264 only for now"; OQ-103 ANSWERED; REQ-ENC-002 DEFERRED). The topic H facts in the REQ-REC-001, REQ-STR-001, REQ-STR-002, ADR-004 and ADR-007 rows are kept as evidence for REQ-ENC-002 (deferred — REQ-ENC-002; not in current scope). In the current scope, the tests named in those rows check the topic I facts and the H.264 path only. One exception: [H-26] also says that FFmpeg's FLV muxer has no Opus, which concerns audio and stays in scope.

## Rule 12 chain for REQ-CAP-001

This is the Rule 12 example chain, adapted to the real state on 2026-10-06 and updated for the owner decisions of 2026-10-07. Each stage shows its status. Facts are cited from [REFERENCES.md](REFERENCES.md).

```text
REQ-CAP-001 — capture HDMI input through the TC358743 at 1920x1080@60Hz
    Acceptance: DRAFT. OQ-001 answered 2026-10-07: 1080p60 is required on the 4-lane
    configuration; 2-lane configurations capture every rate their link carries (REQ-CAP-007).
    60 Hz vs 59.94 Hz, 50 Hz, pixel format and audio scope are owner decisions
    (OQ-002, OQ-003, OQ-004, OQ-017).
    (2026-10-08: audio scope answered — OQ-004 ANSWERED 2026-10-07, audio required, REQ-CAP-006.
    Bring-up runs this chain on CM4 CAM1 and CM5, side by side; ADR-004 OPEN.)
    ↓
TC358743 driver — in-tree drivers/media/i2c/tc358743.c [A-35]
    Design: PROPOSED, not decided: use it unmodified for bring-up (ADR-002 PROPOSED, OQ-013) · TC358743_DRIVER.md
    Implementation: NOT STARTED (no PACSCORDER integration, no patches)
    ↓
Device Tree — tc358743 overlay (Pi 4/CM4); dtoverlay=tc358743 resolves to tc358743-pi5 on Pi 5/CM5 [C-11]
    Design: DEVICE_TREE.md · ADR-004 (OPEN) for the platform of the 4-lane configuration
            · ADR-008 (PROPOSED, OQ-099): keep link-frequency 486 MHz; evaluate 297 MHz on CM4 CAM1 only
    Needed for 1080p60: a 4-lane link (reasoning [C-47]). Necessary, not shown sufficient: at 972 Mbps
    1080p60 UYVY uses 3 of the 4 lanes, which is unproven (OQ-038)
    Declared REFCLK must equal the board oscillator [A-45], [B-47]
    Board facts: lane routing, REFCLK and I2C address are UNKNOWN — VERIFICATION REQUIRED (OQ-019, OQ-021, OQ-026)
    Implementation: NOT STARTED
    ↓
V4L2 / Media Controller — PROPOSED, not decided: Media Controller mode on every platform,
    EDID and DV timings on /dev/v4l-subdevN (ADR-006 PROPOSED, OQ-014); UYVY capture (ADR-005 PROPOSED, OQ-003)
    Design: V4L2.md · CSI_PIPELINE.md
    Implementation: NOT STARTED
    ↓
Capture application — EDID load, timing query, pad and video-node formats, streaming
    Design: SOFTWARE_ARCHITECTURE.md · ADR-007 (OPEN)
    Implementation: NOT STARTED
    ↓
1080p60 test — TEST-CAP-002 (after TEST-HW-001, TEST-DRV-001, TEST-DRV-002, TEST-PLT-001, TEST-CAP-001)
    Status: BLOCKED — HARDWARE REQUIRED — not run
    ↓
Result: none recorded
```

**Source constraints that apply at each stage.** These are not results.

- **Platform.**
  - Pi 4 Model B has a 2-lane camera connector [C-01], and so does the CM4 CAM0 connector [C-02]. Official documentation limits 2 lanes to 1080p30 RGB888 or 1080p50 YUV422 [C-37]. So this chain cannot complete on Pi 4 Model B or on CM4 CAM0 (RISK-001).
  - Under the owner decision of 2026-10-07 they are not excluded from the product: they remain candidates for the 2-lane configuration of REQ-CAP-007, which captures every rate the 2-lane link carries — for 1920x1080, at most 1080p50 UYVY or 1080p30 RGB888 (reasoning [C-48]; [C-37]). The 2-lane configuration is verified through the TEST-CAP-004 supported-mode matrix, not through TEST-CAP-002.
  - 4-lane connectors exist on CM4 CAM1 [C-02], on both Pi 5 ports [C-04] and on CM5 [C-05]. A 4-lane connector is necessary but not shown to be sufficient (see Lanes).
  - *(Added 2026-10-08.)* Owner, 2026-10-07: bring-up evaluates CM4 and CM5 side by side, and ADR-004 stays OPEN until measured. For this 4-lane chain that means CM4 CAM1 and CM5.
- **Lanes** (reasoning). At the overlay default of 972 Mbps per lane, the driver requests 3 lanes for 1080p60 UYVY and 4 for RGB888 [C-47].
  - Capture with 3 of 4 lanes active is unproven (OQ-038), so a 4-lane port alone is not shown to be sufficient for 1080p60 UYVY.
  - ADR-008 (PROPOSED; OQ-099) proposes keeping 486 MHz (972 Mbps) on every platform. It also proposes evaluating 297 MHz (594 Mbps) on CM4 CAM1 with 4 lanes in TEST-CAP-002, where 1080p60 UYVY uses all 4 lanes [C-47], [C-49].
- **Driver.**
  - A reference-clock value other than 26, 27 or 42 MHz leads to a kernel BUG instead of a clean probe failure (kernel source [A-22] and reasoning from it [B-11], both CORRECTED; RISK-007).
  - The FIFO level is hard-coded at 374 [B-12]. An open issue reports image corruption near lane-count limits [C-43] (RISK-006).
- **Pi 5 / CM5 receiver.**
  - The CFE programs its D-PHY for 999 Mbps for this bridge [C-31] (RISK-011).
  - A Raspberry Pi kernel issue reporter found that leaving `field:none` off the csi2 pad formats made STREAMON fail with `-EPIPE` [C-33].
- **EDID.** No source sees a sink until userspace writes an EDID [A-33], [B-21] (REQ-CAP-003, RISK-010).
- **Encode** (outside REQ-CAP-001, but needed for a product). The Pi 4/CM4 hardware encoder is officially specified for 1080p30 [D-10] (RISK-002). Pi 5/CM5 have no hardware video encoder [D-31], [G-22] (RISK-003). *(Added 2026-10-08.)* H.265 is also required (REQ-ENC-001), and no candidate has a hardware HEVC encoder [D-24], [D-31], so H.265 is software-encoded on CM4 as well as CM5 (RISK-022; OQ-103, OQ-104). *(Superseded later on 2026-10-08: the owner deferred H.265 — "H.264 only for now", OQ-103 ANSWERED. REQ-ENC-001 is H.264 only; H.265 is REQ-ENC-002 (DEFERRED), and RISK-022 and OQ-104 are not in the current scope. [D-24] stays as evidence for REQ-ENC-002.)* *(Added later on 2026-10-08.)* Owner answer to OQ-005, "Separate record + live": two simultaneous H.264 encodes are required, a recording encode and one live encode shared by RTMP and WebRTC (REQ-ENC-001).
  - CM4: reasoning: two 1080p30 encodes need the macroblock rate of one 1080p60 encode, about 2.0× the 1080p30 specification [D-10], [D-52]. Whether the hardware encoder runs both is OQ-115 (RISK-002).
  - CM5: both encodes are software. Reasoning from [G-22]: roughly double the encode CPU of one encode (OQ-059, RISK-003).

## Reverse table: test → requirements

Taken from the canonical test table in [README.md](README.md).

| Test ID | Title | Verifies (requirements) | Status |
|---|---|---|---|
| TEST-HW-001 | TC358743 I2C detection | REQ-DRV-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-DRV-001 | TC358743 driver probe and chip ID | REQ-DRV-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-DRV-002 | EDID load and HDMI hot-plug assertion | REQ-CAP-003 | BLOCKED — HARDWARE REQUIRED |
| TEST-PLT-001 | Overlay load and media graph per platform | REQ-PLT-001, REQ-ARCH-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-001 | DV timings detection for each source mode | REQ-CAP-002, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-002 | 1080p60 capture: frame rate and frame integrity | REQ-CAP-001, REQ-CAP-007 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-003 | Source connect / disconnect / mode change | REQ-CAP-004 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-004 | Unsupported-mode rejection and supported-mode matrix | REQ-CAP-005, REQ-CAP-007, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-AUD-001 | HDMI audio capture over I2S | REQ-CAP-006 | BLOCKED — HARDWARE REQUIRED |
| TEST-DMA-001 | DMABUF capture → encoder buffer sharing | REQ-DMA-001, REQ-ARCH-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-ENC-001 | Sustained real-time H.264 encode (H.265 deferred) | REQ-ENC-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-REC-001 | Recording integrity and duration | REQ-REC-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-STR-001 | RTMP publish and playback | REQ-STR-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-STR-002 | WebRTC browser playback | REQ-STR-002 | BLOCKED — HARDWARE REQUIRED |
| TEST-ATEM-001 | ATEM HDMI output capture (scope per OQ-009) | REQ-ATEM-001, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-BLD-001 | Image build from clean checkout | REQ-BLD-001, REQ-BLD-002 | NOT STARTED |
| TEST-PERF-001 | Soak: thermal, CPU, CMA, frame drops | REQ-PERF-001 | BLOCKED — HARDWARE REQUIRED |

*(Added 2026-10-08.)* Titles are the canonical ones from [README.md](README.md). Since 2026-10-08 the [TESTING.md](TESTING.md) procedure for TEST-ENC-001 also contains H.265 runs on CM4 and CM5 (REQ-ENC-001), although its canonical title still says "H.264". TEST-AUD-001 now has a full procedure for CM4 and CM5, and TEST-REC-001, TEST-STR-001, TEST-STR-002 and TEST-PERF-001 carry H.265 and audio expectations. No test ID, title or "Verifies" mapping changed, and every status above is unchanged. *(Superseded later on 2026-10-08: after the owner's answer to OQ-103 ("H.264 only for now"), TEST-ENC-001 is titled "Sustained real-time H.264 encode (H.265 deferred)" in [README.md](README.md). Its H.265 runs, and the H.265 expectations in TEST-REC-001, TEST-STR-001, TEST-STR-002 and TEST-PERF-001, are kept in [TESTING.md](TESTING.md) but are deferred — REQ-ENC-002; not run in current scope. REQ-ENC-002 has no test, so it appears in no row of this table. No test ID or "Verifies" mapping changed, and every status is unchanged.)*

*(Added later on 2026-10-08.)* After the owner's answer to OQ-005 ("Separate record + live": two H.264 encodes), the [TESTING.md](TESTING.md) procedures changed as follows:
- TEST-DMA-001 records one capture buffer feeding two encoders;
- TEST-ENC-001 includes a two-encode run on CM4 and CM5 (OQ-115, OQ-059);
- TEST-REC-001 uses the recording encode, and TEST-STR-001 and TEST-STR-002 share the live encode;
- TEST-PERF-001 measures the combined load: two video encodes, two audio encodes and all three outputs.

No test ID, title or "Verifies" mapping changed, and every status above is unchanged.

## Coverage checks

Checked on 2026-10-06 against [REQUIREMENTS.md](REQUIREMENTS.md) (17 requirements) and the canonical test table in [README.md](README.md) (17 tests). Rechecked on 2026-10-07 after the owner decisions: 20 requirements (REQ-CAP-007, REQ-CAP-008 and REQ-BLD-002 added) and the same 17 tests, whose "Verifies" lists were extended in [README.md](README.md). Rechecked on 2026-10-08 after the second set of owner decisions of 2026-10-07 and research topics H and I: still 20 requirements and 17 tests, with no new requirement and no new test ID, so the two checks below are unchanged. Rechecked again on 2026-10-08 after the owner's answer to OQ-103: 21 requirements (REQ-ENC-002 added, DEFERRED) and the same 17 tests. *(Added later on 2026-10-08.)* Rechecked after the owner's answer to OQ-005 (two H.264 encodes): still 21 requirements and 17 tests, with no new requirement and no new test ID, so the two checks below are unchanged.

### Requirements without tests

None in the current scope. *(Since 2026-10-08, REQ-ENC-002 (DEFERRED) deliberately has no test: a DEFERRED requirement has no planned tests ([REQUIREMENTS.md](REQUIREMENTS.md)).)* Every other requirement has at least one canonical test:

- REQ-ARCH-001 has two (TEST-PLT-001, TEST-DMA-001);
- REQ-DRV-001 has two (TEST-HW-001, TEST-DRV-001);
- REQ-CAP-007 has two (TEST-CAP-002, TEST-CAP-004);
- REQ-CAP-008 has three (TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001);
- every other requirement has one. REQ-BLD-002 shares TEST-BLD-001 with REQ-BLD-001.

### Tests without requirements

None. Every canonical test verifies at least one requirement:

- TEST-CAP-004 verifies three (REQ-CAP-005, REQ-CAP-007, REQ-CAP-008);
- TEST-PLT-001, TEST-DMA-001, TEST-CAP-001, TEST-CAP-002, TEST-ATEM-001 and TEST-BLD-001 each verify two;
- every other test verifies one.

### Coverage limits (not gaps in the ID mapping)

- **Partial coverage of REQ-ARCH-001.** TEST-PLT-001 covers HDMI → TC358743 → CSI-2 → receiver → Media Controller → V4L2. TEST-DMA-001 covers V4L2 → DMABUF → encoder. The "Recorder / RTMP / WebRTC" end of the chain is exercised only through TEST-REC-001, TEST-STR-001 and TEST-STR-002, which are mapped to their own requirements, not to REQ-ARCH-001. *(Added 2026-10-08.)* Since the owner's answer to OQ-005, the encoder stage is two H.264 encodes fed from one capture. TEST-DMA-001 covers the capture buffer reaching both encoders. Whether both sustain real time is measured by TEST-ENC-001, which is mapped to REQ-ENC-001, not to REQ-ARCH-001.
- **No pass criteria yet.** No test can be recorded as `TESTED — PASS` against an accepted criterion until the owner accepts acceptance criteria (OQ-017). [TESTING.md](TESTING.md) lists source-predicted results only.
- **Platform-dependent tests.** TEST-CAP-002's 1080p60 run cannot be done on Pi 4 Model B or on CM4 CAM0, which are 2-lane [C-01], [C-02], [C-37]. These platforms remain candidates for the 2-lane configuration of REQ-CAP-007 (owner, 2026-10-07); that configuration is covered only by the TEST-CAP-004 supported-mode matrix, at the rates a 2-lane link carries [C-48]. TEST-AUD-001 applies only if the owner requires audio (OQ-004). *(Superseded 2026-10-08: the owner requires audio — OQ-004 ANSWERED 2026-10-07 — so TEST-AUD-001 applies on every board.)*
- **Source coverage (REQ-CAP-008).** TEST-ATEM-001 covers the ATEM source only, and only HDMI capture of its output; its network-control and RTMP parts are not in current scope (OQ-009 answered 2026-10-07). Cameras are covered by TEST-CAP-001 and TEST-CAP-004 only. Both depend on the model list, which is open (OQ-102). *(Superseded 2026-10-08: OQ-102 was answered on 2026-10-07 with no model list — any HDMI camera plus ATEM outputs. The tests run with representative cameras and an ATEM, and coverage is judged against the supported-mode matrix and the EDID (OQ-002), not against a model list.)*
- *(Added 2026-10-08.)* **Bring-up platforms.** The owner chose to evaluate CM4 and CM5 side by side (ADR-004 OPEN). Every hardware test is to be run on both, and a result on one board does not cover the other. On CM5, TEST-AUD-001 is also a platform gate, because no source shows audio captured through that path on CM5 (OQ-054).
- *(Added 2026-10-08.)* **Audio coverage (REQ-CAP-006).** Only TEST-AUD-001 is mapped to REQ-CAP-006. Audio inside the outputs is exercised by TEST-REC-001, TEST-STR-001 and TEST-STR-002, and long-run A/V drift by TEST-PERF-001 (RISK-024), but those tests are mapped to their own requirements, as for REQ-ARCH-001 above.
- *(Added 2026-10-08.)* **H.265 coverage (REQ-ENC-001).** TEST-ENC-001 is the only test mapped to REQ-ENC-001. H.265 in recording, RTMP and WebRTC is exercised by TEST-REC-001, TEST-STR-001 and TEST-STR-002, as far as the owner scopes H.265 to each output (OQ-103). HEVC patent and AAC licensing (OQ-109, OQ-113) are LEGAL CLARIFICATION REQUIRED and are covered by no test. *(Superseded later on 2026-10-08: H.265 is deferred — owner, "H.264 only for now" (OQ-103 ANSWERED). TEST-ENC-001 covers REQ-ENC-001 with H.264 only. REQ-ENC-002 (DEFERRED) has no test, and the H.265 runs in TEST-ENC-001, TEST-REC-001, TEST-STR-001, TEST-STR-002 and TEST-PERF-001 are deferred, not run in current scope. OQ-109 is not in the current scope; AAC licensing (OQ-113) is still covered by no test.)*
- *(Added later on 2026-10-08.)* **Two-encode coverage (REQ-ENC-001).** TEST-ENC-001 is the only test mapped to REQ-ENC-001. It includes the two-encode run (recording + live) on CM4 and CM5 (OQ-115, OQ-059; RISK-002, RISK-003).
  - The live encode shared by RTMP and WebRTC is exercised by TEST-STR-001 and TEST-STR-002 run together.
  - The combined load with audio and all outputs is exercised by TEST-PERF-001.
  - Those tests are mapped to their own requirements. Bitrate, rate control and latency have no criteria yet (OQ-005, OQ-017).
- **Own product image (REQ-BLD-002).** Bring-up hardware tests run on the stock Raspberry Pi OS Lite image (ADR-003, ACCEPTED). Their results do not by themselves verify the project-built image; TEST-BLD-001 step 4 re-runs TEST-PLT-001 on it.

## How to update this document

- When an implementation exists, replace "none — NOT STARTED" with the file paths or component names and their Rule 10 status.
- When a test is run, record it first in the [TESTING.md](TESTING.md) result log. Then copy the latest result (Rule 10 status, date) into the Result column, and update the matching row in the reverse table.
- When the owner accepts a requirement, change its Acceptance value and remove the OQ IDs that the decision answered.
- Never delete a row. A withdrawn requirement keeps its row, marked `WITHDRAWN` with the reason (Rule 11, Rule 14).
- Add a change-history row for every update (Rule 21).

## Verification status

### Verified from sources (fact IDs)

This document is a mapping of IDs; the requirement, design and test content it points to is cited in the linked documents. The source constraints in the REQ-CAP-001 chain cite these entries of [REFERENCES.md](REFERENCES.md), all with verdict `CONFIRMED` or `CORRECTED`:

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-22, A-33, A-35, A-45 |
| B — tc358743 Linux driver | B-11, B-12, B-21, B-47 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-04, C-05, C-11, C-31, C-33, C-37, C-43, C-47, C-48, C-49 |
| D — Encoders | D-10, D-24, D-31; added later on 2026-10-08: D-52 (two-encode constraint) |
| G — Raspberry Pi OS and image tooling | G-22 |
| H — H.265 software encoding and transport *(added 2026-10-08; by ID in the evidence-mapping table only)* | H-01, H-02, H-04, H-05, H-08 to H-13, H-15, H-16, H-19 to H-27, H-29 to H-38, H-43 |
| I — HDMI audio path *(added 2026-10-08; by ID in the evidence-mapping table only)* | I-01 to I-10, I-13 to I-15, I-18 to I-22, I-24 to I-27, I-29 to I-31, I-33 to I-47 |

- `CORRECTED` entries cited, used in their corrected wording: A-22, B-11, B-21. *(Added 2026-10-08, by ID only: H-10, H-12, I-40; the linked documents use their corrected wording.)*
- `community` entries cited, worded as reports: C-33, C-43. *(Added 2026-10-08, by ID only: H-19, H-20, H-21, H-22, H-36; worded as reports in the linked documents.)*
- `reasoning` entries cited, labelled as reasoning: B-11, B-47, C-47, C-48, C-49. *(Added 2026-10-08, by ID only: H-23, H-43, I-18; labelled as reasoning in the linked documents.)* *(Added later on 2026-10-08: D-52, in the REQ-CAP-001 encode constraint.)*
- *(Added 2026-10-08.)* The topic H and I rows list the IDs that the evidence-mapping table names. This document states none of those facts itself, except [D-24] in the REQ-CAP-001 encode constraint. *(Added later on 2026-10-08.)* The two-encode constraint there uses [D-10], [D-52] and [G-22] as inputs to reasoning; of these only [D-52] is a reasoning-tier entry. The WebRTC constraints on the live encode are cited in [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-001 and [TESTING.md](TESTING.md), not here.
- The ID mappings were checked against [REQUIREMENTS.md](REQUIREMENTS.md), [README.md](README.md), [DECISIONS.md](DECISIONS.md), [RISKS.md](RISKS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) as they stood on 2026-10-06, and again on 2026-10-07 against the owner-decision entries in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) and [README.md](README.md). *(Added 2026-10-08.)* Checked again on 2026-10-08 against [REQUIREMENTS.md](REQUIREMENTS.md) (REQ-CAP-006 DRAFT, REQ-ENC-001 codecs), [DECISIONS.md](DECISIONS.md) (ADR-004 and ADR-007 still OPEN), [RISKS.md](RISKS.md) (RISK-014, RISK-015, RISK-019, RISK-022 to RISK-025) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) (OQ-004 and OQ-102 ANSWERED; OQ-103 to OQ-114 OPEN), as they stood on that date. *(Added later on 2026-10-08.)* Checked again after the owner's answer to OQ-103 against [REQUIREMENTS.md](REQUIREMENTS.md) (21 requirements; REQ-ENC-001 H.264 only; REQ-ENC-002 DEFERRED), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) (OQ-103 ANSWERED; OQ-104 to OQ-109 OPEN, not in current scope), [RISKS.md](RISKS.md) (RISK-022 and RISK-025 OPEN, not in current scope), [DECISIONS.md](DECISIONS.md) (ADR-007 scope note; no ADR status changed) and [README.md](README.md) (TEST-ENC-001 title). *(Added later on 2026-10-08.)* Checked again after the owner's answer to OQ-005 against [REQUIREMENTS.md](REQUIREMENTS.md) (REQ-ENC-001 two encodes; acceptance still DRAFT), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) (OQ-005 OPEN with the owner input; OQ-115 OPEN; OQ-059 OPEN) and [RISKS.md](RISKS.md) (RISK-002 and RISK-003 OPEN, with owner-decision notes; "Affects" unchanged).

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). Every Implementation cell is `NOT STARTED` and every Result cell is `none recorded`; every hardware test is `BLOCKED — HARDWARE REQUIRED` and TEST-BLD-001 is `NOT STARTED`.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review: REQ-CAP-001 chain now labels the ADR-002/005/006 design choices as PROPOSED inside the chain and cites the overlay, lane and REFCLK constraints; Pi 5/CM5 encoder statement now also cites [D-31]; RISK-016 added to REQ-CAP-001; note added that RISK-004 maps to no single requirement. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: ADR-008 (PROPOSED; OQ-099) added to the REQ-CAP-001 design cell and the Rule 12 chain; 4-lane link stated as necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes, OQ-038) with [C-49]; CM4 CAM0 added as 2-lane wherever Pi 4 Model B is excluded [C-02]; "NOT RUN" replaced by `none recorded` in the Result column and by `BLOCKED — HARDWARE REQUIRED — not run` in the chain (Rule 10); Design column wording no longer says PROPOSED ADRs "decide"; RISK-016 described as a pixel-format-label difference, matching RISKS.md; OQ-090, OQ-093, OQ-096, OQ-099 and OQ-100 added to the per-requirement OQ lists. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: rows for REQ-CAP-007, REQ-CAP-008 and REQ-BLD-002 added to the forward matrix and the risks/OQ table, and to the reverse test table's "Verifies" (TEST-CAP-001, -002, -004, TEST-ATEM-001, TEST-BLD-001, matching README); REQ-CAP-001 rows and Rule 12 chain record OQ-001 as answered (1080p60 required on the 4-lane configuration) and keep Pi 4 Model B / CM4 CAM0 as 2-lane candidates [C-48]; REQ-ATEM-001 rows record OQ-009 as answered (HDMI capture of the ATEM output only; RISK-018, OQ-077, OQ-084 and STREAMING.md kept as not in current scope) with OQ-102; no-ADR note and coverage checks updated (20 requirements, 17 tests); DRAFT definition aligned with REQUIREMENTS.md; C-48 added to the verification table. No ADR status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); REQ-BLD-001 and REQ-BLD-002 design cells ADR-003 (P) → (A), Acceptance cells and risks/OQ rows record OQ-012 as answered (requirement acceptance still DRAFT, implementation still NOT STARTED); ADR mapping note, own-product-image coverage note and header "Verification" row updated; a dated "Since 2026-10-07" line added below the 2026-10-06 state box, which is kept unchanged (Rule 21); TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)" in the reverse table, ID unchanged. | Claude (session 2026-10-07) |
| 2026-10-08 | Second set of owner decisions of 2026-10-07 and research topics H and I propagated; no requirement acceptance, ADR status, test mapping, implementation or result changed, and no row deleted. Header "Applies to" and "Verification" rows and a dated "Since 2026-10-08" state note. Forward matrix: REQ-PLT-001 (CM4 + CM5 bring-up, ADR-004 OPEN), REQ-CAP-001 (OQ-004 answered), REQ-CAP-006 (Acceptance cell's PROPOSED marked superseded by the DRAFT already recorded in REQUIREMENTS.md; design note on ADR-004 and ADR-007), REQ-CAP-008 and REQ-ATEM-001 (OQ-102 answered), REQ-ENC-001 (H.264 + H.265, OQ-103; TEST-ENC-001 H.265 runs), REQ-STR-001 (HEVC RTMP path under ADR-007). No-ADR notes for REQ-CAP-006 and REQ-CAP-008/REQ-ATEM-001 marked superseded. Risks/OQ table: RISK-015, RISK-022 to RISK-025 and OQ-024, OQ-060, OQ-076, OQ-103 to OQ-114 placed per RISKS.md "Affects" and OPEN_QUESTIONS.md "Why it matters"; REQ-REC-001 "none registered" marked superseded; explanatory note added. New section "Source evidence added on 2026-10-08" mapping topic H and I fact IDs to requirements, ADR-004, ADR-007 and tests. Rule 12 chain: OQ-004 answered and CM4/CM5 bring-up noted; encode constraint adds H.265 software-only [D-24]. Reverse table note on TEST-ENC-001 H.265 runs (title unchanged). Coverage checks rechecked (20 requirements, 17 tests); TEST-AUD-001 and OQ-102 coverage statements marked superseded; bring-up-platform, audio and H.265 coverage limits added. Verification status: D-24, topic H and I rows (by ID), tier lists and the 2026-10-08 register check. | Claude (session 2026-10-08) |
| 2026-10-08 | TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (owner chose H.264 + H.265 on 2026-10-07); ID unchanged. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header status, "Applies to" (21 requirements) and "Verification" rows; codec bullet of the "Since 2026-10-08" note marked superseded and a "Later on 2026-10-08" note added; `DEFERRED` added to the Acceptance column definition; forward matrix — REQ-ENC-001 row says H.264 only (2026-10-07 choice kept as history; TEST-ENC-001 H.265 runs deferred), new REQ-ENC-002 row (DEFERRED; tests: none — deferred), REQ-PLT-001 ADR-004 H.265 weighing marked deferred, REQ-STR-001 HEVC-over-RTMP design note marked not in current scope (ADR-007 scope note); risks/OQ table — new REQ-ENC-002 row, RISK-022, RISK-025 and OQ-104 to OQ-109 marked not in current scope and OQ-103 ANSWERED in the REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002 and REQ-PERF-001 rows; placement bullets for RISK-022, RISK-025, OQ-104/OQ-105 and OQ-109 annotated and a new scope bullet added; source-evidence table — REQ-ENC-001 test cell deferred, new REQ-ENC-002 row, note on the deferred topic H facts; Rule 12 chain encode constraint marked superseded; reverse table TEST-ENC-001 title set to the README title, note marked superseded; coverage checks rechecked (21 requirements, 17 tests; REQ-ENC-002 has no test by design), H.265 coverage limit marked superseded; Verification status register check added. No acceptance (other than recording REQ-ENC-002), ADR status, risk or OQ status, test mapping, implementation or result changed; no row deleted. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): header "Verification" row and an "Also on 2026-10-08" state note (recording encode + one live encode shared by RTMP and WebRTC; OQ-005 still OPEN for bitrate, rate control, latency; OQ-115, OQ-059; RISK-002, RISK-003; audio still AAC + Opus); forward matrix — REQ-ENC-001 Acceptance cell records the decision and its Test cell the two-encode run on CM4 and CM5, REQ-ARCH-001 and REQ-DMA-001 design notes (one capture feeding two encoders; OQ-115), REQ-REC-001 (recording encode), REQ-STR-001 and REQ-STR-002 (shared live encode, WebRTC-receivable), REQ-PERF-001 Test cell (combined load); risks/OQ table — OQ-115 under REQ-ENC-001, OQ-005 under REQ-REC-001, REQ-STR-001, REQ-STR-002, placement bullet; Rule 12 chain encode constraint (CM4 2.0× [D-10], [D-52]; CM5 roughly double CPU [G-22]; reasoning); reverse-table note on the changed procedures; coverage checks rechecked (21 requirements, 17 tests), REQ-ARCH-001 partial-coverage note, new two-encode coverage limit; Verification status — D-52 (reasoning) and the register check. No acceptance, ADR status, risk or OQ status, test ID, test mapping, implementation or result changed; no row deleted. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — Verification status note: the two-encode constraint "uses [D-10], [D-52] and [G-22] as inputs to reasoning; only [D-52] is a reasoning-tier entry" (was "states … as reasoning"). No status, mapping or ID changed. | Claude (session 2026-10-08) |
