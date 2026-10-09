# PACSCORDER Troubleshooting

| | |
|---|---|
| Document status | Active — 29 failure signatures collected from sources; **none observed on PACSCORDER**. *(2026-10-08: 37 signatures — eight added from research topics H and I, entries 6.7 to 6.9 and 8.1 to 8.5. Later on 2026-10-08: the H.265 entries 6.7 and 6.9 are deferred — REQ-ENC-002; not in current scope, and kept as reference.)* *(2026-10-09: 49 signatures — twelve added from research topics J and K, entries 9.1 to 9.7 (recording storage and power loss) and 10.1 to 10.5 (live latency); none observed on PACSCORDER.)* |
| Last updated | 2026-10-09 |
| Applies to | TC358743 bridge and its board; the in-tree `tc358743` driver; Unicam (Pi 4 Model B, CM4); RP1 CFE (Pi 5, CM5); the Pi 4/CM4 `bcm2835-codec` encoder; GStreamer/FFmpeg integration; hardware handling. *(Added 2026-10-08.)* Software H.265 encoding (x265) and HEVC transport *(later on 2026-10-08: deferred — REQ-ENC-002; not in current scope, because the owner chose "H.264 only for now" (OQ-103 ANSWERED); entries 6.7 and 6.9 are kept as reference)*; the `tc358743-audio` I2S path (CM4 `bcm2835-i2s`, CM5 RP1 I2S1) and audio encoders. Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07; ADR-004 OPEN). *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* Recording storage as decided in ADR-009 (ACCEPTED, owner 2026-10-08): fragmented MP4 mirrored to a PCIe NVMe SSD (CM4 IO Board PCIe slot; CM5 IO Board M.2 slot) and a USB-to-SATA HDD in a self-powered enclosure; and the WebRTC live path against the owner's < 1 s camera-to-viewer target for WebRTC viewers (OQ-116 ANSWERED; RTMP best-effort) |
| Verification | Every signature comes from the source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)), and, for the entries added on 2026-10-08, from the source research of topics H and I, and, for the entries added on 2026-10-09, from the source research of 2026-10-08 on topics J (recording storage and power loss) and K (live latency). No signature has been observed on PACSCORDER hardware, because none exists as of 2026-10-06. Diagnosis steps and remedies are source-derived and **NOT YET RUN ON PACSCORDER HARDWARE**. |
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
   - *(Added 2026-10-08.)* Audio: the register attests the ALSA device string `hw:CARD=tc358743,DEV=0` [I-15] and one `arecord` capture command from a 2019 forum thread (community source) [I-16]. Listing ALSA cards, reading a V4L2 control, waiting for a control event, and complete GStreamer or FFmpeg H.265 and audio pipelines are NEEDS VERIFICATION (OQ-101).
   - *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* Storage and latency: the register attests the kernel parameter `usb-storage.quirks=VID:PID:Flags` [J-26], given in `cmdline.txt` as a workaround by a Raspberry Pi engineer (community source, CORRECTED) [J-27], the dtparams `pciex1` and `pciex1_gen` [J-15], the fstab options `nofail` and `x-systemd.device-timeout=30` (CORRECTED) [J-33], the muxer settings `fragment-duration` [J-39] and `+frag_keyframe` / `hybrid_fragmented` [J-45], and the element `latency` properties [K-07], [K-08], [K-09]. Listing PCIe or USB devices, showing the bound USB storage driver, inspecting an MP4's structure, reading the browser's selected ICE path and measuring camera-to-viewer latency are NEEDS VERIFICATION (OQ-101). The remedies set the `pciex1` dtparam to on, because it defaults to off [J-15]; no source in the register quotes the `config.txt` line that does so (bare `dtparam=pciex1` or with a value), so its form is NEEDS VERIFICATION (OQ-101).
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
| 6.7 | H.265 will not link to `flvmux` (HEVC cannot be muxed into FLV with GStreamer 1.26.2) *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | userspace / RTMP | all (software H.265) |
| 6.8 | `opusenc` rejects 44.1 kHz audio *(added 2026-10-08)* | userspace / WebRTC audio | all |
| 6.9 | `x265enc` or `libx265` will not accept the UYVY capture format *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | userspace / encoder | all |
| 7.1 | Damage from a wrongly sided FFC adapter | hardware | Pi 5 (reported); CM4 IO Board, CM5 IO Board (22-pin, reasoning) |
| 7.2 | Bridge board unpowered or held in reset because CAM_GPIO stays low | hardware / DT | all |
| 8.1 | Audio plays too fast or too slow, pitch is wrong, A/V drift grows — with no error (sample-rate mismatch) *(added 2026-10-08)* | audio / ALSA | CM4, CM5 |
| 8.2 | No `tc358743` ALSA card, or the card cannot be opened, on CM5 *(added 2026-10-08)* | audio / DT / ASoC | CM5 (CM4 for the shared causes) |
| 8.3 | GPIO 18–21 conflict between `tc358743-audio` and another overlay *(added 2026-10-08)* | audio / DT / pins | CM4, CM5 |
| 8.4 | Audio card present but capture is silent, or "Audio present" reads 0 *(added 2026-10-08)* | audio / HDMI / wiring | CM4, CM5 |
| 8.5 | Lip-sync offset at start, or audio/video drift over time *(added 2026-10-08)* | audio / timestamps | CM4, CM5 |
| 9.1 | No NVMe SSD in the CM4 IO Board's PCIe slot *(added 2026-10-09)* | storage / PCIe / power | CM4 (CM4 IO Board) |
| 9.2 | No NVMe SSD in the CM5 IO Board's M.2 slot *(added 2026-10-09)* | storage / PCIe / DT | CM5 (CM5 IO Board) |
| 9.3 | USB-to-SATA bridge stops responding or resets, or the HDD copy is corrupt (UAS) *(added 2026-10-09)* | storage / USB | CM4, CM5 |
| 9.4 | HDD not detected, drops off at spin-up, or fails intermittently (power) *(added 2026-10-09)* | storage / USB power | CM4, CM5 |
| 9.5 | NVMe copy, recording or live stream stalls when the HDD wakes, resets or is unplugged *(added 2026-10-09)* | storage / pipeline | CM4, CM5 |
| 9.6 | MP4 file unplayable after a power cut *(added 2026-10-09)* | recording / muxer | all |
| 9.7 | Boot waits about 90 s (or 30 s) when the HDD is absent *(added 2026-10-09)* | storage / mount | CM4, CM5 |
| 10.1 | WebRTC camera-to-viewer latency far above 1 s *(added 2026-10-09)* | live path / GStreamer | CM4, CM5 |
| 10.2 | Browser does not show the WebRTC video: B-frames in the live encode *(added 2026-10-09)* | live encode / WebRTC | CM5 (CM4 encoder has no B-frames) |
| 10.3 | New WebRTC viewer waits up to about 2 s for the first picture *(added 2026-10-09)* | live encode / keyframes | CM4, CM5 |
| 10.4 | Internet viewers cannot connect, or their latency grows over time *(added 2026-10-09)* | WebRTC network / ICE | all |
| 10.5 | MediaMTX logs `reader is too slow` and drops packets to a viewer *(added 2026-10-09)* | WebRTC server | all |

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
- The current driver cannot capture interlaced input (RISK-009). The ATEM Mini Pro HDMI output is 1080p only, so ATEM Mini Pro capture is not affected [F-23]. Other ATEM models, and the cameras connected directly that are the other required source type (REQ-CAP-008, owner 2026-10-07): UNKNOWN — VERIFICATION REQUIRED per model (OQ-102). *(2026-10-08: OQ-102 was answered on 2026-10-07 — any HDMI camera, with no model list. A camera that outputs only interlaced modes is therefore possible; such modes are to be rejected and reported, not captured, per REQ-CAP-005.)*

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
- *(Added 2026-10-08.)* H.265 is software-only on **every** candidate, including Pi 4/CM4, whose `bcm2835-codec` has no HEVC encoder [D-24], [D-31]. The H.265 encoders also need planar input (see 6.9). *(Later on 2026-10-08: deferred — REQ-ENC-002; not in current scope. The owner chose "H.264 only for now" (OQ-103 ANSWERED), so in current scope this entry concerns H.264 only.)*

**Related.** RISK-003 · OQ-059, OQ-060 · TEST-ENC-001 · *(added 2026-10-08)* RISK-022, OQ-104 *(not in current scope — H.265 deferred, REQ-ENC-002)*

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

*(Added 2026-10-08.)* This remedy applies to H.264 only. An H.265 stream cannot be linked to `flvmux` at all; see 6.7. *(Later on 2026-10-08: H.264 is the only codec in current scope (OQ-103 ANSWERED), and 6.7 is deferred — REQ-ENC-002; not in current scope.)*

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

