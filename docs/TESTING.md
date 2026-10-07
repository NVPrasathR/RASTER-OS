# PACSCORDER Test Procedures and Results

| | |
|---|---|
| Document status | Active — 17 procedures defined; **no test has been run** |
| Last updated | 2026-10-07 |
| Applies to | PACSCORDER on every candidate platform (Pi 4 Model B, CM4, Pi 5, CM5), in both the 2-lane and the 4-lane CSI-2 configuration (REQ-CAP-007); TC358743 bridge; HDMI sources: ATEM switcher outputs and cameras connected directly (REQ-CAP-008); Raspberry Pi OS Lite 64-bit for bring-up and the project's own product image (REQ-BLD-002), as decided in ADR-003 (ACCEPTED 2026-10-07) |
| Verification | Procedures derived from the source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)). Nothing has been run on PACSCORDER hardware: no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 9 (test documentation), Rule 10 (status words), Rule 12 (traceability), Rule 21 (no retroactive edits), Rule 22 (unknowns) |

This document holds one procedure for each canonical test ID in [README.md](README.md), in the same order, written in the Rule 9 format. The requirement-to-test mapping is in [TRACEABILITY.md](TRACEABILITY.md). Failure signatures that a test may hit are in [TROUBLESHOOTING.md](TROUBLESHOOTING.md).

> **State on 2026-10-06:**
> - No PACSCORDER hardware exists.
> - No PACSCORDER code exists.
> - No test below has been run.
>
> Every hardware test is `BLOCKED — HARDWARE REQUIRED`. TEST-BLD-001 is `NOT STARTED` because no build configuration exists.

---

## Test status summary

| Test ID | Title | Verifies | Status |
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

---

## How to read and run these procedures

### Conventions

- **Commands.** Every command comes from a cited source and is marked **NOT YET RUN ON PACSCORDER HARDWARE**.
  - Where a source attests the action but not the exact command-line option, the step says `NEEDS VERIFICATION` instead of guessing syntax. These steps are tracked together in OQ-101.
  - **`v4l2-ctl -d /dev/v4l-subdevN`**, that is, a sub-device path given to `-d`, is NEEDS VERIFICATION (OQ-101). The register attests `-d 11` [D-17] and running the EDID and timing steps on `/dev/v4l-subdevN` [C-33], but not that option form. The `--set-edid pad=<pad>[,…]` and `--clear-edid <pad>` forms are attested as `v4l2-ctl` help text [B-24]; the argument form `--clear-edid 0` is NEEDS VERIFICATION (OQ-101).
  - Generic read-only actions, such as reading the kernel log or listing device nodes, are written as actions ("inspect the kernel log for …"). No register fact attests a specific command for them. The command actually used must be written into the result log entry.
- **Device names are placeholders.** `/dev/v4l-subdevN`, `/dev/mediaN` (`media-ctl -d <N>`), `/dev/videoN`, `<bus>` and entity names such as `"tc358743 1x-000f"` vary by board and connector, and a Raspberry Pi engineer reported that they changed between kernel versions on Pi 5 [C-28]. On PACSCORDER they are `UNKNOWN — VERIFICATION REQUIRED` (HARDWARE TEST REQUIRED, OQ-043). Record the actual names in every result entry.
- **Control model.** The procedures follow ADR-006, which is **PROPOSED, not accepted** (OQ-014): Media Controller mode on every platform, with EDID and DV timings issued on the TC358743 sub-device node.
  - On Pi 4/CM4 the stock overlay default is the legacy video-node mode instead [B-43], [C-10]. In that mode EDID and DV-timings ioctls go through `/dev/videoN` [B-25], and sub-device nodes are registered read-only [C-36].
  - If ADR-006 is rejected, the Pi 4/CM4 steps must be rewritten for legacy mode.
- **Expected results are source predictions, not acceptance criteria.** No requirement has owner-accepted acceptance criteria yet (OQ-017).
  - Quantitative thresholds (duration, allowed frame drops, latency, temperature) are `UNDEFINED`.
  - A result may be recorded as `TESTED — PASS` only against criteria the owner has accepted. Until then, record what was observed and compare it with the source-predicted result.
- **Community and reasoning sources.** Facts of tier `community` are worded "reported by …". Facts of tier `reasoning` are calculations or inferences from other cited facts and are labelled as reasoning.

### Common setup (applies to every hardware test)

| Item | Value |
|---|---|
| Platform | One of Pi 4 Model B, CM4, Pi 5, CM5. The product platform is **undecided** (ADR-004 OPEN, OQ-011). Run each test on every platform still under consideration. |
| Lane configuration | Both a 2-lane and a 4-lane CSI-2 configuration are required, each capturing every frame rate its link carries (REQ-CAP-007, DRAFT; owner, 2026-10-07). ADR-004 is now a choice per configuration (still OPEN). 2-lane: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port. 4-lane: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05]. Whether one bridge board can serve both is UNKNOWN (OQ-021). Record the lane configuration in every result entry. |
| Bridge board | UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, OQ-018). The lane count, REFCLK oscillator, INT/RESETN wiring, CAM_GPIO use and I/O voltage are all unknown (OQ-019, OQ-020, OQ-021, OQ-022, OQ-024). |
| HDMI source(s) | Source types: the HDMI output of Blackmagic ATEM switchers and cameras connected directly (REQ-CAP-008, DRAFT; owner, 2026-10-07). Which ATEM and camera models is OPEN — OWNER DECISION REQUIRED (OQ-102). The mode list and EDID are OWNER DECISION REQUIRED (OQ-002). |
| Power | UNKNOWN — VERIFICATION REQUIRED (OQ-023). The rules' "12V power" is an example, not a PACSCORDER specification. |
| OS image (bring-up) | Raspberry Pi OS Lite (64-bit) 2026-10-06 image, based on Debian 13 "trixie" [G-01], with Linux kernel 6.18.50 [G-04]. This follows ADR-003, which is ACCEPTED (owner, 2026-10-07; OQ-012 ANSWERED). The stock image is for bring-up only: the product runs the project's own built image (REQ-BLD-002, DRAFT), which TEST-BLD-001 builds and boots. Driver behaviour in the expected results comes from the source inspected in research, the `rpi-6.18.y` tip at 6.18.55 [E-37]; whether `tc358743.c` differs in the shipped 6.18.50 is OQ-097 (KERNEL SOURCE INSPECTION REQUIRED). |
| Tools in that image | `v4l2-ctl` and `media-ctl` (v4l-utils) are installed [G-24]. `i2c-tools` is in the archive but **not installed** [G-25]. **No GStreamer packages are installed** [G-32]. |
| Boot configuration file | `/boot/firmware/config.txt` [G-11] |
| Hardware revision | `—` (no PACSCORDER hardware revision exists; Rule 8 revision history is in [HARDWARE.md](HARDWARE.md)) |

### Platform reference (from sources; nothing measured on PACSCORDER)

| Item | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| CSI-2 data lanes at the connector | 2 [C-01] | CAM1: 4, CAM0: 2 [C-02] | 4 per port [C-04] | 4 per MIPI interface [C-05] |
| Overlay | `tc358743` [G-12] | `tc358743`. `4lane` applies to CAM1 only [A-46]; `cam0` retargets to CAM0 [B-42] | `dtoverlay=tc358743` is redirected to `tc358743-pi5` [C-11], [E-43]; parameters `4lane`, `link-frequency`, `cam0` [G-13] | as Pi 5 [C-11], [E-43] |
| Media Controller mode | `media-controller` overlay parameter [B-43], [C-10] | as Pi 4 | always [C-11] | always [C-11] |
| CSI-2 receiver driver | downstream Unicam, module `bcm2835-unicam-legacy` [C-09], [G-21] | as Pi 4 | downstream RP1 CFE, module `rp1-cfe-downstream` [C-29], [E-44] | as Pi 5 |
| Camera I2C bus (bridge expected at 7-bit 0x0f [A-15]) | `i2c-10` on GPIO 44/45 [C-24] | CAM1 `i2c-10` (GPIO 44/45); CAM0 `i2c-0` (GPIO 0/1) [C-24], [C-25] | CAM/DISP0 `/dev/i2c-10`, CAM/DISP1 `/dev/i2c-11` [C-26]; a Raspberry Pi engineer reported other numbers on earlier kernels [C-28] | depends on the carrier board [C-27]. CM5 IO Board: CAM/DISP 1 is RP1 i2c0 on GPIO 0/1, CAM/DISP 0 is RP1 i2c6 on GPIO 38/39 [C-27], [B-44]; `/dev` number UNKNOWN (OQ-052) |
| I2C jumpers on the official IO board | — | CAM0 needs both J6 jumpers [C-03] | — | CAM/DISP 1 needs two J6 jumpers [C-06]; a DT comment disagrees (research gap, topic C; OQ-052) |
| Encoder | hardware `bcm2835-codec`, `/dev/video11` [D-06], [D-07] | as Pi 4 | software only [G-22], [D-31] | software only [D-31] |

### Test order

Bring-up uses the stock image (ADR-003, ACCEPTED), so TEST-BLD-001 does not gate the hardware tests. The product's own image (REQ-BLD-002) is verified by TEST-BLD-001, whose step 4 re-runs TEST-PLT-001 on it.

```text
TEST-HW-001 → TEST-DRV-001 → TEST-DRV-002 → TEST-PLT-001 → TEST-CAP-001 → TEST-CAP-002
TEST-DRV-002 → TEST-AUD-001                       (only if audio is required, OQ-004)
TEST-CAP-002 → TEST-CAP-003
TEST-CAP-002 → TEST-CAP-004                       (4-lane configuration)
TEST-CAP-001 → TEST-CAP-004                       (2-lane configuration: TEST-CAP-002's 1080p60 run does not apply)
TEST-CAP-002 → TEST-DMA-001 → TEST-ENC-001
TEST-ENC-001 → TEST-REC-001, TEST-STR-001, TEST-STR-002
TEST-CAP-001, TEST-CAP-002 → TEST-ATEM-001 (HDMI capture of the ATEM output; the only part in current scope, OQ-009)
TEST-ENC-001 + the selected outputs → TEST-PERF-001
TEST-BLD-001                                      (independent of bring-up)
```

---

## TEST-HW-001 — TC358743 I2C detection

| | |
|---|---|
| Verifies | REQ-DRV-001 |
| Related risks | RISK-021 (bridge-board wiring hazards) |
| Related open questions | OQ-018, OQ-021, OQ-022, OQ-024, OQ-026, OQ-043, OQ-052, OQ-101 |
| Depends on | Hardware assembled; cable orientation checked (see Setup) |

### Objective

Confirm that the TC358743 on the PACSCORDER bridge board answers on the platform's camera I2C bus at the expected address, independently of the Linux driver.

### Setup

- Platform and connector: see the platform reference table. PACSCORDER's choice is UNKNOWN — OWNER DECISION REQUIRED (OQ-011).
- Bridge board: UNKNOWN — VERIFICATION REQUIRED (OQ-018).
  - The stock overlay and driver request no regulator [C-23]. Reasoning: the connector's CAM_GPIO / power-enable line is therefore expected to stay low [C-51].
  - A board that needs CAM_GPIO for power or reset will not answer (OQ-022, VENDOR CONFIRMATION REQUIRED).
- Cable: UNKNOWN (OQ-021). A Raspberry Pi engineer reported that a wrongly sided 22-to-15-pin adapter on Pi 5 swaps GND and 3V3 and can damage either board [C-45]. Reasoning: the CM4 IO Board camera connectors are also 22-pin [C-03] and the CM5 IO Board uses 22-pin CAM/DISP connectors [C-06], so the same adapter hazard is expected there. **Check cable orientation against the board schematic before power-up.**
- Official IO boards: fit the J6 jumpers where the connector needs them (CM4 CAM0 [C-03]; CM5 CAM/DISP 1 [C-06]).
- HDMI source: not required.
- Software: install `i2c-tools`, which is not in the Lite image [G-25].
- Driver state (reasoning): run first **without** the `tc358743` overlay, so that no driver holds the address. Whether the camera I2C bus node exists without the overlay is NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED). If it does not, run with the overlay loaded and continue with TEST-DRV-001.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

