# PACSCORDER Troubleshooting

| | |
|---|---|
| Document status | Active — 29 failure signatures collected from sources; **none observed on PACSCORDER** |
| Last updated | 2026-10-07 |
| Applies to | TC358743 bridge and its board; the in-tree `tc358743` driver; Unicam (Pi 4 Model B, CM4); RP1 CFE (Pi 5, CM5); the Pi 4/CM4 `bcm2835-codec` encoder; GStreamer/FFmpeg integration; hardware handling |
| Verification | Every signature comes from the source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)). No signature has been observed on PACSCORDER hardware, because none exists as of 2026-10-06. Diagnosis steps and remedies are source-derived and **NOT YET RUN ON PACSCORDER HARDWARE**. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 10 (status words), Rule 21 (record failures honestly), Rule 22 (unknowns), Rule 23 (source priority) |

This document lists the failures that sources say can happen on the PACSCORDER video path:

```text
HDMI → TC358743 → CSI-2 → receiver → Media Controller → V4L2 → DMABUF → encoder → recorder / RTMP / WebRTC
```

For each failure it gives the exact log text where a source attests it, the likely cause, how to diagnose it and the source-derived remedy. Test procedures are in [TESTING.md](TESTING.md); the device-tree and driver background is in [DEVICE_TREE.md](DEVICE_TREE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md), [CSI_PIPELINE.md](CSI_PIPELINE.md) and [V4L2.md](V4L2.md).

> **State on 2026-10-06:**
> - No PACSCORDER hardware or code exists.
> - None of the signatures below has been seen on PACSCORDER.
> - No remedy below has been tried.
>
> A remedy is a starting point to test, not a known cure.

## How to use this document

1. Find the symptom in the index below and go to its entry. Each entry gives, in order: **Symptom**, **Likely cause**, **Evidence** (fact IDs), **Diagnosis steps**, **Remedy** and **Related** (RISK, OQ and TEST IDs).
2. When a signature is seen on PACSCORDER hardware:
   - record the run in the [TESTING.md](TESTING.md) result log and in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md);
   - add a dated "Observed on PACSCORDER" note under the entry, with platform, hardware revision, software version and the exact log text.
   Never rewrite the original entry (Rule 21). A hardware observation overrides the sources (Rule 23).
3. **Log text.** Log text in `code` is the string in the driver source. Format specifiers such as `%u` and `%x` are filled in at runtime.
4. **Placeholders.** `/dev/v4l-subdevN`, `media-ctl -d <N>`, `/dev/videoN` and entity names are placeholders. On PACSCORDER they are UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED, OQ-043).
5. **Commands.**
   - Commands come only from cited sources.
   - Where only the action is attested, the step says NEEDS VERIFICATION. These steps are tracked together in OQ-101.
   - `v4l2-ctl -d /dev/v4l-subdevN`, that is, a sub-device path given to `-d`, is NEEDS VERIFICATION (OQ-101). The register attests `-d 11` [D-17] and running the EDID and timing steps on `/dev/v4l-subdevN` [C-33], but not that option form. The `--set-edid pad=<pad>[,…]` form is attested as `v4l2-ctl` help text [B-24].
   - `config.txt` overlay parameters given by name alone (for example `4lane`, `media-controller`) or combined on one line are NEEDS VERIFICATION (OQ-100). Only `dtoverlay=tc358743,<param>=<val>` [G-12] and appending `,cam0` [C-39] are attested.
   - Reading the kernel log is written as an action. No register fact attests a specific command for it.
6. **Source tiers.** Facts from community sources are worded "reported by …". Reasoning facts are labelled as reasoning.

## Symptom index

| # | Symptom | Layer | Platforms |
|---|---|---|---|
| 1.1 | `not a TC358743 on address 0x1e` | I2C / probe | all |
| 1.2 | `unsupported refclk rate` followed by a kernel BUG | probe / DT | all |
| 1.3 | `untested bps per lane` | probe / DT | all |
| 1.4 | `missing endpoint node`, `missing CSI-2 properties in endpoint`, `invalid number of lanes` | probe / DT | all |
| 1.5 | Nothing on the camera I2C bus of a Compute Module IO board (J6 jumpers) | hardware / I2C | CM4, CM5 |
| 1.6 | Bridge not on the expected Pi 5 / CM5 I2C bus number | I2C | Pi 5, CM5 |
| 2.1 | Source shows no signal / sees no display: no EDID, so no hot-plug | HDMI / EDID | all |
| 2.2 | `query_dv_timings` fails with `-ENOLINK`, `-ENOLCK` or `-ERANGE` | DV timings | all |
| 2.3 | Interlaced source (for example 1080i) rejected | DV timings | all |
| 2.4 | HDCP-protected source gives no video | HDMI | all |
| 2.5 | EDID or DV-timings ioctl rejected on the node used | V4L2 control model | Pi 4, CM4 (mode-dependent); Pi 5, CM5 |
| 3.1 | `Device has requested N data lanes, which is >M configured in DT` | CSI-2 receiver | all |
| 3.2 | `subdevice requires N data lanes when M are supported` | CSI-2 receiver | Pi 4, CM4 |
| 3.3 | `Unable to determine sensor link rate, using 999 Mbps` | CSI-2 receiver | Pi 5, CM5 |
| 3.4 | STREAMON fails with `-EPIPE` (-32) when `field:none` is omitted | Media Controller | Pi 5, CM5 |
| 3.5 | Corrupted image near lane-count boundaries | CSI-2 / bridge FIFO | all (reported on CM4) |
| 4.1 | Colour-swapped image with RGB888 | pixel format | all |
| 4.2 | `Incorrect pixel format` warning from the CFE | pixel format | Pi 5, CM5 |
| 4.3 | Wrong colours or levels after encoding (UYVY colourimetry) | pixel format / encoder | all |
| 5.1 | No `V4L2_EVENT_SOURCE_CHANGE` on `/dev/video0` | events | Pi 5, CM5 |
| 5.2 | Source changes detected late (up to about 1 s), or capture does not resume | events | all |
| 6.1 | DMABUF import fails: `contiguous chunk is too small` | DMABUF / encoder | Pi 4, CM4 |
| 6.2 | GStreamer element `v4l2h264enc` missing | userspace | all |
| 6.3 | No `/dev/video11` | encoder | Pi 5, CM5 |
| 6.4 | Encoder disappears with the cut-down firmware | firmware | Pi 4, CM4 |
| 6.5 | `flvmux` will not link to the hardware encoder output | userspace / RTMP | Pi 4, CM4 |
| 6.6 | Buffer allocation fails at stream start (CMA exhausted) | memory | all |
| 7.1 | Damage from a wrongly sided FFC adapter | hardware | Pi 5 (reported); CM4 IO Board, CM5 IO Board (22-pin, reasoning) |
| 7.2 | Bridge board unpowered or held in reset because CAM_GPIO stays low | hardware / DT | all |