### 6.7 H.265 will not link to `flvmux` (HEVC cannot be muxed into FLV with GStreamer 1.26.2) (deferred — REQ-ENC-002; not in current scope)

*Added 2026-10-08 (research topic H).*

*Scope note (later on 2026-10-08): deferred — REQ-ENC-002; not in current scope.* The owner chose "H.264 only for now" (OQ-103 ANSWERED), so no H.265 stream is published over RTMP in the current scope and this signature is not expected. The entry is kept unchanged as reference for when REQ-ENC-002 is re-activated. RISK-025, OQ-106 and OQ-107 stay OPEN but are not in the current scope.

**Symptom.** A GStreamer RTMP pipeline such as `… x265enc ! h265parse ! flvmux ! rtmp2sink …` fails to link or to negotiate caps between the H.265 stream and `flvmux`. The exact error text is not attested by any source: UNKNOWN.

**Likely cause.**
- GStreamer 1.26.2's `flvmux` has no H.265 on its video sink pad. It accepts only `video/x-flash-video`, `video/x-flash-screen`, `video/x-vp6-flash`, `video/x-vp6-alpha` and `video/x-h264` with `stream-format=avc` [H-27].
- Legacy FLV/RTMP carries only AVC video; HEVC needs Enhanced RTMP FourCC signalling [F-31].
- The separate `eflvmux` element, which muxes H.265 as `hvc1`, first appears in the GStreamer 1.28 branch and is absent from 1.24 and 1.26 (CORRECTED) [F-34]. Raspberry Pi OS ships 1.26.2 [G-26].

**Evidence.** [F-31], [F-34], [G-26], [H-27]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Confirm the installed GStreamer version. The command is NEEDS VERIFICATION (OQ-101).
2. Inspect the caps negotiation error. The debug option is NEEDS VERIFICATION.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Not fixable inside the distribution's GStreamer. The path is an ADR-007 design choice (OQ-107, RISK-025):
- publish H.265 with FFmpeg 7.1.5, which can mux HEVC + AAC into enhanced FLV for RTMP, with FourCC `hvc1` [H-26]. Its `rtmp_enhanced_codecs` option accepts only `hvc1`, `av01` and `vp09`; any other value fails with `AVERROR_PATCHWELCOME` [H-26]. Whether a destination needs that option is OQ-106;
- or use SRT in MPEG-TS, whose components the distribution stacks contain [H-30] (OQ-076);
- or carry a backported `eflvmux` or a newer GStreamer: BUILD TEST REQUIRED (research gap, topic H — not a register fact; OQ-107).

H.264 RTMP through `flvmux` is not affected (see 6.5).

**Related.** RISK-022, RISK-025 · OQ-076, OQ-103, OQ-106, OQ-107 · TEST-STR-001 *(later on 2026-10-08: RISK-022, RISK-025, OQ-106 and OQ-107 not in current scope; OQ-103 ANSWERED; the TEST-STR-001 H.265 run is deferred — REQ-ENC-002)*

### 6.8 `opusenc` rejects 44.1 kHz audio

*Added 2026-10-08 (research topic I).*

**Symptom.** A GStreamer WebRTC (or other Opus) pipeline fails to negotiate caps at `opusenc` when the HDMI source sends 44.1 kHz audio, while the same pipeline works with a 48 kHz source. The exact error text is not attested by any source: UNKNOWN.

**Likely cause.**
- `opusenc` accepts only 48000, 24000, 16000, 12000 or 8000 Hz on its sink pad [I-47]. FFmpeg's `libopus` wrapper accepts the same five rates, and FFmpeg's native `opus` encoder accepts only 48000 Hz [I-41].
- The HDMI source's rate is set by the source, not by PACSCORDER: the TC358743 drives the I2S clocks [I-03], and its audio PLL tracks the source's audio clock [I-26]. The driver reports 44100 Hz as one of the rates it decodes [I-19].

**Evidence.** [I-03], [I-19], [I-26], [I-41], [I-47]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Read "Audio sampling rate" (TEST-AUD-001 step 4; command NEEDS VERIFICATION, OQ-101) and compare it with the caps at the encoder's sink pad.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Insert `audioresample` before `opusenc` [I-47]. In FFmpeg, resample to 48 kHz before `libopus`; the option is NEEDS VERIFICATION (OQ-101).
- Resample only after capturing at the source's real rate. Opening the ALSA card at 48000 Hz while the source sends 44100 Hz is not a resample: it mislabels the samples (see 8.1).
- `avenc_aac` and `voaacenc` accept both 44.1 and 48 kHz [I-47], so the RTMP/recording path needs no resample for that reason. Where audio is resampled is part of OQ-111.

**Related.** RISK-019, RISK-023 · OQ-063, OQ-111 · TEST-STR-002, TEST-AUD-001

### 6.9 `x265enc` or `libx265` will not accept the UYVY capture format (deferred — REQ-ENC-002; not in current scope)

*Added 2026-10-08 (research topic H).*

*Scope note (later on 2026-10-08): deferred — REQ-ENC-002; not in current scope.* The owner chose "H.264 only for now" (OQ-103 ANSWERED), so no H.265 encoder is used in the current scope and this signature is not expected. The entry is kept unchanged as reference for when REQ-ENC-002 is re-activated. In the current scope, the software H.264 encoders on Pi 5/CM5 also reject packed UYVY and need a conversion stage (see 6.3; [D-40], [D-43]).

**Symptom.** An H.265 pipeline fails to negotiate between the capture (UYVY) and `x265enc`, or FFmpeg refuses the input pixel format for `libx265`. The exact error text is not attested by any source: UNKNOWN.

**Likely cause.**
- GStreamer 1.26.2 `x265enc` accepts only planar formats: Y444, Y42B and I420 at 8 bit, plus 10- and 12-bit variants. It does not accept packed UYVY, YUY2 or NV12 [H-13].
- FFmpeg 7.1.5's `libx265` wrapper accepts only planar or gray formats, never packed `uyvy422`, `yuyv422` or `nv12` (CORRECTED) [H-10].
- The capture format is UYVY (ADR-005, PROPOSED).

**Evidence.** [H-10], [H-13]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Check the caps or pixel format at the encoder input.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Convert UYVY to a planar format (for example I420 or Y42B) before the encoder [H-13], [H-10]. Reasoning: on the CPU at 1080p60 that conversion reads about 249 MB/s and writes about 187 MB/s before x265 starts [H-43]. Whether a hardware block can do it is OQ-060.
- Related low-latency setting, not this signature: FFmpeg's wrapper copies its thread count into x265's frame threads after applying preset and tune (CORRECTED) [H-10], while `tune=zerolatency` sets one frame thread [H-16]. Reasoning from both: the FFmpeg thread count overrides zerolatency's setting, so set it explicitly and record it (OQ-104).

**Related.** RISK-022 · OQ-060, OQ-104 · TEST-DMA-001, TEST-ENC-001 *(later on 2026-10-08: RISK-022 and OQ-104 not in current scope; the H.265 parts of TEST-DMA-001 and TEST-ENC-001 are deferred — REQ-ENC-002)*

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

## 8. HDMI audio

*Section added 2026-10-08 (research topic I).* HDMI audio is required (REQ-CAP-006, DRAFT; owner, 2026-10-07). The path is the `tc358743-audio` overlay: the TC358743 drives I2S into the Pi on GPIO 18–20, a `linux,spdif-dir` stub codec stands in for the bridge, and the ALSA card id is `tc358743` [I-01], [I-02], [I-03], [I-15], [A-47]. The CPU side is `bcm2835-i2s` on CM4 [I-10] and RP1 I2S1 on CM5 [I-07]. Test procedure: TEST-AUD-001.

### 8.1 Audio plays too fast or too slow, pitch is wrong, A/V drift grows — with no error

**Symptom.**
- Recorded or streamed audio is too fast and high-pitched, or too slow and low-pitched, and drifts further from the video over time.
- ALSA, the encoder and the muxer report **no error**.
- It happens with sources at one rate (for example 44.1 kHz) and not at another (for example 48 kHz).

