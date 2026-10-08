# PACSCORDER Decision Log (ADRs)

| | |
|---|---|
| Document status | Active — 2 ACCEPTED, 4 PROPOSED, 2 OPEN |
| Last updated | 2026-10-08 |
| Applies to | PACSCORDER architecture on all four candidate platforms: Pi 4 Model B, CM4, Pi 5, CM5 |
| Verification | Evidence is from source research of 2026-10-06 and 2026-10-08 (topics H and I) ([REFERENCES.md](REFERENCES.md)). No decision has been validated on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 13 (decision log), Rule 14 (never delete) |

Every architectural or technical decision gets an ADR. ADRs are never edited to change their meaning. A changed decision gets a new ADR that marks the old one `SUPERSEDED BY ADR-NNN` (Rule 13).

**Decision status values**

| Status | Meaning |
|---|---|
| `ACCEPTED` | The owner has decided. This includes decisions stated in the owner's engineering rules. |
| `PROPOSED` | Claude's recommendation, with evidence. It does not take effect until the owner accepts it. |
| `OPEN` | Needs a decision; options and criteria are recorded. |
| `SUPERSEDED` | Replaced by a later ADR. |

Fact references such as `[C-37]` point to the source register [REFERENCES.md](REFERENCES.md). Where a fact's verdict is anything other than CONFIRMED, that is stated next to the fact.

## Index

| ID | Title | Status | Date |
|---|---|---|---|
| ADR-001 | Use kernel V4L2 / Media Controller for capture | ACCEPTED | 2026-10-06 |
| ADR-002 | Use the in-tree `tc358743` Linux driver as the baseline | PROPOSED | 2026-10-06 |
| ADR-003 | OS and image basis: Raspberry Pi OS Lite (64-bit) + `rpi-image-gen` | ACCEPTED | 2026-10-07 (proposed 2026-10-06) |
| ADR-004 | Product target platform | OPEN | 2026-10-06 |
| ADR-005 | Default capture pixel format: UYVY (YCbCr 4:2:2 8-bit) | PROPOSED | 2026-10-06 |
| ADR-006 | One control model: Media Controller mode on every platform | PROPOSED | 2026-10-06 |
| ADR-007 | Userspace media framework | OPEN | 2026-10-06 |
| ADR-008 | CSI-2 link frequency: keep the default 486 MHz | PROPOSED | 2026-10-06 |

---

# ADR-001

## Title

Use kernel V4L2 / Media Controller for capture.

## Status

ACCEPTED — 2026-10-06. Source: owner's engineering rules, Rule 5 (mandated pipeline) and Rule 13 (this decision is the rules' own ADR example).

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
- *(Added from research, 2026-10-06.)* libcamera is not an option for this bridge regardless. The TC358743 is not a raw sensor and exposes no `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE` control [B-16], [C-19]. Raspberry Pi engineers reported (community source) that libcamera does not support it [C-41].

---

# ADR-002

## Title

Use the in-tree `tc358743` Linux driver as the baseline.

## Status

PROPOSED — 2026-10-06. Awaiting owner decision (OQ-013).

## Context

The rules' examples mention a "PACSCORDER TC358743 driver" (Rule 4 changelog example, Rule 15 commit example). That suggests the owner may intend a project-specific driver. Research found the following:

- A mature driver already exists: `drivers/media/i2c/tc358743.c`. It is identical in mainline Linux and in the Raspberry Pi default branch `rpi-6.18.y`, apart from one formatting difference [A-48], [B-02]. Both Raspberry Pi kernel configurations build it as a module [E-39], [G-16].
- The driver was written against non-public Toshiba documents [A-42]. Only a 20-page summary datasheet is public, with no register map [A-42]. A new driver would have to be written without the register documentation, unless an NDA datasheet is obtained.
- Known defects and limitations in the existing driver:
  - An unsupported reference-clock rate leads to a kernel `BUG_ON` instead of a clean probe failure (kernel source [A-22] and reasoning from it [B-11], both CORRECTED).
  - The FIFO trigger level is hard-coded at 374 [B-12]. Image corruption at 1080p50 RGB888 on 3 of 4 lanes is reported in an open community issue [C-43].
  - The driver reports a continuous CSI-2 clock while the Device Tree requests a non-continuous one [B-37].
  - Interlaced input is unsupported (reasoning from driver source) [B-27].

## Decision (proposed)

Use the in-tree driver unmodified for bring-up. Any defect proven on PACSCORDER hardware is to be corrected by a kernel patch carried in this repository and offered upstream; the driver is not forked. A separate PACSCORDER driver is written only if a later ADR records a defect that cannot be corrected by patching.

## Alternatives Considered

- Write a new PACSCORDER TC358743 driver. Not proposed for bring-up: there is no public register documentation [A-42], and the existing driver already covers probe, EDID, DV timings, CSI-2 configuration and events [B-15], [B-38].
- Fork the driver as an out-of-tree module. Possible later. It adds maintenance cost on every kernel update.
- Patch the in-tree driver. Proposed for defects found on hardware.

## Consequences

