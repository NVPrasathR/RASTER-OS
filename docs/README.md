# PACSCORDER Documentation

| | |
|---|---|
| Document status | Active |
| Last updated | 2026-10-07 |
| Project phase | PHASE 0 — Bootstrap (see [PROJECT_STATUS.md](PROJECT_STATUS.md)) |

PACSCORDER is an embedded Linux product that captures HDMI video through a Toshiba **TC358743** HDMI-to-MIPI-CSI-2 bridge into a **Raspberry Pi**. It then encodes the video for recording, **RTMP** streaming and **WebRTC** streaming, and integrates with **Blackmagic ATEM** switchers. The mandated video path is:

```text
HDMI source → TC358743 → CSI-2 → Raspberry Pi CSI-2 receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder / RTMP / WebRTC
```

> **Current state (2026-10-06):**
> - No hardware exists.
> - No code exists.
> - Nothing has been tested.
>
> The documentation records verified *source* research, requirements and proposed decisions. Every hardware fact about PACSCORDER itself is `UNKNOWN — VERIFICATION REQUIRED`.
>
> The repository directory is named "RASTER OS"; that is only a folder name (owner, 2026-10-06). The product is PACSCORDER.

## Start here (Rule 25 questions)

| A new engineer asks… | Read |
|---|---|
| What is this? | This page, [ARCHITECTURE.md](ARCHITECTURE.md) |
| Why was it designed this way? | [DECISIONS.md](DECISIONS.md), [REQUIREMENTS.md](REQUIREMENTS.md) |
| What hardware does it use? | [HARDWARE.md](HARDWARE.md) |
| How does the video pipeline work? | [ARCHITECTURE.md](ARCHITECTURE.md), [CSI_PIPELINE.md](CSI_PIPELINE.md), [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) |
| How is TC358743 controlled? | [TC358743_DRIVER.md](TC358743_DRIVER.md), [DEVICE_TREE.md](DEVICE_TREE.md), [V4L2.md](V4L2.md) (EDID and DV timings from userspace), [HARDWARE.md](HARDWARE.md) (I2C, reset, interrupt, clock wiring) |
| How is CSI configured? | [CSI_PIPELINE.md](CSI_PIPELINE.md), [DEVICE_TREE.md](DEVICE_TREE.md) |
| How does V4L2 work? | [V4L2.md](V4L2.md), [DMA.md](DMA.md) |
| How is video encoded? | [VIDEO_ENCODER.md](VIDEO_ENCODER.md), [DMA.md](DMA.md) (buffer sharing with the encoder), [PERFORMANCE.md](PERFORMANCE.md) (encode budgets) |
| How is recording performed? | [RECORDING.md](RECORDING.md) |
| How is streaming performed? | [STREAMING.md](STREAMING.md), [ATEM.md](ATEM.md) |
| How is the system built? | [BUILD_SYSTEM.md](BUILD_SYSTEM.md), [RELEASE.md](RELEASE.md) |
| How is it tested? | [TESTING.md](TESTING.md), [TRACEABILITY.md](TRACEABILITY.md), [PERFORMANCE.md](PERFORMANCE.md) (measurement procedures) |
| What currently works? | [PROJECT_STATUS.md](PROJECT_STATUS.md), [TESTING.md](TESTING.md) (test status summary; no test has been run), [TRACEABILITY.md](TRACEABILITY.md) |
| What does not work? | [PROJECT_STATUS.md](PROJECT_STATUS.md), [RISKS.md](RISKS.md), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md), [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |
| What should be done next? | [PROJECT_STATUS.md](PROJECT_STATUS.md), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) (owner decisions first), [TESTING.md](TESTING.md) (test order) |

## Document index

**Process and project state**