**Likely cause** (reasoning from source [I-18]).
- The kernel has no path that carries the HDMI sample rate into ALSA. The `linux,spdif-dir` stub codec has no `hw_params` and no controls and accepts 8–768 kHz [I-11]. The Pi I2S is the clock consumer and ignores the requested rate [I-13], [I-18]. Nothing outside the TC358743 driver uses its sampling-rate control [I-18].
- So if the application opens the card at 48000 Hz while the source sends 44100 Hz, the 44.1 kHz frames are labelled 48 kHz. Played at 48 kHz they run 1.0884 times fast (+8.84 %, about +1.47 semitones), and the audio timeline is 0.919 of real time, so A/V drift accumulates [I-18].
- Reasoning (Claude; the same arithmetic reversed): opening at 44100 Hz while the source sends 48000 Hz gives slow, low-pitched audio and a timeline 1.088 times real time.
- After a source rate change, the new rate takes up to about 1 s, plus I2C time, to reach the driver's control, because without an `interrupts` property the driver polls every 1000 ms [I-22]. Reasoning from [I-22]: audio captured in that window is mislabelled even when the application follows the control (RISK-023).

**Evidence.** [I-11], [I-13], [I-18], [I-19], [I-20], [I-21], [I-22], [I-23]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Read "Audio sampling rate" (ID 0x00981980, read-only) [I-20]. It is on `/dev/videoN` on CM4 with legacy Unicam, and only on the TC358743 sub-device node on CM5 [I-23]. The `v4l2-ctl` option is NEEDS VERIFICATION (OQ-101).
2. Compare it with the rate the application opened the ALSA device at.
3. Play a steady tone of known frequency from the source and measure it in the recording (TEST-AUD-001 step 6).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Open the ALSA device at the rate read from the control. The driver decodes it from register FS_SET and returns 0 when there is no TMDS signal [I-19].
- Subscribe to `V4L2_EVENT_CTRL` on that control, which the driver accepts, and reopen ALSA at the new rate on every change [I-21]. The application that does this is NOT STARTED.
- Or force one rate through the EDID audio descriptors (OQ-002). Whether to read the rate, force it, or both is OQ-111.
- To shorten the detection window, wire the TC358743 INT output to a Pi GPIO and add an `interrupts` property, if the hardware allows (OQ-020); the stock overlays have none [I-22].

**Related.** RISK-023, RISK-024 · OQ-002, OQ-020, OQ-111 · TEST-AUD-001, TEST-REC-001, TEST-STR-001, TEST-STR-002

### 8.2 No `tc358743` ALSA card, or the card cannot be opened, on CM5

**Symptom.** On CM5, after adding `dtoverlay=tc358743-audio`, either:
- no ALSA card with id `tc358743` appears; or
- the card appears, but opening or starting the capture fails; or
- an application that looks for the CM4 PCM name does not find the device.

The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- **Unconfirmed path.** No official statement or test result shows the overlay capturing audio on CM5 (research gap, topic I; OQ-054). What the source shows:
  - the firmware does not block the overlay: `overlay_map` has no `tc358743-audio` entry, and an overlay not in the map is assumed compatible with all platforms [I-05], [I-06];
  - the labels it needs exist on CM5: `i2s_clk_consumer` is RP1 I2S1 (pin group GPIO 18–21), and the `sound` node exists [I-07], [I-08];
  - `dwc-i2s` accepts the codec-master (BC_FC) format only when the hardware reports the instance as a clock consumer, and returns `-EINVAL` for mixed formats; the RP1 datasheet calls I2S1 the clock-consumer instance [I-09];
  - `hw_params` accepts only 2, 4, 6 or 8 channels and S16/S24/S32_LE, and the capture channel count and formats come from RP1 hardware registers that source inspection cannot see [I-14].
- **Missing companion overlay.** A Raspberry Pi engineer reported that `tc358743-audio` requires `tc358743` to be loaded as well, because the bridge must be configured (community source) [I-16]. On CM5, `dtoverlay=tc358743` loads `tc358743-pi5` [I-05]. The overlay README's "tc358743-fast" is a stale name; no such overlay is built [I-04].
- **Wrong device name.** On CM5 the PCM name differs from CM4's `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15]; reasoning from source, it is expected to be `1f000a4000.i2s-dir-hifi dir-hifi-0` [I-17]. The card index also varies; a 2019 forum thread reported the same card as card 0 and as card 1 (community source) [I-16].
- **Pin conflict.** Another overlay may hold GPIO 18–21 (see 8.3).

**Evidence.** [I-04], [I-05], [I-06], [I-07], [I-08], [I-09], [I-12], [I-14], [I-15], [I-16], [I-17]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check `config.txt` for both the `tc358743` line (TEST-DRV-001) and `dtoverlay=tc358743-audio`.
2. Inspect the kernel log for messages from the sound card, from `dwc-i2s` and from the TC358743 driver. Record them.
3. List the ALSA cards and record the card id, index and PCM name. The command is NEEDS VERIFICATION (OQ-101).
4. Check the other overlays for GPIO 18–21 (8.3).
5. The kernel options are not the expected cause: they are set in both defconfigs and in both packaged 6.18.50 kernels [I-12]. Check this only if the image is custom-built (TEST-BLD-001).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Load both overlays: official documentation says audio needs `tc358743-audio` in addition to `tc358743` [C-37], and a Raspberry Pi engineer reported the same requirement (community source) [I-16].
- Open the device by card id, `hw:CARD=tc358743,DEV=0`, never by PCM name or card index [I-15]; on CM5 the PCM name is expected to differ (reasoning from source) [I-17].
- If `dwc-i2s` rejects the format or `hw_params`, no source-derived fix exists. Record the result in TEST-AUD-001. The resolution is KERNEL SOURCE INSPECTION REQUIRED and HARDWARE TEST REQUIRED (OQ-054). Reasoning from ADR-004's analysis: while audio is required, this result gates CM5 as the product platform.

**Related.** RISK-014 · OQ-052, OQ-054, OQ-101, OQ-114 · TEST-AUD-001, TEST-BLD-001

### 8.3 GPIO 18–21 conflict between `tc358743-audio` and another overlay

**Symptom.** With `tc358743-audio` and another GPIO overlay both loaded, either the audio card fails (8.2, 8.4) or the other function (PWM, IR receiver, analogue-audio remap) no longer responds. The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- When `tc358743-audio` is enabled, the I2S pin group claims GPIO 18, 19, 20 **and 21**, although the TC358743 path uses only 18, 19 and 20. On CM4 the pins are set to ALT0; on CM5 to function `i2s1` [I-30].
- Overlays that default to these pins [I-31]:
  - `pwm` and `pwm-2chan` default to pin 18, and the overlay README notes that pin 18 "is the one used by the I2S audio interface";
  - `gpio-ir` defaults to `gpio_pin` 18;
  - `audremap` offers `pins_18_19` on BCM2835/BCM2711. On BCM2712 it is redirected to `audremap-pi5`, where `pins_18_19` is "Not available; this will not enable audio out".
- Not a conflict: `gpio-fan` defaults to GPIO 12 [I-31]. On CM5 the base Device Tree's power button and fan entries do not use header GPIO 18–21 [I-32].

**Evidence.** [I-10], [I-08], [I-30], [I-31], [I-32]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). List every `dtoverlay` line in `config.txt` and compare their pins with GPIO 18–21. Check the carrier's own use of these pins against its schematic (OQ-018).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Remove the conflicting overlay, or move it to another pin with its pin parameter. The parameter syntax is NEEDS VERIFICATION (OQ-100).
- If GPIO 21 must be freed for another use, a custom overlay with its own pin group for GPIO 18–20 is needed (research gap, topic I — not a register fact; BUILD TEST REQUIRED). The pin allocation is an owner decision (OQ-114).

**Related.** RISK-014 · OQ-018, OQ-100, OQ-114 · TEST-AUD-001

### 8.4 Audio card present but capture is silent, or "Audio present" reads 0

**Symptom.** The `tc358743` card opens and capture runs, but the recording is silent or corrupt, or "Audio present" reads 0 while the source is playing. The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- **No HDMI signal or no audio packets.** "Audio sampling rate" reads 0 when there is no TMDS signal, and "Audio present" reflects the chip's audio-sample status bit [I-19]. The driver unmasks its audio-change interrupts only while +5V / cable is detected [I-21]. No EDID means no hot-plug, so no signal (2.1).
- **Wiring.** The overlay expects LRCK/WFS on GPIO 19, BCK/SCK on GPIO 18 and DATA/SD on GPIO 20 [A-47], [G-14]. These are separate from the CSI-2 cable (reasoning; OQ-025).
- **I/O voltage.** The TC358743's four audio pins are VDDIO2 outputs, rated 1.8–3.3 V [I-27]. The CM4 and CM5 IO Boards have a selectable 1.8 V or 3.3 V GPIO voltage, and VDDIO2 should match it, or the lines need level shifting [I-29] (OQ-024).
- **Source format.** The driver always configures 2-channel I2S [I-24]. What the I2S output carries for compressed (AC-3, DTS) or multichannel input is UNKNOWN (OQ-110).