---

## 1. I2C, power and probe

### 1.1 `not a TC358743 on address 0x1e`

**Symptom.**
- The kernel log shows `not a TC358743 on address 0x1e` from `tc358743`.
- Probe returns `-ENODEV`, and no TC358743 sub-device or media entity appears.
- The driver prints the I2C address in 8-bit form, so 7-bit 0x0f appears as 0x1e [A-16].

**Likely cause.**
- The driver logs this message when the CHIPID read (register 0x0000) fails, or when the chip-ID byte is not 0x00 [A-19], [B-14]. In practice that means one of:
  - Nothing answers at 0x0f on that bus. The bridge may be unpowered (see 7.2), on the wrong connector or bus (overlay `cam0` parameter [B-42], [C-39]; see 1.5 and 1.6), or connected through a faulty or wrongly oriented cable (OQ-021).
  - The bridge answers at another address. The public TC358743XBG datasheet states no address [A-15]. The sister part TC358749XBG selects 0x0F or 0x1F with the INT pin level at reset [A-50]. Whether the TC358743XBG behaves the same is UNKNOWN (OQ-026).
  - The chip is held in reset by a circuit the driver does not control. RESETN wiring is UNKNOWN (OQ-020).
  - A different device answers at 0x0f with a non-zero chip-ID byte [A-19].
- A related failure: if the I2C adapter lacks SMBus byte-data support, probe returns `-EIO` before the CHIPID read [A-20].

**Evidence.** [A-15], [A-16], [A-19], [A-20], [A-50], [B-14]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Run the TEST-HW-001 bus scan (`i2cdetect -y <bus>`; `i2c-tools` [G-25]) on the bus for the connector in use. The `-y` form comes from the owner's Rule 9 example, not a register fact: NEEDS VERIFICATION (OQ-101). The bus per platform is in the [TESTING.md](TESTING.md) platform reference [C-24], [C-25], [C-26], [C-27].
2. If a device answers at 0x1f instead of 0x0f, record it (OQ-026).
3. Check the `config.txt` overlay parameters against the physical connector (`cam0` for connector 0) [C-39], [B-42].
4. Measure the bridge-board supply rails and the CAM_GPIO pin (HARDWARE TEST REQUIRED; OQ-022).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Correct the connector, cable or jumpers.
- If the bridge sits at 0x1f, the DT `reg` value must change. Design the change in [DEVICE_TREE.md](DEVICE_TREE.md).
- If the board needs CAM_GPIO, see 7.2.

**Related.** RISK-021 · OQ-018, OQ-020, OQ-022, OQ-026, OQ-043, OQ-101 · TEST-HW-001, TEST-DRV-001

### 1.2 `unsupported refclk rate` followed by a kernel BUG

**Symptom.**
- During probe the kernel log shows `unsupported refclk rate: <N> Hz`, followed by a kernel BUG instead of a clean probe failure.
- A related message, `failed to get refclk`, is logged when the driver's `devm_clk_get` for the clock named `refclk` fails [A-22]. Reasoning: a DT node without a `refclk` clock is one way to get it; the binding requires `clocks`/`clock-names = "refclk"` [A-44].

**Likely cause** (CORRECTED facts [A-22], [B-11]).
- The DT declares a reference-clock frequency other than 26, 27 or 42 MHz.
- For any other rate the driver logs the message, but its error path leaves the return value at 0, so probe continues [A-22].
- If the chip then answers the CHIPID read, `tc358743_initial_setup()` calls `tc358743_set_ref_clk()`, which hits `BUG_ON()` [A-22].
- Reasoning from the source: the CHIPID read is likely to succeed on a Raspberry Pi, because `refclk` there is a `fixed-clock` node whose disable does not stop the physical oscillator [B-11].

**Evidence.** [A-21], [A-22], [A-44], [A-45], [B-07], [B-11], [B-47]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Compare the DT clock frequency with the oscillator on the bridge board.
   - The stock overlay declares 27 MHz [A-45].
   - Reasoning: the Pi does not generate REFCLK; the board must supply its own oscillator at the declared frequency [B-47].
   - The board's oscillator is UNKNOWN — VENDOR CONFIRMATION REQUIRED (OQ-019).
2. Measure the oscillator if the schematic is not available (HARDWARE TEST REQUIRED).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Set the DT clock frequency to the board's real oscillator, which must be 26, 27 or 42 MHz [A-21], [B-07].
- Reasoning: 27 MHz is preferred, because only 27 MHz gives the exact 594/972 Mbps lane rates of the driver's timing tables [A-23], [B-10].
- Making the driver fail cleanly instead of hitting `BUG_ON()` would be a kernel patch under ADR-002 (PROPOSED; OQ-013).

**Related.** RISK-007, RISK-021 · OQ-013, OQ-019 · TEST-DRV-001

### 1.3 `untested bps per lane`

**Symptom.** The kernel log shows `untested bps per lane` at probe.

**Likely cause.**
- The driver has D-PHY timing constants only for 594 Mbps and 972 Mbps per lane (link frequency 297 MHz or 486 MHz). For any other rate it logs this message and falls back to the 594 Mbps constants [A-24], [B-09], [C-14].
- The overlays README says only `link-frequency` 297000000 and 486000000 (default) are supported. It labels 297000000 as "574Mbit/s", which contradicts the driver's 2 × 297 MHz = 594 Mbit/s and looks like a typo [A-46], [C-13].
- Reasoning: with a 26 MHz or 42 MHz reference clock, the PLL-derived lane rate differs from the nominal rate (for example 962 or 966 Mbps instead of 972) [A-23], [B-10]. Whether that alone triggers the message is KERNEL SOURCE INSPECTION REQUIRED.

**Evidence.** [A-23], [A-24], [A-46], [B-09], [B-10], [C-13], [C-14]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check the `link-frequency` overlay parameter and the DT `link-frequencies` value [G-12].
2. Check the reference clock (1.2).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Use the default 486000000, or 297000000 [G-12]. ADR-008 (PROPOSED; OQ-099) proposes keeping the default on every platform, and evaluating 297000000 only on CM4 CAM1 with 4 lanes.
- On Pi 5 / CM5, keep the default; see 3.3 (reasoning) [C-52].

**Related.** RISK-006, RISK-011 · OQ-019, OQ-035, OQ-099 · TEST-DRV-001

### 1.4 `missing endpoint node`, `missing CSI-2 properties in endpoint` or `invalid number of lanes`

**Symptom.** Probe fails with `-EINVAL` and one of these messages [B-06].