- Bring-up can start as soon as hardware exists, using the stock kernel module and overlays.
- Driver documentation ([TC358743_DRIVER.md](TC358743_DRIVER.md)) describes the in-tree driver. Its known defects are tracked in [RISKS.md](RISKS.md).
- If the owner wants a project-specific driver, this ADR must be rejected and a new ADR written.

---

# ADR-003

## Title

OS and image basis: Raspberry Pi OS Lite (64-bit) for bring-up, `rpi-image-gen` for production images; Buildroot kept as the documented alternative.

## Status

**ACCEPTED — 2026-10-07.** The owner wrote "accept ADR-003" (2026-10-07), after Claude restated the proposal and its consequences. The history below is kept unchanged (Rule 21).

PROPOSED — 2026-10-06. The owner said: "which ever is best that raspberry pi os" (answer to the build-system question, 2026-10-06). Claude reads this as: choose the best option, with a preference for Raspberry Pi OS. Awaiting owner acceptance (OQ-012).

Owner input, 2026-10-07: "which is best i need by own one". Recorded as requirement REQ-BLD-002 (the product runs its own project-built OS image); the choice of build tool is again delegated to Claude's recommendation. The proposal is unchanged: the product image is the project's own OS, built with `rpi-image-gen`. Both `rpi-image-gen` and Buildroot produce a project-owned image. `rpi-image-gen` is preferred because REQ-CAP-007 now requires both 2-lane and 4-lane configurations, which may put the two configurations on different board families (for example Pi 4 Model B for 2-lane [C-01] and Pi 5 for 4-lane [C-04]; ADR-004, OPEN). Reasoning from register inputs (reasoning tier, CORRECTED): one Raspberry Pi OS Lite image ships both kernels, with module trees that include `tc358743`, Unicam and RP1 CFE, and boots all four candidate boards [G-71]. Whether that image contains the compiled `tc358743` overlays is NEEDS VERIFICATION (Context below). Buildroot 2026.08 pins kernel 6.12.61 [E-06] and does not install overlays in its Pi 5 defconfig [E-15], and the Buildroot LTS has no CM5 defconfig [E-05] and no `tc358743-pi5.dtbo` [E-53]. Status remained PROPOSED until the owner explicitly accepted it on 2026-10-07 (above).

## Context

Facts verified from sources ([REFERENCES.md](REFERENCES.md)) as of 2026-10-06; nothing here has been checked on PACSCORDER hardware:

**Raspberry Pi OS**

- The current release is Raspberry Pi OS Lite 64-bit 2026-10-06, based on Debian 13 "trixie", with kernel 6.18.50 [G-01], [G-04].
- That image ships `tc358743.ko.xz` for both the Pi 4 family (`rpi-v8`) and the Pi 5 family (`rpi-2712`) kernels [G-16]. The Raspberry Pi `rpi-6.18.y` defconfigs build both Unicam drivers, both RP1 CFE drivers and `bcm2835-codec` as modules [G-17], [G-18]. Reasoning from register inputs (reasoning tier, CORRECTED): the image's module trees include `tc358743`, Unicam, RP1 CFE and `bcm2835-codec` [G-71]. The `tc358743`, `tc358743-pi5` and `tc358743-audio` overlays are documented in the `rpi-6.18.y` overlay README [G-12], [G-13], [G-14]; that the image itself contains the compiled overlays is NEEDS VERIFICATION on the image ([DEVICE_TREE.md](DEVICE_TREE.md) §3.7).
- Reasoning from register inputs: one image boots all four candidate boards [G-71] (reasoning tier, CORRECTED verdict; the correction concerns per-board differences, not the boot claim).
- `v4l2-ctl` and `media-ctl` are preinstalled [G-24]. GStreamer 1.26.2 [G-26], [G-27], [G-28], FFmpeg 7.1.5 (Raspberry Pi build) [G-29], x264 [G-30] and i2c-tools [G-25] are in the archives.

**`rpi-image-gen`**

- It is Raspberry Pi's official tool for building custom images from Raspberry Pi OS and Debian packages, announced as an alternative to `pi-gen` [G-36] (CORRECTED). It is BSD-3-Clause licensed; the latest release is v2.8.0 of 2026-08-13 [G-37].
- It builds from YAML configs and layers [G-38]. The announcement says it produces an SBOM for every build [G-36]; its README says it can generate SBOM and CVE reports [G-39].
- It provides an immutable A/B layout (`image-rota`) with EROFS, dm-verity and LUKS2 options [G-40], [G-41].
- Its README says it integrates with `rpi-sb-provisioner` to set up signed boot and encrypted file systems [G-39]; `rpi-sb-provisioner` is described as "a minimal-input automatic secure boot provisioning system for Raspberry Pi devices" [G-46].
- Limitations: it builds only on native arm64 Debian hosts; containers and QEMU are "not formally supported" [G-39]. Minor releases have contained breaking changes [G-44].

**Buildroot**