**Evidence.** [A-47], [G-14], [I-19], [I-21], [I-24], [I-27], [I-29]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Read "Audio present" and "Audio sampling rate" (8.1, step 1).
2. Check HPD and the EDID (2.1).
3. Measure BCK and LRCK on GPIO 18 and 19 with an instrument (HARDWARE TEST REQUIRED).
4. Check the IO board's GPIO voltage setting against the bridge board's VDDIO2 level (VENDOR CONFIRMATION REQUIRED, OQ-024).
5. Repeat with a 2-channel LPCM source.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Correct the wiring or the GPIO voltage setting, or add level shifting [I-29].
- Use 2-channel LPCM sources. Advertising only 2-channel LPCM in the EDID is a research proposal (research design risk, topic I — not a register fact; OQ-002, OQ-110).

**Related.** RISK-014 · OQ-002, OQ-024, OQ-025, OQ-110 · TEST-AUD-001, TEST-DRV-002

### 8.5 Lip-sync offset at start, or audio/video drift over time

**Symptom.** Audio leads or lags video in recordings or streams from the start, or the offset grows during a long run. No log signature is attested.

**Likely cause.**
- **Separate clocks.** The TC358743's internal audio PLL tracks the N/CTS values in the source's ACR packets, so its I2S clocks follow the source's audio clock [I-26]. Video buffers are stamped with `CLOCK_MONOTONIC` at frame start by every Raspberry Pi CSI receiver driver in `rpi-6.18.y` [I-33].
- **GStreamer 1.26.2.** In a `v4l2src` + `alsasrc` pipeline, `alsasrc`'s audio clock normally becomes the pipeline clock, and ALSA driver timestamps are then not used [I-36], [I-35]. `v4l2src` sets PTS from the pipeline clock minus the measured buffer age; after a bad timestamp it assumes a one-frame delay for the rest of the session [I-37].
- **FFmpeg 7.1.** The ALSA input stamps packets with wall-clock time, while the V4L2 input passes monotonic timestamps through by default. Without `-ts abs` or `mono2abs` the clock bases are mixed, and the CLI's default per-input start shift discards the real offset between the inputs [I-38].
- **Sample-rate mismatch** adds drift (8.1).

**Evidence.** [I-18], [I-26], [I-33], [I-35], [I-36], [I-37], [I-38]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Measure the offset at start and after a long run with a clapper or flash-and-beep source (TEST-AUD-001 step 9; TEST-PERF-001). Record the framework and its clock settings. First rule out 8.1.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- FFmpeg: give the V4L2 input `-ts abs` or `mono2abs`, so that both inputs use one clock base [I-38]. The full command line is NEEDS VERIFICATION (OQ-101).
- GStreamer: the clock arrangement (the monotonic system clock with ALSA driver timestamps, or the audio clock as pipeline clock) is unmeasured on PACSCORDER (research gap, topic I). Choose it from TEST-AUD-001 measurements (OQ-112).
- The clock model is to be recorded with ADR-007 (OPEN). The tolerance is UNDEFINED (OQ-112).

**Related.** RISK-024, RISK-023 · OQ-040, OQ-112 · TEST-AUD-001, TEST-PERF-001

---

## 9. Recording storage and power loss

*Section added 2026-10-09 (research topic J; ADR-009).* ADR-009 (ACCEPTED, owner decisions of 2026-10-08) writes every recording as fragmented MP4 to a PCIe NVMe SSD and to a USB-to-SATA HDD in a self-powered enclosure; ext4 on the recording volumes is Claude's proposal inside ADR-009, not an owner decision (OQ-120). No SSD, adaptor, HDD, enclosure or bridge has been chosen (OQ-121, OQ-122). **None of these signatures has been observed on PACSCORDER hardware**, and none has attested log text. Test procedure: TEST-REC-001.

### 9.1 No NVMe SSD in the CM4 IO Board's PCIe slot

**Symptom** (reasoning: the sources say PCIe cards do not work without the +12 V input [J-05], but do not describe how a failure appears). On CM4 with the CM4 IO Board, an NVMe SSD fitted in the PCIe slot through a PCIe-to-M.2 adaptor does not appear as `/dev/nvme0` / `/dev/nvme0n1`, the names Raspberry Pi documents [J-11], or appears but fails under load. The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- **No +12 V input.** The slot is powered from the +12 V DC barrel input (J19); an on-board +12 V-to-+3.3 V converter is used only for the slot; with a typical +5 V-only PoE HAT, PCIe cards do not work [J-05].
- **Adaptor or SSD.** The slot is one PCIe Gen 2 x1 socket designed for standard PC PCIe cards; Raspberry Pi states that it has been used with an NVMe drive through a passive adaptor [J-03]. It is CM4's only PCIe lane [J-01]. Which SSD and adaptor work, and whether the slot converter covers the SSD's peak current, is UNKNOWN (research gap, topic J; OQ-121).
- **Host-controller limits.** The CM4 PCIe host controller does not support 64-bit accesses from the ARM. The CM4 datasheet says kernels 5.10 and newer support MSI-X with up to 32 IRQs and suggests `pci=nomsi` in `cmdline.txt` as a workaround; the CM4 IO Board datasheet says MSI-X is not supported and devices typically fall back to MSI [J-02]. Which applies to kernel 6.18 is OQ-123.

**Evidence.** [J-01], [J-02], [J-03], [J-05], [J-11]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check that the IO Board is powered from the +12 V barrel input, not from 5 V only.
2. Inspect the kernel log for PCIe link and `nvme` messages, and list the PCIe devices. The command is NEEDS VERIFICATION (OQ-101).
3. Record the interrupt mode the `nvme` driver gets (OQ-123; command NEEDS VERIFICATION).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Power the board from the +12 V barrel input [J-05]. If the product cannot provide 12 V, the NVMe target of ADR-009 is lost on a carrier built like the CM4 IO Board (reasoning from [J-05]; RISK-026, OQ-023).
- For interrupt problems, the CM4 datasheet's workaround is `pci=nomsi` in `cmdline.txt` [J-02]; whether it is needed on kernel 6.18 is KERNEL SOURCE INSPECTION REQUIRED and HARDWARE TEST REQUIRED (OQ-123).
- Try another adaptor or SSD; qualification is OQ-121.

**Related.** RISK-026 · OQ-023, OQ-121, OQ-123 · TEST-REC-001

### 9.2 No NVMe SSD in the CM5 IO Board's M.2 slot

**Symptom** (reasoning from the disabled link [J-17] and the unsupported Gen 3 [J-12], [J-13]; no source describes how a failure appears). On CM5 with the CM5 IO Board, an NVMe SSD in the M.2 M-key slot does not appear, or the link is unstable. The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- **M.2 link not enabled.** In the `rpi-6.18.y` device tree, `pcie1`, the external link used by the M.2 slot, has status "disabled", and neither the CM5 dtsi nor the CM5 IO Board files set it to okay [J-17]. The `pciex1` dtparam (alias `nvme`) controls the link and defaults to off [J-15]. Whether the firmware enables the link at run time on the CM5 IO Board is unknown (research gap, topic J; OQ-124).
- **Gen 3 forced.** `dtparam=pciex1_gen=3` raises the link-speed cap [J-15]. On the CM5 IO Board Gen 3 is "possible, but experimental and therefore unsupported" [J-13], and on CM5 it "might not function reliably" [J-12].
- **Form factor.** The slot takes 2230, 2242, 2260 and 2280 drives [J-14].

**Evidence.** [J-12], [J-13], [J-14], [J-15], [J-17]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check `config.txt` for a `pciex1` line and for `pciex1_gen`.
2. Inspect the kernel log and list the PCIe devices (command NEEDS VERIFICATION, OQ-101). Research suggested also reading the `pcie1` node status from the live device tree (research gap, topic J — not a register fact).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Enable the link by setting the `pciex1` dtparam (alias `nvme`) to on; it defaults to off [J-15]. No source in the register quotes the `config.txt` line: its form is NEEDS VERIFICATION (OQ-101). Whether the product image must set it is OQ-124 (REQ-BLD-002).
- Keep the default Gen 2 cap: remove `pciex1_gen=3` [J-13], [J-15].

**Related.** No registered risk · OQ-121, OQ-124 · TEST-REC-001, TEST-PLT-001, TEST-BLD-001

### 9.3 USB-to-SATA bridge stops responding or resets, or the HDD copy is corrupt (UAS)