**Likely cause.**
- Although the binding marks them optional, the driver requires an endpoint on port 0 that:
  - parses as a CSI-2 D-PHY bus;
  - has 1–4 data lanes;
  - has at least one `link-frequencies` entry [B-06].
- This happens with a custom or edited overlay. The stock overlay sets all of these [B-41].

**Evidence.** [A-44], [B-06], [B-41]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Compare the overlay's endpoint with the binding properties [A-44] and with the stock `tc358743.dtsi` [B-41].

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Restore `data-lanes`, `clock-lanes` and `link-frequencies` in the endpoint ([DEVICE_TREE.md](DEVICE_TREE.md)).

**Related.** OQ-021 · TEST-DRV-001

### 1.5 Nothing on the camera I2C bus of a Compute Module IO board (J6 jumpers)

**Symptom.**
- TEST-HW-001 finds no device at 0x0f, and the driver logs 1.1.
- This happens on the CM4 IO Board CAM0 connector or the CM5 IO Board CAM/DISP 1 connector.

**Likely cause.** On those connectors I2C reaches the camera only when both J6 jumpers are fitted [C-03], [C-06].

**Evidence.** [C-03], [C-06].
- Conflict: a comment in `bcm2712-rpi-cm5io.dtsi` says the CAM/DISP 1 connector needs no jumper. That comment is a research gap (topic C), not a register fact; which statement is right is OQ-052.

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Inspect J6, then repeat TEST-HW-001.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Fit both J6 jumpers, as the official documentation says [C-03], [C-06].
- With CM5 on a CM4 IO Board, note that the CM4 CAM0 pins carry USB 3.0 on CM5 [C-05]. Which overlay parameters pair the CM4 IO Board CAM1 connector with its I2C bus is UNKNOWN (OQ-052).

**Related.** RISK-021 · OQ-052 · TEST-HW-001

### 1.6 Bridge not on the expected Pi 5 / CM5 I2C bus number

**Symptom.** The bridge is not where an instruction expects it:
- an older instruction or forum post names `i2c-4` / `i2c-6`, or an entity `tc358743 4-000f`, but the bridge appears as `tc358743 11-000f` or `tc358743 10-000f`;
- or a bus scan on the bus number from such an instruction is empty.

**Likely cause.** Pi 5 camera I2C bus numbers changed between kernels (CORRECTED) [C-28]:
- A Raspberry Pi engineer wrote in November 2023 that the CAM/DISP buses were `i2c-4` and `i2c-6`, and later posts in that thread showed `tc358743 4-000f`.
- On 6.18 kernels the buses are numbered through the aliases `i2c10` (CAM/DISP0) and `i2c11` (CAM/DISP1), so the bridge appears as `tc358743 10-000f` or `tc358743 11-000f` [C-28], [C-26].
- On CM5 the mapping depends on the carrier board [C-27].

**Evidence.** [C-26], [C-27], [C-28]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Use the platform reference in [TESTING.md](TESTING.md). Read the entity name from the media graph (TEST-PLT-001; the print command is NEEDS VERIFICATION, OQ-101).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Reasoning from [C-28]: do not hard-code bus numbers or entity names in procedures or code. Resolve them at runtime from the media graph ([V4L2.md](V4L2.md)).

**Related.** OQ-043, OQ-052 · TEST-HW-001, TEST-PLT-001

---

## 2. HDMI input, EDID and DV timings

### 2.1 Source shows no signal or sees no display: no EDID, so no hot-plug

**Symptom.**
- The HDMI source behaves as if no display is connected.
- PACSCORDER receives no video.
- `query_dv_timings` returns `-ENOLINK` [B-29].

**Likely cause.**
- The driver does not enable hot-plug (HPD) until userspace has written an EDID. Its code path is labelled `no EDID -> no hotplug` [A-33], [B-21]. Whether that string reaches the kernel log at the default log level is NEEDS VERIFICATION.
- The EDID is held in on-chip SRAM [A-31], and after probe the driver has no EDID stored (`edid_blocks_written == 0`) [B-21]. Reasoning: the EDID is therefore expected to be absent after every boot or driver reload. Research also found no DT property and no default EDID in the driver (research gap, topic B — not a register fact).
- HPD is also not raised without +5V from the source [B-21], [B-22]. When +5V disappears, the driver drops HPD and clears the stored timings [B-23].
- How the board wires HPD and +5V sensing is UNKNOWN (OQ-024).

**Evidence.** [A-31], [A-33], [B-16], [B-21], [B-22], [B-23], [B-29]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Read back the EDID. The driver returns `-ENODATA` when none is stored [B-22]. Command NEEDS VERIFICATION (OQ-101).
2. Read `V4L2_CID_DV_RX_POWER_PRESENT` [B-16]; the driver updates its controls on a +5V interrupt [B-23], so (reasoning) it is expected to reflect source +5V. Command NEEDS VERIFICATION (OQ-101).
3. Measure the HPD line on the board (HARDWARE TEST REQUIRED; OQ-024).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Write the EDID at every boot and driver load (REQ-CAP-003). What triggers the write, and in what order, is OQ-093.
  ```bash
  v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,file=<pacscorder-edid>   # [B-24]; EDID via VIDIOC_S_EDID [C-37]
  ```
  The `-d /dev/v4l-subdevN` form is NEEDS VERIFICATION (OQ-101; see "How to use this document").
- With +5V present, HPD rises after HZ/7 jiffies: about 140 ms at the Raspberry Pi default HZ=250 (CORRECTED) [B-21].
- In Pi 4/CM4 legacy mode the ioctl goes to `/dev/videoN` instead (see 2.5).
- The EDID content is OWNER DECISION REQUIRED (OQ-002).

**Related.** RISK-010 · OQ-002, OQ-024, OQ-032, OQ-093, OQ-101 · TEST-DRV-002

### 2.2 `query_dv_timings` fails with `-ENOLINK`, `-ENOLCK` or `-ERANGE`

**Symptom.** `v4l2-ctl -d /dev/v4l-subdevN --set-dv-bt-timings query` [C-33], [C-37] fails with one of these codes. (The `-d /dev/v4l-subdevN` form is NEEDS VERIFICATION, OQ-101.)

**Likely cause.** Error codes of the driver's `query_dv_timings` [B-29]:

| Code | Driver meaning [B-29] | Next step |
|---|---|---|
| `-EINVAL` | pad is not 0 | Use pad 0. |
| `-ENOLINK` | HPD is low, or there is no TMDS signal | See 2.1. Check the cable and that the source output is enabled. |
| `-ENOLCK` | no stable sync | The source may be switching modes. Retry after a source-change event (5.2). Without an interrupt, detection polls every 1000 ms [A-30]. |
| `-ERANGE` | detected timings outside the capability | The capability is 640–1920 × 350–1200, pixel clock 13–165 MHz, progressive only [A-08], [B-26]. For interlaced sources see 2.3. |