```bash
sudo apt install i2c-tools      # package name [G-25]; "apt install" form as in [G-47]
i2cdetect -y <bus>              # i2cdetect ships in i2c-tools [G-25]; "-y" form is from the owner's Rule 9 example (OQ-101)
```

- `<bus>` is taken from the platform reference table [C-24], [C-25], [C-26], [C-27]. It is UNKNOWN for PACSCORDER until observed (OQ-043).
- No register fact documents the semantics of `i2cdetect` options: NEEDS VERIFICATION (OQ-101).
- Optional: read the CHIPID register (0x0000) [A-18] directly. Register addresses are 16-bit, sent MSB first, and register values are little-endian [A-17]. `i2ctransfer` ships in `i2c-tools` [G-25], but its syntax for this read is NEEDS VERIFICATION.

### Expected Result

- A device answers at 7-bit address **0x0f**. That is the address in the kernel DT binding example and in the Raspberry Pi overlay [A-15]. The bus is the platform's camera I2C bus [C-24], [C-25], [C-26], [C-27]. On Pi 5 the bus numbering changed between kernels, as reported by a Raspberry Pi engineer and corrected in research [C-28].
- **Address uncertainty.** The public TC358743XBG datasheet states no I2C address and no address strap [A-15]. The sister part TC358749XBG documents 0x0F or 0x1F, selected by the INT pin level at reset [A-50]. If the bridge answers at **0x1f**, record it: the DT `reg` value must then change ([DEVICE_TREE.md](DEVICE_TREE.md); OQ-026, DATASHEET REQUIRED).
- Other devices may share the bus. With CM5 on the CM4 IO Board, `i2c_csi_dsi1` also serves DISP1, the RTC and the fan [C-27].
- Optional CHIPID read: the chip-ID byte (bits [15:8]) is expected to be 0x00 [A-18], [A-19]. The revision byte (bits [7:0]) is UNKNOWN — record it (OQ-029).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-DRV-001 — TC358743 driver probe and chip ID

| | |
|---|---|
| Verifies | REQ-DRV-001 |
| Related risks | RISK-005, RISK-007, RISK-021 |
| Related open questions | OQ-013, OQ-019, OQ-020, OQ-029, OQ-043, OQ-052, OQ-100 |
| Depends on | TEST-HW-001 |

### Objective

Confirm that the in-tree `tc358743` driver (ADR-002, PROPOSED) probes the bridge, passes the chip-ID check and registers a V4L2 sub-device, with no clock, endpoint or lane-rate errors.

### Setup

- Image: the driver is built as module `tc358743` [A-35]. It is enabled as a module in both Raspberry Pi arm64 defconfigs [E-39], and the 2026-10-06 image ships `tc358743.ko.xz` for both kernels [G-16].
- **Reference clock — check before first boot.**
  - The DT clock frequency must equal the bridge board's oscillator. That frequency is UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, OQ-019).
  - The stock overlay declares 27 MHz [A-45].
  - The driver accepts only 26, 27 or 42 MHz [B-07]. Any other value leads to a kernel BUG instead of a clean probe failure (kernel source [A-22] and reasoning from it [B-11], both CORRECTED; RISK-007).
- Lanes and link frequency:
  - The overlay default is `data-lanes <1 2>` and `link-frequencies` 486000000 (972 Mbps per lane) [A-45], [C-12].
  - Use `4lane` only where the board and connector carry 4 lanes (OQ-021) [B-42], [A-46].
- `config.txt` lines (PROPOSED; NOT YET RUN ON PACSCORDER HARDWARE). The parameter form is `dtoverlay=tc358743,<param>=<val>` [G-12]; `,cam0` selects connector 0 [C-39]. Every line below that gives a parameter by name alone or combines several parameters is NEEDS VERIFICATION (OQ-100); see the note under the table.

  | Platform | Line |
  |---|---|
  | Pi 4 Model B | `dtoverlay=tc358743,media-controller` (ADR-006 PROPOSED) [B-43] — bare parameter: syntax NEEDS VERIFICATION (OQ-100) |
  | CM4, CAM1 with a 4-lane board | `dtoverlay=tc358743,4lane,media-controller` [B-42], [B-43] — bare and combined parameters: syntax NEEDS VERIFICATION (OQ-100) |
  | CM4, CAM0 | `dtoverlay=tc358743,cam0,media-controller` [B-42], [B-43] — 2 lanes only [C-02]; combined parameters: syntax NEEDS VERIFICATION (OQ-100) |
  | Pi 5 | `dtoverlay=tc358743-pi5` [G-13], or `dtoverlay=tc358743`, which is redirected to it [C-11]. [DEVICE_TREE.md](DEVICE_TREE.md) §6.3 proposes naming `tc358743-pi5` explicitly. Add `,4lane` for a 4-lane board and `,cam0` for CAM/DISP0 [G-13], [C-39]; a bare `4lane` and the combination `cam0,4lane` are NEEDS VERIFICATION (OQ-100) |
  | CM5 on the CM5 IO Board | As Pi 5 [C-11], [E-43]. Which connector the default line targets, and whether the J6 jumpers are needed, NEEDS VERIFICATION (OQ-052) |
  | CM5 on the CM4 IO Board | **No line is proposed** ([DEVICE_TREE.md](DEVICE_TREE.md) §6.4). Research found that neither stock I2C/CSI pairing matches the CAM1 connector on CM5 (research open question, topic C; not a register fact). A custom overlay or test evidence is required (OQ-052). The CAM0 connector cannot be used: on CM5 those pins carry USB 3.0 [C-05] |
  | All | `camera_auto_detect=0` — PROPOSED. Prudent, but documented only for camera-sensor overlays, not for TC358743 (CORRECTED) [C-39], [G-15]; OQ-072 |

  - **Syntax NEEDS VERIFICATION (OQ-100).** The register attests the `dtoverlay=tc358743,<param>=<val>` form [G-12], [G-13] and appending `,cam0` [C-39]. It does not attest giving `4lane` or `media-controller` by name alone, or combining several parameters on one line. Check both against the official `config.txt` documentation before use; [DEVICE_TREE.md](DEVICE_TREE.md) §6.0 records the same caveat.
  - CM5: the configuration depends on the carrier board (OQ-018, OQ-052). A CM5 on a custom carrier is UNKNOWN — VERIFICATION REQUIRED.
- Reset and interrupt: the stock overlay has neither `reset-gpios` nor `interrupts` [B-41]. Whether the board wires RESETN or INT to a Pi GPIO is UNKNOWN (OQ-020).
- HDMI source: not required.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot with the `config.txt` line for the platform.
2. Inspect the kernel log for messages from `tc358743` and from the CSI-2 receiver driver. Record the exact lines.
3. Confirm that a `/dev/v4l-subdevN` node exists for the bridge. The sub-device sets `V4L2_SUBDEV_FL_HAS_DEVNODE` [B-18]; in Media Controller mode the node is read-write [C-36]. The method to map N to the bridge is NEEDS VERIFICATION (OQ-043).
4. Record the CHIPID revision byte. The driver's `log_status` prints it [A-19]; the user-space command that triggers it is NEEDS VERIFICATION.
5. If `reset-gpios` is wired (OQ-020): observe RESETN at probe with an instrument (HARDWARE TEST REQUIRED).

### Expected Result

- **No** `not a TC358743 on address 0x1e`.
  - The driver reads CHIPID and requires the chip-ID byte to be 0x00; otherwise it logs that message and returns `-ENODEV` [A-19], [B-14].
  - The address is printed in 8-bit form, so 7-bit 0x0f appears as 0x1e [A-16].
- **No** `unsupported refclk rate` and no kernel BUG (kernel source [A-22]; reasoning from it [B-11]). **No** `failed to get refclk` [A-22].
- **No** `missing endpoint node`, `missing CSI-2 properties in endpoint` or `invalid number of lanes` [B-06].
- **No** `untested bps per lane` with `link-frequency` 486000000 or 297000000 [A-24], [B-09].
  - With a 26 or 42 MHz reference clock, the PLL-derived lane rate differs from the nominal one (reasoning) [A-23], [B-10].
  - Whether that also triggers the message is KERNEL SOURCE INSPECTION REQUIRED.
- **No** `-EIO` from a missing SMBus byte-data capability [A-20].
- The probe and failure messages show the address as 0x1e [A-16].
- The entity is named `tc358743 <bus>-000f`. On Pi 5 with 6.18 kernels the corrected register entry gives `tc358743 11-000f` on CAM/DISP1 or `tc358743 10-000f` on CAM/DISP0, from the DT aliases and later issue reports; earlier forum posts in a Raspberry Pi engineer's thread showed `tc358743 4-000f` (CORRECTED, community) [C-28]. Names on other platforms are UNKNOWN until observed (OQ-043).
- State after probe:
  - one source pad, entity function `MEDIA_ENT_F_VID_IF_BRIDGE` [B-18];
  - three controls: `V4L2_CID_DV_RX_POWER_PRESENT`, "Audio sampling rate", "Audio present" [B-16];
  - default timings CEA 640x480p59.94 and bus code `RGB888_1X24` [B-15], [C-19].
- Without an `interrupts` property, the driver polls the interrupt status every 1000 ms [A-30], [C-20].
- CHIPID revision byte: UNKNOWN — record the value (OQ-029).
- On Pi 5, Raspberry Pi engineers reported a B102 board probing in December 2023 [C-41]. That is not evidence for the PACSCORDER board.

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-DRV-002 — EDID load and HDMI hot-plug assertion

| | |
|---|---|
| Verifies | REQ-CAP-003 |
| Related risks | RISK-010 |
| Related open questions | OQ-002, OQ-024, OQ-032, OQ-093, OQ-101 |
| Depends on | TEST-DRV-001 |

### Objective

Confirm that a source sees no sink until an EDID is written, that writing the EDID asserts HDMI hot-plug (HPD), and that the EDID must be rewritten after every boot or driver load.

### Setup

- As TEST-DRV-001, with an HDMI source connected and supplying +5V.
- EDID file: the PACSCORDER EDID content is UNKNOWN — OWNER DECISION REQUIRED (OQ-002).
  - REQ-CAP-003 (PROPOSED) requires an EDID that advertises only modes the wired CSI-2 lanes can carry.
  - For bring-up only, `v4l2-ctl` has a built-in `hdmi` EDID that advertises up to 1080p60 [B-24]. Reasoning: that exceeds what a 2-lane link carries [C-48], so on a 2-lane platform a source may pick a mode that later fails at stream start (TEST-CAP-004).
- HPD observation: measure the HPD line with an instrument at a board test point, which is UNKNOWN (OQ-024; HARDWARE TEST REQUIRED). Also record whether the source reports a connected display.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot. Before any EDID is written, observe HPD and the source's display detection.
2. Write the EDID on the sub-device node (pad must be 0 [B-22]):
   ```bash
   v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,file=<pacscorder-edid>   # --set-edid syntax [B-24]; EDID via VIDIOC_S_EDID [C-37]
   # bring-up alternative:
   v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,type=hdmi                # built-in type [B-24]
   ```
   The `-d /dev/v4l-subdevN` form is NEEDS VERIFICATION (OQ-101; see Conventions). In legacy mode on Pi 4/CM4 the same ioctl goes to `/dev/videoN` instead [B-25]. That mode is not used if ADR-006 is accepted.
3. Observe HPD and the source's display detection again.
4. Clear the EDID and observe HPD:
   ```bash
   v4l2-ctl -d /dev/v4l-subdevN --clear-edid 0                            # help-text form "--clear-edid <pad>" [B-24]
   ```
   The argument form `0` and the `-d /dev/v4l-subdevN` form are NEEDS VERIFICATION (OQ-101).
