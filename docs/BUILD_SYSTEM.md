# PACSCORDER Build System

| | |
|---|---|
| Document status | DRAFT. **No build exists.** There is no build configuration, script or image in this repository. The owner requires the product to run its own project-built OS image (REQ-BLD-002, DRAFT, 2026-10-07). The OS and image basis, including the build tool (ADR-003), is **ACCEPTED** (owner, 2026-10-07: "accept ADR-003"): stock Raspberry Pi OS Lite for bring-up, the product's own image built with `rpi-image-gen`. Nothing of it is implemented. |
| Last updated | 2026-10-08 |
| Applies to | Raspberry Pi 4 Model B, CM4, Raspberry Pi 5, CM5. Both a 2-lane and a 4-lane capture configuration are required (REQ-CAP-007); which platform serves each is undecided (ADR-004 OPEN). Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07, second answer). Covers Raspberry Pi OS Lite 64-bit (trixie), `rpi-image-gen` and Buildroot. |
| Verification | Source research of 2026-10-06 and of 2026-10-08 (topic H, H.265/HEVC; topic I, HDMI audio) ([REFERENCES.md](REFERENCES.md)). No image has been built. Nothing has been run on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Traceability | REQ-BLD-001 (DRAFT, NOT STARTED) · REQ-BLD-002 (DRAFT, NOT STARTED) · REQ-CAP-007 (DRAFT) · ADR-003 (ACCEPTED) · ADR-004 (OPEN) · RISK-017 · TEST-BLD-001 (NOT STARTED) · OQ-012 (ANSWERED). Added 2026-10-08: REQ-ENC-001 (H.264 only since 2026-10-08; it read H.264 + H.265 from 2026-10-07) · REQ-ENC-002 (H.265, DEFERRED) · REQ-CAP-006 (HDMI audio, DRAFT) · RISK-022, RISK-025 (not in current scope) · OQ-054 · OQ-105, OQ-107 (not in current scope) · OQ-113 · OQ-114 |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 2, 17, 22, 23, 25 |

This document answers the Rule 25 question **"How is the system built?"**. Today the answer is that it is not built: no build has been set up, run or tested. The document records what the 2026-10-06 source research established about the three candidate build paths, and the plan decided in ADR-003 (ACCEPTED 2026-10-07).