**Symptom.** During recording, the HDD stops responding, or, rarely, write data is thrown away and the HDD copy's filesystem is found corrupt — the faults a Raspberry Pi engineer's forum post describes (community source, CORRECTED) [J-27]. Reasoning and research design risk, topic J (not described by a register fact): the USB device may also reset or disconnect, with I/O errors in the kernel log. The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- **UAS firmware faults.** A sticky forum post by Raspberry Pi engineer jdb reports that some UAS devices "don't fully implement the UAS specification"; they typically stop responding when sent UAS commands they do not like, or in rare cases throw write data away, which can cause filesystem corruption (community source, CORRECTED) [J-27]. Raspberry Pi's documentation warns that USB SATA adapters can be supported by the bootloader in mass-storage mode but fail if Linux selects UAS mode [J-28].
- **Which driver binds differs by board.** `uas` refuses to bind when the host controller reports `sg_tablesize == 0`; `dwc2` hard-codes 0, so on CM4 with `dtoverlay=dwc2,dr_mode=host` a UASP bridge runs under `usb-storage`, while under Raspberry Pi OS's default `otg_mode=1` (XHCI USB 2.0) `uas` can bind [J-24], [J-07]. On CM5 the USB 3.0 ports are on RP1's xHCI controllers [J-21].
- **Kernel quirks.** Before binding, the kernel applies built-in bridge quirks: for ASMedia 0x174c:0x5106/0x55aa the result depends on `bMaxPower`, link speed and stream count (below SuperSpeed a possible ASM1051 gets IGNORE_UAS); all Seagate enclosures (VID 0x0bc2) get NO_ATA_1X; a HIKSEMI MD202 RTL9210 (0bda:9210) gets IGNORE_UAS; user `usb-storage.quirks` are merged afterwards [J-25]. The same bridge can therefore behave differently on CM4 (USB 2.0) and CM5 (USB 3.0) (reasoning from [J-24], [J-25]; RISK-027).

**Evidence.** [J-07], [J-21], [J-24], [J-25], [J-26], [J-27], [J-28]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Record the bridge's VID:PID and which driver is bound, `uas` or `usb-storage`. The command is NEEDS VERIFICATION (OQ-101); research suggested `lsusb -t` (research open question, topic J — not a register fact).
2. Inspect the kernel log for USB resets, UAS errors and I/O errors during a long recording, and record the exact lines (TEST-REC-001 step 6).
3. On CM4, record which host controller is active: `otg_mode=1` or `dwc2` [J-07].

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Make the bridge run under `usb-storage` (Bulk-Only) instead of `uas`: add `usb-storage.quirks=VID:PID:u` to `cmdline.txt` (`/boot/firmware/cmdline.txt` on the current OS), with 4-digit hex IDs; several devices are comma-separated [J-26], [J-27] ([J-27] is a community source, CORRECTED). Flag `u` is IGNORE_UAS [J-26].
- Qualify the chosen bridge on each board separately (OQ-122). The throughput and CPU cost of `usb-storage` against `uas` on CM4's shared USB 2.0 are unmeasured (research open question, topic J; OQ-122).

**Related.** RISK-027, RISK-028 · OQ-117, OQ-122 · TEST-REC-001, TEST-PERF-001

### 9.4 HDD not detected, drops off at spin-up, or fails intermittently (power)

**Symptom.** The HDD fails intermittently while everything appears to work, as Raspberry Pi's documentation warns for HDDs without a powered hub [J-28]. Reasoning (not described by a source): it may also not be detected, or disconnect as it spins up. The exact log text is not attested by any source: UNKNOWN.

**Likely cause.**
- **Bus power.** One Seagate BarraCuda 2.5-inch family (ST2000LM015/ST1000LM048/ST500LM030), the register's only 2.5-inch HDD figure and not a general one, draws up to 1.0 A at +5 V during spin-up (CORRECTED) [J-30]. On the CM4 IO Board one current-limit switch of about 1.2 A supplies VBUS to all USB connectors [J-06]. On the CM5 IO Board the two USB 3.0 ports share about 1.2 A [J-19], and on a 5 V/3 A supply a 600 mA peripheral limit applies [J-20]. Reasoning: a bus-powered 2.5-inch HDD that needs 1.0 A to spin up exceeds the 600 mA limit, and under the about 1.2 A limits it leaves only about 0.2 A for the bridge and other USB devices, so a self-powered enclosure or powered hub is the safe choice [J-31]. Raspberry Pi's documentation says HDDs typically need a powered USB hub, and that without one intermittent failures can occur even when everything appears to work [J-28].
- **3.5-inch HDD.** It needs +12 V as well as +5 V; USB VBUS supplies only 5 V [J-32].
- **CM4 IO Board hub disabled.** Plugging in the micro-USB cable disables the on-board USB hub [J-06], and the HDD with it (reasoning from [J-06]).
- ADR-009 decision 3 puts the HDD in a self-powered enclosure, so (reasoning) in the decided configuration bus power should not be the cause; check that the enclosure's own supply is connected and on.

**Evidence.** [J-06], [J-19], [J-20], [J-28], [J-30], [J-31], [J-32]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check that the enclosure is self-powered and that its supply is on (ADR-009).
2. CM4: check that no micro-USB cable is plugged in [J-06].
3. CM5: check whether the supply negotiated 5 A or 3 A [J-20]; the command is NEEDS VERIFICATION (OQ-101).
4. Measure VBUS during spin-up and inspect the kernel log for disconnect or over-current messages (HARDWARE TEST REQUIRED; research open question, topic J — not a register fact).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Power the HDD from a self-powered enclosure or a powered hub, never from the board's VBUS (ADR-009; [J-28], [J-31]); a 3.5-inch HDD always needs an external 12 V supply [J-32].
- CM4 IO Board: do not plug in the micro-USB cable while the hub is in use [J-06].
- CM5 IO Board: use the 5 V/5 A supply [J-20].

**Related.** RISK-026, RISK-027 · OQ-023, OQ-117, OQ-122 · TEST-REC-001

### 9.5 NVMe copy, recording or live stream stalls when the HDD wakes, resets or is unplugged

**Symptom** (reasoning, as in RISK-028; no source describes it). While the HDD spins up from standby, resets (9.3) or is unplugged, the NVMe copy shows a gap, the recording encode drops frames, or the WebRTC or RTMP stream freezes or its latency jumps. No log signature is attested.

**Likely cause** (reasoning, as in RISK-028).
- One Seagate BarraCuda 2.5-inch family, the register's only HDD timing figure and not a general one, takes 2.5 s typical, 3.0 s maximum from standby to ready (CORRECTED) [J-30].
- If the HDD writer blocks and its branch has no buffer that can absorb or drop, the stall propagates back to the recording encode, then to the NVMe copy and, if it reaches the capture queue, into the live encode. V4L2 queues are FIFOs; reasoning: each waiting frame adds one frame period, 33.3 ms at 30p [K-36]. On CM4 the two encodes share the hardware encoder (OQ-115). A 3.0 s stall is three times the whole < 1 s live budget.

**Evidence.** [J-30], [J-36], [K-36]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Run TEST-REC-001 step 5 with TEST-STR-002's latency measurement. Record the stall duration, the HDD-branch buffer level and overflow, gaps in the NVMe copy, and live latency and frame rate before, during and after.