5. Reboot, or reload the driver, and repeat step 1. When the product writes the EDID, and whether an EDID survives a driver unload and reload, is OQ-093.
6. Optional negative test: write an EDID of more than 8 blocks [B-22].
7. Read back the EDID and the `V4L2_CID_DV_RX_POWER_PRESENT` control [B-16] at each step. Commands NEEDS VERIFICATION (OQ-101).

### Expected Result

- Step 1:
  - No HPD and no sink seen by the source. The driver does not enable hot-plug until an EDID has been written; its code path is labelled `no EDID -> no hotplug` [A-33], [B-21]. Whether that string reaches the kernel log at the default log level is NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED).
  - Reading the EDID returns `-ENODATA` [B-22].
  - The HPD state between power-on and the first EDID write is UNKNOWN (OQ-032).
- Step 2: the write succeeds with pad 0; any other pad or a non-zero start block returns `-EINVAL` [B-22].
- Step 3: with +5V present, delayed work raises HPD after HZ/7 jiffies. The driver comment says 143 ms; with integer division it is 140 ms at the Raspberry Pi default HZ=250 (CORRECTED) [B-21]. The source detects a sink.
- Step 4: zero blocks clears the EDID and HPD stays low [B-22].
- Step 5: the EDID is held in on-chip SRAM [A-31], and after probe the driver has no EDID stored (`edid_blocks_written == 0`) [B-21]. Reasoning: the EDID is therefore expected to be absent after every boot or driver load, so the step 1 result repeats. Research also found no DT property and no default EDID in the driver (research gap, topic B — not a register fact).
- Step 6: more than 8 blocks returns `-E2BIG` [B-22].
- `V4L2_CID_DV_RX_POWER_PRESENT` is one of the driver's three controls [B-16], and the driver updates its controls on a +5V interrupt [B-23]. Reasoning: it is expected to reflect source +5V. When +5V is removed, the driver drops HPD [B-23].

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-PLT-001 — Overlay load and media graph per platform

| | |
|---|---|
| Verifies | REQ-PLT-001, REQ-ARCH-001 |
| Related risks | RISK-001, RISK-012 |
| Related open questions | OQ-011, OQ-014, OQ-021, OQ-043, OQ-044, OQ-046, OQ-049, OQ-052, OQ-065, OQ-072, OQ-100, OQ-101 |
| Depends on | TEST-DRV-001 on the same platform |

### Objective

For each candidate platform, confirm three things:

- the correct overlay loads;
- the expected CSI-2 receiver driver binds;
- the Media Controller graph contains the expected TC358743 → receiver → video-node entities and links.

This covers the "TC358743 → CSI-2 → receiver → Media Controller → V4L2" part of REQ-ARCH-001.

### Setup

- One unit of each platform still under consideration (ADR-004 OPEN), with the `config.txt` lines from TEST-DRV-001. Their bare and combined parameter syntax is NEEDS VERIFICATION (OQ-100); this test is the resolving test for it.
- **CM5 carrier caveat.** The CM5 configuration depends on the carrier board (OQ-052; [DEVICE_TREE.md](DEVICE_TREE.md) §6.4):
  - on the CM4 IO Board no `config.txt` line is proposed;
  - on the CM5 IO Board the connector the default line targets, and the J6 jumper requirement, NEEDS VERIFICATION.

  Record the carrier board in every CM5 result entry. A CM5 result applies only to the carrier board used.
- Drivers for every platform:
  - the 2026-10-06 image ships `tc358743.ko.xz` for both kernels [G-16];
  - both rpi-6.18.y arm64 defconfigs build both Unicam drivers and both RP1 CFE drivers as modules [G-17];
  - reasoning on verified inputs (CORRECTED): the image's module trees include unicam and rp1-cfe, and one image can boot all four candidates, but the overlay, its parameters and the capture model differ per board [G-71].
- HDMI source: optional.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot with the platform's `config.txt` line.
2. Identify which CSI-2 receiver driver is bound (method NEEDS VERIFICATION; OQ-044).
3. Print the media graph of the receiver's `/dev/mediaN`. No register fact attests a `media-ctl` option for printing the topology: NEEDS VERIFICATION (OQ-101).
4. Pi 5 / CM5 only: enable the csi2 → video-node link, as reported by Raspberry Pi engineer 6by9 [C-33]. The report used `-d 2`; the device number is system-specific (OQ-043).
   ```bash
   media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'      # [C-33]
   ```
5. Record every entity name, pad number, link flag and `/dev` node.

### Expected Result

**Pi 4 Model B and CM4 (Media Controller mode)**

- Overlay `tc358743` is loaded [G-12].
- The `media-controller` parameter removes the fragment that would otherwise select `brcm,bcm2835-unicam-legacy` [B-43], [C-10].
- The downstream Unicam driver (`bcm2835-unicam-legacy.ko`) binds the base `brcm,bcm2835-unicam` compatible in Media Controller mode (CORRECTED) [E-40], [C-09], [G-21].
- In the graph (CORRECTED) [C-36]:
  - an image node named `unicam-image`, with an IMMUTABLE|ENABLED link straight from the TC358743 pad;
  - no CSI-2 receiver sub-device in between;
  - no `unicam-embedded` node, because the TC358743 has a single source pad.
- The TC358743 sub-device node is read-write [C-36].
- Research found that filtered `ENUM_FMT` may not list UYVY in this mode (research gap, topic C; OQ-046). Record the result; do not treat a missing UYVY entry as a failure.
- Pi 4 Model B only: the csi1 port is limited to 2 data lanes [B-48], [C-08].

**Pi 5 and CM5**

- `tc358743-pi5` is applied, whether named directly [G-13] or reached through the `dtoverlay=tc358743` redirect. It is always Media Controller [C-11], [E-43].
- The downstream RP1 CFE driver (`rp1-cfe-downstream`) binds `raspberrypi,rp1-cfe` [C-29], [E-44].
- In the graph [C-32]:
  - a `csi2` sub-device with sink pads 0–3 and source pads 4–7;
  - an IMMUTABLE|ENABLED link from the TC358743 to `csi2`;
  - links from `csi2` source pads to `rp1-cfe-<node>` video nodes (for example `rp1-cfe-csi2_ch0`) and to `pisp-fe`, all disabled until enabled. After step 4, `csi2:4 → rp1-cfe-csi2_ch0` is enabled.
- The TC358743 entity is named `tc358743 1x-000f`, where the bus number depends on the connector, as reported by a Raspberry Pi engineer [C-28], [C-33].

**All platforms**

- Reasoning from [C-39]: `camera_auto_detect` makes the firmware auto-load overlays for recognised CSI cameras, so with `camera_auto_detect=0` no sensor overlay is expected to be auto-loaded. Whether that line is required at all is OQ-072.

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-001 — DV timings detection for each source mode

| | |
|---|---|
| Verifies | REQ-CAP-002, REQ-CAP-008 |
| Related risks | RISK-008, RISK-009, RISK-012 |
| Related open questions | OQ-002, OQ-028, OQ-040, OQ-049, OQ-083, OQ-102 |
| Depends on | TEST-DRV-002, TEST-PLT-001 |

### Objective

Confirm that, for every source mode PACSCORDER must accept, the DV timings are detected and applied through the V4L2 DV-timings API on the TC358743 sub-device, and that the error codes for no signal, unstable signal and out-of-range timing match the driver's documented behaviour.

### Setup

- As TEST-DRV-002, with the EDID loaded.
- Sources (REQ-CAP-008): run with both source types the owner named on 2026-10-07 — an ATEM switcher's HDMI output and each camera connected directly. The ATEM and camera models are OPEN (OQ-102); record the model and firmware of every source in the result entry.
  - ATEM Mini Pro: set its HDMI output to Program first; it defaults to multiview [F-25].
  - Cameras: their HDMI output modes are UNKNOWN — VERIFICATION REQUIRED per model (OQ-102).
- Source modes: REQ-CAP-007 requires every frame rate the configured link carries; the exact list is UNKNOWN — OWNER DECISION REQUIRED (OQ-002). Candidates from sources:
  - the ATEM Mini Pro HDMI output standards 1080p23.98, 24, 25, 29.97, 30, 50, 59.94 and 60 [F-23];
  - at least one 59.94 Hz source (OQ-040); fractional rates are part of "all frame rates" unless the owner says otherwise (REQ-CAP-007);
  - one HDCP-requiring source (OQ-028).
- Lane configurations (reasoning): the timings are derived from the TC358743's own timing registers (DE_WIDTH, FV_CNT) [B-28], and the lane count is checked only at stream start [B-30], [B-32]. Detection is therefore expected to behave the same on the 2-lane and the 4-lane configuration (REQ-CAP-007); whether a detected mode can then be streamed on the configured lanes is TEST-CAP-004.
- Interlaced and out-of-range sources are covered by TEST-CAP-004.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.** For each source mode:

```bash
v4l2-ctl -d /dev/v4l-subdevN --set-dv-bt-timings query     # [C-33], [C-37]; "-d <sub-device path>" form: OQ-101
```

- The `-d /dev/v4l-subdevN` form is NEEDS VERIFICATION (OQ-101; see Conventions).
- Record the command's return status and the timings it reports. How `v4l2-ctl` prints the detected timings is NEEDS VERIFICATION (OQ-101).
- Repeat with the HDMI cable unplugged and while the source is switching modes.

### Expected Result

- A progressive mode within the driver capability succeeds. The capability is 640–1920 × 350–1200, pixel clock 13–165 MHz, progressive only [A-08], [B-26].
- The detected width and height equal the source's active size [B-28].
- The frame rate is reported as an integer: fps = round(10000 / FV_CNT), with the pixel clock derived from it. So 59.94 Hz and 60 Hz sources are reported alike [B-28] (OQ-040).
- Error codes from `query_dv_timings` [B-29]:
  - `-ENOLINK` when HPD is low or there is no TMDS signal (cable unplugged, or no EDID loaded);
  - `-ENOLCK` when sync is not stable (for example while the source switches mode);
  - `-ERANGE` when the timings fall outside the capability.
- HDCP: the driver always configures HDCP as disabled on DT platforms [A-04]. A source that enforces HDCP is expected to give no usable video (RISK-008). The exact symptom is UNKNOWN (OQ-028).
- Official Raspberry Pi documentation: timings are set through DV_TIMINGS, and `VIDIOC_S_FMT` changes only the pixel format [C-37].

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-002 — 1080p60 capture: frame rate and frame integrity

| | |
|---|---|
| Verifies | REQ-CAP-001, REQ-CAP-007 |
| Related risks | RISK-001, RISK-006, RISK-011, RISK-012, RISK-016, RISK-020 |
| Related open questions | OQ-001 (ANSWERED 2026-10-07), OQ-003, OQ-011, OQ-021, OQ-037, OQ-038, OQ-041, OQ-045, OQ-049, OQ-050, OQ-095, OQ-099 |
| Depends on | TEST-CAP-001 |

### Objective

Capture 1920x1080 at 60 Hz from the TC358743 into memory through V4L2. Measure the delivered frame rate and check frames for corruption.

Scope against the owner decision of 2026-10-07 (OQ-001 answered: "i need 2 lane and 4 lane with all frame rate"):

- REQ-CAP-001: 1080p60 is required on the 4-lane configuration. This test is its main evidence.
- REQ-CAP-007: this test covers the highest rate of the 4-lane configuration (1080p60). The 2-lane configuration cannot carry 1080p60 [C-37], [C-48]; its highest 1920x1080 rates (1080p50 UYVY, 1080p30 RGB888) and every lower rate of both configurations are covered by the TEST-CAP-004 supported-mode matrix, which reuses this test's configuration and measurement steps.

### Setup

