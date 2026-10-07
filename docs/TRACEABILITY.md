# PACSCORDER Traceability Matrix

| | |
|---|---|
| Document status | Active — every requirement traced to design documents and tests; **nothing implemented, nothing tested** |
| Last updated | 2026-10-07 |
| Applies to | All 20 requirements in [REQUIREMENTS.md](REQUIREMENTS.md); all 17 canonical tests in [README.md](README.md) and [TESTING.md](TESTING.md); all candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5) |
| Verification | Built from the requirement, decision, risk and open-question registers of 2026-10-06 and the source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)); updated for the owner decisions of 2026-10-07 (REQ-CAP-007, REQ-CAP-008, REQ-BLD-002; OQ-001 and OQ-009 answered; OQ-102 added; ADR-003 ACCEPTED, OQ-012 answered). Nothing has been verified on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
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

## How to read the matrix

| Column | Content |
|---|---|
| Requirement | ID and short title from [REQUIREMENTS.md](REQUIREMENTS.md). |
| Acceptance | `DRAFT` (from the owner's rules or a dated owner statement; wording or criteria unconfirmed), `PROPOSED` (derived by Claude from research, not agreed) or `ACCEPTED`. No requirement is `ACCEPTED` yet (OQ-017). The open questions that block acceptance criteria are given in brackets. |
| Design | The documents that describe the design, and the ADRs that bear on it, with each ADR's status. Only an ACCEPTED ADR records a decision; a PROPOSED ADR only proposes, and an OPEN ADR is undecided. |
| Implementation | Code, configuration or hardware that implements the requirement. Rule 10 vocabulary. |
| Test | Canonical TEST IDs. Procedures are in [TESTING.md](TESTING.md). |
| Result | The latest recorded result in the [TESTING.md](TESTING.md) result log, in Rule 10 vocabulary. `none recorded` means no entry exists; the current test status is in the reverse table below. |

ADR status abbreviations used below: **A** = ACCEPTED, **P** = PROPOSED, **O** = OPEN.

## Forward matrix: requirement → design → implementation → test → result

| Requirement | Acceptance | Design (docs + ADR IDs) | Implementation | Test | Result |
|---|---|---|---|---|---|
| REQ-ARCH-001 — Video pipeline architecture | DRAFT (OQ-017) | [ARCHITECTURE.md](ARCHITECTURE.md), [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md), [CSI_PIPELINE.md](CSI_PIPELINE.md), [DMA.md](DMA.md); ADR-001 (A), ADR-006 (P), ADR-007 (O) | none — NOT STARTED | TEST-PLT-001, TEST-DMA-001 | none recorded |
| REQ-PLT-001 — Raspberry Pi platform coverage | DRAFT (OQ-011, OQ-017) | [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [CSI_PIPELINE.md](CSI_PIPELINE.md); ADR-004 (O), ADR-006 (P) | none — NOT STARTED | TEST-PLT-001 | none recorded |
| REQ-DRV-001 — TC358743 controlled by a Linux kernel driver | DRAFT (OQ-013, OQ-017) | [TC358743_DRIVER.md](TC358743_DRIVER.md), [DEVICE_TREE.md](DEVICE_TREE.md), [HARDWARE.md](HARDWARE.md); ADR-001 (A), ADR-002 (P) | none — NOT STARTED | TEST-HW-001, TEST-DRV-001 | none recorded |
| REQ-CAP-001 — 1920x1080@60 HDMI capture | DRAFT (OQ-002, OQ-003, OQ-004, OQ-017). OQ-001 answered 2026-10-07: 1080p60 is required on the 4-lane configuration (REQ-CAP-007) | [CSI_PIPELINE.md](CSI_PIPELINE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md), [DEVICE_TREE.md](DEVICE_TREE.md), [V4L2.md](V4L2.md); ADR-002 (P), ADR-004 (O, now a choice per lane configuration), ADR-005 (P), ADR-006 (P), ADR-008 (P) | none — NOT STARTED | TEST-CAP-002 | none recorded |
| REQ-CAP-002 — Capture through V4L2 and Media Controller | DRAFT (OQ-014, OQ-017) | [V4L2.md](V4L2.md), [CSI_PIPELINE.md](CSI_PIPELINE.md); ADR-001 (A), ADR-006 (P) | none — NOT STARTED | TEST-CAP-001 | none recorded |
| REQ-CAP-003 — EDID provisioning at every start | PROPOSED (OQ-002, OQ-017) | [TC358743_DRIVER.md](TC358743_DRIVER.md), [V4L2.md](V4L2.md); ADR-006 (P) | none — NOT STARTED | TEST-DRV-002 | none recorded |
| REQ-CAP-004 — Source change handling | PROPOSED (OQ-017, OQ-020) | [V4L2.md](V4L2.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); ADR-006 (P) | none — NOT STARTED | TEST-CAP-003 | none recorded |
| REQ-CAP-005 — Unsupported input mode handling | PROPOSED (OQ-002, OQ-017) | [CSI_PIPELINE.md](CSI_PIPELINE.md), [V4L2.md](V4L2.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); ADR-005 (P) | none — NOT STARTED | TEST-CAP-004 | none recorded |
| REQ-CAP-006 — HDMI audio capture | PROPOSED (OQ-004, OQ-017) | [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); no ADR | none — NOT STARTED | TEST-AUD-001 | none recorded |
| REQ-CAP-007 — 2-lane and 4-lane CSI-2 configurations, all frame rates each link carries | DRAFT (OQ-002, OQ-040, OQ-017) | [CSI_PIPELINE.md](CSI_PIPELINE.md), [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); ADR-004 (O, a choice per lane configuration), ADR-005 (P), ADR-008 (P) | none — NOT STARTED | TEST-CAP-002, TEST-CAP-004 | none recorded |
| REQ-CAP-008 — HDMI sources: ATEM switchers and cameras | DRAFT (OQ-102, OQ-017) | [ATEM.md](ATEM.md), [V4L2.md](V4L2.md), [TC358743_DRIVER.md](TC358743_DRIVER.md); no ADR | none — NOT STARTED | TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 | none recorded |
| REQ-DMA-001 — DMABUF buffer sharing | DRAFT (OQ-015, OQ-017) | [DMA.md](DMA.md), [VIDEO_ENCODER.md](VIDEO_ENCODER.md); ADR-004 (O), ADR-007 (O) | none — NOT STARTED | TEST-DMA-001 | none recorded |
| REQ-ENC-001 — Video encoding | DRAFT (OQ-005, OQ-017) | [VIDEO_ENCODER.md](VIDEO_ENCODER.md), [PERFORMANCE.md](PERFORMANCE.md); ADR-004 (O), ADR-005 (P), ADR-007 (O) | none — NOT STARTED | TEST-ENC-001 | none recorded |
| REQ-REC-001 — Recording | DRAFT (OQ-006, OQ-017) | [RECORDING.md](RECORDING.md); ADR-007 (O) | none — NOT STARTED | TEST-REC-001 | none recorded |
| REQ-STR-001 — RTMP streaming | DRAFT (OQ-007, OQ-017) | [STREAMING.md](STREAMING.md); ADR-007 (O) | none — NOT STARTED | TEST-STR-001 | none recorded |
| REQ-STR-002 — WebRTC streaming | DRAFT (OQ-008, OQ-017) | [STREAMING.md](STREAMING.md); ADR-007 (O) | none — NOT STARTED | TEST-STR-002 | none recorded |
| REQ-ATEM-001 — Blackmagic ATEM integration | DRAFT (OQ-102, OQ-017). OQ-009 answered 2026-10-07: scope is HDMI capture of the ATEM output only; network tally/control (UDP 9910) and RTMP exchange are not in current scope | [ATEM.md](ATEM.md); capture path as REQ-CAP-008; no ADR. [STREAMING.md](STREAMING.md) only for the RTMP option, which is not in current scope | none — NOT STARTED | TEST-ATEM-001 | none recorded |
| REQ-BLD-001 — Reproducible product image build | DRAFT (OQ-017). OQ-012 answered 2026-10-07: ADR-003 ACCEPTED (own image built with `rpi-image-gen`, package mirror for reproducibility) | [BUILD_SYSTEM.md](BUILD_SYSTEM.md), [RELEASE.md](RELEASE.md); ADR-003 (A) | none — NOT STARTED | TEST-BLD-001 | none recorded |
| REQ-BLD-002 — Project-owned product OS image | DRAFT (OQ-017). OQ-012 answered 2026-10-07: ADR-003 ACCEPTED (image built with `rpi-image-gen` from Raspberry Pi OS packages) | [BUILD_SYSTEM.md](BUILD_SYSTEM.md), [RELEASE.md](RELEASE.md); ADR-003 (A) | none — NOT STARTED | TEST-BLD-001 | none recorded |
| REQ-PERF-001 — Sustained operation | PROPOSED (OQ-010, OQ-017) | [PERFORMANCE.md](PERFORMANCE.md); ADR-004 (O) | none — NOT STARTED | TEST-PERF-001 | none recorded |

Three requirements have no ADR:

- **REQ-CAP-006:** whether audio is needed at all is OQ-004. An ADR will be written once the owner answers.
- **REQ-ATEM-001** and **REQ-CAP-008:** the owner answered the integration scope on 2026-10-07 (OQ-009: "it can be atem and direct video from camera"). The scope is recorded in the requirements (HDMI capture of the ATEM output and of cameras connected directly), not in an ADR; no ADR has been written for it. Which ATEM and camera models must be supported is still open (OQ-102).

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
| REQ-CAP-006 | RISK-014 | OQ-004, OQ-025, OQ-054, OQ-063 |
| REQ-CAP-007 | RISK-001, RISK-006, RISK-011 | OQ-002, OQ-011, OQ-021, OQ-038, OQ-040, OQ-099 |
| REQ-CAP-008 | RISK-008, RISK-009 | OQ-102, OQ-002, OQ-028, OQ-078, OQ-083 |
| REQ-DMA-001 | RISK-020 | OQ-058, OQ-060, OQ-062 |
| REQ-ENC-001 | RISK-002, RISK-003, RISK-015 | OQ-005, OQ-048, OQ-056, OQ-057, OQ-059, OQ-096 |
| REQ-REC-001 | none registered | OQ-006, OQ-069 |
| REQ-STR-001 | RISK-003 | OQ-007, OQ-063, OQ-075 |
| REQ-STR-002 | RISK-003, RISK-019 | OQ-008, OQ-073, OQ-074 |
| REQ-ATEM-001 | RISK-008 (RISK-018 applies only to network control, not in current scope) | OQ-078, OQ-083, OQ-102 (OQ-009 ANSWERED 2026-10-07; OQ-077 and OQ-084 apply only to network control, not in current scope) |
| REQ-BLD-001 | RISK-015, RISK-017 | OQ-067, OQ-068, OQ-071 (OQ-012 ANSWERED 2026-10-07) |
| REQ-BLD-002 | RISK-017 | OQ-064, OQ-067, OQ-071 (OQ-012 ANSWERED 2026-10-07) |
| REQ-PERF-001 | RISK-003, RISK-006, RISK-020 | OQ-010, OQ-035, OQ-053, OQ-059, OQ-061 |

RISK-016 (RGB888 pixel-format label differs between receivers) affects ADR-005, so it is listed under REQ-CAP-001, whose pixel format ADR-005 proposes. RISK-001 is registered against REQ-CAP-001; it is also listed under REQ-CAP-007 because it sets the 2-lane configuration's limit. RISK-018 is kept against REQ-ATEM-001 only as a reference: it concerns the UDP 9910 network option, which the owner did not select on 2026-10-07 (OQ-009). RISK-004 (TC358743 supply and lifecycle) affects the product as a whole, not one requirement, so it appears in no row; it is tracked through OQ-085.

## Rule 12 chain for REQ-CAP-001

This is the Rule 12 example chain, adapted to the real state on 2026-10-06 and updated for the owner decisions of 2026-10-07. Each stage shows its status. Facts are cited from [REFERENCES.md](REFERENCES.md).

```text
REQ-CAP-001 — capture HDMI input through the TC358743 at 1920x1080@60Hz
    Acceptance: DRAFT. OQ-001 answered 2026-10-07: 1080p60 is required on the 4-lane
    configuration; 2-lane configurations capture every rate their link carries (REQ-CAP-007).
    60 Hz vs 59.94 Hz, 50 Hz, pixel format and audio scope are owner decisions
    (OQ-002, OQ-003, OQ-004, OQ-017).
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
- **Encode** (outside REQ-CAP-001, but needed for a product). The Pi 4/CM4 hardware encoder is officially specified for 1080p30 [D-10] (RISK-002). Pi 5/CM5 have no hardware video encoder [D-31], [G-22] (RISK-003).

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
| TEST-ENC-001 | Sustained real-time H.264 encode | REQ-ENC-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-REC-001 | Recording integrity and duration | REQ-REC-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-STR-001 | RTMP publish and playback | REQ-STR-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-STR-002 | WebRTC browser playback | REQ-STR-002 | BLOCKED — HARDWARE REQUIRED |
| TEST-ATEM-001 | ATEM HDMI output capture (scope per OQ-009) | REQ-ATEM-001, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-BLD-001 | Image build from clean checkout | REQ-BLD-001, REQ-BLD-002 | NOT STARTED |
| TEST-PERF-001 | Soak: thermal, CPU, CMA, frame drops | REQ-PERF-001 | BLOCKED — HARDWARE REQUIRED |

## Coverage checks

Checked on 2026-10-06 against [REQUIREMENTS.md](REQUIREMENTS.md) (17 requirements) and the canonical test table in [README.md](README.md) (17 tests). Rechecked on 2026-10-07 after the owner decisions: 20 requirements (REQ-CAP-007, REQ-CAP-008 and REQ-BLD-002 added) and the same 17 tests, whose "Verifies" lists were extended in [README.md](README.md).

### Requirements without tests

None. Every requirement has at least one canonical test:

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

- **Partial coverage of REQ-ARCH-001.** TEST-PLT-001 covers HDMI → TC358743 → CSI-2 → receiver → Media Controller → V4L2. TEST-DMA-001 covers V4L2 → DMABUF → encoder. The "Recorder / RTMP / WebRTC" end of the chain is exercised only through TEST-REC-001, TEST-STR-001 and TEST-STR-002, which are mapped to their own requirements, not to REQ-ARCH-001.
- **No pass criteria yet.** No test can be recorded as `TESTED — PASS` against an accepted criterion until the owner accepts acceptance criteria (OQ-017). [TESTING.md](TESTING.md) lists source-predicted results only.
- **Platform-dependent tests.** TEST-CAP-002's 1080p60 run cannot be done on Pi 4 Model B or on CM4 CAM0, which are 2-lane [C-01], [C-02], [C-37]. These platforms remain candidates for the 2-lane configuration of REQ-CAP-007 (owner, 2026-10-07); that configuration is covered only by the TEST-CAP-004 supported-mode matrix, at the rates a 2-lane link carries [C-48]. TEST-AUD-001 applies only if the owner requires audio (OQ-004).
- **Source coverage (REQ-CAP-008).** TEST-ATEM-001 covers the ATEM source only, and only HDMI capture of its output; its network-control and RTMP parts are not in current scope (OQ-009 answered 2026-10-07). Cameras are covered by TEST-CAP-001 and TEST-CAP-004 only. Both depend on the model list, which is open (OQ-102).
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
| D — Encoders | D-10, D-31 |
| G — Raspberry Pi OS and image tooling | G-22 |

- `CORRECTED` entries cited, used in their corrected wording: A-22, B-11, B-21.
- `community` entries cited, worded as reports: C-33, C-43.
- `reasoning` entries cited, labelled as reasoning: B-11, B-47, C-47, C-48, C-49.
- The ID mappings were checked against [REQUIREMENTS.md](REQUIREMENTS.md), [README.md](README.md), [DECISIONS.md](DECISIONS.md), [RISKS.md](RISKS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) as they stood on 2026-10-06, and again on 2026-10-07 against the owner-decision entries in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) and [README.md](README.md).

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