- Buildroot 2026.08 pins an older Raspberry Pi kernel, 6.12.61 [E-06], [G-59], and GStreamer 1.24.13 / FFmpeg 6.1.5 [G-64].
- Its Pi 5 defconfig disables overlay installation [E-15], [G-61] and forces 4K pages [E-09], [G-60].
- Buildroot gives the smallest image and pins every package version and hash [G-62], [G-64]. Bit-identical output is experimental [G-66].
- The current Buildroot LTS (2025.02.x) has no CM5 defconfig [E-05], and its firmware lacks `tc358743-pi5.dtbo` [E-53].

## Decision (accepted 2026-10-07; wording as proposed)

1. **Bring-up (now):** stock Raspberry Pi OS Lite 64-bit (trixie), with the kernel and firmware as shipped. This removes every build-system variable while the TC358743 path is first proven on hardware.
2. **Production image:** built with `rpi-image-gen` from the same Raspberry Pi OS packages, configured from files in this repository (REQ-BLD-001), with a package mirror for reproducibility (see Consequences).
3. **Alternative:** Buildroot remains documented in [BUILD_SYSTEM.md](BUILD_SYSTEM.md). Re-evaluate it if measured boot time, image size or reproducibility fails requirements that do not exist yet.

## Alternatives Considered

- Buildroot (described above).
- Yocto with `meta-raspberrypi`: not researched (research gap, topic G — not a register fact). Recorded as NEEDS EVALUATION if the owner wants it compared (OQ-012).
- `pi-gen`, the tool used for the official images [G-34], [G-35]: superseded by `rpi-image-gen` for custom images [G-36].

## Consequences

- Driver, overlay and firmware behaviour is identical to what Raspberry Pi tests and documents.
- Image size and boot time are expected to be larger and slower than Buildroot (Claude's reasoning). **No measurement exists**, so this is not quantified (OQ-068).
- Reproducibility needs a mirror or snapshot of `archive.raspberrypi.com`. A Debian snapshot layer exists, but no equivalent was found for the Raspberry Pi archive (research gap, topic G — not a register fact; OQ-067, RISK-017).
- Production hardening steps are required (OQ-071):
  - pin or freeze the EEPROM bootloader;
  - remove `cloud-init`, `rpi-connect-lite` and `rpi-update` if they are not wanted;
  - provision the EDID at every boot (REQ-CAP-003; trigger and ordering: OQ-093).
- Licence compliance: the GPU firmware is proprietary [G-69] (CORRECTED verdict; OQ-088). x264 is GPL [D-47]; FFmpeg must be built with `--enable-gpl` to use `libx264` [D-42]; in Buildroot, setting `BR2_PACKAGE_FFMPEG_GPL=y` adds GPL-2.0+ to FFmpeg's licence [D-46] (CORRECTED) (OQ-087).

---

# ADR-004

## Title

Product target platform.

## Status

OPEN — 2026-10-06. Owner: "Undecided — keep all four" (OQ-011).

Owner input, 2026-10-07: "i need 2 lane and 4 lane with all frame rate" (REQ-CAP-007). The product therefore has at least two capture configurations, and this ADR becomes a choice per configuration. 2-lane candidates: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port. 4-lane candidates: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05]. Still OPEN.

Owner input, 2026-10-07 (second answer): bring-up evaluates **CM4 and CM5 side by side** ("CM4 + CM5 side by side"). CM4 offers a 2-lane port (CAM0) and a 4-lane port (CAM1) with a hardware H.264 encoder [C-02], [D-10]; CM5 offers two 4-lane ports [C-05] and encodes in software [D-31], [G-22]; a 2-lane configuration on CM5 would use a 2-lane bridge board on a 4-lane port (reasoning; OQ-021). The final product platform is decided from the TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 results. Still OPEN.

## Context and decision criteria

| Criterion | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| CSI-2 lanes to TC358743 | 2 [C-01] | CAM1: 4, CAM0: 2 [C-02] | 4 per port [C-04] | 4 per port [C-05] |
| 1080p60 capture possible (bandwidth; reasoning-tier calculations) | **No** [C-37], [C-48] | Yes on CAM1 [C-37], [C-49] | Yes by bandwidth [C-49] | Yes by bandwidth [C-49] |
| Official RPi TC358743 documentation | Yes (2-lane) [C-37] | Yes (4-lane CM example) [C-37] | **None** [C-38] | **None** [C-38] |
| CSI-2 receiver | Unicam [C-09] | Unicam [C-09] | RP1 CFE, Media Controller only [C-11], [C-29] | RP1 CFE [C-29] |
| Hardware H.264 encoder | Yes, spec 1080p30 [D-10] | Yes, spec 1080p30 [D-10] | **No** [D-31], [G-22] | **No** [D-31] |
| Software H.264 1080p30 encode cost | n/a | n/a | ~30–40% CPU (official) [G-22] | same SoC |
| Known platform issues | — | — | CFE programs D-PHY for 999 Mbps for this bridge [C-31]; reasoning: that matches only the default link rate [C-52] (OQ-050; ADR-008); source-change events are reported not to reach the video node (community source) [C-42] (OQ-051) | as Pi 5; on the CM5 IO Board the CAM/DISP1 jumper documentation conflicts with a DT comment (research gap, topic C — not a register fact; OQ-052) |