| File | Purpose |
|---|---|
| [ENGINEERING_RULES.md](ENGINEERING_RULES.md) | The owner's mandatory engineering and documentation rules (verbatim). |
| [PROJECT_STATUS.md](PROJECT_STATUS.md) | Current phase, what is done, blocked, next step (Rule 18). |
| [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) | Session-by-session journal (Rule 3). |
| [CHANGELOG.md](CHANGELOG.md) | Versioned list of changes (Rule 4). |
| [REQUIREMENTS.md](REQUIREMENTS.md) | Requirements with IDs `REQ-<AREA>-NNN` (Rule 11). |
| [TRACEABILITY.md](TRACEABILITY.md) | Requirement → design → implementation → test → result (Rule 12). |
| [DECISIONS.md](DECISIONS.md) | Architecture decision records `ADR-NNN` (Rule 13). |
| [ARCHIVED_APPROACHES.md](ARCHIVED_APPROACHES.md) | Approaches tried and abandoned (Rule 14). |
| [RISKS.md](RISKS.md) | Technical risk register `RISK-NNN`. |
| [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) | Unknowns and owner decisions `OQ-NNN`, with how to resolve each (Rule 22). |
| [REFERENCES.md](REFERENCES.md) | Source register: every cited fact, with source, tier and verification verdict (Rule 23). |

**Technical**