**Evidence.** [A-08], [A-30], [B-26], [B-29]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Record the code, the source's output mode and the cable state. Repeat with a known-good progressive source at 1080p30 or below.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Per row above. For out-of-range modes, restrict the EDID to supported modes (OQ-002) and configure the source accordingly.

**Related.** RISK-009, RISK-010 · OQ-002, OQ-030 · TEST-CAP-001, TEST-CAP-004

### 2.3 Interlaced source (for example 1080i) rejected

**Symptom.** With an interlaced source, `query_dv_timings` and `s_dv_timings` return `-ERANGE`.

**Likely cause.**
- Reasoning from kernel source: the driver detects interlace, but its timings capability lacks `V4L2_DV_BT_CAP_INTERLACED`, so the timings are rejected [B-27].
- The capability is progressive-only [A-08].

**Evidence.** [A-08], [B-27]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Confirm the source's output mode on the source itself.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Set the source to a progressive mode.
- Do not advertise interlaced modes in the PACSCORDER EDID (OQ-002).
- The current driver cannot capture interlaced input (RISK-009). The ATEM Mini Pro HDMI output is 1080p only, so ATEM Mini Pro capture is not affected [F-23]. Other ATEM models, and the cameras connected directly that are the other required source type (REQ-CAP-008, owner 2026-10-07): UNKNOWN — VERIFICATION REQUIRED per model (OQ-102).

**Related.** RISK-009 · OQ-002 · TEST-CAP-004

### 2.4 HDCP-protected source gives no video

**Symptom.** A source that enforces HDCP produces no usable video, even though an EDID is loaded and HPD is up. The exact symptom (error code, log text, black frames) is UNKNOWN (OQ-028).

**Likely cause.**
- The driver always configures HDCP as disabled on device-tree platforms (manual authentication) [A-04].
- The public datasheet lists HDCP only as "optional" and gives no version [A-03].
- HDCP registers are write-protected through the driver's register-debug interface [B-38].

**Evidence.** [A-03], [A-04], [B-38]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Repeat with a source known not to use HDCP, or with HDCP switched off in the source's settings (HARDWARE TEST REQUIRED).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Disable HDCP on the source where it allows this. Otherwise the source cannot be captured with this driver configuration. Whether that is acceptable is a product decision (RISK-008).

**Related.** RISK-008 · OQ-028, OQ-083 · TEST-CAP-001, TEST-ATEM-001

### 2.5 EDID or DV-timings ioctl rejected on the node used

**Symptom.** `--set-edid` or `--set-dv-bt-timings query` fails on the device node it is issued to.

**Likely cause.** The node does not match the receiver's control model (both CORRECTED) [B-25], [C-36]:

| Model | Where it applies | Video node | Sub-device node |
|---|---|---|---|
| Legacy (video-node) mode | Pi 4/CM4 default with the stock overlay [B-43], [C-10] | Forwards EDID and DV-timings ioctls to the bridge | Registered read-only |
| Media Controller mode | Pi 4/CM4 with `media-controller`; always on Pi 5/CM5 [C-11] | Has no EDID or DV-timings ioctls | Read-write; issue the ioctls here |

Research also found that on a read-only sub-device node `S_DV_TIMINGS` returns `-EPERM`, while `S_EDID` is not blocked. That is a research gap (topic B), not a register fact: KERNEL SOURCE INSPECTION REQUIRED.

**Evidence.** [B-25], [B-43], [C-10], [C-11], [C-36]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check whether `config.txt` sets `media-controller` on Pi 4/CM4 [B-43]. The bare-parameter form `dtoverlay=tc358743,media-controller` is NEEDS VERIFICATION (OQ-100).
2. List which nodes exist (TEST-PLT-001).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Issue the ioctls on the node that matches the mode. ADR-006 (PROPOSED; OQ-014) proposes Media Controller mode on every platform; if it is accepted, the sub-device node is used everywhere.

**Related.** OQ-014, OQ-044, OQ-100 · TEST-DRV-002, TEST-PLT-001

---

## 3. CSI-2 link and receiver

### 3.1 `Device has requested N data lanes, which is >M configured in DT`

**Symptom.**
- STREAMON fails with `-EINVAL`, and Unicam (Pi 4/CM4) or the RP1 CFE (Pi 5/CM5) logs this message [B-32], [C-16].
- Reported example from a CM4 issue: `unicam fe801000.csi: Device has requested 5 data lanes, which is >4 configured in DT` [C-43].

**Likely cause.**
- The TC358743 driver computes its lane count from active pixels × frame rate × bits per pixel ÷ lane rate. It reports the result to the receiver without clamping it to the DT `data-lanes` (CORRECTED) [A-25], [B-31].
- The receiver then rejects it at stream start [B-32], [C-16].
- Typical cases (reasoning) [C-47]:
  - 1080p60 on a 2-lane configuration: 3 lanes needed in UYVY, 4 in RGB888, at 972 Mbps;
  - `link-frequency=297000000`, where 1080p50 RGB888 needs 5 lanes [C-47]; an issue reporter saw exactly this rejection on CM4 [C-43].

**Evidence.** [A-25], [B-31], [B-32], [C-16], [C-43], [C-47]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Look up the lanes needed for the detected mode and format in the TEST-CAP-004 matrix ([TESTING.md](TESTING.md)) [C-47].
2. Compare with the DT lane count (was `4lane` set?) and with the lanes physically routed on the board (UNKNOWN, OQ-021).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- On the 2-lane configuration of REQ-CAP-007 (owner, 2026-10-07), 1080p60 is beyond the link: 2 lanes carry at most 1080p30 RGB888 or 1080p50 YUV422 [C-37]. There the remedy is the EDID restriction below, not a different port.
- On the 4-lane configuration: use a 4-lane path with the `4lane` overlay parameter (bare-parameter syntax NEEDS VERIFICATION, OQ-100). 4-lane paths exist on CM4 CAM1, Pi 5 and CM5 [C-02], [C-04], [C-05], provided the board routes 4 lanes.
  - A 4-lane path is necessary but not shown to be sufficient for 1080p60 UYVY. At 972 Mbps the driver uses 3 of the 4 lanes (reasoning) [C-47], and capture with 3 active lanes is unproven (OQ-038; ADR-008).
- Use UYVY (ADR-005, PROPOSED).
- Keep `link-frequency` at its default, as ADR-008 (PROPOSED; OQ-099) proposes.
- Restrict the EDID to modes the wired lanes carry (REQ-CAP-003, OQ-002).
- Have the application check the lane budget before STREAMON (REQ-CAP-005).

**Related.** RISK-001, RISK-006 · OQ-001, OQ-002, OQ-021, OQ-038, OQ-099 · TEST-CAP-002, TEST-CAP-004

