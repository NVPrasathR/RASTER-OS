# PACSCORDER TC358743 Linux Driver

| | |
|---|---|
| Document status | Active — source research only. Describes the in-tree driver that ADR-002 (PROPOSED) would use unmodified. PACSCORDER integration: NOT STARTED |
| Last updated | 2026-10-08 |
| Applies to | `drivers/media/i2c/tc358743.c`, `drivers/media/i2c/tc358743_regs.h` and `include/media/i2c/tc358743.h` in raspberrypi/linux `rpi-6.18.y` and torvalds/linux `master`, as read on 2026-10-06; the audio setup, audio controls and audio events in `rpi-6.18.y` as read for research topic I on 2026-10-08 (sections 11.7 and 22.1); all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5). Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07; ADR-004 OPEN). |
| Verification | Source inspection only (research of 2026-10-06, plus research topic I of 2026-10-08, [REFERENCES.md](REFERENCES.md)). Nothing has been built, loaded or tested. No PACSCORDER hardware or code exists as of 2026-10-08. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 7 (driver documentation), Rules 8, 10, 22, 23 |

This document answers the Rule 25 question "How is TC358743 controlled?" for the Linux driver layer. It documents what the in-tree `tc358743` driver does, as established by reading its source. It does **not** document observed behaviour: nothing has been run on PACSCORDER hardware ([section 26](#26-what-was-actually-verified-rule-7)).

Related documents:

- Device Tree and overlays: [DEVICE_TREE.md](DEVICE_TREE.md)
- How userspace drives the driver through V4L2 and the Media Controller: [V4L2.md](V4L2.md)
- Lanes, D-PHY rates and receivers: [CSI_PIPELINE.md](CSI_PIPELINE.md)
- The physical board: [HARDWARE.md](HARDWARE.md)
- Test procedures: [TESTING.md](TESTING.md)

Conventions:

- Fact IDs such as `[B-15]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md), `RISK-NNN` to [RISKS.md](RISKS.md), `REQ-…` to [REQUIREMENTS.md](REQUIREMENTS.md), `ADR-NNN` to [DECISIONS.md](DECISIONS.md).
- Facts of tier `community` are worded as reports. Calculations are marked **reasoning** and list their inputs.
- Text marked *research gap* comes from the `gaps` / `open_questions` lists in [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). It is not a register fact and is recorded only to show what is unknown. *(Added 2026-10-08.)* Text marked *research gap* or *research open question* with "topic I" comes from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json), with the same status.
- Source line numbers such as `tc358743.c:2210` are quoted from the register entries. They refer to the file as read on 2026-10-06; the `rpi-6.18.y` and torvalds/linux `master` copies differ only at lines 2361–2362 [B-02], so the quoted numbers hold for both. They will drift as the file changes. *(2026-10-08: the line number quoted in [I-24] comes from the `rpi-6.18.y` branch head as read for research topic I; the packaged 6.18.50 source was not compared — OQ-097 scope note.)*

## 1. Status at a glance

| Item | Status |
|---|---|
| Driver strategy (ADR-002) | PROPOSED — use the in-tree driver unmodified; awaiting owner decision (OQ-013) |
| PACSCORDER patches to the driver | NOT STARTED (none exist; under ADR-002, which is PROPOSED, none would be written until a defect is shown on hardware) |
| Module built and loaded on a PACSCORDER image | NOT STARTED |
| TEST-HW-001 TC358743 I2C detection | BLOCKED — HARDWARE REQUIRED |
| TEST-DRV-001 Driver probe and chip ID | BLOCKED — HARDWARE REQUIRED |
| TEST-DRV-002 EDID load and HDMI hot-plug assertion | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-001 / 002 / 003 / 004 (driver-dependent capture tests) | BLOCKED — HARDWARE REQUIRED |
| TEST-AUD-001 HDMI audio capture over I2S (driver audio setup and controls; added 2026-10-08) | BLOCKED — HARDWARE REQUIRED |
| Requirements served | REQ-DRV-001 (DRAFT), REQ-CAP-003, REQ-CAP-004, REQ-CAP-005 (PROPOSED) — all NOT STARTED. *(Added 2026-10-08: the driver's audio setup and controls also serve REQ-CAP-006, DRAFT — audio required by the owner on 2026-10-07 — NOT STARTED.)* |

## 2. Rule 7 coverage

| Rule 7 item | Section |
|---|---|
| Driver architecture | [5](#5-driver-architecture) |
| Probe sequence | [6](#6-probe-sequence) |
| Remove sequence | [7](#7-remove-sequence) |
| Power sequence | [8](#8-power-sequence) |
| Reset sequence | [9](#9-reset-sequence) |
| I2C communication | [10](#10-i2c-communication) |
| Register initialization | [11](#11-register-initialization) |
| HDMI detection | [12](#12-hdmi-detection) |
| EDID | [13](#13-edid) |
| CSI configuration | [14](#14-csi-configuration) |
| V4L2 sub-device | [15](#15-v4l2-sub-device) |
| Media pads | [16](#16-media-pads) |
| Formats | [17](#17-formats) |
| Frame rates | [18](#18-frame-rates) |
| Interrupts | [19](#19-interrupts) |
| Error handling | [20](#20-error-handling) |
| Recovery | [21](#21-recovery) |
| "Also document what was actually verified" | [26](#26-what-was-actually-verified-rule-7) and [Verification status](#verification-status) |

Additional sections: Kconfig and module ([4](#4-kconfig-and-module)), controls ([22](#22-controls)), module parameters and debugfs ([23](#23-module-parameters-and-debugfs)), mainline vs Raspberry Pi tree parity ([24](#24-mainline-vs-raspberry-pi-tree-parity)), known defects and limitations ([25](#25-known-defects-and-limitations)). *(Added 2026-10-08.)* HDMI audio (REQ-CAP-006): audio configuration ([11.7](#117-audio-configuration-tc358743_set_hdmi_audio)) and audio controls and events ([22.1](#221-hdmi-audio-controls-sampling-rate-decode-updates-and-events)).

## 3. Scope and source baseline

| Item | Value | Facts |
|---|---|---|
| Driver source | `drivers/media/i2c/tc358743.c`, 2385 lines in `rpi-6.18.y` | [B-02] |
| Register definitions | `drivers/media/i2c/tc358743_regs.h`, byte-identical in `rpi-6.18.y` and torvalds/linux `master` | [B-02], [A-48] |
| Platform-data header | `include/media/i2c/tc358743.h` | [A-42], [B-16] |
| Raspberry Pi branch read | `rpi-6.18.y`, the default branch on 2026-10-06; tip `af73e0836bf0`, Makefile version 6.18.55 | [B-01], [E-37] |
| Mainline branch read | torvalds/linux `master` on 2026-10-06; commit SHA not recorded | [A-48]; *research gap* (OQ-036) |
| Kernel in the current Raspberry Pi OS image | 6.18.50 (Raspberry Pi OS Lite 2026-10-06) | [G-04] |
| DT binding | Free-text `Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt`; no YAML schema; identical in both trees | [A-44], [B-03] |
| Documents the driver was written against | Toshiba "TC358743XBG (H2C), Functional Specification, Rev 0.60" (REF_01) and register spreadsheet "TC358743XBG_HDMI-CSI_Tv11p_nm.xls" (REF_02); both non-public. Several registers are marked "Not in REF_01". | [A-42] |
| Public datasheet | 20-page summary, no register map, I2C address or AC timing | [A-42] |

**Version gap.** The driver source was read at the branch tip (6.18.55) [E-37], while the image PACSCORDER would boot carries kernel 6.18.50 [G-04]. Whether `tc358743.c` differs between 6.18.50 and 6.18.55 is UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, OQ-097). OQ-036 covers the related mainline-commit question.

**Source priority (Rule 23).** The driver is "Linux kernel source" (priority 4). Where it disagrees with the datasheet (priority 2), the datasheet wins. Where the public datasheet is silent (register map, timing), the driver is the best public source, but its values come from NDA documents nobody on the project has seen (RISK-005, OQ-027).

## 4. Kconfig and module

| Symbol | Type | Depends on / selects | Raspberry Pi defconfigs | Facts |
|---|---|---|---|---|
| `CONFIG_VIDEO_TC358743` | tristate, "Toshiba TC358743 decoder"; module `tc358743` | depends on `VIDEO_DEV && I2C`; selects `MEDIA_CONTROLLER`, `VIDEO_V4L2_SUBDEV_API`, `HDMI`, `V4L2_FWNODE` | `=m` in arm64 `bcm2711_defconfig` and `bcm2712_defconfig` | [A-35], [E-38], [B-20], [E-39] |
| `CONFIG_VIDEO_TC358743_CEC` | bool | depends on `VIDEO_TC358743`; selects `CEC_CORE` | not set in either defconfig | [A-34], [E-38], [B-20], [E-39] |

- The same defconfig settings hold in the 6.12.61 commit that Buildroot 2026.08 uses [E-39], [E-06]. Both defconfigs also set `CONFIG_I2C_BCM2835=m` and `CONFIG_I2C_MUX_PINCTRL=m` [E-39].
- The Raspberry Pi OS Lite image of 2026-10-06 ships `tc358743.ko.xz` for both the `rpi-v8` and the `rpi-2712` kernels [G-16].
- The driver matches DT compatible `"toshiba,tc358743"` and I2C device id `"tc358743"` [A-20].
- CEC: whether PACSCORDER needs it is OQ-016 (OWNER DECISION REQUIRED). Enabling it changes polling from 1000 ms to 10 ms when no interrupt is wired [A-30], [B-19]. *(2026-10-08: `CONFIG_VIDEO_TC358743_CEC` is also not set in the packaged 6.18.50 `rpi-v8` and `rpi-2712` kernels [I-22].)*
- *(Added 2026-10-08.)* The TC358743 has no ASoC codec driver of its own, and the overlay's ALSA side uses a generic stub codec [I-02]. Reasoning from [I-02]: the driver therefore needs no extra Kconfig symbol for audio. The ALSA, I2S and stub-codec options for CM4 and CM5 are in [DEVICE_TREE.md](DEVICE_TREE.md) §7 [I-12].

## 5. Driver architecture

The driver is an I2C client driver that registers one V4L2 sub-device [A-20], [B-15], [B-18]. It registers that sub-device asynchronously (`v4l2_async_register_subdev`) [B-15]. The CSI-2 receiver driver binds to it: downstream Unicam on Pi 4/CM4 [C-09], downstream RP1 CFE on Pi 5/CM5 [C-29]. The receiver driver owns the capture video node ("unicam-image" [C-36], "rp1-cfe-csi2_ch0" [C-32]). Capture buffers belong to the receiver side: on Pi 4/CM4 Unicam allocates them from CMA, while on Pi 5/CM5 whether CFE buffers come from CMA is not established (reasoning from kernel source, CORRECTED [C-53]; OQ-053; see [DMA.md](DMA.md)).

```text
 userspace (PACSCORDER capture application — NOT STARTED)
   │ /dev/v4l-subdevN: EDID, DV timings, pad format, events, controls      /dev/videoN: formats, buffers
   ▼                                                                          ▲
 ┌─────────────────────────────────────────────┐   media link     ┌─────────┴──────────────────────────┐
 │ tc358743 (I2C client, V4L2 sub-device)       │  pad 0 (source) │ CSI-2 receiver driver               │
 │ entity function MEDIA_ENT_F_VID_IF_BRIDGE    │ ──────────────► │ Pi 4/CM4: downstream Unicam         │
 │ ops: core / video / pad (section 15)         │                 │ Pi 5/CM5: downstream RP1 CFE        │
 │ IRQ handler or 1000 ms poll timer            │                 │ (get_mbus_config → lane count)      │
 │ delayed work: enable hot-plug                │                 └─────────────────────────────────────┘
 └──────────────────────┬──────────────────────┘
                        │ I2C: 16-bit register addresses, little-endian values
                        ▼
                 TC358743 silicon (HDMI-RX → CSI-2-TX)
```

Sources for the diagram: [B-18], [B-38], [B-19], [B-21], [A-17], [B-31], [C-36], [C-32].

| Component | What it is | Facts |
|---|---|---|
| I2C accessors | 16-bit register address sent MSB first; 8/16/32-bit little-endian values; at most 130 bytes per write | [A-17] |
| DT parser `tc358743_probe_of()` | Reads the CSI-2 endpoint, `refclk` clock, link frequency, optional reset GPIO; computes PLL values and selects D-PHY timing constants | [B-06], [B-07], [B-08], [B-09], [B-13] |
| Platform data (hard-coded on DT systems) | `ddc5v_delay = DDC5V_DELAY_100_MS`, `enable_hdcp = false`, `fifo_level = 374` | [B-12] |
| Runtime state | Current DV timings, media bus code, lanes in use (`csi_lanes_in_use`), EDID block count (`edid_blocks_written`) | [B-30], [A-09], [B-31], [A-33] |
| Control handler | 3 controls (section 22) | [B-16] |
| Interrupt path | Threaded IRQ if the I2C client has one; otherwise a poll timer and its work item | [B-19], [B-40] |
| Hot-plug work | Delayed work that raises HPD after an EDID is written and +5V is present | [B-21] |
| CEC adapter | Only with `CONFIG_VIDEO_TC358743_CEC` | [A-34], [B-15] |
| debugfs | InfoFrame entries created at probe, freed at remove | [B-15], [B-40] |

What the driver does **not** contain: regulator handling [C-23], IR support (the IR block is held in reset) [A-49], automatic HDCP authentication on DT platforms [A-04], interlaced capture [B-27], and runtime power management (*research gap*, topic B; OQ-039).

## 6. Probe sequence

Order from [B-15]. Sub-steps 3b–3h of `tc358743_probe_of()` are ordered by the source line numbers quoted in [B-06] (L2021–2045), [B-11] (L2049), [B-12] (L2056–2070), [A-22] (L2072–2085), [B-08] (L2092–2101), [B-09] (L2103–2139) and [B-13] (L2141–2150). The line of the `devm_clk_get` call (3a) is not quoted in the register; [B-15] lists "clock" before "endpoint", so it is placed first here (NEEDS VERIFICATION, KERNEL SOURCE INSPECTION REQUIRED).

| # | Step | Detail | On failure | Facts |
|---|---|---|---|---|
| 1 | I2C adapter check | Requires `I2C_FUNC_SMBUS_BYTE_DATA` | returns `-EIO` | [A-20], [B-14] |
| 2 | Allocate state | `devm_kzalloc` | — | [B-15] |
| 3a | Reference-clock lookup | Gets the clock named `refclk` (`devm_clk_get`) | Logs "failed to get refclk"; return value not stated in the register | [A-22], [B-07] |
| 3b | DT endpoint | Endpoint on port 0 must parse as `V4L2_MBUS_CSI2_DPHY`, with 1–4 data lanes and at least one `link-frequencies` entry. The binding marks these optional; the driver requires them. | `-EINVAL`: "missing endpoint node", "missing CSI-2 properties in endpoint" or "invalid number of lanes" | [B-06] |
| 3c | Reference-clock enable | `clk_prepare_enable(refclk)` (L2049) | — | [B-11], [B-07] |
| 3d | Hard-coded platform data | `ddc5v_delay` 100 ms, `enable_hdcp = false`, `fifo_level = 374` | — | [B-12] |
| 3e | Reference-clock rate | Accepts 26 000 000, 27 000 000 or 42 000 000 Hz; `pll_prd = refclk_hz / 6000000` (4, 4, 7) | Logs "unsupported refclk rate: %u Hz" but **returns 0** (kernel source [A-22]; consequence by reasoning [B-11]); see section 25, defect D1 | [B-07], [A-22], [B-11] |
| 3f | Lane rate | `bps_pr_lane = 2 × link_frequencies[0]`, must be 62.5 Mbps – 1 Gbps; `pll_fbd = bps_pr_lane / refclk_hz × pll_prd` (integer) | `-EINVAL`, "unsupported bps per lane" | [B-08] |
| 3g | D-PHY timing constants | Tables exist only for 594 and 972 Mbps per lane | Other rates: warning "untested bps per lane", 594 Mbps constants used | [B-09], [A-24] |
| 3h | Reset GPIO (optional) | If `reset-gpios` exists: pulse reset (section 9) | — | [A-28], [B-13] |
| — | `probe_of` error path | `clk_disable_unprepare` is called only here | — | [B-40] |
| 4 | Sub-device init | `v4l2_i2c_subdev_init`; flags `V4L2_SUBDEV_FL_HAS_DEVNODE \| V4L2_SUBDEV_FL_HAS_EVENTS` | — | [B-15], [B-18] |
| 5 | Chip detection | Reads 16-bit `CHIPID` (0x0000); requires `(chipid & 0xff00) == 0`. The revision byte is not checked; its value on production silicon is UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-029). | `-ENODEV`, "not a TC358743 on address 0x%x" with the 8-bit address (0x1e for 7-bit 0x0f) | [A-18], [A-19], [B-14], [A-16] |
| 6 | Controls | Handler with 3 controls; values updated | — | [B-15], [B-16] |
| 7 | Media entity | One pad, `MEDIA_PAD_FL_SOURCE`; function `MEDIA_ENT_F_VID_IF_BRIDGE` | — | [B-18] |
| 8 | Default media bus code | `MEDIA_BUS_FMT_RGB888_1X24` | — | [A-09], [B-15] |
| 9 | Locks and work | Mutex; delayed hot-plug work | — | [B-15] |
| 10 | CEC adapter | Allocated only with `CONFIG_VIDEO_TC358743_CEC` | — | [B-15], [A-34] |
| 11 | `tc358743_initial_setup()` | Register initialization (section 11) | `BUG_ON` if the reference clock is not 26/27/42 MHz | [B-50], [A-22] |
| 12 | Default timings | `s_dv_timings(V4L2_DV_BT_CEA_640X480P59_94)` | — | [B-15], [C-19] |
| 13 | CSI colour space | `set_csi_color_space` | — | [B-15] |
| 14 | Interrupt init | `init_interrupts` | — | [B-15] |
| 15 | IRQ or poll timer | Threaded IRQ (`IRQF_TRIGGER_HIGH \| IRQF_ONESHOT`, name "tc358743") if the client has an IRQ; otherwise a timer that first fires after 1000 ms | — | [B-19] |
| 16 | CEC registration | `cec_register_adapter` | — | [B-15] |
| 17 | Enable interrupts | `enable_interrupts(+5V present)`, then `INTMASK` | — | [B-15] |
| 18 | Control setup | `v4l2_ctrl_handler_setup` | — | [B-15] |
| 19 | Async registration | `v4l2_async_register_subdev` — the receiver driver can now bind | — | [B-15] |
| 20 | InfoFrame registers, debugfs | InfoFrame register setup; debugfs InfoFrame entries | — | [B-15] |
| 21 | Probe message | "%s found @ 0x%x (%s)" with the 8-bit address | — | [A-16] |

State after a successful probe: no EDID (`edid_blocks_written == 0`), so the driver does not raise HPD [B-21], [A-33]; timings 640x480p59.94 [B-15]; media bus code RGB888_1X24 [A-09]. A source therefore sees no sink until userspace writes an EDID [A-33] (section 13, REQ-CAP-003, RISK-010). The silicon's own HPD level between power-on and the first EDID write is UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, HARDWARE TEST REQUIRED, OQ-032).

Expected kernel log lines while probing, all from the register (none observed on PACSCORDER):

| Message | Meaning | Facts |
|---|---|---|
| `not a TC358743 on address 0x1e` | CHIPID read failed or chip-ID byte non-zero (7-bit address 0x0f printed as 8-bit) | [A-16], [A-19] |
| `… found @ 0x1e (…)` | Probe reached the end | [A-16] |
| `untested bps per lane: … bps` | Link frequency other than 297 or 486 MHz; 594 Mbps constants used | [A-24], [B-09] |
| `failed to get refclk` | No clock named `refclk` could be obtained (DT `clocks` / `clock-names`, required by the binding) | [A-22], [A-44] |
| `unsupported refclk rate: … Hz` | Reference clock not 26/27/42 MHz; a kernel BUG follows if CHIPID still reads | [A-22] |
| `unsupported bps per lane` | Link frequency outside 31.25–500 MHz (reasoning: half of 62.5 Mbps – 1 Gbps [B-08], [A-44]) | [B-08] |
| `missing endpoint node` / `missing CSI-2 properties in endpoint` / `invalid number of lanes` | DT endpoint problem | [B-06] |

## 7. Remove sequence

Order from [B-40]:

1. If there is no IRQ: stop the poll timer (`timer_delete_sync`) and flush the poll work.
2. Cancel the delayed hot-plug work.
3. Free the debugfs InfoFrame entries.
4. Unregister the CEC adapter.
5. `v4l2_async_unregister_subdev`.
6. `v4l2_device_unregister_subdev`.
7. Destroy the mutex.
8. `media_entity_cleanup`.
9. Free the control handler.

What remove does **not** do [B-40]:

- it does not drop HPD;
- it does not assert reset;
- it does not disable the reference clock.

Consequences:

- After `rmmod`, the HDMI source may continue to see hot-plug asserted (*research gap*, topic B). Whether it keeps transmitting, and whether module reload is a usable recovery action, is UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-039).
- On Raspberry Pi the reference clock is a `fixed-clock` node that only declares a frequency, so the oscillator must be on the bridge board [A-45], [B-47] (PACSCORDER's board: UNKNOWN — VERIFICATION REQUIRED, OQ-019). Reasoning: the physical clock keeps running whatever the driver does with the clock API [B-11].
- What Unicam or RP1 CFE do when their source sub-device unregisters is not in the register: NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED).

## 8. Power sequence

**What the driver does.** Nothing. It requests no regulator, and the stock overlay references no camera regulator [C-23]. It has no runtime power management (*research gap*, topic B). It enables the reference clock in `probe_of` and never disables it after a successful probe [B-40]. During initial setup it takes the chip out of sleep mode [B-50]. No step that puts the chip back to sleep appears in the probe, stream or remove sequences recorded in the register (reasoning from [B-15], [B-36], [B-40]); whether any other code path does so is NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED).

**What the silicon needs (datasheet).**

| Rail | Nominal (range) | Facts |
|---|---|---|
| VDDC1 / VDDC2 (core) | 1.2 V (1.1–1.3 V) | [A-36] |
| VDD_MIPI | 1.2 V (1.1–1.3 V) | [A-36] |
| AVDD12 (HDMI PHY) | 1.2 V (1.15–1.25 V) | [A-36] |
| AVDD33 (HDMI PHY) | 3.3 V (3.135–3.465 V) | [A-36] |
| VDDIO1 (HDMI digital I/O) | 3.3 V (3.0–3.6 V) | [A-36] |
| VDDIO2 (digital I/O: host I2C, RESETN, INT, REFCLK, audio) | 1.8 V or 3.3 V (1.65–3.6 V) | [A-36], [A-39], [A-27], [A-29], [A-21], [A-12] |
| AVDD25 (APLL) | 2.5 V (2.25–2.75 V) | [A-36] |

- Supply noise limits: 0.1 V p-p general, 0.08 V on AVDD33, 0.04 V on AVDD12. REXT connects to AVDD33 through 2 kΩ ±1 %; VPGM is tied to ground [A-37].
- Typical power: 480.5 mW at 720p60, 543.2 mW at 1080p60; sleep mode (register 0x0002 = 0x0001) 108.9 µW. VDDC1 is always on; VDDC2 can be shut off in deep sleep [A-38].
- **Power-up sequencing between rails, and the time from power-good to I2C-ready: UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, OQ-031).** The public datasheet does not contain them (*research gap*, topic A).

**Platform side.** The base Device Trees define camera power-enable regulators: on Pi 4B `cam1_reg` uses expander GPIO 5 and `cam0_reg` is a dummy; on CM4 both ports share expander GPIO 5 [C-21]; on Pi 5 RP1 GPIO 34 (MIPI 0) and RP1 GPIO 46 (MIPI 1); on CM5 RP1 GPIO 34, shared by both ports [C-22]. Because the stock overlay consumes no camera regulator, that line is expected to stay low while the TC358743 is in use (reasoning) [C-51]. On the CM5 IO Board a camera on CAM/DISP 1 cannot be powered down [C-06].

**PACSCORDER board.** How the bridge board is powered, whether it uses the connector power-enable pin, and its VDDIO2 voltage: UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED; OQ-022, OQ-023, OQ-024).

## 9. Reset sequence

**Pin.** RESETN (ball G5) is the system reset input: active low, Schmitt input, VDDIO2 domain [A-27].

**Driver GPIO reset.** `reset-gpios` is optional. The driver gets it with `devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW)`, i.e. deasserted [A-28], [B-13]. If present, after the reference clock is enabled and before the CHIPID read [B-13]:

```text
t = 0           GPIO requested, logical 0 (deasserted)
wait 5–10 ms
assert          logical 1 for 1–2 ms
deassert        logical 0
wait 20 ms
CHIPID read     (probe step 5)
```

- Polarity comes from the DT flag; the binding example uses `GPIO_ACTIVE_LOW` [B-13], [A-27].
- The 1–2 ms assert and 20 ms settle times are the driver's choice, not a published Toshiba specification [A-28]. **Minimum RESETN pulse width and reset-to-I2C-ready time: UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, OQ-031).**

**Stock overlay.** It has no `reset-gpios` property [B-41], [C-23]. With the stock overlay the driver therefore never toggles RESETN.

**Soft resets.** `tc358743_initial_setup()` pulses the CSI-TX and HDMI resets (`MASK_CTXRST | MASK_HDMIRST`). It holds the IR block in reset, and the CEC block too unless `CONFIG_VIDEO_TC358743_CEC` is set [B-50], [A-49].

**Remove** does not assert reset [B-40].

**PACSCORDER.** Whether RESETN is driven by a Pi GPIO (and which), or only by a power-on reset circuit: UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, OQ-020).

## 10. I2C communication

| Item | Value | Facts |
|---|---|---|
| 7-bit address | 0x0f in the DT binding example and the Raspberry Pi overlay; the public datasheet states no address and no strap | [A-15] |
| Address in driver log messages | 8-bit form, 0x1e for 0x0f | [A-16] |
| Address strap | Undocumented for TC358743XBG. The sister part TC358749XBG selects 0x0F/0x1F by the INT pin level at reset. | [A-50]; OQ-026 |
| Register addressing | 16-bit register address, MSB first | [A-17] |
| Register values | Little-endian 8, 16 or 32-bit accesses | [A-17] |
| Largest single write | 130 bytes (2 address bytes + 128 data bytes) | [A-17] |
| EDID upload | 128-byte block writes to EDID RAM at 0x8C00 | [B-22], [A-32] |
| Adapter requirement | `I2C_FUNC_SMBUS_BYTE_DATA` | [A-20] |
| Bus speed (silicon) | Datasheet: 100 kHz and 400 kHz. Older product brief also lists 2 MHz. The documents conflict. | [A-14]; OQ-031 |
| Bus speed (Pi 5 camera bus) | `i2c_csi_dsi1` (MIPI1 connector) is set to 100 kHz in `bcm2712-rpi-5-b.dts` | [B-44] |
| Pin voltage | Host I2C pins are in VDDIO2 (1.8 V or 3.3 V); HDMI DDC pins are separate, in VDDIO1, 5 V tolerant | [A-39] |
| Background traffic without IRQ | Interrupt status read over I2C every 1000 ms (every 10 ms with CEC) | [B-19], [A-30] |
| Debug register access | `g_register` / `s_register` only with `CONFIG_VIDEO_ADV_DEBUG`; HDCP registers are write-protected | [B-38] |

Camera I2C bus per platform (details in [DEVICE_TREE.md](DEVICE_TREE.md)):

| Platform / connector | Bus | Facts |
|---|---|---|
| Pi 4B, CM4 CAM1 | `i2c-10` (`i2c_csi_dsi`, mux channel over BCM2711 i2c0, GPIO 44/45) | [C-24], [C-25] |
| CM4 CAM0 | `i2c-0` (GPIO 0/1) | [C-24], [C-25] |
| Pi 5 CAM/DISP0 | `i2c-10` (RP1 i2c6, GPIO 38/39) | [C-26] |
| Pi 5 CAM/DISP1 | `i2c-11` (RP1 i2c4, GPIO 40/41) | [C-26] |
| CM5 | Depends on the IO board DT | [C-27], [B-44]; OQ-052 |

The register entries above describe the Raspberry Pi side only. The address the chip actually answers at on the PACSCORDER board, and which other devices share that bus: UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-026; resolved by TEST-HW-001).

A *research gap* (topic A) advises against strongly driving or pulling INT during reset until the strap question is resolved, because INT is an address strap on TC358749XBG [A-50]. That is a board-design constraint for [HARDWARE.md](HARDWARE.md), not a register fact.

## 11. Register initialization

### 11.1 Platform data hard-coded on DT systems

| Field | Value | Effect | Facts |
|---|---|---|---|
| `ddc5v_delay` | `DDC5V_DELAY_100_MS` | Reasoning from the name only: a delay applied to +5V (DDC) detection. The register does not describe its effect: NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED for the driver's use, DATASHEET REQUIRED for the register meaning) | [B-12] |
| `enable_hdcp` | `false` | Sets `MASK_MANUAL_AUTHENTICATION` in `HDCP_MODE`: no automatic HDCP authentication | [A-04], [B-12] |
| `fifo_level` | 374 | FIFO trigger level, written to `FIFOCTL` during initial setup [B-50]. The driver comment says 16 fails at higher rates, and that 374 suits 720p60 / 1080p60 at 594 Mbps and "most modes on 972Mbps" | [B-12], [B-50], [C-44] |

A Raspberry Pi engineer (6by9) reported that Toshiba's FIFO formula is in an NDA datasheet; the value 374 in the driver is empirical [A-43]. RISK-006, OQ-035.

### 11.2 PLL from the reference clock

`pll_prd = refclk_hz / 6000000`; `pll_fbd = bps_pr_lane / refclk_hz × pll_prd` (integer); lane rate = `(refclk_hz / pll_prd) × pll_fbd`. A driver comment says the PLL input (`refclk / pll_prd`) must be 6–40 MHz [B-07], [B-08], [A-23].

Resulting lane rates (reasoning, from [B-10] and [A-23]):

| Reference clock | `pll_prd` | Lane rate for link-frequency 486 MHz (nominal 972 Mbps) | Lane rate for link-frequency 297 MHz (nominal 594 Mbps) |
|---|---|---|---|
| 26 MHz | 4 | 962 Mbps (`pll_fbd` 148) | 572 Mbps (`pll_fbd` 88) |
| 27 MHz | 4 | 972 Mbps (`pll_fbd` 144) | 594 Mbps (`pll_fbd` 88) |
| 42 MHz | 7 | 966 Mbps (`pll_fbd` 161) | 588 Mbps |

Only 27 MHz gives the nominal rates [B-10]. Reasoning: the D-PHY timing table is selected by the nominal `bps_pr_lane` [B-09], while the lane-count formula uses the actual rate [B-31]; at 26 or 42 MHz the 972 Mbps constants would be used at 962 or 966 Mbps. Whether that matters is UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, OQ-035).

The bridge board must carry its own oscillator, because the Pi does not generate REFCLK [A-45], [B-47]. **PACSCORDER's oscillator frequency: UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED, OQ-019).**

### 11.3 D-PHY timing constants

Constants exist only for 594 000 000 and 972 000 000 bps per lane. Example at 972 Mbps: `lineinitcnt 0x1b58`, `tclk_headercnt 0x2806`, `ths_headercnt 0x0806`, `twakeup 0x4268`, `ths_trailcnt 0x5`. Any other rate logs "untested bps per lane" and falls through to the 594 Mbps constants [B-09], [A-24]. The driver comment marks them "FIXME: These timings are from REF_02" [B-09].

### 11.4 `tc358743_initial_setup()`

Steps 1–6 follow the source lines quoted in [B-50] (L946–966). Steps 7–10 are listed in the order in which [B-50] names them; their exact call order is not quoted in the register (NEEDS VERIFICATION, KERNEL SOURCE INSPECTION REQUIRED).

1. `SYSCTL`: hold IR in reset (`MASK_IRRST`), and CEC unless `CONFIG_VIDEO_TC358743_CEC` [B-50], [A-49].
2. Pulse CSI-TX and HDMI resets (`MASK_CTXRST | MASK_HDMIRST`) [B-50].
3. Leave sleep mode [B-50].
4. `FIFOCTL = fifo_level` (374) [B-50], [B-12].
5. `tc358743_set_ref_clk()`: `SYS_FREQ = refclk/10000`, `FH_MIN = refclk/100000`, `FH_MAX = FH_MIN × 66 / 10`, `LOCKDET_REF = refclk/100`. Contains `BUG_ON` for rates other than 26/27/42 MHz [B-50], [A-22].
6. `EDID_MODE` = E-DDC [B-50].
7. HDMI PHY configuration [B-50].
8. HDCP: manual authentication (disabled) [B-50], [A-04].
9. Audio: 2-channel I2S — `SDO_MODE1 = MASK_SDO_FMT_I2S`; `CONFCTL` with `MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S | MASK_AUTOINDEX` [A-13]. *(2026-10-08: this is `tc358743_set_hdmi_audio()`; full register list in section 11.7 [I-24].)*
10. InfoFrame capture [B-50].

Reasoning (inputs: formulas in [B-50], 27 MHz from the overlay [A-45]). At 27 MHz the reference-clock registers hold `SYS_FREQ` 2700, `FH_MIN` 270, `FH_MAX` 1782 and `LOCKDET_REF` 270000. These values are useful when reading a register dump during bring-up. Frame-rate detection depends on `SYS_FREQ` being exact [B-28]. Reasoning: a DT `clock-frequency` that is accepted (26/27/42 MHz) but differs from the fitted oscillator would therefore distort reported frame rates and lane rates.

### 11.5 After initial setup

The probe applies default timings `V4L2_DV_BT_CEA_640X480P59_94` through `s_dv_timings` [B-15], which programs PLL and CSI [B-30]. It then sets the CSI colour space [B-15]. Media bus code: RGB888_1X24 [A-09].

### 11.6 Register-header notes

- `CHIPID` 0x0000: chip ID in bits [15:8], revision in [7:0]; no named expected value [A-18].
- `CONFCTL` 0x0004 `YCBCRFMT` codes: 444 = 0x0000, 422_12_BIT = 0x0040, COLORBAR = 0x0080, 422_8_BIT = 0x00c0. The driver does not use the 12-bit 4:2:2 or colour-bar modes [A-10] (OQ-033).
- `CONFCTL` `AUDOUTSEL`: CSI 0x0000, I2S 0x0010, TDM 0x0018 [A-13].
- EDID RAM 0x8C00–0x8FFF; `EDID_LEN1`/`EDID_LEN2` 0x85CA/0x85CB [A-32].
- CEC registers 0x0600–0x06FF (`CECEN` 0x0600) [A-34].
- `CSI_CONTROL` 0x040C is written indirectly through `CSI_CONFW` 0x0500 [A-25].
- `HPD_CTL` 0x8544, bit `HPD_OUT0` [B-21].
- *(Added 2026-10-08.)* Audio: `FS_SET` 0x8621 with `MASK_FS` 0x0f; `AU_STATUS0` 0x8523 with `MASK_S_A_SAMPLE` 0x01 [I-19]. `CONFCTL` channel count: `MASK_AUDCHNUM_8` 0x0000, `_6` 0x0400, `_4` 0x0800, `_2` 0x0c00 [I-24].

### 11.7 Audio configuration (`tc358743_set_hdmi_audio()`)

*Added 2026-10-08 from research topic I.* HDMI audio is required (REQ-CAP-006, DRAFT; owner 2026-10-07, OQ-004 ANSWERED).

**When it runs.** `tc358743_set_hdmi_audio()` is called only from `tc358743_initial_setup()`, which runs once at probe (line 2267 as quoted in [I-24]). Reasoning from [I-24]: the audio configuration is never changed after probe, by stream start, by a mode change or by a sample-rate change.

**What it writes** [I-24]:

| Register | Value written | Effect stated in the source |
|---|---|---|
| `SDO_MODE1` | `MASK_SDO_FMT_I2S` | I2S output format |
| `CONFCTL` | OR-ed with `MASK_AUDCHNUM_2 \| MASK_AUDOUTSEL_I2S \| MASK_AUTOINDEX` | 2 channels, output on I2S |
| `FS_IMODE` | `MASK_NLPCM_SMODE \| MASK_FS_SMODE` | — (meaning not in the public datasheet; see below) |
| `ACR_MODE` | `MASK_CTS_MODE` | — |
| `BUFINIT_START` | 500 ms | — |
| `FS_MUTE` | 0x00 | — |
| Auto-mute / auto-play masks; `ACR_MDF0/1` limits | Set; the values are not quoted in the register | — |
| `DIV_MODE` | Delay 100 ms | — |

- The register header also defines `MASK_AUDOUTSEL_TDM` (0x18), `MASK_AUDOUTSEL_CSI` (0x00) and `MASK_AUDCHNUM_4/6/8`, but the driver never selects them [I-24].
- **Silicon side.** The public datasheet Rev. 1.0 (2017-10-26) gives the I2S output as a single stereo data lane, master-clock mode only, 16/18/20/24-bit data that "depend on HDMI input stream", MSB first left- or right-justified, 32-bit time slots only, with a 256fs oversampling clock; TDM is "Fixed to 8 channels" [I-25]. An internal audio PLL tracks the N/CTS values of the source's ACR packets [I-26], so the I2S clocks follow the source, independently of the CSI-2 video timing (A/V synchronisation: OQ-112, RISK-024). The datasheet says audio can also travel over MIPI CSI-2, but the driver selects I2S [I-26] (OQ-033).
- **Consequences (reasoning from [I-24], [I-25], [I-13]).** The stock driver gives stereo I2S only. 8-channel TDM would need a driver change, and on CM4 `bcm2835-i2s` captures exactly 2 channels [I-13], so it could not take TDM there (research design risk, topic I). Under ADR-002 (PROPOSED) no such patch is written unless a defect is shown on hardware.
- **Unknown (research gaps, topic I — not register facts).** The driver leaves the `SDO_MODE1` bit-length field at 0, so the placement of 16- to 24-bit samples in the 32-bit slots, and whether 32-bit capture keeps every valid bit, are not publicly documented. The meaning of the `FS_IMODE` NLPCM bit and of the auto-mute bits is in Toshiba's non-public register reference. What the chip outputs for compressed (IEC 61937) or multichannel LPCM input is therefore unknown. DATASHEET REQUIRED (OQ-027); HARDWARE TEST REQUIRED — OQ-110, TEST-AUD-001. Which sample rates the silicon supports: OQ-033.
- **Unknown.** How the output behaves while the source changes rate, mutes or switches programme, given the 500 ms `BUFINIT_START` and 100 ms `DIV_MODE` delay [I-24]: research open question, topic I; HARDWARE TEST REQUIRED (OQ-111).

## 12. HDMI detection

The receiver is HDMI-RX 1.4 (reference: HDMI 1.4a); no Audio Return Channel or HDMI Ethernet Channel [A-02]. Maximum TMDS clock is 165 MHz [A-07].

How the driver learns about the source:

| Stage | Driver mechanism | Facts |
|---|---|---|
| Source +5V | +5V (DDC) interrupt handled in `tc358743_hdmi_sys_int_handler()`; `tx_5v_power_present()` | [B-23], [A-33] |
| Hot-plug to source | HPD raised only by delayed work after an EDID is written and +5V is present (section 13) | [B-21], [A-33] |
| TMDS signal and sync | Checked in `get_detected_timings()` (`no_signal`, `no_sync`) | [B-29] |
| Timings | Derived from registers (section 18) | [B-28] |
| Changes | Sync change (`MISC_INT I_SYNC_CHG`) or DE size/position change (`CLK_INT I_IN_DE_CHG`) → `tc358743_format_change()` | [B-39] |
| Delivery to the driver | IRQ, or I2C polling every 1000 ms | [B-19], [A-30] |
| Exposure to userspace | `V4L2_CID_DV_RX_POWER_PRESENT` control; `V4L2_EVENT_SOURCE_CHANGE`; `QUERY_DV_TIMINGS` result | [B-16], [B-39], [B-29] |
| Audio (added 2026-10-08) | CBIT interrupt status: sample-rate change (`MASK_I_CBIT_FS`) updates "Audio sampling rate"; audio lock or unlock (`MASK_I_AF_LOCK`, `MASK_I_AF_UNLOCK`) updates "Audio present". These interrupts are unmasked only while +5V is detected. Userspace can get a `V4L2_EVENT_CTRL` control-change event on a rate change (section 22.1). | [I-21] |

Result of `QUERY_DV_TIMINGS` by input state. Rows 1–2 are reasoning that combines [B-21] (HPD needs EDID and +5V) with [B-29] (HPD low → `-ENOLINK`):

| Input state | Result | Facts |
|---|---|---|
| No EDID written | `-ENOLINK` (HPD low) | [B-21], [B-29] |
| EDID written, no source +5V | `-ENOLINK` (HPD low) | [B-21], [B-29] |
| HPD high, no TMDS signal | `-ENOLINK` | [B-29] |
| Signal, no stable sync | `-ENOLCK` | [B-29] |
| Stable, outside the capability (including interlaced, by reasoning from [B-27]) | `-ERANGE` | [B-29], [B-27] |
| Stable, inside the capability | 0 and the detected timings | [B-29] |
| Pad other than 0 | `-EINVAL` | [B-29] |

When +5V is removed the driver masks every interrupt except DDC, drops HPD, zeroes the stored timings, erases BKSV and updates the controls [B-23].

Unknowns:

- Which pin senses source +5V, and whether the board needs level shifting on HPD/+5V: UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, VENDOR CONFIRMATION REQUIRED, OQ-024).
- HDCP sources: the driver disables automatic HDCP authentication [A-04]. Behaviour with sources that require HDCP is UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-028; RISK-008).

## 13. EDID

**Silicon.** 1 KB embedded EDID SRAM. EDID support follows E-EDID Release A Rev 1: a 128-byte EDID 1.3 base block plus a 128-byte CEA-861-D (version 3) extension [A-31].

**Driver.**

| Behaviour | Detail | Facts |
|---|---|---|
| Interface | `VIDIOC_SUBDEV_S_EDID` / `G_EDID` (`set_edid` / `get_edid` pad ops) | [A-32], [B-22], [B-38] |
| Register window | EDID RAM 0x8C00–0x8FFF; length in `EDID_LEN1/2` | [A-32] |
| Limits | Pad must be 0 and `start_block` 0 (`-EINVAL` otherwise); at most 8 blocks (`-E2BIG`, `blocks` set to 8) | [B-22], [A-32] |
| Validation | CEC physical address in the EDID is validated | [B-22] |
| Write sequence | Drop HPD → write `EDID_LEN1/2` → copy blocks in 128-byte I2C writes → re-enable HPD only if +5V is present | [B-22] |
| Clear | `blocks = 0` clears the EDID; HPD stays low | [B-22] |
| Read with no EDID | `-ENODATA` | [B-22] |
| Hot-plug gating | No EDID → no hot-plug ("no EDID -> no hotplug"); `edid_blocks_written` is 0 after probe | [A-33], [B-21] |
| HPD delay | Delayed work after `HZ/7` jiffies: 140 ms at the Raspberry Pi default `HZ=250` (driver comment says 143 ms) | [B-21] |
| DDC gating | Per a driver comment, DDC access to the EDID is enabled together with HPD (not checked against the datasheet) | [B-21] |
| Persistence | The EDID is held in the chip's 1 KB embedded EDID SRAM [A-31]. After every probe `edid_blocks_written` is 0, so no HPD is raised until userspace writes an EDID [B-21], [A-33]. That the SRAM content is volatile, that the driver has no default EDID and that no DT property supplies one: *research gap* (topic B), not register facts. Whether an EDID survives a driver unload and reload: OQ-093 | [A-31], [B-21], [A-33]; *research gap* (topic B) |

**Userspace tool.** `v4l2-ctl --set-edid pad=<pad>[,type=<type>|file=<file>][,format=<fmt>][modifiers]` sets an EDID; `--clear-edid <pad>` clears it. The built-in type `hdmi` is CTA-861 with HDMI support up to 1080p60 [B-24]. Usage is in [V4L2.md](V4L2.md) (NOT YET RUN ON PACSCORDER HARDWARE). Only these help-text forms are attested; the device-selection option with a sub-device path and the exact `--clear-edid` argument form are NEEDS VERIFICATION (OQ-101).

**Constraints on the PACSCORDER EDID** (PROPOSED, REQ-CAP-003; content is OQ-002, OWNER DECISION REQUIRED):

- Advertise no mode above the 165 MHz TMDS limit [A-07] or outside the driver capability (640–1920 × 350–1200, 13–165 MHz, progressive only) [A-08], [B-26].
- Advertise no interlaced mode: interlaced input returns `-ERANGE` (reasoning) [B-27].
- Advertise no mode the wired lanes cannot carry. Reasoning: the built-in `hdmi` type advertises up to 1080p60 [B-24], which exceeds a 2-lane link at 972 Mbps per lane [C-48].
- Prefer base block + one CEA extension, the structure the datasheet describes [A-31]. The driver would accept up to 8 blocks [A-32]; whether the silicon serves more than 2 blocks to a source is UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, HARDWARE TEST REQUIRED; EDID content is OQ-002). A *research gap* (topic A) also says not to advertise deep colour, because the chip is HDMI 1.4.
- Write the EDID after every boot and every driver load, because the driver starts every probe with `edid_blocks_written == 0` and raises no HPD until an EDID is written [B-21], [A-33]. That the EDID SRAM [A-31] loses its content is a *research gap* (topic B), not a register fact (REQ-CAP-003, RISK-010; TEST-DRV-002). What triggers the write, and its order relative to overlay load, module load and media-graph setup: OQ-093 (OWNER DECISION REQUIRED, HARDWARE TEST REQUIRED).
- HPD level from power-on until the first EDID write: UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, HARDWARE TEST REQUIRED, OQ-032).

## 14. CSI configuration

### 14.1 Inputs from the Device Tree

| Input | Driver use | Facts |
|---|---|---|
| Endpoint on port 0, bus type CSI-2 D-PHY | Required | [B-06] |
| `data-lanes` | Count must be 1–4. Not compared with the lane count the driver later computes. | [B-06], [A-25] |
| `link-frequencies` | Only entry 0 is used; `bps_pr_lane = 2 ×` value; 62.5 Mbps – 1 Gbps | [B-08] |
| `clock-noncontinuous` | Affects only `TXOPTIONCNTRL` in `tc358743_set_csi()` | [B-37] |
| `clocks` / `clock-names = "refclk"` | Required | [B-07], [A-44] |

Raspberry Pi overlay values (`tc358743.dtsi`): 27 MHz reference clock, `link-frequencies` 486000000 (972 Mbps per lane), `data-lanes <1 2>`, `clock-noncontinuous`, and a `4lane` parameter for `<1 2 3 4>` [A-45], [B-41], [C-12]. The overlays README says only 297000000 and 486000000 are supported. Its "574Mbit/s" label for 297 MHz is a typo for 594 [A-46], [B-45]. Overlay details are in [DEVICE_TREE.md](DEVICE_TREE.md). Keeping the 486 MHz default on every platform is proposed in ADR-008 (PROPOSED; owner decision OQ-099).

### 14.2 Lane count at runtime

- The driver computes `lanes = DIV_ROUND_UP(width × height × fps × bpp, lane_rate)`. It counts active pixels only; bpp is 16 for `UYVY8_1X16` and 24 otherwise; `lane_rate = (refclk / pll_prd) × pll_fbd` [A-25], [B-31], [C-15].
- It writes the count into the `NOL` field of `CSI_CONTROL` (0x040C) through the `CSI_CONFW` write port (0x0500). It disables unused data lanes (`D1W/D2W/D3W_CNTRL LANEDISABLE`) and enables `HSTXVREGEN` only for lanes in use [A-25].
- The result is stored in `csi_lanes_in_use`. It is reported to the receiver by `get_mbus_config` (type `V4L2_MBUS_CSI2_DPHY`, flags 0, `num_data_lanes = csi_lanes_in_use`), **not clamped** to DT `data-lanes` or to 4 [B-31], [A-25].
- It is recomputed on every `s_dv_timings` that changes the timings (identical timings return 0 without action) and on every ACTIVE `set_fmt` [B-30], [B-35].
- Both Raspberry Pi receivers call `get_mbus_config` at stream start. They fail with `-EINVAL` ("Device has requested %u data lanes, which is >%u configured in DT") when the bridge asks for more lanes than the receiver endpoint has. Fewer lanes, such as 3 of 4, pass this check [B-32], [C-16].
- Reasoning: a computed count above 4 falls into the `MASK_NOL_1` branch, i.e. a mis-programmed lane count [B-33]. On a 4-lane DT the receiver rejects such a request at stream start [B-32]; a Unicam rejection of a 5-lane request has been reported [C-43].

Lanes the driver requests (reasoning, from [C-47]; payloads from [C-46]):

| Mode | UYVY8_1X16 at 972 Mbps | RGB888_1X24 at 972 Mbps | UYVY8_1X16 at 594 Mbps | RGB888_1X24 at 594 Mbps |
|---|---|---|---|---|
| 1080p60 | 3 | 4 | 4 | 6 |
| 1080p50 | 2 | 3 | 3 | 5 |
| 1080p30 | 2 | 2 | 2 | 3 |

- Whether the receivers capture correctly with 3 active of 4 configured lanes, as 1080p60 UYVY requires at 972 Mbps: UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-038). A 4-lane port is therefore necessary but not shown sufficient for 1080p60 UYVY; ADR-008 (PROPOSED) proposes evaluating 297 MHz, where that mode uses all 4 lanes, on a CM4 CAM1 4-lane link in TEST-CAP-002 (OQ-099).
- A Raspberry Pi engineer (6by9) reported, in an open issue about corruption at 1080p50 RGB888 on 3 of 4 lanes, that the formula uses the active height rather than the total line time, so the computed time per line is wrong [C-43]. RISK-006.
- Bandwidth per platform is analysed in [CSI_PIPELINE.md](CSI_PIPELINE.md).

### 14.3 Stream on and stream off

From [B-36]:

| `s_stream(1)` | `s_stream(0)` |
|---|---|
| Write `TXOPTIONCNTRL = 0`, then `TXOPTIONCNTRL = MASK_CONTCLKMODE`, so the receiver sees the clock-lane LP-11 → HS transition | Mute video (`MASK_AUTO_MUTE \| MASK_VI_MUTE`) |
| Unmute video (`VI_MUTE = MASK_AUTO_MUTE`) | Clear `VBUFEN \| ABUFEN` |
| Set `CONFCTL VBUFEN \| ABUFEN` | Re-run `tc358743_set_csi()` to put all lanes in LP-11 |

`s_stream` always returns 0 and does not check whether a signal is present [B-36].

### 14.4 Clock-lane mode

The DT `clock-noncontinuous` flag only sets `TXOPTIONCNTRL` in `tc358743_set_csi()`. Every stream-on writes `MASK_CONTCLKMODE` (continuous clock). `get_mbus_config` reports flags 0, with the driver comment "Support for non-continuous CSI-2 clock is missing in the driver" [B-37]. The Raspberry Pi overlay sets `clock-noncontinuous` [C-12]. The clock mode actually on the wire, and how Unicam and RP1 CFE treat the mismatch: UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED, OQ-037).

### 14.5 Effect on the Pi 5 / CM5 receiver

The bridge exposes no `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE` control [B-16] and is not a `MEDIA_ENT_F_CAM_SENSOR` entity [B-18]. The RP1 CFE driver therefore cannot read a link rate from it. It logs "Unable to determine sensor link rate, using 999 Mbps" and programs its D-PHY for 999 Mbps [B-49] (reasoning), [C-31]. That matches the default 972 Mbps link but not 594 Mbps (reasoning) [C-52]. RISK-011, OQ-050.

## 15. V4L2 sub-device

Implemented operations, from [B-38]:

| Ops group | Operations |
|---|---|
| core | `log_status`; `g_register` / `s_register` (only with `CONFIG_VIDEO_ADV_DEBUG`; HDCP registers write-protected); `interrupt_service_routine`; `subscribe_event` (`V4L2_EVENT_SOURCE_CHANGE`, `V4L2_EVENT_CTRL`); `unsubscribe_event` |
| video | `g_input_status`, `s_stream` |
| pad | `enum_mbus_code`, `set_fmt`, `get_fmt`, `get_edid`, `set_edid`, `s_dv_timings`, `g_dv_timings`, `query_dv_timings`, `enum_dv_timings`, `dv_timings_cap`, `get_mbus_config` |

Not implemented [B-38]: `enum_frame_size`, `enum_frame_interval`, `enable_streams` / `disable_streams`, `init_state`.

Consequences (reasoning from [B-38], [B-35]):

- Frame size and rate cannot be enumerated on the sub-device pad. Userspace uses the DV-timings operations (`dv_timings_cap`, `enum_dv_timings`, `query_dv_timings`) instead. See [V4L2.md](V4L2.md).
- TRY formats are not stored: `set_fmt` with `V4L2_SUBDEV_FORMAT_TRY` returns 0 without effect [B-35].
- Streaming is started only through the classic `s_stream` operation.
- *(Added 2026-10-08.)* `subscribe_event` passes `V4L2_EVENT_CTRL` to `v4l2_ctrl_subdev_subscribe_event()` [I-21]. This is how userspace can learn of an HDMI audio sample-rate change and reopen ALSA at the new rate (section 22.1; OQ-111).

Sub-device flags: `V4L2_SUBDEV_FL_HAS_DEVNODE | V4L2_SUBDEV_FL_HAS_EVENTS`. A `/dev/v4l-subdevN` node therefore exists when the receiver driver registers sub-device nodes [B-18]. On Pi 4/CM4 in legacy Unicam mode those nodes are registered read-only [C-36]; see [V4L2.md](V4L2.md) for which node carries which ioctl on each platform.

## 16. Media pads

| Property | Value | Facts |
|---|---|---|
| Pads | 1, index 0, `MEDIA_PAD_FL_SOURCE` | [B-18] |
| Entity function | `MEDIA_ENT_F_VID_IF_BRIDGE` | [B-18] |
| Entity name | "tc358743 &lt;bus&gt;-000f". Reported on Pi 5 with kernel 6.18 as "tc358743 11-000f" (CAM/DISP1) or "tc358743 10-000f" (CAM/DISP0); the bus number changed between kernels | [C-28] (community) |

Downstream link per platform:

| Platform | Link from `tc358743:0` | Facts |
|---|---|---|
| Pi 4 / CM4, Unicam in Media Controller mode | `IMMUTABLE \| ENABLED` link straight to the video node "unicam-image"; no CSI-2 receiver sub-device in between. "unicam-embedded" is not registered for this single-pad source. | [C-36] |
| Pi 5 / CM5, RP1 CFE | `IMMUTABLE \| ENABLED` link to the "csi2" sub-device (sink pads 0–3). The "csi2" source pads link to "rp1-cfe-…" video nodes and "pisp-fe" with flags 0 (disabled). | [C-32] |

PACSCORDER's actual entity names, media device and node numbers: UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-043; TEST-PLT-001). Link and pad-format setup is in [V4L2.md](V4L2.md).

## 17. Formats

The driver offers exactly two media bus codes [A-09], [B-34]:

| Index | Media bus code | bpp in lane formula | Colorspace reported | Output encoding | Receiver pixel format | Facts |
|---|---|---|---|---|---|---|
| 0 (probe default) | `MEDIA_BUS_FMT_RGB888_1X24` | 24 | `V4L2_COLORSPACE_SRGB` | RGB, full range | Unicam: `V4L2_PIX_FMT_RGB24`; CFE: `V4L2_PIX_FMT_BGR24` | [A-09], [B-34], [B-31], [C-34] |
| 1 | `MEDIA_BUS_FMT_UYVY8_1X16` | 16 | `V4L2_COLORSPACE_SMPTE170M` | 4:2:2, `VI_REP` BT.601 YCbCr limited range; `CONFCTL YCBCRFMT = 422_8_BIT` | `V4L2_PIX_FMT_UYVY` on both | [A-09], [B-34], [B-31], [C-34] |

`get_fmt` / `set_fmt` behaviour [B-35]:

- Pad 0 only.
- Width and height always come from the current DV timings. They cannot be set through `set_fmt`.
- `field` is always `V4L2_FIELD_NONE`.
- `set_fmt` changes only the media bus code; an unknown code keeps the current one.
- TRY formats are not stored.
- An ACTIVE `set_fmt` mutes the stream and reprograms PLL, CSI (lane count) and colour space.

Notes:

- The silicon accepts RGB / YCbCr 4:4:4 and YCbCr 4:2:2 HDMI input up to 1080p60 [A-07]. The output conversion is selected by the driver as above.
- UYVY is reported as SMPTE170M with BT.601 limited-range conversion [B-34]. That this also applies to 720p/1080p sources, and that the driver has no quantisation-range control, come from the research (*research gap*, topic B), not from the register entry. Encoder colour metadata must match: OQ-041.
- The fourcc assigned to RGB888 differs between receivers: `RGB24` on Unicam, `BGR24` on CFE [C-34]. On CM4, B,G,R memory order despite the `RGB24` label was reported in an issue and explained by a Raspberry Pi engineer [C-35]. RISK-016, OQ-045.
- PACSCORDER default: UYVY — PROPOSED in ADR-005, awaiting owner decision (OQ-003). Because the probe default is RGB888 [A-09], userspace must set UYVY explicitly after every probe (reasoning).
- Unused silicon output modes (12-bit 4:2:2, colour bar) [A-10]: OQ-033.

## 18. Frame rates

There is no frame-interval operation (section 15). The frame rate is part of the DV timings.

How the driver derives detected timings [B-28]:

- Width and height come from the `DE_WIDTH` registers.
- The `hsync` / `vsync` fields carry the total horizontal / vertical blanking; porches are left 0.
- `fps = round(10000 / FV_CNT)`, where `FV_CNT` is the frame interval in 0.1 ms units. This needs `SYS_FREQ` set correctly.
- The pixel clock is computed from the total frame size and the integer fps.
- Fractional rates (59.94, 29.97, 23.976 Hz) are therefore reported as integer-rate pixel clocks [B-28]. How PACSCORDER determines the true rate: OQ-040 (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED).

Capability (`tc358743_timings_cap`) [A-08], [B-26], [C-18]:

| Limit | Value | Source of the limit |
|---|---|---|
| Width | 640–1920 | Driver comment: unknown |
| Height | 350–1200 | Driver comment: unknown |
| Pixel clock | 13–165 MHz | REF_01 p. 20 (NDA) |
| Standards | CEA-861, DMT, GTF, CVT | — |
| Capabilities | progressive, reduced blanking, custom; **not** interlaced | — |

- Datasheet: video input up to 1080p60; TMDS clock up to 165 MHz [A-07].
- Interlaced input is detected but rejected with `-ERANGE` by `query_dv_timings` and `s_dv_timings` (reasoning) [B-27]. RISK-009.
- Example (reasoning from [C-46]): CEA 1080p60 has a 148.5 MHz pixel clock and lies inside the 165 MHz limit.
- Whether the lanes can carry a given rate is section 14.2. The real silicon limits beyond the driver capability: OQ-030 (DATASHEET REQUIRED).

## 19. Interrupts

| Item | Value | Facts |
|---|---|---|
| INT pin | Ball B3; output, active high, level-triggered; VDDIO2; low at initialisation | [A-29] |
| IRQ request | `devm_request_threaded_irq(…, IRQF_TRIGGER_HIGH \| IRQF_ONESHOT, "tc358743")` when the I2C client has an IRQ | [B-19], [A-29] |
| Binding example | `interrupts = <5 IRQ_TYPE_LEVEL_HIGH>` | [A-29], [B-05] |
| Stock Raspberry Pi overlay | No `interrupts` property → polling mode | [A-30], [B-41], [C-20] |
| Poll interval | Timer first fires after 1000 ms, then every 1000 ms (`POLL_INTERVAL_MS`), or every 10 ms when a CEC adapter exists (`POLL_INTERVAL_CEC_MS`) | [B-19], [A-30] |
| Masking at probe | `enable_interrupts(+5V present)`, then `INTMASK` | [B-15] |
| Masking on +5V loss | All interrupts except DDC masked | [B-23] |
| Sources handled (from the register) | +5V / DDC (section 12); `MISC_INT I_SYNC_CHG`; `CLK_INT I_IN_DE_CHG`. CEC interrupt handling (when `CONFIG_VIDEO_TC358743_CEC` is set) is not covered by the register: KERNEL SOURCE INSPECTION REQUIRED | [B-23], [B-39] |
| Audio sources (added 2026-10-08) | CBIT interrupt: `MASK_I_CBIT_FS` (sample rate changed) and `MASK_I_AF_LOCK` / `MASK_I_AF_UNLOCK` (audio lock); unmasked only while +5V / cable is detected | [I-21] |
| Sub-device op | `interrupt_service_routine` | [B-38] |

- Reasoning: without an IRQ, a connect, disconnect or mode change is noticed up to about 1 s late [A-30]. HPD follows about 140 ms after +5V is seen with an EDID loaded [B-21]. RISK-013.
- *(Added 2026-10-08.)* The same polling delays audio: the shared `tc358743.dtsi` has no `interrupts` property and CEC is not enabled in the packaged kernels, so a source sample-rate change can take up to about 1 s, plus I2C time, to reach the "Audio sampling rate" control [I-22] (RISK-023, OQ-111, OQ-020).
- Whether the PACSCORDER board wires INT to a Pi GPIO, and which: UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, OQ-020). Wiring it also needs a DT `interrupts` property ([DEVICE_TREE.md](DEVICE_TREE.md)).

## 20. Error handling

### 20.1 Return codes userspace will see

| Operation | Return | Condition | Facts |
|---|---|---|---|
| `S_EDID` | `-EINVAL` | pad ≠ 0 or `start_block` ≠ 0 | [B-22] |
| `S_EDID` | `-E2BIG` | more than 8 blocks (`blocks` set to 8) | [B-22] |
| `G_EDID` | `-ENODATA` | no EDID stored | [B-22] |
| `QUERY_DV_TIMINGS` | `-EINVAL` / `-ENOLINK` / `-ENOLCK` / `-ERANGE` | pad ≠ 0 / HPD low or no TMDS / no stable sync / outside capability | [B-29] |
| `S_DV_TIMINGS` | `-EINVAL` | pad ≠ 0 or NULL timings | [B-30] |
| `S_DV_TIMINGS` | `-ERANGE` | invalid for the capability | [B-30] |
| `S_DV_TIMINGS` | 0, no action | identical to the current timings | [B-30] |
| `get_fmt` / `set_fmt` | — | pad 0 only; TRY not stored | [B-35] |
| `s_stream` | always 0 | no signal check | [B-36] |
| Receiver STREAMON | `-EINVAL` | bridge requests more lanes than DT `data-lanes` | [B-32], [C-16] |
| Receiver STREAMON (Pi 5) | `-EPIPE` (−32) | reported when `field:none` is left out of the csi2 pad formats | [C-33] (community) |

### 20.2 What the driver does not check

- `S_DV_TIMINGS` does not compare the lane count it needs with DT `data-lanes` [B-30]; the error appears only at the receiver's STREAMON [B-32].
- `s_stream(1)` starts the transmitter even with no signal [B-36]. Reasoning: userspace must have a successful `QUERY_DV_TIMINGS` before STREAMON.
- The reference-clock rate error path does not fail the probe (defect D1, section 25).

### 20.3 Probe errors

See the failure column of section 6.

## 21. Recovery

The driver detects changes and notifies; it never reconfigures timings itself [B-39]. Recovery is therefore a userspace responsibility. The procedure below is **PROPOSED** (REQ-CAP-004, PROPOSED; control model per ADR-006, PROPOSED). It is NOT STARTED, **NOT YET RUN ON PACSCORDER HARDWARE**, and will be checked by TEST-CAP-003.

| Trigger | What the driver does | What userspace must do (PROPOSED) | Facts |
|---|---|---|---|
| Boot or module load | `edid_blocks_written = 0`, so no HPD; timings 640x480p59.94; code RGB888_1X24 | Write the project EDID; set the media bus code (UYVY per ADR-005, PROPOSED); then wait for a signal. What triggers the EDID write and in which order: OQ-093 | [A-33], [B-21], [B-15], [A-09], [B-22] |
| Source connected (+5V appears) | `tc358743_enable_edid()`: HPD after about 140 ms if an EDID is stored | Wait for `V4L2_EVENT_SOURCE_CHANGE`, or poll `QUERY_DV_TIMINGS` until it returns 0 | [B-23], [B-21], [B-29] |
| Sync change or resolution change | Mutes the stream if the signal is lost or the timings differ; sends `V4L2_EVENT_SOURCE_CHANGE` (`V4L2_EVENT_SRC_CH_RESOLUTION`) if a sub-device node exists | Stop streaming → `QUERY_DV_TIMINGS` → `S_DV_TIMINGS` → re-set pad and video-node formats → restart streaming | [B-39]; *research gap* (topic B) |
| Source disconnected (+5V lost) | Masks all but DDC interrupt, drops HPD, zeroes the stored timings, erases BKSV, updates controls | Treat as loss of signal: stop streaming, wait for +5V (control `V4L2_CID_DV_RX_POWER_PRESENT`) | [B-23], [B-16] |
| Unsupported mode | `QUERY_DV_TIMINGS` → `-ERANGE` | Report the mode as unsupported; do not stream (REQ-CAP-005) | [B-29], [B-27] |
| Too many lanes for the wiring | Receiver STREAMON → `-EINVAL` | Report the mode as unsupported for the wired lanes (REQ-CAP-005) | [B-32] |
| No stable sync | `QUERY_DV_TIMINGS` → `-ENOLCK` | Retry until stable or until a timeout to be defined | [B-29] |
| Driver unload and reload | Remove leaves HPD as it was and does not reset the chip; re-probe starts with no EDID | Re-write the EDID (write trigger after a reload: OQ-093). Whether reload is a usable recovery step is UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-039). | [B-40], [B-21]; OQ-039, OQ-093 |
| HDMI audio sample-rate change (added 2026-10-08) | On `MASK_I_CBIT_FS` updates "Audio sampling rate"; accepts `V4L2_EVENT_CTRL` subscriptions. The rate reads 0 when there is no TMDS signal. Up to about 1 s late without INT. The ALSA side is not told. | Subscribe to `V4L2_EVENT_CTRL` for `TC358743_CID_AUDIO_SAMPLING_RATE`; on a change, read the control and reopen ALSA at the new rate; treat 0 as no audio rate. Rate policy (follow the source, or force one rate through the EDID): OQ-111. | [I-19], [I-21], [I-22], [I-18] (reasoning); RISK-023 |
| HDMI audio lock lost or regained (added 2026-10-08) | On `MASK_I_AF_LOCK` / `MASK_I_AF_UNLOCK` updates "Audio present" | Read "Audio present" before opening ALSA and after a change; whether a control event is raised for it, and how the I2S output behaves meanwhile, is NEEDS VERIFICATION (HARDWARE TEST REQUIRED, OQ-111) | [I-21] |

Notes:

- Reasoning: after a disconnect the stored timings are zero [B-23]. `S_DV_TIMINGS` with the re-queried timings is therefore not skipped as "identical" [B-30], even if the source returns in the same mode.
- Reasoning: the capture buffer size depends on the format. A resolution change therefore implies stopping the stream and re-allocating buffers before restarting. NEEDS VERIFICATION on hardware (TEST-CAP-003). Buffer handling is in [DMA.md](DMA.md).
- Whether `tc358743_update_controls()` on +5V loss [B-23] raises a `V4L2_EVENT_CTRL` that userspace can subscribe to [B-38]: NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED, TEST-CAP-003). *(2026-10-08, partly addressed: [I-21] states that userspace can get a control-change event on an audio sample-rate change. The +5V-loss path itself is still NEEDS VERIFICATION.)*
- *(Added 2026-10-08.)* The audio rows above are PROPOSED, NOT STARTED and NOT YET RUN ON PACSCORDER HARDWARE; they will be checked by TEST-AUD-001. The `hw:CARD=tc358743` naming and the per-platform node that carries the controls are in section 22.1.
- Pi 5/CM5: an open issue reports that the rp1-cfe video nodes do not deliver the source-change event, and a Raspberry Pi engineer replied that applications should subscribe on the source sub-device node instead [C-42] (OQ-051). The PROPOSED procedure therefore subscribes on the sub-device node. See [V4L2.md](V4L2.md).
- Detection latency without INT wired: up to about 1 s (section 19, RISK-013).
- Whether HDMIRST in a re-probe's initial setup [B-50] clears HPD: UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED, HARDWARE TEST REQUIRED, OQ-032).

## 22. Controls

The control handler holds exactly 3 controls [B-16]:

| Control | ID | Type / range | Access | Facts |
|---|---|---|---|---|
| `V4L2_CID_DV_RX_POWER_PRESENT` | standard | 0..1 | — | [B-16] |
| "Audio sampling rate" (`TC358743_CID_AUDIO_SAMPLING_RATE`) | `V4L2_CID_USER_TC358743_BASE + 0` = 0x00981980 | INTEGER, 0..768000 | read-only | [B-16], [B-17] |
| "Audio present" (`TC358743_CID_AUDIO_PRESENT`) | `V4L2_CID_USER_TC358743_BASE + 1` = 0x00981981 | BOOLEAN | read-only | [B-16], [B-17] |

- `V4L2_CID_USER_TC358743_BASE = V4L2_CID_USER_BASE + 0x1080` [B-16]. The numeric values are reasoning [B-17].
- No `V4L2_CID_LINK_FREQ` and no `V4L2_CID_PIXEL_RATE` control is registered [B-16]. Consequence: the RP1 CFE falls back to 999 Mbps (section 14.5) [C-31], [B-49]. Separately, Raspberry Pi engineers reported that libcamera does not support the bridge because it is not a raw sensor [C-41] ([V4L2.md](V4L2.md), ADR-001).
- The access mode of `V4L2_CID_DV_RX_POWER_PRESENT` is not stated in the register: NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED).
- With this driver, audio samples leave the chip on I2S, not through V4L2: the driver configures 2-channel I2S output [A-13] (REQ-CAP-006, RISK-014, OQ-004). The I2S path is the driver's choice, not a silicon limit: the chip can also send audio over CSI-2 [A-05]. *(2026-10-08: OQ-004 was ANSWERED on 2026-10-07 — audio is required. The audio controls are described in section 22.1.)*

### 22.1 HDMI audio controls: sampling-rate decode, updates and events

*Added 2026-10-08 from research topic I.* The samples travel on I2S to ALSA (card `tc358743`, [DEVICE_TREE.md](DEVICE_TREE.md) §3.5). The driver's two audio controls are the only place where the HDMI sample rate and audio presence are visible to userspace.

**Control IDs** [I-20] (consistent with [B-16], [B-17]):

| Control | Definition | Numeric ID | Type and access |
|---|---|---|---|
| "Audio sampling rate" `TC358743_CID_AUDIO_SAMPLING_RATE` | `V4L2_CID_USER_TC358743_BASE + 0` | 0x00981980 | INTEGER, 0–768000, step 1, read-only |
| "Audio present" `TC358743_CID_AUDIO_PRESENT` | `V4L2_CID_USER_TC358743_BASE + 1` | 0x00981981 | BOOLEAN, read-only |

`V4L2_CID_USER_TC358743_BASE = V4L2_CID_USER_BASE + 0x1080`, and `V4L2_CID_USER_BASE = V4L2_CID_BASE = (V4L2_CTRL_CLASS_USER | 0x900) = 0x00980900`; so 0x980900 + 0x1080 = 0x981980 [I-20].

**Sampling-rate decode** [I-19]:

- `get_audio_sampling_rate()` reads `FS_SET` (0x8621) masked with `MASK_FS` (0x0f) and maps the code through `code_to_rate[]`:

  | Code | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 |
  |---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
  | Rate (Hz) | 44100 | 0 | 48000 | 32000 | 22050 | 384000 | 24000 | 352800 | 88200 | 768000 | 96000 | 705600 | 176400 | 0 | 192000 | 0 |

- It returns 0 when `no_signal()` is true, that is when `SYS_STATUS` lacks `MASK_S_TMDS`. The driver comment says "Register FS_SET is not cleared when the cable is disconnected".
- `audio_present()` reads `AU_STATUS0` (0x8523) masked with `MASK_S_A_SAMPLE` (0x01).
- The table is the driver's decoder, not a list of rates the silicon supports; the public datasheet gives none (research open question, topic I; OQ-033).

**Updates and events** [I-21], [I-22]:

- The driver updates "Audio sampling rate" when the CBIT interrupt status has `MASK_I_CBIT_FS` set, and "Audio present" on `MASK_I_AF_LOCK` or `MASK_I_AF_UNLOCK`. These CBIT interrupts are unmasked only while +5V / cable is detected [I-21].
- `tc358743_subscribe_event()` accepts `V4L2_EVENT_CTRL` (and `V4L2_EVENT_SOURCE_CHANGE`), so userspace can get a control-change event on a rate change and reopen the ALSA stream at the new rate [I-21].
- Without `interrupts` in the Device Tree the driver polls every 1000 ms; the 10 ms CEC interval does not apply because CEC is not enabled in the defconfigs or packaged kernels. A rate change can therefore take up to about 1 s, plus I2C time, to reach the control [I-22] (OQ-020, OQ-111).

**Where the controls appear** [I-23]:

| Platform / mode | Node that carries the audio controls | Facts |
|---|---|---|
| CM4, `tc358743` overlay default (legacy Unicam, `media-controller` off) | The capture video node `/dev/videoN`: Unicam copies the sensor's controls onto it with `v4l2_ctrl_add_handler()` | [I-23] |
| CM5 (RP1 CFE, `"raspberrypi,rp1-cfe"`) | Only the TC358743 sub-device node `/dev/v4l-subdevN`: CFE never calls `v4l2_ctrl_add_handler()` | [I-23] |
| Pi 4 Model B (legacy mode) / Pi 5 | Reasoning: the same overlay and receiver drivers bind as on CM4 / CM5 ([DEVICE_TREE.md](DEVICE_TREE.md) §6.5), so the same split is expected; not stated in [I-23] | [I-23], [B-43], [C-11] |
| CM4 in Media Controller mode (ADR-006, PROPOSED) | Reasoning from [I-23]: with `mc_api` true Unicam does not copy the controls, so they are only on the sub-device node | [I-23] |

[V4L2.md](V4L2.md) §11 gives the userspace view (ADR-006 PROPOSED would use the sub-device node everywhere).

**What the kernel does not do (reasoning)** [I-18]: the ALSA side has no path from these controls. The `linux,spdif-dir` stub codec has no controls and no `hw_params` and accepts 8–768 kHz; the Pi I2S runs as clock consumer and ignores the requested rate; nothing outside `tc358743.c` uses `TC358743_CID_AUDIO_SAMPLING_RATE`. If the application opens the card at 48000 Hz while the source sends 44100 Hz, the frames arrive at 44.1 kHz but are labelled 48 kHz; played at 48 kHz they run 48000/44100 = 1.0884 times fast (+8.84 %, about +1.47 semitones), and the audio timeline is 44100/48000 = 0.919 of real time, so A/V drift accumulates [I-18]. RISK-023, OQ-111.

**Community evidence.** A 2019 forum thread on a Pi with `bcm2835-i2s` shows `audio_sampling_rate` (0x00981980) reading 48000, read-only, and `audio_present` (0x00981981) reading 1, with capture by `arecord ... -D sysdefault:CARD=tc358743` (community source, 2019 kernel) [I-16]. Not observed on PACSCORDER hardware.

**PACSCORDER use (PROPOSED — NOT STARTED, NOT YET RUN ON PACSCORDER HARDWARE).** Before opening ALSA, read "Audio present" and "Audio sampling rate" on the node that carries them; subscribe to `V4L2_EVENT_CTRL` for the rate control; on a change, close and reopen ALSA at the new rate. Whether to follow the source rate or force one rate through the EDID audio descriptors is OQ-111 (OWNER DECISION REQUIRED, to be recorded with ADR-007). Verified by TEST-AUD-001.

## 23. Module parameters and debugfs

**Source status: from research gaps (topics A and B), not register facts. KERNEL SOURCE INSPECTION REQUIRED (OQ-042).**

| Item | What the research gaps say | Register fact |
|---|---|---|
| Module parameter `debug` | Debug level 0–3 (*research gap*, topic B) | — (the driver uses `v4l2_dbg(2, debug, …)` [A-33]) |
| Module parameter `packet_type` | Default 0x87 (DRM InfoFrame), written to `TYP_ACP_SET` 0x8706; that packet is captured in the ACP slot (*research gaps*, topics A and B) | — |
| debugfs InfoFrames | AVI, AUDIO, SPD, HDMI vendor-specific and DRM InfoFrames, under the V4L2 debugfs root in a directory named after the sub-device (*research gaps*, topics A and B) | Created at probe [B-15], freed at remove [B-40] |
| `hdmi_phy_auto_reset_*`, `hdmi_detection_delay` | Left 0 on DT platforms (zeroed allocation); effect on lock behaviour with marginal sources unknown (*research gap*, topic B) | DT platform data is hard-coded [B-12] |

These are candidate diagnostics for [TROUBLESHOOTING.md](TROUBLESHOOTING.md) once confirmed.

## 24. Mainline vs Raspberry Pi tree parity

| Comparison | Result | Facts |
|---|---|---|
| `tc358743.c`, `rpi-6.18.y` vs torvalds/linux `master` | Identical except the formatting of the `i2c_device_id` table entry (`{ "tc358743" }` vs `{ .name = "tc358743" }`) | [A-48], [B-02], [C-44] |
| `tc358743_regs.h` | Byte-identical | [A-48], [B-02] |
| DT binding `toshiba,tc358743.txt` | Identical | [B-03] |
| 972 Mbps support | Upstream (part of the identical file) | [C-44] |

Gaps:

- The mainline commit SHA was not recorded: OQ-036 (KERNEL SOURCE INSPECTION REQUIRED).
- The shipped kernel 6.18.50 [G-04] versus the inspected 6.18.55 [E-37]: not compared (section 3; OQ-097, KERNEL SOURCE INSPECTION REQUIRED).
- Buildroot 2026.08 builds Linux 6.12.61 [E-06]. Only its defconfig settings for the driver were checked [E-39], not the driver source. This matters only if ADR-003 (ACCEPTED: Raspberry Pi OS with `rpi-image-gen`) is re-evaluated and its documented Buildroot alternative is chosen: NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED; OQ-064).

Consequence for ADR-002 (PROPOSED): a patch carried by PACSCORDER would apply to the same code in both trees (reasoning from [A-48]).

## 25. Known defects and limitations

From source inspection only; none reproduced on hardware.

| ID | Defect or limitation | Effect | Facts | Tracked as |
|---|---|---|---|---|
| D1 | An unsupported `refclk` rate logs an error but `probe_of` returns 0. If CHIPID then reads, `tc358743_set_ref_clk()` hits `BUG_ON` | A DT `clock-frequency` mistake causes a kernel BUG instead of a clean probe failure | [A-22], [B-11] (both CORRECTED) | RISK-007, RISK-021, OQ-013, OQ-019 |
| D2 | FIFO trigger level hard-coded at 374 (empirical) | Image corruption reported at 1080p50 RGB888 on 3 of 4 lanes | [B-12], [A-43], [C-43] | RISK-006, OQ-035 |
| D3 | Lane formula counts active pixels only; result not clamped to DT lanes or to 4 | >4 lanes mis-programmed (reasoning [B-33]); a request above DT lanes fails only at receiver STREAMON [B-32]; a Raspberry Pi engineer reported that the formula's time per line is wrong, in an issue about corruption at 1080p50 RGB888 on 3 of 4 lanes [C-43] | [A-25], [B-31], [B-32], [B-33], [C-43] | RISK-001, RISK-006, OQ-038 |
| D4 | D-PHY timing constants only for 594 and 972 Mbps | Other link frequencies run on 594 Mbps constants ("untested") | [B-09], [A-24] | OQ-035 |
| D5 | Non-continuous CSI-2 clock unsupported; flags 0 reported while DT requests non-continuous | Transmitter/receiver clock-mode mismatch possible | [B-37] | OQ-037 |
| D6 | No `LINK_FREQ` / `PIXEL_RATE` controls; bridge entity function | RP1 CFE D-PHY set to 999 Mbps (reasoning [B-49]); libcamera does not support the bridge, as reported by Raspberry Pi engineers [C-41] | [B-16], [B-49], [C-31], [C-41] | RISK-011, OQ-050 |
| D7 | Interlaced input rejected | 1080i sources cannot be captured | [B-27] | RISK-009, OQ-002 |
| D8 | HDCP always disabled on DT platforms | HDCP-requiring sources may give no video | [A-04] | RISK-008, OQ-028 |
| D9 | HPD only after an EDID write; `edid_blocks_written` is 0 after every probe. No default EDID and a volatile EDID SRAM: *research gap* (topic B) | No video until userspace writes an EDID after every boot or load | [A-33], [B-21], [A-31]; *research gap* (topic B) | RISK-010, OQ-002, OQ-032, OQ-093 |
| D10 | Polling every 1000 ms with the stock overlay | Up to about 1 s detection latency | [A-30], [B-19] | RISK-013, OQ-020 |
| D11 | Integer fps | 59.94 Hz reported as 60 Hz | [B-28] | OQ-040 |
| D12 | UYVY is BT.601 limited, reported SMPTE170M; also for HD sources per the research (*research gap*, topic B) | Colour metadata must be set downstream | [B-34]; *research gap* (topic B) | OQ-041 |
| D13 | Remove does not drop HPD, assert reset or disable the clock; no runtime PM | Unknown state after unload | [B-40]; *research gap* (topic B) | OQ-039 |
| D14 | No regulator handling | Board power must be independent of the driver | [C-23] | OQ-022, OQ-031 |
| D15 | Driver configures audio only as 2-channel I2S, although the silicon can also send audio over CSI-2 [A-05] | Reasoning: with this driver, audio needs separate I2S wiring and the audio overlay [B-46] | [A-13], [A-05], [B-46] | RISK-014, OQ-004, OQ-025 |
| D16 | `s_stream` does not check signal presence and always returns 0 | STREAMON can succeed with no input | [B-36] | REQ-CAP-004 |
| D17 | IR not supported (held in reset) | — | [A-49] | — |
| D18 | CEC not enabled in Raspberry Pi defconfigs | CEC unavailable without a kernel rebuild (reasoning: `CONFIG_VIDEO_TC358743_CEC` is a build-time bool) | [A-34], [B-20], [E-39] | OQ-016 |
| D19 (added 2026-10-08) | Audio configured once at probe, hard-coded to 2-channel I2S; TDM, CSI and 4/6/8-channel settings never selected | Stereo only; compressed or multichannel HDMI audio behaviour undocumented (research gap, topic I) | [I-24], [I-25] | RISK-014, OQ-110 |
| D20 (added 2026-10-08) | No kernel path carries the HDMI sample rate into ALSA; only the driver's read-only control has it (reasoning) | A rate mismatch makes audio run fast or slow with no ALSA error, and A/V drift accumulates (reasoning) | [I-18], [I-11], [I-20] | RISK-023, OQ-111 |
| D21 (added 2026-10-08) | Audio-rate control updated from the 1000 ms poll with the stock overlay | A source rate change can take up to about 1 s, plus I2C time, to reach userspace | [I-22], [I-21] | RISK-023, OQ-111, OQ-020 |

Possible mitigation for D6 (reasoning only; not evaluated, not proposed): the CFE's graph walk accepts an entity that exposes `V4L2_CID_LINK_FREQ` [C-31], so a driver patch adding that control could give CFE the real rate. Under ADR-002 (PROPOSED) such a patch is written only after the defect is shown on hardware.

## 26. What was actually verified (Rule 7)

**On PACSCORDER hardware: nothing.** No hardware exists as of 2026-10-06. The module has not been built, loaded or probed by this project. No I2C transaction, EDID write, capture or measurement has been made.

**From source inspection (research of 2026-10-06):**

- A research agent read `tc358743.c` and `tc358743_regs.h` in both `rpi-6.18.y` and torvalds/linux `master` [A-48], [B-02]; the DT binding in both trees [B-03]; `include/media/i2c/tc358743.h` [A-42], [B-16]; and the Raspberry Pi overlays and defconfigs in `rpi-6.18.y` [B-41], [B-43], [B-20], [E-39]. A second, independent agent re-fetched the sources and tried to refute every claim ([REFERENCES.md](REFERENCES.md), "How this register was produced").
- Topic A (TC358743 hardware): 50 claims, 47 CONFIRMED, 3 CORRECTED (A-22, A-25, A-26). Topic B (driver and binding): 50 claims, 46 CONFIRMED, 4 CORRECTED (B-11, B-21, B-25, B-44) ([REFERENCES.md](REFERENCES.md) summary).
- "CONFIRMED" means the cited source says so. It does not mean the behaviour has been observed.
- *(Added 2026-10-08.)* Research topic I (HDMI audio path) read the driver's audio setup, audio controls and audio events, the `tc358743-audio` overlay, the ALSA stub codec and both Pi I2S drivers in `rpi-6.18.y`, with the same researcher-plus-independent-verifier method: 47 claims, 46 CONFIRMED, 1 CORRECTED (I-40, not cited here) ([REFERENCES.md](REFERENCES.md) summary). The sources were read at the branch head, while the packaged kernels are 6.18.50 (research gap, topic I; OQ-097 scope note).

**Not done, even at source level:**

- The NDA Toshiba documents REF_01 / REF_02 were not consulted [A-42] (OQ-027).
- The mainline commit SHA was not recorded (OQ-036).
- The shipped kernel's (6.18.50) driver source was not compared with the inspected 6.18.55 (section 3, OQ-097).
- Module parameters, debugfs and the zeroed platform-data fields are not register facts (section 23, OQ-042).
- Receiver behaviour on source unregister was not inspected (section 7).

**How each driver behaviour will be verified** (all BLOCKED — HARDWARE REQUIRED; procedures in [TESTING.md](TESTING.md)):

| Behaviour | Section | Test |
|---|---|---|
| Chip answers on I2C at the expected address | 10 | TEST-HW-001 |
| Probe completes; CHIPID and revision; log lines | 6, 11 | TEST-DRV-001 |
| Reset GPIO sequence (if wired) | 9 | TEST-DRV-001 |
| EDID write, HPD assertion, HPD timing | 13 | TEST-DRV-002 |
| Media entity, pad and links per platform | 16 | TEST-PLT-001 |
| DV timings detection per source mode | 12, 18 | TEST-CAP-001 |
| 1080p60 capture; lane count; clock mode | 14 | TEST-CAP-002 |
| Connect / disconnect / mode change; event delivery; recovery | 19, 21 | TEST-CAP-003 |
| Unsupported-mode rejection; FIFO behaviour near lane limits | 14, 18, 20 | TEST-CAP-004 |
| I2S audio controls and path | 22 | TEST-AUD-001 |
| Audio configuration, sampling-rate decode, control updates and events; rate-change handling (added 2026-10-08) | 11.7, 22.1, 21 | TEST-AUD-001 |

## 27. Open questions referenced

| OQ | Topic | Marker |
|---|---|---|
| OQ-002 | Supported input modes and EDID content | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-003 | Capture pixel format (ADR-005) | OWNER DECISION REQUIRED |
| OQ-004 | Is HDMI audio required? | OWNER DECISION REQUIRED *(superseded: ANSWERED 2026-10-07 — audio required, REQ-CAP-006 DRAFT)* |
| OQ-013 | Driver strategy (ADR-002) | OWNER DECISION REQUIRED |
| OQ-016 | Is HDMI CEC required? | OWNER DECISION REQUIRED; KERNEL SOURCE INSPECTION REQUIRED |
| OQ-019 | REFCLK oscillator frequency | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-020 | INT and RESETN wiring | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED |
| OQ-022 | Use of CAM_GPIO | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-023 | Product power input | OWNER DECISION REQUIRED; DATASHEET REQUIRED; HARDWARE TEST REQUIRED |
| OQ-024 | VDDIO2 voltage, HPD / +5V interface | VENDOR CONFIRMATION REQUIRED; DATASHEET REQUIRED |
| OQ-025 | I2S wiring | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-026 | I2C address, strap, bus sharing | DATASHEET REQUIRED; HARDWARE TEST REQUIRED |
| OQ-027 | Toshiba NDA documentation | VENDOR CONFIRMATION REQUIRED; OWNER DECISION REQUIRED |
| OQ-028 | HDCP | DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED; LEGAL CLARIFICATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-029 | CHIPID revision byte of production silicon | HARDWARE TEST REQUIRED; DATASHEET REQUIRED |
| OQ-030 | Silicon input and lane-rate limits | DATASHEET REQUIRED; HARDWARE TEST REQUIRED |
| OQ-031 | Power-up sequencing, reset timing, I2C speed | DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED |
| OQ-032 | HPD state before the first EDID | DATASHEET REQUIRED; HARDWARE TEST REQUIRED |
| OQ-033 | Silicon capabilities the driver does not use | DATASHEET REQUIRED; KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-035 | FIFO level and D-PHY constants per mode | HARDWARE TEST REQUIRED; DATASHEET REQUIRED |
| OQ-036 | Mainline commit used for the comparison | KERNEL SOURCE INSPECTION REQUIRED |
| OQ-037 | Effective CSI-2 clock mode | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-038 | 3 active of 4 configured lanes | HARDWARE TEST REQUIRED |
| OQ-039 | Behaviour after unload; reload as recovery | HARDWARE TEST REQUIRED |
| OQ-040 | Fractional frame rates | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-041 | Colourimetry and quantisation range | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED |
| OQ-042 | Module parameters, debugfs, zeroed platform data | KERNEL SOURCE INSPECTION REQUIRED; DATASHEET REQUIRED |
| OQ-043 | Runtime nodes and entity names | HARDWARE TEST REQUIRED |
| OQ-045 | RGB888 byte order on Pi 4/CM4 | HARDWARE TEST REQUIRED |
| OQ-050 | CFE 999 Mbps D-PHY setting | HARDWARE TEST REQUIRED |
| OQ-051 | Source-change events on Pi 5/CM5 | HARDWARE TEST REQUIRED |
| OQ-052 | CM5 carrier connector and I2C mapping | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED |
| OQ-053 | Whether CFE capture buffers use CMA on Pi 5/CM5 | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-054 | `tc358743-audio` overlay on Pi 5/CM5 (added 2026-10-08) | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-064 | Buildroot baseline (kernel 6.12.61) | OWNER DECISION REQUIRED; BUILD TEST REQUIRED; HARDWARE TEST REQUIRED |
| OQ-093 | EDID provisioning trigger and ordering | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-097 | `tc358743.c` in the shipped 6.18.50 versus the inspected 6.18.55 | KERNEL SOURCE INSPECTION REQUIRED |
| OQ-099 | CSI-2 link frequency (ADR-008) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-101 | Command syntax used in procedures but not in the source register | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED |
| OQ-110 | TC358743 I2S output for compressed, multichannel and 24-bit HDMI audio (added 2026-10-08) | DATASHEET REQUIRED; HARDWARE TEST REQUIRED |
| OQ-111 | HDMI audio sample-rate detection, rate changes and output sample rate (added 2026-10-08) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED; BUILD TEST REQUIRED |
| OQ-112 | A/V synchronisation across the I2S audio and CSI-2 video clock domains (added 2026-10-08) | HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED |

## Verification status

### Verified from sources (fact IDs)

This document cites the following register entries, all with verdict `CONFIRMED` or `CORRECTED`:

| Topic | Fact IDs |
|---|---|
| A — TC358743 hardware | A-02, A-04, A-05, A-07, A-08, A-09, A-10, A-12, A-13, A-14, A-15, A-16, A-17, A-18, A-19, A-20, A-21, A-22, A-23, A-24, A-25, A-27, A-28, A-29, A-30, A-31, A-32, A-33, A-34, A-35, A-36, A-37, A-38, A-39, A-42, A-43, A-44, A-45, A-46, A-48, A-49, A-50 |
| B — tc358743 Linux driver | B-01, B-02, B-03, B-05, B-06, B-07, B-08, B-09, B-10, B-11, B-12, B-13, B-14, B-15, B-16, B-17, B-18, B-19, B-20, B-21, B-22, B-23, B-24, B-26, B-27, B-28, B-29, B-30, B-31, B-32, B-33, B-34, B-35, B-36, B-37, B-38, B-39, B-40, B-41, B-43, B-44, B-45, B-46, B-47, B-49, B-50 |
| C — Raspberry Pi CSI-2 receive path | C-06, C-09, C-11 (added 2026-10-08), C-12, C-15, C-16, C-18, C-19, C-20, C-21, C-22, C-23, C-24, C-25, C-26, C-27, C-28, C-29, C-31, C-32, C-33, C-34, C-35, C-36, C-41, C-42, C-43, C-44, C-46, C-47, C-48, C-51, C-52, C-53 |
| E — Buildroot and kernel configuration | E-06, E-37, E-38, E-39 |
| G — Raspberry Pi OS | G-04, G-16 |
| I — HDMI audio path (added 2026-10-08) | I-02, I-11, I-12, I-13, I-16, I-18, I-19, I-20, I-21, I-22, I-23, I-24, I-25, I-26 |

- `CORRECTED` entries, used in their corrected wording only: A-22, A-25, B-11, B-21, B-44, C-28, C-36, C-53.
- `community` entries, worded as reports: A-43, C-28, C-33, C-35, C-41, C-42, C-43; added 2026-10-08: I-16.
- `reasoning` entries, labelled as reasoning: A-23, B-10, B-11, B-17, B-27, B-33, B-47, B-49, C-46, C-47, C-48, C-51, C-52, C-53; added 2026-10-08: I-18.
- Statements marked *research gap* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json) and are not register facts. Those marked *research gap*, *research open question* or *research design risk* for topic I (added 2026-10-08) come from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json), with the same status.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-08). The owner decisions of 2026-10-07 (audio required; CM4 and CM5 side by side) are requirements and plans, not hardware evidence.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against the source register. Changes: probe sub-steps 3a–3h re-ordered by the quoted source lines (clock lookup, endpoint, clock enable) and "failed to get refclk" log line added; post-probe HPD statement limited to what the driver does (silicon HPD state → OQ-032); CHIPID revision byte linked to OQ-029; "never back to sleep" downgraded to reasoning + KERNEL SOURCE INSPECTION REQUIRED; camera power-enable lines described per base DT; `ddc5v_delay` meaning marked NEEDS VERIFICATION; FIFO-level attribution of [A-43] corrected; call order of initial-setup steps 7–10 marked NEEDS VERIFICATION; RGB888 difference described as a fourcc-label difference [C-34], [C-35]; libcamera statement reworded as a report [C-41]; audio statement cited [A-13]; open-question markers aligned with [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) and OQ-029, OQ-053 added; UNKNOWN markers normalised to "UNKNOWN — VERIFICATION REQUIRED"; architecture paragraph cited; source-inspection scope in section 26 stated per tree. No hardware result added: nothing has been tested. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: EDID persistence no longer cited to [B-22] (now [A-31], [B-21], [A-33] plus *research gap*, topic B) in sections 13 and 25 (D9); OQ-097 linked for the 6.18.50 vs 6.18.55 driver-source question (sections 3, 24, 26); OQ-093 linked for the EDID write trigger (sections 13, 21, 25); `--clear-edid` argument form and sub-device `-d` form pointed to OQ-101; "UYVY SMPTE170M even for HD sources" attributed to the research gap, not [B-34] (section 17, D12); I2S audio stated as the driver's choice with [A-05] (section 22, D15); 3-of-4-lane note and link-frequency proposal linked to ADR-008 / OQ-099 (section 14); section 27 table extended with OQ-093, OQ-097, OQ-099, OQ-101. No status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); section 24 gap: "only if the Buildroot alternative in ADR-003 is chosen" → "only if ADR-003 (ACCEPTED: Raspberry Pi OS with `rpi-image-gen`) is re-evaluated and its documented Buildroot alternative is chosen" (OQ-064 unchanged). No evidence, other ADR status (ADR-002, ADR-005, ADR-006 PROPOSED) or implementation status changed. | Claude (session 2026-10-07) |
| 2026-10-08 | Owner decisions of 2026-10-07 (second set: audio required, CM4 and CM5 side by side) and research topic I propagated. Header, conventions (2026-10-08 research JSON; [I-24] line number from the branch head, OQ-097) and §1 (TEST-AUD-001 row; REQ-CAP-006 served). §2: pointers to the new sections. §4: CEC not set in packaged kernels [I-22]; no audio Kconfig in the driver [I-02], [I-12]. §11.4 step 9 points to the new §11.7; §11.6 adds the audio registers [I-19], [I-24]. New §11.7 `tc358743_set_hdmi_audio()`: called once at probe, registers written [I-24], datasheet I2S/TDM limits [I-25], audio PLL [I-26] (OQ-112, RISK-024), stereo-only consequence (reasoning; research design risk), and unknowns (bit length, NLPCM and auto-mute meaning, compressed/multichannel, rate-change behaviour: research gaps; OQ-110, OQ-111, OQ-027, OQ-033). §12: audio row (CBIT interrupts) [I-21]. §15: `V4L2_EVENT_CTRL` for audio-rate changes [I-21]. §19: audio interrupt sources [I-21] and audio-rate polling latency [I-22]. §21: PROPOSED recovery rows for audio rate change and audio lock [I-18], [I-19], [I-21], [I-22]; the +5V-loss `V4L2_EVENT_CTRL` NEEDS VERIFICATION note marked partly addressed by [I-21]. §22: OQ-004 noted as ANSWERED; new §22.1 with control IDs [I-20], the `code_to_rate[]` decode and no-signal rule [I-19], updates and events [I-21], [I-22], the per-platform node that carries the controls [I-23] (Pi 4 Model B / Pi 5 and CM4 Media Controller rows labelled reasoning), the missing ALSA rate path (reasoning [I-18]; RISK-023, OQ-111), a community report [I-16] and a PROPOSED userspace use. §25: defects D19–D21. §26: topic I source-inspection scope and counts; test-map row. §27: OQ-004 marked ANSWERED; OQ-054, OQ-110, OQ-111, OQ-112 added. Verification status: topic I IDs, community I-16, reasoning I-18. No REQ or ADR status changed; driver strategy ADR-002 still PROPOSED; nothing tested. | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic I additions: §4 audio-Kconfig bullet split into the sourced fact ([I-02]: no ASoC codec driver of its own; generic stub codec) and a labelled inference ("Reasoning from [I-02]: no extra Kconfig symbol for audio"). All other [I-xx] citations checked against the register; no change needed. No status changed. | Claude (session 2026-10-08) |