The "1080p60 capture possible" row is a bandwidth statement only. Reasoning from the driver's lane formula at 972 Mbps: 1080p60 UYVY (ADR-005, PROPOSED) activates 3 of the 4 configured lanes [C-47]. Capture with 3 of 4 lanes is unproven on both receivers (OQ-038), and image corruption on 3 of 4 lanes (1080p50 RGB888) is reported in a community issue [C-43] (RISK-006). A 4-lane port is therefore necessary for 1080p60 but not yet shown to be sufficient. ADR-008 (PROPOSED; OQ-099) proposes evaluating 297 MHz on a CM4 CAM1 4-lane link, where 1080p60 UYVY uses all 4 lanes.

## Analysis (Claude's reasoning — not a decision)

- If 1080p60 (REQ-CAP-001) is mandatory, **Pi 4 Model B is excluded**. *(Superseded in scope by the owner input of 2026-10-07: 1080p60 is required on 4-lane configurations, and Pi 4 Model B remains a 2-lane candidate under REQ-CAP-007.)*
- CM4 on CAM1 is the only candidate with both an officially documented 4-lane 1080p60 TC358743 configuration and a hardware H.264 encoder. That encoder is officially specified only to 1080p30 [D-10], and a Raspberry Pi engineer reported 1080p60 as an "edge case" on the hardware encoder (community source) [D-50], so 1080p60 hardware encode is **unproven** (RISK-002, OQ-056).
- Pi 5 and CM5 have the lanes and CPU, but no official TC358743 documentation. They also need software encoding and per-frame format conversion [D-43].
- Recommended next step: evaluate CM4 (CAM1) and Pi 5/CM5 side by side during bring-up, using TEST-CAP-002 and TEST-ENC-001. Then decide this ADR on measured results. This covers the 4-lane configuration. For the 2-lane configuration of REQ-CAP-007 (owner, 2026-10-07), TEST-CAP-004 records the supported-mode matrix on the chosen 2-lane candidate (Pi 4 Model B, CM4 CAM0 or a 2-lane bridge board).
- *(Added 2026-10-08, research topic H — H.265 is required, REQ-ENC-001.)* H.265 is software-encoded on both CM4 and CM5 [D-24], [D-31], so CM4's hardware H.264 encoder does not help it. x265's Neon DotProd kernels can apply only on CM5's Cortex-A76; CM4's Cortex-A72 has no DotProd, and I8MM, SVE and SVE2 paths apply on neither [H-04], [H-05]. Cost evidence is weak: research found no official H.265 figure [H-19]; a Raspberry Pi engineer stated on the forum that software H.265 encode "is too intensive an operation to perform at any significant resolution" (community source) [H-19]; community benchmarks report `libx265` "Live" results of 10.00 FPS on Pi 5 and 4.33 FPS on a Cortex-A72 Pi 400, in a test that is not a 1080p60 live measurement (community sources) [H-20], [H-21], [H-22]; reasoning: in that harness `libx265` was about 6.6 times slower than `libx264` on Pi 5 [H-23]. Claude's reading (reasoning, not a measurement): if H.265 is required at 1080p, CM5 is the more likely of the two to sustain it; neither is shown to do so. TEST-ENC-001 should include H.265 runs on both boards (OQ-104, OQ-105; RISK-022). On CM5, an H.264 WebRTC track alongside H.265 would be a second concurrent software encode (OQ-108).
- *(Added 2026-10-08, research topic I — HDMI audio is required, REQ-CAP-006.)* CM4: the `tc358743-audio` overlay drives the single `bcm2835-i2s` on GPIO 18–21, which captures exactly 2 channels [I-10], [I-13]. CM5: `overlay_map` has no entry for the overlay, so the firmware does not block it [I-05], [I-06], and its labels resolve to RP1 I2S1 on GPIO 18–21 [I-07]; but no official statement or test result shows audio being captured through this path on CM5 (research gap, topic I; OQ-054). Reasoning: HDMI audio is therefore a bring-up gate on CM5 that TEST-AUD-001 must pass before CM5 can be chosen for the product.

## Decision

Not yet made.

---

# ADR-005

## Title

Default capture pixel format: UYVY (`MEDIA_BUS_FMT_UYVY8_1X16`).

## Status

PROPOSED — 2026-10-06 (OQ-003).

## Context

The driver offers RGB888 (`RGB888_1X24`, the probe default) and UYVY (`UYVY8_1X16`) [A-09], [B-34].