### 3.2 `subdevice requires N data lanes when M are supported`

**Symptom.** On Pi 4/CM4, Unicam logs this message, and probe continues.

**Likely cause.**
- The DT endpoint lists more data lanes than the port supports. Typically, the `4lane` overlay parameter has been used on the Pi 4 Model B port or the CM4 CAM0 port, both of which are 2-lane [B-48], [C-08].
- Unicam then adopts the endpoint count instead of failing [C-17].

**Evidence.** [A-46], [B-45], [B-48], [C-08], [C-17]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Check `config.txt` for `4lane` and compare it with the connector in use.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Remove `4lane` on 2-lane ports. The overlays README says `4lane` is only applicable to the Compute Module CAM1 connector [A-46], [B-45].
- Reasoning: if left in place, the bridge may be told it can use lanes that are not wired. The resulting capture behaviour on hardware is UNKNOWN (HARDWARE TEST REQUIRED).

**Related.** RISK-001 · OQ-021 · TEST-PLT-001

### 3.3 `Unable to determine sensor link rate, using 999 Mbps`

**Symptom.** On Pi 5/CM5, the RP1 CFE logs this message when it configures its D-PHY.

**Likely cause.**
- The CFE looks for a camera-sensor entity, or a `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE` control, to set its D-PHY rate. If none is found, it uses 999 Mbps [C-31].
- The TC358743 is a bridge entity with neither control [C-19], [B-16]. So the fallback is always taken for this bridge (kernel source [C-31]; reasoning [B-49]).

**This message is expected with TC358743 and is not a fault by itself.**
- Reasoning: 999 Mbps matches the default 972 Mbps link.
- It does not match `link-frequency=297000000` (594 Mbps) [C-52].
- Whether reception is reliable at either setting is UNKNOWN (OQ-050).

**Evidence.** [B-16], [B-49], [C-19], [C-31], [C-52]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Investigate only if it comes with stream errors or corruption. How to read CFE receive errors is UNKNOWN (OQ-050). Check that `link-frequency` is at its default.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Reasoning: keep the default link frequency (486000000) on Pi 5/CM5 [C-52]. ADR-008 (PROPOSED; OQ-099) proposes this.

**Related.** RISK-011 · OQ-050, OQ-099 · TEST-CAP-002, TEST-CAP-004

### 3.4 STREAMON fails with `-EPIPE` (-32) when `field:none` is omitted

**Symptom.** On Pi 5/CM5, `VIDIOC_STREAMON` on the CFE video node returns `-EPIPE` (-32).

**Likely cause.**
- In a Raspberry Pi kernel issue, the reporter found that leaving `field:none` out of the formats set on the `csi2` pads makes STREAMON fail with `-EPIPE` [C-33].
- Related, reported by Raspberry Pi engineers: on Pi 5 the pipeline must be configured through the Media Controller API; `VIDIOC_S_FMT` alone does not set the resolution [C-41].
- The `csi2` → video-node link is disabled until userspace enables it [C-32].

**Evidence.** [C-32], [C-33], [C-41]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Inspect the formats on the TC358743 pad 0, `csi2:0`, `csi2:4` and the video node, and the link state. The print command is NEEDS VERIFICATION (OQ-101).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Use the sequence reported by Raspberry Pi engineer 6by9 on kernel 6.18.39 [C-33].

- 6.18.39 is only the kernel of that community report. The bring-up image ships 6.18.50 [G-04], and the driver source inspected in research is the `rpi-6.18.y` tip at 6.18.55 [E-37] (OQ-097).
- `<N>` and the entity name are placeholders (the report used `-d 2`).
- The source shows the `-V` line literally for `"csi2":0`; the other two `-V` lines use the same form on the pads the report names:

```bash
media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'
media-ctl -d <N> -V '"tc358743 1x-000f":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
media-ctl -d <N> -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
media-ctl -d <N> -V '"csi2":4 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
```

Then set the video node to `pixelformat=UYVY` (or `BGR3` for `RGB888_1X24`) [C-33]. The full `v4l2-ctl` option is NEEDS VERIFICATION (OQ-101).

**Related.** RISK-012 · OQ-049, OQ-097, OQ-101 · TEST-CAP-002

### 3.5 Corrupted image near lane-count boundaries

**Symptom.** Visibly corrupted frames in some input modes while other modes are clean.
- Reported in an open issue at 1080p50 RGB24 on a 4-lane CM4, where the driver chose 3 lanes [C-43].
- Other modes near lane-capacity limits (for example 720p30 RGB and 1280x1024@60 RGB) were also reported. That is a research gap (topic C), not a register fact: NEEDS VERIFICATION.

**Likely cause.**
- A Raspberry Pi engineer attributed the corruption to two things [C-43]:
  - the hard-coded FIFO trigger level of 374 [B-12] (he noted that Toshiba's spreadsheet gives a minimum of 120);
  - the lane formula using active height instead of total line time.
- A Raspberry Pi engineer also reported that the FIFO level of 374 is an empirical value [A-43].

**Evidence.** [A-43], [B-12], [C-43], [C-50]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Capture a known test pattern in the affected mode and in neighbouring modes. Record the lane count the driver chose and whether corruption follows mode, format or lane count (TEST-CAP-004). Record CSI-2 receive errors where a method exists; how to read them is UNKNOWN on both receivers (RP1 CFE: OQ-050; Unicam: OQ-095).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Prefer UYVY (ADR-005, PROPOSED).
- Leave affected modes out of the EDID (OQ-002).
- A driver change to the FIFO level would be a patch under ADR-002 (PROPOSED). Computing the correct value needs Toshiba's register spreadsheet (OQ-027, OQ-035).
- Reasoning: 2-lane 1080p50 UYVY also has little margin, about 7.6 % above the spreadsheet's minimum rate [C-50]. Its per-active-lane load equals that of the reported-corrupt 3-lane 1080p50 RGB888 case, and RISK-006 covers it.
- Reasoning: a 4-lane port adds no per-lane margin to modes the driver packs into fewer lanes, because the driver activates only the lanes it computes [C-49]. On CM4 CAM1, ADR-008 (PROPOSED; OQ-099) proposes evaluating `link-frequency=297000000` for 1080p60 UYVY, which then uses all 4 lanes (reasoning) [C-47], [C-49].

**Related.** RISK-006 · OQ-027, OQ-035, OQ-038, OQ-050, OQ-095, OQ-099 · TEST-CAP-004, TEST-PERF-001

---

## 4. Pixel format and colour

### 4.1 Colour-swapped image with RGB888

**Symptom.** Red and blue are swapped when capturing `RGB888_1X24`.

**Likely cause.**
- The two receivers label the same bus format differently. The Pi 5 CFE maps `RGB888_1X24` to `V4L2_PIX_FMT_BGR24`; Pi 4/CM4 Unicam maps it to `V4L2_PIX_FMT_RGB24` [C-34].
- On CM4, an issue reporter found the bytes in memory in B,G,R order. A Raspberry Pi engineer explained that `RGB888_1X24` puts blue in the least significant bits and CSI-2 sends blue first [C-35].

**Evidence.** [C-33], [C-34], [C-35]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Capture a frame of a single primary colour and inspect the bytes (OQ-045).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Capture UYVY instead (ADR-005, PROPOSED).
- If RGB888 is required:
  - On Pi 5/CM5, set `BGR3` on the video node, as in the sequence reported by a Raspberry Pi engineer [C-33].
  - On Pi 4/CM4, treat the buffer as B,G,R. Users have been reported swapping it with GStreamer `capssetter` [C-35].

**Related.** RISK-016 · OQ-003, OQ-045 · TEST-CAP-002

### 4.2 `Incorrect pixel format` warning from the CFE

**Symptom.** On Pi 5/CM5, a one-time kernel warning that begins `Incorrect pixel format` and asks to fix the application.

**Source status.** This signature comes from a research gap (topic C), **not a register fact**: NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED). The research found that the CFE accepts the legacy `RGB3` + `RGB888_1X24` pairing with only this one-time warning, and that the memory order is B,G,R either way.

