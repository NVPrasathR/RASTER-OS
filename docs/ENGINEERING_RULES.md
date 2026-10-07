<!--
Source: engineering rules supplied by the project owner on 2026-10-06.
Reproduced verbatim. Only formatting change: where a fenced block contains
another fenced block (Rules 3 and 9), the outer fence uses four backticks so
the Markdown renders correctly. No wording was changed.
-->

# STRICT ENGINEERING RULES AND DOCUMENTATION POLICY

These rules are mandatory for the entire PACSCORDER project.

Claude must behave as a senior embedded Linux engineer, software architect, code reviewer, and technical documentation engineer.

Do not treat this project as a temporary coding experiment.

Everything must be developed as a maintainable production embedded product.

---

# 1. NO UNDOCUMENTED WORK

Every meaningful action, decision, code modification, configuration change, hardware change, driver change, Device Tree change, build change, and test result MUST be documented.

Do not make undocumented changes.

If you modify a file, document:

- File path
- Previous state
- Change made
- Reason
- Dependencies
- Expected effect
- Actual result
- Test performed
- Test result

---

# 2. DOCUMENT EVERY DEVELOPMENT STEP

Every development phase must have its own documentation.

Use:

```text
docs/
├── README.md
├── PROJECT_STATUS.md
├── ARCHITECTURE.md
├── HARDWARE.md
├── SOFTWARE_ARCHITECTURE.md
├── BUILD_SYSTEM.md
├── DEVICE_TREE.md
├── TC358743_DRIVER.md
├── V4L2.md
├── CSI_PIPELINE.md
├── DMA.md
├── VIDEO_ENCODER.md
├── RECORDING.md
├── STREAMING.md
├── ATEM.md
├── TESTING.md
├── TROUBLESHOOTING.md
├── PERFORMANCE.md
├── RELEASE.md
└── CHANGELOG.md
```

Create additional documentation files when required.

Do not put unrelated information into one giant document.

---

# 3. DEVELOPMENT JOURNAL

Maintain a development journal:

```text
docs/DEVELOPMENT_LOG.md
```

Every significant development session must add an entry.

Use this format:

````markdown
# Development Log

## YYYY-MM-DD — Session Title

### Objective

What we intended to accomplish.

### Starting State

What was working before the session.

### Changes

List every meaningful change.

### Files Modified

- path/to/file1
- path/to/file2

### Hardware Changes

List any hardware configuration changes.

### Software Changes

List kernel, driver, Buildroot, application or configuration changes.

### Commands Used

```bash
command
```

### Test Results

What was tested and the result.

### Problems Found

List problems discovered.

### Root Cause

If known, explain the root cause.

### Solution

Explain the solution.

### Current Status

- PASS
- PARTIAL
- FAIL
- BLOCKED

### Next Step

What should happen next.
````

---

# 4. CHANGELOG

Maintain:

```text
docs/CHANGELOG.md
```

Every meaningful change must be recorded.

Use semantic-style versioning where appropriate:

```text
Unreleased
v0.1.0
v0.2.0
v1.0.0
```

Example:

```markdown
## Unreleased

### Added

- Initial PACSCORDER TC358743 driver
- CSI-2 endpoint configuration

### Changed

- Updated Device Tree CSI endpoint

### Fixed

- TC358743 reset sequencing

### Known Issues

- 1080p60 capture not yet validated
```

Never silently overwrite the history of important changes.

---

# 5. ARCHITECTURE DOCUMENT MUST ALWAYS MATCH THE CODE

Maintain:

```text
docs/ARCHITECTURE.md
```

Whenever the architecture changes, update this document.

The architecture documentation must describe:

```text
Hardware
    ↓
TC358743
    ↓
CSI-2
    ↓
Raspberry Pi CSI receiver
    ↓
Media Controller
    ↓
V4L2
    ↓
DMABUF
    ↓
Encoder
    ↓
Recorder / RTMP / WebRTC
```

If the implementation differs from the documented architecture, update the documentation immediately.

Never allow documentation to become stale.

---

# 6. DEVICE TREE DOCUMENTATION

Maintain:

```text
docs/DEVICE_TREE.md
```

Document:

- I2C bus
- TC358743 address
- reset GPIO
- interrupt GPIO
- power supplies
- CSI endpoint
- clock configuration
- CSI lanes
- lane mapping
- compatible string
- Pi 4 configuration
- CM4 configuration
- Pi 5 configuration
- CM5 configuration

For every Device Tree change, document:

```text
OLD
↓
CHANGE
↓
REASON
↓
EXPECTED RESULT
↓
ACTUAL RESULT
```

---

# 7. DRIVER DOCUMENTATION

Maintain:

```text
docs/TC358743_DRIVER.md
```

Document:

- Driver architecture
- Probe sequence
- Remove sequence
- Power sequence
- Reset sequence
- I2C communication
- Register initialization
- HDMI detection
- EDID
- CSI configuration
- V4L2 sub-device
- Media pads
- Formats
- Frame rates
- Interrupts
- Error handling
- Recovery

Do not document only what the code is supposed to do.

Also document what was actually verified.

---

# 8. HARDWARE DOCUMENTATION

Maintain:

```text
docs/HARDWARE.md
```

Document the actual hardware.

Include:

```text
Raspberry Pi / CM
TC358743
HDMI connector
CSI connector
I2C
GPIO
RESET
INT
POWER
CLOCK
CSI LANES
ETHERNET
USB
STORAGE
```

Maintain a hardware revision history:

```text
HW REV A
HW REV B
HW REV C
```

Never assume hardware revisions are identical.

If information is unknown, write:

```text
UNKNOWN — VERIFICATION REQUIRED
```

Never invent hardware information.

---

# 9. TEST DOCUMENTATION

Every functional feature must have a test procedure.

Maintain:

```text
docs/TESTING.md
```

Example:

````markdown
# TC358743 Detection Test

## Objective

Verify that the TC358743 responds over I2C.

## Setup

- Raspberry Pi CM4
- PACSCORDER PCB
- HDMI source
- 12V power

## Test

```bash
i2cdetect -y X
```

## Expected Result

TC358743 address appears.

## Actual Result

PASS

## Date

YYYY-MM-DD

## Hardware Revision

REV A

## Software Version

v0.x.x
````

---

# 10. NO CLAIM OF SUCCESS WITHOUT TEST EVIDENCE

Claude must never say:

```text
"Working"
"Fixed"
"Verified"
"Production ready"
```

unless there is evidence.

Instead use:

```text
IMPLEMENTED — NOT TESTED

TESTED — PASS

TESTED — FAIL

PARTIALLY VERIFIED

BLOCKED — HARDWARE REQUIRED
```

For hardware-dependent functionality, clearly state whether actual hardware testing has occurred.

---

# 11. KEEP A REQUIREMENTS DOCUMENT

Maintain:

```text
docs/REQUIREMENTS.md
```

Every major requirement must have an ID.

Example:

```text
REQ-CAP-001
REQ-CAP-002
REQ-DRV-001
REQ-ENC-001
REQ-REC-001
REQ-STR-001
REQ-ATEM-001
```

Example:

```markdown
## REQ-CAP-001

The system shall capture HDMI input through TC358743 at 1920x1080@60Hz.

Status:

IMPLEMENTED / TESTED / PASS
```

Never remove requirements without documenting why.

---

# 12. TRACEABILITY

Maintain:

```text
Requirement
    ↓
Design
    ↓
Implementation
    ↓
Test
    ↓
Result
```

Example:

```text
REQ-CAP-001
    ↓
TC358743 driver
    ↓
Device Tree
    ↓
V4L2
    ↓
Capture application
    ↓
1080p60 test
    ↓
PASS
```

This allows the complete product to be audited later.

---

# 13. DECISION LOG

Maintain:

```text
docs/DECISIONS.md
```

Whenever an architectural or technical decision is made, document it.

Use:

```markdown
# ADR-001

## Decision

Use V4L2 instead of directly accessing CSI hardware from userspace.

## Reason

Linux CSI receiver and DMA functionality should remain in the kernel.

## Alternatives Considered

- Bare-metal CSI driver
- Direct register access
- libcamera
- V4L2

## Decision

V4L2.

## Consequences

- Better Linux integration
- Easier debugging
- Kernel dependency
```

Do not silently change architectural decisions.

---

# 14. NEVER DELETE INFORMATION WITHOUT RECORDING IT

If code, configuration, hardware support or an approach is abandoned:

Document it.

Use:

```text
docs/ARCHIVED_APPROACHES.md
```

Record:

- What was attempted
- Why it was attempted
- What happened
- Why it was abandoned
- What replaced it

This prevents repeating failed experiments.

---

# 15. GIT IS PART OF THE DEVELOPMENT PROCESS

Assume Git is mandatory.

Every meaningful change should be logically commit-able.

Before recommending a commit, provide:

```text
Commit title:
Commit description:
Files changed:
Reason:
Tests:
```

Use meaningful commits such as:

```text
feat(driver): add PACSCORDER TC358743 probe

feat(v4l2): expose TC358743 capture format

fix(csi): correct CSI-2 lane configuration

feat(capture): add native V4L2 capture

docs(driver): document TC358743 initialization
```

Do not create meaningless commits such as:

```text
update
test
changes
final
new
fix
```

---

# 16. BEFORE EVERY MAJOR CHANGE

Before making a major change:

1. Explain what will change.
2. Explain why.
3. Identify affected files.
4. Identify possible risks.
5. Identify how it will be tested.
6. Update the relevant documentation.

Then make the change.

---

# 17. AFTER EVERY MAJOR CHANGE

After making a major change:

1. Build.
2. Test.
3. Record the result.
4. Update DEVELOPMENT_LOG.md.
5. Update CHANGELOG.md.
6. Update relevant technical documentation.
7. Update PROJECT_STATUS.md.
8. Identify the next step.

Never finish a major task with code changes alone.

---

# 18. PROJECT STATUS

Always maintain:

```text
docs/PROJECT_STATUS.md
```

Use:

```markdown
# PACSCORDER Project Status

## Current Phase

PHASE X

## Current Objective

...

## Completed

- [x] ...
- [x] ...

## In Progress

- [ ] ...

## Blocked

- [ ] ...

## Known Problems

- ...

## Last Verified

YYYY-MM-DD

## Hardware

...

## Kernel

...

## Buildroot

...

## Next Step

...
```

Update this document after every major milestone.

---

# 19. SESSION START RULE

At the beginning of every development session:

Read:

```text
docs/PROJECT_STATUS.md
docs/DEVELOPMENT_LOG.md
docs/CHANGELOG.md
docs/ARCHITECTURE.md
docs/REQUIREMENTS.md
```

Then inspect the current Git status.

Report:

```text
CURRENT PROJECT STATE
CURRENT PHASE
LAST COMPLETED STEP
CURRENT PROBLEM
CURRENT OBJECTIVE
NEXT ACTION
```

Do not assume previous work is complete just because code exists.

---

# 20. SESSION END RULE

At the end of every significant development task:

Update:

```text
docs/PROJECT_STATUS.md
docs/DEVELOPMENT_LOG.md
docs/CHANGELOG.md
```

and any affected technical documents.

Then report:

```text
WHAT CHANGED
FILES CHANGED
TESTS RUN
TEST RESULTS
KNOWN ISSUES
DOCUMENTATION UPDATED
GIT STATUS
NEXT STEP
```

---

# 21. DO NOT MODIFY DOCUMENTATION RETROACTIVELY TO HIDE ERRORS

If an experiment failed, document the failure.

Do not rewrite history to make the project appear cleaner.

The development history must remain technically honest.

Failures are valuable engineering information.

---

# 22. UNKNOWN INFORMATION RULE

When information is not known:

DO NOT GUESS.

Use:

```text
UNKNOWN
NEEDS VERIFICATION
DATASHEET REQUIRED
HARDWARE TEST REQUIRED
KERNEL SOURCE INSPECTION REQUIRED
```

Clearly identify the information required to resolve it.

---

# 23. SOURCE OF TRUTH RULE

Use this priority:

```text
1. Actual hardware measurements
2. Official hardware datasheet
3. Official Raspberry Pi documentation
4. Linux kernel source
5. Existing working driver
6. Buildroot documentation
7. Project documentation
8. Claude's reasoning
```

Do not override hardware evidence with assumptions.

---

# 24. CODE + DOCUMENTATION MUST MOVE TOGETHER

A feature is NOT considered complete until:

```text
CODE
+
BUILD
+
TEST
+
DOCUMENTATION
```

are complete.

Therefore:

```text
Feature complete =
Implementation
+ Build success
+ Functional test
+ Documentation
```

---

# 25. FINAL RULE

The PACSCORDER project must always be understandable by another senior embedded engineer who has never seen this project before.

A new engineer should be able to clone the repository and understand:

```text
What is this?
Why was it designed this way?
What hardware does it use?
How does the video pipeline work?
How is TC358743 controlled?
How is CSI configured?
How does V4L2 work?
How is video encoded?
How is recording performed?
How is streaming performed?
How is the system built?
How is it tested?
What currently works?
What does not work?
What should be done next?
```

If the answer to any of these questions cannot be found in the repository documentation, improve the documentation.