- Reasoning: UYVY needs two-thirds of RGB888's bandwidth, 1.99 Gbit/s versus 2.99 Gbit/s at 1080p60 [C-46].
- UYVY allows 1080p50 on 2 lanes (official documentation [C-37]; reasoning-tier lane calculation [C-48]).
- The open image-corruption report, a community issue, concerns RGB888 near lane boundaries [C-43]. Reasoning (inputs [C-48]): 1080p50 UYVY on 2 lanes loads each active lane at 85.3 % of 972 Mbps, the same as the reported 1080p50 RGB888 case on 3 lanes (128 % of 2 lanes = 85.3 % of 3 lanes). UYVY therefore does not by itself remove that risk on 2-lane links (RISK-006, OQ-035).
- The two receivers label RGB888 differently: Unicam (Pi 4/CM4) maps `RGB888_1X24` to `RGB24`, RP1 CFE (Pi 5/CM5) to `BGR24` [C-34]. On CM4 the bytes in memory were reported as B, G, R, so software that trusts Unicam's `RGB24` label needs a swap (community source) [C-35] (OQ-045).
- UYVY uses BT.601 limited range and is reported as `SMPTE170M` [B-34]. Research found that this is so even for 720p/1080p sources (research gap, topic B — not a register fact; OQ-041). Encoder colour metadata must match.

## Decision (proposed)

Configure UYVY by default on all platforms. Use RGB888 only if a later ADR justifies it.

## Consequences

- Pi 4/CM4: a Raspberry Pi engineer reported that the hardware encoder accepts UYVY directly [D-18], and a 2021 encoder input-format listing posted by a Raspberry Pi engineer includes UYVY [D-17] (both community sources; OQ-057).
- Pi 5/CM5: `libx264` / `x264enc` do not accept packed UYVY. A conversion to I420/NV12 is needed per frame, at CPU cost [D-40], [D-43] (OQ-060).

---

# ADR-006

## Title

One control model: Media Controller mode on every platform.

## Status

PROPOSED — 2026-10-06 (OQ-014).

## Context

- On Pi 4/CM4 the stock `tc358743` overlay switches Unicam to legacy video-node mode [B-43], [C-10]. In that mode EDID and DV-timings ioctls go through `/dev/videoN` [B-25] (CORRECTED verdict), and sub-device nodes are read-only [C-36] (CORRECTED verdict).
- The overlay's `media-controller` parameter selects Media Controller mode. EDID and DV timings must then go through `/dev/v4l-subdevN` [B-25], [C-36].
- Pi 5/CM5 (RP1 CFE) support Media Controller mode only, and EDID and DV timings must use the sub-device node there [B-25], [C-11]. A Raspberry Pi engineer reported that source-change events must also be subscribed on the sub-device node (community source) [C-42] (OQ-051).
- On Pi 5/CM5 the csi2 → video-node link starts disabled and must be enabled by userspace [C-32]. A Raspberry Pi engineer reported a sequence that enables the link and sets the pad formats with `media-ctl` (community source) [C-33].

## Decision (proposed)

Use Media Controller mode on all platforms. On Pi 4/CM4 this means `dtoverlay=tc358743,media-controller`. Control the TC358743 only through its sub-device node, so one capture application and one procedure serve every platform.

*Syntax note (added 2026-10-06, cross-document review; the decision is unchanged):* the source register attests the overlay form `dtoverlay=tc358743,<param>=<val>` [G-12] and appending `,cam0` [C-39]. Giving `media-controller` by name alone, as above, and combining it with other parameters on one line are NEEDS VERIFICATION against the official `config.txt` documentation ([DEVICE_TREE.md](DEVICE_TREE.md) §6.0; OQ-100).

## Alternatives Considered

- Legacy video-node mode on Pi 4/CM4 and Media Controller on Pi 5/CM5. Simpler on Pi 4, and it matches the official Raspberry Pi example [C-37], but it doubles the control code and test matrix.

## Consequences

- The capture application and test procedures use `media-ctl` and `v4l2-ctl` on the TC358743 sub-device node (`/dev/v4l-subdevN`) everywhere. The exact `v4l2-ctl -d` form with a sub-device path is NEEDS VERIFICATION: the register attests `-d 11` [D-17] and running the timing command "on /dev/v4l-subdevN" [C-33] (both community sources), but not the combined form ([V4L2.md](V4L2.md) §7; OQ-101).
- Research found that Unicam in Media Controller mode filters `ENUM_FMT` such that UYVY may not be listed even though `S_FMT` works (research gap, topic C — not a register fact; OQ-046). The application must not rely on filtered `ENUM_FMT`.

---

# ADR-007

## Title

Userspace media framework for capture, encode, record and stream.

## Status

OPEN — 2026-10-06 (OQ-015).

## Options and evidence