**Likely cause.** The application set `RGB3` (`RGB24`) on the video node for `RGB888_1X24`, while the CFE maps that bus format to `BGR24` [C-34].

**Evidence.** [C-34], [C-33]; research gap, topic C.

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Check the video-node pixel format set by the application.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Use `BGR3` for `RGB888_1X24` on Pi 5/CM5, matching the CFE mapping [C-34] and the reported Pi 5 sequence [C-33], or capture UYVY (ADR-005, PROPOSED).

**Related.** RISK-016 · OQ-045 · TEST-CAP-002

### 4.3 Wrong colours or levels after encoding (UYVY colourimetry)

**Symptom.** Recorded or streamed video looks washed out, too contrasty or slightly hue-shifted compared with the source. No log signature is attested.

**Likely cause.**
- The TC358743 converts UYVY output with the BT.601 limited-range matrix and reports it as `V4L2_COLORSPACE_SMPTE170M`. RGB output is full range [B-34].
- Research found that the driver makes this BT.601 choice even for HD sources (research gap, topic B — not a register fact).
- If the encoder signals different colour metadata, players will render it wrongly. Reasoning; OQ-041.

**Evidence.** [B-34]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Pass a known colour reference through the whole chain and compare (HARDWARE TEST REQUIRED; OQ-041).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Set the encoder's colour metadata to match what the capture delivers. The exact settings are UNKNOWN (OQ-041).

**Related.** OQ-041 · TEST-CAP-002, TEST-ENC-001

---

## 5. Source-change events

### 5.1 No `V4L2_EVENT_SOURCE_CHANGE` on `/dev/video0`

**Symptom.** On Pi 5/CM5, an application that subscribes to source-change events on the CFE video node never receives them.

**Likely cause.**
- An open issue reports that the CFE video nodes do not deliver `V4L2_EVENT_SOURCE_CHANGE` raised by a CSI-2 source sub-device.
- A Raspberry Pi engineer replied that, under the Media Controller, applications should subscribe on the source sub-device (`/dev/v4l-subdevN`) [C-42].
- Research found the cause in the CFE's notify function (research gap, topics B and C — not a register fact).

**Evidence.** [B-18], [B-38], [B-39], [C-42]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Subscribe on the TC358743 sub-device node and repeat the source change. The `v4l2-ctl` event option is NEEDS VERIFICATION (OQ-101).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Subscribe on the TC358743 sub-device node.
- The driver supports `V4L2_EVENT_SOURCE_CHANGE` subscriptions [B-38], has the events flag [B-18], and sends the event when a sub-device node exists [B-39].

**Related.** RISK-013 · OQ-051, OQ-101 · TEST-CAP-003

### 5.2 Source changes detected late (up to about 1 s), or capture does not resume

**Symptom.**
- Connect, disconnect or resolution changes are noticed only after up to about one second.
- Or, after a change, capture stays muted or fails until the pipeline is reconfigured.

**Likely cause.**
- The stock overlays wire no interrupt, so the driver polls the chip's interrupt status every 1000 ms [A-30], [B-19], [C-20].
- On a sync or size change the driver mutes the stream and sends an event, but **does not apply the new timings itself** [B-39].
- On +5V loss it drops HPD and zeroes the stored timings [B-23].

**Evidence.** [A-29], [A-30], [B-19], [B-23], [B-39], [C-20], [E-39]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Time the interval between the physical change and event delivery (TEST-CAP-003).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- On every event, the application must re-query timings, reconfigure the formats and restart streaming (REQ-CAP-004).
- To reduce latency, wire the TC358743 INT output (active-high, level-triggered [A-29]) to a Pi GPIO and add an `interrupts` property, if the hardware allows it (OQ-020).
- Enabling CEC shortens the poll interval to 10 ms [A-30], but the Raspberry Pi defconfigs do not enable the driver's CEC option [E-39] (OQ-016).

**Related.** RISK-013 · OQ-016, OQ-020, OQ-039, OQ-051 · TEST-CAP-003

---

## 6. Encoder, DMABUF and memory

### 6.1 DMABUF import fails: `contiguous chunk is too small`

**Symptom.** On Pi 4/CM4, importing a capture buffer into the hardware encoder (`/dev/video11`) fails, and the kernel log shows `contiguous chunk is too small`.

**Likely cause.**
- The encoder queues use `videobuf2-dma-contig`. An imported DMABUF must be a single DMA-contiguous region at least as large as the plane [D-20].
- The encoder uses one memory plane, even for planar formats [D-21]. It rounds `bytesperline` up to a 64-byte multiple for UYVY and YUV420 [D-22].
- A buffer that is not contiguous, or is smaller than the encoder's computed plane size, fails.

**Evidence.** [D-19], [D-20], [D-21], [D-22], [C-53]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Record which allocator produced the buffer, its size, and the capture and encoder `bytesperline` and `sizeimage`.
- Reasoning from [C-53] and [D-22]: a 1920-pixel UYVY line is 3840 bytes, a 64-byte multiple. A 720-pixel line is 1440 bytes, which is not (OQ-058).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Use contiguous buffers. Unicam allocates with `videobuf2-dma-contig` and no IOMMU (reasoning, CORRECTED) [C-53]. The Raspberry Pi kernels provide a CMA DMA-BUF heap [E-48].
- Make the capture buffer at least the encoder's plane size.