**Remedy** (design; NOT YET RUN ON PACSCORDER HARDWARE).
- Decouple the HDD branch, as ADR-009's consequences require; the buffering and overflow policy are open (OQ-117). Research suggests a large or leaky queue in front of the HDD branch and disabling or extending spin-down where the bridge allows (research design risks, topic J — not register facts). Whether `hdparm` standby settings work through the bridge is unknown (research open question, topic J; OQ-117).
- Reasoning from [J-30] and [J-36] (as in OQ-117; not a design figure): riding out a 3.0 s stall (that one Seagate family's standby-to-ready maximum [J-30]) at about 3.15 MB/s per destination needs about 3.0 × 3.15 ≈ 9.5 MB of HDD-branch buffering, before any margin.
- Keep the live branch from building a backlog (OQ-126, RISK-032).

**Related.** RISK-028, RISK-027, RISK-032 · OQ-115, OQ-117, OQ-126 · TEST-REC-001, TEST-STR-002, TEST-PERF-001

### 9.6 MP4 file unplayable after a power cut

**Symptom.** After power is lost during a recording, the MP4 file does not open or play. The exact player or tool error is not attested by any source: UNKNOWN.

**Likely cause.**
- **Written in normal (moov-at-end) mode.** In GStreamer 1.26.2 `qtmux`/`mp4mux`'s default mode, the moov index is written only at EOS and the mdat size is fixed up then; a file with no moov is not playable, so an unclean stop leaves an unplayable MP4 unless fragmented or robust mode is used [J-38]. FFmpeg 7.1 documents that a normal MOV/MP4 is undecodable if not properly finished [J-45].
- **Fragmentation not active.** `fragment-duration` defaults to 0; only a value > 0 produces a fragmented file [J-39]. Whether the shipped GStreamer 1.26.2 and FFmpeg 7.1.5 builds expose the options as upstream documents them was not checked (research open question, topic J; OQ-118).
- A fragmented file that **plays but ends early** is a different case: see the remaining loss below.

**Evidence.** [J-35], [J-38], [J-39], [J-40], [J-45]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check the muxer settings used: `fragment-duration` > 0 in GStreamer, or FFmpeg's `+frag_keyframe` or `hybrid_fragmented`.
2. Inspect the file's structure. The tool is NEEDS VERIFICATION (OQ-101).
3. Repeat with TEST-REC-001 step 4.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Write fragmented MP4, as ADR-009 decides: GStreamer `fragment-duration` > 0, in milliseconds [J-39]; FFmpeg `+frag_keyframe`, which starts a fragment at each video keyframe, or `hybrid_fragmented`, which writes a fragmented file and converts it to a normal one at the end [J-45]. FFmpeg 7.1's muxer documentation says a fragmented file stays decodable if writing is interrupted [J-45]; no register entry states this for GStreamer's fragmented output (TEST-REC-001 step 4 checks it).
- Do not set robust-muxing properties together with `fragment-duration` > 0 and expect both: the muxer then silently produces a fragmented file only [J-39]. Robust muxing [J-40] was not chosen by ADR-009.
- For a file already affected: FFmpeg documents that an aborted `hybrid_fragmented` file can be remuxed by hand [J-45]; GStreamer names the experimental `moov-recovery-file` property as a recovery measure for normal mode [J-38] (not part of ADR-009; NEEDS VERIFICATION).
- **Remaining loss.** Even a decodable file loses its last seconds: with ext4's defaults a power loss loses at most the last 5 s of metadata changes, but because of delayed allocation even older data can be lost (CORRECTED) [J-35]. How many seconds are lost per drive is OQ-119 (RISK-030).

**Related.** RISK-029, RISK-030 · OQ-118, OQ-119, OQ-120 · TEST-REC-001

### 9.7 Boot waits about 90 s (or 30 s) when the HDD is absent

**Symptom.** With the HDD disconnected or switched off, boot pauses for about 90 s, or about 30 s, before continuing.

**Likely cause.** Raspberry Pi's external-storage guide warns that an absent disk adds 90 s to boot, and that appending `,x-systemd.device-timeout=30` after `nofail` shortens that wait to 30 s rather than removing it (CORRECTED) [J-33].

**Evidence.** [J-33]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Check the HDD's fstab options, and time the boot with the HDD absent (TEST-REC-001 step 8).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Use `nofail` with `x-systemd.device-timeout=30` [J-33]; the wait is shortened, not removed. The recording volumes' mount options are OQ-120.
- Whether recording to the NVMe SSD must start when the HDD is missing is UNDEFINED — OWNER DECISION REQUIRED (OQ-117).

**Related.** No registered risk · OQ-117, OQ-120 · TEST-REC-001

---

## 10. Live latency (WebRTC)

*Section added 2026-10-09 (research topic K; OQ-116).* The owner's target is under 1 s camera-to-viewer for WebRTC viewers only; RTMP outputs are best-effort (OQ-116 ANSWERED; REQ-STR-002, REQ-STR-001). The live H.264 encode is shared by RTMP and WebRTC (OQ-005). **None of these signatures has been observed on PACSCORDER hardware.** Test procedures: TEST-STR-002 and TEST-ENC-001.

### 10.1 WebRTC camera-to-viewer latency far above 1 s

**Symptom.** The measured camera-to-viewer latency of a WebRTC viewer is well above the < 1 s target, for example several seconds (reasoning: one 2000 ms default [K-09] would do that on its own; no source describes this symptom on the PACSCORDER stack). No log signature is attested.

**Likely cause.**
- **RTSP element default.** `rtspclientsink` and `rtspsrc` have `latency` = "Amount of ms to buffer", default 2000; MediaMTX's own GStreamer reader examples set `rtspsrc latency=0` [K-09]. Whether `rtspclientsink`'s default adds delay on the sending side is unknown (research open question, topic K; OQ-126).
- **Jitter buffers inside the pipeline.** `webrtcbin` `latency` defaults to 200 ms [K-07]; `rtpbin` and `rtpjitterbuffer` default to 200 ms, and `rtpjitterbuffer` adds that much latency [K-08]. Reasoning: these apply only to RTP received inside the live path, for example an `rtspsrc` relay; a browser viewer uses its own buffer [K-10].
- **x264 defaults (CM5).** GStreamer 1.26 `x264enc` runs x264's medium preset by default (3 B-frames, rc-lookahead 40, MB-tree on) unless downstream caps force a profile such as baseline; only properties set explicitly are layered on top, and the `bframes=0` property default does not guarantee a B-frame-free stream (CORRECTED) [K-29]. x264 holds frames back for B-frames (raised to rc-lookahead when MB-tree is on), sync lookahead and frame threads, none with `zerolatency` [K-28]. Reasoning: with the medium preset's MB-tree and rc-lookahead 40 that is at least 40 frames, about 1.33 s at 30p.
- **Queue backlog.** V4L2 queues are FIFOs; reasoning: each waiting frame adds one frame period [K-36]. `v4l2src` reports a maximum latency of buffer-pool depth × frame duration [K-37]. On CM5, if x264 cannot finish a frame within one frame period, queues grow (research design risk, topic K — not a register fact; RISK-032, OQ-059).
- **Under-reported pipeline latency.** `v4l2h264enc` on BCM2711 reports 0 latency to the pipeline [K-32]; reasoning (as in RISK-032): GStreamer's own latency figure then leaves out the real encode time.
- **Viewer buffer.** The browser's jitter buffer is its own; `jitterBufferTarget` only influences it [K-11].
- An HDD stall reaching the live path (9.5).

**Evidence.** [K-07], [K-08], [K-09], [K-10], [K-11], [K-12], [K-27], [K-28], [K-29], [K-32], [K-36], [K-37], [K-45]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. List every live-path element's latency property and queue settings.
2. In the viewer, read `jitterBufferDelay`, `jitterBufferEmittedCount`, `jitterBufferTargetDelay`, `jitterBufferMinimumDelay` and `totalProcessingDelay` from the W3C statistics API (CORRECTED) [K-12], to separate the viewer's buffering from the sending path. How to read them is NEEDS VERIFICATION (OQ-101).
3. Measure the live encode's per-frame time (TEST-ENC-001), not the pipeline's reported latency [K-32].
4. Run TEST-STR-002 step 6.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- Set every buffering element's latency explicitly and record it (OQ-126); MediaMTX's examples use `rtspsrc latency=0` [K-09].
- On CM5, use `tune=zerolatency` [K-27], [K-28].
- Whether leaky queues are needed in front of the live encoder and payloader is a research open question (topic K; OQ-126): BUILD TEST REQUIRED.
- For orientation only (reasoning, a labelled budget, not a measurement; CORRECTED): for a CM4 1080p30 LAN viewer the documented or extrapolated terms come to about 56 ms typical and about 75 ms worst case, so most of the 1 s is in undocumented terms [K-45] (OQ-125).

**Related.** RISK-031, RISK-032 · OQ-059, OQ-125, OQ-126 · TEST-STR-002, TEST-ENC-001

### 10.2 Browser does not show the WebRTC video: B-frames in the live encode

**Symptom** (reasoning; the sources say only that browsers do not support H.264 B-frames over WebRTC [K-04], not how the failure appears). A browser viewer connects but the video does not decode or plays incorrectly, while RTMP from the same live encode plays. The exact browser error is not attested by any source: UNKNOWN.

**Likely cause.**
- MediaMTX documents that browsers deliberately do not support H.264 with B-frames over WebRTC, and recommends H.264 Baseline (no B-frames) with Opus [K-04]; the MediaMTX project reports the same (community source) [F-45].
- **CM5.** GStreamer 1.26 `x264enc` runs x264's medium preset with 3 B-frames by default, and its `bframes=0` property default does not guarantee a B-frame-free stream (CORRECTED) [K-29]. rpicam-apps' normal-mode `libx264` defaults set `max_b_frames=1` [D-35].
- **CM4 is not affected:** `bcm2835-codec` limits B-frames to 0 [K-30].
- YouTube recommends 2 B-frames for RTMP [K-17]; reasoning from [K-04] and [K-17]: a live encode set up for that advice breaks WebRTC, because the two share one encode (OQ-127).

**Evidence.** [D-35], [F-45], [K-04], [K-17], [K-29], [K-30]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Check the live encode's output for B-frames (TEST-ENC-001 two-encode run, step 4; tool NEEDS VERIFICATION, OQ-101), and record the `x264enc` settings.

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). On CM5, set `tune=zerolatency`, set `bframes=0` explicitly, or force `profile=baseline` in caps (CORRECTED) [K-29]. Do not follow YouTube's B-frame advice on the shared live encode (OQ-127). Profile and level negotiation are separate (OQ-073, RISK-019).

