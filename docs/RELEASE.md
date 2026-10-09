# PACSCORDER Release Process

| | |
|---|---|
| Document status | DRAFT. **No release exists.** The current version is `Unreleased`. The release procedure below is PROPOSED and has never been executed. |
| Last updated | 2026-10-09 |
| Applies to | Every PACSCORDER image — the project's own OS image (REQ-BLD-002, DRAFT, owner 2026-10-07) — for Raspberry Pi 4 Model B, CM4, Raspberry Pi 5 and CM5. Both a 2-lane and a 4-lane capture configuration are required (REQ-CAP-007); the platform for each is undecided (ADR-004 OPEN), and bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07, second answer). The build tool is `rpi-image-gen` under ADR-003 (ACCEPTED 2026-10-07); the process also applies if the documented Buildroot alternative is ever adopted. Since the owner decisions of 2026-10-07 and 2026-10-08 a release ships H.264 encoding only (REQ-ENC-001; owner, 2026-10-08: "H.264 only for now", answer to OQ-103) and HDMI audio (REQ-CAP-006), which add the licence items in [§4](#4-licence-and-compliance). H.265 is deferred (REQ-ENC-002, DEFERRED); its licence items stay in §4, labelled as not in current scope, except x265's own licence, which still applies because the Raspberry Pi FFmpeg depends on `libx265-215` [H-09]. |
| Verification | Source research of 2026-10-06 and of 2026-10-08 (topic H, H.265/HEVC; topic I, HDMI audio; topic J, recording storage and power loss; topic K, live latency) ([REFERENCES.md](REFERENCES.md)). Nothing has been built, released or tested on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Traceability | REQ-BLD-001 · REQ-BLD-002 · REQ-CAP-007 · ADR-003 (ACCEPTED) · RISK-015 · RISK-017 · TEST-BLD-001 · OQ-069, OQ-071, OQ-086, OQ-087, OQ-088, OQ-094, OQ-101. Added 2026-10-08: REQ-ENC-001 (H.264 only since 2026-10-08) · REQ-ENC-002 (H.265, DEFERRED) · REQ-CAP-006 · RISK-022 (not in current scope) · OQ-103 (ANSWERED 2026-10-08), OQ-109 (not in current scope), OQ-113. Added 2026-10-09: ADR-009 (ACCEPTED 2026-10-08) · REQ-REC-001 · RISK-027, RISK-034 · OQ-120, OQ-121, OQ-122, OQ-123, OQ-124 · TEST-REC-001 |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 4, 9, 10, 12, 15, 17, 18, 20, 21, 24 |

This document defines:

- how PACSCORDER versions are named;
- when a feature counts as done;
- what must be true before a version is released;
- the licence, SBOM, update and provisioning facts a release depends on.

How the image itself is built is in [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

Fact references such as `[G-69]` point to [REFERENCES.md](REFERENCES.md). For `CORRECTED` entries only the corrected wording is used, and the verdict is noted next to the citation. Text marked *research gap* comes from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json) or, for topics H and I, [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json), or, for topics J and K (added 2026-10-09), [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json); it is **not** a register fact.