**Related.** RISK-020 · OQ-058, OQ-062 · TEST-DMA-001

### 6.2 GStreamer element `v4l2h264enc` missing

**Symptom.** A pipeline using `v4l2h264enc` fails because GStreamer has no element of that name.

**Likely cause.**
1. GStreamer is not installed. The Raspberry Pi OS Lite image contains no GStreamer packages [G-32]; the V4L2 elements are in `gstreamer1.0-plugins-good` [G-26].
2. The element is not a static element. It is registered at plugin load by probing V4L2 memory-to-memory devices, and only when the plugin was built with the `v4l2-probe` option [D-38], [G-23].
   - In Buildroot this needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y`, which is not on by default [E-32], [D-39], [G-65].
3. There is no H.264 encoder device to probe. On Pi 5/CM5, `/dev/video11` does not exist (see 6.3). On Pi 4/CM4 the codec can be missing because of the firmware (see 6.4).

**Evidence.** [D-38], [D-39], [E-32], [G-23], [G-26], [G-32], [G-65]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check that `/dev/video11` exists (Pi 4/CM4).
2. List the registered GStreamer elements. The command is NEEDS VERIFICATION.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Install the GStreamer packages, or enable the Buildroot option.
- On Pi 5/CM5, official documentation says to replace `v4l2h264enc` with `x264enc speed-preset=1 threads=1` [D-37].

**Related.** RISK-003 · OQ-015, OQ-065, OQ-066 · TEST-ENC-001, TEST-BLD-001

### 6.3 No `/dev/video11`

**Symptom.** On Pi 5/CM5, there is no `/dev/video11` (`bcm2835-codec-encode`), even though the `bcm2835-codec` module is built for the Pi 5 kernel [G-18].

**Likely cause.** This is expected.
- Pi 5 and CM5 have no hardware video encoder [D-31], [G-22].
- The VCHIQ platform driver that registers `bcm2835-codec` matches only BCM2835/2836/2711 compatibles, and no VCHIQ node exists in the BCM2712 device trees [D-28].
- Reasoning: `bcm2835-codec` therefore cannot probe on Pi 5/CM5 [D-29].

**Evidence.** [D-28], [D-29], [D-31], [G-18], [G-22]

**Diagnosis steps.** None needed; confirm the platform.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Use software encoding. Official documentation gives `x264enc speed-preset=1 threads=1` [D-37]. `rpicam-apps` switches to `libx264` on Pi 5 [D-33].
- The software H.264 encoders researched, GStreamer `x264enc` and FFmpeg `libx264`, do not accept packed UYVY, so convert to a planar format first [D-40], [D-43] (OQ-060).

**Related.** RISK-003 · OQ-059, OQ-060 · TEST-ENC-001

### 6.4 Encoder disappears with the cut-down firmware

**Symptom.** On Pi 4/CM4, the hardware encoder is absent after a `config.txt` or firmware change.

**Likely cause.**
- `gpu_mem=16` selects the cut-down firmware (`start4cd.elf` / `fixup4cd.dat`), which removes codec support [D-48].
- In Buildroot, the `PI4_CD` firmware variant contains "only features required to boot a Linux kernel" [D-48], [E-13].
- The codec also depends on the VCHIQ platform device [D-28], [D-03].

**Evidence.** [D-03], [D-28], [D-48], [E-13]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Check `config.txt` for `gpu_mem=16`, and check which firmware variant is installed.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Do not set `gpu_mem=16`.
- Use the standard (`PI4`) or extended (`PI4_X`) firmware [D-48].
- The firmware and GPU-memory settings the codec needs are UNKNOWN (OQ-048).

**Related.** RISK-002 · OQ-048, OQ-065 · TEST-ENC-001

### 6.5 `flvmux` will not link to the hardware encoder output

**Symptom.** On Pi 4/CM4, a GStreamer RTMP pipeline `v4l2h264enc ! flvmux ! rtmp2sink …` fails to link or negotiate.

**Likely cause.**
- `flvmux` needs H.264 as `stream-format=avc` (CORRECTED) [F-34].
- Reasoning: `v4l2h264enc` outputs `stream-format=byte-stream`, so an `h264parse` element (or an equivalent conversion) is needed between them [F-35].

**Evidence.** [F-33], [F-34], [F-35]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Inspect the caps negotiation error. The debug option is NEEDS VERIFICATION.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Reasoning: insert `h264parse` between the encoder and `flvmux` [F-35]. The documented reference pipeline is `x264enc ! flvmux ! rtmp2sink location=rtmp://...` [F-33].

**Related.** RISK-003 · OQ-007, OQ-075 · TEST-STR-001

### 6.6 Buffer allocation fails at stream start (CMA exhausted)

**Symptom.** Buffer allocation or STREAMON fails at stream start. **The exact log text is not attested by any source: UNKNOWN.**

**Likely cause.** The contiguous-memory (CMA) pool is too small for the capture buffers, plus encoder and display buffers.
- Reasoning (CORRECTED) [C-53]: four 1080p capture buffers take about 16.6 MB in UYVY or 24.9 MB in RGB888.
- The DT CMA pool is 64 MB (CORRECTED) [E-47].
- `vc4-kms-v3d-pi5` sets 64 MB, and `vc4-kms-v3d-pi4` sets (512 − 4) MB [C-40].
- Whether Pi 5/CM5 CFE buffers use CMA at all is not established (reasoning from kernel source, CORRECTED) [C-53] (OQ-053).

**Evidence.** [C-40], [C-53], [E-47]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Read `CmaFree` in `/proc/meminfo` while streaming. This is the measurement named in the reasoning entry [C-53].

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Raise the CMA size with the `cma-*` / `cma-size` parameters of the `vc4-kms-v3d` or `cma` overlays [C-40]. The exact `config.txt` line is NEEDS VERIFICATION.

**Related.** RISK-020 · OQ-053, OQ-061 · TEST-PERF-001

---

## 7. Hardware handling

### 7.1 Damage from a wrongly sided FFC adapter

**Symptom.** The bridge board or the Pi does not power up, or is damaged, after a 15-pin bridge board is connected to a 22-pin camera connector through an adapter cable.

**Likely cause.** Reported by Raspberry Pi engineer 6by9 [C-45]:
- the Auvidea B101 uses a 15-pin FFC with contacts on the same side, while Pi 5 has 22-pin connectors;
- a wrongly sided 22-to-15-pin adapter swaps pin 1 (GND) with pin 15 (3V3), which can short power rails and damage either board.

Reasoning: the warning was given for Pi 5 [C-04], but the CM4 IO Board [C-03] and the CM5 IO Board [C-06] also use 22-pin camera connectors, so the same adapter hazard is expected there. The Pi 4 Model B connector is 15-pin [C-01].