**Related.** RISK-019 · OQ-073, OQ-127 · TEST-ENC-001, TEST-STR-002

### 10.3 New WebRTC viewer waits up to about 2 s for the first picture

**Symptom** (reasoning from [K-30]; see Likely cause). A viewer who joins a running stream sees no picture, or a frozen one, for up to about 2 s at 30 fps before video starts. No log signature is attested.

**Likely cause.** Reasoning from [K-30]: the `bcm2835-codec` default GOP of 60 frames is 2 s at 30 fps, so without an on-demand keyframe a new viewer waits for the next IDR. Whether MediaMTX passes a viewer's keyframe request back to an RTSP-publishing pipeline is undocumented (research open question, topic K; OQ-127).

**Evidence.** [K-17], [K-30], [K-31]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Record the GOP set and the join time (TEST-STR-002 step 7).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE).
- CM4: request an IDR when a viewer joins. `bcm2835-codec` implements force-keyframe, and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe [K-31]. How the join event reaches the pipeline is OQ-127. The CM5 (x264) equivalent is not in the register: NEEDS VERIFICATION.
- Or shorten the GOP. YouTube recommends 2 s keyframes, not over 4 s [K-17]; the interval for the shared live encode is OQ-127.

**Related.** RISK-019 · OQ-127 · TEST-STR-002, TEST-ENC-001

### 10.4 Internet viewers cannot connect, or their latency grows over time

**Symptom.** Viewers on the LAN play, but viewers outside it get no media (reasoning from [K-42]), or their session plays but its latency grows when the network is congested (MediaMTX's configuration says this of TCP [K-41]). No log signature is attested.

**Likely cause.**
- **Unreachable listener or wrong advertised address.** MediaMTX v1.21.1 listens for WebRTC on UDP `:8189` and leaves TCP disabled by default [K-41]. It advertises its interface addresses by default; for internet clients its docs say to add the public IP or DNS name to `webrtcAdditionalHosts`, and STUN/TURN (`webrtcICEServers2`) is "Needed only when local listeners can't be reached by clients" [K-42]. Reasoning from [K-42]: a configuration that only reaches LAN clients fails for remote viewers (research design risk, topic K; RISK-033).
- **TCP path.** MediaMTX's configuration says TCP "is less efficient than UDP and introduces a progressive delay when network is congested" [K-41]. MediaMTX documents four connection methods — static UDP port, static TCP port, random UDP port with STUN hole punching, TURN relay — and recommends TCP transport only for a coturn relay [K-43].

**Evidence.** [K-12], [K-41], [K-42], [K-43]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE).
1. Check MediaMTX's WebRTC settings: UDP listener, `webrtcAdditionalHosts`, `webrtcICEServers2`, and the site's port forwarding.
2. In the viewer, record which path the session uses and its round-trip time; the W3C statistics API exposes candidate-pair `currentRoundTripTime` (CORRECTED) [K-12]. Reading the selected path is NEEDS VERIFICATION (OQ-101).

**Remedy** (source-derived; NOT YET RUN ON PACSCORDER HARDWARE). Make UDP 8189 reachable and advertise the public IP or DNS name in `webrtcAdditionalHosts` [K-41], [K-42]; otherwise provide STUN or a TURN relay [K-42], [K-43]. Whether internet viewers are in scope is OQ-008; the NAT-traversal design is OQ-074; latency per path is OQ-128.

**Related.** RISK-033 · OQ-008, OQ-074, OQ-075, OQ-128 · TEST-STR-002

### 10.5 MediaMTX logs `reader is too slow` and drops packets to a viewer

**Symptom.** MediaMTX logs "reader is too slow" and drops packets to that reader [K-44]. Reasoning (not described by a source): the viewer sees gaps or damaged frames.

**Likely cause.** MediaMTX documents that it favours real-time delivery over reliability: most protocols run over UDP, so late packets can be dropped, and outgoing packets go through a circular buffer (`writeQueueSize`, default 512) that drops packets when full and logs "reader is too slow" [K-44]. Reasoning from the log text: the viewer or its network path is not keeping up.

**Evidence.** [K-44]

**Diagnosis steps** (NOT YET RUN ON PACSCORDER HARDWARE). Note which reader is named, check its network path (10.4), and record the live bitrate (OQ-005).

**Remedy** (NOT YET RUN ON PACSCORDER HARDWARE). No remedy is attested by the register. Reasoning (Claude): a larger write queue would trade dropped packets for added delay, against the < 1 s target, and a lower live bitrate (OQ-005) reduces what each reader must receive. NEEDS VERIFICATION.

**Related.** RISK-031, RISK-033 · OQ-005, OQ-125, OQ-128 · TEST-STR-002

---

## Verification status

### Verified from sources (fact IDs)

Every signature, cause and remedy above cites entries of [REFERENCES.md](REFERENCES.md). All of them have verdict `CONFIRMED` or `CORRECTED`; CORRECTED entries are used in their corrected wording only.

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-03, A-04, A-08, A-15, A-16, A-19, A-20, A-21, A-22, A-23, A-24, A-25, A-29, A-30, A-31, A-33, A-43, A-44, A-45, A-46, A-50; added 2026-10-08: A-47 |
| B — tc358743 Linux driver | B-06, B-07, B-09, B-10, B-11, B-12, B-14, B-16, B-18, B-19, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-29, B-31, B-32, B-34, B-38, B-39, B-41, B-42, B-43, B-45, B-47, B-48, B-49 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-08, C-10, C-11, C-13, C-14, C-16, C-17, C-19, C-20, C-21, C-22, C-23, C-24, C-25, C-26, C-27, C-28, C-31, C-32, C-33, C-34, C-35, C-36, C-37, C-39, C-40, C-41, C-42, C-43, C-45, C-47, C-49, C-50, C-51, C-52, C-53 |
| D — Encoders | D-03, D-17, D-19, D-20, D-21, D-22, D-28, D-29, D-31, D-33, D-37, D-38, D-39, D-40, D-43, D-48; added 2026-10-08: D-24; added 2026-10-09: D-35 |
| E — Buildroot and kernel configuration | E-13, E-32, E-37, E-39, E-47, E-48 |
| F — ATEM and streaming | F-23, F-33, F-34, F-35; added 2026-10-08: F-31; added 2026-10-09: F-45 |
| G — Raspberry Pi OS and image tooling | G-04, G-12, G-18, G-22, G-23, G-25, G-26, G-32, G-65; added 2026-10-08: G-14 |
| H — H.265 software encoding and transport (added 2026-10-08) | H-10, H-13, H-16, H-26, H-27, H-30, H-43 |
| I — HDMI audio path (added 2026-10-08) | I-01, I-02, I-03, I-04, I-05, I-06, I-07, I-08, I-09, I-10, I-11, I-12, I-13, I-14, I-15, I-16, I-17, I-18, I-19, I-20, I-21, I-22, I-23, I-24, I-26, I-27, I-29, I-30, I-31, I-32, I-33, I-35, I-36, I-37, I-38, I-41, I-47 |
| J — Recording storage and power loss (research of 2026-10-08; cited from 2026-10-09) | J-01, J-02, J-03, J-05, J-06, J-07, J-11, J-12, J-13, J-14, J-15, J-17, J-19, J-20, J-21, J-24, J-25, J-26, J-27, J-28, J-30, J-31, J-32, J-33, J-35, J-36, J-38, J-39, J-40, J-45 |
| K — Live latency (research of 2026-10-08; cited from 2026-10-09) | K-04, K-07, K-08, K-09, K-10, K-11, K-12, K-17, K-27, K-28, K-29, K-30, K-31, K-32, K-36, K-37, K-41, K-42, K-43, K-44, K-45 |