- **4-lane configuration only:** CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05].
  - **2-lane configurations cannot run this test's 1080p60 run.** The Pi 4 Model B connector has 2 lanes [C-01], and so does CM4 CAM0 [C-02]. Official documentation limits 2 lanes to 1080p30 RGB888 or 1080p50 YUV422 [C-37], and the reasoning in [C-48] agrees. Under REQ-CAP-007 these remain candidates for the 2-lane configuration (ADR-004, OPEN); they are tested at the rates their link carries in TEST-CAP-004.
  - The bridge board must route 4 lanes (UNKNOWN, OQ-021), with the `4lane` overlay parameter [B-42].
  - **A 4-lane port is necessary but not shown to be sufficient.** At 972 Mbps the driver uses 3 of the 4 lanes for 1080p60 UYVY (reasoning) [C-47]. Capture with 3 active lanes is unproven (OQ-038; ADR-008).
- Link frequency: overlay default 486000000 (972 Mbps per lane) [A-45]. ADR-008 (PROPOSED; OQ-099) proposes keeping this default on every platform.
  - CM4 CAM1 with a 4-lane board only: ADR-008 also proposes an evaluation run at `link-frequency=297000000` (594 Mbps per lane) [G-12]. Reasoning: at that rate the driver requests all 4 lanes for 1080p60 UYVY [C-47], [C-49], which avoids the 3-of-4-lane case. A driver comment says 594 Mbps is meant for 4-lane 1080p60 [C-44].
  - Not on Pi 5 / CM5. Reasoning: the CFE programs 999 Mbps for this bridge, which matches only the default rate [C-52].
- Capture format: UYVY (`UYVY8_1X16`), following ADR-005 (PROPOSED; OQ-003).
- 1080p60 HDMI source and an EDID that offers 1080p60 (TEST-DRV-002). Sources are ATEM outputs and cameras (REQ-CAP-008): run with each required model that outputs 1080p60 (models OQ-102). The ATEM Mini Pro lists 1080p60 among its output standards [F-23]; set its HDMI output to Program first [F-25].

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

**Pi 5 / CM5** — sequence reported by Raspberry Pi engineer 6by9 on kernel 6.18.39 [C-33].

- 6.18.39 is only the kernel of that community report. The bring-up image ships 6.18.50 [G-04], and the driver source inspected in research is the `rpi-6.18.y` tip at 6.18.55 [E-37] (OQ-097).
- `<N>` and the entity name are placeholders (OQ-043).
- The source shows the `-V` line literally for `"csi2":0`; the other two `-V` lines use the same form on the pads the report names.
- The `-d /dev/v4l-subdevN` form of the two `v4l2-ctl` lines is NEEDS VERIFICATION (OQ-101; see Conventions).

```bash
v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,file=<pacscorder-edid>          # [B-24]; -d form: OQ-101
v4l2-ctl -d /dev/v4l-subdevN --set-dv-bt-timings query                       # [C-33], [C-37]; -d form: OQ-101
media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'                   # [C-33]
media-ctl -d <N> -V '"tc358743 1x-000f":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'   # [C-33]
media-ctl -d <N> -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'               # [C-33]
media-ctl -d <N> -V '"csi2":4 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'               # [C-33]
```

- Then set the video-node format to `pixelformat=UYVY` [C-33]. The full `v4l2-ctl` option string is NEEDS VERIFICATION (OQ-101).
- Then stream and count frames. The streaming command is NEEDS VERIFICATION (OQ-101).

**CM4 (CAM1, Media Controller mode)**

- EDID and DV timings: same as above, on the sub-device node [B-25], [C-36].
- Pad format on the TC358743 and the `unicam-image` format: no register fact gives the Unicam Media Controller command sequence, so these are NEEDS VERIFICATION.

**All platforms — measurements**

- Delivered frame rate, from buffer timestamps.
- Dropped and repeated frames.
- Frame integrity: visual inspection of a known test pattern, and a byte check of at least one frame.
- Kernel log during streaming.
- CSI-2 receive errors. How to read them is UNKNOWN on both receivers: RP1 CFE is OQ-050, Unicam (Pi 4/CM4) is OQ-095.
- Duration: UNDEFINED (OQ-010, OQ-017).

### Expected Result

- **Lane count** (reasoning): the driver requests 3 lanes for 1080p60 UYVY at 972 Mbps [C-47], [B-33].
  - Both receivers accept fewer lanes than configured, such as 3 of 4 [C-16].
  - Whether frames are captured correctly with 3 active lanes is UNKNOWN (OQ-038).
  - In the ADR-008 evaluation run on CM4 CAM1 at `link-frequency=297000000`, the driver is expected to request 4 lanes (reasoning) [C-47], [C-49]. Which rate to keep is OQ-099.
- **No** `Device has requested N data lanes, which is >M configured in DT` [B-32], [C-16].
- **Pi 5 / CM5:** `Unable to determine sensor link rate, using 999 Mbps` is **expected**. The CFE always takes this fallback for TC358743 (kernel source [C-31]; reasoning [B-49]).
  - Reasoning: 999 Mbps matches the default 972 Mbps link [C-52].
  - Reception reliability at this setting is UNKNOWN (OQ-050).
- Video-node pixel format UYVY (both receivers map `UYVY8_1X16` to `V4L2_PIX_FMT_UYVY`) [C-34]. Each frame is 4,147,200 bytes (reasoning, CORRECTED) [C-53].
- Colourimetry: BT.601 limited range, reported as SMPTE170M [B-34] (OQ-041).
- Frame rate: 60 frames/s from a 60 Hz source. The driver cannot distinguish 59.94 Hz from 60 Hz [B-28].
- No visible or byte-level corruption.
  - An open issue reports corruption at 1080p50 RGB888 on 3 of 4 lanes on CM4. A Raspberry Pi engineer attributed it to the hard-coded FIFO level of 374 [C-43], [B-12] (RISK-006).
  - The driver's continuous-clock behaviour versus the overlay's `clock-noncontinuous` setting may matter [B-37] (OQ-037).
- If RGB888 is used instead of UYVY:
  - Pi 5 / CM5: CFE maps `RGB888_1X24` to `BGR24` [C-34]; set `BGR3` on the video node, as in the reported Pi 5 sequence [C-33].
  - Pi 4 / CM4: Unicam labels it `RGB24` [C-34], but a B,G,R memory order was reported on CM4 [C-35] (RISK-016, OQ-045).
- Thresholds for duration and allowed drops: UNDEFINED (OQ-010, OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-003 — Source connect / disconnect / mode change

| | |
|---|---|
| Verifies | REQ-CAP-004 |
| Related risks | RISK-012, RISK-013 |
| Related open questions | OQ-020, OQ-039, OQ-051, OQ-090, OQ-093 |
| Depends on | TEST-CAP-002 (or a lower-rate capture on a 2-lane platform) |

### Objective

Confirm that HDMI connect, disconnect, signal loss and timing changes are detected, and that capture resumes with the new timings without a reboot. Measure detection latency.

### Setup

- As TEST-CAP-002 (or TEST-CAP-001 on a 2-lane platform at a mode the link carries).
- A source that can change output mode on command.
- Access to unplug the HDMI cable.
- The application that reacts to events and reconfigures the pipeline does not exist yet: NOT STARTED. Its process model is OQ-090; when it rewrites the EDID is OQ-093.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Subscribe to `V4L2_EVENT_SOURCE_CHANGE` on the TC358743 sub-device node `/dev/v4l-subdevN`, not on `/dev/videoN`, as a Raspberry Pi engineer advised [B-38], [C-42]. The `v4l2-ctl` option to wait for an event is NEEDS VERIFICATION (OQ-101).
2. While streaming:
   - (a) unplug HDMI;
   - (b) re-plug;
   - (c) change the source's output mode;
   - (d) power-cycle the source.
3. After each event, re-run the detection and configuration steps of TEST-CAP-002 (`--set-dv-bt-timings query`, pad formats, video-node format) and restart streaming.
4. Measure the time from the physical change to event delivery.
5. Optional recovery check (OQ-039, OQ-093): unload and reload the driver module. The command is NEEDS VERIFICATION.

### Expected Result

- On a sync change, or a DE size/position change with a stable signal, the driver mutes the stream if the signal is lost or the timings differ. It sends `V4L2_EVENT_SOURCE_CHANGE` (`V4L2_EVENT_SRC_CH_RESOLUTION`) when a sub-device node exists [B-39].
- **The driver does not apply the new timings itself** [B-39]. Capture resumes only after userspace re-queries and reconfigures (REQ-CAP-004).
- On +5V loss:
  - the driver drops HPD, zeroes the stored timings and updates its controls [B-23];
  - when +5V returns and an EDID is stored, HPD is raised again [B-23], [B-21].
- Latency: up to about 1 s. Without a wired interrupt, the driver polls every 1000 ms [A-30], [B-19] (RISK-013). INT wiring is UNKNOWN (OQ-020).
- Pi 5 / CM5: an open issue reports that the CFE video nodes do not deliver this event; a Raspberry Pi engineer said applications should subscribe on the source sub-device [C-42] (OQ-051).
- Driver removal does not drop HPD, assert reset or disable the reference clock [B-40]. Whether a module reload recovers capture is UNKNOWN (OQ-039).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-004 — Unsupported-mode rejection and supported-mode matrix

| | |
|---|---|
| Verifies | REQ-CAP-005, REQ-CAP-007, REQ-CAP-008 |
| Related risks | RISK-001, RISK-006, RISK-008, RISK-009, RISK-011 |
| Related open questions | OQ-002, OQ-021, OQ-030, OQ-035, OQ-038, OQ-040, OQ-050, OQ-083, OQ-095, OQ-099, OQ-102 |
| Depends on | TEST-CAP-001; TEST-CAP-002 on the 4-lane configuration (its 1080p60 run does not apply to the 2-lane configuration) |

### Objective

Establish, for each platform and lane configuration, which input modes capture without corruption, and confirm that unsupported modes are reported rather than streamed corrupted.

The resulting supported-mode matrix is also the evidence for two DRAFT requirements from the owner decisions of 2026-10-07:

- REQ-CAP-007: both the 2-lane and the 4-lane configuration capture every frame rate their link carries. The matrix must be completed for **both** configurations. Its acceptance criteria are the supported-mode matrix per lane count, which the owner has not yet defined (OQ-002).
- REQ-CAP-008: HDMI sources are ATEM switcher outputs and cameras connected directly. The matrix must be completed for **both** source types, with every model the owner lists (OQ-102).

Unsupported modes include:

- interlaced input;
- pixel clock above 165 MHz;
- modes that need more lanes than are wired. The receivers compare the bridge's lane request with the Device Tree `data-lanes` value, not with the physical wiring [B-32], [C-16], so the DT value must match the routed lanes (OQ-021; REQ-CAP-005 note).

### Setup

- Both lane configurations (REQ-CAP-007), on every platform still under consideration for each:
  - 2-lane: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port (board lane count OQ-021);
  - 4-lane: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05].
  - The DT `data-lanes` value must match the lanes the board routes: setting `4lane` on a 2-lane Unicam port does not fail the probe [C-17], and the STREAMON check compares against the DT value only (see Objective).
- Both source types (REQ-CAP-008):
  - **ATEM switcher HDMI output**, set to Program [F-25]. The ATEM Mini Pro outputs 1080p23.98, 24, 25, 29.97, 30, 50, 59.94 and 60 only, with no 720p or 1080i [F-23], so for that model only the 1080p rows apply. Whether the ATEM honours the sink EDID or always outputs its own video standard is UNKNOWN (OQ-083). Other ATEM models: OQ-102.
  - **Cameras connected directly.** Output modes, interlaced output and HDCP behaviour are UNKNOWN — VERIFICATION REQUIRED per model (OQ-102). Record what each camera actually outputs.
- Sources able to output:
  - each mode in the matrix below;
  - an interlaced mode (for example 1080i);
  - a mode outside 640–1920 × 350–1200 or 13–165 MHz.