| File | Purpose |
|---|---|
| [ARCHITECTURE.md](ARCHITECTURE.md) | System architecture, end to end (Rule 5). |
| [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) | Userspace software components and their interfaces. |
| [HARDWARE.md](HARDWARE.md) | Actual hardware and revision history (Rule 8). |
| [DEVICE_TREE.md](DEVICE_TREE.md) | Device Tree / overlay configuration per platform (Rule 6). |
| [TC358743_DRIVER.md](TC358743_DRIVER.md) | The TC358743 Linux driver (Rule 7). |
| [V4L2.md](V4L2.md) | V4L2 / Media Controller usage. |
| [CSI_PIPELINE.md](CSI_PIPELINE.md) | CSI-2 link, lanes, bandwidth, receivers. |
| [DMA.md](DMA.md) | Buffers, DMABUF, CMA. |
| [VIDEO_ENCODER.md](VIDEO_ENCODER.md) | Encoding per platform. |
| [RECORDING.md](RECORDING.md) | Recording. |
| [STREAMING.md](STREAMING.md) | RTMP and WebRTC. |
| [ATEM.md](ATEM.md) | Blackmagic ATEM integration. |
| [BUILD_SYSTEM.md](BUILD_SYSTEM.md) | How the image is built. |
| [RELEASE.md](RELEASE.md) | Versioning and release procedure. |
| [TESTING.md](TESTING.md) | Test procedures and results (Rule 9). |
| [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Known failure signatures and diagnosis. |
| [PERFORMANCE.md](PERFORMANCE.md) | Bandwidth, CPU, latency budgets and measurements. |

## Documentation conventions

These conventions apply to every file in `docs/`.

### 1. Citations

- Every technical fact cites the source register: `[C-37]` means entry C-37 in [REFERENCES.md](REFERENCES.md).
- Only facts with verdict `CONFIRMED` or `CORRECTED` may be stated as fact. For `CORRECTED` entries, use the corrected wording.
- A fact whose verdict is `UNVERIFIABLE` is written as `NEEDS VERIFICATION`.
- A `community` tier fact is always identified as such ("reported by…").
- Calculations are shown with their inputs and marked as reasoning.

### 2. Unknowns

Never guess (Rules 8, 22). Write `UNKNOWN — VERIFICATION REQUIRED` and name what resolves it, using one of these markers:

- `DATASHEET REQUIRED`
- `HARDWARE TEST REQUIRED`
- `KERNEL SOURCE INSPECTION REQUIRED`
- `VENDOR CONFIRMATION REQUIRED`
- `OWNER DECISION REQUIRED`
- `LEGAL CLARIFICATION REQUIRED`
- `BUILD TEST REQUIRED`

Link the matching `OQ-NNN` where one exists.

### 3. Status words (Rule 10)

Implementation and test status use only these:

- `NOT STARTED`
- `IMPLEMENTED — NOT TESTED`
- `TESTED — PASS`
- `TESTED — FAIL`
- `PARTIALLY VERIFIED`
- `BLOCKED — HARDWARE REQUIRED`

The words "working", "fixed", "verified" and "production ready" are not used without test evidence.

### 4. Commands

Commands shown in procedures are taken from cited sources. Until a command has been run on PACSCORDER hardware, it is marked **NOT YET RUN ON PACSCORDER HARDWARE**. Results are recorded in [TESTING.md](TESTING.md).

### 5. Platforms

Where behaviour differs, every technical document covers all four platforms: Pi 4 Model B, CM4, Pi 5, CM5.

### 6. Verification section

Every technical document ends with a section that separates **what is verified from sources** and **what has been verified on PACSCORDER hardware** (Rule 7).

### 7. Change history

Every document ends with a change-history table. Changes are added, never rewritten (Rule 21).

### 8. ID schemes

| Prefix | Meaning | Defined in |
|---|---|---|
| `REQ-<AREA>-NNN` | Requirement | [REQUIREMENTS.md](REQUIREMENTS.md) |
| `ADR-NNN` | Decision | [DECISIONS.md](DECISIONS.md) |
| `RISK-NNN` | Risk | [RISKS.md](RISKS.md) |
| `OQ-NNN` | Open question / owner decision | [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) |
| `TEST-<AREA>-NNN` | Test procedure | [TESTING.md](TESTING.md) |
| `A-NN` … `G-NN` | Source fact | [REFERENCES.md](REFERENCES.md) |

### 9. Canonical test IDs

Other documents must use exactly these IDs:

| Test ID | Title | Verifies |
|---|---|---|
| TEST-HW-001 | TC358743 I2C detection | REQ-DRV-001 |
| TEST-DRV-001 | TC358743 driver probe and chip ID | REQ-DRV-001 |
| TEST-DRV-002 | EDID load and HDMI hot-plug assertion | REQ-CAP-003 |
| TEST-PLT-001 | Overlay load and media graph per platform | REQ-PLT-001, REQ-ARCH-001 |
| TEST-CAP-001 | DV timings detection for each source mode | REQ-CAP-002, REQ-CAP-008 |
| TEST-CAP-002 | 1080p60 capture: frame rate and frame integrity | REQ-CAP-001, REQ-CAP-007 |
| TEST-CAP-003 | Source connect / disconnect / mode change | REQ-CAP-004 |
| TEST-CAP-004 | Unsupported-mode rejection and supported-mode matrix | REQ-CAP-005, REQ-CAP-007, REQ-CAP-008 |
| TEST-AUD-001 | HDMI audio capture over I2S | REQ-CAP-006 |
| TEST-DMA-001 | DMABUF capture → encoder buffer sharing | REQ-DMA-001, REQ-ARCH-001 |
| TEST-ENC-001 | Sustained real-time H.264 encode | REQ-ENC-001 |
| TEST-REC-001 | Recording integrity and duration | REQ-REC-001 |
| TEST-STR-001 | RTMP publish and playback | REQ-STR-001 |
| TEST-STR-002 | WebRTC browser playback | REQ-STR-002 |
| TEST-ATEM-001 | ATEM HDMI output capture (scope per OQ-009) | REQ-ATEM-001, REQ-CAP-008 |
| TEST-BLD-001 | Image build from clean checkout | REQ-BLD-001, REQ-BLD-002 |
| TEST-PERF-001 | Soak: thermal, CPU, CMA, frame drops | REQ-PERF-001 |

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Documentation index and conventions created. | Claude (session 2026-10-06) |
| 2026-10-06 | Convention 2: added `LEGAL CLARIFICATION REQUIRED` and `BUILD TEST REQUIRED`, which OPEN_QUESTIONS.md, RISKS.md and several technical documents already used (found by the per-document reviews). | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: Start-here table (Rule 25) links added — V4L2.md and HARDWARE.md for TC358743 control; DMA.md and PERFORMANCE.md for encoding; PERFORMANCE.md for testing; TESTING.md and TRACEABILITY.md for current status; OPEN_QUESTIONS.md for what does not work; TESTING.md test order for next steps. | Claude (session 2026-10-06) |
| 2026-10-07 | Canonical test table: "Verifies" extended for the new requirements REQ-CAP-007, REQ-CAP-008 and REQ-BLD-002 (owner decisions of 2026-10-07). No test ID added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)" after OQ-009 was answered; ID unchanged. | Claude (session 2026-10-07) |