| Option | Evidence for | Evidence against |
|---|---|---|
| **GStreamer** (`v4l2src`, `v4l2h264enc`, `x264enc`, `flvmux` + `rtmp2sink`, `webrtcbin`) | Covers capture, hardware encode, RTMP and WebRTC in one framework [F-33], [F-42]. V4L2 M2M elements support `dmabuf-import` I/O modes [D-53]. Official Raspberry Pi documentation gives a `v4l2h264enc` streaming pipeline [D-37]. Packaged in Raspberry Pi OS (1.26.2) [G-26], [G-27]. | `v4l2h264enc` exists only if V4L2 probing is compiled in. Raspberry Pi OS/Debian enable it; Buildroot needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y` [E-32], [D-39]. `webrtcbin` has no built-in signalling [F-42]. |
| **FFmpeg** (`h264_v4l2m2m`, `libx264`, `-f flv`) | Simple RTMP publishing [F-32]. Raspberry Pi OS's patched FFmpeg adds DMABUF input to the V4L2 M2M encoder [D-45]. | Upstream FFmpeg's V4L2 M2M uses MMAP only, so every frame is copied [D-44]. WebRTC output: no register fact covers it, so "no WebRTC" is NEEDS VERIFICATION ([DMA.md](DMA.md), [STREAMING.md](STREAMING.md)). `libx264` needs a GPL build [D-42]. |
| **Direct V4L2 application** (the model `rpicam-apps` uses) | Full control of buffers and DMABUF import [D-34]. | Recording, RTMP and WebRTC must be built or linked separately. |

All D-series facts cited above are `CONFIRMED` in [REFERENCES.md](REFERENCES.md).

### Evidence added 2026-10-08 (research topics H and I; the options and status are unchanged)

H.265 (required by REQ-ENC-001) and HDMI audio (required by REQ-CAP-006) add the following evidence. All cited H and I entries have verdict `CONFIRMED` or `CORRECTED`.

| Option | Evidence for (added 2026-10-08) | Evidence against (added 2026-10-08) |
|---|---|---|
| **GStreamer** (Raspberry Pi OS 1.26.2) | `x265enc` is shipped in plugins-bad [H-11], [H-12] (CORRECTED). `qtmux`/`mp4mux` and `matroskamux` accept H.265 [H-37]. `mpegtsmux` accepts H.265, and the SRT and MPEG-TS plugins are shipped [H-30]. Audio encoders `voaacenc` and `opusenc` are shipped, and `avenc_aac` is available [I-44], [I-45], [I-46]. | `flvmux` has no H.265, so the distribution's GStreamer cannot mux HEVC into FLV/RTMP [H-27]; `eflvmux` arrives only in 1.28 (CORRECTED) [F-34]. `x265enc` reports a hard-coded 5-frame latency unless `tune=zerolatency`; the fix arrived in 1.26.8 and is not backported [H-15]. `rtph265pay` lacks profile, tier and level in its caps until 1.26.4 [H-31]. A/V timestamps: in a `v4l2src` + `alsasrc` pipeline, `alsasrc` normally provides the pipeline clock and then does not use ALSA driver timestamps [I-35], [I-36]; `v4l2src` maps monotonic buffer timestamps through a measured delay and falls back to a one-frame delay after a bad timestamp [I-37]. |
| **FFmpeg** (Raspberry Pi build 7.1.5) | Can mux HEVC + AAC into enhanced FLV for RTMP publishing [H-26]; links `libx265` [H-09]; built with `--enable-libsrt` [H-08]; its MP4 and Matroska muxers handle HEVC [H-38]. Has the native `aac` encoder and `libopus` [I-39], [I-41]. | Its FLV muxer has no Opus [H-26]. Its `libx265` wrapper copies the thread count into x265's frame threads after applying tune [H-10] (CORRECTED); reasoning from [H-10] and [H-16]: the `-threads` setting replaces `tune=zerolatency`'s single frame thread unless set explicitly. Its ALSA input stamps packets with wall-clock time while its V4L2 input passes monotonic timestamps through by default, so the two clock bases are mixed unless `-ts abs` or `mono2abs` is used [I-38]. Linking `libx264`/`libx265` makes it a GPL build [I-39]. |
| **Direct V4L2 application** | Video buffers carry `CLOCK_MONOTONIC` frame-start timestamps on every Raspberry Pi CSI receiver driver [I-33], and alsa-lib 1.2.14 (the trixie version) switches each newly opened `hw` PCM to monotonic timestamps when the kernel PCM protocol is 2.0.9 or later [I-34]; reasoning: one timestamp clock is then available for both, although the audio samples are still clocked by the source [I-26]. | H.265 encoding, muxing and transport still have to come from x265 or FFmpeg libraries (reasoning from [H-09], [H-13]). |

Reasoning from [H-26], [H-27], [F-34]: if RTMP must carry H.265 (OQ-103) and GStreamer is chosen for the rest, HEVC RTMP needs FFmpeg, a backported or newer GStreamer, or SRT instead — a possible split GStreamer/FFmpeg architecture (RISK-025, OQ-107). Whichever framework is chosen must also define how audio and video timestamps are aligned (RISK-024, OQ-112) and how the audio sample rate follows the source (RISK-023, OQ-111).

## Decision

Not yet made. Decide after bring-up proves capture (TEST-CAP-002) and encode (TEST-ENC-001) on the candidate platforms (ADR-004).

*(Added 2026-10-08; the decision status is unchanged.)* The decision should also record the HEVC-over-RTMP path (OQ-107) and the A/V clock model (OQ-112), measured with TEST-AUD-001 and TEST-STR-001.

---

# ADR-008

## Title

CSI-2 link frequency: keep the overlay default of 486 MHz (972 Mbit/s per lane).

## Status

PROPOSED — 2026-10-06. Owner decision tracked as OQ-099. Added after the per-document reviews, because [CSI_PIPELINE.md](CSI_PIPELINE.md) and [DEVICE_TREE.md](DEVICE_TREE.md) both recommended this setting without a decision record.

## Context

- The overlay supports only `link-frequency` 297000000 or 486000000 (the default) [A-46], [B-45]. The driver has D-PHY timing tables only for 594 and 972 Mbit/s [A-24], [B-09].
- Reasoning from the driver's integer PLL arithmetic: with the 27 MHz reference clock assumed by the stock overlay, both rates come out exactly [A-23], [B-10].
- Lanes the driver requests (reasoning, [C-47], [C-49]):

  | Mode | 972 Mbit/s | 594 Mbit/s |
  |---|---|---|
  | 1080p60 UYVY | 3 lanes | 4 lanes |
  | 1080p60 RGB888 | 4 lanes | 6 lanes (rejected) |
  | 1080p50 UYVY | 2 lanes | 3 lanes |
  | 1080p50 RGB888 | 3 lanes | 5 lanes (rejected) |

- A driver comment says 594 Mbit/s is meant for 4-lane 1080p60 or 2-lane 720p60, and that 972 Mbit/s allows 1080p50 UYVY over 2 lanes [C-44].
- On Pi 5/CM5 the RP1 CFE driver falls back to 999 Mbit/s when it cannot determine a link rate, and that fallback is always taken for this bridge [C-31]; reasoning from driver source: the bridge exposes no link-rate control [B-49]. Reasoning: 999 Mbit/s matches the 972 Mbit/s default but not 594 Mbit/s [C-52].
- Capture with 3 active lanes of 4 configured is unproven (OQ-038), and image corruption has been reported near lane-count limits [C-43] (community report).

## Decision (proposed)

Use the default 486 MHz on every platform for bring-up, because (both reasons are reasoning from the register inputs cited):

- it is the only value consistent with the Pi 5/CM5 receiver configuration [C-52];
- it is the only rate that fits 1080p50 UYVY on a 2-lane link [C-48].

On a CM4 CAM1 4-lane link only, evaluate 297 MHz as an alternative for 1080p60 UYVY in TEST-CAP-002. That is the configuration the driver comment describes [C-44], and it avoids the unproven 3-of-4-lane case (OQ-038). Adopting it would need a new ADR.

## Alternatives Considered

- 297 MHz on all platforms: not proposed. Reasoning: it cannot carry 1080p50/60 RGB888 [C-49], it carries 1080p50 UYVY only on 3+ lanes [C-47], and it mismatches the Pi 5/CM5 receiver [C-52].
- Other link frequencies: unsupported by the overlay [A-46], and they fall back to the 594 Mbit/s timing constants with an "untested bps per lane" warning [A-24], [B-09].

## Consequences

- 1080p60 UYVY on a 4-lane link uses 3 active lanes at 972 Mbit/s. TEST-CAP-002 must show this mode is free of corruption (RISK-006). How to read CSI-2 error counters for that check is not in the source register (OQ-050 for CFE, OQ-095 for Unicam).
- The Device Tree baseline stays the unmodified stock overlay ([DEVICE_TREE.md](DEVICE_TREE.md)).

---

# Verification status

## Verified from sources (fact IDs)

The evidence in every ADR cites [REFERENCES.md](REFERENCES.md) entries with verdict `CONFIRMED` or `CORRECTED`. `CORRECTED` entries (A-22, B-11, B-25, C-36, C-39, D-46, G-36, G-69, G-71) are used in their corrected wording. Community-tier entries (C-33, C-35, C-41, C-42, C-43, D-17, D-18, D-50) are worded as reports. Reasoning-tier entries (A-23, B-10, B-11, B-27, B-49, C-46, C-47, C-48, C-49, C-52, G-71) are labelled as reasoning. Statements marked *research gap* come from the research JSON and are not register facts. "Verified from sources" means only that the cited source says so.

Added 2026-10-08 (research topics H and I, cited in ADR-004 Analysis and ADR-007): topic H facts H-04, H-05, H-08, H-09, H-10, H-11, H-12, H-13, H-15, H-16, H-19, H-20, H-21, H-22, H-23, H-26, H-27, H-30, H-31, H-37, H-38; topic I facts I-05, I-06, I-07, I-10, I-13, I-26, I-33, I-34, I-35, I-36, I-37, I-38, I-39, I-41, I-44, I-45, I-46. `CORRECTED` entries H-10, H-12 and F-34 are used in their corrected wording. Community-tier entries H-19, H-20, H-21 and H-22 are worded as reports. The reasoning-tier entry H-23 is labelled as reasoning. The topic I research gap on CM5 audio comes from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json) and is not a register fact.

## Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No ADR has been validated by a test. ADR-001 is ACCEPTED by the owner's rules, not by test evidence.

---

# Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06: ADR-001 (ACCEPTED), ADR-002, ADR-003, ADR-005, ADR-006 (PROPOSED), ADR-004, ADR-007 (OPEN). | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document review. **No decision, status or option changed.** Evidence and citation corrections only: non-standard citations ("[G-gap: …]", "[G open questions]", "[G gaps]", "[C gaps]") replaced by research-gap labels plus OQ-012, OQ-046, OQ-052, OQ-067, OQ-068; each ADR status linked to its OQ (OQ-003, OQ-011 to OQ-015); ADR-003 image-content claim limited to what [G-16], [G-17], [G-18], [G-71] support; SBOM statement re-cited to [G-36]; licence line re-cited to [D-47] / [D-42] / [D-46]; ADR-004 note that a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (OQ-038); ADR-005 RGB888 wording changed from "byte order differs" to "label differs" per [C-34], [C-35], the HD-source colourimetry statement attributed to the research gap (OQ-041), and the 2-lane UYVY lane-load reasoning added; ADR-006 syntax note (bare `media-controller` parameter and `v4l2-ctl -d` sub-device form are NEEDS VERIFICATION); ADR-007 uncited "No WebRTC" marked NEEDS VERIFICATION; community-tier facts worded as reports; stale "see REFERENCES.md for verdict" hedges removed; header rows, Verification status and Change history added. Cross-document consistency fixes (second pass, same date; no decision, status or option changed): header count corrected to 5 PROPOSED (ADR-008); ADR-002 alternatives worded as proposals ("Chosen" → "Proposed", "Rejected" → "Not proposed") and [B-11] labelled as reasoning; ADR-003 EDID provisioning linked to OQ-093; ADR-004 links ADR-008 / OQ-099 for the 4-lane-necessary-not-sufficient note and the CFE 999 Mbps row; ADR-006 syntax note and `v4l2-ctl -d` consequence linked to OQ-100 and OQ-101; ADR-008 reasoning labels added for [A-23]/[B-10], [B-49], [C-52], [C-48], its "rejected" alternative worded "not proposed", and the CSI-2 error-counter OQs (OQ-050, OQ-095) linked; reasoning labels added for [G-71] (ADR-003) and [C-48] (ADR-005); [D-17] and [C-33] in ADR-006 labelled as community sources; Verification status reasoning list completed (A-23, B-10, B-49). Final verification pass (same date; no decision, status or option changed): ADR-002 proposed decision worded "is to be corrected by a kernel patch" / "cannot be corrected by patching" instead of "fixed" (Rule 10 vocabulary; meaning unchanged); ADR-003 Context heading says the facts are verified from sources only, and the `rpi-sb-provisioner` line is attributed to the rpi-image-gen README [G-39] instead of "It works with". | Claude (session 2026-10-06) |
| 2026-10-06 | ADR-008 (CSI-2 link frequency, PROPOSED) added after the per-document reviews; owner decision tracked as OQ-099. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner input recorded in ADR-003 Status (own OS image, REQ-BLD-002; proposal unchanged, still PROPOSED) and ADR-004 Status/Analysis (2-lane and 4-lane configurations, REQ-CAP-007; still OPEN). No decision status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated (verification pass): ADR-003 Status no longer says REQ-CAP-007 "spans the Pi 4 family and the Pi 5 family" (both configurations could sit in one family; now "may put the two configurations on different board families", ADR-004 OPEN); the [G-71] sentence no longer claims the image carries "the tested kernel" and "both `tc358743` overlays" — it states what [G-71] supports (both kernels, module trees with `tc358743`, Unicam and RP1 CFE, boots all four boards) and keeps the overlays NEEDS VERIFICATION as in the Context; ADR-004 recommended next step adds the 2-lane configuration (TEST-CAP-004). No decision status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner ("accept ADR-003", 2026-10-07): Status, index row and header counts updated; decision text unchanged. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-004: owner input recorded — bring-up evaluates CM4 and CM5 side by side; the decision stays OPEN until measured. | Claude (session 2026-10-07) |
| 2026-10-08 | Research topics H and I propagated; **no decision, status or option changed**. ADR-004 Analysis: two dated bullets — H.265 is software on both CM4 and CM5, DotProd only on CM5's Cortex-A76 [H-04], [H-05], H.265 cost evidence (no official figure; community statement and community benchmarks that are not 1080p60 measurements; 6.6x reasoning) [H-19] to [H-23], H.265 runs to be included in TEST-ENC-001 (OQ-104, OQ-105); CM4 audio via `bcm2835-i2s`, CM5 audio labels resolve but operation unconfirmed [I-05], [I-06], [I-07] (OQ-054), so audio is a CM5 bring-up gate (reasoning). ADR-007: dated evidence table — GStreamer 1.26.2 cannot mux HEVC into FLV [H-27] while FFmpeg 7.1.5 can [H-26]; `x265enc` latency and `rtph265pay` caps limits [H-15], [H-31]; HEVC MP4/Matroska/MPEG-TS support [H-30], [H-37], [H-38]; audio encoders [I-39], [I-41], [I-44], [I-45], [I-46]; A/V timestamp handling differs between GStreamer and FFmpeg [I-35] to [I-38]; split-architecture reasoning (RISK-025, OQ-107); note that the decision should also record the HEVC RTMP path and A/V clock model. Header Verification row and Verification status updated. | Claude (session 2026-10-08) |