**Evidence.** [C-45], [C-01], [C-03], [C-04], [C-06]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Inspect the cable orientation and contact sides against both connector pinouts. Measure the rails only after removing the suspect cable.

**Remedy (prevention; NOT YET RUN ON PACSCORDER HARDWARE).**
- Before first power-up, check the adapter orientation against the bridge board's schematic and the Pi connector pinout (VENDOR CONFIRMATION REQUIRED, OQ-021).
- Record the cable part number in [HARDWARE.md](HARDWARE.md).

**Related.** RISK-021 · OQ-018, OQ-021 · TEST-HW-001

### 7.2 Bridge board unpowered or held in reset because CAM_GPIO stays low

**Symptom.**
- The bridge does not answer on I2C (1.1), and its supply or reset input is controlled by the camera connector's CAM_GPIO / power-enable pin.
- The board-level symptom (LEDs, rails) depends on the board: UNKNOWN.

**Likely cause.**
- The stock TC358743 overlay and driver request no camera regulator [C-23].
- Reasoning: the camera power-enable line is therefore expected to stay low while the TC358743 is in use [C-51]. That line is expander GPIO 5 on Pi 4/CM4 [C-21], and RP1 GPIO 34/46 on Pi 5 or RP1 GPIO 34 on CM5 [C-22].

**Evidence.** [C-21], [C-22], [C-23], [C-51]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check the board schematic for CAM_GPIO use (VENDOR CONFIRMATION REQUIRED).
2. Measure the pin and the board rails (HARDWARE TEST REQUIRED).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Reasoning from [C-23] and [C-51]: a board that needs CAM_GPIO requires a custom overlay that drives the line, for example as the driver's optional `reset` GPIO [C-23] or as a regulator that is kept on. The design is UNKNOWN — VERIFICATION REQUIRED; design it in [DEVICE_TREE.md](DEVICE_TREE.md) (OQ-022).

**Related.** RISK-021 · OQ-020, OQ-022 · TEST-HW-001

---

## Verification status

### Verified from sources (fact IDs)

Every signature, cause and remedy above cites entries of [REFERENCES.md](REFERENCES.md). All of them have verdict `CONFIRMED` or `CORRECTED`; CORRECTED entries are used in their corrected wording only.

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-03, A-04, A-08, A-15, A-16, A-19, A-20, A-21, A-22, A-23, A-24, A-25, A-29, A-30, A-31, A-33, A-43, A-44, A-45, A-46, A-50 |
| B — tc358743 Linux driver | B-06, B-07, B-09, B-10, B-11, B-12, B-14, B-16, B-18, B-19, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-29, B-31, B-32, B-34, B-38, B-39, B-41, B-42, B-43, B-45, B-47, B-48, B-49 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-08, C-10, C-11, C-13, C-14, C-16, C-17, C-19, C-20, C-21, C-22, C-23, C-24, C-25, C-26, C-27, C-28, C-31, C-32, C-33, C-34, C-35, C-36, C-37, C-39, C-40, C-41, C-42, C-43, C-45, C-47, C-49, C-50, C-51, C-52, C-53 |
| D — Encoders | D-03, D-17, D-19, D-20, D-21, D-22, D-28, D-29, D-31, D-33, D-37, D-38, D-39, D-40, D-43, D-48 |
| E — Buildroot and kernel configuration | E-13, E-32, E-37, E-39, E-47, E-48 |
| F — ATEM and streaming | F-23, F-33, F-34, F-35 |
| G — Raspberry Pi OS and image tooling | G-04, G-12, G-18, G-22, G-23, G-25, G-26, G-32, G-65 |

- `CORRECTED` entries cited: A-22, A-25, B-11, B-21, B-25, C-28, C-36, C-39, C-53, E-47, F-34.
- `community` entries cited, worded as reports: A-43, C-28, C-33, C-35, C-41, C-42, C-43, C-45, D-17.
- `reasoning` entries cited, labelled as reasoning: A-23, B-10, B-11, B-27, B-47, B-49, C-47, C-49, C-50, C-51, C-52, C-53, D-29, F-35.
- Two signatures rest partly on research gaps, not register facts: 4.2 (`Incorrect pixel format`) and the extra modes in 3.5. They are marked NEEDS VERIFICATION where used. The "even for HD sources" note in 4.3 is also a research gap, labelled as such.
- "Verified from sources" means only that the cited source says so. Under Rule 23, a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No signature in this document has been observed on PACSCORDER, and no diagnosis step or remedy has been run.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review: every remedy marked NOT YET RUN ON PACSCORDER HARDWARE; EDID-persistence statement in 2.1 re-sourced to [A-31], [B-21] plus a research gap instead of [B-22]; `failed to get refclk` cause reworded to match [A-22]; extra modes in 3.5 marked NEEDS VERIFICATION; 3.4 notes which `-V` line is literal in the source; encoder UYVY scope (D-40, D-43) narrowed; 7.1 extended to CM4/CM5 IO Board 22-pin connectors as reasoning; 7.2 remedy labelled reasoning. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: 2.5 "ADR-006 (PROPOSED) chooses" changed to "proposes … if it is accepted"; `v4l2-ctl -d /dev/v4l-subdevN` marked NEEDS VERIFICATION (OQ-101) in the conventions, 2.1 and 2.2, and other unattested command steps (`i2cdetect -y`, media-graph print, event wait, video-node format, EDID/control read-back) linked to OQ-101; bare/combined `config.txt` parameter syntax marked NEEDS VERIFICATION (OQ-100) in the conventions, 2.5 and 3.1; link-frequency remedies in 1.3, 3.1, 3.3 and 3.5 linked to ADR-008 (PROPOSED; OQ-099); 3.1 states that a 4-lane path is necessary but not shown sufficient for 1080p60 UYVY (OQ-038); 3.4 labels kernel 6.18.39 as the kernel of the [C-33] report only, with 6.18.50 [G-04] / 6.18.55 [E-37] and OQ-097; 3.3 and 3.5 link the CSI-2 error-counter method to OQ-050/OQ-095; 3.5 notes RISK-006 covers 2-lane 1080p50 UYVY; 4.3 "even for HD sources" no longer attributed to [B-34] (research gap, topic B); 2.1 EDID trigger linked to OQ-093. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: interlaced-input remedy no longer generalises "ATEM capture is not affected" from the ATEM Mini Pro [F-23] to all ATEM models; other ATEM models and directly connected cameras (REQ-CAP-008) are UNKNOWN per model (OQ-102). §3.1 remedy split by lane configuration (REQ-CAP-007): on the 2-lane configuration 1080p60 is beyond the link [C-37] and the EDID restriction applies; the `4lane` remedy is for the 4-lane configuration. No new fact ID cited. | Claude (session 2026-10-07) |