- `CORRECTED` entries cited: A-22, A-25, B-11, B-21, B-25, C-28, C-36, C-39, C-53, E-47, F-34; added 2026-10-08: H-10; added 2026-10-09: J-27, J-30, J-33, J-35, K-12, K-29, K-45.
- `community` entries cited, worded as reports: A-43, C-28, C-33, C-35, C-41, C-42, C-43, C-45, D-17; added 2026-10-08: I-16; added 2026-10-09: F-45, J-27.
- `reasoning` entries cited, labelled as reasoning: A-23, B-10, B-11, B-27, B-47, B-49, C-47, C-49, C-50, C-51, C-52, C-53, D-29, F-35; added 2026-10-08: H-43, I-17, I-18; added 2026-10-09: J-31, J-36, K-10, K-45, and the reasoning sentence of the `kernel-source` entry K-36.
- Two signatures rest partly on research gaps, not register facts: 4.2 (`Incorrect pixel format`) and the extra modes in 3.5. They are marked NEEDS VERIFICATION where used. The "even for HD sources" note in 4.3 is also a research gap, labelled as such. *(Added 2026-10-08.)* Entries 6.7 to 6.9 and 8.1 to 8.5 also use items marked *research gap* or *research design risk* (topics H and I) from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json): the backported-`eflvmux` option (6.7), the CM5 audio path being unconfirmed (8.2), the custom GPIO 18–20 overlay (8.3), the 2-channel-only EDID proposal (8.4) and the unmeasured GStreamer clock choice (8.5). They are not register facts and are labelled where used. None of the eight new signatures has attested log text; each says so. *(Added 2026-10-09.)* Entries 9.1 to 9.7 and 10.1 to 10.5 also use items marked *research gap*, *research open question* or *research design risk* (topics J and K) from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json): the slot-converter rating and SSD peak current (9.1), firmware enablement of the CM5 M.2 link and the device-tree status check (9.2), the `lsusb -t` check and the `usb-storage` versus `uas` cost (9.3), VBUS measurement at spin-up (9.4), the HDD-branch queue, spin-down and `hdparm` (9.5), the unchecked muxer options on the shipped builds (9.6), the `rtspclientsink` sender-side question, the CM5 backlog and leaky queues (10.1), keyframe-request forwarding (10.3) and the LAN-only configuration risk (10.4). They are not register facts and are labelled where used. Of the twelve signatures added on 2026-10-09, only 10.5 has attested log text ("reader is too slow" [K-44]); the others say that none is attested. Entries 9.5 and 10.5 rest partly on Claude's reasoning, labelled where used. *(Verifier pass, same date.)* The symptoms of 9.1 to 9.5 and 10.1 to 10.5 that no source describes are also Claude's reasoning, labelled in each Symptom line.
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
| 2026-10-08 | Second set of owner decisions of 2026-10-07 and research topics H and I propagated; no entry rewritten or deleted. Header: status (37 signatures), "Applies to" (software H.265, `tc358743-audio` path, CM4 + CM5 bring-up) and "Verification" rows. Conventions: audio command attestation note (OQ-101). Symptom index: eight rows added. New entries: 6.7 H.265 will not link to `flvmux` (GStreamer 1.26.2 has no H.265 in `flvmux`, `eflvmux` only in 1.28; FFmpeg enhanced FLV, SRT or backport; OQ-107, RISK-025); 6.8 `opusenc` rejects 44.1 kHz audio ([I-47]; resample after capturing at the true rate); 6.9 `x265enc` / `libx265` reject UYVY (planar only; conversion cost; FFmpeg thread-count note); new section 8 HDMI audio: 8.1 sample-rate mismatch with no error (RISK-023, OQ-111), 8.2 missing or unusable `tc358743` card on CM5 (unconfirmed path, companion overlay, PCM name, OQ-054), 8.3 GPIO 18–21 conflicts (OQ-114), 8.4 silent capture or "Audio present" 0 (signal, wiring, VDDIO2 voltage, source format), 8.5 lip-sync offset and drift (RISK-024, OQ-112). Dated notes: 2.3 (OQ-102 answered, no model list), 6.3 (H.265 software-only on every candidate), 6.5 (H.265 see 6.7). Verification table: A-47, D-24, F-31, G-14, topic H and I rows; CORRECTED H-10; community I-16; reasoning H-43, I-17, I-18; 2026-10-08 research JSON items labelled. | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic H and I additions: 8.2 Remedy — "Load both overlays [I-16]" now rests on the official statement [C-37] with the community report [I-16] labelled as such; the card-id bullet labels [I-17] as reasoning from source. All other [H-xx] and [I-xx] citations checked against the register; no change needed. No status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header status and "Applies to" rows note that software H.265 / HEVC transport and entries 6.7 and 6.9 are deferred — REQ-ENC-002; not in current scope; symptom index rows 6.7 and 6.9 and their headings labelled "(deferred — REQ-ENC-002; not in current scope)"; dated scope notes added under 6.7 and 6.9 (signature not expected in current scope; entry kept unchanged as reference; 6.9 notes the H.264 software encoders also reject UYVY [D-40], [D-43]); 6.3 H.265 note and Related line, 6.5 H.265 pointer, and 6.7/6.9 Related lines annotated (RISK-022, RISK-025, OQ-104, OQ-106, OQ-107 not in current scope; OQ-103 ANSWERED). No entry text rewritten or deleted (the 6.7 and 6.9 headings only gained the label); signature count unchanged (37); no status changed. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header — status (49 signatures; none observed on PACSCORDER), "Last updated", "Applies to" (ADR-009 storage path; WebRTC live path against < 1 s, RTMP best-effort) and "Verification" (topics J and K); How to use, item 5 — storage and latency command attestation note (OQ-101); symptom index — twelve rows; new section 9 "Recording storage and power loss": 9.1 no NVMe SSD in the CM4 IO Board PCIe slot (+12 V input, adaptor, MSI/MSI-X) [J-01] to [J-03], [J-05], [J-11]; 9.2 no NVMe SSD in the CM5 IO Board M.2 slot (`pciex1` dtparam, Gen 3) [J-12] to [J-15], [J-17]; 9.3 USB-to-SATA bridge hangs, resets or corruption under UAS, with `usb-storage.quirks` (community report) [J-07], [J-21], [J-24] to [J-28]; 9.4 HDD power and VBUS limits [J-06], [J-19], [J-20], [J-28], [J-30] to [J-32]; 9.5 HDD stall back-pressure into the NVMe copy and live path (reasoning, RISK-028) [J-30], [J-36], [K-36]; 9.6 MP4 unplayable after a power cut, fragmented MP4 as the mitigation, remaining ext4 loss [J-35], [J-38] to [J-40], [J-45]; 9.7 boot delay with the HDD absent [J-33]; new section 10 "Live latency (WebRTC)": 10.1 latency far above 1 s from default element latencies, x264 defaults, queue backlog and under-reported encoder latency [K-07] to [K-12], [K-27] to [K-29], [K-32], [K-36], [K-37], [K-45]; 10.2 browser rejects B-frames in the live encode [D-35], [F-45], [K-04], [K-17], [K-29], [K-30]; 10.3 viewer join waits for the next IDR [K-17], [K-30], [K-31]; 10.4 internet viewers, ICE and TCP fallback [K-12], [K-41] to [K-43]; 10.5 MediaMTX `reader is too slow` [K-44]; Verification status — D-35, F-45, topic J and K rows, CORRECTED, community and reasoning entries, topic J/K research items. No existing entry rewritten or deleted; no status changed. Verifier pass (same date): every [J-xx]/[K-xx] citation checked against the register (181 in the entries); fixed — symptoms of 9.1 to 9.5 and 10.1 to 10.5 that no source describes are now labelled reasoning (9.3 resets and I/O errors also a research design risk; 10.5 retitled "MediaMTX logs `reader is too slow` and drops packets to a viewer", in the index too); 9.2 remedy and the item-5 note: `pciex1` set to on, defaulting to off [J-15], `config.txt` line form NEEDS VERIFICATION; [J-30] named as one Seagate BarraCuda 2.5-inch family (9.4, 9.5, with the 9.5 MB arithmetic); [J-31] limited to an HDD that needs 1.0 A; [J-20] "USB peripherals" → peripheral limit; 9.4 decision-3 inference and VBUS item labelled (reasoning; research open question); 9.6 "fragmented file stays decodable" attributed to FFmpeg [J-45]; 10.1 x264 defaults reworded to [K-29] (medium preset unless caps force a profile; explicit properties layered on top) with the [K-28] lookahead delay as labelled reasoning, [K-37] as what `v4l2src` reports, [K-45] terms "documented or extrapolated", 56 ms typical / 75 ms worst; 10.2 "breaks WebRTC" labelled reasoning; 10.4 TCP statement attributed to MediaMTX's configuration [K-41]; `usb-storage.quirks` worded as a kernel parameter [J-26] given in `cmdline.txt` [J-27]. | Claude (session 2026-10-09) |