- For the negative cases, the EDID may have to advertise the mode, or the source must force it. EDID content is OQ-002. Reasoning: because the two configurations carry different modes [C-48], [C-49], the EDID may have to differ per lane configuration (OQ-002).

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

- For each lane configuration (2-lane and 4-lane), each source (ATEM output and each listed camera) and each mode and format: run the TEST-CAP-001 detection step, then the TEST-CAP-002 configuration and streaming steps. Record:
  - the lane configuration, platform, connector and source model;
  - the return code of each step;
  - the kernel log;
  - the lane count the driver chose;
  - frame integrity;
  - CSI-2 receive errors, where a method exists (RP1 CFE: OQ-050; Unicam: OQ-095).
- Run the matrix with the default link frequency (486000000), which ADR-008 (PROPOSED; OQ-099) proposes to keep. Repeat it with `link-frequency=297000000` [G-12] only if the owner wants that rate evaluated (OQ-099); ADR-008 proposes evaluating it only on CM4 CAM1 with 4 lanes, in TEST-CAP-002.

### Expected Result

**Lane matrix at the default 486 MHz link frequency (972 Mbps per lane).** Lane counts are reasoning from the driver formula [C-47]; feasibility percentages are reasoning [C-48], [C-49].

| Mode | Format | Lanes requested | 2-lane link | 4-lane link |
|---|---|---|---|---|
| 1080p60 | UYVY | 3 | rejected at STREAMON [B-32] | fits by bandwidth [C-49]; capture with 3 of 4 lanes active is unproven (OQ-038; ADR-008) |
| 1080p60 | RGB888 | 4 | rejected | fits, 76.8 % of link [C-49] |
| 1080p50 | UYVY | 2 | fits, 85.3 % [C-48]; per-line check leaves about 2.0 µs slack before LP/HS overhead, and 972 Mbps is about 7.6 % above the 898.12 Mbps/lane minimum from Toshiba's spreadsheet as read by a Raspberry Pi engineer (reasoning) [C-50]. Reasoning: the per-active-lane load equals that of the reported-corrupt 3-lane 1080p50 RGB888 case (RISK-006) | fits, but still on 2 lanes at 85.3 % (reasoning) [C-49], [C-48] |
| 1080p50 | RGB888 | 3 | rejected [C-48] | fits by bandwidth; corruption reported on CM4 [C-43] (RISK-006) |
| 1080p30 | UYVY | 2 | fits, 51.2 % [C-48] | fits |
| 1080p30 | RGB888 | 2 | fits, 76.8 % [C-48] | fits |

**Other rates required by REQ-CAP-007 ("all frame rates" each link carries).** None of these has a lane count in the source register; record the lane count the driver chooses (HARDWARE TEST REQUIRED).

- 1080p23.98, 1080p24, 1080p25 (reasoning): the driver's payload is active pixels × frame rate × bits per pixel [C-46], so at the same size and format these carry less payload than 1080p30. They are expected to need no more lanes than 1080p30 (2 at 972 Mbps) [C-47], and therefore to fit on both configurations.
- 1080p29.97 and 1080p59.94: the driver reports fractional rates as integer rates, so these are reported as 30 and 60 fps [B-28] (OQ-040). Reasoning: they are expected to request the same lanes as 1080p30 and 1080p60. On the 2-lane configuration 1080p59.94 is therefore expected to be rejected in both formats, as 1080p60 is [C-48].
- 720p60 (reasoning): 1 lane in UYVY and 2 in RGB888 at 972 Mbps [B-33], so it is within both configurations' lane count. The ATEM Mini Pro has no 720p output [F-23]; this row applies to cameras only (OQ-102).
- Thus "all frame rates" on the 2-lane configuration means, for 1920x1080, at most 1080p50 in UYVY and 1080p30 in RGB888 [C-37], [C-48]; on the 4-lane configuration every 1080p rate up to 60 fits by bandwidth [C-49], with the 3-of-4-lane caveat for 1080p60 UYVY (OQ-038).

**At `link-frequency=297000000` (594 Mbps per lane)**

- Lanes requested (reasoning) [C-47]:
  - 1080p60: UYVY 4, RGB888 6;
  - 1080p50: UYVY 3, RGB888 5;
  - 1080p30: UYVY 2, RGB888 3.
- Reasoning: on 4 lanes, 1080p50 RGB888 and 1080p60 RGB888 are rejected [C-49]. An issue reporter saw `Device has requested 5 data lanes, which is >4 configured in DT` [C-43].
- Reasoning: on 2 lanes only 1080p30 UYVY fits [C-48].
- On Pi 5 / CM5 this rate does not match the CFE's 999 Mbps D-PHY setting (reasoning) [C-52] (RISK-011, OQ-050). This is one reason ADR-008 (PROPOSED; OQ-099) proposes keeping the default rate.

**Rejections**

- The driver does not compare the lanes it needs with the DT `data-lanes` [B-30], [C-15]. A mode needing more lanes than configured fails only at STREAMON, with `-EINVAL` and `Device has requested N data lanes, which is >M configured in DT` [B-32], [C-16].
- Interlaced input returns `-ERANGE` from `query_dv_timings` and `s_dv_timings`, because the capability lacks the interlaced flag (reasoning from source) [B-27] (RISK-009).
- Timings outside the capability, including a pixel clock above 165 MHz, return `-ERANGE` [A-08], [B-29], [C-18].

**Per source type (REQ-CAP-008)**

- ATEM output: only progressive 1080p standards are expected [F-23]. Reasoning from that list and the matrix above: on the 2-lane configuration its 1080p59.94 and 1080p60 standards cannot be captured, and 1080p50 only in UYVY [C-37], [C-48]. What reaches the TC358743 (pixel encoding, quantisation range, HDCP) is UNKNOWN (OQ-083).
- Cameras: an interlaced output mode returns `-ERANGE` [B-27] (RISK-009). A source that enforces HDCP is expected to give no usable video, because the driver disables HDCP [A-04] (RISK-008). Every other camera result is UNKNOWN until observed (OQ-102).

**Product behaviour (REQ-CAP-005)**