**Owner input of 2026-10-07.** Asked about the OS and image build (OQ-012), the owner answered "which is best i need by own one". This is recorded as REQ-BLD-002 (DRAFT): the product runs a project-built OS image of its own, not an unmodified stock distribution image. The choice of build tool was again delegated to Claude's recommendation, which is unchanged: Raspberry Pi OS Lite for bring-up ([§1](#1-bring-up-os-raspberry-pi-os-lite-64-bit-trixie-2026-10-06)), the product's own image built with `rpi-image-gen` ([§2](#2-production-image-adr-003-accepted-rpi-image-gen)), Buildroot as the documented alternative ([§3](#3-alternative-buildroot)). The owner then accepted ADR-003 on 2026-10-07 ("accept ADR-003"); OQ-012 is ANSWERED.

**Owner decisions of 2026-10-07 (second set) and source research of 2026-10-08.** The owner chose CM4 and CM5 for side-by-side bring-up (ADR-004 stays OPEN), made HDMI audio required (REQ-CAP-006, DRAFT; OQ-004 ANSWERED), and chose H.264 **and** H.265 for recording and streaming (REQ-ENC-001; which output uses which is OQ-103). *(Superseded 2026-10-08: H.264 only; H.265 deferred — see the next paragraph.)* Research topics H and I (2026-10-08) establish which Raspberry Pi OS packages provide H.265 encoding and the audio path, and at which versions. Their effect on the build is in [§1.3](#13-one-image-four-boards-what-differs) (H.265 and audio rows), [§1.4](#14-configtxt) (audio overlay), [§1.5](#15-kernel-modules-and-kernel-configuration-present) (audio kernel options), [§1.6](#16-bring-up-package-list) (packages) and [§1.8](#18-version-limits-of-the-distribution-media-stack-added-2026-10-08) (version limits). Buildroot equivalents were not researched ([§3.6](#36-packages-and-symbols-pacscorder-would-need)).

**Owner decision of 2026-10-08.** The owner answered OQ-103 with "H.264 only for now": REQ-ENC-001 is H.264 only for every output (recording, RTMP, WebRTC), and H.265 is deferred as REQ-ENC-002 (DEFERRED). The build therefore needs no package installed for H.265. The H.265 research is kept as the evidence for REQ-ENC-002 and is labelled "deferred — REQ-ENC-002; not in current scope" in [§1.3](#13-one-image-four-boards-what-differs), [§1.6](#16-bring-up-package-list), [§1.8](#18-version-limits-of-the-distribution-media-stack-added-2026-10-08), [§2.9](#29-proposed-rpi-image-gen-configuration-content), [§3.6](#36-packages-and-symbols-pacscorder-would-need), [§3.8](#38-buildroot-pitfalls-for-pacscorder) and [§7](#7-open-questions-for-the-build). Reasoning from [H-09]: `libx265-215` still enters the image as a dependency of the Raspberry Pi `libavcodec61` whenever `ffmpeg` is installed, so it stays in the package mirror, the SBOM and the licence review ([RELEASE.md](RELEASE.md) §4). The CM4 + CM5 bring-up and the audio requirement are unchanged.

Fact references such as `[G-04]` point to [REFERENCES.md](REFERENCES.md). For entries whose verdict is `CORRECTED`, only the corrected wording is used, and the verdict is noted next to the citation. Open questions (`OQ-NNN`) are in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Text marked *research gap* comes from the `gaps` / `open_questions` lists in [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json) or, for topics H and I, [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json); it is **not** a register fact.

## Contents

- [Current status](#current-status)
- [The build paths at a glance](#the-build-paths-at-a-glance)
- [1. Bring-up OS: Raspberry Pi OS Lite 64-bit (trixie), 2026-10-06](#1-bring-up-os-raspberry-pi-os-lite-64-bit-trixie-2026-10-06)
- [2. Production image (ADR-003, ACCEPTED): rpi-image-gen](#2-production-image-adr-003-accepted-rpi-image-gen)
- [3. Alternative: Buildroot](#3-alternative-buildroot)
- [4. Kernel and firmware versions](#4-kernel-and-firmware-versions)
- [5. Proposed repository layout for build configuration (PROPOSED)](#5-proposed-repository-layout-for-build-configuration-proposed)
- [6. Build test: TEST-BLD-001](#6-build-test-test-bld-001)
- [7. Open questions for the build](#7-open-questions-for-the-build)
- [Verification status](#verification-status)
- [Change history](#change-history)

## Current status

| Item | Status |
|---|---|
| Build configuration in this repository | NOT STARTED. Nothing exists. |
| Bring-up image (stock Raspberry Pi OS Lite) | NOT STARTED. Never flashed or booted for PACSCORDER. |
| Production image (`rpi-image-gen`) | NOT STARTED. Decided in ADR-003 (ACCEPTED 2026-10-07). |
| Buildroot alternative | NOT STARTED. Kept only as a documented alternative. |
| REQ-BLD-001 Reproducible product image build | DRAFT, implementation NOT STARTED. It has no acceptance criteria yet. |
| REQ-BLD-002 Project-owned product OS image | DRAFT (owner statement of 2026-10-07), implementation NOT STARTED. The tool that builds the image is ADR-003. |
| TEST-BLD-001 Image build from clean checkout | NOT STARTED |
| ADR-003 OS and image basis | ACCEPTED (owner, 2026-10-07: "accept ADR-003"; OQ-012 ANSWERED). Owner input of 2026-10-07 recorded as REQ-BLD-002; the build-tool choice was delegated to Claude's recommendation (`rpi-image-gen`), which the owner then accepted. Implementation NOT STARTED. |

## The build paths at a glance

| Path | Role in ADR-003 (ACCEPTED) | Kernel on 2026-10-06 | Pi 5 / CM5 kernel page size | Section |
|---|---|---|---|---|
| Stock Raspberry Pi OS Lite 64-bit (trixie) | Bring-up only. Not the product image: REQ-BLD-002 excludes an unmodified stock image. | 6.18.50 [G-04] | 16K (`bcm2712_defconfig`) [G-20] | [§1](#1-bring-up-os-raspberry-pi-os-lite-64-bit-trixie-2026-10-06) |
| `rpi-image-gen` building from Raspberry Pi OS and Debian packages | Production image: the project's own OS image (REQ-BLD-002) | Whatever the archives carry at build time. Reasoning: it installs from binary packages [G-36] (CORRECTED), and the archive changes over time [G-05], [G-07]. | 16K. Its EROFS slots are tuned to 16K pages on Pi 5/CM5 [G-41]. | [§2](#2-production-image-adr-003-accepted-rpi-image-gen) |
| Buildroot 2026.08 | Documented alternative; would also produce a project-owned image (REQ-BLD-002) | 6.12.61 [E-06] | 4K, forced by a config fragment [E-09], [G-60] | [§3](#3-alternative-buildroot) |
| Yocto (`meta-raspberrypi`) | Not evaluated (*research gap*, topic G) | — | — | OQ-012 |

---

## 1. Bring-up OS: Raspberry Pi OS Lite 64-bit (trixie), 2026-10-06

**Status:** ACCEPTED (ADR-003, decision item 1; owner, 2026-10-07). NOT STARTED.

**Purpose (ADR-003, ACCEPTED 2026-10-07; OQ-012 ANSWERED).** Under ADR-003, the first PACSCORDER hardware is to run the unmodified Raspberry Pi OS image, with its kernel and firmware as shipped. The stated aim is to remove every build-system variable while the TC358743 capture path is first tested on hardware. The test procedures that would exercise it are in [TESTING.md](TESTING.md). Because REQ-BLD-002 (DRAFT, owner 2026-10-07) requires the product to run a project-built image, the unmodified stock image is for bring-up only and is never shipped as the product image.

### 1.1 The image

| Property | Value | Facts |
|---|---|---|
| Image file | `2026-10-06-raspios-trixie-arm64-lite.img.xz` | [G-01] |
| Base | Debian 13 "trixie" | [G-01] |
| "latest" link | `https://downloads.raspberrypi.com/raspios_lite_arm64_latest` returned HTTP 302 to this file on 2026-10-06. | [G-01] |
| Size | 550,466,056 bytes compressed (`.img.xz`). Expands to 3,078,619,136 bytes. | [G-33] |
| Kernel | Linux 6.18.50, commit `cff533aec2fa601846766b32ff57204e0a61bed7`. The 2026-09-15 release-notes entry lists the same kernel. | [G-04] |
| Firmware (release notes) | raspberrypi/firmware `6f0881cba8bea8ec24956a5718714730e9936ec5` | [G-04] |
| Kernel packages | Meta-packages `linux-image-rpi-v8` and `linux-image-rpi-2712` (`1:6.18.50-1+rpt1`) pull in `linux-image-6.18.50+rpt-rpi-v8` and `linux-image-6.18.50+rpt-rpi-2712`. The matching `linux-headers` are installed. | [G-06] |
| GPU firmware and bootloader files | `raspi-firmware 1:1.20260915-1` | [G-06] |
| EEPROM updater | `rpi-eeprom 28.33-1` | [G-06] |
| Image generator | pi-gen commit `b2cd98a9ee08a31ef7d0110ed09f2492bbd972ec`, stage2. pi-gen stage 2 "produces the Raspberry Pi OS Lite image". | [G-34], [G-35] |
| SBOM | SPDX-2.3 (`.sbom.xz`), generated by syft-1.54.0 | [G-34] |
| Installed packages | 633 (`ii`) | [G-32] |
| Imager device tags | `pi5-64bit`, `pi4-64bit`, `pi3-64bit`. CM4 and CM5 are not named separately, though the image includes CM4 and CM5 device trees. | [G-33] |

**Pin the dated file, not the "latest" link.** The redirect target changes with each release. Reasoning from [G-01] and [G-05]: the kernel series changed between images within the same OS release. Every PACSCORDER test record must name the dated image.

**Release context**

- Raspberry Pi OS trixie was announced on 2025-10-02. The first trixie Lite image file is dated 2025-10-01 [G-02].
- The default kernel moved from 6.12 to 6.18 partway through the trixie release. Release notes list 6.12.75 on 2026-04-13 and 6.18.34 on 2026-06-18; the re-spun 2026-04-21 image still had 6.12.75 [G-05].
- Debian 13 was released on 2025-08-09. Full support runs to 2028-08-09 and LTS to 2030-06-30 [G-03]. These are Debian's dates. No Raspberry Pi support horizon for its own kernel, firmware and `+rpt` packages was found (*research gap*, topic G; OQ-070).
- In-place upgrades between major versions are not recommended or supported; the documentation recommends a clean install [G-55] (CORRECTED). A new major Raspberry Pi OS release follows each new major Debian release [G-56].

### 1.2 What the Lite image already contains

| Component | Version | Relevance to PACSCORDER | Facts |
|---|---|---|---|
| `systemd` | 257.13-1~deb13u1 | Init system | [G-32] |
| `cloud-init` | 25.2-1~bpo13+1+rpt20 | First-boot configuration. A candidate for removal in production (OQ-071). | [G-32] |
| `netplan.io`, `network-manager` | 1.1.2-7+rpt1, 1.52.1-1+rpt4 | Networking | [G-32] |
| `openssh-server` | 1:10.0p1-7+deb13u4 | Remote access during bring-up | [G-32] |
| `v4l-utils` | 1.30.1-1 | Ships `v4l2-ctl`, `media-ctl`, `v4l2-compliance` and `cec-ctl`. **Preinstalled.** | [G-24] |
| `libcamera0.7`, `rpicam-apps-core` | `rpicam-apps-core` 1.13.0-1; `libcamera0.7` version not recorded in the register | Not on the TC358743 path. ADR-001 (ACCEPTED) uses V4L2 / Media Controller. That libcamera does not support the TC358743 is reported by Raspberry Pi engineers in a community-tier source [C-41]. | [G-32], [C-41] |
| `rpi-connect-lite` | 2.13.0 | Raspberry Pi Connect client. A candidate for removal in production (OQ-071). | [G-32], [G-53] |
| `rpi-update` | 20230904 | Pre-release updater. Must not be used on PACSCORDER units ([§1.7](#17-updates-apt-full-upgrade-and-rpi-update)). | [G-10] |
| GStreamer | not installed | Must be added ([§1.6](#16-bring-up-package-list)) | [G-32] |
| `i2c-tools` | not installed | Must be added ([§1.6](#16-bring-up-package-list)) | [G-25] |

Passwordless `sudo` has been disabled by default since the 2026-04-13 release [G-57].

### 1.3 One image, four boards: what differs

The Lite image ships both the `rpi-v8` kernel (bcm2711, 4K pages) and the `rpi-2712` kernel (16K pages). Their module trees include `tc358743`, `unicam`, `rp1-cfe` and `bcm2835-codec`, and the image has device trees for Pi 4B, CM4, Pi 5, CM5 and CM5 Lite. The firmware loads `kernel_2712.img` on BCM2712 and `kernel8.img` otherwise, so one image can boot all four candidates [G-71] (reasoning-tier entry, CORRECTED). The boards still differ in more than storage:

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Kernel build | `bcm2711_defconfig`, `CONFIG_LOCALVERSION="-v8"` [G-20] | same as Pi 4 | `bcm2712_defconfig`, `"-v8-16k"`, `CONFIG_ARM64_16K_PAGES=y` [G-20] | same as Pi 5 |
| TC358743 overlay | `tc358743` [G-12] | `tc358743`; its `4lane` parameter is for Compute Module CAM1 only [G-12] | `tc358743-pi5` [G-13] | `tc358743-pi5` [G-13] |
| Default capture model | Legacy Unicam in video-node mode unless the `media-controller` parameter is given [G-12], [G-21] | same as Pi 4 | RP1 CFE, Media Controller only [G-71] | same as Pi 5 |
| CSI-2 lanes at the camera interface(s) | One 15-pin connector, 2 data lanes [C-01] | 2-lane CSI0 and 4-lane CSI1 (CM4 IO Board connectors) [C-03]; the `tc358743` `4lane` parameter applies to Compute Module CAM1 only [G-12] | Two 22-pin ports, each backed by a 4-lane transceiver [C-04]. `tc358743-pi5` has a `4lane` parameter [G-13]; whether it works on the Pi 5 connectors is NEEDS VERIFICATION (*research gap*, topic G) | Two 4-lane MIPI interfaces [C-05] |
| H.264 encode | Hardware `bcm2835-codec` [G-71] | same as Pi 4 | Software only [G-22] | same as Pi 5 |
| H.265 encode *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | Software only: `bcm2835-codec` has no HEVC encoder [D-24]; Debian x265 4.1-2 [H-01], [H-02]; Cortex-A72 (BCM2711) has no DotProd [H-05] | same as Pi 4 | Software only [D-31]; same x265 package; Cortex-A76 (BCM2712) has DotProd [H-05] | same as Pi 5 |
| HDMI audio kernel options *(added 2026-10-08)* | `bcm2711_defconfig`: `CONFIG_SND_SIMPLE_CARD=m`, `CONFIG_SND_BCM2835_SOC_I2S=m`, `CONFIG_SND_DESIGNWARE_I2S=m`, `CONFIG_SND_DESIGNWARE_PCM=y`; packaged `rpi-v8` kernel has `CONFIG_SND_SOC_SPDIF=m` [I-12] | same as Pi 4 | `bcm2712_defconfig`: the same options; packaged `rpi-2712` kernel has `CONFIG_SND_SOC_SPDIF=m` [I-12] | same as Pi 5 |
| HDMI audio CPU I2S *(added 2026-10-08)* | Not researched separately in topic I: NEEDS VERIFICATION | `bcm2835-i2s` on GPIO 18–21 [I-10] | Not researched separately in topic I: NEEDS VERIFICATION | RP1 I2S1 on GPIO 18–21; capture unconfirmed (OQ-054) [I-07], [I-08] |
| Bring-up board (owner, 2026-10-07, second answer) *(added 2026-10-08)* | No (documented candidate) | **Yes** | No (documented candidate) | **Yes** |
| Storage and provisioning | SD [G-71] | eMMC flashed with `rpiboot` [G-71], [G-47] | SD [G-71] | eMMC flashed with `rpiboot` [G-71], [G-47] |

The rows citing [G-71] are reasoning-tier. Which Compute Module variant PACSCORDER would use, and its boot storage, are **UNKNOWN — VERIFICATION REQUIRED** (OWNER DECISION REQUIRED; OQ-018). The boards' own storage, Ethernet and USB facts are not in the register (DATASHEET REQUIRED; OQ-098). The image also ships a CM5 Lite device tree [G-71]; how a Lite variant is provisioned is not in the register (NEEDS VERIFICATION).

The CSI-2 lane count and connector of the PACSCORDER board are **UNKNOWN — VERIFICATION REQUIRED** (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED; OQ-021). The platform choice is ADR-004 (OPEN). Per-platform overlay configuration is in [DEVICE_TREE.md](DEVICE_TREE.md), and the receive path is in [CSI_PIPELINE.md](CSI_PIPELINE.md).

**Two capture configurations (owner, 2026-10-07).** REQ-CAP-007 (DRAFT) requires both a 2-lane and a 4-lane configuration, so ADR-004 is now a choice per configuration. 2-lane candidates: Pi 4 Model B [C-01], CM4 CAM0 [C-03], or a 2-lane bridge board on any port. 4-lane candidates: CM4 CAM1 [C-03], Pi 5 [C-04], CM5 [C-05]. Depending on that choice, the two configurations may run on different kernel builds (`rpi-v8` and `rpi-2712`, table above). ADR-003 gives this as a reason to prefer one Raspberry Pi OS based image basis for all four boards [G-71] (reasoning tier, CORRECTED). Whether one bridge-board design can serve both configurations is OQ-021.

### 1.4 config.txt

| Topic | Fact |
|---|---|
| Location | Raspberry Pi OS reads `config.txt` from the boot partition, which is mounted at `/boot/firmware/`. `raspi-config` (trixie branch) uses `/boot/firmware/config.txt` when that file exists. Otherwise it falls back to `/boot/config.txt`, the legacy location used before Bookworm [G-11] (CORRECTED). **PACSCORDER documents and scripts use `/boot/firmware/config.txt`.** |
| Overlay syntax | `dtoverlay=tc358743,<param>=<val>` [G-12]. Appending `,cam0` selects connector 0 on boards with two connectors [C-39] (CORRECTED). These are the only attested forms. Bare boolean parameters other than `,cam0` (for example `,4lane` or `,media-controller` without `=<val>`), several parameters on one line, and the loader's handling of an unknown parameter are **NEEDS VERIFICATION** (OQ-100). |
| `tc358743` parameters | `4lane` (Compute Module CAM1 only), `link-frequency` (only `297000000` or `486000000`; default `486000000`), `media-controller` (default off), `cam0` [G-12] |
| `tc358743-pi5` parameters | `4lane`, `link-frequency` (`297000000` or `486000000`), `cam0`. There is no `media-controller` parameter [G-13]. |
| Pi 5 remapping | `overlay_map` maps `tc358743` to `tc358743-pi5` on bcm2712 [E-43]. |
| Audio overlay | `tc358743-audio`, whose only parameter is `card-name` [G-14]. Whether it works on Pi 5/CM5 is open (OQ-054); [G-14] is not read as confirming that. *(Added 2026-10-08; HDMI audio is required, REQ-CAP-006.)* Official documentation says audio needs `tc358743-audio` in addition to `tc358743` [C-37]; a Raspberry Pi engineer stated that it "*requires* dtoverlay=tc358743 to be loaded too" [I-16] (reported; community source). On CM5 `overlay_map` has no entry for it, so the firmware does not block it and it loads under its own name [I-05], [I-06]. The README's "tc358743-fast" is a stale name: the overlays Makefile builds only `tc358743.dtbo`, `tc358743-audio.dtbo` and `tc358743-pi5.dtbo` [I-04]. The overlay claims GPIO 18–21 [I-30]; the `pwm`, `pwm-2chan` and `gpio-ir` overlays default to GPIO 18, and `audremap` offers `pins_18_19` on BCM2711 [I-31], so the project `config.txt` must not enable those on the same pins (reasoning; OQ-114). The `config.txt` line form for loading both overlays is NEEDS VERIFICATION (OQ-100). |
| Per-model sections | The `[pi4]`, `[pi5]`, `[cm4]` and `[cm5]` filters let one `config.txt` hold the settings for each model [G-71] (reasoning-tier entry, CORRECTED). Whether a `[pi4]` section also matches a CM4 is NEEDS VERIFICATION (OQ-100). |
| `camera_auto_detect` | Official documentation requires `camera_auto_detect=0` for the listed camera-sensor overlays [G-15]. It does not state this for the TC358743. Disabling it is prudent but not a documented requirement [C-39] (CORRECTED). Open: OQ-072. |

- **Lines PACSCORDER will add:** see [DEVICE_TREE.md](DEVICE_TREE.md). The choice between Media Controller and video-node mode is ADR-006 (PROPOSED). The board-dependent values (`4lane`, `cam0`) are **UNKNOWN — VERIFICATION REQUIRED** (OQ-018, OQ-021). Reasoning from [G-12] and [G-13]: the 2-lane and 4-lane configurations required by REQ-CAP-007 differ at least in the `4lane` setting, so the project `config.txt` must carry the setting that matches each configuration's wiring (see [DEVICE_TREE.md](DEVICE_TREE.md)).
- **Stock contents:** whether the stock image's `config.txt` already enables any TC358743 overlay is not recorded in the source register. The research verifier noted that none is enabled by default (*research critique*, topic G; not a register fact). Check it at first boot and record the file in [TESTING.md](TESTING.md).

### 1.5 Kernel modules and kernel configuration present

The table below is the kernel configuration of the Raspberry Pi default branch `rpi-6.18.y` [G-16]–[G-19]. That the shipped 6.18.50 kernel uses these settings is reasoning: it is a 6.18 kernel [G-04], and the image does ship `tc358743.ko.xz` [G-16] and module trees with `unicam`, `rp1-cfe` and `bcm2835-codec` [G-71] (CORRECTED). On hardware, check the shipped kernel's own configuration: KERNEL SOURCE INSPECTION REQUIRED (exact method NEEDS VERIFICATION, OQ-101). Whether `tc358743.c` in the shipped 6.18.50 kernel [G-04] differs from the source inspected at the `rpi-6.18.y` tip 6.18.55 [E-37] is OQ-097 (KERNEL SOURCE INSPECTION REQUIRED).

| Kconfig symbol(s) | Value in `bcm2711_defconfig` and `bcm2712_defconfig` | What it gives PACSCORDER | Facts |
|---|---|---|---|
| `CONFIG_VIDEO_TC358743` | `=m`. `tc358743.ko.xz` actually ships for both the `rpi-v8` and the `rpi-2712` kernel. | The bridge driver | [G-16] |
| `CONFIG_VIDEO_TC358743_CEC` | not set | No CEC support (OQ-016) | [E-39] |
| `CONFIG_VIDEO_BCM2835_UNICAM_LEGACY`, `CONFIG_VIDEO_BCM2835_UNICAM` | `=m`, `=m` | Pi 4/CM4 CSI-2 receiver. The downstream module is `bcm2835-unicam-legacy.ko`. It matches `brcm,bcm2835-unicam`, which forces Media Controller mode, and `brcm,bcm2835-unicam-legacy`, where the mode follows the `media_controller` module parameter (default 0). The mainline driver matches only `brcm,bcm2835-unicam-upstream`. | [G-17], [G-21] |
| `CONFIG_VIDEO_RP1_CFE`, `CONFIG_VIDEO_RP1_CFE_DOWNSTREAM` | `=m`, `=m` | Pi 5/CM5 CSI-2 receiver. The DT nodes use `raspberrypi,rp1-cfe`, which is served by the downstream driver ([§3.7](#37-kernel-kconfig-symbols-and-how-their-meaning-changed)). | [G-17], [E-44] |
| `CONFIG_VIDEO_RASPBERRYPI_PISP_BE`, `CONFIG_VIDEO_RPI_HEVC_DEC` | `=m`, `=m` | Pi 5 ISP back end and HEVC decoder. Not on the PACSCORDER capture path. | [G-17] |
| `CONFIG_BCM2835_VCHIQ`, `CONFIG_VIDEO_BCM2835`, `CONFIG_VIDEO_CODEC_BCM2835`, `CONFIG_VIDEO_ISP_BCM2835` | `=y`, `=m`, `=m`, `=m` | `bcm2835-codec`, the Pi 4/CM4 hardware H.264 encoder. Building it for bcm2712 does **not** give Pi 5 a hardware encoder. | [G-18], [G-22] |
| `CONFIG_DMABUF_HEAPS`, `_SYSTEM`, `_CMA`; `CONFIG_CMA`, `CONFIG_DMA_CMA` | `=y` | DMA-BUF heaps ([DMA.md](DMA.md)) | [G-19] |
| `CONFIG_CMA_SIZE_MBYTES` | `5` | Not the CMA size in use. The CMA region in use comes from the device tree `linux,cma` node, 64 MB in the base device trees. Whether the firmware or other overlays change it at boot is not verified [E-47] (CORRECTED); HARDWARE TEST REQUIRED. | [G-19], [E-47] (CORRECTED) |
| `CONFIG_PREEMPT` | `=y` | Preemptible kernel | [G-19] |
| `CONFIG_MEDIA_SUPPORT`; `CONFIG_MODULE_COMPRESS_XZ` | `=m`; `=y` | Media drivers are XZ-compressed loadable modules. | [E-49] |
| `CONFIG_I2C_BCM2835`, `CONFIG_I2C_MUX_PINCTRL` | `=m`, `=m` | I2C controller, and the `i2c-mux-pinctrl` mux that provides the camera I2C bus `i2c_csi_dsi` on Pi 4B and CM4 [C-24] | [E-39], [C-24] |
| *(Added 2026-10-08.)* `CONFIG_SND_SIMPLE_CARD`, `CONFIG_SND_BCM2835_SOC_I2S`, `CONFIG_SND_DESIGNWARE_I2S`, `CONFIG_SND_DESIGNWARE_PCM` | `=m`, `=m`, `=m`, `=y` in both defconfigs | HDMI audio path: the `simple-audio-card`, the CM4 CPU I2S (`bcm2835-i2s`) and the CM5 RP1 I2S (`dwc-i2s`) | [I-12] |
| *(Added 2026-10-08.)* `CONFIG_SND_SOC_SPDIF` | Not set directly; selected by `CONFIG_SND_RP1_AUDIO_OUT=m`. The packaged `rpi-v8` and `rpi-2712` 6.18.50 kernels both contain `CONFIG_SND_SOC_SPDIF=m`. | The `linux,spdif-dir` stub codec the audio overlay uses (module `snd-soc-spdif-rx`) | [I-12], [I-11] |
| *(Added 2026-10-08.)* `CONFIG_VIDEO_TC358743_CEC` in the packaged kernels | Not set in the packaged 6.18.50 `rpi-v8` and `rpi-2712` kernels either | Without CEC (and without an INT pin) the driver polls every 1000 ms, so an audio sample-rate change takes up to about 1 s to reach its control | [I-22] |

Whether each module loads and binds on PACSCORDER hardware: **BLOCKED — HARDWARE REQUIRED** (TEST-PLT-001, TEST-DRV-001).

### 1.6 Bring-up package list

These versions are from the trixie archive indexes as read on 2026-10-06. The archive changes over time (RISK-017), so record the versions actually installed.

| Package | Version (trixie, arm64) | Origin | In the 2026-10-06 Lite image | Provides | Facts |
|---|---|---|---|---|---|
| `v4l-utils` | 1.30.1-1 | Debian | **Preinstalled** | `v4l2-ctl`, `media-ctl`, `v4l2-compliance`, `cec-ctl` | [G-24] |
| `i2c-tools` | 4.4-2 | Debian | Not installed | `i2cdetect`, `i2cget`, `i2cset`, `i2cdump`, `i2ctransfer` | [G-25] |
| `gstreamer1.0-plugins-good` | 1.26.2-1+deb13u2 | Debian; not overridden by the Raspberry Pi archive | Not installed | `libgstvideo4linux2` (`v4l2src` and the probed V4L2 M2M elements), `rtp`, `rtpmanager`, `isomp4`, `matroska`, `flv` | [G-26], [G-32] |
| `gstreamer1.0-plugins-bad` | 1.26.2-3+rpt4+deb13u3 | Raspberry Pi build. It sorts higher than Debian's 1.26.2-3+deb13u3. | Not installed | `rtmp2`, `webrtc`, `webrtcdsp`, `srtp`, `dtls`, `sctp`, `v4l2codecs`, `kms`. *(Added 2026-10-08.)* Also `x265` (`x265enc`, `libgstx265.so`; the Raspberry Pi build still Build-Depends on `libx265-dev`; installed with the package but not used in the current scope — H.265 deferred, REQ-ENC-002); `voaacenc` (`libgstvoaacenc.so`) and `opusparse`, with dependencies on `libvo-aacenc0` (>= 0.1.3) and `libopus0` (>= 1.1), but no `fdkaacenc`; `srt` and `mpegtsmux` (`libgstsrt.so`, `libgstmpegtsmux.so`, as shipped in trixie). | [G-27], [G-32], [H-11], [H-12] (CORRECTED), [H-30], [I-44] |
| `gstreamer1.0-nice` | 0.1.22-1 | Debian | Not installed | `libgstnice`. `webrtcbin` needs it at runtime. | [G-28] |
| `gstreamer1.0-plugins-base` | 1.26.2-1+rpt3+deb13u2 | Raspberry Pi override | Not installed | GStreamer base plugins. GStreamer core is `libgstreamer1.0-0` 1.26.2-2. *(Added 2026-10-08.)* Includes the `opus` plugin (`opusenc`, `libgstopus.so`) and depends on `libopus0` (>= 1.1). | [G-31], [G-32], [I-45] |
| `ffmpeg` | 8:7.1.5-0+deb13u1+rpt2 | Raspberry Pi build. Its higher epoch wins over Debian's 7:7.1.5-0+deb13u1. | Not recorded in the register | FFmpeg with a Raspberry Pi patch that adds DMABUF input to the V4L2 M2M encoder. *(Added 2026-10-08.)* Configured with `--enable-libx265` in every flavour, and with `--enable-libx264` and `--enable-libsrt` in the full build [H-08]. `libavcodec61` (same version) depends on `libx265-215` (>= 4.1), `libx264-164`, `libopus0` and `libmp3lame0`, not on `libfdk-aac`; it has the `libx265` encoder, the native `aac` encoder and the `libopus` wrapper, and no `libfdk_aac`. Linking `libx264` and `libx265` makes it a GPL build. Reasoning from [H-09]: installing `ffmpeg` therefore still installs `libx265-215`, although H.265 is deferred (REQ-ENC-002). | [G-29], [D-45], [H-08], [H-09], [I-39], [I-41] |
| `x264` | 2:0.164.3108+git31e19f9-2+b1 | Debian. The Raspberry Pi archive has no `x264` or `libx264-164`. | Not recorded in the register | x264 H.264 software encoder | [G-30] |
| `x265`, `libx265-215` *(added 2026-10-08; the `x265` CLI only if REQ-ENC-002 is re-activated)* | 4.1-2 | Debian. The Raspberry Pi archive has no x265 source or binary package, so Raspberry Pi OS uses Debian's unchanged. | Not recorded in the register | `x265` CLI and the H.265 library. Debian's build enables arm64 assembly and links Main, Main10 and Main12 into the one library. `libx265-215` is a dependency of `libavcodec61` [H-09], so it is installed with `ffmpeg` even though H.265 is deferred (REQ-ENC-002); the `x265` CLI is not installed in the current scope. | [H-01], [H-02], [H-03], [H-09] |
| `gstreamer1.0-alsa` *(added 2026-10-08)* | 1.26.2-1+rpt3+deb13u2 | Version as recorded in [I-45]. Reasoning: the `+rpt3` suffix matches the Raspberry Pi `gstreamer1.0-plugins-base` override [G-31]. | Not recorded in the register | `alsasrc` (HDMI audio capture in GStreamer) | [I-45] |
| `gstreamer1.0-libav` *(added 2026-10-08)* | 1.26.2-1+deb13u1 | Debian; not rebuilt by Raspberry Pi | Not recorded in the register | `libgstlibav.so`, including `avenc_aac`, which wraps FFmpeg's native AAC encoder. Depends on `libavcodec61` (>= 7:7.1.4), which the Raspberry Pi 8:7.1.5 build satisfies and wins on epoch. | [I-46] |
| `libopus0` *(added 2026-10-08)* | 1.5.2-2 | Debian; not rebuilt by Raspberry Pi | Not recorded in the register | Opus library behind FFmpeg `libopus` and GStreamer `opusenc` | [I-45], [I-39] |
| `libvo-aacenc0` *(added 2026-10-08)* | Version not in the register; `gstreamer1.0-plugins-bad` depends on (>= 0.1.3) | Not in the register | Not recorded in the register | AAC library behind `voaacenc` | [I-44] |
| alsa-lib *(added 2026-10-08)* | 1.2.14 (the trixie version) | Binary package name not in the register (NEEDS VERIFICATION) | Not recorded in the register | ALSA library. Its `hw` plugin switches each newly opened PCM to monotonic timestamps when the kernel PCM protocol is 2.0.9 or later. | [I-34] |
| `fdk-aac` (`libfdk-aac2t64`, `libfdk-aac-dev`, `aac-enc`) *(added 2026-10-08; listed so that it is **not** installed)* | 2.0.3-1 | Debian **non-free**; not in the Raspberry Pi archive | Not installed. PROPOSED: never installed in a PACSCORDER image ([RELEASE.md](RELEASE.md) §4) | Licence "Fraunhofer-FDK-AAC-for-Android", which Debian says "is incompatible with any version of the GNU GPL"; it grants no patent licence. Not used by the Raspberry Pi FFmpeg or GStreamer packages. | [I-39], [I-43], [I-44] |

Not established by the source register (**NEEDS VERIFICATION**):

- **`x264enc` package.** The GStreamer `x264enc` element is in GStreamer Ugly Plug-ins [D-40]. The Debian trixie package name and version that provide it are not in the register.
- **FFmpeg build options.** Whether Raspberry Pi's `ffmpeg` build is configured with `--enable-gpl` and `libx264` is not in the register. `libx264` requires `--enable-gpl` [D-42]. *(Superseded 2026-10-08: the Raspberry Pi FFmpeg source package is configured with `--enable-libx265`, and its full build with `--enable-libx264` [H-08]; `libavcodec61` links both, which makes it a GPL build [H-09], [I-39]. Research notes that the encoder list behind [I-39] was inferred from package dependencies, so confirming it on the image itself remains a BUILD TEST REQUIRED item — research gap, topic I.)*
- **Dependency upgrades.** Whether installing these packages also upgrades packages that are already installed is not known. Record versions after installing.
- *(Added 2026-10-08.)* **FFmpeg ALSA input.** Whether the Raspberry Pi FFmpeg build includes the ALSA input device described in [I-38] is not in the register: BUILD TEST REQUIRED.
- *(Added 2026-10-08.)* **`arecord`.** Which package provides `arecord`, and whether it is preinstalled in the Lite image, is not in the register (NEEDS VERIFICATION). A 2019 forum test used it against the `tc358743` card [I-16] (reported; community source).
- *(Added 2026-10-08.)* **Debian non-free in the image's apt sources.** Whether the 2026-10-06 image enables non-free is a research open question (topic I). It matters only if `fdk-aac` were wanted, which this document does not propose. BUILD TEST REQUIRED.
- *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* **x265 SIMD in the Debian binary.** Whether the arm64 `libx265-215` contains the Neon DotProd kernels, and whether CM5 reports `asimddp`, is OQ-105 (BUILD TEST REQUIRED; HARDWARE TEST REQUIRED). x265 enables those kernels only when `AT_HWCAP` reports ASIMDDP [H-04].

**Install procedure (bring-up). NOT YET RUN ON PACSCORDER HARDWARE.** The `apt install` form is from [G-46] and [G-47], `apt update` is from [G-08], and the package names are from [G-25]–[G-31].

```bash
sudo apt update
sudo apt install i2c-tools gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-nice ffmpeg x264
```

**Additional packages for HDMI audio (added 2026-10-08; until H.265 was deferred the same day this was "H.265 and HDMI audio" and also installed `x265`). NOT YET RUN ON PACSCORDER HARDWARE.** The `apt install` form is as above; the package names are from [I-45] (`gstreamer1.0-alsa`) and [I-46] (`gstreamer1.0-libav`). Reasoning from the dependencies recorded in [H-09], [I-39], [I-44] and [I-45]: `libx265-215`, `libopus0` and `libvo-aacenc0` arrive as dependencies of `ffmpeg`/`libavcodec61`, `gstreamer1.0-plugins-base` and `gstreamer1.0-plugins-bad` from the first command; `libx265-215` therefore arrives even though H.265 is deferred (REQ-ENC-002). `x264enc` still needs the plugins-ugly package, whose name is NEEDS VERIFICATION (above).

```bash
sudo apt install gstreamer1.0-alsa gstreamer1.0-libav
```

Only if REQ-ENC-002 is re-activated (deferred — REQ-ENC-002; not in current scope): add `x265`, the optional H.265 command-line encoder [H-01], to that command.

The package that provides the `gst-launch-1.0` command-line tool is not in the register (**NEEDS VERIFICATION**).

After installing, record the full installed package list with the test results. The exact command to export it is **NEEDS VERIFICATION** (OQ-101): it is not attested in the register. The image's own `.info` manifest lists packages in `ii` state [G-32].

### 1.7 Updates: `apt full-upgrade` and `rpi-update`

| Mechanism | What it does | Facts |
|---|---|---|
| `sudo apt update` then `sudo apt full-upgrade` | The official update procedure. It "also updates your Linux kernel and firmware". Normal firmware updates arrive through the `raspi-firmware` package. | [G-08] |
| Kernel series moves | The default kernel moved from 6.12 to 6.18 within trixie [G-05]. The archive keeps older versioned kernel images (6.12.25 to 6.18.50) and has no `raspberrypi-kernel` package [G-07]. | [G-05], [G-07] |
| `rpi-update` | Downloads the latest **pre-release** kernel, modules, device tree files and VideoCore firmware. On Pi 4 and Pi 5 it also updates the EEPROM bootloader. The documentation warns that pre-release firmware "can introduce instability, rendering your system unreliable or unbootable". It is installed by default. | [G-09], [G-10] |

**PROPOSED bring-up rules** (for the bring-up OS of ADR-003, ACCEPTED; the rules themselves are reasoning from [G-05], [G-08], [G-09] and are not part of the accepted decision):

1. Start every bring-up unit from the dated 2026-10-06 image ([§1.1](#11-the-image)).
2. Never run `rpi-update` on a PACSCORDER unit.
3. Do not run `apt full-upgrade` during a test campaign. If it is run, treat the result as a new software version and record the new kernel and firmware versions.
4. Record the kernel version, firmware version, EEPROM bootloader version and `config.txt` with every test result in [TESTING.md](TESTING.md). The commands to read these are **NEEDS VERIFICATION** (OQ-101); they are not attested in the register.

Field units are a separate question from bring-up: whether an update may be installed, or a reboot triggered, while a recording or stream is active is not decided (OWNER DECISION REQUIRED; OQ-094).

**EEPROM bootloader.** `rpi-eeprom` 28.33-1 is installed [G-06]. The research reports two further items, neither of which is a register fact (*research gap*, topic G):

- the `rpi-eeprom-update` service can update the EEPROM at boot;
- a bootloader `FREEZE_VERSION` setting exists.

Both are **NEEDS VERIFICATION** (OQ-071). See [RELEASE.md](RELEASE.md) for why the release process must pin and record the EEPROM version.

### 1.8 Version limits of the distribution media stack (added 2026-10-08)

**Deferred — REQ-ENC-002; not in current scope** (owner, 2026-10-08: "H.264 only for now", OQ-103). Every row of the table below is tied to an H.265 question (OQ-104 to OQ-108, RISK-025), so with H.264 only no package has to be carried outside the distribution for these gaps (reasoning). The section is kept as evidence for REQ-ENC-002.

H.265 (REQ-ENC-001 as decided on 2026-10-07; REQ-ENC-002, DEFERRED, since 2026-10-08) and HDMI audio (REQ-CAP-006) depend on features that the trixie package versions above either lack or have only in a later upstream release. Under ADR-003 (ACCEPTED) the product image installs Raspberry Pi OS and Debian binary packages [G-36] (CORRECTED), so each gap below is either accepted, worked around in the application, or closed by carrying a newer package outside the distribution.

| Component (version in the image) | What that version lacks | Fixed in | Facts | Open |
|---|---|---|---|---|
| GStreamer `flvmux` (1.26.2) | H.265 on its video sink pad, so no HEVC into FLV/RTMP | `eflvmux`, first in the 1.28 branch | [H-27], [F-34] (CORRECTED) | OQ-107, RISK-025 |
| GStreamer `x265enc` (1.26.2) | Latency computed from the encoder's real parameters; it reports a hard-coded 5 frames unless `tune=zerolatency` | 1.26.8; not backported by Debian or Raspberry Pi | [H-15] | OQ-104 |
| GStreamer `rtph265pay` (1.26.2) | profile-id, tier-flag and level-id in its output caps | 1.26.4 | [H-31] | OQ-108 |
| x265 (4.1-2) | The 4.2 (NEON/SVE, "8% faster encoding speed compared to v4.1") and 4.3 AArch64 speed-ups; only their Neon parts can help Cortex-A72/A76 | 4.2 (2026-04-19), 4.3 (2026-07-31) | [H-07] | OQ-105 |
| FFmpeg (7.1.5) | Nothing for HEVC in enhanced FLV (it has that since 6.1); Enhanced FLV v2 (multitrack, more codecs) arrived in 8.0 | 8.0 (v2 only) | [H-25], [H-26] | OQ-106 |

**Build consequences (reasoning from the table and [G-36], [G-41], [G-44]).**

- Carrying a newer GStreamer or x265 means building and maintaining packages outside both archives, including their security updates (ADR-003; OQ-070, OQ-105, OQ-107). Such packages would also have to enter the PACSCORDER package mirror and the SBOM, or the build is not reproducible (RISK-017; [§2.6](#26-reproducibility-caveat-risk-017)).
- The alternative paths that need no new package are FFmpeg 7.1.5 for HEVC RTMP [H-26] and SRT/MPEG-TS, whose components the distribution stacks already contain [H-30] (OQ-076, OQ-107). Which one is used is an ADR-007 decision.
- None of these choices has been built or tested: BUILD TEST REQUIRED (TEST-BLD-001, TEST-STR-001).

---

## 2. Production image (ADR-003, ACCEPTED): rpi-image-gen

**Status:** ACCEPTED (ADR-003, decision item 2; owner, 2026-10-07). NOT STARTED. No `rpi-image-gen` configuration exists.

This is the project's own OS image that REQ-BLD-002 (DRAFT, owner 2026-10-07) requires. The owner delegated the choice of build tool to Claude's recommendation (`rpi-image-gen`) and then accepted ADR-003 on 2026-10-07 ("accept ADR-003"; OQ-012 ANSWERED).

### 2.1 What rpi-image-gen is

| Topic | Fact | Facts |
|---|---|---|
| Purpose | Announced on 2025-03-21. It "is designed to generate highly customised software images for Raspberry Pi devices" and is "an alternative to pi-gen". Like pi-gen, it installs a Debian system from Raspberry Pi OS and Debian binary packages. It produces an SBOM for every build. | [G-36] (CORRECTED) |
| Repository, licence, releases | `github.com/raspberrypi/rpi-image-gen`, created 2024-12-09, BSD-3-Clause. Releases: v1.0.0 (2025-09-01); v2.0.0-rc.1 (2025-09-05; there is no final v2.0.0); v2.1.0 to v2.7.0 (2026-01-22 to 2026-06-26); **v2.8.0 (2026-08-13, latest)**. | [G-37] |
| Configuration model | Declarative YAML configs and composable layers. Layers carry `X-Env` metadata (variables, validation, dependencies, Provides/Requires) and use shell hooks. v2.7.0 made `X-Env-Layer-Version` mandatory. v2.8.0 added a trait registry (for example `hw:soc:bcm2712`) with conditional `Requires`, and dropped INI configs. | [G-38] |
| Stated capabilities | "designed to create reproducible operating system artefacts"; "Generate Software Bill of Materials and CVE reports"; "Integrate with rpi-sb-provisioner to automatically set up signed boot and encrypted filesystems". | [G-39] |
| Internals | Uses `bdebstrap`, `mmdebstrap`, `genimage` and `podman unshare`. Needs `CAP_SYS_ADMIN`. | [G-39] |
| Breaking changes in minor releases | v2.3.0 changed the A/B default rootfs to EROFS. v2.4.0 disabled passwordless sudo by default. v2.7.0 renamed `rpi-user-credentials` to `device-user-admin`. v2.8.0 removed INI config support. | [G-44] |
| Relation to pi-gen | pi-gen is the "Tool used to create the official Raspberry Pi OS images" [G-35]. The Compute Module documentation still points to pi-gen (not `rpi-image-gen`) for customising the OS image [G-47]. | [G-35], [G-47] |

**Consequence (reasoning from [G-44]):** a PACSCORDER build must pin one `rpi-image-gen` release tag. Every tag upgrade must be treated as a build change and re-run TEST-BLD-001.

### 2.2 Build host requirement

- The only supported hosts are **native Debian Bookworm or Trixie on arm64**. Containers and non-arm64 (QEMU) hosts are "not formally supported" [G-39].
- Reasoning from [G-39]: an x86-64 CI runner is outside the supported configuration. To stay within it, the build host must be a native arm64 machine running Debian Bookworm or Trixie.
- Which host PACSCORDER uses: **UNKNOWN — OWNER DECISION REQUIRED** (OQ-067).

### 2.3 Layers relevant to PACSCORDER

The layer reference lists the following [G-40]:

| Layer kind | Layers | PACSCORDER relevance |
|---|---|---|
| Device | `rpi-cm4`, `rpi-cm5`, `rpi3`, `rpi4`, `rpi5`, `rpizero2w` | One per platform chosen for each capture configuration (2-lane and 4-lane, REQ-CAP-007; ADR-004 OPEN) |
| Kernel | `rpi-linux-2712`, `rpi-linux-v8`, `rpi-linux-v7` | `rpi-linux-v8` for Pi 4/CM4 and `rpi-linux-2712` for Pi 5/CM5. Reasoning: this matches the kernel split in [§1.3](#13-one-image-four-boards-what-differs). |
| SBOM | `sbom-base` | SBOM per build ([§2.5](#25-sbom)) |
| Image layout | `image-rpios`; `image-rota` ("Immutable GPT A/B layout for rotational OTA updates, boot/system redundancy, and a shared persistent data partition") | Single-slot or A/B ([§2.4](#24-ab-layout-image-rota-open-design-option)) |

### 2.4 A/B layout: image-rota (OPEN design option)

| Feature | Since | Facts |
|---|---|---|
| Immutable A/B system slots with one persistent partition | — | [G-40], [G-41] |
| Slots default to EROFS with zstd compression, tuned to the kernel page size (16K on Pi 5/CM5, 4K otherwise). ext4 is still selectable. | v2.3.0 (2026-03-04) | [G-41] |
| dm-verity hash generation | v2.4.0 | [G-41] |
| LUKS2 encryption, declared by the vendor fit for production use for `image-rpios` and `image-rota` (vendor statement; untested by PACSCORDER) | v2.5.0 | [G-41] |
| `slot-shared` framework for paths that persist across slot rotation | v2.6.0 | [G-41] |

Still open (*research open questions*, topic G; OQ-069):

- whether `image-rota` uses the firmware `tryboot` / `autoboot.txt` mechanism;
- whether it supports eMMC Compute Modules (CM4/CM5).

**The update mechanism is not decided** (OQ-069). The options and their constraints are compared in [RELEASE.md](RELEASE.md). How updates interact with an active recording or stream is a separate owner decision (OQ-094). Reasoning from [G-41]: on an immutable slot image, package versions are set at image build time. Pinning therefore belongs in the build configuration and the package mirror, not in `apt` holds on the device.

### 2.5 SBOM

- The 2026-10-06 Raspberry Pi OS Lite image is published with an SPDX-2.3 SBOM generated by syft-1.54.0 [G-34]. Whether every official image has one is not in the register.
- `rpi-image-gen` generates an SBOM and CVE reports [G-39], produces an SBOM for every build [G-36] (CORRECTED), and has an `sbom-base` layer [G-40].
- Whether that SBOM maps binaries to exact source packages and versions across both the Debian and the Raspberry Pi archive is untested (*research gap*, topic G; OQ-087). SBOM handling in a release is in [RELEASE.md](RELEASE.md).

### 2.6 Reproducibility caveat (RISK-017)

REQ-BLD-001 (DRAFT) requires a build that "pins kernel, firmware and package versions". On a Raspberry Pi OS basis this is **not yet solved**:

| Evidence | Facts |
|---|---|
| `apt full-upgrade` changes kernel and firmware. The default kernel series changed in the middle of the trixie release. | [G-08], [G-05] |
| `rpi-image-gen` installs from Raspberry Pi OS and Debian binary packages | [G-36] (CORRECTED) |
| The Raspberry Pi archive keeps older *versioned kernel* packages (6.12.25 and later) | [G-07] |
| `rpi-image-gen` ships a snapshot.debian.org-based layer for the **Debian half only**. No snapshot service or retention policy was found for `archive.raspberrypi.com`, which carries the kernel, firmware and `+rpt` media packages. | *research gap*, topic G |
| Minor `rpi-image-gen` releases contain breaking changes | [G-44] |

**PROPOSED mitigation** (ADR-003, ACCEPTED, requires a package mirror for reproducibility; how the mirror is built and the steps below are OWNER DECISION REQUIRED; OQ-067):

1. Mirror the exact package set used for a release, from both archives. The research suggests `aptly` or `reprepro` (*research gap*, topic G; not evaluated).
2. Pin the `rpi-image-gen` release tag.
3. Archive the SBOM of every release build.
4. Run TEST-BLD-001 twice, weeks apart, and compare the SBOMs.

Until this is done, two builds of the same configuration can differ (RISK-017).

### 2.7 Provisioning tools

| Tool | Facts | Notes |
|---|---|---|
| `rpiboot` (usbboot) | "provides a file server for loading software into memory on a Raspberry Pi for provisioning". By default it exposes the device as USB mass storage; this covers Pi 4B, CM4/4S, Pi 5 and CM5. **Pi 4B must first have rpiboot enabled, which permanently programs an OTP GPIO.** The `secure-boot-recovery` / `secure-boot-recovery5` extensions flash a secure-boot bootloader and provision OTP on Pi 4 and Pi 5 [G-45]. | Irreversible OTP writes: OWNER DECISION REQUIRED before use (OQ-071) |
| `rpi-sb-provisioner` | "A minimal-input automatic secure boot provisioning system". Supports Pi 5, Pi 4, CM5, CM4 and Zero 2 W, in modes `secure-boot`, `fde-only` and `naked`. Version 2.3.4 in the trixie archive, with rpiboot 20261005~110850. The host must be a Raspberry Pi 5 or other 64-bit Pi on Bookworm or newer, with at least 32 GB free and an official 27W supply [G-46]. | `rpi-image-gen` integrates with it [G-39] |
| Compute Module eMMC flashing | Fit `nRPI_BOOT` (J2), run `sudo apt install rpiboot` and `sudo rpiboot`, then write the image with Imager or `sudo dd ... of=/dev/sdX`. To flash the same image to many modules, the documentation points to the Secure Boot Provisioner [G-47]. | J2 is the reference used in Raspberry Pi's Compute Module documentation [G-47]. The boot-mode jumper of a PACSCORDER carrier is **UNKNOWN — VERIFICATION REQUIRED** (OQ-018). |

**Provisioning commands from [G-46] and [G-47]. NOT YET RUN ON PACSCORDER HARDWARE.** The full `dd` options are elided in the source and are **NEEDS VERIFICATION**.

```bash
sudo apt install rpiboot
sudo rpiboot
sudo apt install -y rpi-sb-provisioner
```

Reasoning from [G-45] and [G-46]: secure-boot provisioning writes OTP, which cannot be undone. The secure-boot decision must therefore precede any production provisioning (OQ-071).

### 2.8 Production hardening items (PROPOSED)

These are from ADR-003 Consequences and the topic G research gaps. All are NOT STARTED, and all need an owner decision (OQ-071).

| Item | Evidence |
|---|---|
| Remove or disable `cloud-init` | Preinstalled [G-32] |
| Remove or disable `rpi-connect-lite`, unless Connect Remote Update is chosen | Preinstalled [G-32], [G-53] |
| Remove `rpi-update` | Installed by default [G-10]; it can leave a system unbootable [G-09] |
| Pin and record the EEPROM bootloader version | `rpi-eeprom` installed [G-06]; `rpi-update` also updates the EEPROM on Pi 4/5 [G-09]; pinning method NEEDS VERIFICATION ([§1.7](#17-updates-apt-full-upgrade-and-rpi-update)) |
| Boot watchdog | `BOOT_WATCHDOG_TIMEOUT` can reset a unit whose OS never starts [G-54] (CORRECTED) |
| Read-only root, if not using `image-rota` | `raspi-config`'s Overlay File System uses `overlayroot=tmpfs`. It refuses when `MemTotal` is 262144 kB or less [G-48]. Reasoning: a tmpfs upper layer loses every write at reboot, so recordings need a persistent partition (OQ-006), and device configuration needs a persistent store (OQ-092). |
| EDID provisioning at every boot | REQ-CAP-003 (PROPOSED); RISK-010; trigger and ordering OQ-093; see [TC358743_DRIVER.md](TC358743_DRIVER.md) |
| `camera_auto_detect=0` | OQ-072; [§1.4](#14-configtxt) |
| *(Added 2026-10-08.)* Keep `fdk-aac` out of the image (no `libfdk-aac2t64`, no `fdkaacenc`, no FFmpeg rebuilt with `libfdk_aac`) | In a GPL FFmpeg build `libfdk_aac` needs `--enable-nonfree`, which makes the result unredistributable [I-42]; Debian calls its licence incompatible with every GPL version [I-43]; the Raspberry Pi FFmpeg is a GPL build [I-39]. See [RELEASE.md](RELEASE.md) §4 (OQ-113). |
| *(Added 2026-10-08.)* Load `tc358743-audio` with `tc358743`, and keep other overlays off GPIO 18–21 | [C-37], [I-30], [I-31]; [§1.4](#14-configtxt); OQ-114 |

### 2.9 Proposed rpi-image-gen configuration content

PROPOSED, nothing exists. The YAML syntax, file names and build invocation are not in the source register and are **NEEDS VERIFICATION** against the `rpi-image-gen` documentation for the pinned release (the build command is part of OQ-101).

| Element | Proposed content | Evidence |
|---|---|---|
| Device layer | One of `rpi4`, `rpi-cm4`, `rpi5`, `rpi-cm5` per build target; the targets are the platforms chosen for the 2-lane and 4-lane configurations (REQ-CAP-007) | [G-40]; ADR-004 OPEN |
| Kernel layer | `rpi-linux-v8` (Pi 4/CM4) or `rpi-linux-2712` (Pi 5/CM5) | [G-40] |
| Image layout | `image-rpios` (single system) or `image-rota` (A/B). OPEN, see OQ-069. | [G-40], [G-41] |
| SBOM | `sbom-base` | [G-40] |
| Package list | The [§1.6](#16-bring-up-package-list) packages that the media framework needs once one is chosen (ADR-007 is OPEN), plus the PACSCORDER application | [G-24]–[G-31] |
| *(Added 2026-10-08.)* H.265 and audio packages *(H.265 part deferred — REQ-ENC-002; not in current scope)* | Whichever of `libx265-215` (through `ffmpeg`/`libavcodec61` or `gstreamer1.0-plugins-bad`), `gstreamer1.0-alsa`, `gstreamer1.0-libav`, `libopus0` and `libvo-aacenc0` the chosen framework needs (ADR-007); never `fdk-aac`. Any package carried outside the archives for the [§1.8](#18-version-limits-of-the-distribution-media-stack-added-2026-10-08) gaps goes into the mirror and the SBOM. *(2026-10-08: with H.265 deferred no H.265 package is selected; `libx265-215` is present only as a dependency of `ffmpeg`/`libavcodec61` [H-09] or with `gstreamer1.0-plugins-bad`, and is recorded in the mirror and the SBOM like any other package. The §1.8 gaps apply only if REQ-ENC-002 is re-activated.)* | [H-01], [H-09], [H-12] (CORRECTED), [I-39], [I-44], [I-45], [I-46] |
| *(Added 2026-10-08.)* Audio overlay in the boot configuration | `tc358743-audio` with `tc358743` in the project `config.txt` | [C-37], [G-14] |
| Boot configuration | Project `config.txt` with per-model sections; content in [DEVICE_TREE.md](DEVICE_TREE.md) | [G-11], [G-71] |
| Package sources | A PACSCORDER mirror of both archives ([§2.6](#26-reproducibility-caveat-risk-017)) | RISK-017 |

---

## 3. Alternative: Buildroot

**Status:** documented alternative under ADR-003 (ACCEPTED 2026-10-07). NOT STARTED. ADR-003 provides for re-evaluating it if measured boot time, image size or reproducibility fails a requirement that does not exist yet (OQ-068).

Buildroot would also produce a project-owned image, so it would satisfy REQ-BLD-002 as well (ADR-003). ADR-003 still prefers `rpi-image-gen` because the 2-lane and 4-lane configurations (REQ-CAP-007) may land on both board families, and on the Pi 5 family Buildroot has the limits listed below: an older kernel pin [E-06], no overlay installation in the Pi 5 defconfig [E-15], and an LTS series with no CM5 defconfig [E-05] and no `tc358743-pi5.dtbo` [E-53] (see [§3.8](#38-buildroot-pitfalls-for-pacscorder), pitfalls 1 and 6).

### 3.1 Releases and support

| Series | Latest release | End of life | Facts |
|---|---|---|---|
| 2026.08.x (stable) | 2026.08, released 2026-09-04 | December 2026 | [E-01], [G-58] |
| 2026.05.x (old stable) | 2026.05.3 (2026-09-10) | September 2026, already past on 2026-10-06 | [E-02], [G-58] |
| 2025.02.x (LTS) | 2025.02.18 (2026-09-10) | March 2028 | [E-02], [G-58] |
| Next LTS | 2027.02 | — | [E-03] |

From 2025.02 on, the first release of each odd-numbered year is an LTS with 3 years of support; other releases come every 3 months [E-03]. **Which series PACSCORDER would track is unresolved** (OQ-064).

### 3.2 Upstream Raspberry Pi defconfigs (Buildroot 2026.08)

| Defconfig | Board | Kernel defconfig | In-tree DTBs built | Facts |
|---|---|---|---|---|
| `raspberrypi4_64_defconfig` | Pi 4 B, 400, CM4, CM4S (64-bit) | `bcm2711` | `bcm2711-rpi-4-b`, `-rpi-400`, `-rpi-cm4`, `-rpi-cm4s` | [E-04], [E-07], [E-08] |
| `raspberrypicm4io_64_defconfig` | CM4 on IO Board (64-bit) | `bcm2711` | `bcm2711-rpi-cm4` | [E-04], [E-07], [E-08] |
| `raspberrypi5_defconfig` | Pi 5 B and 500 | `bcm2712` | `bcm2712-rpi-5-b`, `bcm2712d0-rpi-5-b`, `bcm2712-rpi-500` | [E-04], [E-07], [E-08] |
| `raspberrypicm5io_defconfig` | CM5 on IO Board | `bcm2712` | `bcm2712-rpi-cm5-cm5io`, `bcm2712-rpi-cm5l-cm5io` | [E-04], [E-07], [E-08] |

Common settings in all four defconfigs:

- `BR2_PACKAGE_RPI_FIRMWARE=y`, `BR2_PACKAGE_KMOD=y` [E-07].
- External toolchain `BR2_TOOLCHAIN_EXTERNAL_BOOTLIN_AARCH64_GLIBC_STABLE` ("aarch64 glibc stable 2025.08-1"), which declares GCC ≥ 14 and kernel headers ≥ 5.4 [E-12].
- A 120M ext4 rootfs and `BR2_DOWNLOAD_FORCE_CHECK_HASHES=y` [G-62].
- The `cm4io_64` and `cm5io` defconfigs also build the host `rpiboot` (`BR2_PACKAGE_HOST_RASPBERRYPI_USBBOOT=y`) [G-62].
- All `board/raspberrypi*` directories other than `board/raspberrypi` are symlinks to it [E-04].

**LTS 2025.02.18** has the Pi 4, CM4 IO and Pi 5 defconfigs but **no CM5 defconfig** [E-05].

### 3.3 Kernel and firmware pins

| Item | Buildroot 2026.08 | Buildroot LTS 2025.02.18 | Facts |
|---|---|---|---|
| Kernel source | `raspberrypi/linux` commit `21b410140c47ffab5668399f6f143c7d7b935c8b` = Linux **6.12.61** (2025-12-09), via `BR2_LINUX_KERNEL_CUSTOM_TARBALL`. 2026.05.3 and master on 2026-10-06 use the same commit. | commit `576cc10e1ed50a9eacffc7a05c796051d7343ea4` = Linux **6.6.28** (2024-04-18) | [E-06], [G-59], [E-11] |
| Kernel tarball hash | `board/raspberrypi/patches/linux/linux.hash` (sha256 of the commit tarball). `linux-headers/linux-headers.hash` is a symlink to it. Selected by `BR2_GLOBAL_PATCH_DIR="board/raspberrypi/patches"`. | — | [E-21] (CORRECTED) |
| Firmware (`rpi-firmware`) | `063bcab6c8a90efb0d19f69d88cbbc7ec79cab68` (2025-12-08). Its `uname_string8` reports `6.12.61-v8+`, matching the kernel. | `5476720d52cf579dc1627715262b30ba1242525e` ("kernel: Bump to 6.6.28") | [E-10], [E-11] |
| TC358743 overlays in that firmware | `tc358743.dtbo`, `tc358743-pi5.dtbo`, `overlay_map.dtb` | `tc358743.dtbo`, `overlay_map.dtb`; **no `tc358743-pi5.dtbo`** | [E-53] |
| Pi 5/CM5 page size | 4K. The `linux-4k-page-size.fragment` sets `CONFIG_ARM64_4K_PAGES=y` and overrides the 16K page size in `bcm2712_defconfig`. | — | [E-09], [G-60] |

### 3.4 Boot partition, config.txt and image assembly

| Topic | Fact | Facts |
|---|---|---|
| `rpi-firmware` options | `_BOOTCODE_BIN`; variant bools `_VARIANT_PI4` (start4.elf/fixup4.dat), `_PI4_X`, `_PI4_CD`, `_PI4_DB` (and the `_PI*` equivalents); `_CONFIG_FILE`; `_CMDLINE_FILE`; `_INSTALL_DTBS`; `_INSTALL_DTB_OVERLAYS` (default y); `_EXTRA_FILES`. **There is no Pi 5 firmware variant.** | [E-13] |
| Pi 4 firmware variants and the codec | The cut-down firmware (`start4cd.elf`, selected by `gpu_mem=16`) removes codec support. `_PI4_X` is "more audio/video codecs". Which variant and `gpu_mem` `bcm2835-codec` needs is open (OQ-048). | [D-48] |
| Overlays | `_INSTALL_DTB_OVERLAYS` copies the prebuilt `boot/overlays/*.dtbo` and `overlay_map.dtb` **from the firmware tarball, not from the kernel build**. With kernel DTS support it also adds `DTC_FLAGS=-@`. | [E-14], [G-61] |
| Overlays on Pi 5 | `raspberrypi5_defconfig` explicitly disables `_INSTALL_DTB_OVERLAYS` (both 2026.08 and 2025.02.18). `raspberrypicm5io_defconfig` sets it to y, and the Pi 4/CM4 64-bit defconfigs keep the default y. | [E-15], [G-61] |
| Kernel-built DTBs | The `linux` package installs only the DTBs named in `BR2_LINUX_KERNEL_INTREE_DTS_NAME`, `_INTREE_DTSO_NAMES` and the custom DTS path/dir. Overlays built by the Raspberry Pi kernel tree are not installed. | [E-20] |
| `config.txt` | Taken from `BR2_PACKAGE_RPI_FIRMWARE_CONFIG_FILE`; samples are `config_4_64bit.txt`, `config_cm4io_64bit.txt`, `config_5.txt`, `config_cm5io.txt`. Pi 5 uses `cmdline_5.txt` (`console=ttyAMA10,115200`); the others use `cmdline.txt` (`console=ttyAMA0,115200`). | [E-16] |
| Sample contents | `config_4_64bit.txt` has `start_file=start4.elf`, `fixup_file=fixup4.dat`, `kernel=Image`, `disable_overscan=1`, `gpu_mem_256/512/1024=100`, `dtoverlay=miniuart-bt`, `arm_64bit=1`. `config_5.txt` has only `kernel=Image` and `disable_overscan=1`. **Neither contains a camera or TC358743 overlay**; both say they are samples to be overridden. | [E-17] |
| Image assembly | `post-image.sh` uses `genimage-${BOARD_NAME}.cfg` if it exists, otherwise generates `genimage.cfg` from `genimage.cfg.in` and runs genimage. | [E-18] |
| Partition sizes | `boot.vfat` is 32M. `sdcard.img` has a `boot` partition (0xC, bootable) and a `rootfs` partition (0x83, ext4). `BR2_TARGET_ROOTFS_EXT2_SIZE="120M"`. | [E-19] |

### 3.5 Customisation mechanisms

| Mechanism | What it does | Facts |
|---|---|---|
| `BR2_EXTERNAL` tree | Keeps project customisation outside the Buildroot tree. Selected with `make BR2_EXTERNAL=/path/to/foo menuconfig`. Must contain at least `external.desc`, `Config.in` and `external.mk`. Its path is available as `BR2_EXTERNAL_$(NAME)_PATH`. | [E-22] |
| Rootfs overlays and post-build scripts | `BR2_ROOTFS_OVERLAY` (directories copied over the target filesystem) and `BR2_ROOTFS_POST_BUILD_SCRIPT` (run before rootfs images are assembled): the two recommended ways to customise the target filesystem | [E-23] |
| Post-image scripts | `BR2_ROOTFS_POST_IMAGE_SCRIPT` runs after all images are created. It gets the images directory as its first argument and can use `BINARIES_DIR`, `TARGET_DIR`, `HOST_DIR` and others. | [E-24] |
| Global patch directories | `BR2_GLOBAL_PATCH_DIR` (space-separated) holds `<pkg>/<version>/` patches and extra `.hash` files. The manual calls it the preferred method for project patches. | [E-25] |
| Kernel configuration file | `BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE`, saved with `make linux-update-defconfig`. The update targets cannot be used when fragment files are set; `<pkg>-diff-config` shows changes to move into fragments. | [E-26] |
| Kernel config fragments | `BR2_LINUX_KERNEL_CONFIG_FRAGMENT_FILES`: fragments merged into the main configuration. Can be combined with `BR2_LINUX_KERNEL_DEFCONFIG="bcm2711"` / `"bcm2712"`, as the Pi 5 defconfig does. | [E-27] |
| Custom DTS | `BR2_LINUX_KERNEL_CUSTOM_DTS_DIR` (copied over `arch/<arch>/boot/dts/`; should contain the vendor subdirectory, e.g. `broadcom/`). `BR2_LINUX_KERNEL_CUSTOM_DTS_PATH` is deprecated because of a kernel build change in 6.12. | [E-28] |

**Pitfall** (reasoning from [E-21], [E-25]; *research design risk*, topic E). A `BR2_EXTERNAL` project that sets its own `BR2_GLOBAL_PATCH_DIR` must keep `board/raspberrypi/patches` in the list, or supply its own `linux.hash`. Otherwise the kernel download fails the forced hash check.

### 3.6 Packages and symbols PACSCORDER would need

| Need | Buildroot symbol(s) | Version in 2026.08 | Facts |
|---|---|---|---|
| `v4l2-ctl`, `media-ctl`, `v4l2-compliance` | `BR2_PACKAGE_LIBV4L` **and** `BR2_PACKAGE_LIBV4L_UTILS` | 1.32.0 (1.28.1 in 2025.02.18) | [E-29], [G-64] |
| `i2cdetect` etc. | `BR2_PACKAGE_I2C_TOOLS`. Depends on `BR2_PACKAGE_BUSYBOX_SHOW_OTHERS`, which the RPi defconfigs already set. | 4.4 | [E-30], [G-64] |
| GStreamer and `v4l2src` | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2` | GStreamer 1.24.13 (also in 2025.02.18) | [E-31], [G-64] |
| `v4l2h264enc` / `v4l2convert` (Pi 4/CM4 hardware encoder) | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE`. **Not on by default.** Without it, `-Dv4l2-probe=false` is passed and the M2M elements are never registered. | — | [E-32], [E-33], [D-39], [G-65] |
| `rtmp2sink` | `BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2` (no external library) | — | [E-34] |
| FLV / MP4 muxing | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV` / `_ISOMP4` | — | [E-34] |
| `webrtcbin` | `BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_WEBRTC`. Needs shared libraries (`!BR2_STATIC_LIBS`). Selects libnice and the DTLS (→ OpenSSL), SCTP and SRTP (→ libsrtp) plugins. libnice builds its GStreamer elements only with `BR2_PACKAGE_GST1_PLUGINS_BASE=y`. | libnice 0.1.21 | [E-35], [G-64] |
| `x264enc` (Pi 5/CM5 software encode) | `BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264` (selects `BR2_PACKAGE_X264`) | x264 git `baee400f…` | [E-36], [G-64] |
| FFmpeg `libx264` | `BR2_PACKAGE_FFMPEG` + `BR2_PACKAGE_FFMPEG_GPL` + `BR2_PACKAGE_X264`. `--enable-libx264` is passed only when both `X264` and `FFMPEG_GPL` are y. | FFmpeg 6.1.5, upstream, without Raspberry Pi V4L2/DRM_PRIME patches | [E-36], [D-46] (CORRECTED), [G-64] |
| Loading XZ-compressed modules | `BR2_PACKAGE_KMOD`, `_KMOD_TOOLS`, `_XZ`, `_HOST_KMOD_XZ`, already set in the RPi defconfigs | — | [E-49] |
| Module autoloading | `/dev` management. The default is devtmpfs only (`BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_DEVTMPFS`). The manual names devtmpfs + mdev as able to load kernel modules automatically; with systemd, udev handles `/dev` [E-50]. That devtmpfs only does not autoload modules, and the eudev option, come from the research (*research gap*, topic E; NEEDS VERIFICATION). | — | [E-50] |
| Field update (if chosen) | Packages `rauc`, `swupdate`, `mender`. Their Kconfig symbol names are not in the register (NEEDS VERIFICATION). | 1.15.2, 2026.05.1, 3.5.3 | [G-64] |
| *(Added 2026-10-08.)* H.265 (`x265enc`, FFmpeg `libx265`; deferred — REQ-ENC-002; not in current scope), ALSA capture (`alsasrc`), Opus and AAC encoders | Not in the register: research topics H and I covered Raspberry Pi OS only. Symbol names and versions are NEEDS VERIFICATION (BUILD TEST REQUIRED). Reasoning from [H-25]: Buildroot's FFmpeg 6.1.5 belongs to the first FFmpeg series that can mux HEVC into FLV; whether Buildroot's build enables it is NEEDS VERIFICATION. Buildroot's GStreamer 1.24.13 has no `eflvmux` [F-34] (CORRECTED). | — | [H-25], [F-34] |

**Not available in stock Buildroot 2026.08:**

- `gst1-plugins-rs`, so no `webrtcsink`. Buildroot master on 2026-10-06 has no such package [F-43]; reasoning: the older 2026.08 release has none either;
- `eflvmux`, which first appears in GStreamer 1.28 [F-34] (CORRECTED).

Whether Buildroot's FFmpeg 6.1.5 build includes `h264_v4l2m2m` is open (*research open question*, topic E; OQ-066). Upstream FFmpeg's V4L2 M2M code uses MMAP buffers only, with no DMABUF import [D-44].

### 3.7 Kernel Kconfig symbols and how their meaning changed

The same symbol name selects **different drivers** in different Raspberry Pi kernel branches. Any kernel config fragment, module list or `modprobe` step must be rechecked at every kernel bump.

| Kconfig symbol | rpi 6.6.28 (Buildroot LTS 2025.02.18) | rpi 6.12.61 (Buildroot 2026.08) | `rpi-6.18.y` (Raspberry Pi OS 6.18.50; branch tip 6.18.55) |
|---|---|---|---|
| `VIDEO_TC358743` | Not in the register: KERNEL SOURCE INSPECTION REQUIRED | `=m` in `bcm2711` and `bcm2712` defconfigs [E-39], [G-63] | `=m` in both [G-16], [E-39]. Tristate; selects `MEDIA_CONTROLLER`, `VIDEO_V4L2_SUBDEV_API`, `HDMI`, `V4L2_FWNODE` [E-38]. |
| `VIDEO_TC358743_CEC` | Not in the register | Not set [E-39] | Not set [E-39]. Bool; selects `CEC_CORE` [E-38]. |
| `VIDEO_BCM2835_UNICAM` | **Downstream** driver in `drivers/media/platform/bcm2835/`, matches `brcm,bcm2835-unicam` [E-41] | **Mainline**, Media-Controller-only driver (`drivers/media/platform/broadcom/`, `bcm2835-unicam.ko`), matches only `brcm,bcm2835-unicam-upstream` [E-40] (CORRECTED) | Same as 6.12.61 [E-40] (CORRECTED), [G-21] |
| `VIDEO_BCM2835_UNICAM_LEGACY` | **Does not exist** [E-41] | Downstream driver, `bcm2835-unicam-legacy.ko`. Matches `brcm,bcm2835-unicam` (forces MC mode) and `brcm,bcm2835-unicam-legacy` (video-node mode unless the `media_controller` module parameter or the `brcm,media-controller` DT property is set) [E-40] (CORRECTED) | Same as 6.12.61 [E-40] (CORRECTED), [G-21] |
| `VIDEO_RP1_CFE` | Not in the register | **Downstream** driver (`rp1_cfe/`, `rp1-cfe.ko`), matches `raspberrypi,rp1-cfe` [E-45] | **Mainline** driver (`rp1-cfe/`, `rp1-cfe.ko`), matches only `raspberrypi,rp1-cfe-upstream` [E-44] |
| `VIDEO_RP1_CFE_DOWNSTREAM` | Not in the register | **Absent.** The only CFE directory is `rp1_cfe/`, whose Kconfig defines `VIDEO_RP1_CFE` [E-45]; the 6.12.61 defconfigs list only `CONFIG_VIDEO_RP1_CFE=m` [G-63] | Downstream driver `rp1-cfe-downstream.ko`, matches `raspberrypi,rp1-cfe`, which is the compatible used by the `rp1.dtsi` CSI nodes [E-44] |
| `VIDEO_CODEC_BCM2835`, `VIDEO_ISP_BCM2835` | Not in the register | `=m` [E-46] | `=m` [E-46], [G-18]. Both select `BCM2835_VCHIQ_MMAL` → `BCM_VC_SM_CMA` (rpi-6.18.y Kconfig) [E-46] |
| CMA DMA-BUF heap name | Not in the register | Registered only under `cma_get_name(cma)` [E-48] | `"default_cma_region"`, plus a heap named after the CMA area when `DMABUF_HEAPS_CMA_LEGACY` (default y) is set [E-48] |
| Page size (`bcm2712_defconfig`) | Not in the register | 16K in the RPi defconfig [G-63]; Buildroot forces 4K [E-09] | 16K, `CONFIG_ARM64_16K_PAGES=y` [G-20], [E-51] |

Reasoning from [E-40], [E-41], [E-44], [E-45]:

- **Pi 5/CM5 receiver.** On 6.12.61, `VIDEO_RP1_CFE` is the driver for the stock `raspberrypi,rp1-cfe` nodes. On 6.18 those nodes need `VIDEO_RP1_CFE_DOWNSTREAM`.
- **Pi 4/CM4 receiver.** On 6.6.28, `VIDEO_BCM2835_UNICAM` serves `brcm,bcm2835-unicam`. From 6.12 on, that compatible is served by `VIDEO_BCM2835_UNICAM_LEGACY`.
- **Default binding.** The base DT `csi0`/`csi1` nodes use `brcm,bcm2835-unicam`, so by default the downstream driver binds, in MC mode [E-40] (CORRECTED). The stock `tc358743` overlay changes `csi1` to `brcm,bcm2835-unicam-legacy` unless `media-controller` is given [G-12], [E-42].

### 3.8 Buildroot pitfalls for PACSCORDER

These are from the topic E research gaps and design risks, with the register facts behind them. All are NOT STARTED and need a BUILD TEST or HARDWARE TEST.

| # | Pitfall | Evidence | PROPOSED handling | OQ |
|---|---|---|---|---|
| 1 | **Overlays are not installed on Pi 5.** `raspberrypi5_defconfig` disables overlay installation, and no sample `config.txt` contains a TC358743 overlay. | [E-15], [G-61], [E-17] | Set `BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS=y`, or ship `tc358743-pi5.dtbo` and `overlay_map.dtb` through `_EXTRA_FILES` (subdirectory placement NEEDS VERIFICATION). Supply a project `config.txt`. Why upstream disables overlays on Pi 5 is unknown. Whether Buildroot's DTSO options can build Raspberry Pi `-overlay.dts` files is untested. | OQ-065 |
| 2 | **Overlays come from the firmware tarball, not the kernel.** If the kernel and `rpi-firmware` pins drift apart, overlay compatibles and labels can drift from the drivers. | [E-14], [E-20], [E-10] | Bump kernel and firmware together. Choose the firmware commit whose `uname_string8` matches the kernel, and update `linux.hash` for both `linux` and `linux-headers`. | OQ-064 |
| 3 | **Modules may never load.** All media drivers are modules, and the default devtmpfs-only `/dev` has no module autoloading. | [E-49], [E-50], [G-63] | Choose mdev, eudev or systemd-udev, or add an explicit module-load step | OQ-065 |
| 4 | **`v4l2h264enc` is absent.** `V4L2_PROBE` is off by default. | [E-32], [D-39], [G-65] | Enable `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE` | OQ-066 |
| 5 | **Images are too small.** The samples have a 32M boot partition and a 120M ext4 rootfs; research judged these too small for GStreamer, FFmpeg, x264 and the WebRTC dependencies (*research gap*, topic E). | [E-19], [G-62] | Add a project `genimage-<board>.cfg`, which `post-image.sh` picks up [E-18], and a larger `BR2_TARGET_ROOTFS_EXT2_SIZE` | OQ-068 |
| 6 | **The LTS series is unsuitable for Pi 5/CM5.** It has no CM5 defconfig, kernel 6.6.28, and no `tc358743-pi5.dtbo`. | [E-05], [E-11], [E-53] | Do not use 2025.02.x for Pi 5/CM5 without a kernel and firmware bump | OQ-064 |
| 7 | **2026.08 has a short life.** It reaches end of life in December 2026; the next LTS is 2027.02. | [E-01], [E-03] | Owner decision on the series and on how to track it | OQ-064 |
| 8 | **4K vs 16K pages.** Buildroot's Pi 5/CM5 kernel uses 4K pages, while Raspberry Pi OS uses 16K. Results measured on one may not carry over. | [E-09], [G-60], [G-20] | Re-run performance tests on the shipped configuration | OQ-055 |
| 9 | **32-bit Pi 4 kernels.** Research reports that 32-bit BCM2711 kernels are not supported from 6.18 (*research gap*, topic E; **NEEDS VERIFICATION**). | — | Use only the 64-bit defconfigs | OQ-064 |
| 10 | **Toolchain headers.** The toolchain declares kernel headers ≥ 5.4 [E-12]. Research found `linux/dma-heap.h` first appears in 5.6 (*research gap*, topic E; NEEDS VERIFICATION). | [E-12] | Check the sysroot headers; carry the uapi header if needed | OQ-062 |
| 11 | **Pi 4/CM4 codec firmware.** Which firmware variant and `gpu_mem` `bcm2835-codec` needs is unknown. | [D-48], [E-13], [E-17] | VENDOR CONFIRMATION / HARDWARE TEST | OQ-048 |
| 12 | **EEPROM bootloader.** Buildroot 2026.08 has no `rpi-eeprom` package, so the EEPROM version must be pinned and recorded outside Buildroot (*research gap*, topic E). | — | See [RELEASE.md](RELEASE.md) | OQ-071 |
| 13 | **RTC on the camera I2C bus.** The CM4IO/CM5IO sample configs put an RTC on the camera I2C bus, according to the research (*research gap*, topic E; not a register fact; NEEDS VERIFICATION). | — | Check bus sharing with the TC358743 before reusing those samples | OQ-026 |
| 14 | **Media stack differs from Raspberry Pi OS.** Buildroot has GStreamer 1.24.13 and upstream FFmpeg 6.1.5; Raspberry Pi OS has GStreamer 1.26.2 and FFmpeg 7.1.5 `+rpt2`. *(Added 2026-10-08.)* The diff now also covers H.265 (x265 package and SIMD build), HEVC muxing and the audio encoders, which were researched for Raspberry Pi OS only (the H.265 and HEVC items deferred — REQ-ENC-002; not in current scope) ([§1.6](#16-bring-up-package-list), [§1.8](#18-version-limits-of-the-distribution-media-stack-added-2026-10-08)). | [G-64], [D-46] (CORRECTED), [G-26], [G-29] | Diff the features the pipeline needs | OQ-066 |
| 15 | **Reproducibility.** `BR2_REPRODUCIBLE` is experimental and limited to builds that use the same output directory. Download hashes are enforced. | [G-66], [G-62] | Pin the Buildroot release; keep the download cache | OQ-064 |

---

## 4. Kernel and firmware versions

| Basis | Kernel | Kernel commit | Firmware | Pi 5/CM5 page size | Facts |
|---|---|---|---|---|---|
| Raspberry Pi OS Lite 2026-10-06 (bring-up, ADR-003 ACCEPTED) | **6.18.50** (`1:6.18.50-1+rpt1`) | `cff533aec2fa601846766b32ff57204e0a61bed7` | Release notes: `6f0881cba8bea8ec24956a5718714730e9936ec5`; package `raspi-firmware 1:1.20260915-1` | 16K | [G-04], [G-06], [G-20] |
| `raspberrypi/linux` default branch `rpi-6.18.y`, tip on 2026-10-06 | **6.18.55** | `af73e0836bf0` ("configs: Regenerate the defconfigs", 2026-10-06) | `raspberrypi/firmware` master reports `6.18.55-v8+` (built 2026-10-05) | 16K | [E-37], [G-20] |
| Buildroot 2026.08 (alternative) | **6.12.61** | `21b410140c47ffab5668399f6f143c7d7b935c8b` (2025-12-09) | `063bcab6c8a90efb0d19f69d88cbbc7ec79cab68` (2025-12-08) | 4K (forced) | [E-06], [E-10], [E-09] |
| Buildroot LTS 2025.02.18 | **6.6.28** | `576cc10e1ed50a9eacffc7a05c796051d7343ea4` (2024-04-18) | `5476720d52cf579dc1627715262b30ba1242525e` | Not in the register. This series has no CM5 defconfig [E-05]. | [E-11], [E-05] |
| Versioned kernels still in the trixie archive | 6.12.25, 6.12.34, 6.12.47, 6.12.62, 6.12.75, 6.18.29, 6.18.33, 6.18.34, 6.18.39, 6.18.50 | — | — | — | [G-07] |
| Raspberry Pi OS legacy (Bookworm) images, refreshed 2026-10-06 | 6.12.109 | — | — | — | [G-55] (CORRECTED) |

Which kernel version PACSCORDER ships is not decided. Under ADR-003 (ACCEPTED 2026-10-07; OQ-012 ANSWERED) the kernel comes from the Raspberry Pi OS packages; the version to pin is open (OQ-067, RISK-017), and OQ-064 covers the Buildroot alternative. The behaviour of the `tc358743` driver and the receivers was researched against `rpi-6.18.y` (see [TC358743_DRIVER.md](TC358743_DRIVER.md)). Whether the driver source in the shipped 6.18.50 kernel differs from the inspected 6.18.55 tip is OQ-097. 6.18.39, listed above as an archive version [G-07], is also the kernel of a community capture report for Pi 5 [C-33]; it is neither the shipped nor the inspected kernel. Results on any other kernel need their own tests.

---

## 5. Proposed repository layout for build configuration (PROPOSED)

**PROPOSED. Nothing exists. OWNER DECISION REQUIRED** for the layout itself. The build basis it serves is decided: ADR-003 (ACCEPTED 2026-10-07; OQ-012 ANSWERED).

The layout below keeps every input to the project's own image (REQ-BLD-002) in this repository (REQ-BLD-001). Under ADR-003 (ACCEPTED) only `rpi-image-gen/` would be populated; `buildroot/` stays empty unless the re-evaluation in ADR-003 item 3 leads to a new decision (Rule 13). The file names inside `rpi-image-gen/` are placeholders; the real names follow that tool's documentation (NEEDS VERIFICATION, [§2.9](#29-proposed-rpi-image-gen-configuration-content)).

```text
build/
├── README.md                     # How to build; pinned tool versions; host requirements
├── common/
│   ├── config.txt                # [pi4]/[cm4]/[pi5]/[cm5] sections (G-71); content per DEVICE_TREE.md
│   └── edid/                     # EDID file(s) loaded at every boot (REQ-CAP-003, PROPOSED); possibly one per lane configuration (REQ-CAP-007, OQ-002)
├── bringup/                      # Stock Raspberry Pi OS Lite (ADR-003 item 1)
│   ├── IMAGE                     # Dated image file name and SBOM reference (G-01, G-34)
│   └── packages.txt              # §1.6 package list with versions
├── rpi-image-gen/                # Production image (ADR-003 item 2, ACCEPTED)
│   ├── RPI_IMAGE_GEN_VERSION     # Pinned release tag (G-44)
│   ├── pacscorder-<board>.yaml   # One per platform; device/kernel/image/sbom layers (G-40)
│   ├── layers/                   # Project layers (G-38)
│   └── mirror/                   # Package-mirror definition for both archives (RISK-017)
├── buildroot/                    # Alternative (ADR-003 item 3), a BR2_EXTERNAL tree (E-22)
│   ├── BUILDROOT_VERSION
│   ├── external.desc
│   ├── Config.in
│   ├── external.mk
│   ├── configs/pacscorder_<board>_defconfig
│   ├── board/pacscorder/
│   │   ├── config_<board>.txt    # Replaces the sample config.txt (E-16, E-17)
│   │   ├── genimage-<board>.cfg  # Larger partitions (E-18, E-19)
│   │   ├── linux.fragment        # Kconfig fragment (E-27)
│   │   ├── rootfs-overlay/       # (E-23)
│   │   └── post-build.sh
│   └── patches/                  # BR2_GLOBAL_PATCH_DIR entries, incl. linux.hash (E-21, E-25)
└── kernel-patches/               # Driver patches carried per ADR-002 (PROPOSED)
```

Every change to these files is a build change under Rules 1 and 17: record it in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md) and [CHANGELOG.md](CHANGELOG.md), and re-run TEST-BLD-001.

---

## 6. Build test: TEST-BLD-001

| | |
|---|---|
| Test | TEST-BLD-001 — Image build from clean checkout (canonical ID, [README.md](README.md)) |
| Verifies | REQ-BLD-001, REQ-BLD-002 |
| Status | NOT STARTED. No build configuration exists. |
| Procedure | Draft procedure in [TESTING.md](TESTING.md) (TEST-BLD-001), NOT YET RUN. Its build commands (including the `rpi-image-gen` build command) are NEEDS VERIFICATION (OQ-101) until a build configuration exists. ADR-003 is ACCEPTED (2026-10-07), so that configuration is `rpi-image-gen`-based. |

PROPOSED evidence the test should record (reasoning; REQ-BLD-001 and REQ-BLD-002 have no accepted acceptance criteria yet, OQ-017):

1. Build host: OS, architecture and tool versions. For `rpi-image-gen` the host must be native arm64 Debian [G-39].
2. Pinned inputs: the `rpi-image-gen` tag or Buildroot release, the kernel and firmware versions, and the package mirror snapshot.
3. Outputs: the image file and its checksum, and the SBOM ([G-39]) or Buildroot `legal-info` ([G-68]).
4. Reproducibility: a second build from the same checkout, with the SBOMs compared (OQ-067).
5. Project-owned image (REQ-BLD-002): the image is produced from this repository's build configuration, not taken unmodified from a stock download.
6. Boot of the image on the platform chosen for each capture configuration (2-lane and 4-lane, REQ-CAP-007). That part is TEST-PLT-001 and is **BLOCKED — HARDWARE REQUIRED**.

---

## 7. Open questions for the build

| OQ | Question | Resolution |
|---|---|---|
| OQ-012 | Accept ADR-003? Should Yocto also be evaluated? Owner input of 2026-10-07 is recorded as REQ-BLD-002 (own OS image). **ANSWERED 2026-10-07:** the owner accepted ADR-003 ("accept ADR-003"); Yocto is not evaluated unless the owner asks. | ANSWERED (owner decision, 2026-10-07) |
| OQ-016 | Is HDMI CEC required? It would need `CONFIG_VIDEO_TC358743_CEC`. | OWNER DECISION / KERNEL SOURCE INSPECTION REQUIRED |
| OQ-048 | Pi 4/CM4 firmware variant and `gpu_mem` for the codec | VENDOR CONFIRMATION / HARDWARE TEST REQUIRED |
| OQ-055 | Effect of 16K vs 4K pages on Pi 5/CM5 | HARDWARE TEST REQUIRED |
| OQ-062 | DMA-BUF heap names; `dma-heap.h` in the toolchain | HARDWARE / BUILD TEST REQUIRED |
| OQ-064 | Buildroot release series, kernel and firmware pin | OWNER DECISION / BUILD TEST / HARDWARE TEST REQUIRED |
| OQ-065 | Buildroot overlays and module autoloading | BUILD / HARDWARE TEST REQUIRED |
| OQ-066 | Media-stack differences between Buildroot and Raspberry Pi OS | BUILD TEST REQUIRED |
| OQ-067 | Reproducible Raspberry Pi OS based builds; build host | VENDOR CONFIRMATION / BUILD TEST REQUIRED |
| OQ-068 | Image size, RAM use and boot-to-first-frame time | OWNER DECISION / BUILD / HARDWARE TEST REQUIRED |
| OQ-069 | Field update mechanism | OWNER DECISION / VENDOR CONFIRMATION / BUILD TEST REQUIRED ([RELEASE.md](RELEASE.md)) |
| OQ-070 | Support horizon for Raspberry Pi packages | VENDOR CONFIRMATION REQUIRED |
| OQ-071 | Production hardening, EEPROM pinning, secure boot | OWNER DECISION / BUILD TEST REQUIRED |
| OQ-072 | Is `camera_auto_detect=0` needed? | HARDWARE TEST REQUIRED |
| OQ-094 | May an update or reboot happen while a recording or stream is active? | OWNER DECISION REQUIRED |
| OQ-097 | Does `tc358743.c` in the shipped 6.18.50 kernel differ from the inspected 6.18.55 tip? | KERNEL SOURCE INSPECTION REQUIRED |
| OQ-100 | `config.txt` overlay-parameter syntax (bare booleans, several parameters per line, `[pi4]` matching CM4, unknown parameters) | VENDOR CONFIRMATION / HARDWARE TEST REQUIRED |
| OQ-101 | Command syntax not in the register. For this document: recording kernel, firmware, EEPROM and package versions, and the `rpi-image-gen` build command | VENDOR CONFIRMATION / HARDWARE TEST REQUIRED |
| OQ-018, OQ-021, OQ-026 | PACSCORDER hardware composition, lanes (including whether one bridge board serves both the 2-lane and the 4-lane configuration, REQ-CAP-007), I2C bus | OQ-018: OWNER DECISION / VENDOR CONFIRMATION REQUIRED. OQ-021: VENDOR CONFIRMATION / HARDWARE TEST REQUIRED. OQ-026: DATASHEET / HARDWARE TEST REQUIRED. |
| OQ-054 *(added 2026-10-08)* | Does `tc358743-audio` load and capture on CM5 (RP1 I2S1)? | KERNEL SOURCE INSPECTION / HARDWARE TEST REQUIRED |
| OQ-105 *(added 2026-10-08; not in current scope — H.265 deferred, REQ-ENC-002)* | x265 SIMD paths active on CM4 and CM5; will a newer x265 than 4.1 be provided, or must PACSCORDER carry one? | BUILD TEST / HARDWARE TEST / VENDOR CONFIRMATION REQUIRED |
| OQ-107 *(added 2026-10-08; not in current scope — H.265 deferred, REQ-ENC-002)* | HEVC-over-RTMP path with GStreamer 1.26.2: FFmpeg, a backported or newer GStreamer, or SRT | BUILD TEST REQUIRED; OWNER DECISION REQUIRED |
| OQ-113 *(added 2026-10-08)* | AAC patent licensing; `fdk-aac` licence | LEGAL CLARIFICATION REQUIRED ([RELEASE.md](RELEASE.md)) |
| OQ-114 *(added 2026-10-08)* | GPIO 18–21 allocation when the audio overlay is enabled; custom overlay if a pin must be freed | OWNER DECISION / BUILD TEST REQUIRED |

---

## Verification status

### Verified from sources (fact IDs)

These statements are supported by entries in [REFERENCES.md](REFERENCES.md), with verdict CONFIRMED or CORRECTED (corrected wording used):

- **Raspberry Pi OS Lite 2026-10-06 image, kernel, firmware, packages and preinstalled components:** [G-01]–[G-10], [G-24]–[G-34], [G-53], [G-57].
- **`config.txt` location and overlays:** [G-11] (CORRECTED), [G-12]–[G-15], [C-39] (CORRECTED), [E-43].
- **Kernel configuration and modules in `rpi-6.18.y`:** [G-16]–[G-21], [E-38]–[E-40] ([E-40] CORRECTED), [E-44], [E-47] (CORRECTED), [E-49], [E-51]. Camera I2C mux: [C-24]. One image for four boards: [G-71] (reasoning tier, CORRECTED).
- **Per-platform CSI-2 interfaces:** [C-01], [C-03], [C-04], [C-05].
- **Community-tier (reported, not official):** [C-41] (Raspberry Pi engineers report that libcamera does not support the TC358743); [C-33] (a Pi 5 capture report on kernel 6.18.39).
- **`rpi-image-gen`, image layouts, SBOM and provisioning:** [G-35]–[G-41], [G-44]–[G-48], [G-54] (CORRECTED). [G-36] is CORRECTED.
- **Buildroot:** [E-01]–[E-36], [E-41], [E-42], [E-45], [E-46], [E-48], [E-50], [E-53], [G-58]–[G-66], [G-68]. [E-21] is CORRECTED.
- **Encoder and licensing context:** [D-39], [D-40], [D-42], [D-44], [D-45], [D-46] (CORRECTED), [D-48], [F-34] (CORRECTED), [F-43], [G-22].
- **Kernel versions and OS lifecycle:** [E-37], [G-55] (CORRECTED), [G-56].
- *(Added 2026-10-08.)* **H.265 packages, versions and version limits (topic H):** [H-01], [H-02], [H-03], [H-04], [H-05], [H-07], [H-08], [H-09], [H-11], [H-12] (CORRECTED), [H-15], [H-25], [H-26], [H-27], [H-30], [H-31]; context [D-24], [D-31]. Since 2026-10-08 these are evidence for REQ-ENC-002 (deferred); [H-08], [H-09] also support the current-scope statement that `libx265-215` arrives with `ffmpeg`.
- *(Added 2026-10-08.)* **HDMI audio overlay, kernel options and packages (topic I):** [I-04], [I-05], [I-06], [I-07], [I-08], [I-10], [I-11], [I-12], [I-22], [I-30], [I-31], [I-34], [I-38], [I-39], [I-41], [I-42], [I-43], [I-44], [I-45], [I-46]; official audio-overlay statement [C-37]. Community-tier, worded as a report: [I-16].

Items marked *research gap* or *research open question* are **not** register facts and remain NEEDS VERIFICATION.

### Verified on PACSCORDER hardware

**Nothing.** No hardware exists as of 2026-10-06. No image has been built, flashed or booted. No command in this document has been run for PACSCORDER. TEST-BLD-001 is NOT STARTED, and TEST-PLT-001 is BLOCKED — HARDWARE REQUIRED.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review pass against [REFERENCES.md](REFERENCES.md): §1 purpose marked PROPOSED; lane-count row and Compute Module storage caveat added to §1.3; libcamera statement attributed to community-tier [C-41]; CMA, I2C mux, module-autoload, `gst1-plugins-rs` and Kconfig-table wording aligned with the cited facts; `gstreamer1.0-plugins-base` added to the bring-up install command; TEST-BLD-001 procedure pointer aligned with [TESTING.md](TESTING.md); OQ resolution markers aligned with [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: `config.txt` overlay syntax now states the only attested forms (`<param>=<val>` [G-12], `,cam0` [C-39]) and marks bare booleans, several parameters per line, unknown parameters and `[pi4]` matching CM4 NEEDS VERIFICATION (OQ-100); version-recording, installed-package-list, shipped-kernel-config and `rpi-image-gen` build-command gaps linked to OQ-101; update policy versus active recordings linked to OQ-094 (§1.7, §2.4); driver-source difference 6.18.50 vs 6.18.55 linked to OQ-097 (§1.5, §4), and 6.18.39 labelled as an archive version [G-07] and the kernel of the community report [C-33] only; EDID provisioning trigger OQ-093 and configuration store OQ-092 added to §2.8; board storage/Ethernet/USB facts OQ-098; §7 table gains OQ-094, OQ-097, OQ-100, OQ-101; §2.9 package-list row no longer implies ADR-007 (OPEN) has chosen a framework; `tc358743-audio` row notes Pi 5/CM5 support is open (OQ-054); [C-33] added to the verification list | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: REQ-BLD-002 (own project-built OS image) and the owner input to ADR-003 added to the header, intro, Current status table, build-paths table (stock image is bring-up only), §1 purpose, §2 status, §3 status (Buildroot would also satisfy REQ-BLD-002; why ADR-003 still prefers `rpi-image-gen`), §5 layout and §7 OQ-012 row — ADR-003 still PROPOSED, OQ-012 still OPEN; REQ-CAP-007 (2-lane and 4-lane configurations) added to the header, §1.3 (per-configuration candidates, ADR-004 per configuration), §1.4 (`4lane` setting per configuration, reasoning from [G-12], [G-13]), §2.3 and §2.9 device layers, §5 EDID note (OQ-002) and §7 OQ-021 row; §6 TEST-BLD-001 "Verifies" extended to REQ-BLD-002 per the canonical table, with a REQ-BLD-002 evidence item and boot per capture configuration. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); header status and traceability (ADR-003 ACCEPTED, OQ-012 ANSWERED), intro, owner-input paragraph, Current status rows, build-paths table heading, §1 status and purpose, §1.7 bring-up rules attribution (rules stay PROPOSED; ADR-003 marked ACCEPTED), §2 heading (anchor changed to `#2-production-image-adr-003-accepted-rpi-image-gen`; the three internal links updated), §2 status and intro, §2.6 mitigation note (the package mirror is part of accepted ADR-003; method still OQ-067), §3 status, §4 table row and kernel note, §5 status, population note and layout comment, §6 procedure note, §7 OQ-012 row. Implementation status unchanged (NOT STARTED); other ADR statuses unchanged; no citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-08 | Second set of owner decisions of 2026-10-07 (CM4 + CM5 bring-up; HDMI audio required; H.264 + H.265) and research topics H and I propagated. Header (Last updated, Applies to, Verification, Traceability: REQ-ENC-001, REQ-CAP-006, RISK-022, RISK-025, OQ-054, OQ-105, OQ-107, OQ-113, OQ-114); new intro paragraph; research-gap source note extended to the 2026-10-08 JSON; §1.3 rows for H.265 encode, audio kernel options, CPU I2S and bring-up board; §1.4 audio-overlay row extended ([C-37], [I-16] community, [I-05], [I-06], [I-04], [I-30], [I-31]; OQ-114, OQ-100); §1.5 three kernel-option rows ([I-12], [I-11], [I-22]); §1.6 rows for `gstreamer1.0-plugins-bad`, `gstreamer1.0-plugins-base` and `ffmpeg` extended with their H.265/audio contents ([H-08], [H-09], [H-11], [H-12], [H-30], [I-39], [I-41], [I-44], [I-45]), new rows `x265`/`libx265-215` 4.1-2, `gstreamer1.0-alsa` 1.26.2-1+rpt3+deb13u2, `gstreamer1.0-libav` 1.26.2-1+deb13u1, `libopus0` 1.5.2-2, `libvo-aacenc0`, alsa-lib 1.2.14 and `fdk-aac` 2.0.3-1 (listed as not to be installed); "FFmpeg build options" NEEDS VERIFICATION marked superseded by [H-08], [H-09], [I-39]; four new NEEDS VERIFICATION items (FFmpeg ALSA input, `arecord`, non-free sources, x265 SIMD/OQ-105); second bring-up install command for `gstreamer1.0-alsa gstreamer1.0-libav x265` (NOT YET RUN); new §1.8 version limits of the distribution media stack ([H-27], [F-34], [H-15], [H-31], [H-07], [H-25], [H-26]) with build consequences; §2.8 two hardening rows (`fdk-aac` excluded; audio overlay and GPIO 18–21); §2.9 two configuration rows; §3.6 Buildroot H.265/audio row (NEEDS VERIFICATION); §3.8 pitfall 14 extended; §7 OQ-054, OQ-105, OQ-107, OQ-113, OQ-114 rows; Verification status gains topics H and I, [C-37], [D-24], [D-31]. No requirement, decision, risk or test status changed; implementation status unchanged (NOT STARTED). | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header Traceability (REQ-ENC-001 H.264 only, REQ-ENC-002 DEFERRED; RISK-022, RISK-025, OQ-105, OQ-107 not in current scope); intro "H.264 and H.265" sentence marked superseded and new "Owner decision of 2026-10-08" paragraph (no H.265 package needed; `libx265-215` still arrives with `ffmpeg` [H-09]); §1.3 H.265 row, §1.6 x265 SIMD NEEDS VERIFICATION item, §1.8 (new label line; intro wording), §2.9 H.265 part, §3.6 Buildroot H.265 item, §3.8 pitfall 14 and §7 OQ-105 / OQ-107 rows labelled "deferred — REQ-ENC-002; not in current scope"; §1.6 `gstreamer1.0-plugins-bad` (`x265enc` installed but unused), `ffmpeg` (still pulls in `libx265-215`) and `x265`/`libx265-215` (CLI only if REQ-ENC-002 is re-activated) rows annotated; additional install command reduced to `gstreamer1.0-alsa gstreamer1.0-libav` (still NOT YET RUN), with `x265` only if REQ-ENC-002 is re-activated; §2.9 note that the §1.8 gaps apply only on re-activation; Verification status note on topic H. No other decision, status or citation changed. | Claude (session 2026-10-08) |