*(Added 2026-10-09; research topic J / ADR-009.)* The owner decided on 2026-10-08 that every recording is mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure (ADR-009, ACCEPTED). That adds release items outside the OS image: which storage parts each release was tested with ([§3.1](#31-scope-and-decisions), [§9](#9-release-record-template-proposed)), how the recording volumes are prepared ([§7](#7-provisioning)), and, if NVMe boot is chosen, the EEPROM boot order ([§8](#8-eeprom-bootloader-pinning)).

## Contents

- [1. Versioning](#1-versioning)
- [2. Definition of done (Rule 24)](#2-definition-of-done-rule-24)
- [3. Release checklist](#3-release-checklist)
- [4. Licence and compliance](#4-licence-and-compliance)
- [5. SBOM](#5-sbom)
- [6. Field update and A/B mechanisms (OPEN design options)](#6-field-update-and-ab-mechanisms-open-design-options)
- [7. Provisioning](#7-provisioning)
- [8. EEPROM bootloader pinning](#8-eeprom-bootloader-pinning)
- [9. Release record template (PROPOSED)](#9-release-record-template-proposed)
- [10. Release history](#10-release-history)
- [Verification status](#verification-status)
- [Change history](#change-history)

---

## 1. Versioning

### 1.1 Version names (Rule 4)

Rule 4 asks for a maintained [CHANGELOG.md](CHANGELOG.md) and "semantic-style versioning where appropriate", with these example names:

```text
Unreleased
v0.1.0
v0.2.0
v1.0.0
```

**Current version: `Unreleased`.** No version has been tagged.

### 1.2 Meaning of the numbers (PROPOSED)

PROPOSED, and OWNER DECISION REQUIRED. The rules say only "semantic-style versioning where appropriate". This interpretation is Claude's proposal:

| Change | Version step | Example |
|---|---|---|
| Development release before the product is accepted for field use | `v0.MINOR.PATCH` | `v0.1.0` |
| A release that adds a capability (for example a new requirement reaching TESTED — PASS) | MINOR + 1 | `v0.1.0` → `v0.2.0` |
| A release that only corrects defects in already-released capability | PATCH + 1 | `v0.2.0` → `v0.2.1` |
| First release the owner accepts for field use | `v1.0.0` | — |
| A change that breaks field compatibility (partition layout, update format, configuration format) after `v1.0.0` | MAJOR + 1 | `v1.4.2` → `v2.0.0` |

What any particular version (for example `v0.1.0`) must contain is **not defined**; no milestone plan exists (OWNER DECISION REQUIRED).

### 1.3 What a version identifies (PROPOSED)

A PACSCORDER version identifies **one complete image** — the project-built OS image required by REQ-BLD-002 (DRAFT, owner 2026-10-07), never an unmodified stock image — plus the record of everything that is not inside the image. Reasoning: the EEPROM bootloader is changed by separate tools (`rpi-eeprom`, `rpi-update`) [G-06], [G-09], not by writing the OS image to storage. Whether a booted image updates it automatically is NEEDS VERIFICATION ([§8](#8-eeprom-bootloader-pinning)). A Git tag `vX.Y.Z` (Git per Rule 15) marks the commit the image was built from.

- **Hardware revisions** (`HW REV A`, `HW REV B`, …; Rule 8) are versioned separately. Each release states which hardware revisions it was tested on, and must never assume that revisions are identical (Rule 8). PROPOSED: because both a 2-lane and a 4-lane capture configuration are required (REQ-CAP-007), each release also states which configuration each tested unit used.
- **History is never rewritten** (Rule 21). A release found to be defective is marked as such in [§10](#10-release-history) and in [CHANGELOG.md](CHANGELOG.md); it is not deleted or re-tagged.

---

## 2. Definition of done (Rule 24)

Rule 24:

```text
Feature complete =
Implementation
+ Build success
+ Functional test
+ Documentation
```

| Element | What counts as evidence (PROPOSED interpretation) | Where it is recorded | Status word while incomplete (Rule 10) |
|---|---|---|---|
| Implementation | Code, configuration or Device Tree change committed to Git with a Rule 15 commit message | Git; [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) | `NOT STARTED` |
| Build success | The image builds from a clean checkout: TEST-BLD-001 | [TESTING.md](TESTING.md) | `IMPLEMENTED — NOT TESTED` |
| Functional test | The canonical TEST ID for the requirement ([README.md](README.md) §9) is `TESTED — PASS` on named hardware, with the Rule 9 fields recorded: setup, command, expected result, actual result, date, hardware revision, software version | [TESTING.md](TESTING.md) | `IMPLEMENTED — NOT TESTED`, `TESTED — FAIL`, `PARTIALLY VERIFIED` or `BLOCKED — HARDWARE REQUIRED` |
| Documentation | The technical document is updated and states what was actually tested (Rule 7). The requirement's row in [TRACEABILITY.md](TRACEABILITY.md) shows requirement → design → implementation → test → result (Rule 12). [CHANGELOG.md](CHANGELOG.md), [PROJECT_STATUS.md](PROJECT_STATUS.md) and [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) are updated (Rules 17, 20). | Those files | — |

Rules applied in this document (the first is a PROPOSED interpretation; OWNER DECISION REQUIRED):

- A feature that is `PARTIALLY VERIFIED` is **not done**. PROPOSED: a release may still contain it, but only if it is listed under *Known Issues* in [CHANGELOG.md](CHANGELOG.md) (Rule 4).
- A hardware-dependent feature cannot be done before hardware exists. As of 2026-10-06 every hardware-dependent test is `BLOCKED — HARDWARE REQUIRED`.
- A test result applies only to the hardware revision, platform and software version it was recorded on. A result on Pi 5 says nothing about CM4, and vice versa (Rule 8; [BUILD_SYSTEM.md](BUILD_SYSTEM.md) §1.3). Likewise, a capture result on the 2-lane configuration says nothing about the 4-lane configuration, and vice versa (reasoning: the two configurations carry different mode sets, as recorded in REQ-CAP-007).
- A source fact in [REFERENCES.md](REFERENCES.md) is never test evidence. `CONFIRMED` means "the source says so", not "observed on PACSCORDER".

---

## 3. Release checklist

PROPOSED procedure. **Never executed.** Copy the list into the release's entry in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) and tick items there, with evidence links.

### 3.1 Scope and decisions

- [ ] Each requirement claimed by the release is `TESTED — PASS` in [TRACEABILITY.md](TRACEABILITY.md), with its canonical TEST ID ([§2](#2-definition-of-done-rule-24)).
- [ ] Each ADR the release depends on is `ACCEPTED`. PROPOSED rule: an ADR that is still PROPOSED or OPEN must be named under *Known Issues*. Today this affects ADR-002 and ADR-004 to ADR-008 (ADR-001 and ADR-003 are ACCEPTED). *(2026-10-09: ADR-009 is also ACCEPTED, since 2026-10-08; the list of not-yet-accepted ADRs is unchanged.)*
- [ ] Each risk in [RISKS.md](RISKS.md) that affects a claimed requirement is either retired with test evidence or listed as a known issue.
- [ ] Each `OQ-NNN` answered during the cycle is closed with evidence in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).
- [ ] PROPOSED: for each capture configuration required by REQ-CAP-007 (2-lane and 4-lane), the release names the platform and hardware revision tested. A configuration that was not tested is listed under *Known Issues*.
- [ ] *(Added 2026-10-09; ADR-009.)* PROPOSED: for each platform, the release names the recording storage it was tested with — the NVMe SSD (and on CM4 the PCIe adaptor), the HDD enclosure and its USB-to-SATA bridge VID:PID with the driver it bound to (`uas` or `usb-storage`) — and the TEST-REC-001 result. Reasoning from [J-24]: driver binding can differ between CM4 and CM5 (on CM4 it depends on the USB host controller), so a bridge qualified on one board is not qualified on the other (RISK-027; OQ-121, OQ-122).

### 3.2 Documentation (Rules 5, 12, 17, 18, 20)

- [ ] [CHANGELOG.md](CHANGELOG.md): the `Unreleased` entries are moved under the new version heading with the date, using *Added / Changed / Fixed / Known Issues* (Rule 4). Earlier entries are not edited (Rule 21).
- [ ] [PROJECT_STATUS.md](PROJECT_STATUS.md): phase, *Last Verified* date, and the *Hardware*, *Kernel* and *Buildroot* (or OS) fields (Rule 18).
- [ ] [TRACEABILITY.md](TRACEABILITY.md): result column for every requirement (Rule 12).
- [ ] [REQUIREMENTS.md](REQUIREMENTS.md): implementation status matches TRACEABILITY.
- [ ] [ARCHITECTURE.md](ARCHITECTURE.md) matches the code (Rule 5). [DEVICE_TREE.md](DEVICE_TREE.md) has an OLD → CHANGE → REASON → EXPECTED → ACTUAL record for each DT change (Rule 6).
- [ ] [TC358743_DRIVER.md](TC358743_DRIVER.md) and the other technical documents state what was actually tested (Rule 7).
- [ ] [HARDWARE.md](HARDWARE.md) names the hardware revisions the release was tested on (Rule 8).
- [ ] [BUILD_SYSTEM.md](BUILD_SYSTEM.md) matches the build configuration used.
- [ ] [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) has the release session entry (Rule 3).
- [ ] [ARCHIVED_APPROACHES.md](ARCHIVED_APPROACHES.md) records anything abandoned during the cycle (Rule 14).

### 3.3 Build and artefacts

- [ ] TEST-BLD-001 is `TESTED — PASS` from a clean checkout of the release commit.
- [ ] The released image is the project-built image produced by that build (REQ-BLD-002), not an unmodified stock image.
- [ ] The build inputs are pinned and recorded: the `rpi-image-gen` tag or Buildroot release, the kernel and firmware versions, and the package-mirror snapshot (REQ-BLD-001, RISK-017).
- [ ] The image file and its checksum are archived.
- [ ] The SBOM is generated and archived ([§5](#5-sbom)).
- [ ] The EEPROM bootloader version used in testing is recorded, along with how units are set to it ([§8](#8-eeprom-bootloader-pinning)). The commands that read the kernel, firmware and EEPROM versions on a unit are not attested in the register: NEEDS VERIFICATION (OQ-101).

### 3.4 Licence compliance (RISK-015)

- [ ] Licence review done for every shipped component, including x264, FFmpeg, GStreamer and the Raspberry Pi firmware ([§4](#4-licence-and-compliance)). LEGAL CLARIFICATION REQUIRED (OQ-086, OQ-087, OQ-088).
- [ ] Corresponding source for copyleft components is archived for exactly the shipped versions, from every archive the packages came from ([§4.3](#43-corresponding-source)).
- [ ] Licence texts and notices are included as the review requires.
- [ ] *(Added 2026-10-08; scope updated the same day, H.265 deferred.)* The review covers x265 / `libx265` (GPLv2-or-later or commercial) and every binary that links it, including the GPL FFmpeg build and GStreamer's `x265enc` plugin, and the AAC and Opus encoders shipped ([§4.1](#41-components-with-licence-obligations)). x265 stays in the review although H.265 is deferred (REQ-ENC-002): reasoning from [H-09] and [H-12] (CORRECTED), an image that contains the Raspberry Pi `ffmpeg` (whose `libavcodec61` depends on `libx265-215`) or `gstreamer1.0-plugins-bad` (which installs `libgstx265.so`) still contains x265. LEGAL CLARIFICATION REQUIRED (OQ-087).
- [ ] *(Added 2026-10-08; scope updated the same day, H.265 deferred.)* Patent licensing is resolved, or recorded as a known issue, for every codec the release ships: H.264 (OQ-086) and AAC (OQ-113). H.265/HEVC (OQ-109) applies only if REQ-ENC-002 is re-activated (deferred — REQ-ENC-002; not in current scope); whether an image that contains `libx265` only as a package dependency raises any HEVC patent question is for the legal review. LEGAL CLARIFICATION REQUIRED.
- [ ] *(Added 2026-10-08.)* PROPOSED: the image contains no `fdk-aac` (`libfdk-aac2t64`, `fdkaacenc`, or an FFmpeg built with `libfdk_aac` / `--enable-nonfree`); checked in the SBOM ([§4.4](#44-proposed-release-rules-for-codecs-added-2026-10-08), [§5](#5-sbom)).

### 3.5 Test evidence

- [ ] For every TEST ID run, [TESTING.md](TESTING.md) records the Rule 9 fields: setup, command, expected result, actual result, date, hardware revision and software version (`vX.Y.Z`).
- [ ] Logs, captures and measurements are archived with the release record, and their location is written into it ([§9](#9-release-record-template-proposed)).
- [ ] Failed tests are recorded as `TESTED — FAIL`, not removed (Rule 21).

### 3.6 Git and publication (Rule 15)

- [ ] The release commit has a Rule 15 description: commit title, description, files changed, reason and tests.
- [ ] A Git tag `vX.Y.Z` is on that commit.
- [ ] A row is added to [§10](#10-release-history).

---

## 4. Licence and compliance

**Status:** no licence review has been done. **LEGAL CLARIFICATION REQUIRED** (RISK-015; OQ-086, OQ-087, OQ-088). The facts below say what the sources state. They are not legal advice and not a compliance decision. *(Added 2026-10-08.)* Since the owner decisions of 2026-10-07 (H.265 required, REQ-ENC-001; HDMI audio required, REQ-CAP-006), the review also covers x265, HEVC patent pools and the audio encoders (OQ-109, OQ-113); the rows marked "added 2026-10-08" come from research topics H and I. *(Superseded in part 2026-10-08: the owner deferred H.265 — "H.264 only for now", OQ-103 ANSWERED; REQ-ENC-002 DEFERRED. x265's software licence stays in the review because the Raspberry Pi FFmpeg depends on `libx265-215` [H-09]. The HEVC patent row is kept as evidence for REQ-ENC-002 and is not in current scope.)*

### 4.1 Components with licence obligations

| Component | What the source says | Facts |
|---|---|---|
| x264 | Buildroot's x264 help text says x264 is released under the GNU GPL. In Buildroot, the GStreamer `x264enc` element (`BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264`) selects x264. | [D-47] |
| FFmpeg with `libx264` | `libx264` is in FFmpeg configure's `EXTERNAL_LIBRARY_GPL_LIST`, so FFmpeg must be built with `--enable-gpl` to use it. | [D-42] |
| FFmpeg in Buildroot | Buildroot packages upstream FFmpeg 6.1.5. It passes `--enable-libx264` only when `BR2_PACKAGE_X264=y` and `BR2_PACKAGE_FFMPEG_GPL=y`. `BR2_PACKAGE_FFMPEG_GPL=y` on its own changes `FFMPEG_LICENSE` from `LGPL-2.1+, libjpeg license` to `LGPL-2.1+, libjpeg license and GPL-2.0+`, adding `COPYING.GPLv2`. | [D-46] (CORRECTED) |
| FFmpeg in Raspberry Pi OS | `ffmpeg` 8:7.1.5-0+deb13u1+rpt2 comes from the Raspberry Pi archive [G-29]. Whether that build enables `--enable-gpl` / `libx264` is **not in the register: NEEDS VERIFICATION**. *(Superseded 2026-10-08: the source package is configured with `--enable-libx265` in every flavour and `--enable-libx264` in the full build [H-08]; `libavcodec61` depends on `libx265-215` and `libx264-164` [H-09]; linking `libx264` and `libx265`, which are on FFmpeg's GPL library list, makes it a GPL build [I-39]. Confirming the configuration on the shipped image remains BUILD TEST REQUIRED — research gap, topic I.)* | [G-29], [H-08], [H-09], [I-39] |
| x265 / `libx265` *(added 2026-10-08)* | Copyright MulticoreWare; licensed under GPL version 2 "or (at your option) any later version", and also under a commercial proprietary licence. The x265 documentation states that neither the GPL nor the commercial licence covers HEVC patents. Raspberry Pi OS ships Debian's x265 4.1-2 (`libx265-215`) unchanged. *(2026-10-08: H.265 is deferred, REQ-ENC-002, but this row stays in current scope: the Raspberry Pi `libavcodec61` depends on `libx265-215` [H-09], so x265 is in any image that ships that FFmpeg.)* | [H-39], [H-01], [H-02], [H-09] |
| GStreamer `x265enc` *(added 2026-10-08)* | In `gstreamer1.0-plugins-bad`; the Raspberry Pi build Build-Depends on `libx265-dev` and installs `libgstx265.so`. Reasoning: the plugin links the GPL x265, so it is part of the x265 licence review (OQ-087). *(2026-10-08: unused while H.265 is deferred, REQ-ENC-002, but still installed with `gstreamer1.0-plugins-bad`, so the row stays in current scope.)* | [H-11], [H-12] (CORRECTED), [H-39] |
| HEVC patents *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | Access Advance (HEVC Advance pool) says a licence is "most likely" needed for any product that can encode and/or decode HEVC; the royalty falls due when a Consumer HEVC Product is sold to an end user, if a listed essential patent is in force in the country of manufacture or sale [H-41]. Its rate table for licences effective on or after 1 July 2026 lists "Connected Home & Other Devices" (examples include surveillance cameras, conferencing products, digital signage and HEVC software): for devices over $80 the in-compliance rate without trademark discount is $1.111 (Region 1) / $0.555 (Region 2) per unit, with caps and an annual credit; the non-compliant standard rate is $1.333 / $0.667 [H-42]. VCL Advance (the former Via LA HEVC/VVC programme, acquired by Access Advance as of 15 December 2025): units 1–100,000 $0.00 (for one legal entity in an affiliated group), then $0.30 (Region 1) / $0.20 (Region 2) per unit, with a $30,000,000 annual cap per enterprise [H-40]. Which category PACSCORDER falls into, and whether licensors outside both pools assert patents, is not established (research gap, topic H). **LEGAL CLARIFICATION REQUIRED (OQ-109).** | [H-40], [H-41], [H-42] |
| AAC encoders *(added 2026-10-08)* | Shipped options: FFmpeg's native `aac` encoder in the Raspberry Pi build [I-39], [I-40] (CORRECTED); GStreamer `voaacenc` in `gstreamer1.0-plugins-bad`, linking `libvo-aacenc0` [I-44]; `avenc_aac` in Debian's `gstreamer1.0-libav`, which wraps FFmpeg's native encoder [I-46]. Their software licences are covered by the FFmpeg and GStreamer reviews above; `libvo-aacenc0`'s licence is not in the register (NEEDS VERIFICATION). | [I-39], [I-40], [I-44], [I-46] |
| AAC patents *(added 2026-10-08)* | Not researched beyond the `fdk-aac` licence below. Research notes that the native FFmpeg `aac`, `voaacenc` and `fdk-aac` all implement patented AAC (research gap, topic I — not a register fact). **LEGAL CLARIFICATION REQUIRED (OQ-113).** | — |
| `fdk-aac` *(added 2026-10-08)* | Debian trixie ships `fdk-aac` 2.0.3-1 in **non-free** under the "Fraunhofer-FDK-AAC-for-Android" licence; Debian's copyright file says it "is incompatible with any version of the GNU GPL", and its clause 3 grants no patent licence [I-43]. In FFmpeg 7.1's configure `libfdk_aac` is on the non-free list: a `--enable-gpl` build (needed for x264/x265) can enable it only with `--enable-nonfree`, which makes "the resulting libs and binaries ... unredistributable" [I-42]. The Raspberry Pi FFmpeg and GStreamer packages do not include it [I-39], [I-44]. Reasoning from [I-39], [I-42], [I-43]: `fdk-aac` cannot be shipped with PACSCORDER's GPL media stack. | [I-39], [I-42], [I-43], [I-44] |
| Opus *(added 2026-10-08)* | `libopus0` 1.5.2-2 from Debian, not rebuilt by Raspberry Pi; used by FFmpeg `libopus` and GStreamer `opusenc` [I-45], [I-41]. Opus licensing is not in the register (NEEDS VERIFICATION; noted in OQ-113). | [I-41], [I-45] |
| GStreamer `webrtcbin` | Declared with licence "LGPL" | [F-42] |
| GStreamer `webrtcsink` (gst-plugins-rs) | MPL-2.0. Not packaged in Buildroot. *(Added 2026-10-09.)* Not packaged in Debian trixie either [K-06] (Raspberry Pi archive: see [BUILD_SYSTEM.md](BUILD_SYSTEM.md) §1.9). If ADR-007 (OPEN) chooses it, a self-built plugin would ship, and its corresponding source would have to be archived by PACSCORDER itself (reasoning, as in [§4.3](#43-corresponding-source); RISK-034; LEGAL CLARIFICATION REQUIRED for the MPL-2.0 obligations, OQ-087). | [F-43], [K-06] |
| MediaMTX *(added 2026-10-09; only if the chosen live route uses it, ADR-007 OPEN)* | MediaMTX's licence file is the MIT licence, and MediaMTX describes itself as a single executable (community-tier entry) [F-44]. Packaging in Debian or the Raspberry Pi archive is NEEDS VERIFICATION ([BUILD_SYSTEM.md](BUILD_SYSTEM.md) §1.9). | [F-44] |
| Raspberry Pi GPU firmware and bootloader files | Buildroot 2026.08's `rpi-firmware` declares `BSD-3-Clause` with licence file `boot/LICENCE.broadcom`. That file is a binary-only, no-modification licence restricted to use "for the purposes of developing for, running or using a Raspberry Pi device", so the BSD-3-Clause label is misleading. Raspberry Pi OS installs the equivalent, newer proprietary files through `raspi-firmware 1:1.20260915-1`. | [G-69] (CORRECTED) |
| `rpi-image-gen` (build tool) | BSD-3-Clause | [G-37] |
| ATEM SDK (if used) | Downloading it requires accepting the "bmd-standard-sdk" terms (OQ-089). Reasoning: the current REQ-ATEM-001 scope (HDMI capture of the ATEM output, owner 2026-10-07) needs no ATEM SDK, so this applies only if the owner adds network integration. | [F-02] |
| H.264 patents | **Not researched** (*research gap*, topics D and G). LEGAL CLARIFICATION REQUIRED (OQ-086). *(2026-10-08: still not researched; HEVC and AAC patents are now listed in the rows above.)* | — |

### 4.2 Why this matters more on Pi 5/CM5

- Raspberry Pi 5 uses software video encoders [G-22]. The Pi 5 and CM5 product briefs list no hardware video encoder [D-31].
- Reasoning from [G-22], [D-31], [D-42] and [D-47]: on Pi 5/CM5 an H.264 product needs a software encoder. If that encoder is x264, GPL obligations follow.
- A possible non-GPL alternative is `openh264enc`, which accepts only I420 input [D-41]. Its licence is **not in the register (NEEDS VERIFICATION, OQ-087)**.
- On Pi 4/CM4 the hardware encoder driver `bcm2835-codec` exists [D-02], [G-18], but the H.264 patent question still applies (OQ-086).
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* **If H.265 is re-activated, it applies on every platform.** `bcm2835-codec` has no HEVC encoder [D-24] and Pi 5/CM5 have no hardware video encoder [D-31]. Reasoning from [D-24], [D-31], [H-39]: a release that ships H.265 encoding on any candidate — including the CM4 bring-up board — uses the GPL x265 and falls under the HEVC patent question (OQ-109), not only Pi 5/CM5. Since 2026-10-08 the owner requires H.264 only ("H.264 only for now", OQ-103), so this is not in current scope.
- *(Added 2026-10-08.)* **x265 in the H.264-only scope.** Reasoning from [H-09]: the Raspberry Pi `libavcodec61` depends on `libx265-215`, so on every platform an image that ships the Raspberry Pi FFmpeg still contains the GPL x265, although no output uses H.265. Its licence therefore stays in the review (OQ-087).

### 4.3 Corresponding source

| Build basis | Where source comes from | Facts |
|---|---|---|
| Raspberry Pi OS / `rpi-image-gen` | **Two origins.** Debian, plus Raspberry Pi's overriding `+rpt` builds (for example `gstreamer1.0-plugins-bad` [G-27], `ffmpeg` [G-29] and `gstreamer1.0-plugins-base` [G-31]). `archive.raspberrypi.com` publishes trixie `main/source/Sources` and `Sources.gz` (HTTP 200), so `+rpt` source can be fetched with `apt deb-src`. | [G-27], [G-29], [G-31], [G-70] |
| Buildroot | `make legal-info` collects a README, the config, sources, patches, a manifest with licences, and licence texts under `legal-info/`. It does **not** produce some material, such as some external toolchains' source and Buildroot's own source. "You (or your legal department) have to check the output of make legal-info before using it as your own compliance delivery." | [G-68] |

The source must be archived for **exactly** the shipped versions. Reasoning from [G-07] and [G-08]: the archives move, and that is part of RISK-017.

*(Added 2026-10-08.)* For H.265 and audio this includes Debian's x265 4.1-2 [H-01], [H-02], the Raspberry Pi `ffmpeg` and `gstreamer1.0-plugins-bad` builds [H-08], [H-12] (CORRECTED), and Debian's `gstreamer1.0-libav` [I-46]. *(2026-10-08, H.265 deferred — REQ-ENC-002: x265 stays in this list whenever the Raspberry Pi `ffmpeg` ships, because `libavcodec61` depends on `libx265-215` [H-09].)* Any package carried outside both archives to close a version gap ([BUILD_SYSTEM.md](BUILD_SYSTEM.md) §1.8) has to have its corresponding source archived by PACSCORDER itself (reasoning). *(2026-10-08: every gap listed in BUILD_SYSTEM.md §1.8 is tied to an H.265 question, so none applies in the current scope — deferred, REQ-ENC-002.)*

### 4.4 PROPOSED release rules for codecs (added 2026-10-08)

PROPOSED, OWNER DECISION REQUIRED; none of this is a legal conclusion (LEGAL CLARIFICATION REQUIRED):

1. **`fdk-aac` is not shippable.** No release image contains `libfdk-aac2t64`, `fdkaacenc` or an FFmpeg built with `libfdk_aac` / `--enable-nonfree`. Reasoning from [I-42], [I-43]: with the GPL FFmpeg build that x264/x265 require, enabling it makes the result unredistributable, and Debian calls its licence incompatible with every GPL version. AAC comes from FFmpeg's native `aac`, `avenc_aac` or `voaacenc` instead [I-39], [I-40] (CORRECTED), [I-44], [I-46].
2. **Codecs are named per release.** Each release record states which video codecs and which audio encoders it ships on each output (OQ-103, OQ-063), because each brings its own licence and patent items ([§4.1](#41-components-with-licence-obligations)). *(2026-10-08: the video codec is H.264 on every output — owner, "H.264 only for now", OQ-103 ANSWERED. H.265 is named only if REQ-ENC-002 is re-activated.)*
3. **Patent status is recorded.** For H.264 (OQ-086) and AAC (OQ-113) the release record names the licence position reached in legal review, or lists it under *Known Issues*. *(2026-10-08: H.265 (OQ-109) is added to this rule only if REQ-ENC-002 is re-activated; deferred — not in current scope.)*
4. **x265 licence route is recorded.** Whether x265 is shipped under the GPL, with corresponding source, or under MulticoreWare's commercial licence [H-39] is decided in legal review (research open question, topic H; OQ-087) and recorded with the release. *(2026-10-08: this rule stays in current scope although H.265 is deferred, because x265 ships as a dependency of the Raspberry Pi FFmpeg [H-09].)*

---

## 5. SBOM

| Source | What exists | Facts |
|---|---|---|
| Official Raspberry Pi OS image (2026-10-06 Lite) | SPDX-2.3 SBOM (`.sbom.xz`) generated by syft-1.54.0. The `.info` file names the pi-gen commit used. | [G-34] |
| `rpi-image-gen` | "Generate Software Bill of Materials and CVE reports"; an SBOM for every build; an `sbom-base` layer | [G-39], [G-36] (CORRECTED), [G-40] |
| Buildroot | `make legal-info` manifest with licences ([§4.3](#43-corresponding-source)). Whether Buildroot also emits an SPDX SBOM is **not in the register: NEEDS VERIFICATION**. | [G-68] |

Open:

- Whether the `rpi-image-gen` SBOM maps each binary to the exact source package and version, across both the Debian and the Raspberry Pi archive, is untested (*research gap*, topic G; OQ-087).
- PROPOSED release rule: archive the SBOM with every release, and compare it with the previous release's SBOM as part of the change record (RISK-017).
- *(Added 2026-10-08.)* PROPOSED release rule: use the SBOM to confirm that no `fdk-aac` package is present ([§4.4](#44-proposed-release-rules-for-codecs-added-2026-10-08)) and to record the shipped versions of x265 / `libx265-215`, FFmpeg, the GStreamer plugin packages, `libopus0` and the AAC libraries ([BUILD_SYSTEM.md](BUILD_SYSTEM.md) §1.6). Whether the SBOM lists these at the needed granularity is part of the untested SBOM-to-source mapping (OQ-087).

---

## 6. Field update and A/B mechanisms (OPEN design options)

**No update mechanism has been chosen.** This is OQ-069 (OWNER DECISION REQUIRED). The table lists what the sources offer; none of it has been tried on PACSCORDER.

Whatever mechanism is chosen, the update policy must also say whether an update may be installed, or a reboot into a new slot triggered, while a recording or stream is active, and what happens to an in-progress recording. That is OQ-094 (OWNER DECISION REQUIRED).

| Option | What the source says | Platforms (source) | Constraints and unknowns | Facts |
|---|---|---|---|---|
| `apt` package updates on the unit | `sudo apt full-upgrade` "also updates your Linux kernel and firmware". | All | The default kernel series changed from 6.12 to 6.18 within trixie [G-05]. Reasoning: an unattended fleet can move to an untested kernel, and the unit no longer matches the released image. | [G-08], [G-05] |
| Firmware `autoboot.txt` + `tryboot` | `autoboot.txt` in the first boot partition sets `boot_partition`. It accepts `[all]`, `[none]` and `[tryboot]`, and supports `tryboot_a_b=1`. Combined with tryboot it is the firmware's fail-safe A/B mechanism. It only picks the boot partition: a production A/B system also needs redundant partitions and update tooling. | Pi 4 Model B, CM4, Pi 5 (as summarised by the verifier; the device list was not quoted verbatim). CM5: NEEDS VERIFICATION (*research open question*, topic G) | Update tooling must be supplied by something else | [G-50] |
| `tryboot` one-shot | Set by adding `tryboot` after the partition number in the reboot command, for example `sudo reboot '0 tryboot'`. The flag is cleared before the firmware starts, so a crash or reset reverts to the original configuration. All models support it, but Pi 4B rev 1.0/1.1 need an EEPROM that is not write-protected. With secure boot, tryboot mode loads `tryboot.img` instead of `boot.img`. | Pi 4 Model B, CM4, Pi 5, CM5 | Command NOT YET RUN ON PACSCORDER HARDWARE | [G-51] (CORRECTED) |
| `rpi-image-gen` `image-rota` | Immutable GPT A/B system slots with a shared persistent data partition. EROFS (zstd) by default since v2.3.0, dm-verity since v2.4.0, LUKS2 since v2.5.0, and `slot-shared` paths since v2.6.0. | Pi 4 Model B, CM4, Pi 5, CM5 | Whether it uses tryboot / `autoboot.txt`, and whether it supports CM4/CM5 eMMC, is open (*research open question*, topic G) | [G-40], [G-41] |
| Raspberry Pi Connect Remote Update | A/B boot updates (an image and `update.tar.zst` built with `rpi-image-gen`) need a Raspberry Pi 4 or later that is connected to the internet, opted in and signed in to Connect, with a storage device (typically microSD) of at least 16 GB. Personal-account deployments need the device online; Connect for Organisations devices update the next time they sign in. The current docs do not call the feature beta. Connect also supports script artefacts built with `otamaker`. `rpi-image-gen` v2.2.0 added Connect image builds (`examples/ota`). | Pi 4 Model B, Pi 5 (applies-to of [G-42], [G-43]) | Compute Module support is unconfirmed (*research gap*, topic G). Depends on Raspberry Pi's Connect service. `rpi-connect-ota` 1.3.16 is in the archive [G-53]. | [G-42], [G-43] (CORRECTED), [G-53] |
| Bootloader EEPROM A/B | A/B updates of the bootloader EEPROM itself are "compatible only with the Raspberry Pi 5, Raspberry Pi Compute Module 5, and Raspberry Pi 5 keyboard computer devices". They are enabled with `raspi-config`; while on, direct EEPROM writers such as `flashrom` no longer work. | Pi 5, CM5 | Not available on Pi 4/CM4 | [G-52] |
| RAUC / SWUpdate / Mender | Debian trixie: `rauc` 1.13-3+deb13u1, `swupdate` 2024.12.1+dfsg-3+deb13u2 (2026.05.1 in trixie-backports), `mender-client` 3.4.0+ds1-5+b15. Buildroot 2026.08: `rauc` 1.15.2, `swupdate` 2026.05.1, `mender` 3.5.3. | All | Integration with the Raspberry Pi boot flow was not researched | [G-49], [G-64] |
| Whole-image upgrade (Buildroot's position) | The Buildroot manual recommends upgrading the whole root filesystem image at once, so that the deployed image "is guaranteed to really be the one that has been tested and validated". It says "Buildroot is not meant to be a distribution". | — | — | [G-67] |
| Read-only root via `raspi-config` | Overlay File System: `overlayroot=tmpfs`, refused when `MemTotal` is 262144 kB or less. Not an update mechanism. | All | Reasoning: writes are lost at reboot, so recordings need a persistent partition (OQ-006). *(2026-10-09: ADR-009 puts recordings on the NVMe SSD and the USB HDD.)* | [G-48] |
| No field updates | — | — | OWNER DECISION REQUIRED | — |

Storage interacts with this choice. Connect A/B needs at least 16 GB [G-43] (CORRECTED), and A/B layouts hold two system slots [G-41]. Storage size is UNKNOWN — OWNER DECISION REQUIRED (OQ-006, OQ-068). *(Added 2026-10-09; ADR-009.)* Recordings now go to the NVMe SSD and the USB-to-SATA HDD (ADR-009); the boot medium is still not decided (OQ-124). Reasoning: whichever update mechanism is chosen must never rewrite the recording volumes, and if the NVMe SSD also holds the system, its partition layout has to keep the system slots and the recording volume apart (OQ-068, OQ-069). The policy for updates during an active recording stays OQ-094.

---

## 7. Provisioning

Details and commands are in [BUILD_SYSTEM.md §2.7](BUILD_SYSTEM.md#27-provisioning-tools). Facts that affect release planning:

| Topic | Fact | Facts |
|---|---|---|
| USB provisioning | `rpiboot` (usbboot) loads software into a Pi's memory for provisioning and by default exposes the device as USB mass storage (Pi 4B, CM4/4S, Pi 5, CM5 and others). | [G-45] |
| Pi 4 Model B | rpiboot must first be enabled, which on Pi 4B means **permanently programming an OTP GPIO**. | [G-45] |
| Secure boot | The `secure-boot-recovery` / `secure-boot-recovery5` extensions flash a secure-boot bootloader and provision OTP on Pi 4 and Pi 5 [G-45]. `rpi-sb-provisioner` supports Pi 5, Pi 4, CM5, CM4 and Zero 2 W in `secure-boot`, `fde-only` and `naked` modes. Its host must be a 64-bit Pi on Bookworm or newer, with at least 32 GB free and an official 27W supply [G-46]. `rpi-image-gen` integrates with it [G-39]. | [G-45], [G-46], [G-39] |
| Compute Module eMMC | Fit `nRPI_BOOT` (J2 in Raspberry Pi's Compute Module documentation), run `rpiboot`, then write with Imager or `dd`. For many modules the documentation points to the Secure Boot Provisioner. | [G-47] |
| Buildroot | The CM4 IO and CM5 IO defconfigs build the host `rpiboot`. | [G-62] |
| NVMe boot *(added 2026-10-09; only if NVMe is chosen as the boot medium, not decided — OQ-124)* | `BOOT_ORDER` nibble 0x6 boots from an NVMe SSD and is documented for CM4, CM5, Pi 5 and 500+ only. For Compute Modules, edit usbboot `recovery/boot.conf` with `BOOT_ORDER=0xf6`, run `update-pieeprom.sh` and flash with `rpiboot`; CM4 Lite boots NVMe automatically when the SD slot is empty, and eMMC CM4s must put NVMe first. NOT YET RUN ON PACSCORDER HARDWARE. Whether CM5 uses usbboot `recovery5`, and the minimum bootloader version, are research gaps (topic J; OQ-124). | [J-10] |
| Recording volumes *(added 2026-10-09; ADR-009)* | The NVMe SSD and the USB HDD each carry a recording filesystem that is not part of the OS image (ext4 by Claude's proposal in ADR-009, not an owner decision; OQ-120). Reasoning: they must be partitioned and formatted once per unit, and kept through every image update; whether production provisioning or the device's first boot does it, and with which tools, is not decided (OWNER DECISION REQUIRED; OQ-120). Each drive's model and the bridge VID:PID are recorded per unit or per release (OQ-121, OQ-122). | ADR-009 |

Reasoning from [G-45] and [G-46]: secure-boot provisioning writes OTP, which cannot be undone. Signing-key custody and the secure-boot decision must therefore be settled before any production unit is provisioned (OQ-071). The boot-mode jumper and USB boot path of a PACSCORDER carrier are **UNKNOWN — VERIFICATION REQUIRED** (OQ-018).

---

## 8. EEPROM bootloader pinning

**Concern.** A PACSCORDER test result depends on the bootloader as well as the OS image. The bootloader can change independently of the image.

| Fact | Facts |
|---|---|
| The Lite image installs the EEPROM updater `rpi-eeprom` 28.33-1. GPU firmware and bootloader files come from `raspi-firmware`. | [G-06] |
| `rpi-update` also updates the EEPROM bootloader on Raspberry Pi 4 and 5, and is installed by default. | [G-09], [G-10] |
| EEPROM A/B updates exist only on Pi 5 / CM5 ([§6](#6-field-update-and-ab-mechanisms-open-design-options)). | [G-52] |
| Tryboot on Pi 4B rev 1.0/1.1 needs an EEPROM that is not write-protected. | [G-51] (CORRECTED) |
| `BOOT_WATCHDOG_TIMEOUT` can reset a unit whose OS never starts. `NET_INSTALL_ENABLED` defaults to 1 on flagship boards since Pi 4B and to 0 on Compute Modules since CM4. | [G-54] (CORRECTED) |
| Secure-boot provisioning flashes the bootloader. | [G-45], [G-46] |
| *(Added 2026-10-09.)* The boot order is an EEPROM setting: `BOOT_ORDER` nibble 0x6 selects NVMe and is documented for CM4, CM5, Pi 5 and 500+; Compute Modules get it through usbboot `recovery/boot.conf` (`BOOT_ORDER=0xf6`) and `rpiboot`. | [J-10] |
| *(Added 2026-10-09.)* On the CM5 IO Board, which negotiates 5 V at 5 A over USB PD by default, `PSU_MAX_CURRENT=5000` in the EEPROM configuration suppresses the low-current warning. | [J-20] |

**Not register facts** (*research gaps*, topics E and G; all **NEEDS VERIFICATION**):

- the `rpi-eeprom-update` service updates the EEPROM at boot;
- a bootloader `FREEZE_VERSION` setting exists;
- Buildroot 2026.08 has no `rpi-eeprom` package, so a Buildroot image does not manage the EEPROM.

Reasoning from these facts: the OS image version alone does not identify the bootloader version.

PROPOSED release rules (OQ-071; OWNER DECISION REQUIRED):

1. Each release record names the EEPROM bootloader version it was tested with, per platform. The command that reads it is NEEDS VERIFICATION (OQ-101).
2. Production provisioning sets that version.
3. Automatic EEPROM updates are disabled in the image or explicitly accepted; the method is NEEDS VERIFICATION.
4. The bootloader configuration (watchdog, network install) is recorded with the release. *(Added 2026-10-09:)* including `BOOT_ORDER` per platform if NVMe boot is used [J-10] (OQ-124), and `PSU_MAX_CURRENT` if it is set on a CM5 IO Board [J-20].

Per-model EEPROM behaviour on CM4 and CM5 beyond the facts above is **NEEDS VERIFICATION**.

---

## 9. Release record template (PROPOSED)

Fill one record per release, in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) or a release-notes file linked from [§10](#10-release-history).

| Field | Value |
|---|---|
| Version | `vX.Y.Z` |
| Date | YYYY-MM-DD |
| Git tag and commit | |
| Build basis and tool version | Raspberry Pi OS + `rpi-image-gen` tag (ADR-003, ACCEPTED), or Buildroot release if the documented alternative is adopted; the project-built image (REQ-BLD-002) |
| Build host | OS, architecture |
| Package mirror snapshot | (RISK-017) |
| Kernel version and commit | (read-out command NEEDS VERIFICATION, OQ-101) |
| Firmware version | (read-out command NEEDS VERIFICATION, OQ-101) |
| EEPROM bootloader version per platform | ([§8](#8-eeprom-bootloader-pinning)); read-out command NEEDS VERIFICATION, OQ-101 |
| Update policy during active recordings | (OQ-094) |
| `config.txt` and EDID file(s) | Commit-pinned paths; per capture configuration where they differ (REQ-CAP-007, OQ-002) |
| Platforms, capture configurations and hardware revisions tested | e.g. CM4 CAM1, 4-lane, on HW REV A. Every platform and every capture configuration claimed (2-lane, 4-lane; REQ-CAP-007) must be listed. |
| Requirements `TESTED — PASS` | REQ IDs with TEST IDs |
| Known issues | Including PARTIALLY VERIFIED features, open RISKs and PROPOSED/OPEN ADRs |
| Image file and checksum | |
| SBOM file | |
| Source archive | Location |
| Licence review | Reference to the review (RISK-015) |
| Codecs shipped *(added 2026-10-08)* | Video codec per output (H.264 on every output — OQ-103 ANSWERED 2026-10-08; H.265 only if REQ-ENC-002 is re-activated) and audio encoder per output (AAC, Opus; OQ-063), with their package versions; confirmation that no `fdk-aac` is present ([§4.4](#44-proposed-release-rules-for-codecs-added-2026-10-08)) |
| Patent licence position *(added 2026-10-08)* | H.264 (OQ-086), AAC (OQ-113): licence held, or listed under *Known Issues*. HEVC (OQ-109) only if REQ-ENC-002 is re-activated (deferred — not in current scope) |
| x265 licence route *(added 2026-10-08)* | GPL with corresponding source, or MulticoreWare commercial licence [H-39] (OQ-087). Needed while x265 ships as a dependency of the Raspberry Pi FFmpeg [H-09], even with H.265 deferred |
| Recording storage tested *(added 2026-10-09; ADR-009)* | Per platform: NVMe SSD model and firmware (CM4: PCIe adaptor); HDD, enclosure, bridge VID:PID and bound driver (`uas` or `usb-storage`); recording-volume filesystem and mount options (OQ-120); TEST-REC-001 result (OQ-121, OQ-122; RISK-027) |
| Kernel command line *(added 2026-10-09)* | Commit-pinned `cmdline.txt`, including any `usb-storage.quirks` entry (OQ-122) and `pci=nomsi` (CM4, OQ-123) ([BUILD_SYSTEM.md](BUILD_SYSTEM.md) §1.9) |
| Boot medium and boot order *(added 2026-10-09)* | SD, eMMC or NVMe per platform; `BOOT_ORDER` if NVMe [J-10] (OQ-124) |
| Test evidence archive | Location |

---

## 10. Release history

| Version | Date | Commit / tag | Platforms and HW revisions tested | Notes |
|---|---|---|---|---|
| — | — | — | — | **No release exists** as of 2026-10-06. The current version is `Unreleased`. |

---

## Verification status

### Verified from sources (fact IDs)

These statements are supported by entries in [REFERENCES.md](REFERENCES.md), with verdict CONFIRMED or CORRECTED (corrected wording used):

- **Licences:** [D-42], [D-46] (CORRECTED), [D-47], [F-42], [F-43], [G-37], [G-69] (CORRECTED), [F-02].
- **Software-encode context:** [G-22], [D-31], [D-41]. Pi 4/CM4 hardware encoder driver: [D-02], [G-18].
- **Corresponding source:** [G-27], [G-29], [G-31], [G-68], [G-70], [G-07], [G-08].
- **SBOM:** [G-34], [G-36] (CORRECTED), [G-39], [G-40].
- **Update mechanisms:** [G-05], [G-08], [G-40]–[G-43] ([G-43] CORRECTED), [G-48]–[G-53] ([G-51] CORRECTED), [G-64], [G-67].
- **Provisioning and bootloader:** [G-06], [G-09], [G-10], [G-45]–[G-47], [G-54] (CORRECTED), [G-62].
- *(Added 2026-10-08.)* **H.265 licensing and packages (topic H):** [H-01], [H-02], [H-08], [H-09], [H-11], [H-12] (CORRECTED), [H-39], [H-40], [H-41], [H-42]; context [D-24]. Since H.265 was deferred (REQ-ENC-002), the HEVC patent facts [H-40]–[H-42] are evidence for REQ-ENC-002 only; the x265 package and licence facts still apply because x265 ships as an FFmpeg dependency [H-09].
- *(Added 2026-10-08.)* **Audio encoders and `fdk-aac` (topic I):** [I-39], [I-40] (CORRECTED), [I-41], [I-42], [I-43], [I-44], [I-45], [I-46].
- *(Added 2026-10-09.)* **Recording storage, NVMe boot and EEPROM settings (topic J):** [J-10], [J-20], [J-24]. **Live-publishing licences and packaging (topic K):** [K-06]; MediaMTX licence [F-44] (community-tier entry, worded as such). All cited J and K entries have verdict `CONFIRMED`. The research gaps on CM5 `recovery5` and the minimum bootloader version come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json) and are not register facts.

The *research gap* items (EEPROM update service, `FREEZE_VERSION`, Buildroot EEPROM handling, Connect support on Compute Modules, `image-rota` with tryboot, SBOM-to-source mapping, H.264 patents) are **not** register facts and remain NEEDS VERIFICATION. *(Added 2026-10-08:)* so are the topic H and I items cited as research gaps or open questions here (HEVC pool category for PACSCORDER and licensors outside the pools; AAC patents of the native encoders; confirming the FFmpeg configuration on the image; buying a commercial x265 licence instead of meeting the GPL).

### Verified on PACSCORDER hardware

**Nothing.** No hardware exists as of 2026-10-06. No image has been built or released, no update or provisioning mechanism has been tried, and no command in this document has been run for PACSCORDER. *(2026-10-09: still nothing; no storage part has been chosen, and no NVMe boot or recording-volume provisioning has been tried.)*

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review pass against [REFERENCES.md](REFERENCES.md): Rule 4 quoted exactly; EEPROM reasoning in §1.3 qualified; PROPOSED label added to the Known-Issues rule in §2; Pi 4/CM4 encoder statement cited ([D-02], [G-18]); `autoboot.txt` platform list limited to what [G-50] records (CM5 NEEDS VERIFICATION); J2 reference attributed to [G-47]. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: update policy versus active recordings and streams linked to OQ-094 (§6, release record template); commands that read kernel, firmware and EEPROM versions marked NEEDS VERIFICATION and linked to OQ-101 (§3.3, §8, release record template); traceability row lists OQ-094 and OQ-101 | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: REQ-BLD-002 (the release image is the project-built OS image; build tool per ADR-003, still PROPOSED) in the header, §1.3, §3.3 checklist and §9 template; REQ-CAP-007 (2-lane and 4-lane configurations) in the header, §1.3, §2 (results do not carry across configurations), §3.1 checklist and §9 template (configurations tested; EDID per configuration, OQ-002); §4.1 ATEM SDK row notes that the current REQ-ATEM-001 scope (HDMI capture only) needs no SDK. No citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated (verification pass): §3.1 checklist list of PROPOSED/OPEN ADRs corrected from "ADR-002 to ADR-007" to "ADR-002 to ADR-008" (ADR-008 is PROPOSED in DECISIONS.md). No citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); header "Applies to" (build tool is `rpi-image-gen` under ADR-003, Buildroot the documented alternative) and traceability (ADR-003 ACCEPTED); §3.1 checklist list of not-yet-accepted ADRs changed to "ADR-002 and ADR-004 to ADR-008" (ADR-001 and ADR-003 ACCEPTED); §9 template build-basis row. Other ADR statuses unchanged; no citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-08 | Licensing additions from the second set of owner decisions of 2026-10-07 (H.264 + H.265, REQ-ENC-001; HDMI audio required, REQ-CAP-006; CM4 + CM5 bring-up) and research topics H and I. Header (Last updated, Applies to, Verification, Traceability: REQ-ENC-001, REQ-CAP-006, RISK-022, OQ-103, OQ-109, OQ-113); research-gap source note extended to the 2026-10-08 JSON; §3.4 three checklist items (x265 and audio-encoder licence review; H.264/HEVC/AAC patent position; no `fdk-aac` in the image); §4 status note; §4.1 "FFmpeg in Raspberry Pi OS" NEEDS VERIFICATION marked superseded by [H-08], [H-09], [I-39], and new rows for x265 / `libx265` (GPLv2-or-later or commercial; no patent coverage [H-39]), GStreamer `x265enc`, HEVC patents (VCL Advance and Access Advance rates [H-40]–[H-42]; OQ-109), AAC encoders, AAC patents (OQ-113), `fdk-aac` (non-free, GPL-incompatible, unredistributable in a GPL FFmpeg build [I-42], [I-43]: not shippable) and Opus; H.264 patents row annotated; §4.2 H.265 applies on every platform including CM4 [D-24]; §4.3 corresponding-source note for the H.265/audio packages; new §4.4 PROPOSED release rules for codecs (`fdk-aac` not shippable; codecs, patent position and x265 licence route recorded per release); §5 SBOM rule for `fdk-aac` absence and codec package versions; §9 template rows for codecs shipped, patent licence position and x265 licence route; Verification status gains topics H and I and [D-24]. No requirement, decision, risk or test status changed; no release exists. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header Applies to (a release ships H.264 only; H.265 deferred; x265 licence still applies because `libavcodec61` depends on `libx265-215` [H-09]) and Traceability (REQ-ENC-001 H.264 only, REQ-ENC-002 DEFERRED, RISK-022 and OQ-109 not in current scope, OQ-103 ANSWERED); §3.4 x265 licence-review item kept with the dependency reasoning ([H-09], [H-12]) and patent item narrowed to H.264 and AAC, HEVC (OQ-109) only if REQ-ENC-002 is re-activated; §4 status note marked superseded in part; §4.1 x265 row annotated (still in scope as an FFmpeg dependency, [H-09] added to its Facts), GStreamer `x265enc` row annotated (unused but still installed with plugins-bad) and HEVC patents row labelled "deferred — REQ-ENC-002; not in current scope"; §4.2 H.265-on-every-platform bullet relabelled as deferred and made conditional, new bullet on x265 in the H.264-only scope; §4.3 notes that x265 source stays archived with `ffmpeg` and that the BUILD_SYSTEM.md §1.8 gaps are all tied to H.265 questions; §4.4 rules 2–4 annotated (H.264 only; HEVC patents only if re-activated; x265 route still needed); §9 template rows for codecs, patent position and x265 route updated; Verification status note on topic H. No other decision, status or citation changed. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header (Last updated; Verification adds topics J and K; Traceability adds ADR-009, REQ-REC-001, RISK-027, RISK-034, OQ-120 to OQ-124, TEST-REC-001); research-gap note names the 2026-10-08 storage-latency JSON; new dated paragraph on the release items ADR-009 adds; §3.1 ADR-list sentence annotated (ADR-009 ACCEPTED; not-yet-accepted list unchanged) and a PROPOSED checklist item naming the storage parts tested per platform (reasoning from [J-24]; RISK-027); §4.1 `webrtcsink` row extended (not packaged in Debian trixie [K-06]; self-built plugin source archived by PACSCORDER, reasoning; RISK-034) and a MediaMTX licence row [F-44] (community; only if ADR-007 chooses it); §6 read-only-root row annotated and a dated note that updates must never rewrite the recording volumes (reasoning; OQ-068, OQ-069, OQ-124); §7 rows for NVMe boot [J-10] (NOT YET RUN) and recording-volume preparation (reasoning; OQ-120); §8 EEPROM facts `BOOT_ORDER` [J-10] and `PSU_MAX_CURRENT` [J-20], release rule 4 extended; §9 template rows for recording storage tested, kernel command line and boot medium / boot order; Verification status lists [J-10], [J-20], [J-24], [K-06], [F-44]. No release exists; no requirement, decision, risk or test status changed. Verifier pass (same date): §3.1 checklist item "driver binding differs between CM4 and CM5" → "can differ … (on CM4 it depends on the USB host controller)" [J-24]; §7 and §8 `BOOT_ORDER` 0x6 rows: "on CM4, CM5, Pi 5 and 500+" → "documented for CM4, CM5, Pi 5 and 500+" [J-10]. | Claude (session 2026-10-09) |