The application must report the mode as unsupported and must not stream corrupted video. The application is NOT STARTED. Thresholds for "corrupted" are UNDEFINED (OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-AUD-001 — HDMI audio capture over I2S

| | |
|---|---|
| Verifies | REQ-CAP-006 |
| Related risks | RISK-014 |
| Related open questions | OQ-004, OQ-024, OQ-025, OQ-033, OQ-054, OQ-063 |
| Depends on | TEST-DRV-002; owner decision that audio is required (OQ-004) |

### Objective

Capture the HDMI source's embedded audio from the TC358743 I2S output into an ALSA device on the Pi.

The silicon can also send audio over CSI-2 [A-05]. The Linux driver always configures 2-channel I2S output instead [A-13], so this test covers the I2S path only. Reasoning: the `tc358743-audio` overlay expects the I2S signals on Pi GPIO 18–20 [A-47], [G-14], not on the camera connector, so the bridge board needs separate audio wiring to the Pi (OQ-025).

### Setup

- **Precondition:** OQ-004 answered "audio required". Otherwise this test does not apply; REQ-CAP-006 is PROPOSED.
- Wiring:
  - The bridge board must bring out the I2S pins. That is UNKNOWN (VENDOR CONFIRMATION REQUIRED, OQ-025). The audio pins are outputs in the VDDIO2 domain (1.8 V or 3.3 V) [A-12]; the board's VDDIO2 level is UNKNOWN (OQ-024).
  - Wiring expected by the `tc358743-audio` overlay: LRCK/WFS → GPIO 19, BCK/SCK → GPIO 18, DATA/SD → GPIO 20 [A-47], [G-14].
- `config.txt`: `dtoverlay=tc358743-audio` in addition to the `tc358743` line [C-37], [G-14].
  - The overlay README refers to a `tc358743-fast` overlay that has no README entry [B-46].
  - Behaviour on Pi 5 / CM5 is UNKNOWN (OQ-054).
- An HDMI source sending 2-channel PCM audio at a known sample rate.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot. Confirm that an ALSA card is registered (command NEEDS VERIFICATION).
2. Read the "Audio present" and "Audio sampling rate" controls on the TC358743 sub-device [B-16] (command NEEDS VERIFICATION).
3. Record audio from the ALSA card (command NEEDS VERIFICATION). Compare with the source audio.
4. Change the source sample rate and repeat step 2.

### Expected Result

- An ALSA card named `tc358743` (the default `card-name`) [A-47], [G-14].
- The driver always configures 2-channel I2S output [A-13], although the chip could also send audio over CSI-2 [A-05]. I2S on the TC358743 is controller (master) clock mode only, with 32-bit slots [A-11]. The overlay makes the Pi the clock consumer with two 32-bit slots [B-46].
- "Audio present" reads 1 while the source sends audio. "Audio sampling rate" (read-only, 0–768000) reports the source rate [B-16].
- Audio/video synchronisation tolerance: UNDEFINED (OQ-004).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-DMA-001 — DMABUF capture → encoder buffer sharing

| | |
|---|---|
| Verifies | REQ-DMA-001, REQ-ARCH-001 |
| Related risks | RISK-003, RISK-020 |
| Related open questions | OQ-015, OQ-057, OQ-058, OQ-060, OQ-062, OQ-066 |
| Depends on | TEST-CAP-002 |

### Objective

Confirm that captured frames reach the encoder as DMABUF buffers without a CPU copy, where the platform encoder supports DMABUF import. On Pi 5 / CM5, which have no hardware encoder, record the buffer path that is used instead. This covers the "V4L2 → DMABUF → Encoder" part of REQ-ARCH-001.

### Setup

- As TEST-CAP-002.
- Userspace framework: undecided (ADR-007 OPEN, OQ-015). Both candidates below need packages that are not in the Lite image [G-32]:
  - GStreamer: `gstreamer1.0-plugins-good` contains the V4L2 elements [G-26].
  - FFmpeg: Raspberry Pi OS ships its own FFmpeg build [G-29].
- **Pi 4 / CM4:** encoder `/dev/video11` (`bcm2835-codec-encode`) [D-06], [D-07], [D-08].
- **Pi 5 / CM5:**
  - There is no hardware encoder [D-31], [G-22].
  - Reasoning: `/dev/video11` does not exist there [D-29].
  - The software H.264 encoders researched, GStreamer `x264enc` and FFmpeg `libx264`, do not accept packed UYVY [D-40], [D-43], so a conversion stage is needed (OQ-060).

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

**Pi 4 / CM4 with GStreamer**

- Reference pipeline, reported by a Raspberry Pi engineer [D-54]:
  ```bash
  gst-launch-1.0 v4l2src ! "video/x-raw,framerate=30/1,format=UYVY" ! v4l2h264enc extra-controls="controls,h264_profile=4,h264_level=10,video_bitrate=256000;" ...
  ```
- Add `output-io-mode=dmabuf-import` to `v4l2h264enc` [D-53].
- Points to resolve before running:
  - Per the same report, `h264_level=10` is Level 3.2 [D-54]. Reasoning: that is below the Level 4 needed for 1080p [F-40]. The level value to use is NEEDS VERIFICATION.
  - The `v4l2src` device selection and the combined pipeline are NEEDS VERIFICATION.

**Pi 4 / CM4 with FFmpeg (alternative)**

- Raspberry Pi OS's patched FFmpeg imports DMABUF into `h264_v4l2m2m` when the input pixel format is `AV_PIX_FMT_DRM_PRIME` [D-45]. Command NEEDS VERIFICATION.
- Upstream FFmpeg's V4L2 M2M path is MMAP-only and copies every frame [D-44].

**Pi 5 / CM5**

Run capture → conversion → software encoder and record where buffers are copied. The method is NEEDS VERIFICATION (OQ-060).

**All platforms**

- Record whether DMABUF import succeeded.
- Record any import error in the kernel log.
- Record the `bytesperline` of the capture buffers.
- Record the CPU load compared with a copy-based run. The comparison method is NEEDS VERIFICATION.

### Expected Result

**Pi 4 / CM4**

- Both encoder queues support `VB2_DMABUF` through `videobuf2-dma-contig` [D-19].
- An imported buffer must be one contiguous region at least as large as the plane; otherwise import fails with `contiguous chunk is too small` [D-20].
- The encoder uses one memory plane even for planar formats [D-21]. For UYVY it rounds `bytesperline` up to a multiple of 64 bytes [D-22].
- Reasoning, from inputs [C-53] (2 bytes per UYVY pixel) and [D-22]: a 1920-pixel UYVY line is 3840 bytes, a multiple of 64 (OQ-058).
- Unicam allocates capture buffers with `videobuf2-dma-contig` and no IOMMU (reasoning, CORRECTED) [C-53].
- Expected: no `contiguous chunk is too small`, and the encoder consumes the capture buffers.
- `rpicam-apps` uses this zero-copy import into `/dev/video11` for camera buffers [D-34]. That is not evidence for TC358743 buffers.

**Pi 5 / CM5**

- Zero-copy *into the encoder* does not apply, because encoding is in software [G-22].
- Whether CFE buffers come from CMA is not established (reasoning, CORRECTED) [C-53] (OQ-053).
- The DMA-BUF heap names on rpi-6.18.y are `default_cma_region` plus a legacy heap named after the CMA area [E-48] (OQ-062).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-ENC-001 — Sustained real-time H.264 encode

| | |
|---|---|
| Verifies | REQ-ENC-001 |
| Related risks | RISK-002, RISK-003, RISK-015 |
| Related open questions | OQ-001, OQ-005, OQ-011, OQ-015, OQ-041, OQ-048, OQ-056, OQ-057, OQ-059, OQ-060, OQ-096 |
| Depends on | TEST-CAP-002 and TEST-DMA-001 (see Preconditions) |

### Objective

Measure whether each platform can encode the captured video to H.264 in real time for a sustained period, at the frame rates PACSCORDER captures: 1080p60 on the 4-lane configuration (REQ-CAP-001), and every rate each lane configuration carries (REQ-CAP-007; OQ-001 answered by the owner on 2026-10-07).

### Setup

- As TEST-DMA-001.
- **Preconditions** (the same as [PERFORMANCE.md](PERFORMANCE.md) §10.3):
  - TEST-CAP-002 has a recorded result of TESTED — PASS for the mode under test. TEST-CAP-002 cannot run on a 2-lane connector; there, as for TEST-CAP-003, a passing lower-rate capture of the mode under test is needed instead.
  - TEST-DMA-001 has been run.
  - The encoder settings are recorded: profile, level (set from the mode; [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §6), bitrate and mode, GOP, and the repeat-header setting.
- Encoding parameters (codec, bitrate, latency target, number of simultaneous encodes): UNDEFINED — OWNER DECISION REQUIRED (OQ-005).
- Pi 4 / CM4:
  - **2-lane connectors.** Pi 4 Model B and CM4 CAM0 have 2 lanes, so 1080p60 cannot be captured from the TC358743 there [C-01], [C-02], [C-48]. On those connectors, which are candidates for the 2-lane configuration of REQ-CAP-007, the highest encode run is 1080p50 UYVY (reasoning) [C-48]. The 1080p60 run applies to CM4 CAM1 only.
  - Do not use the cut-down firmware: `gpu_mem=16` selects `start4cd.elf`, which removes codec support [D-48].
  - The firmware variant and GPU memory needed are OQ-048.
- Pi 5 / CM5: GStreamer `x264enc` is in the GStreamer Ugly plug-ins [D-40]. The Debian package name is NEEDS VERIFICATION.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

**Pi 4 / CM4**

1. List the encoder input formats. A 2021 listing posted by a Raspberry Pi engineer used this command [D-17]:
   ```bash
   v4l2-ctl --list-formats-out -d 11       # [D-17]
   ```
2. Encode with `v4l2h264enc`. Official documentation uses `extra-controls="controls,repeat_sequence_header=1"` with caps `video/x-h264,level=(string)4` [D-37].
   - Reasoning: 1080p60 needs Level 4.2 [F-40], [D-36]. The level string to use is NEEDS VERIFICATION.
   - Run 1080p60 on CM4 CAM1 only; on Pi 4 Model B and CM4 CAM0 start at 1080p50 (see Setup).
3. Optional, informational: repeat with `gpu_freq=550`, the value suggested in the official 720p120 recipe (reasoning entry) [D-52], only if the owner allows overclocking to be evaluated (OQ-056). Whether an overclock is acceptable in the product is OQ-096.

**Pi 5 / CM5**

- Official documentation says to replace the encoder with `x264enc speed-preset=1 threads=1` [D-37].
- Insert a UYVY → I420/NV12 conversion before it [D-40]. The element and its placement are NEEDS VERIFICATION.

**All platforms — measurements**

- Output frame rate against input frame rate.
- Dropped frames.
- Encode latency.
- CPU load per core.
- SoC temperature.
- Duration: UNDEFINED.
  - The research suggested at least 10 minutes (research open question, topic D), and [RISKS.md](RISKS.md) RISK-002 repeats it. The owner has not accepted it (OQ-010, OQ-017).
  - Commands for these measurements are NEEDS VERIFICATION.

### Expected Result

**Pi 4 / CM4**

- UYVY appears among the encoder input formats. It did in the 2021 listing posted by a Raspberry Pi engineer [D-17], but the list is read from the firmware at probe [D-16] (OQ-057).
- A Raspberry Pi engineer reported that both TC358743 formats are accepted directly [D-18].
- 1080p30 is within the official specification [D-10].
- **1080p60 is unproven** (RISK-002, OQ-056), and applies to CM4 CAM1 only (2-lane connectors cannot capture it [C-01], [C-02], [C-48]):
  - the driver calls Level 4.0 the hardware specification and says higher levels may not keep up in real time [D-12];
  - reasoning: 1080p60 needs 2.0x the specified macroblock rate [D-52];
  - one Raspberry Pi engineer (6by9) reported 1080p60 hardware encode as an "edge case" (community report) [D-50];
  - the official 720p120 recipe suggests a GPU overclock, `gpu_freq=550` (reasoning entry) [D-52]; whether an overclock is acceptable in the product is OQ-096.
- Encoder limits:
  - bitrate 25 kbit/s to 25 Mbit/s, VBR or CBR [D-13];
  - no B-frames [D-14];
  - default encoded-buffer size 768 KiB above 720p [D-23].

**Pi 5 / CM5**

- Official figure: H.264 1080p30 encode (from ISP) costs about 30–40 % CPU [G-22]. Whether that is the whole CPU or one core is UNKNOWN (OQ-059).
- One Raspberry Pi engineer (6by9) reported software 1080p60 encode from camera capture as "easily achievable" (community report) [D-50]. Not measured with TC358743 input.
- Software encoders usually have more latency than the old hardware encoders [D-32].

**All platforms**

No candidate platform has a hardware HEVC encoder [D-24], [D-31]. Pass thresholds are UNDEFINED (OQ-005, OQ-010, OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-REC-001 — Recording integrity and duration

| | |
|---|---|
| Verifies | REQ-REC-001 |
| Related risks | None registered in [RISKS.md](RISKS.md) |
| Related open questions | OQ-006, OQ-069 |
| Depends on | TEST-ENC-001 |

### Objective

Confirm that encoded video is written to local storage as a playable file of the expected duration, and record the behaviour on power loss.

### Setup

- As TEST-ENC-001.
- Container, storage medium, minimum duration and power-loss behaviour: UNDEFINED — OWNER DECISION REQUIRED (OQ-006). The storage options of the candidate boards are not in the source register (OQ-098).
- Available muxers: `gstreamer1.0-plugins-good` ships the MP4 (`isomp4`), Matroska and FLV muxers [G-26].
- Storage location (reasoning):
  - The raspi-config read-only option uses `overlayroot=tmpfs` [G-48]. If that option is used, recordings must go to a separate persistent partition.
  - `rpi-image-gen`'s `image-rota` layout provides a shared persistent data partition [G-40].
  - The partition layout is OQ-069.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.** No register fact gives a recording pipeline for this chain, so commands are NEEDS VERIFICATION.

1. Record for the owner-defined duration.
2. Stop recording normally and check the file plays, its duration and its frame count.
3. If OQ-006 requires it, remove power during recording, reboot, and check what is recoverable.

### Expected Result

- No source-derived expectation exists for PACSCORDER recording. Acceptance criteria are UNDEFINED (OQ-006, OQ-017).
- For comparison only: ATEM switchers record H.264 + AAC in MP4 [F-07], [F-26].

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-STR-001 — RTMP publish and playback

| | |
|---|---|
| Verifies | REQ-STR-001 |
| Related risks | RISK-003 |
| Related open questions | OQ-007, OQ-063, OQ-075 |
| Depends on | TEST-ENC-001 |

### Objective

Publish the encoded stream (and audio, if required) to an RTMP server and confirm that it plays back.

### Setup

- As TEST-ENC-001.
- RTMP destination: UNKNOWN — OWNER DECISION REQUIRED (OQ-007).
- Test server: UNKNOWN (OQ-075). Candidates from sources:
  - MediaMTX, which its project reports listens for RTMP on `:1935` [F-44];
  - FFmpeg with its `listen` option [F-32].
- Packages:
  - `rtmp2sink` is in `gstreamer1.0-plugins-bad` (`libgstrtmp2`) [G-27];
  - `flvmux` is in `gstreamer1.0-plugins-good` (`libgstflv`) [G-26].

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

- GStreamer, documented form [F-33]:
  ```bash
  ... x264enc ! flvmux ! rtmp2sink location=rtmp://<server>/<app>/<key>     # [F-33]
  ```
  - On Pi 4 / CM4 with `v4l2h264enc`, an `h264parse` element is needed before `flvmux` (reasoning) [F-35], because `flvmux` needs `stream-format=avc` (CORRECTED) [F-34].
  - Full pipelines are NEEDS VERIFICATION.
- FFmpeg, documented publish form (shown with a file input) [F-32]:
  ```bash
  ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream                  # [F-32]
  ```
  The live-capture input options are NEEDS VERIFICATION.
- Play the stream back from the server with a client. The client is UNDEFINED (OQ-007).

### Expected Result

- The server accepts the publish connection. The default RTMP port is TCP 1935 [F-32].
- The stream carries H.264 video (FLV CodecID 7) and, if audio is included, AAC audio [F-31]. `flvmux` needs raw AAC (CORRECTED) [F-34].
- Pi 4 / CM4: in-band SPS/PPS repetition is off by default on the encoder [D-15]. The official pipeline enables it with `repeat_sequence_header=1` [D-37].
- Pass criteria (bitrate, latency, duration): UNDEFINED (OQ-007, OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-STR-002 — WebRTC browser playback

| | |
|---|---|
| Verifies | REQ-STR-002 |
| Related risks | RISK-019, RISK-003 |
| Related open questions | OQ-008, OQ-063, OQ-073, OQ-074 |
| Depends on | TEST-ENC-001 |

### Objective

Confirm that live PACSCORDER video plays in each target browser over WebRTC, and record the negotiated H.264 profile and level.

### Setup

- As TEST-ENC-001.
- Target browsers, viewer reach (LAN or internet), number of viewers and latency target: UNDEFINED — OWNER DECISION REQUIRED (OQ-008). RISK-019 names Chrome, Firefox and Safari.
- Signalling and NAT traversal: UNDEFINED (OQ-074).
- Packages: `webrtcbin` is in `gstreamer1.0-plugins-bad` (`libgstwebrtc`) [G-27] and needs `gstreamer1.0-nice` at runtime [G-28]. `webrtcbin` has no built-in signalling [F-42].

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.** No register fact gives a WebRTC pipeline or signalling command, so commands are NEEDS VERIFICATION.

1. Start a WebRTC session from each target browser.
2. Capture the SDP offer and answer. Record `profile-level-id`, `packetization-mode` and `level-asymmetry-allowed`.
3. Play 1080p video for the owner-defined duration. Record whether it decodes, plus latency and stalls.
4. If audio is required: confirm an Opus or G.711 audio track.

### Expected Result

- RFC 7742 requires, for H.264 in WebRTC [F-36]:
  - Constrained Baseline;
  - `packetization-mode` 1;
  - `profile-level-id` present in the SDP;
  - SPS/PPS sent in-band, not in `sprop-parameter-sets`.
- Level signalling:
  - `42e01f` is Constrained Baseline Level 3.1 (reasoning) [F-38], and libwebrtc assumes it when `profile-level-id` is absent [F-39].
  - Reasoning: 1080p needs Level 4 or above, and 1080p60 needs Level 4.2, so a strict `42e01f` negotiation does not cover 1080p [F-40].
  - Browser behaviour in that case is UNKNOWN (OQ-073).
- The MediaMTX project reports that browsers do not accept H.264 B-frames in WebRTC [F-45]. The Pi 4 / CM4 encoder produces none [D-14].
- Audio: RFC 7874 requires WebRTC endpoints to implement Opus and G.711; AAC is not a required WebRTC codec, so AAC audio has to be transcoded, typically to Opus, for browser playback [F-41].

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-ATEM-001 — ATEM HDMI output capture (scope per OQ-009)

| | |
|---|---|
| Verifies | REQ-ATEM-001, REQ-CAP-008 |
| Related risks | RISK-008; RISK-018 applies only to part (b), which is not in current scope |
| Related open questions | OQ-009 (ANSWERED 2026-10-07), OQ-078, OQ-083, OQ-102. Not in current scope, kept for reference: OQ-075, OQ-077, OQ-079, OQ-080, OQ-081, OQ-082, OQ-084 |
| Depends on | TEST-CAP-001 and TEST-CAP-002 for (a) (TEST-CAP-002 on the 4-lane configuration only); TEST-STR-001 for (c), which is not in current scope |

### Objective

Confirm the ATEM integration in current scope: **capture of the ATEM switcher's HDMI output through the TC358743** — sub-test (a) below (REQ-ATEM-001, REQ-CAP-008).

Scope is the owner's answer to OQ-009 of 2026-10-07: "it can be atem and direct video from camera". Recorded interpretation (Claude; the owner may correct it):

- In scope: (a) HDMI capture of the ATEM output. Cameras connected directly are the other source type of REQ-CAP-008; they are covered by TEST-CAP-001, TEST-CAP-002 and TEST-CAP-004, not here.
- **Not in current scope:** (b) network tally, state and control over UDP 9910, and (c) receiving the ATEM's RTMP stream. Both were offered and not selected. Their procedures are kept below as reference only and are run only if the owner adds them to REQ-ATEM-001. Sending PACSCORDER video into an ATEM setup (OQ-081) and the ATEM USB-C webcam output (OQ-082) were not selected either.
- Which ATEM models and firmware versions must be supported is OPEN (OQ-102).

### Setup

- ATEM model and firmware version: UNKNOWN — OWNER DECISION REQUIRED (OQ-102). Record both in every result entry.
- (a) **HDMI capture of the ATEM output** (in scope).
  - The ATEM Mini Pro HDMI output defaults to multiview, not program. It is changed with the front-panel VIDEO OUT buttons or in ATEM Software Control [F-25]. On ATEM Mini Extreme, HDMI out 1 defaults to program [F-25].
  - Whether PACSCORDER could switch it over the network is OQ-078. Network control is not in current scope, so this procedure has the operator set the output source.
  - Lane configurations: run on both the 2-lane and the 4-lane configuration (REQ-CAP-007). Reasoning from [F-23] and the TEST-CAP-004 matrix: on the 2-lane configuration the ATEM Mini Pro's 1080p59.94 and 1080p60 standards cannot be captured, and 1080p50 only in UYVY [C-37], [C-48]. Whether the ATEM honours the sink EDID or always outputs its own video standard is UNKNOWN (OQ-083).
- (b) **Network state and control (UDP 9910) — not in current scope (reference only).** The protocol is reported as reverse-engineered by the OpenSwitcher project [F-11]; the official SDK does not document it [F-10]. The library is UNDEFINED (OQ-084).
- (c) **Receiving the ATEM's RTMP stream — not in current scope (reference only).** Reasoning: PACSCORDER must run a listening RTMP server [F-46]; GStreamer `rtmp2src` cannot accept incoming publish connections [F-33].

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

- (a) Set the ATEM HDMI output to Program [F-25]. Run TEST-CAP-001 against it at each output standard of the ATEM model(s) listed in OQ-102. Run TEST-CAP-002 at 1080p60 on the 4-lane configuration, and complete the ATEM rows of the TEST-CAP-004 matrix on both lane configurations.
- (b) *Not in current scope — reference only.* Connect the chosen library. Change program/preview inputs, start and stop recording and streaming on the ATEM, and record the state PACSCORDER receives. Commands depend on OQ-084: NEEDS VERIFICATION.
- (c) *Not in current scope — reference only.* Configure the ATEM to publish to the PACSCORDER server (method OQ-080), receive the stream, and analyse its parameters (OQ-079).

### Expected Result

**(a) HDMI capture** (in scope)

- Frames at the ATEM output standard. ATEM Mini Pro outputs 1080p23.98 to 1080p60 only, with no 1080i or 720p [F-23].
- On the 2-lane configuration, only the standards the link carries are captured; the others are expected to fail at stream start as in TEST-CAP-004 [B-32], [C-48].
- What arrives at the TC358743 is UNKNOWN (OQ-083): pixel encoding, quantisation range, and whether HDCP is asserted. The ATEM's own sampling is 4:2:2 YUV 10-bit Rec 709 [F-23].
- Program audio is embedded on the HDMI output (CORRECTED) [F-24]. Capturing it needs TEST-AUD-001, which applies only if audio is required (OQ-004).

**(b) Network state** (not in current scope — reference only)

- Tally and state changes are received.
- The atem-connection project reports:
  - tally via `TlSr` [F-17];
  - recording and streaming status commands that need protocol 2.30 or later [F-16];
  - protocol versions defined only up to V9_6 [F-18], while Blackmagic lists ATEM 10.4.1 software [F-02].
- Compatibility with current firmware is UNKNOWN (OQ-077, RISK-018).

**(c) RTMP from the ATEM** (not in current scope — reference only)

A stream with H.264 or H.265 video and AAC audio [F-06]. The exact parameters are UNKNOWN (OQ-079).

**All sub-tests**

Pass criteria are UNDEFINED (OQ-017). The scope question OQ-009 is answered; the ATEM model list is OQ-102.

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-BLD-001 — Image build from clean checkout

| | |
|---|---|
| Verifies | REQ-BLD-001, REQ-BLD-002 |
| Related risks | RISK-017, RISK-015 |
| Related open questions | OQ-012 (ANSWERED 2026-10-07), OQ-064, OQ-065, OQ-066, OQ-067, OQ-068, OQ-071, OQ-101 |
| Depends on | A build configuration in this repository (none exists) |

### Objective

Build the PACSCORDER product image from a clean checkout of this repository, by the documented procedure, with pinned kernel, firmware and package versions. Confirm that a later rebuild produces the same image content (REQ-BLD-001).

Confirm also that the product runs this project-built image of its own, not an unmodified stock distribution image (REQ-BLD-002, DRAFT; owner, 2026-10-07: "which is best i need by own one"), and that it boots on the platforms of both lane configurations (REQ-CAP-007).

### Setup

- Build basis: ADR-003 is ACCEPTED (owner, 2026-10-07: "accept ADR-003"; OQ-012 ANSWERED). The product image is built from Raspberry Pi OS packages into the project's own image with `rpi-image-gen`, with a package mirror for reproducibility; Buildroot is the documented alternative, re-evaluated only if measured boot time, image size or reproducibility fails a requirement ([BUILD_SYSTEM.md](BUILD_SYSTEM.md)). Both produce a project-owned image as REQ-BLD-002 requires. The owner delegated the tool choice to Claude's recommendation on 2026-10-07 and accepted ADR-003 the same day.
- `rpi-image-gen`:
  - latest release v2.8.0 [G-37];
  - builds from YAML configs and layers [G-38];
  - supports only native Debian Bookworm or Trixie arm64 hosts; containers and QEMU are "not formally supported" [G-39];
  - minor releases contain breaking changes [G-44].
- Build host: UNKNOWN (OQ-067).
- Package pinning: a mirror or snapshot of `archive.raspberrypi.com` is needed (RISK-017, OQ-067).
- Build configuration in this repository: **none exists** (NOT STARTED).

### Test

**NOT YET RUN.** No build configuration exists, and no register fact gives the `rpi-image-gen` command line, so commands are NEEDS VERIFICATION (OQ-101).

1. Make a clean checkout and run the documented build.
2. Record, as listed in [BUILD_SYSTEM.md](BUILD_SYSTEM.md) §6:
   - the build host OS, architecture and tool versions;
   - the pinned inputs: the `rpi-image-gen` tag or Buildroot release, the kernel and firmware versions, and the package-mirror snapshot;
   - the SBOM and the image checksum.

   The commands that read kernel, firmware and EEPROM versions are NEEDS VERIFICATION (OQ-101).
3. Rebuild later from the same commit and compare SBOMs and image contents.
4. Boot the image on each candidate platform, covering at least one platform of each lane configuration (REQ-CAP-007), and run TEST-PLT-001. Record that the booted image is the one built in step 1 (its checksum from step 2), not the stock bring-up image (REQ-BLD-002). The command that confirms this on the running system is NEEDS VERIFICATION (OQ-101).
5. Check that the image contains:
   - the `tc358743` module for both kernels;
   - the `tc358743` / `tc358743-pi5` / `tc358743-audio` overlays;
   - v4l-utils;
   - the chosen media framework;
   - `i2c-tools`, if TEST-HW-001 is to be run on product images.

### Expected Result

- The build completes on a supported host [G-39] and produces an SBOM [G-36], [G-39].
- Image contents. The stock 2026-10-06 Lite image already has:
  - `tc358743.ko.xz` for both kernels [G-16];
  - Unicam, RP1 CFE and `bcm2835-codec` modules (built as modules in both defconfigs [G-17], [G-18]; present in the image's module trees per reasoning on verified inputs, CORRECTED [G-71]);
  - `v4l2-ctl` / `media-ctl` [G-24].

  It does **not** have `i2c-tools` [G-25] or GStreamer [G-32]. The product configuration must add whatever the chosen framework needs.
- One image can boot all four candidates (reasoning, CORRECTED) [G-71]. Reasoning: one product image can therefore serve the platforms of both lane configurations (REQ-CAP-007), although the overlay, its `cam0`/`4lane` parameters and the capture model still differ per board [G-71].
- The booted system is the project-built image of step 1, identified by its recorded checksum and SBOM (REQ-BLD-002).
- A rebuild produces the same package set. Reasoning: this needs pinning, because `apt full-upgrade` changes the kernel and firmware [G-08], and the default kernel series changed in the middle of the trixie release [G-05].
- If Buildroot were chosen instead (OQ-064, OQ-065, OQ-066):
  - it pins kernel 6.12.61 [E-06];
  - its Pi 5 defconfig installs no overlays [E-15], [G-61];
  - `v4l2h264enc` needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y` [E-32], [D-39].
- Image size, RAM use and boot-to-first-frame targets: UNDEFINED (OQ-068).

### Actual Result

NOT STARTED — no build configuration exists; not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-PERF-001 — Soak: thermal, CPU, CMA, frame drops

| | |
|---|---|
| Verifies | REQ-PERF-001 |
| Related risks | RISK-003, RISK-006, RISK-020 |
| Related open questions | OQ-010, OQ-035, OQ-053, OQ-055, OQ-059, OQ-061, OQ-096 |
| Depends on | TEST-ENC-001 and the selected outputs (TEST-REC-001, TEST-STR-001, TEST-STR-002) |

### Objective

Run the full pipeline (capture, encode, and record and/or stream) continuously across the operating temperature range. Measure frame drops, CPU load, temperatures and contiguous-memory (CMA) use.

### Setup

- Full pipeline as selected by the owner.
- Soak duration, frame-drop threshold, ambient temperature range and enclosure: UNDEFINED — OWNER DECISION REQUIRED (OQ-010).
- Temperature chamber: required if OQ-010 sets a range beyond room temperature.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Start the pipeline at the highest required mode.
2. Sample `CmaFree` from `/proc/meminfo` while streaming at maximum load. This is the measurement named in the reasoning entry [C-53].
3. Sample per-core CPU load, SoC temperature, frame drops and CSI-2 receiver errors at a regular interval. Commands are NEEDS VERIFICATION. How to read CSI-2 receive errors is UNKNOWN on both receivers (RP1 CFE: OQ-050; Unicam: OQ-095).
4. Pi 4 / CM4: if the owner allows an overclocked configuration to be evaluated, soak it as well; whether an overclock is acceptable in the product is OQ-096.
5. Repeat at the temperature extremes from OQ-010.
6. Pi 5 / CM5: if both OS options are evaluated, repeat on both page sizes. Raspberry Pi OS's default bcm2712 kernel uses 16K pages; Buildroot's Pi 5/CM5 defconfigs force 4K pages [E-09], [G-60] (OQ-055).

### Expected Result

**CMA**

- Reasoning, CORRECTED [C-53]: four 1080p capture buffers take about 16.6 MB in UYVY or 24.9 MB in RGB888.
- The DT CMA pool is 64 MB, limited to the lower 768 MB on Pi 4 / CM4 and the lower 1 GB on Pi 5 (CORRECTED) [E-47].
- `vc4-kms-v3d-pi4` sets (512 − 4) MB and `vc4-kms-v3d-pi5` sets 64 MB [C-40].
- Whether Pi 5 / CM5 CFE buffers count against CMA is not established (reasoning from kernel source, CORRECTED) [C-53] (OQ-053).
- `CmaFree` must stay above zero (RISK-020). The margin is UNDEFINED (OQ-061).

**Thermal**

The TC358743XBG is rated −30 to +70 °C ambient [A-41]. Pi thermal limits were not researched: UNKNOWN.

**Capture stability**

- A Raspberry Pi engineer reported that the driver's FIFO trigger level of 374 is an empirical value [A-43]. The driver hard-codes it, with a comment that it suits most modes at 972 Mbps [B-12], [C-44].
- D-PHY timing constants exist only for 594 and 972 Mbps [B-09].
- Long-run and temperature behaviour is therefore UNKNOWN (OQ-035, RISK-006).

**CPU**

Pi 5 / CM5 encode is CPU-bound: about 30–40 % CPU for 1080p30 (official) [G-22] (OQ-059, RISK-003).

**Thresholds**

UNDEFINED (OQ-010, OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## Documentation checks

Documentation consistency checks are recorded in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md), not here. Examples are cross-checking that every cited fact ID exists in [REFERENCES.md](REFERENCES.md), and that REQ, ADR, RISK, OQ and TEST IDs agree across documents.

These checks are not product tests: they have no TEST ID and do not change the status of any requirement.

## Test result log

**Append-only (Rule 21).** Add one row per run. Never edit or delete a row; correct a mistake with a new row that references the earlier one.

- "Result" uses only the Rule 10 status words.
- "Evidence" points to the stored logs and measurements.
- Record the actual device nodes, entity names, kernel version and `config.txt` lines in the evidence. For CM5, also record the carrier board (OQ-052). The commands that read kernel, firmware and EEPROM versions are NEEDS VERIFICATION (OQ-101); record the command actually used.

| Date | Test ID | Platform | Hardware revision | Software version | Result | Evidence | By |
|---|---|---|---|---|---|---|---|

No results recorded as of 2026-10-06.

---

## Verification status

### Verified from sources (fact IDs)

The procedures and expected results above rest on these entries of [REFERENCES.md](REFERENCES.md). All of them have verdict `CONFIRMED` or `CORRECTED`; CORRECTED entries are used in their corrected wording only.

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-04, A-05, A-08, A-11, A-12, A-13, A-15, A-16, A-17, A-18, A-19, A-20, A-22, A-23, A-24, A-30, A-31, A-33, A-35, A-41, A-43, A-45, A-46, A-47, A-50 |
| B — tc358743 Linux driver | B-06, B-07, B-09, B-10, B-11, B-12, B-14, B-15, B-16, B-18, B-19, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-28, B-29, B-30, B-32, B-33, B-34, B-37, B-38, B-39, B-40, B-41, B-42, B-43, B-44, B-46, B-48, B-49 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-08, C-09, C-10, C-11, C-12, C-15, C-16, C-17, C-18, C-19, C-20, C-23, C-24, C-25, C-26, C-27, C-28, C-29, C-31, C-32, C-33, C-34, C-35, C-36, C-37, C-39, C-40, C-41, C-42, C-43, C-44, C-45, C-46, C-47, C-48, C-49, C-50, C-51, C-52, C-53 |
| D — Encoders | D-06, D-07, D-08, D-10, D-12, D-13, D-14, D-15, D-16, D-17, D-18, D-19, D-20, D-21, D-22, D-23, D-24, D-29, D-31, D-32, D-34, D-36, D-37, D-39, D-40, D-43, D-44, D-45, D-48, D-50, D-52, D-53, D-54 |
| E — Buildroot and kernel configuration | E-06, E-09, E-15, E-32, E-37, E-39, E-40, E-43, E-44, E-47, E-48 |
| F — ATEM and streaming | F-02, F-06, F-07, F-10, F-11, F-16, F-17, F-18, F-23, F-24, F-25, F-26, F-31, F-32, F-33, F-34, F-35, F-36, F-38, F-39, F-40, F-41, F-42, F-44, F-45, F-46 |
| G — Raspberry Pi OS and image tooling | G-01, G-04, G-05, G-08, G-11, G-12, G-13, G-14, G-15, G-16, G-17, G-18, G-21, G-22, G-24, G-25, G-26, G-27, G-28, G-29, G-32, G-36, G-37, G-38, G-39, G-40, G-44, G-47, G-48, G-60, G-61, G-71 |

- `CORRECTED` entries cited: A-22, B-11, B-21, B-25, B-44, C-28, C-36, C-39, C-53, E-40, E-47, F-24, F-34, G-11, G-36, G-71.
- `community` entries cited, worded as reports: A-43, C-28, C-33, C-35, C-41, C-42, C-43, C-45, D-17, D-18, D-50, D-54, F-11, F-16, F-17, F-18, F-44, F-45.
- `reasoning` entries cited, labelled as reasoning: A-23, B-10, B-11, B-27, B-33, B-49, C-46, C-47, C-48, C-49, C-50, C-51, C-52, C-53, D-29, D-36, D-52, F-35, F-38, F-40, F-46, G-71.
- Items marked *research gap* or *research open question* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). They are not register facts.
- "Verified from sources" means only that the cited source says so. Under Rule 23, a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No procedure in this document has been run, and no command in it has been executed on PACSCORDER hardware.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review: combined/bare `dtoverlay` parameter syntax marked NEEDS VERIFICATION (consistent with DEVICE_TREE.md §6.0); EDID-persistence statement re-sourced to [A-31], [B-21] plus a research gap instead of [B-22]; C-28 entity-name wording, C-50 margin wording, WebRTC audio wording (F-41) and encoder UYVY scope (D-40, D-43) corrected; defconfig-versus-image distinction added for G-17/G-18 with [G-71]; page-size citations [E-09], [G-60] and CHIPID citation [A-18] added; 22-pin adapter hazard extended to CM4/CM5 IO Boards as reasoning; related-OQ lists completed from the "Resolving test" fields of OPEN_QUESTIONS.md (TEST-CAP-001, TEST-CAP-002, TEST-PLT-001, TEST-DMA-001, TEST-ENC-001, TEST-STR-002, TEST-ATEM-001) and RISK-012 added to TEST-CAP-001 and TEST-CAP-003 per RISKS.md. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: Actual Result of unrun hardware tests now `BLOCKED — HARDWARE REQUIRED — not run` (no "NOT RUN" status; TEST-BLD-001 `NOT STARTED`); `v4l2-ctl -d /dev/v4l-subdevN` and `--clear-edid 0` marked NEEDS VERIFICATION (OQ-101), other unattested command steps linked to OQ-101; bare/combined `config.txt` parameters marked per line (OQ-100); Pi 5 line names `tc358743-pi5` and CM5 split by carrier board with the OQ-052 caveat (no line on the CM4 IO Board), also in TEST-PLT-001; kernel 6.18.39 labelled as the kernel of the [C-33] report only, with 6.18.50 [G-04] / 6.18.55 [E-37] and OQ-097; TEST-CAP-002 notes CM4 CAM0 is 2-lane, that a 4-lane port is necessary but not shown sufficient (OQ-038), and the ADR-008 (PROPOSED; OQ-099) link-frequency proposal and CM4 CAM1 297 MHz evaluation run; TEST-CAP-004 link-frequency text and matrix aligned with ADR-008 and RISK-006; CSI-2 error-counter method linked to OQ-050/OQ-095; TEST-ENC-001 preconditions (TEST-CAP-002 and TEST-DMA-001) aligned with PERFORMANCE.md §10.3, 2-lane 1080p60 limit added [C-01], [C-02], [C-48], optional `gpu_freq=550` run (OQ-056) and overclock acceptability (OQ-096); [D-50] statements attributed to one engineer; TEST-AUD-001 notes audio over CSI-2 [A-05] versus the driver's I2S choice [A-13], with the camera-connector point labelled reasoning; WebRTC audio "typically to Opus" [F-41]; EDID trigger/reload linked to OQ-093; storage facts OQ-098; TEST-BLD-001 evidence list synced with BUILD_SYSTEM.md §6; related-OQ lists extended with OQ-093, OQ-095, OQ-096, OQ-099, OQ-100, OQ-101 per their Resolving-test fields. Final verification pass (same date): TEST-CAP-004 objective notes that the STREAMON lane check compares against the DT `data-lanes` value, not the wiring [B-32], [C-16] (matches REQ-CAP-005 note and CSI_PIPELINE.md §5). | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: "Verifies" (summary and test sections) matched to the README canonical table (TEST-CAP-001, -002, -004, TEST-ATEM-001, TEST-BLD-001 gain REQ-CAP-007/-008, REQ-BLD-002); common setup and test order cover the 2-lane and 4-lane configurations (REQ-CAP-007), ATEM and camera sources (REQ-CAP-008, OQ-102) and the own product image (REQ-BLD-002); TEST-CAP-002 scoped to the 4-lane configuration, 2-lane platforms kept as REQ-CAP-007 candidates; TEST-CAP-004 matrix extended to both configurations, both source types and the other required rates (reasoning [B-28], [B-33], [C-46], [C-47]; [C-17]); TEST-ENC-001 no longer frames OQ-001 as open; TEST-ATEM-001 scope = HDMI capture of the ATEM output (OQ-009 answered), network and RTMP parts marked not in current scope; C-17 and C-46 added to the verification table. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); "Applies to" header, common setup (OS image row), test order and TEST-BLD-001 setup and related open questions now say ADR-003 ACCEPTED and OQ-012 ANSWERED (Buildroot kept as the documented alternative; build configuration still NOT STARTED); TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)" in the summary table and section heading, ID unchanged. | Claude (session 2026-10-07) |
