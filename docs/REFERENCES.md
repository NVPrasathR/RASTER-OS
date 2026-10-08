# PACSCORDER Source Register (REFERENCES.md)
| | |
|---|---|
| Document status | Active — topics A–G generated 2026-10-06; topics H and I appended 2026-10-08 (existing entries unchanged) |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 22 (unknowns), Rule 23 (source priority) |
| Raw data | [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json) (A–G), [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json) (H, I) |

Every technical fact used in the PACSCORDER documentation is cited by an ID from this register, for example `[C-37]`.

## How this register was produced

On 2026-10-06, seven research topics (A–G) were investigated:

1. For each topic, one research agent read primary sources and recorded atomic claims, each with a source URL and an evidence excerpt.
2. A second, independent agent then re-fetched the sources and tried to **refute** every claim.

The verifier's verdict and final wording are what this register records.

## What a verdict means

| Verdict | Meaning | May the docs state it as fact? |
|---|---|---|
| `CONFIRMED` | The verifier saw source evidence for the exact statement. | Yes, with the citation. |
| `CORRECTED` | The original claim was partly wrong; the **Fact** text below is the corrected wording. | Yes, the corrected wording only. |
| `UNVERIFIABLE` | The verifier could not see supporting evidence (NDA, paywall, 404). | **No.** Treat as `NEEDS VERIFICATION`. |
| `REFUTED` | The claim is wrong. The Fact text explains what is true if known. | Only the refutation. |

> **Important.** `CONFIRMED` means *the cited source says this*. It does **not** mean the behaviour has been observed on PACSCORDER hardware. Under Rule 23, an actual hardware measurement (priority 1) overrides everything in this register. When a measurement contradicts an entry, record the measurement in [TESTING.md](TESTING.md) and add a note here; do not edit the original entry away (Rule 21).

## Register notes

These notes record caveats found after the register was generated. Per Rule 21, the entries themselves are not edited.

- **2026-10-06 — "Applies to" is the researcher's tag, not a verified support statement.** For example, [A-47] tags the `tc358743-audio` overlay "Pi4, CM4", while [G-14] tags the same overlay "Pi 4 Model B, CM4, Pi 5, CM5". Neither entry shows the overlay working on Pi 5/CM5; that remains open (OQ-054). Read each entry's **Fact** text, not its tag, to decide what it supports.
- **2026-10-06 — Statements not in this register.** Some documents quote material from the research `open_questions` and `gaps` lists in the raw JSON. Those items are labelled *research gap* or *research open question*. They are leads, not verified facts.
- **2026-10-08 — Topics H and I appended.** Researched after the owner decisions of 2026-10-07 (H.264 + H.265 required; HDMI audio required). Two earlier attempts on 2026-10-07/08 failed (network loss, then host sleep) and produced no results.

## Source tiers

| Tier label | Rank |
|---|---|
| `datasheet` | Rule 23 priority 2 — official hardware datasheet |
| `official-rpi` | Rule 23 priority 3 — official Raspberry Pi documentation |
| `kernel-source` | Rule 23 priority 4 — Linux kernel source |
| `buildroot` | Rule 23 priority 6 — Buildroot documentation/source |
| `vendor-other` | not ranked by Rule 23 — official documentation of another vendor or standards body |
| `community` | not ranked by Rule 23 — community source (forum, issue tracker, third-party project) |
| `reasoning` | Rule 23 priority 8 — reasoning/calculation from cited inputs |

## Summary

| Topic | Claims | CONFIRMED | CORRECTED | UNVERIFIABLE | REFUTED |
|---|---|---|---|---|---|
| [A — TC358743 hardware](#topic-a) | 50 | 47 | 3 | 0 | 0 |
| [B — tc358743 Linux driver and Device Tree binding](#topic-b) | 50 | 46 | 4 | 0 | 0 |
| [C — Raspberry Pi CSI-2 receive path (Pi 4 / CM4 / Pi 5 / CM5)](#topic-c) | 53 | 49 | 4 | 0 | 0 |
| [D — Video encoding on Pi 4/CM4 and Pi 5/CM5](#topic-d) | 54 | 53 | 1 | 0 | 0 |
| [E — Buildroot and kernel configuration](#topic-e) | 53 | 50 | 3 | 0 | 0 |
| [F — Blackmagic ATEM integration and streaming protocols](#topic-f) | 46 | 40 | 6 | 0 | 0 |
| [G — Raspberry Pi OS and official image tooling](#topic-g) | 71 | 63 | 8 | 0 | 0 |
| [H — H.265/HEVC software encoding and transport (research of 2026-10-08)](#topic-h) | 43 | 40 | 3 | 0 | 0 |
| [I — HDMI audio capture path: TC358743 → I2S → ALSA → AAC/Opus (research of 2026-10-08)](#topic-i) | 47 | 46 | 1 | 0 | 0 |
| **Total** | 467 | 434 | 33 | 0 | 0 |

Open questions and gaps raised by the research are consolidated in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md); the raw lists are kept in the JSON file linked above.

---

## Topic A

**TC358743 hardware** — <a id="topic-a"></a>50 claims.

### A-01

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** TC358743XBG is an HDMI-RX to MIPI CSI-2-TX bridge. The current public Toshiba datasheet is the combined TC358743XBG/TC9590XBG document, Rev. 2.20 dated 2026-05-11, 20 pages.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 (2026-05-11) — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** p.1: 'The HDMI-RX to MIPI CSI-2-TX is a bridge device that converts HDMI stream to MIPI CSI-2 TX.' Footer: '1 / 20 2026-05-11 Rev. 2.20'

### A-02

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The datasheet lists the HDMI receiver as HDMI-RX 1.4 and cites HDMI Specification Version 1.4a (March 4, 2010) as a reference. The receiver does not support Audio Return Channel or HDMI Ethernet Channel; the datasheet's wording is 'Audio Return Path and HDMI Ethernet Channels'.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.8: '+ HDMI-RX 1.4'; '+ Does not support Audio Return Path and HDMI Ethernet Channels'; REFERENCES p.6: 'High-Definition Multimedia Interface Specification Version 1.4a March 4, 2010'

### A-03

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The public datasheet lists HDCP support only as 'Support HDCP (optional)'. The block diagram shows HDCP eFuse Keys, an Authentication Engine and an HDCP Decryption Engine. The public datasheet does not state the HDCP version.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.8: '- Support HDCP (optional)'; Figure 1.1 blocks 'HDCP eFuse Keys', 'Authentication Engine', 'HDCP Decryption Engine'; Rev 1.0 history: 'Added comment to HDCP in Features.'

### A-04

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The Linux tc358743 driver always configures HDCP as disabled on device-tree platforms. It sets pdata.enable_hdcp = false, which sets the MASK_MANUAL_AUTHENTICATION bit in HDCP_MODE, so the bridge does not do automatic HDCP authentication.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2057 'state->pdata.enable_hdcp = false;'; tc358743_set_hdmi_hdcp(): else branch 'i2c_wr8_and_or(sd, HDCP_MODE, ~MASK_MANUAL_AUTHENTICATION, MASK_MANUAL_AUTHENTICATION);'

### A-05

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The datasheet's CSI-2 transmitter supports up to 4 data lanes and up to 1 Gbps per data lane. Video, audio and InfoFrame data can be sent over CSI-2.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.8: '+ MIPI CSI-2 compliant', '+ Supports up to 1 Gbps per data lane', '- Video, Audio and InfoFrame data can be transmit over MIPI CSI-2', '+ Supports up to 4 data lanes'

### A-06

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The datasheet cites MIPI D-PHY specification v01-00-00 (May 14, 2009) and MIPI CSI-2 Version 1.01 (Revision Nov 2010) as its interface standards. The Toshiba product brief gives the CSI-2 version as 'Version 1.01 Revision 0.04 – 2 April 2009' and says 1, 2, 3 or 4 data lanes are configurable.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20; Toshiba Product Brief 'TC358743 Camera Serial Interface Converter Chipset (HDMI to MIPI)' (mirror on Octopart) — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Verifier's best source:** <https://datasheet.octopart.com/TC358743XBG(EL)-Toshiba-datasheet-13695243.pdf>
- **Evidence:** REFERENCES p.6: 'MIPI_D-PHY_specification_v01-00-00, May 14, 2009', 'Camera Serial Interface 2 (CSI-2) Version 1.01 Revision Nov 2010'; Product brief p.1: 'configurable 1, 2, 3, or 4 data lanes with lane speeds of up to 1 Gbps per lane'

### A-07

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The datasheet gives the maximum HDMI clock (TMDS) speed as 165 MHz. Supported video input is up to 1080P at 60 fps: RGB and YCbCr444 at 24 bpp, and YCbCr422 at 24 bpp.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.8: '- Video Formats Support (Up to 1080P @60fps) RGB, YCbCr444: 24-bpp @60fps YCbCr422 24-bpp @60fps'; '- Maximum HDMI clock speed: 165 MHz'

### A-08

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver's DV timings capability (tc358743_timings_cap) allows width 640–1920, height 350–1200 and pixel clock 13,000,000–165,000,000 Hz. It covers the CEA-861, DMT, GTF and CVT standards and advertises progressive, reduced-blanking and custom timings. It does not set the interlaced capability. A driver comment says the min/max width and height are unknown.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:75-80 '/* Pixel clock from REF_01 p. 20. Min/max height/width are unknown */ V4L2_INIT_BT_TIMINGS(640, 1920, 350, 1200, 13000000, 165000000, ... V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_REDUCED_BLANKING | V4L2_DV_BT_CAP_CUSTOM)'

### A-09

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver offers exactly two CSI-2 media bus codes: MEDIA_BUS_FMT_RGB888_1X24 (index 0, the default set at probe, colorspace SRGB) and MEDIA_BUS_FMT_UYVY8_1X16 (index 1, colorspace SMPTE170M, CONFCTL YCBCRFMT = MASK_YCBCRFMT_422_8_BIT).
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743_enum_mbus_code: 'case 0: code->code = MEDIA_BUS_FMT_RGB888_1X24; case 1: code->code = MEDIA_BUS_FMT_UYVY8_1X16;'; probe line 2247 'state->mbus_fmt_code = MEDIA_BUS_FMT_RGB888_1X24;'

### A-10

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** tc358743_regs.h defines four CONFCTL (0x0004) YCBCRFMT output-format codes: MASK_YCBCRFMT_444 = 0x0000, MASK_YCBCRFMT_422_12_BIT = 0x0040, MASK_YCBCRFMT_COLORBAR = 0x0080 and MASK_YCBCRFMT_422_8_BIT = 0x00c0. The driver does not use the 12-bit 4:2:2 or colour-bar modes.
- **Source:** torvalds/linux master drivers/media/i2c/tc358743_regs.h (identical in raspberrypi/linux rpi-6.18.y) — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743_regs.h>
- **Evidence:** tc358743_regs.h:30 '#define CONFCTL 0x0004'; :40-44 'MASK_YCBCRFMT 0x00c0 / _444 0x0000 / _422_12_BIT 0x0040 / _COLORBAR 0x0080 / _422_8_BIT 0x00c0'

### A-11

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** Audio output is either I2S or TDM; the two share pins. I2S is a single stereo data lane, controller (master) clock mode only, 16/18/20/24-bit data, left- or right-justified MSB first, 32-bit time slots only, with a 256fs oversampling clock output. TDM is fixed at 8 channels with 32-bit slots, controller clock mode only.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.8-9: 'Either I2S or TDM Audio interface available (pins are multiplexed)'; '+ Single data lane for stereo data'; '+ Support Controller Clock mode only'; '+ Support 32 bit-wide time-slot only'; '+ Output Audio Oversampling clock (256fs)'; TDM '+ Fixed to 8 channels'

### A-12

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The audio pins are A_SCK (I2S/TDM bit clock out), A_WFS (I2S word clock or TDM frame sync out), A_SD (data out) and A_OSCK (oversampling clock out). All four are outputs in the VDDIO2 (1.8 V or 3.3 V) domain.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, Table 3.1 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Table 3.1: 'A_SCK O L N I2S/TDM Bit Clock signal VDDIO2 1.8 V or 3.3 V'; 'A_WFS ... I2S Word Clock or TDM Frame Sync signal'; 'A_SD ... I2S/TDM data signal'; 'A_OSCK ... Audio Oversampling Clock'

### A-13

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** CONFCTL selects the audio output path with MASK_AUDOUTSEL (0x0018): CSI = 0x0000, I2S = 0x0010, TDM = 0x0018. The driver always configures 2-channel I2S output (MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S, SDO_MODE1 = MASK_SDO_FMT_I2S).
- **Source:** raspberrypi/linux rpi-6.18.y tc358743_regs.h and tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743_regs.h:46-49 'MASK_AUDOUTSEL 0x0018 / _CSI 0x0000 / _I2S 0x0010 / _TDM 0x0018'; tc358743.c:914 'i2c_wr8(sd, SDO_MODE1, MASK_SDO_FMT_I2S);' :918-919 'MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S | MASK_AUTOINDEX'

### A-14

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The TC358743 public datasheet gives I2C target speeds of Normal-mode 100 kHz and Fast-mode 400 kHz. The older Toshiba product brief also lists an 'ultra fast mode (2 MHz)'. The two documents conflict, and the newer datasheet does not list 2 MHz.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20; Toshiba TC358743 Product Brief — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Datasheet §2 p.8: '+ Support for Normal-mode (100 kHz) and Fast-mode (400 kHz)'; Product brief p.2 (https://datasheet.octopart.com/TC358743XBG(EL)-Toshiba-datasheet-13695243.pdf): 'normal mode (100 KHz), fast mode (400 KHz), and ultra fast mode (2 MHz)'

### A-15

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** The kernel DT binding example and the Raspberry Pi tc358743.dtsi both place the TC358743 at 7-bit I2C address 0x0f (reg = <0x0f>, node tc358743@f). The public TC358743XBG datasheet does not state the I2C address or any address-select strap.
- **Source:** torvalds/linux Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt; raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743.dtsi — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743.dtsi>
- **Evidence:** binding example: 'tc358743@f { compatible = "toshiba,tc358743"; reg = <0x0f>;'; tc358743.dtsi: 'tc358743: tc358743@f { compatible = "toshiba,tc358743"; reg = <0x0f>;' ; datasheet Rev 2.20 Table 3.1 lists I2C_SCL/I2C_SDA with no address info

### A-16

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver prints the I2C address shifted left by one (8-bit form). For the usual 7-bit address 0x0f, its probe and failure messages therefore show 0x1e.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2212-2213 'v4l2_info(sd, "not a TC358743 on address 0x%x\n", client->addr << 1);' and :2321 '"%s found @ 0x%x (%s)\n", client->name, client->addr << 1'

### A-17

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The TC358743 is accessed over I2C with 16-bit register addresses sent MSB first. Register values are little-endian: 8, 16 or 32-bit accesses go through le32_to_cpu/cpu_to_le32. A single driver write is limited to 130 bytes (I2C_MAX_XFER_SIZE = EDID_BLOCK_SIZE + 2), i.e. 2 address bytes plus 128 data bytes.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** i2c_rd(): 'u8 buf[2] = { reg >> 8, reg & 0xff };'; i2c_rdreg_err(): 'return le32_to_cpu(val);'; line 66 '#define I2C_MAX_XFER_SIZE  (EDID_BLOCK_SIZE + 2)'

### A-18

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** CHIPID is the 16-bit register at address 0x0000. Bits [15:8] hold the chip ID (MASK_CHIPID = 0xff00) and bits [7:0] the revision ID (MASK_REVID = 0x00ff). tc358743_regs.h defines no named constant for an expected chip-ID value.
- **Source:** torvalds/linux master drivers/media/i2c/tc358743_regs.h — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743_regs.h>
- **Evidence:** tc358743_regs.h:19-21 '#define CHIPID 0x0000 / #define MASK_CHIPID 0xff00 / #define MASK_REVID 0x00ff'

### A-19

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** At probe the driver reads CHIPID with i2c_rd16_err() and requires (chipid & MASK_CHIPID) == 0, meaning the chip-ID byte must read 0x00. If the read fails or the byte is non-zero it logs 'not a TC358743 on address 0x%x' and returns -ENODEV. The revision byte is not checked; tc358743_log_status only prints it.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2210-2215 'if (i2c_rd16_err(sd, CHIPID, &chipid) || (chipid & MASK_CHIPID) != 0) { v4l2_info(sd, "not a TC358743 on address 0x%x\n", ...); return -ENODEV; }'; log_status: '(i2c_rd16(sd, CHIPID) & MASK_REVID)'

### A-20

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Before the CHIPID read, probe also checks that the I2C adapter supports I2C_FUNC_SMBUS_BYTE_DATA and returns -EIO if it does not. The driver matches DT compatible 'toshiba,tc358743' and I2C id 'tc358743'.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2181-2182 'if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) return -EIO;'; :2369 '{ .compatible = "toshiba,tc358743" }'

### A-21

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** REFCLK (ball H5) is the reference clock input. The datasheet lists 27/26 MHz or 42 MHz, in the VDDIO2 domain (1.8 V or 3.3 V).
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, Table 3.1 / Figure 3.1 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Table 3.1: 'REFCLK I - N Reference clock input (27/26 MHz or 42 MHz) VDDIO2 1.8 V or 3.3 V'; Fig 3.1: 'H5 REFCLK'

### A-22

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver requires a clock named 'refclk' (devm_clk_get; on error it logs 'failed to get refclk'). It supports only 26000000, 27000000 or 42000000 Hz and sets pll_prd = refclk_hz / 6000000, giving 4, 4 or 7. A comment says the PLL input (refclk/pll_prd) must be 6–40 MHz. For any other rate it logs 'unsupported refclk rate: %u Hz', but the error path leaves ret at 0 (the value from clk_prepare_enable), so tc358743_probe_of() returns 0 and probe continues. If the chip answers the CHIPID read, tc358743_initial_setup() then calls tc358743_set_ref_clk(), which hits BUG_ON(refclk not 26/27/42 MHz). A wrong refclk rate therefore causes a kernel BUG rather than a clean probe failure.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2072-2085 '* It must be between 6 MHz and 40 MHz, lower frequency is better. */ switch (state->pdata.refclk_hz) { case 26000000: case 27000000: case 42000000: state->pdata.pll_prd = state->pdata.refclk_hz / 6000000;' ; default: 'unsupported refclk rate: %u Hz'
- **Original claim (before verification):** The driver requires a clock named 'refclk' and accepts only 26000000, 27000000 or 42000000 Hz. Any other rate fails probe with 'unsupported refclk rate'. It sets pll_prd = refclk_hz / 6000000 and notes that the PLL input (refclk/pll_prd) must be 6–40 MHz.
- **Verifier note:** Lines 2049-2085 show that 'ret = clk_prepare_enable(refclk)' succeeds with ret = 0, and the switch default runs 'dev_err(...); goto disable_clk;' without setting ret. disable_clk falls through to 'return ret' (0). tc358743_probe() only returns early when err != 0. Lines 717-719 contain the BUG_ON in tc358743_set_ref_clk(), which is reached from tc358743_initial_setup() at probe line 2267. The torvalds master copy has the same code. The rest of the original claim (accepted rates, pll_prd, the 6–40 MHz comment) is correct.

### A-23

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux
- **Fact:** With integer arithmetic, pll_fbd = (bps_pr_lane / refclk_hz) * pll_prd, and the lane rate is refclk/pll_prd*pll_fbd. Only a 27 MHz refclk gives exactly 594 or 972 Mbps (prd 4, fbd 88 or 144). At 26 MHz, 972 Mbps comes out as 962 Mbps (fbd 148) and 594 as 572 Mbps (fbd 88). At 42 MHz (prd 7), 972 becomes 966 Mbps (fbd 161).
- **Source:** Calculation from raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c probe_of PLL code — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** Inputs: tc358743.c:2079 'pll_prd = refclk_hz / 6000000'; :2101-2102 'pll_fbd = bps_pr_lane / state->pdata.refclk_hz * state->pdata.pll_prd'; tc358743.h 'Bps pr lane is (refclk_hz / pll_prd) * pll_fbd'. E.g. 972e6/26e6=37 (int) *4=148; 26e6/4*148=962e6

### A-24

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver needs at least one link-frequency in the DT endpoint and takes bps per lane = 2 × link_frequencies[0], which must be 62.5 Mbps–1 Gbps. It has register timing tables only for 594 Mbps (297 MHz link) and 972 Mbps (486 MHz link). Any other rate logs 'untested bps per lane' and falls back to the 594 Mbps values.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2088-2093 'The CSI bps per lane must be between 62.5 Mbps and 1 Gbps. ... 972 Mbps allows 1080P50 UYVY over 2-lane. */ bps_pr_lane = 2 * endpoint.link_frequencies[0];'; :2109-2112 'default: dev_warn(dev, "untested bps per lane..."); fallthrough; case 594000000U:'

### A-25

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver picks the number of CSI-2 lanes at runtime as DIV_ROUND_UP(width × height × fps × bpp, bps_per_lane). Only active pixels count, bpp = 16 for UYVY8_1X16 and 24 otherwise, and bps_per_lane = refclk/pll_prd*pll_fbd. It writes the lane count into the NOL field of CSI_CONTROL (0x040C) indirectly, through the CSI_CONFW write port (0x0500) with MASK_MODE_SET | MASK_ADDRESS_CSI_CONTROL. It disables unused data lanes with D1W/D2W/D3W_CNTRL LANEDISABLE and enables HSTXVREGEN only for lanes in use. The result is not clamped to the DT data-lanes count; it is reported to the receiver via get_mbus_config (csi_lanes_in_use).
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c and tc358743_regs.h — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:795-800 'bits_pr_pixel = (... MEDIA_BUS_FMT_UYVY8_1X16) ? 16 : 24; u32 bps = bt->width * bt->height * fps(bt) * bits_pr_pixel; ... return DIV_ROUND_UP(bps, bps_pr_lane);'; regs.h:137-146 'CSI_CONTROL 0x040C ... MASK_NOL_1 0x0000 ... MASK_NOL_4 0x0006'
- **Original claim (before verification):** The driver picks the number of CSI-2 lanes at runtime as DIV_ROUND_UP(width × height × fps × bpp, bps_per_lane), with bpp = 16 for UYVY8_1X16 and 24 otherwise. It programs CSI_CONTROL (0x040C) NOL to 1–4 lanes and disables the unused lanes.
- **Verifier note:** The formula is at lines 790-800. tc358743_set_csi() (803-866) writes CSI_CONFW, never 0x040C directly. regs.h: CSI_CONTROL 0x040C, MASK_NOL_1..4 = 0x0000/0x0002/0x0004/0x0006, CSI_CONFW 0x0500, MASK_ADDRESS_CSI_CONTROL 0x03000000. num_data_lanes from DT is only checked to be 1–4 at probe and is never compared with the computed lane count. get_mbus_config (1742-1754) reports num_data_lanes = csi_lanes_in_use and flags = 0, with the comment 'Support for non-continuous CSI-2 clock is missing in the driver'.

### A-26

- **Verdict:** `CORRECTED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux, Pi4
- **Fact:** At 972 Mbps per lane, the driver's lane formula gives: 1080p60 RGB888 (2.986 Gbps) needs 4 lanes; 1080p60 UYVY (1.991 Gbps) needs 3; 1080p50 UYVY (1.659 Gbps) fits in 2; 720p60 RGB888 (1.327 Gbps) needs 2. So on a 2-lane link at 972 Mbps (the RPi overlay default and the fastest rate with driver timing tables), 1080p60 needs more lanes than are available in either format. With link-frequency 500 MHz (999 Mbps actual at 27 MHz), the formula would return 2 lanes for 1080p60 UYVY, but that rate is 'untested', uses the 594 Mbps D-PHY timings and leaves almost no margin, so it is not a supported configuration.
- **Source:** Calculation from raspberrypi/linux rpi-6.18.y tc358743.c tc358743_num_csi_lanes_needed() — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** Inputs: formula of A-25; 1920*1080*60*24=2,985,984,000/972e6=3.07->4; 1920*1080*60*16=1,990,656,000/972e6=2.05->3; 1920*1080*50*16=1,658,880,000/972e6=1.71->2; driver comment '972 Mbps allows 1080P50 UYVY over 2-lane.'
- **Original claim (before verification):** At 972 Mbps per lane, the driver's lane formula gives: 1080p60 RGB888 (2.986 Gbps) needs 4 lanes; 1080p60 UYVY (1.991 Gbps) needs 3; 1080p50 UYVY (1.659 Gbps) fits in 2; 720p60 RGB888 (1.327 Gbps) needs 2. On a 2-lane CSI-2 link, 1080p60 is therefore not possible in either format.
- **Verifier note:** Recomputed: 1920*1080*60*24 = 2,985,984,000 / 972e6 = 3.07 → 4; *16 = 1,990,656,000 / 972e6 = 2.05 → 3; 1080p50*16 = 1,658,880,000 → 1.71 → 2; 1280*720*60*24 = 1,327,104,000 → 1.37 → 2. The driver comment says '972 Mbps allows 1080P50 UYVY over 2-lane.' Edge case: bps_pr_lane = 1e9 passes the range check, pll_fbd = 37*4 = 148, actual 999e6, and 1,990,656,000/999e6 = 1.993 → 2 lanes. The original 'not possible in either format' therefore needed the 972 Mbps qualifier.

### A-27

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743, Linux
- **Fact:** RESETN (ball G5) is the system reset input, active low, with a Schmitt input in the VDDIO2 domain. The DT binding example declares reset-gpios as GPIO_ACTIVE_LOW.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 Table 3.1; torvalds/linux toshiba,tc358743.txt — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Table 3.1: 'RESETN I - Sch System reset input, active low VDDIO2 1.8 V or 3.3 V'; Fig 3.1 'G5 RESETN'; binding: 'reset-gpios = <&gpio6 9 GPIO_ACTIVE_LOW>;'

### A-28

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The reset-gpios property is optional (devm_gpiod_get_optional, initially GPIOD_OUT_LOW, i.e. deasserted). When present, the driver waits 5–10 ms, asserts reset for 1–2 ms, deasserts it, then waits 20 ms before the CHIPID read. The 1–2 ms assert and 20 ms settle times come from the driver, not from public Toshiba timing specs.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743_gpio_reset(): 'usleep_range(5000, 10000); gpiod_set_value(state->reset_gpio, 1); usleep_range(1000, 2000); gpiod_set_value(state->reset_gpio, 0); msleep(20);'; :2141 'devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW)'

### A-29

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743, Linux
- **Fact:** INT (ball B3) is the interrupt output: active high, level-triggered, VDDIO2 domain, low at initialisation. The driver requests its IRQ with IRQF_TRIGGER_HIGH | IRQF_ONESHOT. The binding example uses IRQ_TYPE_LEVEL_HIGH.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 Table 3.1; raspberrypi/linux rpi-6.18.y tc358743.c — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Table 3.1: 'INT O L N Interrupt Output signal – active high (Level) VDDIO2'; Fig 3.1 'B3 INT'; tc358743.c:2279 'IRQF_TRIGGER_HIGH | IRQF_ONESHOT'; binding: 'interrupts = <5 IRQ_TYPE_LEVEL_HIGH>;'

### A-30

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** If the I2C client has no IRQ, the driver polls the interrupt status over I2C with a timer: every 1000 ms normally, or every 10 ms when a CEC adapter is registered. The Raspberry Pi tc358743.dtsi overlay has no 'interrupts' property, so it runs in polling mode.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743.c and arch/arm/boot/dts/overlays/tc358743.dtsi — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:68-69 'POLL_INTERVAL_CEC_MS 10 / POLL_INTERVAL_MS 1000'; :1609 'msecs = state->cec_adap ? POLL_INTERVAL_CEC_MS : POLL_INTERVAL_MS;'; tc358743.dtsi tc358743@f node contains compatible, reg, clocks, clock-names, port only (no interrupts)

### A-31

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** The chip has a 1 KB embedded EDID SRAM. Its EDID support follows E-EDID Release A Rev 1: a 128-byte EDID 1.3 base block plus a 128-byte CEA-861-D extension (version 3).
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.8: 'Release A, Revision 1 (Feb 9, 2000) First 128 bytes (EDID 1.3 structure) First E-EDID Extension: 128 bytes of CEA Extension version 3 (specified in CEA-861-D) Embedded 1K-byte SRAM (EDID_SRAM)'

### A-32

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The EDID RAM is mapped at registers 0x8C00–0x8FFF (1024 bytes), with length in EDID_LEN1/EDID_LEN2 (0x85CA/0x85CB). The driver accepts at most EDID_NUM_BLOCKS_MAX = 8 blocks of 128 bytes through VIDIOC_SUBDEV_S_EDID.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743_regs.h and tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** regs.h:779 '#define EDID_RAM 0x8C00'; :578-579 'EDID_LEN1 0x85CA / EDID_LEN2 0x85CB'; tc358743.c:63 '#define EDID_NUM_BLOCKS_MAX 8'; :1474 '0x8C00-0x8FFF: HDMIRX EDID-RAM (1024bytes)'

### A-33

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver does not enable hotplug until an EDID has been written. tc358743_enable_edid() returns early with 'no EDID -> no hotplug' when edid_blocks_written == 0. HPD is asserted only after an EDID is set and +5V is present, so a source sees no sink until userspace loads an EDID.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:446-447 'if (state->edid_blocks_written == 0) { v4l2_dbg(2, debug, sd, "%s: no EDID -> no hotplug\n", __func__);'; s_edid: 'if (tx_5v_power_present(sd)) tc358743_enable_edid(sd);'

### A-34

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The chip has a CEC pin (ball G1, VDDIO1 3.3 V domain) and an internal CEC block with registers at 0x0600–0x06FF (CECEN = 0x0600). Linux CEC support is optional, behind Kconfig CONFIG_VIDEO_TC358743_CEC (bool, depends on VIDEO_TC358743, selects CEC_CORE).
- **Source:** Toshiba datasheet Rev. 2.20 Table 3.1; raspberrypi/linux rpi-6.18.y drivers/media/i2c/Kconfig and tc358743_regs.h — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/Kconfig>
- **Evidence:** Datasheet Table 3.1: 'CEC CEC IO - N (Note 2) CEC signal VDDIO1 3.3 V'; Kconfig:1428-1431 'config VIDEO_TC358743_CEC bool "Enable Toshiba TC358743 CEC support" depends on VIDEO_TC358743 select CEC_CORE'; regs.h:190 'CECEN 0x0600'

### A-35

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Buildroot
- **Fact:** The main driver Kconfig symbol is CONFIG_VIDEO_TC358743 (tristate, 'Toshiba TC358743 decoder'). It depends on VIDEO_DEV && I2C and selects MEDIA_CONTROLLER, VIDEO_V4L2_SUBDEV_API, HDMI and V4L2_FWNODE. The module is named tc358743.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/Kconfig — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/Kconfig>
- **Evidence:** Kconfig:1415-1426 'config VIDEO_TC358743 tristate "Toshiba TC358743 decoder" depends on VIDEO_DEV && I2C select MEDIA_CONTROLLER select VIDEO_V4L2_SUBDEV_API select HDMI select V4L2_FWNODE ... module will be called tc358743.'

### A-36

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** Supply rails and recommended operating ranges: VDDC1/VDDC2 (core) 1.2 V (1.1–1.3 V); VDD_MIPI 1.2 V (1.1–1.3 V); AVDD12 (HDMI PHY) 1.2 V (1.15–1.25 V); AVDD33 (HDMI PHY) 3.3 V (3.135–3.465 V); VDDIO1 (HDMI digital IO) 3.3 V (3.0–3.6 V); VDDIO2 (digital IO) 1.8 V or 3.3 V (1.65–3.6 V); AVDD25 (APLL) 2.5 V (2.25–2.75 V).
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, §3 Table 3.1 (POWER) and §5.2 Operating Condition — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §5.2: 'VDDIO2 1.65 1.8 3.6 V'; 'VDDIO1 3.0 3.3 3.6'; 'VDDC 1.1 1.2 1.3'; 'VDD_MIPI 1.1 1.2 1.3'; 'AVDD25 2.25 2.5 2.75'; 'AVDD33 3.135 3.3 3.465'; 'AVDD12 1.15 1.2 1.25'

### A-37

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** Supply-noise limits are 0.1 V peak-to-peak in general, 0.08 V peak-to-peak on AVDD33 and 0.04 V peak-to-peak on AVDD12. REXT must connect to AVDD33 through a 2 kΩ ±1% resistor, and VPGM (eFuse programming supply) must be tied to ground.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §5.2: 'Supply Noise Voltage (Peak to Peak) VSN - - 0.1 V'; 'VSN33 - - 0.08 V'; 'VSN12 - - 0.04 V'; Table 3.1 Misc: 'REXT ... connect to AVDD33 with a 2 kΩ resistor (± 1%)'; 'VPGM ... eFuse program power supply, please tie to ground'

### A-38

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** Typical total power is 480.5 mW at 720P60 and 543.2 mW at 1080P60. Sleep mode (register 0x0002 = 0x0001) draws 108.9 µW. The core has two power domains: VDDC1 is always on, and VDDC2 can be shut off in deep sleep.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, Table 2.1 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §2 p.9: '+ 720P : 480.5 mW', '+ 1080P @60fps: 543.2 mW'; Table 2.1 'Sleep 0x0002 = 0x0001 ... 108.9 μW'; '- VDDC1 is always on power domain - VDDC2 can be shut-off during deep sleep mode'

### A-39

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** DDC_SCL, DDC_SDA and HPDI are 5 V tolerant and sit in the VDDIO1 3.3 V domain. HPDO (Hot Plug Detect output) is also in VDDIO1. EDID_SCL/EDID_SDA (EDID controller) and I2C_SCL/I2C_SDA (host control bus) are in the VDDIO2 (1.8 V or 3.3 V) domain.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, Table 3.1 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Table 3.1: 'DDC_SCL IO ... VDDIO1 3.3 V (Note 1)'; 'HPDI I ... VDDIO1 3.3 V (Note 1)'; 'I2C_SCL IO ... VDDIO2 1.8 V or 3.3 V'; 'Note 1: These IO are 5 V tolerant.'

### A-40

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** TC358743XBG package: P-TFBGA64-0606-0.65-001, 64 balls, 6.0 × 6.0 mm, 0.65 mm ball pitch, 1.2 mm maximum height, 76 mg typical. The TC9590XBG variant uses a different package, P-LFBGA64-0707-0.80-002 (7.0 × 7.0 mm, 0.80 mm pitch, 1.4 mm maximum).
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, §4 Tables 4.1/4.2 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** Table 4.1: 'Solder ball pitch 0.65 mm; Package dimension 6.0 × 6.0 mm2; Package height 1.2 mm (Max)'; Fig 4.1 'Weight: 76 mg (typ.)'; Table 4.2 TC9590XBG '0.80 mm', '7.0 × 7.0 mm2', '1.4 mm'

### A-41

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** TC358743
- **Fact:** TC358743XBG operating temperature is Ta −30 to +70 °C (ambient, with voltage applied); TC9590XBG is −40 to +85 °C. Storage temperature is −40 to +125 °C. The datasheet also notes that the product is weak against ESD.
- **Source:** Toshiba TC358743XBG/TC9590XBG Datasheet Rev. 2.20, §5.1/§5.2 — <https://toshiba.semicon-storage.com/info/TC358743XBG_datasheet_en_20260511.pdf?did=35655&prodName=TC358743XBG>
- **Evidence:** §5.2: 'Operating temperature (ambient temperature with voltage applied) for TC358743XBG Ta -30 25 70 °C'; '... for TC9590XBG Ta -40 25 85 °C'; §5.1 'Storage temperature Tstg -40 to +125 °C'; p.9 'This product is weak against ESD.'

### A-42

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Only a 20-page summary datasheet is public, with no register map, I2C address or AC timing. The Linux driver was written against non-public Toshiba documents: 'TC358743XBG (H2C), Functional Specification, Rev 0.60' (REF_01) and the register-settings spreadsheet 'TC358743XBG_HDMI-CSI_Tv11p_nm.xls' (REF_02). Several registers it uses are marked 'Not in REF_01'.
- **Source:** raspberrypi/linux rpi-6.18.y include/media/i2c/tc358743.h, tc358743_regs.h; Toshiba datasheet Rev. 2.20 TOC — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/include/media/i2c/tc358743.h>
- **Evidence:** tc358743.h:10-11 'REF_01 - Toshiba, TC358743XBG (H2C), Functional Specification, Rev 0.60 / REF_02 - Toshiba, TC358743XBG_HDMI-CSI_Tv11p_nm.xls'; regs.h e.g. ':551 DE_WIDTH_H_LO 0x8582 /* Not in REF_01 */'; datasheet TOC sections: Overview, Features, External Pins, Package, Electrical Characteristics, Revision History only

### A-43

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** TC358743, Linux
- **Fact:** A Raspberry Pi engineer (6by9) stated on the Raspberry Pi forums that Toshiba's FIFO computation formula is in a Toshiba datasheet covered by NDA. The driver's FIFO trigger level (fifo_level = 374) is an empirical value.
- **Source:** Raspberry Pi Forums 'TC358743 on rpi5' (t=359412); raspberrypi/linux rpi-6.18.y tc358743.c — <https://forums.raspberrypi.com/viewtopic.php?t=359412>
- **Evidence:** 6by9: 'I do have the magic formula to compute the FIFO in a datasheet from Toshiba, but it's covered under NDAs.'; tc358743.c:2061-2070 'the calculations required are buried in Toshiba's register settings spreadsheet... A value of 374 works with both those modes at 594Mbps, and with most modes on 972Mbps.' state->pdata.fifo_level = 374;

### A-44

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** In mainline Linux the TC358743 device-tree binding is still the free-text file Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt; there is no YAML schema (toshiba,tc358743.yaml returned HTTP 404 on master). It requires compatible 'toshiba,tc358743' and clocks/clock-names 'refclk'. reset-gpios, interrupts, data-lanes (<1 2 3 4> or <1 2>), clock-lanes <0>, clock-noncontinuous and link-frequencies (half the per-lane bit rate) are optional.
- **Source:** torvalds/linux master Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt — <https://raw.githubusercontent.com/torvalds/linux/master/Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt>
- **Evidence:** 'Required Properties: - compatible: value should be "toshiba,tc358743" - clocks, clock-names: ... the clock input is named "refclk".' 'link-frequencies: ... The frequency is half of the bps per lane due to DDR transmission.'

### A-45

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Pi4, CM4
- **Fact:** The Raspberry Pi tc358743.dtsi sets the refclk source (cam1_clk, or cam0_clk with the cam0 override) to clock-frequency = 27000000 and link-frequencies to 486000000 (972 Mbps per lane). It defaults to data-lanes <1 2>, uses clock-noncontinuous, and offers a '4lane' override for data-lanes <1 2 3 4>. cam1_clk/cam0_clk are 'fixed-clock' nodes in bcm270x.dtsi, so the Pi does not generate REFCLK: the bridge board must supply its own 27 MHz oscillator.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743.dtsi; arch/arm/boot/dts/broadcom/bcm270x.dtsi — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743.dtsi>
- **Evidence:** tc358743.dtsi: 'clock-frequency = <27000000>;', 'link-frequencies = /bits/ 64 <486000000>;', 'data-lanes = <1 2>;', '4lane = <0>, "-2+3-7+8";'; bcm270x.dtsi:156-159 'cam1_clk: cam1_clk { compatible = "fixed-clock"; #clock-cells = <0>; status = "disabled";'

### A-46

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** TC358743, Linux, Pi4, CM4
- **Fact:** The Raspberry Pi overlays README (rpi-6.18.y) says the tc358743 link-frequency parameter supports only 297000000 and 486000000 (default). It labels 297 MHz as '574Mbit/s', which contradicts the driver's 594 Mbps (2 × 297 MHz); the README value looks like a typo. It also says 4lane applies only to the Compute Module CAM1 connector.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/README — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** README 'Name: tc358743 ... 4lane Use 4 lanes (only applicable to Compute Modules CAM1 connector). link-frequency Set the link frequency. Only values of 297000000 (574Mbit/s) and 486000000 (972Mbit/s - default) are supported by the driver.'

### A-47

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** TC358743, Linux, Pi4, CM4
- **Fact:** The tc358743-audio overlay routes TC358743 I2S audio to Pi GPIOs: LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18 and DATA/SD to GPIO 20. The default ALSA card name is 'tc358743'.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/README — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** 'Name: tc358743-audio Info: Used in combination with the tc358743-fast overlay to route the audio from the TC358743 over I2S to the Pi. Wiring is LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18, and DATA/SD to GPIO 20.'

### A-48

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The drivers/media/i2c/tc358743.c and tc358743_regs.h in raspberrypi/linux rpi-6.18.y (the repo's default branch on 2026-10-06) match torvalds/linux master. The regs header is byte-identical, and the .c file differs only in the formatting of the i2c_device_id table entry.
- **Source:** diff of torvalds/linux master vs raspberrypi/linux rpi-6.18.y tc358743.c / tc358743_regs.h — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** GitHub API default_branch for raspberrypi/linux = 'rpi-6.18.y'; diff output: regs identical; tc358743.c only '< { .name = "tc358743" }, { }' vs '> { "tc358743" }, {}'

### A-49

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The chip also has an InfraRed (IR) input supporting the NEC protocol, but the Linux driver does not support IR: it holds the IR block in reset via SYSCTL MASK_IRRST.
- **Source:** Toshiba datasheet Rev. 2.20; raspberrypi/linux rpi-6.18.y tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** Datasheet §2: '+ Support NEC Infrared protocol.'; tc358743.c:943-947 '* IR is not supported by this driver. ... i2c_wr16_and_or(sd, SYSCTL, ~(MASK_IRRST | MASK_CECRST), (MASK_IRRST | MASK_CECRST));'

### A-50

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** TC358743
- **Fact:** The sister part TC358749XBG has a documented strap: its datasheet (Rev. 1.1, 2017-11-13) gives two I2C target addresses, 7'h0F and 7'h1F, selected by the INT pin at reset. The equivalent statement does not appear in the public TC358743XBG datasheet.
- **Source:** Toshiba TC358749XBG Datasheet Rev. 1.1 (third-party mirror datasheet.live) — <https://pdf.datasheet.live/40f47cca/toshiba.co.jp/TC358743XBG(EL,H4).html>
- **Evidence:** 'Support 2 I2C Slave Addresses (7'h0F & 7'h1F) selected through boot-strap pin (INT)' — page content identifies as TC358749XBG Rev. 1.1 2017-11-13, not TC358743XBG

---

## Topic B

**tc358743 Linux driver and Device Tree binding** — <a id="topic-b"></a>50 claims.

### B-01

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi4, CM4, Pi5, CM5
- **Fact:** The current default branch of github.com/raspberrypi/linux is rpi-6.18.y (as queried on 2026-10-06); all raspberrypi/linux claims below use this branch.
- **Source:** GitHub API repos/raspberrypi/linux — <https://api.github.com/repos/raspberrypi/linux>
- **Evidence:** "default_branch": "rpi-6.18.y", "pushed_at": "2026-10-06T09:07:16Z"

### B-02

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** drivers/media/i2c/tc358743.c (2385 lines) in raspberrypi/linux rpi-6.18.y is identical to torvalds/linux master except the i2c_device_id initializer style; drivers/media/i2c/tc358743_regs.h is byte-identical in both trees.
- **Source:** tc358743.c in torvalds/linux master vs raspberrypi/linux rpi-6.18.y (diff) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** diff output only: 2361,2362c2361,2362 < { .name = "tc358743" }, --- > { "tc358743" }, ; diff of tc358743_regs.h: no differences

### B-03

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The DT binding for the TC358743 is the plain-text file Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt (no YAML schema exists on torvalds master: toshiba,tc358743.yaml returns 404); the file is identical in rpi-6.18.y.
- **Source:** toshiba,tc358743.txt DT binding — <https://raw.githubusercontent.com/torvalds/linux/master/Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt>
- **Evidence:** Directory listing shows "toshiba,tc358743.txt"; raw .../toshiba,tc358743.yaml -> "404: Not Found"; diff torvalds vs rpi binding: SAME

### B-04

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Binding required properties: compatible = "toshiba,tc358743"; clocks + clock-names with the reference clock input named "refclk". 'reg' is not listed in the property text but the example uses reg = <0x0f>.
- **Source:** toshiba,tc358743.txt DT binding — <https://raw.githubusercontent.com/torvalds/linux/master/Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt>
- **Evidence:** Required Properties: - compatible: value should be "toshiba,tc358743" - clocks, clock-names: ... the clock input is named "refclk". Example: tc358743@f { ... reg = <0x0f>;

### B-05

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Binding optional properties: reset-gpios, interrupts, data-lanes (<1 2 3 4> or <1 2>), clock-lanes (<0>), clock-noncontinuous (boolean), link-frequencies (64-bit; equal to half the bps per lane). Example uses reset-gpios GPIO_ACTIVE_LOW, interrupts IRQ_TYPE_LEVEL_HIGH, link-frequencies /bits/ 64 <297000000>.
- **Source:** toshiba,tc358743.txt DT binding — <https://raw.githubusercontent.com/torvalds/linux/master/Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt>
- **Evidence:** Optional Properties: - reset-gpios ... - interrupts ... - data-lanes: should be <1 2 3 4> for four-lane operation, or <1 2> for two-lane operation - clock-lanes: should be <0> - clock-noncontinuous ... - link-frequencies: ... The frequency is half of the bps per lane due to DDR transmission.

### B-06

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Contrary to the binding marking them optional, tc358743_probe_of() requires an endpoint on port 0 that parses as V4L2_MBUS_CSI2_DPHY with data-lanes count 1..4 and at least one link-frequencies entry; otherwise probe fails with -EINVAL ('missing endpoint node', 'missing CSI-2 properties in endpoint', or 'invalid number of lanes').
- **Source:** tc358743.c tc358743_probe_of() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2021-2045: of_graph_get_endpoint_by_regs(dev->of_node, 0, -1) ... if (endpoint.bus_type != V4L2_MBUS_CSI2_DPHY || ...num_data_lanes == 0 || endpoint.nr_of_link_frequencies == 0) ... if (...num_data_lanes > 4) { dev_err(dev, "invalid number of lanes\n"); ret = -EINVAL;

### B-07

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver obtains the clock named "refclk" (devm_clk_get), enables it, and accepts only clk_get_rate() values of 26000000, 27000000 or 42000000 Hz; pll_prd = refclk_hz / 6000000 (integer), giving 4, 4 and 7 respectively; the PLL input (refclk/pll_prd) must be 6-40 MHz per the driver comment.
- **Source:** tc358743.c tc358743_probe_of() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2072-2085: "It must be between 6 MHz and 40 MHz, lower frequency is better." switch (state->pdata.refclk_hz) { case 26000000: case 27000000: case 42000000: state->pdata.pll_prd = state->pdata.refclk_hz / 6000000;

### B-08

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Only link_frequencies[0] is used: bps_pr_lane = 2 * link_frequencies[0], which must be within 62,500,000..1,000,000,000 bps or probe returns -EINVAL; pll_fbd = bps_pr_lane / refclk_hz * pll_prd (integer arithmetic), and the CSI lane rate is (refclk_hz / pll_prd) * pll_fbd.
- **Source:** tc358743.c tc358743_probe_of() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2092-2101: bps_pr_lane = 2 * endpoint.link_frequencies[0]; if (bps_pr_lane < 62500000U || bps_pr_lane > 1000000000U) {... "unsupported bps per lane" ret = -EINVAL; ... state->pdata.pll_fbd = bps_pr_lane / state->pdata.refclk_hz * state->pdata.pll_prd;

### B-09

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** D-PHY timing constants exist only for 594000000 and 972000000 bps/lane (link-frequency 297 MHz / 486 MHz); any other rate logs 'untested bps per lane' and falls through to the 594 Mbps constants (e.g. 972 Mbps: lineinitcnt 0x1b58, tclk_headercnt 0x2806, ths_headercnt 0x0806, twakeup 0x4268, ths_trailcnt 0x5).
- **Source:** tc358743.c tc358743_probe_of() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2103-2139: "FIXME: These timings are from REF_02 for 594 or 972 Mbps per lane" switch (bps_pr_lane) { default: dev_warn(dev, "untested bps per lane: %u bps\n", bps_pr_lane); fallthrough; case 594000000U: ... case 972000000U: state->pdata.lineinitcnt = 0x1b58;

### B-10

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux
- **Fact:** Because pll_fbd uses truncating integer division, only a 27 MHz refclk yields the exact nominal lane rates: 27 MHz -> 594/972 Mbps (fbd 88/144); 26 MHz -> 572/962 Mbps; 42 MHz -> 588/966 Mbps for link-frequency 297/486 MHz.
- **Source:** Calculation from tc358743.c PLL formulas — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** Inputs: pll_prd = refclk/6000000; pll_fbd = bps/refclk*prd (u32); rate = (refclk/prd)*fbd (L2079, L2100, L798). Python: 26e6/972e6 -> prd 4 fbd 148 -> 962000000; 42e6/972e6 -> prd 7 fbd 161 -> 966000000; 27e6/972e6 -> fbd 144 -> 972000000

### B-11

- **Verdict:** `CORRECTED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux
- **Fact:** Latent bug: for an unsupported refclk rate, tc358743_probe_of() logs 'unsupported refclk rate' and jumps to disable_clk with ret still 0 (from the successful clk_prepare_enable), so probe_of returns 0. Probe then continues. If the CHIPID read still succeeds, tc358743_initial_setup() -> tc358743_set_ref_clk() hits BUG_ON() (kernel BUG) instead of failing cleanly. The CHIPID read is likely to succeed on Raspberry Pi, where refclk is a 'fixed-clock' whose disable does not stop the physical oscillator.
- **Source:** Inference from tc358743.c probe_of / set_ref_clk — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2049 ret = clk_prepare_enable(refclk) (0 on success); L2081-2084 default: dev_err(..."unsupported refclk rate"); goto disable_clk; (ret not set); L2161 return ret; L717-719 BUG_ON(!(refclk_hz == 26000000 || == 27000000 || == 42000000));
- **Original claim (before verification):** Latent bug: for an unsupported refclk rate tc358743_probe_of() logs 'unsupported refclk rate' but jumps to disable_clk with ret still 0, so probe continues and tc358743_set_ref_clk() hits BUG_ON() (kernel BUG) instead of failing cleanly.
- **Verifier note:** Checked L2049 (ret = clk_prepare_enable), L2081-2084 (default: dev_err; goto disable_clk; ret is never assigned), L2156 clk_disable_unprepare and L2161 return ret. In tc358743_probe, only a non-zero err aborts. CHIPID read and update_controls run next, then tc358743_initial_setup calls tc358743_set_ref_clk (BUG_ON at L717-719) before any division by pll_prd, which is 0. The original claim left out the precondition that the I2C CHIPID read succeeds after refclk is 'disabled'. Whether the TC358743 answers I2C without REFCLK needs the datasheet or a hardware test. In practice, keep the DT clock-frequency to 26/27/42 MHz.

### B-12

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** With DT (no platform data) the driver hard-codes ddc5v_delay = DDC5V_DELAY_100_MS, enable_hdcp = false and FIFO trigger level fifo_level = 374 (comment: 16 fails at higher rates; 374 works for 720p60/1080p60 at 594 Mbps and most modes at 972 Mbps).
- **Source:** tc358743.c tc358743_probe_of() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2056-2070: state->pdata.ddc5v_delay = DDC5V_DELAY_100_MS; state->pdata.enable_hdcp = false; ... "A value of 374 works with both those modes at 594Mbps, and with most modes on 972Mbps." state->pdata.fifo_level = 374;

### B-13

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** reset-gpios is optional (devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW)); if present, after refclk is enabled and before chip-ID read the driver waits 5-10 ms, asserts reset (logical 1) for 1-2 ms, deasserts (logical 0) and waits 20 ms. Polarity comes from the DT flag (binding example: GPIO_ACTIVE_LOW).
- **Source:** tc358743.c tc358743_gpio_reset()/probe_of() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1998-2005: usleep_range(5000, 10000); gpiod_set_value(state->reset_gpio, 1); usleep_range(1000, 2000); gpiod_set_value(state->reset_gpio, 0); msleep(20); L2141-2150 devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW) ... if (state->reset_gpio) tc358743_gpio_reset(state);

### B-14

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Chip detection: the driver first requires I2C_FUNC_SMBUS_BYTE_DATA (-EIO otherwise), then reads 16-bit register CHIPID (0x0000); if the read fails or (chipid & 0xff00) != 0 it logs 'not a TC358743 on address 0x%x' (8-bit address) and returns -ENODEV.
- **Source:** tc358743.c tc358743_probe(); tc358743_regs.h — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2181-2182 if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) return -EIO; L2210-2215 if (i2c_rd16_err(sd, CHIPID, &chipid) || (chipid & MASK_CHIPID) != 0) { v4l2_info(sd, "not a TC358743 on address 0x%x\n", client->addr << 1); return -ENODEV; regs.h: CHIPID 0x0000, MASK_CHIPID 0xff00

### B-15

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Probe order: SMBus check -> devm_kzalloc -> platform data or tc358743_probe_of (clock, endpoint, PLL params, GPIO reset) -> v4l2_i2c_subdev_init -> CHIPID check -> ctrl handler (3 ctrls) + update -> media_entity_pads_init (1 pad) -> mbus code RGB888_1X24 -> mutex/delayed hotplug work -> CEC adapter alloc (if CONFIG_VIDEO_TC358743_CEC) -> tc358743_initial_setup -> s_dv_timings(V4L2_DV_BT_CEA_640X480P59_94) -> set_csi_color_space -> init_interrupts -> IRQ or poll timer -> cec_register_adapter -> enable_interrupts(+5V present) -> INTMASK -> v4l2_ctrl_handler_setup -> v4l2_async_register_subdev -> InfoFrame regs -> debugfs.
- **Source:** tc358743.c tc358743_probe() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2170-2324: static struct v4l2_dv_timings default_timing = V4L2_DV_BT_CEA_640X480P59_94; ... tc358743_initial_setup(sd); tc358743_s_dv_timings(sd, 0, &default_timing); tc358743_set_csi_color_space(sd); tc358743_init_interrupts(sd); ... err = v4l2_async_register_subdev(sd);

### B-16

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The control handler holds exactly 3 controls: V4L2_CID_DV_RX_POWER_PRESENT (0..1), TC358743_CID_AUDIO_SAMPLING_RATE = V4L2_CID_USER_TC358743_BASE+0 ("Audio sampling rate", INTEGER 0..768000, read-only) and TC358743_CID_AUDIO_PRESENT = V4L2_CID_USER_TC358743_BASE+1 ("Audio present", BOOLEAN, read-only); V4L2_CID_USER_TC358743_BASE = V4L2_CID_USER_BASE + 0x1080. No V4L2_CID_LINK_FREQ or V4L2_CID_PIXEL_RATE control is registered.
- **Source:** tc358743.c; include/media/i2c/tc358743.h; include/uapi/linux/v4l2-controls.h — <https://raw.githubusercontent.com/torvalds/linux/master/include/media/i2c/tc358743.h>
- **Evidence:** tc358743.c L2218 v4l2_ctrl_handler_init(&state->hdl, 3); L1973-1993 .name = "Audio sampling rate" ... .max = 768000 ... V4L2_CTRL_FLAG_READ_ONLY; tc358743.h: #define TC358743_CID_AUDIO_SAMPLING_RATE (V4L2_CID_USER_TC358743_BASE + 0); v4l2-controls.h:155 #define V4L2_CID_USER_TC358743_BASE (V4L2_CID_USER_BASE + 0x1080)

### B-17

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux
- **Fact:** Numerically, TC358743_CID_AUDIO_SAMPLING_RATE = 0x00981980 and TC358743_CID_AUDIO_PRESENT = 0x00981981.
- **Source:** Calculation from v4l2-controls.h — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/include/uapi/linux/v4l2-controls.h>
- **Evidence:** V4L2_CTRL_CLASS_USER 0x00980000; V4L2_CID_BASE (V4L2_CTRL_CLASS_USER | 0x900) = 0x00980900; V4L2_CID_USER_BASE = V4L2_CID_BASE; +0x1080 = 0x00981980; +1 = 0x00981981

### B-18

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The subdev exposes one media pad (index 0, MEDIA_PAD_FL_SOURCE), entity function MEDIA_ENT_F_VID_IF_BRIDGE, and flags V4L2_SUBDEV_FL_HAS_DEVNODE | V4L2_SUBDEV_FL_HAS_EVENTS (so a /dev/v4l-subdevN node is available when the bridge driver registers subdev nodes).
- **Source:** tc358743.c tc358743_probe() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2207 sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE | V4L2_SUBDEV_FL_HAS_EVENTS; L2241-2243 state->pad.flags = MEDIA_PAD_FL_SOURCE; sd->entity.function = MEDIA_ENT_F_VID_IF_BRIDGE; err = media_entity_pads_init(&sd->entity, 1, &state->pad);

### B-19

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** If the I2C client has an IRQ the driver uses devm_request_threaded_irq(..., IRQF_TRIGGER_HIGH | IRQF_ONESHOT, "tc358743"); otherwise it polls the interrupt status over I2C from a timer that first fires after 1000 ms and re-arms every 10 ms when a CEC adapter exists (POLL_INTERVAL_CEC_MS) or every 1000 ms otherwise (POLL_INTERVAL_MS).
- **Source:** tc358743.c IRQ / poll timer — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L68-69 #define POLL_INTERVAL_CEC_MS 10 / #define POLL_INTERVAL_MS 1000; L1609 msecs = state->cec_adap ? POLL_INTERVAL_CEC_MS : POLL_INTERVAL_MS; L2275-2290 IRQF_TRIGGER_HIGH | IRQF_ONESHOT ... state->timer.expires = jiffies + msecs_to_jiffies(POLL_INTERVAL_MS);

### B-20

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** Kconfig: VIDEO_TC358743 is tristate, depends on VIDEO_DEV && I2C and selects MEDIA_CONTROLLER, VIDEO_V4L2_SUBDEV_API, HDMI, V4L2_FWNODE; CEC is a separate bool VIDEO_TC358743_CEC (selects CEC_CORE). RPi bcm2711_defconfig and bcm2712_defconfig set CONFIG_VIDEO_TC358743=m and do not set CONFIG_VIDEO_TC358743_CEC.
- **Source:** drivers/media/i2c/Kconfig; arch/arm64/configs/bcm2711_defconfig, bcm2712_defconfig (rpi-6.18.y) — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/Kconfig>
- **Evidence:** Kconfig L1370-1389: config VIDEO_TC358743 tristate ... select MEDIA_CONTROLLER select VIDEO_V4L2_SUBDEV_API select HDMI select V4L2_FWNODE ... config VIDEO_TC358743_CEC bool ... select CEC_CORE; bcm2711_defconfig:1077 CONFIG_VIDEO_TC358743=m; bcm2712_defconfig:1079 CONFIG_VIDEO_TC358743=m; grep TC358743_CEC in both: no match

### B-21

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The driver never raises HDMI hotplug (HPD_CTL 0x8544 bit HPD_OUT0) unless an EDID has been written. tc358743_enable_edid() returns early ('no EDID -> no hotplug') when edid_blocks_written == 0, which is the value after probe. With an EDID and +5V present, delayed work raises HPD after HZ/7 jiffies. The driver comment says 143 ms; with integer division it is 140 ms at the RPi defconfig default HZ=250 (142 ms at HZ=1000). Per the driver comment, DDC access to the EDID is gated by HPD.
- **Source:** tc358743.c tc358743_enable_edid()/delayed_work_enable_hotplug — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L446-456: if (state->edid_blocks_written == 0) { v4l2_dbg(2, debug, sd, "%s: no EDID -> no hotplug\n", __func__); ... return; } /* Enable hotplug after 143 ms. DDC access to EDID is also enabled when hotplug is enabled. */ schedule_delayed_work(&state->delayed_work_enable_hotplug, HZ / 7);
- **Original claim (before verification):** The driver never raises HDMI hotplug (HPD_CTL 0x8544 bit HPD_OUT0) unless an EDID has been written: tc358743_enable_edid() returns early ('no EDID -> no hotplug') when edid_blocks_written == 0 (the value after probe); with an EDID and +5V present, HPD is raised by delayed work after HZ/7 (~143 ms). DDC access to the EDID is gated by HPD.
- **Verifier note:** L446-456 checked. The only HPD_OUT0 set is in tc358743_delayed_work_enable_hotplug (L404), which is scheduled only from enable_edid (L456). regs.h: HPD_CTL 0x8544, MASK_HPD_OUT0 0x01. RPi defconfigs set no CONFIG_HZ and kernel/Kconfig.hz defaults to HZ_250, so 250/7 = 35 jiffies = 140 ms. That is the only correction, to the '~143 ms' figure. The DDC gating comes from a driver comment ('See register DDC_CTL') and has not been checked against the datasheet.

### B-22

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** set_edid (VIDIOC_SUBDEV_S_EDID): pad must be 0 and start_block 0 (-EINVAL otherwise); more than 8 blocks returns -E2BIG with blocks set to 8; the CEC physical address is validated; HPD is dropped, EDID_LEN1/2 written, blocks copied to EDID RAM at 0x8C00 in 128-byte I2C writes; blocks = 0 clears the EDID (HPD stays low); HPD is re-enabled only if +5V is present. get_edid returns -ENODATA when no EDID is stored.
- **Source:** tc358743.c tc358743_s_edid()/tc358743_g_edid() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1896-1930: if (edid->start_block != 0) return -EINVAL; if (edid->blocks > EDID_NUM_BLOCKS_MAX) { edid->blocks = EDID_NUM_BLOCKS_MAX; return -E2BIG; } ... tc358743_disable_edid(sd); ... if (edid->blocks == 0) { state->edid_blocks_written = 0; return 0; } ... if (tx_5v_power_present(sd)) tc358743_enable_edid(sd); L1863-1864 return -ENODATA; L63 EDID_NUM_BLOCKS_MAX 8

### B-23

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** On a +5V (DDC) interrupt: if +5V is present the driver calls tc358743_enable_edid(); if +5V is removed it masks all but the DDC interrupt, drops HPD, zeroes the stored timings, erases BKSV and updates the controls.
- **Source:** tc358743.c tc358743_hdmi_sys_int_handler() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1313-1321: if (tx_5v) { tc358743_enable_edid(sd); } else { tc358743_enable_interrupts(sd, false); tc358743_disable_edid(sd); memset(&state->timings, 0, sizeof(state->timings)); tc358743_erase_bksv(sd); tc358743_update_controls(sd); }

### B-24

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Linux, TC358743
- **Fact:** v4l2-ctl sets an EDID with '--set-edid pad=<pad>[,type=<type>|file=<file>][,format=<fmt>][modifiers]' (built-in type 'hdmi' = CTA-861 with HDMI support up to 1080p60; file format hex by default or raw) and clears it with '--clear-edid <pad>'.
- **Source:** v4l-utils utils/v4l2-ctl/v4l2-ctl-edid.cpp (help text) — <https://git.linuxtv.org/v4l-utils.git/plain/utils/v4l2-ctl/v4l2-ctl-edid.cpp>
- **Evidence:** "  --set-edid pad=<pad>[,type=<type>|file=<file>][,format=<fmt>][modifiers]\n" ... "hdmi: CTA-861 with HDMI support up to 1080p60\n" ... "  --clear-edid <pad> clear the EDID for the input index <pad>.\n"

### B-25

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, Linux
- **Fact:** On Pi 4/CM4 the downstream bcm2835-unicam driver forwards VIDIOC_S_EDID/G_EDID, S/G/QUERY/ENUM_DV_TIMINGS and DV_TIMINGS_CAP from the video node to the tc358743 subdev only in legacy (non-Media-Controller) mode. That is the case with compatible "brcm,bcm2835-unicam-legacy" (the tc358743 overlay default), media_controller module param 0 and no brcm,media-controller property. In MC mode the video node uses unicam_mc_ioctl_ops, which has no EDID or DV-timings ioctls, so these must be issued on the tc358743 subdev node. The downstream RP1 CFE driver used on Pi 5 (drivers/media/platform/raspberrypi/rp1_cfe/cfe.c) contains no EDID or dv_timings handling, so on Pi 5 they are always issued on the subdev node.
- **Source:** bcm2835-unicam.c and rp1_cfe/cfe.c (rpi-6.18.y) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/platform/bcm2835/bcm2835-unicam.c>
- **Evidence:** bcm2835-unicam.c L1505 return v4l2_subdev_call(dev->sensor, pad, set_edid, edid); L1763-1777 .vidioc_s_edid = unicam_s_edid, ... .vidioc_query_dv_timings = unicam_query_dv_timings; grep -n 'edid\|dv_timings' rp1_cfe/cfe.c -> no matches
- **Original claim (before verification):** On Pi 4/CM4 the downstream bcm2835-unicam driver (drivers/media/platform/bcm2835/) forwards VIDIOC_S_EDID/G_EDID, S_DV_TIMINGS and QUERY_DV_TIMINGS from the video node to the sensor subdev; the downstream RP1 CFE driver used on Pi 5 (drivers/media/platform/raspberrypi/rp1_cfe/cfe.c) contains no EDID or dv_timings handling, so on Pi 5 these must be issued on the tc358743 subdev node.
- **Verifier note:** bcm2835-unicam.c L1500-1513 and L1639-1709 forward to the subdev. Those handlers are in unicam_ioctl_ops (L1763-1779), but L2948 picks 'vdev->ioctl_ops = unicam->mc_api ? &unicam_mc_ioctl_ops : &unicam_ioctl_ops'. unicam_mc_ioctl_ops (L1994-2020) has no edid/dv_timings entries. mc_api is set at L3300-3307. In legacy mode subdev nodes are registered read-only (L3136), so on the subdev node VIDIOC_SUBDEV_S_DV_TIMINGS returns -EPERM (v4l2-subdev.c L971-972) while S_EDID is not blocked. grep for edid/dv_timings in rp1_cfe/cfe.c finds nothing.

### B-26

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** DV timings capability: type V4L2_DV_BT_656_1120, width 640..1920, height 350..1200, pixel clock 13,000,000..165,000,000 Hz, standards CEA-861/DMT/GTF/CVT, capabilities PROGRESSIVE | REDUCED_BLANKING | CUSTOM (no INTERLACED). Only the pixel clock limit is sourced (REF_01 p.20); min/max width/height are marked unknown.
- **Source:** tc358743.c tc358743_timings_cap — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L75-81: /* Pixel clock from REF_01 p. 20. Min/max height/width are unknown */ V4L2_INIT_BT_TIMINGS(640, 1920, 350, 1200, 13000000, 165000000, V4L2_DV_BT_STD_CEA861 | V4L2_DV_BT_STD_DMT | V4L2_DV_BT_STD_GTF | V4L2_DV_BT_STD_CVT, V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_REDUCED_BLANKING | V4L2_DV_BT_CAP_CUSTOM)

### B-27

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux
- **Fact:** Interlaced input is not usable: the driver detects interlace (VI_STATUS1 bit S_V_INTERLACE) but the cap lacks V4L2_DV_BT_CAP_INTERLACED, so v4l2_valid_dv_timings() rejects it and query_dv_timings/s_dv_timings return -ERANGE; get_fmt always reports V4L2_FIELD_NONE.
- **Source:** Inference from tc358743.c and v4l2-dv-timings.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/v4l2-core/v4l2-dv-timings.c>
- **Evidence:** tc358743.c L361-362 bt->interlaced = i2c_rd8(sd, VI_STATUS1) & MASK_S_V_INTERLACE ? V4L2_DV_INTERLACED : ...; L1722-1726 return -ERANGE; L1811 format->format.field = V4L2_FIELD_NONE; v4l2-dv-timings.c L163 (bt->interlaced && !(caps & V4L2_DV_BT_CAP_INTERLACED)) -> return false

### B-28

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Detected timings are derived, not measured per field: width/height from DE_WIDTH registers, hsync/vsync fields carry total horizontal/vertical blanking (porches left 0), fps = round(10000 / FV_CNT) (frame interval in 0.1 ms units, requires SYS_FREQ set correctly) and pixelclock = H_SIZE * (V_SIZE/2) * fps, so fractional rates (e.g. 59.94) are reported as integer-fps pixel clocks.
- **Source:** tc358743.c tc358743_get_detected_timings() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L372-383: /* frame interval in milliseconds * 10 * Require SYS_FREQ0 and SYS_FREQ1 are precisely set */ ... fps = (frame_interval > 0) ? DIV_ROUND_CLOSEST(10000, frame_interval) : 0; bt->vsync = frame_height - height; bt->hsync = frame_width - width; bt->pixelclock = frame_width * frame_height * fps;

### B-29

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** query_dv_timings returns -EINVAL for pad != 0, -ENOLINK if HPD is low or no TMDS signal, -ENOLCK if no stable sync, and -ERANGE if detected timings fall outside the capability.
- **Source:** tc358743.c get_detected_timings / query_dv_timings — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L347-358: /* if HPD is low, ignore any video */ if (!(i2c_rd8(sd, HPD_CTL) & MASK_HPD_OUT0)) return -ENOLINK; if (no_signal(sd)) {... return -ENOLINK; } if (no_sync(sd)) {... return -ENOLCK; } L1711-1726 ... return -ERANGE;

### B-30

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** s_dv_timings: -EINVAL for pad != 0 or NULL; returns 0 without action if identical to current timings; -ERANGE if invalid for the capability; otherwise stores the timings, mutes the stream (enable_stream(false)), reprograms the PLL and CSI (recomputing the lane count). It does not compare the needed lane count with the DT data-lanes.
- **Source:** tc358743.c tc358743_s_dv_timings() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1663-1680: if (v4l2_match_dv_timings(&state->timings, timings, 0, false)) { ... return 0; } if (!v4l2_valid_dv_timings(timings, &tc358743_timings_cap, NULL, NULL)) { ... return -ERANGE; } state->timings = *timings; enable_stream(sd, false); tc358743_set_pll(sd); tc358743_set_csi(sd);

### B-31

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Lane count is computed dynamically as DIV_ROUND_UP(width * height * fps * bpp, (refclk_hz / pll_prd) * pll_fbd) with bpp = 16 for UYVY8_1X16 and 24 otherwise; the result is stored in csi_lanes_in_use and reported by get_mbus_config (type V4L2_MBUS_CSI2_DPHY, flags 0, num_data_lanes = csi_lanes_in_use) without clamping to DT data-lanes or to 4.
- **Source:** tc358743.c tc358743_num_csi_lanes_needed()/get_mbus_config() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L795-800: u32 bits_pr_pixel = (state->mbus_fmt_code == MEDIA_BUS_FMT_UYVY8_1X16) ? 16 : 24; u32 bps = bt->width * bt->height * fps(bt) * bits_pr_pixel; ... return DIV_ROUND_UP(bps, bps_pr_lane); L1748-1752 cfg->type = V4L2_MBUS_CSI2_DPHY; cfg->bus.mipi_csi2.flags = 0; cfg->bus.mipi_csi2.num_data_lanes = state->csi_lanes_in_use;

### B-32

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, Linux
- **Fact:** Both RPi CSI-2 receivers (downstream bcm2835-unicam on Pi 4/CM4 and downstream rp1_cfe on Pi 5/CM5) call get_mbus_config at stream start and fail with -EINVAL ('Device has requested %u data lanes, which is >%u configured in DT') when the bridge requests more lanes than the receiver endpoint's data-lanes.
- **Source:** bcm2835-unicam.c / rp1_cfe/cfe.c (rpi-6.18.y) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** bcm2835-unicam.c L2510-2517 dev->active_data_lanes = mbus_config.bus.mipi_csi2.num_data_lanes; ... if (dev->active_data_lanes > dev->max_data_lanes) { unicam_err(dev, "Device has requested %u data lanes, which is >%u configured in DT\n" ... ret = -EINVAL; cfe.c L1164-1171 same check, ret = -EINVAL

### B-33

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** Lanes required by the driver formula: at 972 Mbps/lane (link-frequency 486 MHz) 1080p60 UYVY = 3, 1080p60 RGB888 = 4, 1080p50 UYVY = 2, 1080p50 RGB888 = 3, 1080p30 RGB888 = 2, 720p60 RGB888 = 2, 720p60 UYVY = 1; at 594 Mbps/lane 1080p60 UYVY = 4 and 1080p60 RGB888 = 6. Values > 4 fall into the CSI_CONFW 'MASK_NOL_1' branch, i.e. a mis-programmed lane count.
- **Source:** Calculation from tc358743_num_csi_lanes_needed()/tc358743_set_csi() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** Formula L795-800; e.g. 1920*1080*60*16 = 1,990,656,000 / 972e6 = 2.05 -> 3; 1920*1080*60*24 = 2,985,984,000 / 594e6 = 5.03 -> 6; L852-854 ((lanes == 4) ? MASK_NOL_4 : (lanes == 3) ? MASK_NOL_3 : (lanes == 2) ? MASK_NOL_2 : MASK_NOL_1)

### B-34

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Supported media bus codes: enum_mbus_code index 0 = MEDIA_BUS_FMT_RGB888_1X24 (colorspace V4L2_COLORSPACE_SRGB, RGB full range) and index 1 = MEDIA_BUS_FMT_UYVY8_1X16 (colorspace V4L2_COLORSPACE_SMPTE170M, VOUT 4:2:2 with VI_REP set to 601 YCbCr limited); the probe default is RGB888_1X24.
- **Source:** tc358743.c enum_mbus_code / g_colorspace / set_csi_color_space — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1774-1782 case 0: code->code = MEDIA_BUS_FMT_RGB888_1X24; case 1: code->code = MEDIA_BUS_FMT_UYVY8_1X16; L1790-1793 return V4L2_COLORSPACE_SRGB; ... return V4L2_COLORSPACE_SMPTE170M; L766-767 MASK_VOUT_COLOR_601_YCBCR_LIMITED; L2247 state->mbus_fmt_code = MEDIA_BUS_FMT_RGB888_1X24;

### B-35

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** get_fmt/set_fmt only accept pad 0; width/height always come from the current DV timings and field is V4L2_FIELD_NONE; set_fmt changes only the media bus code (unknown codes keep the current code), TRY formats are not stored, and an ACTIVE set_fmt mutes the stream and reprograms PLL, CSI (lane count) and colour space.
- **Source:** tc358743.c tc358743_get_fmt()/tc358743_set_fmt() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1808-1811 format->format.width = state->timings.bt.width; ... field = V4L2_FIELD_NONE; L1835-1843 if (format->which == V4L2_SUBDEV_FORMAT_TRY) return 0; state->mbus_fmt_code = format->format.code; enable_stream(sd, false); tc358743_set_pll(sd); tc358743_set_csi(sd); tc358743_set_csi_color_space(sd);

### B-36

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** s_stream(1) writes TXOPTIONCNTRL=0 then TXOPTIONCNTRL=MASK_CONTCLKMODE (to force the clock-lane LP11->HS transition), unmutes video (VI_MUTE = MASK_AUTO_MUTE) and sets CONFCTL VBUFEN|ABUFEN; s_stream(0) mutes video (MASK_AUTO_MUTE|MASK_VI_MUTE), clears VBUFEN|ABUFEN and re-runs tc358743_set_csi() to put all lanes in LP-11. It always returns 0 and does not check signal presence.
- **Source:** tc358743.c enable_stream()/tc358743_s_stream() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L648-665: /* It is critical for CSI receiver to see lane transition LP11->HS. ...*/ i2c_wr32(sd, TXOPTIONCNTRL, 0); i2c_wr32(sd, TXOPTIONCNTRL, MASK_CONTCLKMODE); i2c_wr8(sd, VI_MUTE, MASK_AUTO_MUTE); ... L1757-1765 enable_stream(sd, enable); if (!enable) { /* Put all lanes in LP-11 state (STOPSTATE) */ tc358743_set_csi(sd); } return 0;

### B-37

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The DT clock-noncontinuous flag only affects TXOPTIONCNTRL in tc358743_set_csi(); every stream-on writes MASK_CONTCLKMODE (continuous clock) and get_mbus_config reports flags = 0, with the driver comment 'Support for non-continuous CSI-2 clock is missing in the driver'.
- **Source:** tc358743.c set_csi / enable_stream / get_mbus_config — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L843-844 i2c_wr32(sd, TXOPTIONCNTRL, (state->bus.flags & V4L2_MBUS_CSI2_NONCONTINUOUS_CLOCK) ? 0 : MASK_CONTCLKMODE); L653 i2c_wr32(sd, TXOPTIONCNTRL, MASK_CONTCLKMODE); L1750-1751 /* Support for non-continuous CSI-2 clock is missing in the driver */ cfg->bus.mipi_csi2.flags = 0;

### B-38

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Implemented subdev ops: core = log_status, g_register/s_register (CONFIG_VIDEO_ADV_DEBUG only; HDCP registers are write-protected), interrupt_service_routine, subscribe_event (V4L2_EVENT_SOURCE_CHANGE, V4L2_EVENT_CTRL), unsubscribe_event; video = g_input_status, s_stream; pad = enum_mbus_code, set_fmt, get_fmt, get_edid, set_edid, s_dv_timings, g_dv_timings, query_dv_timings, enum_dv_timings, dv_timings_cap, get_mbus_config. There is no enum_frame_size, enum_frame_interval, enable_streams/disable_streams or init_state.
- **Source:** tc358743.c ops tables — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1935-1969: tc358743_core_ops { .log_status ... .interrupt_service_routine = tc358743_isr, .subscribe_event ... } tc358743_video_ops { .g_input_status, .s_stream } tc358743_pad_ops { .enum_mbus_code ... .get_mbus_config = tc358743_get_mbus_config, }

### B-39

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** On a sync change (MISC_INT I_SYNC_CHG) or a DE size/position change with a stable signal (CLK_INT I_IN_DE_CHG), tc358743_format_change() mutes the stream if the signal is lost or the timings differ from the configured ones, and sends V4L2_EVENT_SOURCE_CHANGE (V4L2_EVENT_SRC_CH_RESOLUTION) if a subdev devnode exists. It does not reconfigure timings itself.
- **Source:** tc358743.c tc358743_format_change() and int handlers — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L1113-1134: .type = V4L2_EVENT_SOURCE_CHANGE, .u.src_change.changes = V4L2_EVENT_SRC_CH_RESOLUTION ... if (!v4l2_match_dv_timings(&state->timings, &timings, 0, false)) enable_stream(sd, false); ... if (sd->devnode) v4l2_subdev_notify_event(sd, &tc358743_ev_fmt); L1283-1284 if (!no_signal(sd) && !no_sync(sd)) tc358743_format_change(sd);

### B-40

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Remove sequence: stop the poll timer and flush the poll work (no-IRQ case), cancel hotplug delayed work, free debugfs InfoFrame entries, unregister CEC adapter, v4l2_async_unregister_subdev, v4l2_device_unregister_subdev, destroy mutex, media_entity_cleanup, free ctrl handler. Remove does not drop HPD, assert reset or disable refclk (clk_disable_unprepare appears only in the probe_of error path).
- **Source:** tc358743.c tc358743_remove() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L2340-2358: if (!state->i2c_client->irq) { timer_delete_sync(&state->timer); flush_work(&state->work_i2c_poll); } cancel_delayed_work_sync(...); v4l2_debugfs_if_free(...); ... cec_unregister_adapter(state->cec_adap); v4l2_async_unregister_subdev(sd); ... v4l2_ctrl_handler_free(&state->hdl); grep clk_disable -> only L2156

### B-41

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, TC358743
- **Fact:** The RPi overlay include tc358743.dtsi adds node tc358743@f (compatible "toshiba,tc358743", reg = <0x0f>) on &i2c_csi_dsi with clocks = <&cam1_clk>, clock-names = "refclk"; the endpoint has clock-lanes = <0>, clock-noncontinuous, link-frequencies = /bits/ 64 <486000000>, data-lanes = <1 2> by default; cam1_clk gets clock-frequency = <27000000>. It has no reset-gpios and no interrupts property.
- **Source:** arch/arm/boot/dts/overlays/tc358743.dtsi (rpi-6.18.y) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743.dtsi>
- **Evidence:** L9-23: tc358743: tc358743@f { compatible = "toshiba,tc358743"; reg = <0x0f>; ... clocks = <&cam1_clk>; clock-names = "refclk"; ... clock-lanes = <0>; clock-noncontinuous; link-frequencies = /bits/ 64 <486000000>; L46 data-lanes = <1 2>; L75 clock-frequency = <27000000>;

### B-42

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, TC358743
- **Fact:** Overlay parameters in tc358743.dtsi: 4lane = <0>,"-2+3-7+8" switches data-lanes to <1 2 3 4> on both the tc358743 endpoint and csi1_ep; link-frequency writes link-frequencies#0; cam0 retargets the I2C fragment to i2c_csi_dsi0, the CSI fragment to csi0 and the clock to cam0_clk.
- **Source:** arch/arm/boot/dts/overlays/tc358743.dtsi (rpi-6.18.y) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743.dtsi>
- **Evidence:** L94-99: 4lane = <0>, "-2+3-7+8"; link-frequency = <&tc358743_0>,"link-frequencies#0"; cam0 = <&i2c_frag>, "target:0=",<&i2c_csi_dsi0>, <&csi_frag>, "target:0=",<&csi0>, <&clk_frag>, "target:0=",<&cam0_clk>, <&tc358743>, "clocks:0=",<&cam0_clk>;

### B-43

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, TC358743
- **Fact:** tc358743-overlay.dts (compatible brcm,bcm2835; Pi 4/CM4) by default applies fragment@100 setting csi1 compatible to "brcm,bcm2835-unicam-legacy" (video-node-centric, non-MC mode in the downstream unicam driver); media-controller = <0>,"!100" disables that fragment when set, leaving the MC-mode compatible. Per RPi docs '!<n>' enables fragment n if the value is false, otherwise disables it.
- **Source:** tc358743-overlay.dts; bcm2835-unicam.c; RPi configuration docs 'Overlay/fragment parameters' — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-overlay.dts>
- **Evidence:** overlay L11-19: legacy_frag: fragment@100 { target = <&csi1>; __overlay__ { compatible = "brcm,bcm2835-unicam-legacy"; ... media-controller = <0>,"!100"; unicam.c L3425-3426 { .compatible = "brcm,bcm2835-unicam", .data = (void *)1 }, { .compatible = "brcm,bcm2835-unicam-legacy", .data = 0 }; docs: "!<n> // Enable fragment <n> if the assigned parameter value is false, otherwise disable it"

### B-44

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, CM5, TC358743
- **Fact:** A separate tc358743-pi5-overlay.dts (compatible "brcm,bcm2712") includes the same tc358743.dtsi with no media-controller parameter. On Pi 5 (arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts), i2c_csi_dsi aliases i2c_csi_dsi1 = &i2c4 (MIPI1 connector, 100 kHz), csi1 = &rp1_csi1 (compatible "raspberrypi,rp1-cfe"), and dummy i2c0if/i2c0mux nodes exist for overlay compatibility. On CM5 the I2C mapping depends on the IO-board DT. bcm2712-rpi-cm5io.dtsi maps i2c_csi_dsi1 to &i2c0 (symlink i2c-11) and i2c_csi_dsi0 to &i2c6. bcm2712-rpi-cm4io.dtsi maps i2c_csi_dsi1 to &i2c6 and i2c_csi_dsi0 to &i2c0. bcm2712-rpi-cm5.dtsi provides csi0/csi1 = rp1_csi0/1 and the dummy i2c0if/i2c0mux.
- **Source:** tc358743-pi5-overlay.dts; bcm2712-rpi-5-b.dts; rp1.dtsi (rpi-6.18.y) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-pi5-overlay.dts>
- **Evidence:** #include "tc358743.dtsi" / { compatible = "brcm,bcm2712"; }; bcm2712-rpi-5-b.dts L215 i2c_csi_dsi1: &i2c4 { // Note: This is for MIPI1 connector only ... clock-frequency = <100000>; L222 i2c_csi_dsi: &i2c_csi_dsi1 { }; L225 csi1: &rp1_csi1 { }; L116-117 i2c0if: i2c0if {}; i2c0mux: i2c0mux {}; rp1.dtsi L1038 compatible = "raspberrypi,rp1-cfe";
- **Original claim (before verification):** A separate tc358743-pi5-overlay.dts (compatible "brcm,bcm2712") includes the same tc358743.dtsi with no media-controller parameter; on Pi 5 i2c_csi_dsi aliases i2c_csi_dsi1 = &i2c4 (MIPI1 connector, 100 kHz), csi1 = &rp1_csi1 (compatible "raspberrypi,rp1-cfe"), and dummy i2c0if/i2c0mux nodes exist for overlay compatibility.
- **Verifier note:** The Pi 5 facts are verified at bcm2712-rpi-5-b.dts L116-117, L215-225 and rp1.dtsi L1037-1038 (all under arch/arm64/boot/dts/broadcom, not arch/arm). The original claim lists CM5 under applies_to, but the CM5 I2C bus differs, as shown in bcm2712-rpi-cm5io.dtsi and bcm2712-rpi-cm4io.dtsi.

### B-45

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, CM4, Pi5, CM5, TC358743
- **Fact:** Overlays README entries for tc358743 and tc358743-pi5: 'Uses Unicam 1'; params 4lane ('only applicable to Compute Modules CAM1 connector'), link-frequency ('Only values of 297000000 (574Mbit/s) and 486000000 (972Mbit/s - default) are supported by the driver'), cam0; tc358743 additionally has media-controller ('default off'). 2 x 297 MHz is 594 Mbit/s, so '574Mbit/s' is a README typo.
- **Source:** arch/arm/boot/dts/overlays/README (rpi-6.18.y) L5592-5631 — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** link-frequency Set the link frequency. Only values of 297000000 (574Mbit/s) and 486000000 (972Mbit/s - default) are supported by the driver. media-controller Configure use of Media Controller API for configuring the sensor (default off)

### B-46

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, TC358743
- **Fact:** tc358743-audio overlay: simple-audio-card named "tc358743" (card-name param), format i2s, dummy codec compatible "linux,spdif-dir" acting as bit-clock and frame master, CPU DAI i2s_clk_consumer with 2 TDM slots of 32 bits; README wiring LRCK/WFS GPIO19, BCK/SCK GPIO18, DATA/SD GPIO20. The driver configures 2-channel I2S audio output (CONFCTL MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S). The README says to use it with a 'tc358743-fast' overlay that has no README entry.
- **Source:** tc358743-audio-overlay.dts; overlays README; tc358743.c set_hdmi_audio — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts>
- **Evidence:** overlay L22 compatible = "linux,spdif-dir"; L34-41 bitclock-master = <&dailink0_master>; ... dai-tdm-slot-num = <2>; dai-tdm-slot-width = <32>; README: "Used in combination with the tc358743-fast overlay ... LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18, and DATA/SD to GPIO 20." tc358743.c L918-919 MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S

### B-47

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi4, CM4, Pi5, CM5, TC358743
- **Fact:** cam1_clk/cam0_clk are 'fixed-clock' nodes (disabled by default) in the Pi 4 (bcm270x.dtsi) and Pi 5/CM5 base DTs; the overlay only declares the frequency (27 MHz) and does not make the Pi generate it, so the TC358743 board must supply its own REFCLK oscillator matching the declared frequency.
- **Source:** Inference from bcm270x.dtsi / bcm2712-rpi-5-b.dts and tc358743.dtsi — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm270x.dtsi>
- **Evidence:** bcm270x.dtsi L156-160 cam1_clk: cam1_clk { compatible = "fixed-clock"; #clock-cells = <0>; status = "disabled"; }; bcm2712-rpi-5-b.dts L78-82 same; tc358743.dtsi L72-76 clk_frag target <&cam1_clk> status okay clock-frequency = <27000000>

### B-48

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4
- **Fact:** Pi 4 Model B csi1 is limited to brcm,num-data-lanes = <2> (bcm283x-rpi-csi1-2lane.dtsi); CM4 includes bcm283x-rpi-csi1-4lane.dtsi and bcm283x-rpi-csi0-2lane.dtsi.
- **Source:** bcm2711-rpi-4-b.dts / bcm2711-rpi-cm4.dts / bcm283x-rpi-csi1-2lane.dtsi (rpi-6.18.y) — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm2711-rpi-4-b.dts>
- **Evidence:** bcm2711-rpi-4-b.dts L300 #include "bcm283x-rpi-csi1-2lane.dtsi"; bcm283x-rpi-csi1-2lane.dtsi: &csi1 { brcm,num-data-lanes = <2>; bcm2711-rpi-cm4.dts L273-274 #include "bcm283x-rpi-csi0-2lane.dtsi" #include "bcm283x-rpi-csi1-4lane.dtsi"

### B-49

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi5, CM5, TC358743, Linux
- **Fact:** On Pi 5 the downstream RP1 CFE driver (compatible "raspberrypi,rp1-cfe"; the upstream copy in rpi-6.18.y binds only "raspberrypi,rp1-cfe-upstream") cannot read a link rate from tc358743, because the bridge is not MEDIA_ENT_F_CAM_SENSOR and has no LINK_FREQ/PIXEL_RATE control. It logs 'Unable to determine sensor link rate, using 999 Mbps' and programs the D-PHY for 999 Mbps.
- **Source:** Inference from rp1_cfe/cfe.c sensor_link_rate() and tc358743.c — <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** cfe.c L1077-1090 if (!(pad->flags & MEDIA_PAD_FL_SINK)) goto err; ... if (entity->function == MEDIA_ENT_F_CAM_SENSOR || v4l2_ctrl_find(..., V4L2_CID_LINK_FREQ) || v4l2_ctrl_find(..., V4L2_CID_PIXEL_RATE)) break; L1103 cfe_err("Unable to determine sensor link rate, using 999 Mbps\n"); tc358743.c: single SOURCE pad, MEDIA_ENT_F_VID_IF_BRIDGE, 3 ctrls (B-16, B-18); rp1-cfe/cfe.c L2504 .compatible = "raspberrypi,rp1-cfe-upstream"

### B-50

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** tc358743_initial_setup holds the IR block in reset (and CEC too unless CONFIG_VIDEO_TC358743_CEC), pulses CSI-TX/HDMI resets, leaves sleep mode, programs FIFOCTL and the refclk-derived registers (SYS_FREQ = refclk/10000, FH_MIN = refclk/100000, FH_MAX = FH_MIN*66/10, LOCKDET_REF = refclk/100), sets EDID_MODE to E-DDC, and configures HDMI PHY, HDCP (disabled = manual authentication), I2S audio and InfoFrame capture.
- **Source:** tc358743.c tc358743_initial_setup()/tc358743_set_ref_clk() — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** L946-966: i2c_wr16_and_or(sd, SYSCTL, ~(MASK_IRRST | MASK_CECRST), (MASK_IRRST | MASK_CECRST)); tc358743_reset(sd, MASK_CTXRST | MASK_HDMIRST); ... i2c_wr16(sd, FIFOCTL, pdata->fifo_level); tc358743_set_ref_clk(sd); ... MASK_EDID_MODE_E_DDC); L721 sys_freq = pdata->refclk_hz / 10000; L729 fh_min = pdata->refclk_hz / 100000; L733 fh_max = (fh_min * 66) / 10; L737 lockdet_ref = pdata->refclk_hz / 100;

---

## Topic C

**Raspberry Pi CSI-2 receive path (Pi 4 / CM4 / Pi 5 / CM5)** — <a id="topic-c"></a>53 claims.

### C-01

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4
- **Fact:** Raspberry Pi 4 Model B has exactly one camera connector: a standard 15-pin, 1.0 mm pitch, 16 mm wide CSI port that carries 2 MIPI CSI-2 data lanes.
- **Source:** Raspberry Pi 4 Model B datasheet §5.2; RPi docs raspberry-pi/introduction.adoc — <https://datasheets.raspberrypi.com/rpi4/raspberry-pi-4-datasheet.pdf>
- **Evidence:** "The Pi4B has 1x Raspberry Pi 2-lane MIPI CSI Camera and 1x Raspberry Pi 2-lane MIPI DSI Display connector."; intro table for Pi 4 Model B: "standard 15-pin, 1.0 mm pitch, 16 mm width, CSI (camera) port"

### C-02

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4
- **Fact:** Compute Module 4 has two camera ports: CAM0 with 2 data lanes and CAM1 with 4 data lanes.
- **Source:** Raspberry Pi Compute Module 4 Datasheet §2.7 — <https://datasheets.raspberrypi.com/cm4/cm4-datasheet.pdf>
- **Evidence:** "The CM4 supports two camera ports: CAM0 (2 lanes) and CAM1 (4 lanes)."

### C-03

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4
- **Fact:** The CM4 IO Board routes both CSI-2 interfaces (2-lane CSI0 and 4-lane CSI1) to separate 22-pin, 0.5 mm pitch connectors. Using CSI0 requires both J6 jumpers to be fitted so that I2C reaches that connector.
- **Source:** Compute Module 4 IO Board datasheet §2.11 and J6 note — <https://datasheets.raspberrypi.com/cm4io/cm4io-datasheet.pdf>
- **Evidence:** "Both CSI-2 interfaces (2-channel and 4-channel) are brought out to separate 22-way 0.5mm pitch connectors... If the CSI0 interface (2-channel) is used, then the two jumpers on J6 must be fitted to route the I2C bus to the connector."

### C-04

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi5
- **Fact:** Raspberry Pi 5 has two mini 22-pin, 0.5 mm pitch combined CSI/DSI ports. Each port is backed by a 4-lane MIPI transceiver rated at 1.5 Gbps per lane.
- **Source:** Raspberry Pi 5 product brief; RPi docs raspberry-pi/introduction.adoc — <https://datasheets.raspberrypi.com/rpi5/raspberry-pi-5-product-brief.pdf>
- **Evidence:** "...replaced by a pair of four-lane 1.5Gbps MIPI transceivers..."; "2 × 4-lane MIPI camera / display transceivers"; intro: "2 × mini 22-pin, 0.5 mm (fine) pitch, 11.5 mm width, combined CSI (camera)/DSI (display) ports"

### C-05

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM5
- **Fact:** CM5 provides two 4-lane MIPI interfaces (MIPI0 on CM4 CAM1 pins 115-141 and MIPI1 on CM4 DSI1 pins 175-196), each usable as CSI-2 or DSI. The CM4 CAM0 pins (128-142) carry USB 3.0 on CM5.
- **Source:** Raspberry Pi Compute Module 5 datasheet §2.5.2, pinout, Appendix B.1.2 — <https://datasheets.raspberrypi.com/cm5/cm5-datasheet.pdf>
- **Evidence:** "CM5 supports two 4-lane MIPI interfaces for connecting cameras (CSI) and displays (DSI)."; pinout "115 MIPI0_D0_N" ... "141 MIPI0_D3_P", "175 MIPI1_D0_N"...; "The CAM0 port (pins 128 to 142) on CM4 is a USB 3.0 port on CM5."

### C-06

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM5
- **Fact:** The CM5 IO Board has two dual-purpose 22-pin CAM/DISP connectors. CAM/DISP 0 includes a camera power-down signal. CAM/DISP 1 needs two J6 jumpers to route I2C, and a camera on it cannot be powered down.
- **Source:** Compute Module 5 IO Board datasheet §3.2, §5.3 — <https://datasheets.raspberrypi.com/cm5/cm5io-datasheet.pdf>
- **Evidence:** "The CAM/DISP 0 connector can connect either a display or a camera, and includes a signal to power down the camera... The CAM/DISP 1 connector ... requires two jumpers at J6 to route I2C signals from the GPIO connector. When used with a camera, it isn't possible to power down the camera."

### C-07

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, CM4
- **Fact:** The official docs describe the BCM283x/BCM2711 CSI-2 receiver (Unicam) as two instances: the first supports 2 data lanes and the second supports 4. Each lane runs at up to 1 Gbit/s (DDR, so the maximum link frequency is 500 MHz). Non-Compute-Module boards before Pi 5 expose only 2 lanes of the second instance.
- **Source:** RPi documentation camera/csi-2-usage.adoc (Unicam), docs commit 858e9bc (2026-10-05) — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/csi-2-usage.adoc>
- **Evidence:** "The first instance of Unicam supports two CSI-2 data lanes, while the second supports four. Each lane can run at up to 1Gbit/s (DDR, so the max link frequency is 500 MHz). Compute Modules and Raspberry Pi 5 route out all lanes from both peripherals. Other models prior to Raspberry Pi 5 only expose the second instance, routing out only two of the data lanes"

### C-08

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** In the BCM2711 device tree, csi0 (csi@7e800000) has brcm,num-data-lanes=<2> and csi1 (csi@7e801000) has brcm,num-data-lanes=<4>, both compatible "brcm,bcm2835-unicam". The Pi 4B DTS includes bcm283x-rpi-csi1-2lane.dtsi, which limits csi1 to 2 lanes. The CM4 DTS includes csi0-2lane and csi1-4lane.
- **Source:** raspberrypi/linux rpi-6.18.y (default branch, commit af73e083): bcm283x.dtsi, bcm2711-rpi-4-b.dts, bcm2711-rpi-cm4.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm283x.dtsi>
- **Evidence:** bcm283x.dtsi:457-478 "csi1: csi@7e801000 { compatible = \"brcm,bcm2835-unicam\"; ... brcm,num-data-lanes = <4>;"; bcm2711-rpi-4-b.dts:300 #include "bcm283x-rpi-csi1-2lane.dtsi" (&csi1 { brcm,num-data-lanes = <2>; }); bcm2711-rpi-cm4.dts:273-274 csi0-2lane, csi1-4lane

### C-09

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** rpi-6.18.y contains two Unicam drivers. The downstream drivers/media/platform/bcm2835/bcm2835-unicam.c (CONFIG_VIDEO_BCM2835_UNICAM_LEGACY, module bcm2835-unicam-legacy) binds "brcm,bcm2835-unicam" (Media Controller mode) and "brcm,bcm2835-unicam-legacy" (video-node-centric mode). The mainline drivers/media/platform/broadcom/bcm2835-unicam.c (CONFIG_VIDEO_BCM2835_UNICAM) binds only "brcm,bcm2835-unicam-upstream" in this tree.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2835/ and broadcom/ Unicam drivers, Makefiles, Kconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/bcm2835/bcm2835-unicam.c>
- **Evidence:** bcm2835/bcm2835-unicam.c:3425-3426 "{ .compatible = \"brcm,bcm2835-unicam\", .data = (void *)1 }, { .compatible = \"brcm,bcm2835-unicam-legacy\", .data = 0 }"; broadcom/bcm2835-unicam.c:2764 "brcm,bcm2835-unicam-upstream"; bcm2835/Makefile "bcm2835-unicam-legacy-y := bcm2835-unicam.o"

### C-10

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Pi4, CM4, Linux
- **Fact:** The Pi 0-4 tc358743 overlay changes csi1's compatible to "brcm,bcm2835-unicam-legacy" (fragment@100), so Unicam runs in video-node-centric mode by default. The media-controller parameter removes that fragment to select Media Controller mode. The cam0 parameter retargets I2C to i2c_csi_dsi0, CSI to csi0, the clock to cam0_clk, and the legacy fragment to csi0.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743-overlay.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-overlay.dts>
- **Evidence:** "legacy_frag: fragment@100 { target = <&csi1>; __overlay__ { compatible = \"brcm,bcm2835-unicam-legacy\"; } }"; "media-controller = <0>,\"!100\";"; cam0 = ... <&legacy_frag>, "target:0=",<&csi0>

### C-11

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Pi5, CM5, Linux
- **Fact:** On BCM2712 (Pi 5/CM5), the overlay map silently redirects dtoverlay=tc358743 to tc358743-pi5. That overlay only includes tc358743.dtsi with compatible "brcm,bcm2712": it has no legacy-Unicam fragment and no media-controller parameter, so the Pi 5 path always uses Media Controller.
- **Source:** raspberrypi/linux rpi-6.18.y overlays/overlay_map.dts and tc358743-pi5-overlay.dts; PR #6795 — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-pi5-overlay.dts>
- **Evidence:** overlay_map.dts:457-461 "tc358743 { bcm2835; bcm2711; bcm2712 = \"tc358743-pi5\"; };"; tc358743-pi5-overlay.dts: '#include "tc358743.dtsi" / { compatible = "brcm,bcm2712"; };'

### C-12

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Pi4, CM4, Pi5, CM5, Linux
- **Fact:** The shared tc358743.dtsi does the following: (1) puts the bridge at I2C reg 0x0f on &i2c_csi_dsi; (2) declares refclk as a fixed 27000000 Hz clock (cam1_clk); (3) sets clock-lanes <0> and clock-noncontinuous; (4) sets link-frequencies to 486000000 by default; (5) sets data-lanes <1 2> by default. The 4lane override ("-2+3-7+8") switches both endpoints to <1 2 3 4>, and link-frequency overrides link-frequencies#0.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743.dtsi>
- **Evidence:** "tc358743: tc358743@f { compatible = \"toshiba,tc358743\"; reg = <0x0f>; clocks = <&cam1_clk>; ... clock-noncontinuous; link-frequencies = /bits/ 64 <486000000>;"; clk_frag clock-frequency = <27000000>; __overrides__ "4lane = <0>, \"-2+3-7+8\"; link-frequency = <&tc358743_0>,\"link-frequencies#0\";"

### C-13

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** TC358743, Pi4, CM4, Pi5, CM5
- **Fact:** The overlays README says only link-frequency values 297000000 and 486000000 (default) are supported by the driver. It labels 297000000 as "574Mbit/s", which is inconsistent with the driver's 2 x 297 MHz = 594 Mbit/s. The README text for tc358743-pi5 is stale: it still says "Uses Unicam 1" and calls 4lane CM-CAM1-only.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/README (tc358743, tc358743-pi5 entries) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** "link-frequency  Set the link frequency. Only values of 297000000 (574Mbit/s) and 486000000 (972Mbit/s - default) are supported by the driver."; tc358743-pi5 Info: "Uses Unicam 1, which is the standard camera connector on most Pi variants."

### C-14

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The tc358743 driver computes bps_pr_lane = 2 x link_frequencies[0]. It requires 62.5 Mbps to 1 Gbps per lane and a refclk of 26, 27 or 42 MHz, and sets pll_prd = refclk/6 MHz. It has PHY timing tables only for 594000000 and 972000000 bps; any other rate logs "untested bps per lane" and falls through to the 594 Mbps values.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c tc358743_probe_of() — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2092-2111 "bps_pr_lane = 2 * endpoint.link_frequencies[0]; if (bps_pr_lane < 62500000U || bps_pr_lane > 1000000000U)" ... "default: dev_warn(dev, \"untested bps per lane: %u bps\\n\"...); fallthrough; case 594000000U: ... case 972000000U:"; :2075-2079 refclk 26/27/42 MHz, pll_prd = refclk_hz / 6000000

### C-15

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The tc358743 driver picks its active CSI-2 lane count as DIV_ROUND_UP(width x height x fps x bpp, bps_pr_lane), with bpp=16 for MEDIA_BUS_FMT_UYVY8_1X16 and 24 otherwise. It uses active pixels only, ignoring blanking. It reports this count via get_mbus_config and never checks it against the DT data-lanes.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:790-801 "u32 bits_pr_pixel = (state->mbus_fmt_code == MEDIA_BUS_FMT_UYVY8_1X16) ? 16 : 24; u32 bps = bt->width * bt->height * fps(bt) * bits_pr_pixel; ... return DIV_ROUND_UP(bps, bps_pr_lane);"; :1752 "cfg->bus.mipi_csi2.num_data_lanes = state->csi_lanes_in_use;"

### C-16

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Pi4, CM4, Pi5, CM5, Linux
- **Fact:** Both the downstream Unicam driver and the downstream rp1_cfe driver call get_mbus_config at stream start. If the bridge asks for more lanes than the DT endpoint provides, they fail with -EINVAL and the message "Device has requested %u data lanes, which is >%u configured in DT". A lane count below the DT maximum, such as 3 of 4, is accepted.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2835/bcm2835-unicam.c and raspberrypi/rp1_cfe/cfe.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** bcm2835-unicam.c:2510-2518 and rp1_cfe/cfe.c:1164-1171 "if (cfe->csi2.dphy.active_lanes > cfe->csi2.dphy.max_lanes) { cfe_err(\"Device has requested %u data lanes, which is >%u configured in DT\\n\" ... ret = -EINVAL;"

### C-17

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The downstream Unicam driver accepts only 1, 2 or 4 data lanes, in order, on its DT endpoint. If the endpoint lists more lanes than brcm,num-data-lanes, it only logs "subdevice requires %u data lanes when %u are supported" and then adopts the endpoint count; it does not fail the probe.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/platform/bcm2835/bcm2835-unicam.c of_unicam_connect_subdevs() — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/bcm2835/bcm2835-unicam.c>
- **Evidence:** bcm2835-unicam.c:3203-3231 "case 1: case 2: case 4: break;" ... "if (ep.bus.mipi_csi2.num_data_lanes > dev->max_data_lanes) { unicam_err(dev, \"subdevice requires %u data lanes when %u are supported\\n\"...); } dev->max_data_lanes = ep.bus.mipi_csi2.num_data_lanes;"

### C-18

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The tc358743 timing capability allows widths of 640-1920, heights of 350-1200, pixel clocks of 13-165 MHz, and progressive scan only. s_dv_timings and query_dv_timings return -ERANGE for anything outside these limits.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c tc358743_timings_cap — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:76-81 "V4L2_INIT_BT_TIMINGS(640, 1920, 350, 1200, 13000000, 165000000, ... V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_REDUCED_BLANKING | V4L2_DV_BT_CAP_CUSTOM)"; :1667-1671 "return -ERANGE;"

### C-19

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** The tc358743 subdev has the following properties: (1) it enumerates only MEDIA_BUS_FMT_RGB888_1X24 (index 0, the probe default) and MEDIA_BUS_FMT_UYVY8_1X16; (2) its entity function is MEDIA_ENT_F_VID_IF_BRIDGE with a single source pad; (3) it starts with V4L2_DV_BT_CEA_640X480P59_94 timings; (4) it registers only V4L2_CID_DV_RX_POWER_PRESENT plus audio sampling-rate and audio-present controls, and no V4L2_CID_LINK_FREQ or V4L2_CID_PIXEL_RATE.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:1770-1783 enum_mbus_code index 0 RGB888_1X24, 1 UYVY8_1X16; :2242 "sd->entity.function = MEDIA_ENT_F_VID_IF_BRIDGE;"; :2247 "state->mbus_fmt_code = MEDIA_BUS_FMT_RGB888_1X24;"; :2172-2173 default_timing = V4L2_DV_BT_CEA_640X480P59_94; :2224-2234 ctrl creation

### C-20

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** The stock tc358743 overlay has no interrupts property, so the driver polls signal status with a timer instead of an IRQ. The interval is POLL_INTERVAL_MS = 1000 ms, or 10 ms when a CEC adapter exists. The RPi bcm2711_defconfig and bcm2712_defconfig set CONFIG_VIDEO_TC358743=m but do not enable CONFIG_VIDEO_TC358743_CEC.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743.c, tc358743.dtsi, arch/arm64/configs/bcm2711_defconfig, bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:69 "#define POLL_INTERVAL_MS 1000"; :1608 "msecs = state->cec_adap ? POLL_INTERVAL_CEC_MS : POLL_INTERVAL_MS;"; :2275-2289 IRQ else timer_setup; bcm2711_defconfig:1077 / bcm2712_defconfig:1079 "CONFIG_VIDEO_TC358743=m" (no TC358743_CEC line)

### C-21

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Camera power-enable on Pi 4 family: on Pi 4B, cam1_reg is a regulator-fixed on expgpio 5 ("CAM_GPIO", a firmware-controlled expander GPIO) and cam0_reg is cam_dummy_reg. On CM4, cam0_reg is an alias of cam1_reg on expgpio 5, so both ports share one enable.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2711-rpi-4-b.dts, bcm2711-rpi-cm4.dts; RPi docs compute-module/cmio-camera.adoc — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm2711-rpi-cm4.dts>
- **Evidence:** bcm2711-rpi-4-b.dts:486-491 "&cam1_reg { gpio = <&expgpio 5 GPIO_ACTIVE_HIGH>; }; cam0_reg: &cam_dummy_reg {};"; bcm2711-rpi-cm4.dts:459-461 "cam0_reg: &cam1_reg { gpio = <&expgpio 5 GPIO_ACTIVE_HIGH>; };"; docs: "The CM4 IO board provides a single GPIO pin for both aliases, so both cameras share the same regulator."

### C-22

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, CM5, Linux
- **Fact:** Camera power-enable on Pi 5 family: on Pi 5, cam0_reg is RP1 GPIO 34 (CD0_IO0_MICCLK, MIPI 0 connector) and cam1_reg is RP1 GPIO 46 (CD1_IO0_MICCLK, MIPI 1 connector). On CM5, cam0_reg is RP1 GPIO 34 (CAM_GPIO0) and cam1_reg is an alias of it ("Shares CAM_GPIO with cam0_reg").
- **Source:** raspberrypi/linux rpi-6.18.y bcm2712-rpi-5-b.dts, bcm2712-rpi-cm5.dtsi; CM5 datasheet §2.9.3 — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts>
- **Evidence:** bcm2712-rpi-5-b.dts:90-102 "gpio = <&rp1_gpio 34 0>; // CD0_IO0_MICCLK, to MIPI 0 connector" / "gpio = <&rp1_gpio 46 0>; // CD1_IO0_MICCLK, to MIPI 1 connector"; bcm2712-rpi-cm5.dtsi:198 "cam1_reg: &cam0_reg { // Shares CAM_GPIO with cam0_reg"; CM5 datasheet: "CAM_GPIO0 corresponds to GPIO34 on RP1."

### C-23

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** Neither the tc358743 overlay (tc358743.dtsi) nor the tc358743 driver references cam0_reg or cam1_reg or requests any regulator. The driver only takes an optional "reset" GPIO (devm_gpiod_get_optional), which the stock overlay does not supply.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743.dtsi and tc358743.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2141 "state->reset_gpio = devm_gpiod_get_optional(dev, \"reset\", GPIOD_OUT_LOW);"; grep for "regulator" in tc358743.c and tc358743.dtsi returns nothing

### C-24

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** On Pi 4B and CM4, i2c_csi_dsi is channel i2c@1 of an i2c-mux-pinctrl (i2c0mux) over the BCM2711 i2c0 controller, aliased i2c10 (/dev/i2c-10) and muxed to GPIO 44/45. i2c_csi_dsi0 (CM CAM0) is mux channel i2c@0 on GPIO 0/1 (/dev/i2c-0).
- **Source:** raspberrypi/linux rpi-6.18.y bcm270x.dtsi, bcm270x-rpi.dtsi, bcm283x-rpi-i2c0mux_0_44.dtsi, bcm2711-rpi-cm4.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm270x-rpi.dtsi>
- **Evidence:** bcm270x.dtsi:125-145 "i2c0mux: i2c0mux { compatible = \"i2c-mux-pinctrl\"; i2c-parent = <&i2c0if>; ... i2c_csi_dsi: i2c@1 {"; bcm270x-rpi.dtsi:23 "i2c10 = &i2c_csi_dsi;"; i2c0mux_0_44.dtsi "pinctrl-1 = <&i2c0_gpio44>;"; bcm2711-rpi-cm4.dts:463 "i2c_csi_dsi0: &i2c0"

### C-25

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4
- **Fact:** Official Compute Module docs say camera drivers assume CAM1 uses i2c-10 and CAM0 uses i2c-0. On the CM4 IO Board these are GPIOs 44,45 (i2c-10) and GPIOs 0,1 (i2c-0).
- **Source:** RPi documentation compute-module/cmio-camera.adoc 'I2C mapping of GPIO pins' — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/compute-module/cmio-camera.adoc>
- **Evidence:** "By default, the supplied camera drivers assume that CAM1 uses `i2c-10` and CAM0 uses `i2c-0`." table: "CM4 I/O Board | GPIOs 44,45 | GPIOs 0,1"

### C-26

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, Linux
- **Fact:** On Pi 5, i2c_csi_dsi0 is RP1 i2c6 on GPIO 38/39 for the MIPI0 connector (alias i2c10, symlink "i2c-6"). i2c_csi_dsi1 is RP1 i2c4 on GPIO 40/41 for the MIPI1 connector (alias i2c11, symlink "i2c-4"). i2c_csi_dsi is an alias of i2c_csi_dsi1. So CAM/DISP0 is /dev/i2c-10 and CAM/DISP1 is /dev/i2c-11.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2712-rpi-5-b.dts and bcm2712-rpi.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts>
- **Evidence:** bcm2712-rpi-5-b.dts:208-222 "i2c_csi_dsi0: &i2c6 { // Note: This is for MIPI0 connector only ... symlink = \"i2c-6\";" "i2c_csi_dsi1: &i2c4 { // Note: This is for MIPI1 connector only ... symlink = \"i2c-4\";" "i2c_csi_dsi: &i2c_csi_dsi1 { }; // An alias for compatibility"; bcm2712-rpi.dtsi:147-148 "i2c10 = &i2c_csi_dsi0; i2c11 = &i2c_csi_dsi1;"

### C-27

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM5, Linux
- **Fact:** CM5 camera I2C depends on the IO board. On CM5IO, CAM/DISP 1 is RP1 i2c0 on GPIO 0/1 (symlink "i2c-11") and CAM/DISP 0 is RP1 i2c6 on GPIO 38/39. On CM4IO, i2c_csi_dsi1 (CAM1, DISP1, RTC, fan) is i2c6 and i2c_csi_dsi0 (CAM0/DISP0) is i2c0; there, alias i2c10 points at i2c_csi_dsi and alias i2c11 is deleted.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2712-rpi-cm5io.dtsi, bcm2712-rpi-cm4io.dtsi; overlays README i2c_csi_dsi entries — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5io.dtsi>
- **Evidence:** cm5io.dtsi: "i2c_csi_dsi1: &i2c0 { // Note: This is for CAM/DISP 1 connector symlink = \"i2c-11\";" "i2c_csi_dsi0: &i2c6 { // Note: This is for CAM/DISP 0 connector"; cm4io.dtsi: "i2c_csi_dsi1: &i2c6 { // Note: For CAM1, DISP1, on-board RTC, and fan controller" "&aliases { /delete-property/ i2c11; i2c10 = &i2c_csi_dsi; };"; README: "CM5 on CM5IO: i2c-0 on 0 & 1"

### C-28

- **Verdict:** `CORRECTED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi5, TC358743, Linux
- **Fact:** The Pi 5 camera I2C bus numbers have changed between kernels. In a Raspberry Pi Forums post on 2023-11-11 (thread t=359412), 6by9 wrote: 'Note that the CAM/DISP connector I2C buses are i2c-4 and i2c-6, not 10 and 0 as before.' Later posts in the same thread (Dec 2023 to Feb 2024) show the bridge as 'tc358743 4-000f' on CAM/DISP1. On 6.18 kernels the buses are numbered through the aliases i2c10 = i2c_csi_dsi0 (CAM/DISP0) and i2c11 = i2c_csi_dsi1 (CAM/DISP1), so the bridge appears as 'tc358743 11-000f' on CAM/DISP1 or 'tc358743 10-000f' on CAM/DISP0. The DT keeps symlink properties 'i2c-6' and 'i2c-4'.
- **Source:** Raspberry Pi Forums t=359412 (6by9) and raspberrypi/linux issue #7523 — <https://forums.raspberrypi.com/viewtopic.php?t=359412>
- **Evidence:** 6by9 2023-11-13: "Note that the CAM/DISP connector I2C buses are i2c-4 and i2c-6, not 10 and 0 as before."; #7523 (6.18.34/6.18.39): media-ctl '"tc358743 11-000f":0' and 6by9 '"tc358743 10-000f":0'
- **Original claim (before verification):** The Pi 5 camera I2C bus numbers have changed between kernels. In November 2023 Raspberry Pi engineer 6by9 wrote that the CAM/DISP I2C buses were i2c-4 and i2c-6, and the bridge enumerated as "tc358743 4-000f". On 6.18 kernels it enumerates as "tc358743 10-000f" / "11-000f", and the DT keeps i2c-4/i2c-6 symlinks.
- **Verifier note:** The forum quote is from 6by9 and dated Sat Nov 11, 2023, not 2023-11-13. The '4-000f' enumeration lines come from later posts in the thread (some by 6by9 and some by users), not from the same post. The 6.18 numbering is confirmed by the DT aliases (bcm2712-rpi.dtsi:147-148), the README, the #7523 reporter on 6.18.34 using 'tc358743 11-000f', and 6by9's #7523 recipe using both 11-000f and 10-000f.

### C-29

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, CM5, Linux
- **Fact:** On Pi 5/CM5, rp1_csi0 (csi@110000, regs 0xc0_40110000) and rp1_csi1 (csi@128000, regs 0xc0_40128000) are compatible "raspberrypi,rp1-cfe", with labels csi0/csi1. That compatible is bound by the downstream driver drivers/media/platform/raspberrypi/rp1_cfe (CONFIG_VIDEO_RP1_CFE_DOWNSTREAM, module rp1-cfe-downstream). The mainline-derived rp1-cfe driver (CONFIG_VIDEO_RP1_CFE) binds only "raspberrypi,rp1-cfe-upstream" in this tree. bcm2712_defconfig builds both as modules.
- **Source:** raspberrypi/linux rpi-6.18.y rp1.dtsi, rp1_cfe/ and rp1-cfe/ drivers, bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** rp1.dtsi:1018-1038 "rp1_csi0: csi@110000 { compatible = \"raspberrypi,rp1-cfe\";" / "rp1_csi1: csi@128000"; rp1_cfe/cfe.c:2470 "{ .compatible = \"raspberrypi,rp1-cfe\" }"; rp1-cfe/cfe.c:2504 "raspberrypi,rp1-cfe-upstream"; rp1_cfe/Makefile "obj-$(CONFIG_VIDEO_RP1_CFE_DOWNSTREAM) += rp1-cfe-downstream.o"; bcm2712_defconfig:1040-1041 both =m

### C-30

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi5, CM5
- **Fact:** The RP1 CSI-2 host byte clock is at most 1500 MHz/8 = 187.5 MHz, i.e. 1.5 Gbps per lane. RP1 has two 4-lane D-PHYs shared between CSI-2 and DSI, supporting 8 Gbps in total.
- **Source:** RP1 Peripherals datasheet §1 overview and §8.2 CSI-2 host — <https://datasheets.raspberrypi.com/rp1/rp1-peripherals.pdf>
- **Evidence:** "CSI controller outputs a clock at link byte clock (line rate/8) called clk_csi2_byte. Maximum frequency will be 1500MHz/8 = 187.5MHz."; "connected to two shared 4-lane MIPI DPHY transceiver PHYs. Together, they support 8Gbps of downstream traffic"

### C-31

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, CM5, TC358743, Linux
- **Fact:** The rp1_cfe D-PHY hsfreqrange table covers 80-1500 Mbps. The driver gets the D-PHY rate from a graph walk that looks for a MEDIA_ENT_F_CAM_SENSOR entity or one exposing V4L2_CID_LINK_FREQ or V4L2_CID_PIXEL_RATE. If none is found it logs "Unable to determine sensor link rate, using 999 Mbps" and uses 999 Mbps. That fallback is always taken for tc358743 (see C-19).
- **Source:** raspberrypi/linux rpi-6.18.y rp1_cfe/cfe.c sensor_link_rate(), rp1_cfe/dphy.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** cfe.c:1055-1106 "if (entity->function == MEDIA_ENT_F_CAM_SENSOR || v4l2_ctrl_find(... V4L2_CID_LINK_FREQ) || v4l2_ctrl_find(... V4L2_CID_PIXEL_RATE)) break;" ... "cfe_err(\"Unable to determine sensor link rate, using 999 Mbps\\n\"); return 999 * 1000000UL;"; cfe.c:1175 "dphy_rate = sensor_link_rate(cfe) / 1000000UL"; dphy.c:131 "if (mbps < 80 || mbps > 1500)"

### C-32

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, CM5, Linux
- **Fact:** In the rp1_cfe media graph: (1) the "csi2" subdev has sink pads 0-3 and source pads 4-7; (2) sensor-to-csi2 links are created IMMUTABLE|ENABLED; (3) csi2 source pads link to video nodes named "rp1-cfe-<node>" (e.g. rp1-cfe-csi2_ch0) and to "pisp-fe", with flags 0 (disabled). So userspace must enable a csi2:4 -> rp1-cfe-csi2_ch0 link to write raw frames to memory.
- **Source:** raspberrypi/linux rpi-6.18.y rp1_cfe/csi2.c, cfe.c, pisp_fe.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** csi2.c:585-587 "csi2->pad[i].flags = i < CSI2_NUM_CHANNELS ? MEDIA_PAD_FL_SINK : MEDIA_PAD_FL_SOURCE;" (CSI2_NUM_CHANNELS 4); csi2.c:601 name "csi2"; cfe.c:2064-2067 sensor link "MEDIA_LNK_FL_IMMUTABLE | MEDIA_LNK_FL_ENABLED"; cfe.c:2076-2078 "media_create_pad_link(&cfe->csi2.sd.entity, node_desc[i].link_pad, &node->video_dev.entity, 0, 0)"; cfe.c:1995 "%s-%s", CFE_MODULE_NAME "rp1-cfe"

### C-33

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi5, TC358743, Linux
- **Fact:** Raspberry Pi engineer 6by9 gave a TC358743 capture sequence for Pi 5 that he reported working on 6.18.39. Step 1: set the EDID and run --set-dv-bt-timings query on /dev/v4l-subdevN. Step 2: run media-ctl -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'. Step 3: set matching formats on '"tc358743 1x-000f":0', '"csi2":0' and '"csi2":4' with field:none and colorspace. Step 4: set the /dev/video0 format, using UYVY for UYVY8_1X16 or BGR3 for RGB888_1X24. The reporter found that leaving out field:none on the csi2 pads makes STREAMON fail with -EPIPE (-32).
- **Source:** raspberrypi/linux issue #7523 (closed 2026-07-31) comments by 6by9 and reporter — <https://github.com/raspberrypi/linux/issues/7523>
- **Evidence:** 6by9 2026-07-27: "media-ctl -d 2 -l '\"csi2\":4 -> \"rp1-cfe-csi2_ch0\":0 [1]'" ... "media-ctl -d 2 -V '\"csi2\":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'" ... "pixelformat=UYVY" / "pixelformat=BGR3"; reporter table: "UYVY8_1X16/1920x1080 colorspace:smpte170m (no field) | fails, -EPIPE"

### C-34

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, TC358743, Linux
- **Fact:** For MEDIA_BUS_FMT_RGB888_1X24 the two receivers use different fourccs. The downstream rp1_cfe (Pi 5) maps it to V4L2_PIX_FMT_BGR24 (and BGR888_1X24 to RGB24). The downstream Unicam (Pi 4/CM4) still maps it to V4L2_PIX_FMT_RGB24 (and BGR888_1X24 to BGR24). Both map UYVY8_1X16 to V4L2_PIX_FMT_UYVY (CSI-2 DT YUV422_8B).
- **Source:** raspberrypi/linux rpi-6.18.y rp1_cfe/cfe_fmts.h and bcm2835/bcm2835-unicam.c format tables; PR #7406 — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe_fmts.h>
- **Evidence:** cfe_fmts.h:65-75 ".fourcc = V4L2_PIX_FMT_RGB24, /* rgb */ .code = MEDIA_BUS_FMT_BGR888_1X24" / ".fourcc = V4L2_PIX_FMT_BGR24, /* bgr */ .code = MEDIA_BUS_FMT_RGB888_1X24"; bcm2835-unicam.c:242-248 ".fourcc = V4L2_PIX_FMT_RGB24, /* rgb */ .code = MEDIA_BUS_FMT_RGB888_1X24"; PR #7406 (6by9): "Ideally we do want to fix the downstream Unicam driver too... That one can follow later"

### C-35

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** TC358743, CM4, Pi4, Linux
- **Fact:** On CM4, TC358743 RGB24 capture was reported (issue #6068) to put bytes in memory as B,G,R. 6by9 explained that MEDIA_BUS_FMT_RGB888_1X24 puts blue in the LSBs and CSI-2 sends blue first, and that users had been swapping RGB and BGR with GStreamer capssetter.
- **Source:** raspberrypi/linux issue #6068 'TC358743 produces BGR instead of RGB' (closed 2025-11-07) — <https://github.com/raspberrypi/linux/issues/6068>
- **Evidence:** reporter: "it produces BGR sequence in the raw data instead of RGB"; 6by9: "it is putting B in the LSBs of the bus. CSI-2 does send the data LSB and blue first" and "there are various forum threads where GStreamer's capssetter is being used to swap bgr to rgb when using tc358743"

### C-36

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The downstream Unicam selects Media Controller mode if any of these is set: the media_controller module parameter, compatible match data ("brcm,bcm2835-unicam"), or the DT bool "brcm,media-controller". In legacy mode the video node forwards EDID and DV_TIMINGS ioctls to the subdev, and subdev nodes are registered read-only. In MC mode the video node lacks these ioctls; configure them through a read-write /dev/v4l-subdevN. The image node is named "unicam-image" and gets an IMMUTABLE|ENABLED link straight from the sensor pad, with no CSI-2 receiver subdev in between. "unicam-embedded" is registered only if the source has 2 or more source pads, so with the single-pad TC358743 only "unicam-image" exists.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/platform/bcm2835/bcm2835-unicam.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/bcm2835/bcm2835-unicam.c>
- **Evidence:** bcm2835-unicam.c:3300-3307 "unicam->mc_api = media_controller; ... if (of_device_get_match_data(...)) unicam->mc_api = true; if (of_property_read_bool(pdev->dev.of_node, \"brcm,media-controller\"))"; :1775-1779 .vidioc_s_dv_timings etc. in legacy ops; unicam_mc_ioctl_ops has no dv_timings/edid; :2963 "%s-%s", "unicam", "image"/"embedded"; :3052-3056 IMMUTABLE|ENABLED link
- **Original claim (before verification):** The downstream Unicam chooses Media Controller mode if any of these is set: the module parameter media_controller, compatible match data ("brcm,bcm2835-unicam"), or the DT bool "brcm,media-controller". In legacy mode the video node forwards EDID and DV_TIMINGS ioctls to the subdev. In MC mode those ioctls are absent from the video node (configure via /dev/v4l-subdevN). The video nodes are "unicam-image" and "unicam-embedded", each with an IMMUTABLE|ENABLED link from the sensor and no CSI-2 receiver subdev in between.
- **Verifier note:** bcm2835-unicam.c:3299-3307 handles the mode selection. :1763-1779 are the legacy ioctl ops, and :1994-2020 unicam_mc_ioctl_ops has no dv/edid. :2963 sets the node names. :3115-3131 registers METADATA_PAD only when 'source_pads >= 2'. :3051-3056 creates the link only for the image pad or when there is embedded data. :3133-3136 registers subdev nodes read-write in MC mode and with v4l2_device_register_ro_subdev_nodes otherwise. The original claim that both nodes exist with links was wrong for TC358743.

### C-37

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** TC358743, Pi4, CM4
- **Fact:** Official RPi docs on TC358743: (1) with 2 CSI-2 lanes the maximum is 1080p30 RGB888 or 1080p50 YUV422; (2) with 4 lanes on a Compute Module, 1080p60 works in either format; (3) only RGB888 and YUV422 have been tested; (4) the user must supply the EDID via VIDIOC_S_EDID (e.g. v4l2-ctl --set-edid); (5) timings are set via DV_TIMINGS (v4l2-ctl --set-dv-bt-timings query), and VIDIOC_S_FMT changes only the pixel format. The overlay is tc358743, and audio needs tc358743-audio in addition.
- **Source:** RPi documentation camera/csi-2-usage.adoc 'Currently supported devices' — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/csi-2-usage.adoc>
- **Evidence:** "When using two CSI-2 lanes, the maximum rates that can be supported are 1080p30 as RGB888, or 1080p50 as YUV422. When using four lanes on a Compute Module, 1080p60 can be received in either format."; "it is up to the user to provide a suitable file through the VIDIOC_S_EDID ioctl"; "This driver is loaded using the `config.txt` dtoverlay `tc358743`."

### C-38

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** TC358743, Pi5, CM5
- **Fact:** The official RPi documentation has no Raspberry Pi 5-specific TC358743 instructions. Its only TC358743 text is in the Unicam section (csi-2-usage.adoc), and the only 4-lane case it describes is a Compute Module.
- **Source:** raspberrypi/documentation master @858e9bce (2026-10-05), grep of documentation/asciidoc/computers — <https://github.com/raspberrypi/documentation/tree/master/documentation/asciidoc/computers>
- **Evidence:** grep -rli "tc358743\|hdmi to csi" documentation/asciidoc -> only camera/csi-2-usage.adoc, which is under '== Unicam'

### C-39

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, CM4, Pi5, CM5
- **Fact:** The official docs require camera_auto_detect=0 (or removing camera_auto_detect=1) in /boot/firmware/config.txt to use the listed camera-sensor overlays (imx219/imx477/imx296/imx708/imx290/imx378/ov9281). camera_auto_detect makes the firmware auto-load overlays for recognised CSI cameras. The docs do not state this requirement for tc358743 specifically; disabling it for TC358743 is prudent but is not an explicitly documented requirement. On boards with two connectors (Pi 5, Compute Modules), appending ,cam0 selects connector 0; otherwise the overlay defaults to connector 1.
- **Source:** RPi documentation camera/rpicam_configuration.adoc and config_txt/common.adoc — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/rpicam_configuration.adoc>
- **Evidence:** "To use one of these overlays, you must disable automatic camera detection. To disable automatic detection, set `camera_auto_detect=0`"; "you can specify the use of camera connector 0 by adding `,cam0` to the `dtoverlay`... If you do not add this, it will default to checking camera connector 1."
- **Original claim (before verification):** Using a manually specified camera overlay requires camera_auto_detect=0 (or removing camera_auto_detect=1) in /boot/firmware/config.txt. On boards with two connectors (Pi 5, Compute Modules), appending ,cam0 selects connector 0; otherwise the overlay defaults to connector 1.
- **Verifier note:** rpicam_configuration.adoc:39 reads 'To use one of these overlays, you must disable automatic camera detection', where 'these overlays' is the sensor table above it, which does not include tc358743. :41 covers ,cam0. config_txt/common.adoc:13-19 describes camera_auto_detect. Counter-evidence: a Dec 2025 community comment on #5764 reports tc358743-pi5 probing with camera_auto_detect=1 left on. HARDWARE TEST REQUIRED to confirm firmware auto-detect does not interfere with the TC358743 I2C bus.

### C-40

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, Linux
- **Fact:** The vc4-kms-v3d overlay (and the cma overlay) take CMA size parameters cma-64, cma-96, cma-128, cma-192 ... cma-512 and cma-size (bytes, 4 MB aligned); cma-192 and above need 1 GB. Defaults: the generic cma overlay sets 256 MB, vc4-kms-v3d-pi4 sets (512-4) MB, vc4-kms-v3d-pi5 sets 64 MB, and the bcm2712 base DT CMA pool is 64 MB within the first 1 GB.
- **Source:** raspberrypi/linux rpi-6.18.y overlays README, cma-overlay.dts, vc4-kms-v3d-pi4/pi5-overlay.dts, bcm2712.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/cma-overlay.dts>
- **Evidence:** README: "cma-96 CMA is 96MB" ... "cma-192 CMA is 192MB (needs 1GB)"; cma-overlay.dts "size = <0x10000000>;" (256 MB); vc4-kms-v3d-pi4-overlay.dts "&frag0 { size = <((512-4)*1024*1024)>; };"; vc4-kms-v3d-pi5-overlay.dts "&frag0 { size = <(64*1024*1024)>; };"; bcm2712.dtsi:180-185 "size = <0x0 0x4000000>; /* 64MB */ ... alloc-ranges = <0x0 0x00000000 0x0 0x40000000>"

### C-41

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi5, TC358743, Linux
- **Fact:** Raspberry Pi engineers have described TC358743 on Pi 5 as follows. It probes (B102 tested in Dec 2023), but Pi 5 must be configured through the Media Controller API, so VIDIOC_S_FMT alone does not set the resolution. The main tested Pi 5 camera case is raw sensors going through the CFE. libcamera does not support TC358743 because it is not a raw sensor.
- **Source:** raspberrypi/linux issue #5764 (6by9, 2023-12-01) and PR #7406 (6by9, 2026-06-11) — <https://github.com/raspberrypi/linux/issues/5764>
- **Evidence:** 6by9: "FWIW I have had a B102 probing properly, but ... Pi5 has to use Media Controller API so simply setting resolutions via V4L2 VIDIOC_S_FMT will not work. The main supported use case is for raw cameras that run through the Camera Front End (CFE)"; #7406: "This is for things like the Toshiba TC358743 HDMI to CSI2 bridge, which isn't supported by libcamera as it isn't a raw sensor."

### C-42

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi5, CM5, TC358743, Linux
- **Fact:** Open issue #7399 reports that the rp1-cfe video nodes do not deliver V4L2_EVENT_SOURCE_CHANGE raised by a CSI-2 source subdev. 6by9 replied that under Media Controller, applications should subscribe to source-change events on the source subdev (/dev/v4l-subdevN), not on /dev/video0.
- **Source:** raspberrypi/linux issue #7399 (open, created 2026-05-25) — <https://github.com/raspberrypi/linux/issues/7399>
- **Evidence:** 6by9 2026-05-26: "I would have expected you to be using the /dev/v4l-subdevN for your source subdev. I'm fairly certain that is how others using the Toshiba TC358743 on Pi5 have architected it."

### C-43

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** TC358743, CM4, Linux
- **Fact:** Open issue #6322 reports corrupted images at 1080p50 RGB24 on a 4-lane CM4, where the driver chose 3 lanes. 6by9 attributed this to the fixed FIFO trigger level (fifo_level=374), noting Toshiba's spreadsheet gives a minimum of 120, and to the lane formula using active height instead of total line time. With link-frequency=297000000, Unicam rejected the mode with "Device has requested 5 data lanes, which is >4 configured in DT".
- **Source:** raspberrypi/linux issue #6322 (open, created 2024-08-23) — <https://github.com/raspberrypi/linux/issues/6322>
- **Evidence:** log "Lanes needed: 3 / Lanes in use: 3"; 6by9: "I suspect it's the FIFO trigger point"; "I get min 120, max 511, which implies that the driver setting of 374 isn't being set"; "it's using bt->height in the calculation, therefore the time per line ... is going to be wrong"; "unicam fe801000.csi: Device has requested 5 data lanes, which is >4 configured in DT"

### C-44

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** A driver comment states that the default FIFO trigger level of 374 works at 594 Mbps for 720p60 (2 lanes) and 1080p60 (4 lanes) and for "most modes on 972Mbps". Another comment says 594 Mbps is meant for 4-lane 1080p60 or 2-lane 720p60, and that 972 Mbps allows 1080p50 UYVY over 2 lanes. The mainline torvalds/linux master tc358743.c is identical except for id-table syntax, so 972 Mbps support is upstream.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743.c probe comments; diff vs torvalds/linux master — <https://raw.githubusercontent.com/torvalds/linux/master/drivers/media/i2c/tc358743.c>
- **Evidence:** tc358743.c:2061-2070 "A value of 374 works with both those modes at 594Mbps, and with most modes on 972Mbps. state->pdata.fifo_level = 374;"; :2088-2090 "The default is 594 Mbps for 4-lane 1080p60 or 2-lane 720p60. 972 Mbps allows 1080P50 UYVY over 2-lane."; diff rpi vs mainline: only `{ "tc358743" }` vs `{ .name = "tc358743" }`

### C-45

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi5, CM5, TC358743
- **Fact:** 6by9 warned that the Auvidea B101 uses a 15-pin FFC with contacts on the same side, while Pi 5 has 22-pin connectors. A wrongly sided 22-to-15 adapter swaps pin 1 (GND) with pin 15 (3V3) and can short power rails and damage either board.
- **Source:** raspberrypi/linux issue #5764 comments by 6by9 — <https://github.com/raspberrypi/linux/issues/5764>
- **Evidence:** "B101 has a 15pin connector. Pi5 has a 22pin." ... "Having the incorrect sided cables effective means pin 1 is swapped with 15... Pin 1 is GND. Pin 15 is 3V3. Swapping those 2 is not going to work, and can potentially cause damage to either device."

### C-46

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, All
- **Fact:** CSI-2 payload is computed with the driver's formula (active pixels only). The fps values come from pixelclock/(htotal x vtotal): 1080p60 = 148.5 MHz/(2200x1125) = 60; 1080p50 = 148.5 MHz/(2640x1125) = 50; 1080p30 = 74.25 MHz/(2200x1125) = 30. Resulting payload: UYVY 16bpp is 1.990656 Gbps (p60), 1.658880 Gbps (p50), 0.995328 Gbps (p30); RGB888 24bpp is 2.985984 Gbps (p60), 2.488320 Gbps (p50), 1.492992 Gbps (p30). Link capacity at 2 x link frequency per lane: 297 MHz (594 Mbps/lane) gives 1.188 Gbps on 2 lanes and 2.376 Gbps on 4; 486 MHz (972 Mbps/lane) gives 1.944 Gbps on 2 lanes and 3.888 Gbps on 4.
- **Source:** Calculation; inputs: CEA timings from include/uapi/linux/v4l2-dv-timings.h, formula from tc358743.c:790-801, link freqs from tc358743.dtsi/README — <https://raw.githubusercontent.com/torvalds/linux/master/include/uapi/linux/v4l2-dv-timings.h>
- **Evidence:** V4L2_DV_BT_CEA_1920X1080P60: 148500000, 88/44/148, 4/5/36; ..._P50: 148500000, 528/44/148, 4/5/36 (VIC 31); ..._P30: 74250000, 88/44/148, 4/5/36; 1920*1080*60*16 = 1,990,656,000; 1920*1080*60*24 = 2,985,984,000; 2*486e6*4 = 3.888e9

### C-47

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, All
- **Fact:** Lanes the driver requests, DIV_ROUND_UP(payload, bps per lane). At 486 MHz (972 Mbps/lane): 1080p60 UYVY needs 3, 1080p60 RGB888 needs 4, 1080p50 UYVY needs 2, 1080p50 RGB888 needs 3, 1080p30 UYVY needs 2, 1080p30 RGB888 needs 2. At 297 MHz (594 Mbps/lane): 1080p60 UYVY needs 4, 1080p60 RGB888 needs 6, 1080p50 UYVY needs 3, 1080p50 RGB888 needs 5, 1080p30 UYVY needs 2, 1080p30 RGB888 needs 3. Any request above the DT data-lanes is rejected at STREAMON by Unicam or CFE (C-16).
- **Source:** Calculation using tc358743_num_csi_lanes_needed() and payloads from C-46 — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** e.g. ceil(1,990,656,000/972,000,000)=ceil(2.048)=3; ceil(2,985,984,000/594,000,000)=ceil(5.03)=6; cross-check: #6322 log shows "Lanes needed: 3" for 1080p50 RGB at 972 Mbps and "requested 5 data lanes" at 594 Mbps

### C-48

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Pi4, CM4, Pi5, CM5
- **Fact:** Feasibility on 2-lane links (Pi 4B, CM4 CAM0, any 2-lane TC358743 board) at the default 486 MHz: feasible are 1080p30 UYVY (51.2% of 1.944 Gbps), 1080p30 RGB888 (76.8%) and 1080p50 UYVY (85.3%). Not feasible are 1080p50 RGB888 (128%, needs 3 lanes), 1080p60 UYVY (102.4%, needs 3 lanes) and 1080p60 RGB888 (153.6%, needs 4 lanes). At 297 MHz on 2 lanes only 1080p30 UYVY fits (83.8%). This matches the official 2-lane limits of 1080p30 RGB888 and 1080p50 YUV422 (C-37).
- **Source:** Calculation from C-46/C-47; cross-checked against RPi docs csi-2-usage.adoc — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/csi-2-usage.adoc>
- **Evidence:** 1.658880/1.944=0.853; 1.990656/1.944=1.024; 2.488320/1.944=1.280; 0.995328/1.188=0.838; docs: "When using two CSI-2 lanes, the maximum rates that can be supported are 1080p30 as RGB888, or 1080p50 as YUV422."

### C-49

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, CM4, Pi5, CM5
- **Fact:** Feasibility on 4-lane links (CM4 CAM1, Pi 5 either port, CM5 MIPI0/1) at the default 486 MHz: all six 1080p50/60/30 x UYVY/RGB888 combinations fit, the highest being 1080p60 RGB888 at 76.8% of 3.888 Gbps on 4 lanes. The driver activates only 2-4 lanes, so not every mode uses all four. At 297 MHz on 4 lanes, 1080p60 UYVY (4 lanes, 83.8%), 1080p50 UYVY (3) and both 1080p30 formats fit, but 1080p50 RGB888 (5 lanes) and 1080p60 RGB888 (6 lanes) are rejected. 1080p50 RGB888 on a 4-lane CM4 is bandwidth-feasible but has an open image-corruption report (C-43).
- **Source:** Calculation from C-46/C-47; cross-checked against RPi docs and issues #6322/#7523 — <https://github.com/raspberrypi/linux/issues/7523>
- **Evidence:** 2.985984/3.888=0.768; 1.990656/2.376=0.838; docs "When using four lanes on a Compute Module, 1080p60 can be received in either format"; #7523: Pi 5 4-lane BGR888 1080p60 "streams 60.14 fps" and UYVY 1080p60 works with correct pad setup; 6by9: "With a Pi5 and suitable TC358743 board with 4 lanes, it makes little difference."

### C-50

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, All
- **Fact:** Per-line cross-check (stricter than the driver's frame-average formula) at 972 Mbps/lane. HDMI line periods: 1080p60 = 2200/148.5 MHz = 14.815 us; 1080p50 = 2640/148.5 MHz = 17.778 us; 1080p30 = 2200/74.25 MHz = 29.630 us. Transmitting one active line takes 15.80 us for UYVY and 23.70 us for RGB888 on 2 lanes, and 7.90 us and 11.85 us on 4 lanes. So 2-lane 1080p60 UYVY and 2-lane 1080p50 RGB888 cannot keep up. 2-lane 1080p50 UYVY fits with about 2.0 us of slack before LP/HS overhead (about 675 ns per 6by9's reading of Toshiba's spreadsheet). That spreadsheet's 898.12 Mbps/lane minimum for this mode leaves about 7.6% margin at 972 Mbps.
- **Source:** Calculation; inputs: CEA timings (v4l2-dv-timings.h), 1920 px x bpp per line, 6by9 spreadsheet figures in issue #6322 — <https://github.com/raspberrypi/linux/issues/6322>
- **Evidence:** 30720 bits/(2*972e6)=15.80us > 14.815us; 46080/(2*972e6)=23.70us > 17.778us; 17.778-15.80=1.98us; 6by9: "It's claiming a minimum lane speed of 898.12Mbit/s/lane on 2 lanes" and "it computes the LP<>HS transition time in B61 as 675ns"

### C-51

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** TC358743, Pi4, CM4, Pi5, CM5, Linux
- **Fact:** Because the stock tc358743 overlay consumes no camera regulator (C-23), and regulator-fixed drives its enable GPIO low at probe unless regulator-boot-on is set, the connector CAM_GPIO/power-enable line is expected to stay LOW while TC358743 is in use. This applies on Pi 4 (expgpio 5), Pi 5 (RP1 GPIO 34/46) and CM5 (RP1 GPIO 34).
- **Source:** Inference from drivers/regulator/fixed.c and the DTS regulator nodes (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/regulator/fixed.c>
- **Evidence:** fixed.c:195-196 "if (init_data->constraints.boot_on) config->enabled_at_boot = true;" and :299-302 "if (config->enabled_at_boot) gflags = GPIOD_OUT_HIGH; else gflags = GPIOD_OUT_LOW;"; cam1_reg/cam0_reg nodes have no regulator-boot-on (C-21, C-22)

### C-52

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi5, CM5, TC358743
- **Fact:** On the Pi 5 downstream CFE the D-PHY is always configured for 999 Mbps when the source is TC358743 (C-31). That matches the default 972 Mbps link, but with link-frequency=297000000 (594 Mbps) the RP1 hsfreqrange setting would not match the actual lane rate.
- **Source:** Inference from rp1_cfe/cfe.c sensor_link_rate() and dphy.c hsfreqrange table — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/dphy.c>
- **Evidence:** dphy.c:114-128 table rows "{ 599, 0b010111 }" vs "{ 999, 0b011010 }"; cfe.c returns 999*1000000UL for tc358743

### C-53

- **Verdict:** `CORRECTED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi5, CM5, Pi4, CM4, Linux
- **Fact:** Single-frame sizes are 4,147,200 bytes for 1920x1080 UYVY and 6,220,800 bytes for RGB888/BGR3, so four capture buffers take about 16.6 MB (UYVY) or 24.9 MB (RGB888). On Pi 4/CM4, Unicam (videobuf2-dma-contig, no IOMMU) allocates these from CMA. On Pi 5/CM5 the DT gives the RP1 CSI nodes iommus = <&iommu5> (with CONFIG_BCM2712_IOMMU=y), and the HVS uses iommu4. So it is not established that CFE capture buffers or display buffers count against the 64 MB vc4-kms-v3d-pi5 CMA default. KERNEL SOURCE INSPECTION or a runtime measurement (/proc/meminfo CmaFree while streaming) is required.
- **Source:** Calculation; inputs: frame geometry, bpp, CMA defaults from C-40 — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/vc4-kms-v3d-pi5-overlay.dts>
- **Verifier's best source:** <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts>
- **Evidence:** 1920*1080*2=4,147,200; 1920*1080*3=6,220,800; vc4-kms-v3d-pi5 frag0 size = 64 MB
- **Original claim (before verification):** Single-frame sizes are 4,147,200 bytes for 1920x1080 UYVY and 6,220,800 bytes for RGB888/BGR3. Four capture buffers therefore take about 16.6 MB (UYVY) or 24.9 MB (RGB888). On Pi 5 that is a large share of the 64 MB vc4-kms-v3d-pi5 CMA default, which is also used by display and other DMA users.
- **Verifier note:** I re-checked the arithmetic: 1920*1080*2 = 4,147,200; *3 = 6,220,800; ×4 = 16.59 MB and 24.88 MB (decimal). bcm2712-rpi-5-b.dts:241-249 and bcm2712-rpi-cm5.dtsi:220-226 set iommus=<&iommu5> on csi0/csi1. bcm2712.dtsi:613 sets the HVS iommus=<&iommu4>. bcm2712_defconfig:1568 has CONFIG_BCM2712_IOMMU=y. The original claim's assumption that Pi 5 capture and display both draw on the 64 MB CMA was unsupported.

---

## Topic D

**Video encoding on Pi 4/CM4 and Pi 5/CM5** — <a id="topic-d"></a>54 claims.

### D-01

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux
- **Fact:** As of 2026-10-06 the default branch of github.com/raspberrypi/linux is rpi-6.18.y; all raspberrypi/linux kernel-source claims in this topic were read from that branch unless stated otherwise.
- **Source:** GitHub API: repos/raspberrypi/linux — <https://api.github.com/repos/raspberrypi/linux>
- **Verifier's best source:** <https://github.com/raspberrypi/linux>
- **Evidence:** "default_branch": "rpi-6.18.y" (pushed_at 2026-10-06T09:07:16Z)

### D-02

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The Pi 4 hardware video codec V4L2 driver is still present in rpi-6.18.y at drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c, built as module bcm2835-codec.ko under Kconfig symbol VIDEO_CODEC_BCM2835 (tristate "BCM2835 Video codec support"), which depends on MEDIA_SUPPORT && MEDIA_CONTROLLER and VIDEO_DEV && (ARCH_BCM2835 || COMPILE_TEST) and selects BCM2835_VCHIQ_MMAL, VIDEOBUF2_DMA_CONTIG and V4L2_MEM2MEM_DEV.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/staging/vc04_services/bcm2835-codec/Kconfig and Makefile — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/Kconfig>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/Kconfig>
- **Evidence:** config VIDEO_CODEC_BCM2835 / tristate "BCM2835 Video codec support" / select BCM2835_VCHIQ_MMAL / select VIDEOBUF2_DMA_CONTIG / select V4L2_MEM2MEM_DEV; Makefile: obj-$(CONFIG_VIDEO_CODEC_BCM2835) += bcm2835-codec.o

### D-03

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux, Buildroot
- **Fact:** Kconfig BCM2835_VCHIQ_MMAL (selected by both VIDEO_CODEC_BCM2835 and VIDEO_ISP_BCM2835) itself selects BCM2835_VCHIQ and BCM_VC_SM_CMA, so the codec/ISP drivers pull in the VCHIQ core and vc-sm-cma shared-memory driver.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/staging/vc04_services/vchiq-mmal/Kconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/vchiq-mmal/Kconfig>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/vchiq-mmal/Kconfig>
- **Evidence:** config BCM2835_VCHIQ_MMAL ... select BCM2835_VCHIQ / select BCM_VC_SM_CMA

### D-04

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, Linux
- **Fact:** Both arm64 defconfigs bcm2711_defconfig and bcm2712_defconfig on rpi-6.18.y set CONFIG_BCM2835_VCHIQ=y, CONFIG_VIDEO_CODEC_BCM2835=m and CONFIG_VIDEO_ISP_BCM2835=m.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/configs/bcm2711_defconfig, bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2711_defconfig>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm64/configs/bcm2712_defconfig>
- **Evidence:** bcm2711_defconfig:1553 CONFIG_BCM2835_VCHIQ=y, :1556 CONFIG_VIDEO_CODEC_BCM2835=m, :1557 CONFIG_VIDEO_ISP_BCM2835=m; bcm2712_defconfig:1555/1558/1559 same values

### D-05

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi4, CM4
- **Fact:** bcm2835-codec is downstream-only: mainline torvalds/linux master drivers/staging/vc04_services/Kconfig sources only bcm2835-audio/Kconfig, and drivers/staging/vc04_services/bcm2835-codec/Kconfig does not exist in mainline (HTTP 404).
- **Source:** torvalds/linux master drivers/staging/vc04_services/Kconfig — <https://github.com/torvalds/linux/blob/master/drivers/staging/vc04_services/Kconfig>
- **Verifier's best source:** <https://raw.githubusercontent.com/torvalds/linux/master/drivers/staging/vc04_services/Kconfig>
- **Evidence:** only 'source "drivers/staging/vc04_services/bcm2835-audio/Kconfig"'; raw.githubusercontent.com/torvalds/linux/master/drivers/staging/vc04_services/bcm2835-codec/Kconfig -> 404

### D-06

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** bcm2835-codec registers five M2M video devices with requested node numbers set by module parameters: decode_video_nr=10, encode_video_nr=11, isp_video_nr=12, deinterlace_video_nr=18, encode_image_nr=31 (JPEG).
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 52-70: static int decode_video_nr = 10; encode_video_nr = 11; isp_video_nr = 12; deinterlace_video_nr = 18; encode_image_nr = 31;

### D-07

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Official Raspberry Pi documentation lists the codec/ISP device nodes as: video10 Video decode, video11 Video encode, video12 Simple ISP (conversion and resizing between RGB/YUV formats), video13 input to fully programmable ISP, video14/video15 high/low resolution ISP outputs, video16 ISP statistics, video19 HEVC decode.
- **Source:** Raspberry Pi documentation - Camera software - V4L2 drivers — <https://www.raspberrypi.com/documentation/computers/camera_software.html#v4l2-drivers>
- **Evidence:** | `video11` | Video encode ... | `video12` | Simple ISP, can perform conversion and resizing between RGB/YUV formats in addition to Bayer to RGB/YUV conversion ... | `video19` | HEVC decode

### D-08

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The encoder node is a V4L2 multiplanar M2M device (V4L2_CAP_VIDEO_M2M_MPLANE | V4L2_CAP_STREAMING), media entity function MEDIA_ENT_F_PROC_VIDEO_ENCODER, card name bcm2835-codec-encode, backed by the VideoCore firmware MMAL component "ril.video_encode" over VCHIQ.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** line 112 "ril.video_encode"; line 3801 vfd->device_caps = V4L2_CAP_VIDEO_M2M_MPLANE | V4L2_CAP_STREAMING; line 3823 function = MEDIA_ENT_F_PROC_VIDEO_ENCODER; snprintf(vfd->name, "%s-%s", bcm2835_codec_videodev.name, roles[role])

### D-09

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The bcm2835-codec decode/encode roles clamp frame size to MAX_W_CODEC=1920 x MAX_H_CODEC=1920 (minimum 32x32); only the ISP role uses 16384x16384. 4K H.264 hardware encode is therefore not possible on Pi 4/CM4.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 121-126: #define MIN_W 32 / MIN_H 32 / MAX_W_CODEC 1920 / MAX_H_CODEC 1920 / MAX_W_ISP 16384 / MAX_H_ISP 16384; lines 3808-3834 dev->max_w = MAX_W_CODEC ... case ISP: dev->max_w = MAX_W_ISP

### D-10

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, CM4
- **Fact:** The official BCM2711 (Pi 4B, CM4) multimedia specification is H.265 4Kp60 decode and H.264 1080p60 decode / 1080p30 encode.
- **Source:** Raspberry Pi documentation - Processors - BCM2711; Raspberry Pi 4 and CM4 product briefs — <https://www.raspberrypi.com/documentation/computers/processors.html#bcm2711>
- **Verifier's best source:** <https://datasheets.raspberrypi.com/rpi4/raspberry-pi-4-product-brief.pdf>
- **Evidence:** *Multimedia:* H.265 (4Kp60 decode); H.264 (1080p60 decode, 1080p30 encode); also verbatim in raspberry-pi-4-product-brief.pdf and cm4-product-brief.pdf: "Multimedia H.265 (4Kp60 decode); H.264 (1080p60 decode, 1080p30 encode)"

### D-11

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The encoder exposes V4L2_CID_MPEG_VIDEO_H264_PROFILE with Baseline, Constrained Baseline, Main and High (default High).
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 3473-3479: V4L2_CID_MPEG_VIDEO_H264_PROFILE, max V4L2_MPEG_VIDEO_H264_PROFILE_HIGH, mask ~(BASELINE|CONSTRAINED_BASELINE|MAIN|HIGH), default V4L2_MPEG_VIDEO_H264_PROFILE_HIGH

### D-12

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The encoder H.264 level menu accepts levels 1.0 to 5.1 (default 4.0), but the driver states the hardware spec is level 4.0 and higher levels exist only to signal correct headers and may not keep up with real time.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** line 2233: "Note that the hardware spec is level 4.0. Levels above that are there for correctly encoding the headers and may not be able to keep up with real-time."; lines 3452-3471 menu max LEVEL_5_1, default LEVEL_4_0

### D-13

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Encoder bitrate control V4L2_CID_MPEG_VIDEO_BITRATE ranges 25,000 to 25,000,000 bps (step 25,000, default 10,000,000); V4L2_CID_MPEG_VIDEO_BITRATE_MODE supports VBR (default) and CBR.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 3433-3439: BITRATE_MODE menu max CBR default VBR; V4L2_CID_MPEG_VIDEO_BITRATE, 25 * 1000, 25 * 1000 * 1000, 25 * 1000, 10 * 1000 * 1000

### D-14

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The Pi 4 hardware encoder produces no B-frames (V4L2_CID_MPEG_VIDEO_B_FRAMES min 0 max 0), GOP size defaults to 60, and every I-frame is an IDR frame.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 3491-3497: V4L2_CID_MPEG_VIDEO_B_FRAMES, 0, 0, 1, 0; V4L2_CID_MPEG_VIDEO_GOP_SIZE, 0, 0x7FFFFFFF, 1, 60; line 2334: "the MMAL encoder never produces I-frames that aren't IDR frames"

### D-15

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux, Streaming
- **Fact:** V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER (SPS/PPS inline with every IDR) defaults to 0 (off) on the Pi 4 encoder; other controls are H264_MIN_QP (0-51, default 20), H264_MAX_QP (default 51), FORCE_KEY_FRAME and HEADER_MODE (joined with 1st frame).
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 3444-3446: V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER, 0, 1, 1, 0; lines 3480-3490 H264_MIN_QP 0,51,1,20; H264_MAX_QP 0,51,1,51; FORCE_KEY_FRAME

### D-16

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The encoder's accepted input (OUTPUT queue) pixel formats are not fixed in the driver: at probe the driver queries the firmware port with MMAL_PARAMETER_SUPPORTED_ENCODINGS and keeps those matching its static table (YUV420, YVU420, NV12, NV21, RGB565, YUYV, UYVY, YVYU, VYUY, NV12_COL128, RGB24, BGR24, BGR32, RGBA32, Bayer and grey formats).
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 3670-3714: vchiq_mmal_port_parameter_get(..., &component->input[0], MMAL_PARAMETER_SUPPORTED_ENCODINGS, ...); for each: fmt = get_fmt(fourccs[i]); static supported_formats[] lines 170-640

### D-17

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi4, CM4, Linux, TC358743
- **Fact:** A Raspberry Pi engineer's 2021 listing of v4l2-ctl --list-formats-out -d 11 (bcm2835-codec-encode, Video Output Multiplanar) showed 13 input formats: YU12, YV12, NV12, NV21, RGBP, RGB3, BGR3, XB24, XR24, YUYV, YVYU, UYVY, VYUY.
- **Source:** Raspberry Pi Forums - ffmpeg to capture uvc to file (6by9, 2021-04-09) — <https://forums.raspberrypi.com/viewtopic.php?t=309098>
- **Evidence:** pi@raspberrypi:~ $ v4l2-ctl --list-formats-out -d 11 ... [0]: 'YU12' ... [11]: 'UYVY' (UYVY 4:2:2) [12]: 'VYUY' (VYUY 4:2:2)

### D-18

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi4, CM4, TC358743
- **Fact:** A Raspberry Pi engineer stated that both TC358743 kernel-driver output formats, RGB888 (24bpp) and UYVY (YUV422), are supported directly by the MMAL and V4L2 video encoder.
- **Source:** Raspberry Pi Forums - Encoding Toshiba TC358743 video (6by9) — <https://forums.raspberrypi.com/viewtopic.php?t=323465>
- **Evidence:** "The kernel driver will produce RGB888 (24bpp) or UYVY (YUV422), both of which are supported by the MMAL (and V4L2) video encoder."

### D-19

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Both queues of every bcm2835-codec M2M device (OUTPUT_MPLANE input and CAPTURE_MPLANE output) support io_modes VB2_MMAP | VB2_DMABUF using vb2_dma_contig_memops, with V4L2_BUF_FLAG_TIMESTAMP_COPY (input timestamps copied to encoded buffers).
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) queue_init() — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 3243-3264: src_vq->type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE; src_vq->io_modes = VB2_MMAP | VB2_DMABUF; src_vq->mem_ops = &vb2_dma_contig_memops; src_vq->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY; dst_vq ... same

### D-20

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Because the codec queues use videobuf2-dma-contig, an imported DMABUF must be a single DMA-contiguous region at least as large as the plane; otherwise import fails with "contiguous chunk is too small".
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/common/videobuf2/videobuf2-dma-contig.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/common/videobuf2/videobuf2-dma-contig.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/common/videobuf2/videobuf2-dma-contig.c>
- **Evidence:** lines 714-718: contig_size = vb2_dc_get_contiguous_size(sgt); if (contig_size < buf->size) { pr_err("contiguous chunk is too small %lu/%lu\n", ...

### D-21

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** bcm2835-codec always uses one memory plane (num_planes = 1) even for planar YUV420/NV12, so the encoder imports a single DMABUF fd that holds all planes contiguously.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) vidioc_try_fmt() — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** line 1506: f->fmt.pix_mp.num_planes = 1;

### D-22

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** For the ENCODE role the driver rounds bytesperline up to a 64-byte multiple for YUV420/YVU420 and for YUYV/UYVY/YVYU/VYUY, and to 32 bytes for NV12/NV21/RGB24/BGR24. It does not align encoder height to 16; only DECODE and ENCODE_IMAGE do.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** YUV420 .bytesperline_align = { 32, 64, 64, 32, 32 } (index 1 = ENCODE); NV12 { 32, 32, 32, 32, 32 }; UYVY { 64, 64, 64, 64, 64 }; line 1499-1503: "For decoders and image encoders the buffer must have a vertical alignment of 16 lines" if (role == DECODE || role == ENCODE_IMAGE)

### D-23

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Default encoded (CAPTURE) buffer sizeimage is 768 KiB when width*height > 1280*720 and 512 KiB otherwise; clients may request a larger sizeimage. The driver notes that some 1080p frames exceed 512 KiB.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** lines 145-150: "The 1080P version of Big Buck Bunny has some frames that exceed 512kB." #define DEF_COMP_BUF_SIZE_GREATER_720P (768 << 10) / DEF_COMP_BUF_SIZE_720P_OR_LESS (512 << 10); get_sizeimage(): if (width * height > 1280 * 720)

### D-24

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, Linux
- **Fact:** Pi 4/CM4 has no HEVC encoder. bcm2835-codec's compressed formats are only H264, JPEG, MJPEG, MPEG4, H263, MPEG2 and VC1_ANNEX_G, with no HEVC. The separate Raspberry Pi HEVC driver (Kconfig VIDEO_RPI_HEVC_DEC, module rpi-hevc-dec) is a stateless decoder only.
- **Source:** bcm2835-v4l2-codec.c and drivers/media/platform/raspberrypi/hevc_dec/Kconfig (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/hevc_dec/Kconfig>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/media/platform/raspberrypi/hevc_dec/Kconfig>
- **Evidence:** codec.c lines 603-640 /* Compressed formats */ H264, JPEG, MJPEG, MPEG4, H263, MPEG2, VC1_ANNEX_G; hevc_dec/Kconfig: "Support for the Raspberry Pi HEVC / H265 H/W decoder as a stateless V4L2 decoder device."

### D-25

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The bcm2835-codec ISP role (/dev/video12) is a simple M2M converter/scaler (MEDIA_ENT_F_PROC_VIDEO_SCALER, MMAL component ril.isp, up to 16384x16384) with MMAP/DMABUF on both queues. It is a candidate for UYVY to YUV420/NV12 conversion between capture and encoder on Pi 4/CM4.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) bcm2835_codec_create() — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** case ISP: ... function = MEDIA_ENT_F_PROC_VIDEO_SCALER; video_nr = isp_video_nr; dev->max_w = MAX_W_ISP; dev->max_h = MAX_H_ISP; components[] "ril.isp"

### D-26

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** The separate bcm2835-isp driver (Kconfig VIDEO_ISP_BCM2835, module bcm2835-isp) creates 2 instances with base node numbers {13, 20}. Each instance has 1 output node, 2 capture nodes and 1 stats (metadata) node, all queues with VB2_MMAP | VB2_DMABUF. Frame dimensions are 64..16384 and the format table includes UYVY, YUV420 and NV12.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/staging/vc04_services/bcm2835-isp/ — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-isp/bcm2835-v4l2-isp.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-isp/bcm2835-v4l2-isp.c>
- **Evidence:** line 42: video_nr[BCM2835_ISP_NUM_INSTANCES] = { 13, 20 }; BCM2835_ISP_NUM_OUTPUTS 1, NUM_CAPTURES 2, NUM_METADATA 1; line 1399 queue->io_modes = VB2_MMAP | VB2_DMABUF; MAX_DIM 16384U, MIN_DIM 64U; bcm2835-isp-fmts.h lists V4L2_PIX_FMT_UYVY, YUV420, NV12

### D-27

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Linux, TC358743
- **Fact:** bcm2835-codec also provides a deinterlace M2M device (/dev/video18, MMAL ril.image_fx). It uses the advanced deinterlace algorithm only when source crop width <= 800 (module param advanced_deinterlace default true) and the fast algorithm otherwise, which includes 1920-wide 1080i.
- **Source:** bcm2835-v4l2-codec.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/bcm2835-v4l2-codec.c>
- **Evidence:** advanced_deinterlace && ctx->q_data[V4L2_M2M_SRC].crop_width <= 800 ? MMAL_PARAM_IMAGEFX_DEINTERLACE_ADV : MMAL_PARAM_IMAGEFX_DEINTERLACE_FAST

### D-28

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi4, CM4, Pi5, CM5, Linux
- **Fact:** The VCHIQ platform driver (which registers the child devices "bcm2835-codec" and "bcm2835-isp") matches only brcm,bcm2835-vchiq, brcm,bcm2836-vchiq and brcm,bcm2711-vchiq. Pi 4 DTs set compatible "brcm,bcm2711-vchiq" (bcm2711-rpi-ds.dtsi). No vchiq node appears in bcm2712.dtsi, bcm2712-ds.dtsi, bcm2712-rpi.dtsi, bcm2712-rpi-5-b.dts or bcm2712-rpi-cm5.dtsi.
- **Source:** vchiq_arm.c and BCM2711/BCM2712 device trees (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/interface/vchiq_arm/vchiq_arm.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/interface/vchiq_arm/vchiq_arm.c>
- **Evidence:** vchiq_arm.c:1398-1401 vchiq_of_match = { "brcm,bcm2835-vchiq", "brcm,bcm2836-vchiq", "brcm,bcm2711-vchiq" }; :1457 vchiq_device_register(&pdev->dev, "bcm2835-codec"); bcm2711-rpi-ds.dtsi:171-172 &vchiq { compatible = "brcm,bcm2711-vchiq"; }; grep -i vchiq in bcm2712 DT files only hits DMA-channel comments

### D-29

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi5, CM5, Linux
- **Fact:** Inference from D-28: on Pi 5/CM5 with rpi-6.18.y DTs, bcm2835-codec cannot probe, so /dev/video11 (bcm2835-codec-encode) does not exist even though CONFIG_VIDEO_CODEC_BCM2835=m is in bcm2712_defconfig.
- **Source:** Reasoning from D-04 and D-28 — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/interface/vchiq_arm/vchiq_arm.c>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/drivers/staging/vc04_services/interface/vchiq_arm/vchiq_arm.c>
- **Evidence:** Inputs: bcm2835-codec is a vchiq child device (vchiq_arm.c:1457); vchiq_of_match has no bcm2712 entry; BCM2712 DTs contain no vchiq node; bcm2712_defconfig:1558 CONFIG_VIDEO_CODEC_BCM2835=m

### D-30

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi5, CM5
- **Fact:** Official BCM2712 (Pi 5) codec statement: 4Kp60 HEVC hardware decode, and other codecs run in software. Stated CPU costs are H.264 1080p24 decode ~10-20% of CPU, 1080p60 decode ~50-60% of CPU, and 1080p30 encode (from ISP) ~30-40% CPU.
- **Source:** Raspberry Pi documentation - Processors - BCM2712 — <https://www.raspberrypi.com/documentation/computers/processors.html#bcm2712>
- **Evidence:** "* 4Kp60 HEVC hardware decode ** Other CODECs run in software ** H264 1080p24 decode ~10–20% of CPU ** H264 1080p60 decode ~50–60% of CPU ** H264 1080p30 encode (from ISP) ~30–40% CPU"

### D-31

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi5, CM5
- **Fact:** The Pi 5 product brief, CM5 product brief and CM5 datasheet list only a 4Kp60 HEVC decoder under video/multimedia, with no hardware video encoder.
- **Source:** Raspberry Pi 5 product brief; CM5 product brief; CM5 datasheet (Release 3, build date 08/06/2026) §1.3 — <https://datasheets.raspberrypi.com/cm5/cm5-datasheet.pdf>
- **Evidence:** Pi 5 brief: "4Kp60 HEVC decoder"; CM5 brief: "Multimedia 4Kp60 HEVC decoder"; CM5 datasheet 1.3 Features: "Video decoding. Hardware-accelerated 4kp60 HEVC video decoder." (no encode entry)

### D-32

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi5, CM5, Streaming
- **Fact:** Official rpicam-vid documentation: Raspberry Pi 5 uses software video encoders, which usually output frames with more latency than the old hardware encoders. --low-latency reduces latency by dropping B-frames and arithmetic coding, and 'will still easily achieve 1080p30'.
- **Source:** Raspberry Pi documentation - Camera software - rpicam-vid / low-latency option — <https://www.raspberrypi.com/documentation/computers/camera_software.html#rpicam-vid>
- **Evidence:** "Raspberry Pi 5 uses software video encoders. These generally output frames with a longer latency than the old hardware encoders" ... "The maximum framerate that can be encoded might be slightly reduced (though it will still easily achieve 1080p30)."; low-latency: "(for example, B frames and arithmetic coding will no longer be used)"

### D-33

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi4, CM4, Pi5, CM5
- **Fact:** rpicam-apps selects its H.264 encoder by platform. On VC4 (Pi 4, detected by a V4L2 card named "bcm2835-isp") it uses the hardware H264Encoder. On other platforms (PISP/Pi 5, detected by card "pispbe") it switches to libav with libav_video_codec = "libx264".
- **Source:** raspberrypi/rpicam-apps main encoder/encoder.cpp and core/options.cpp — <https://github.com/raspberrypi/rpicam-apps/blob/main/encoder/encoder.cpp>
- **Evidence:** if (options->GetPlatform() == Platform::VC4) return factory.CreateEncoder("h264")...; // No hardware codec available, use x264 through libav. options->Set().libav_video_codec = "libx264"; options.cpp: "bcm2835-isp" -> Platform::VC4, "pispbe" -> Platform::PISP

### D-34

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** rpicam-apps' Pi 4 hardware encoder path opens /dev/video11 and feeds V4L2_PIX_FMT_YUV420 frames on the OUTPUT queue with V4L2_MEMORY_DMABUF (zero-copy import of camera buffers). It reads the H.264 bitstream from MMAP CAPTURE buffers.
- **Source:** raspberrypi/rpicam-apps main encoder/h264_encoder.cpp — <https://github.com/raspberrypi/rpicam-apps/blob/main/encoder/h264_encoder.cpp>
- **Evidence:** const char device_name[] = "/dev/video11"; fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420; "The output queue (input to the encoder) shares buffers from our caller, these must be DMABUFs." reqbufs.memory = V4L2_MEMORY_DMABUF; capture reqbufs.memory = V4L2_MEMORY_MMAP

### D-35

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi5, CM5, Streaming
- **Fact:** rpicam-apps' libx264 defaults: in normal mode preset "superfast", max_b_frames=1, frame threading, partitions i8x8,i4x4. In --low-latency mode preset "ultrafast", tune "zerolatency", slice threading with 4 slices, refs=1. Both modes use motion-est dia and rc-lookahead 0.
- **Source:** raspberrypi/rpicam-apps main encoder/libav_encoder.cpp encoderOptionsLibx264() — <https://github.com/raspberrypi/rpicam-apps/blob/main/encoder/libav_encoder.cpp>
- **Evidence:** if (options->Get().low_latency) { codec->thread_type = FF_THREAD_SLICE; codec->slices = 4; codec->refs = 1; preset ultrafast; tune zerolatency } else { FF_THREAD_FRAME; max_b_frames = 1; preset superfast; partitions i8x8,i4x4 }; motion-est dia; rc-lookahead 0

### D-36

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi4, CM4, Pi5, CM5
- **Fact:** rpicam-apps forces H.264 level 4.2 when the macroblock rate exceeds 245,760 MB/s (for codec h264 or libav+libx264). 1080p60 gives 120 x 68 x 60 = 489,600 MB/s, which triggers level 4.2. 1080p30 gives 244,800 MB/s, which stays within that threshold.
- **Source:** raspberrypi/rpicam-apps main core/options.cpp + calculation — <https://github.com/raspberrypi/rpicam-apps/blob/main/core/options.cpp>
- **Evidence:** options.cpp: double mbps = ((width + 15) >> 4) * ((height + 15) >> 4) * framerate; if ((codec == "h264" || (codec == "libav" && libav_video_codec == "libx264")) && mbps > 245760.0) { level = "4.2"; } Calc: (1935>>4)=120, (1095>>4)=68

### D-37

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, Pi5, Streaming
- **Fact:** Official documentation gives a GStreamer streaming pipeline using v4l2h264enc extra-controls="controls,repeat_sequence_header=1" with caps video/x-h264,level=(string)4. It says that on Raspberry Pi 5 the encoder must be replaced with x264enc speed-preset=1 threads=1, and the decoder v4l2h264dec with avdec_h264.
- **Source:** Raspberry Pi documentation - Camera software - Streaming (GStreamer) — <https://www.raspberrypi.com/documentation/computers/camera_software.html#streaming>
- **Evidence:** "On a Raspberry Pi 5 device, replace `v4l2h264enc extra-controls="controls,repeat_sequence_header=1"` by `x264enc speed-preset=1 threads=1`." and "For a Raspberry Pi 5 device, replace `v4l2h264dec` by `avdec_h264`."

### D-38

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Linux, Streaming
- **Fact:** GStreamer's v4l2h264enc (gst-plugins-good video4linux2 plugin) is not a static element. It is registered at plugin load by probing M2M devices, under the name "v4l2h264enc" (or "v4l2<basename>h264enc" if that name is already taken), with rank GST_RANK_PRIMARY + 1 and klass "Codec/Encoder/Video/Hardware". Probing is compiled in only when the meson option v4l2-probe is true (upstream default true).
- **Source:** GStreamer gst-plugins-good sys/v4l2 gstv4l2videoenc.c, gstv4l2h264enc.c, gstv4l2.c, meson.options — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/main/subprojects/gst-plugins-good/sys/v4l2/gstv4l2videoenc.c>
- **Evidence:** gstv4l2videoenc.c:1403 type_name = g_strdup_printf ("v4l2%senc", codec_name); :1407 "v4l2%s%senc"; :1412 GST_RANK_PRIMARY + 1; gstv4l2h264enc.c:114-115 "V4L2 H.264 Encoder", "Codec/Encoder/Video/Hardware"; gstv4l2.c:214 #ifdef GST_V4L2_ENABLE_PROBE; meson.options:111 option('v4l2-probe', type : 'boolean', value : true

### D-39

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4
- **Fact:** In Buildroot, gst1-plugins-good passes -Dv4l2-probe=true only when BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y (option "v4l2-probe (m2m)") and -Dv4l2-probe=false otherwise. Without that option v4l2h264enc and v4l2convert are not registered.
- **Source:** Buildroot package/gstreamer1/gst1-plugins-good (Config.in, gst1-plugins-good.mk) — <https://github.com/buildroot/buildroot/blob/master/package/gstreamer1/gst1-plugins-good/gst1-plugins-good.mk>
- **Evidence:** gst1-plugins-good.mk:394-397 ifeq ($(BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE),y) -Dv4l2-probe=true else -Dv4l2-probe=false; Config.in:321 config BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE bool "v4l2-probe (m2m)"

### D-40

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi5, CM5, Streaming, TC358743
- **Fact:** GStreamer x264enc (plugin x264, package GStreamer Ugly Plug-ins) accepts sink formats Y444, Y42B, I420, YV12, NV12, GRAY8 and 10-bit variants, but not packed UYVY. speed-preset value 1 is ultrafast, and tune includes zerolatency (0x00000004).
- **Source:** GStreamer documentation - x264enc — <https://gstreamer.freedesktop.org/documentation/x264/index.html>
- **Evidence:** Plugin – x264 / Package – GStreamer Ugly Plug-ins; sink formats Y444, Y42B, I420, YV12, NV12, GRAY8, Y444_10LE, I422_10LE, I420_10LE; "ultrafast (1) – ultrafast"; "zerolatency (0x00000004) – Zero latency"

### D-41

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi5, CM5, Streaming
- **Fact:** GStreamer openh264enc (openh264 plugin, GStreamer Bad Plug-ins) accepts only I420 raw input and outputs H.264 constrained-baseline, baseline, main, constrained-high or high profile byte-stream.
- **Source:** GStreamer documentation - openh264enc — <https://gstreamer.freedesktop.org/documentation/openh264/openh264enc.html>
- **Evidence:** Sink: video/x-raw format: I420; Src: video/x-h264 profiles constrained-baseline, baseline, main, constrained-high, high; stream-format byte-stream, alignment au

### D-42

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Linux, Pi4, Pi5, Streaming
- **Fact:** FFmpeg encoder names: h264_v4l2m2m ("V4L2 mem2mem H.264 encoder wrapper", AV_CODEC_CAP_HARDWARE, built when v4l2_m2m is enabled) and libx264 ("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10"). libx264 is in configure's EXTERNAL_LIBRARY_GPL_LIST, so FFmpeg must be built with --enable-gpl to use it.
- **Source:** FFmpeg master libavcodec/v4l2_m2m_enc.c, libavcodec/libx264.c, configure — <https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/v4l2_m2m_enc.c>
- **Evidence:** v4l2_m2m_enc.c:424-425 .p.name = #NAME "_v4l2m2m", CODEC_LONG_NAME("V4L2 mem2mem " LONGNAME " encoder wrapper"); :444 M2MENC(h264, "H.264", ...); libx264.c:1638 .p.name = "libx264"; configure:2015/2024 EXTERNAL_LIBRARY_GPL_LIST= ... libx264; :3629 h264_v4l2m2m_encoder_deps="v4l2_m2m h264_v4l2_m2m"

### D-43

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi5, CM5, TC358743, Streaming
- **Fact:** FFmpeg libx264's 8-bit input formats are YUV420P, YUVJ420P, YUV422P, YUVJ422P, YUV444P, YUVJ444P, NV12, NV16 and NV21. Packed UYVY422/YUYV422 is not accepted, so TC358743 UYVY frames need conversion (e.g. swscale) first.
- **Source:** FFmpeg master libavcodec/libx264.c — <https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/libx264.c>
- **Evidence:** lines 1459-1471 static const enum AVPixelFormat pix_fmts_8bit[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUVJ422P, AV_PIX_FMT_YUV444P, AV_PIX_FMT_YUVJ444P, AV_PIX_FMT_NV12, AV_PIX_FMT_NV16, AV_PIX_FMT_NV21 }; grep UYVY/YUYV in libx264.c: no match

### D-44

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Linux, Pi4, CM4, Buildroot
- **Fact:** Upstream FFmpeg (master) V4L2 M2M code allocates buffers only with V4L2_MEMORY_MMAP (no DMABUF import), so h264_v4l2m2m copies each raw frame into driver buffers. The encoder also forces B-frames to 0 ("Encoder does not support b-frames yet").
- **Source:** FFmpeg master libavcodec/v4l2_context.c, v4l2_buffers.c, v4l2_m2m.c, v4l2_m2m_enc.c — <https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/v4l2_context.c>
- **Evidence:** only V4L2_MEMORY_MMAP occurrences (v4l2_context.c:380, 447, 730; v4l2_buffers.c:523); no V4L2_MEMORY_DMABUF or DRM_PRIME in the four files; v4l2_m2m_enc.c:151 av_log(... "Encoder does not support b-frames yet\n")

### D-45

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi4, CM4, Linux
- **Fact:** Raspberry Pi OS's patched FFmpeg (RPi-Distro/ffmpeg pios/trixie, patch ffmpeg-7.1.5-rpi_29.patch) adds DMABUF input to the V4L2 M2M encoder: when avctx->pix_fmt is AV_PIX_FMT_DRM_PRIME the OUTPUT queue uses V4L2_MEMORY_DMABUF. rpicam-apps sets AV_PIX_FMT_DRM_PRIME for h264_v4l2m2m.
- **Source:** RPi-Distro/ffmpeg pios/trixie debian/patches/ffmpeg-7.1.5-rpi_29.patch; rpicam-apps libav_encoder.cpp — <https://github.com/RPi-Distro/ffmpeg/blob/pios/trixie/debian/patches/ffmpeg-7.1.5-rpi_29.patch>
- **Evidence:** patch (v4l2_m2m_enc.c hunk): "+    s->input_drm = (avctx->pix_fmt == AV_PIX_FMT_DRM_PRIME);" (v4l2_m2m.c hunk): "+    s->output.buf_mem = s->input_drm ? V4L2_MEMORY_DMABUF : V4L2_MEMORY_MMAP;"; rpicam-apps libav_encoder.cpp:79 codec->pix_fmt = AV_PIX_FMT_DRM_PRIME;

### D-46

- **Verdict:** `CORRECTED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux
- **Fact:** Buildroot master packages upstream FFmpeg 6.1.5 from https://ffmpeg.org/releases with patches 0001-0007 only, none of which add Raspberry Pi V4L2/DRM_PRIME support. It passes --enable-libx264 only when BR2_PACKAGE_X264=y and BR2_PACKAGE_FFMPEG_GPL=y. BR2_PACKAGE_FFMPEG_GPL=y on its own changes FFMPEG_LICENSE from 'LGPL-2.1+, libjpeg license' to 'LGPL-2.1+, libjpeg license and GPL-2.0+' (adding COPYING.GPLv2).
- **Source:** Buildroot package/ffmpeg/ffmpeg.mk and directory listing — <https://github.com/buildroot/buildroot/blob/master/package/ffmpeg/ffmpeg.mk>
- **Evidence:** FFMPEG_VERSION = 6.1.5; FFMPEG_SITE = https://ffmpeg.org/releases; ifeq ($(BR2_PACKAGE_FFMPEG_GPL),y) FFMPEG_LICENSE += and GPL-2.0+; ifeq ($(BR2_PACKAGE_X264)$(BR2_PACKAGE_FFMPEG_GPL),yy) FFMPEG_CONF_OPTS += --enable-libx264; patches: 0001-swscale..., 0005-avcodec-mmaldec-Fix-build-error.patch, ... 0007
- **Original claim (before verification):** Buildroot master packages upstream FFmpeg 6.1.5 from ffmpeg.org (patches 0001-0007 only, none adding Raspberry Pi V4L2/DRM_PRIME support). It enables --enable-libx264 only when BR2_PACKAGE_X264=y and BR2_PACKAGE_FFMPEG_GPL=y, which makes the FFmpeg licence LGPL-2.1+ and GPL-2.0+.
- **Verifier note:** I saw FFMPEG_VERSION = 6.1.5, FFMPEG_SITE = https://ffmpeg.org/releases, FFMPEG_LICENSE = LGPL-2.1+, libjpeg license, and '+= and GPL-2.0+' under BR2_PACKAGE_FFMPEG_GPL, plus ifeq ($(BR2_PACKAGE_X264)$(BR2_PACKAGE_FFMPEG_GPL),yy) --enable-libx264 at lines 439-440. The directory has patches 0001-swscale-x86..., 0002 vaapi_h264, 0003 mips, 0004 configure extralibs, 0005 mmaldec build fix, 0006 pcm-bluray, 0007 hwcontext-vaapi. The original claim left out the 'libjpeg license' term, and it is the GPL option that adds GPL-2.0+, not libx264. The .mk passes --disable-mmal and does not disable v4l2_m2m, so upstream h264_v4l2m2m is auto-detected.

### D-47

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi5, CM5
- **Fact:** In Buildroot, the GStreamer x264enc element comes from BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264, which selects BR2_PACKAGE_X264. Buildroot's x264 help text says x264 is released under the GNU GPL.
- **Source:** Buildroot package/gstreamer1/gst1-plugins-ugly/Config.in; package/x264/Config.in — <https://github.com/buildroot/buildroot/blob/master/package/x264/Config.in>
- **Evidence:** config BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264 bool "x264" select BR2_PACKAGE_X264; x264 help: "...compression format, and is released under the terms of the GNU GPL."

### D-48

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi4, CM4, Buildroot
- **Fact:** The Pi 4 cut-down firmware (start4cd.elf/fixup4cd.dat, selected only via gpu_mem=16) removes codec support. Buildroot's rpi-firmware offers BR2_PACKAGE_RPI_FIRMWARE_VARIANT_PI4 (default), _PI4_X (extended, 'more audio/video codecs') and _PI4_CD (cut-down, 'only features required to boot a Linux kernel').
- **Source:** Raspberry Pi documentation config.txt boot options; Buildroot package/rpi-firmware/Config.in — <https://www.raspberrypi.com/documentation/computers/config_txt.html#start_file-fixup_file>
- **Evidence:** "The only way to enable the cut-down firmware is to specify `gpu_mem=16`. The cut-down firmware removes support for codecs, 3D and debug logging"; Buildroot Config.in:55-59 VARIANT_PI4_CD "only features required to boot a Linux kernel"

### D-49

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, Pi5, Linux
- **Fact:** Buildroot master's raspberrypi4_64_defconfig and raspberrypi5_defconfig pin raspberrypi/linux commit 21b410140c47ffab5668399f6f143c7d7b935c8b, which is Linux 6.12.61 rather than the rpi-6.18.y default branch. They use BR2_LINUX_KERNEL_DEFCONFIG "bcm2711" or "bcm2712", and the Pi 4 config sets BR2_PACKAGE_RPI_FIRMWARE_VARIANT_PI4=y. bcm2835-codec/Kconfig exists at that commit.
- **Source:** Buildroot configs/raspberrypi4_64_defconfig, raspberrypi5_defconfig; raspberrypi/linux Makefile at pinned commit — <https://github.com/buildroot/buildroot/blob/master/configs/raspberrypi4_64_defconfig>
- **Evidence:** BR2_LINUX_KERNEL_CUSTOM_TARBALL_LOCATION="$(call github,raspberrypi,linux,21b410140c47ffab5668399f6f143c7d7b935c8b)..."; BR2_LINUX_KERNEL_DEFCONFIG="bcm2711"; Makefile at commit: VERSION = 6 PATCHLEVEL = 12 SUBLEVEL = 61; .../bcm2835-codec/Kconfig at commit -> HTTP 200

### D-50

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi5, CM5, Pi4
- **Fact:** Raspberry Pi engineers stated on the forum (2023-10-17) that BCM2712 has no H.264 hardware block for encode or decode (jamesh). 6by9 added that software 1080p60 encode from camera capture is 'easily achievable (which was edge case on the hardware encode)' and that 4K reaches at least 20 fps.
- **Source:** Raspberry Pi Forums - RPi5 Codec confusion (jamesh 09:08, 6by9 09:17, 2023-10-17) — <https://forums.raspberrypi.com/viewtopic.php?t=357870>
- **Evidence:** jamesh: "The 2712 does NOT have a H264 HW block for encoding or decoding." 6by9: "Software encoding has already been tested with camera capture. 1080p60 is easily achievable (which was edge case on the hardware encode). 4k is being profiled, but at least 20fps is possible."

### D-51

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi5, CM5, Linux
- **Fact:** The rpi-6.18.y BCM2712 device tree defines hevc_dec (compatible "brcm,bcm2712-hevc-dec", "raspberrypi,hevc-dec") and pisp_be ("raspberrypi,pispbe") behind iommu2, whose comment reads "IOMMU2 for PISP-BE, HEVC; and (unused) H264 accelerators". No H.264 encoder node or driver is instantiated.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/boot/dts/broadcom/bcm2712-ds.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-ds.dtsi>
- **Verifier's best source:** <https://raw.githubusercontent.com/raspberrypi/linux/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-ds.dtsi>
- **Evidence:** iommu2: iommu@5100 { /* IOMMU2 for PISP-BE, HEVC; and (unused) H264 accelerators */ ...; hevc_dec: codec@800000 { compatible = "brcm,bcm2712-hevc-dec", "raspberrypi,hevc-dec"; ...; pisp_be: pisp_be@880000 { compatible = "raspberrypi,pispbe";

### D-52

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi4, CM4
- **Fact:** Macroblock-rate comparison for the Pi 4 encoder (inference). The official 1080p30 spec is 244,800 MB/s. The officially documented high-framerate recipe (1280x720 at 120 fps with --level 4.2, Pi 4 GPU overclock gpu_freq=550 suggested) is 432,000 MB/s. 1080p60 needs 489,600 MB/s, which is 2.0x the specified rate and about 1.13x the documented 720p120 recipe.
- **Source:** Reasoning from Raspberry Pi docs (rpicam-vid 'Capture high framerate video') and D-10 — <https://www.raspberrypi.com/documentation/computers/camera_software.html#capture-high-framerate-video>
- **Evidence:** Doc: "Set the H.264 target level to 4.2 with `--level 4.2`" ... "On Raspberry Pi 4, you can overclock the GPU ... `gpu_freq=550` or higher" ... "rpicam-vid --level 4.2 --framerate 120 --width 1280 --height 720". Calc: 80x45x120=432,000; 120x68x30=244,800; 120x68x60=489,600

### D-53

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Linux, Pi4, CM4, Streaming
- **Fact:** GStreamer V4L2 M2M elements (including v4l2h264enc) expose output-io-mode and capture-io-mode properties whose values include dmabuf-import, plus an extra-controls property for setting encoder V4L2 controls.
- **Source:** GStreamer gst-plugins-good sys/v4l2/gstv4l2object.c — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/main/subprojects/gst-plugins-good/sys/v4l2/gstv4l2object.c>
- **Evidence:** line 388-389 {GST_V4L2_IO_DMABUF_IMPORT, "GST_V4L2_IO_DMABUF_IMPORT", "dmabuf-import"}; line 544 g_param_spec_enum ("output-io-mode", ...); line 550 "capture-io-mode"; line 556 "extra-controls"

### D-54

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Pi4, CM4, TC358743, Streaming
- **Fact:** Raspberry Pi's TC358743 instructions (6by9, 2020-08-05) feed UYVY from v4l2src directly into v4l2h264enc with extra-controls "controls,h264_profile=4,h264_level=10,video_bitrate=256000;". h264_level=10 is V4L2_MPEG_VIDEO_H264_LEVEL_3_2, not level 4.x, and h264_profile=4 is High.
- **Source:** Raspberry Pi Forums - TC358743 HDMI to CSI-2 install instructions (Pi 0-4); include/uapi/linux/v4l2-controls.h — <https://forums.raspberrypi.com/viewtopic.php?t=281972>
- **Evidence:** gst-launch-1.0 v4l2src ! "video/x-raw,framerate=30/1,format=UYVY" ! v4l2h264enc extra-controls="controls,h264_profile=4,h264_level=10,video_bitrate=256000;" ...; v4l2-controls.h:521 V4L2_MPEG_VIDEO_H264_LEVEL_3_2 = 10; :546 V4L2_MPEG_VIDEO_H264_PROFILE_HIGH = 4

---

## Topic E

**Buildroot and kernel configuration** — <a id="topic-e"></a>53 claims.

### E-01

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** As of 2026-10-06 the latest Buildroot stable release is 2026.08, released 2026-09-04. The 2026.08.x series is listed with end of life in December 2026.
- **Source:** Buildroot - Download — <https://buildroot.org/download.html>
- **Evidence:** Download table: 'Stable | 2026.08.x | December 2026 | 2026.08 | 2026-09-04'; news.html: '2026.08 released 4 September 2026'

### E-02

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** The current Buildroot long-term-support series is 2025.02.x. Its latest point release is 2025.02.18 (2026-09-10), and the series ends in March 2028. The previous stable series, 2026.05.x (latest 2026.05.3, 2026-09-10), ends in September 2026.
- **Source:** Buildroot - Download — <https://buildroot.org/download.html>
- **Evidence:** 'Long-term support | 2025.02.x | March 2028 | 2025.02.18 | 2026-09-10'; 'Old stable | 2026.05.x | September 2026 | 2026.05.3 | 2026-09-10'

### E-03

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** Under Buildroot's LTS policy, starting with 2025.02, the first release of every odd-numbered year is an LTS release with 3 years of support. Non-LTS releases come out every 3 months (2026.02, 2026.05, 2026.08 and 2026.11 are 3-month releases). The next LTS is 2027.02.
- **Source:** Buildroot LTS — <https://buildroot.org/lts.html>
- **Evidence:** 'Starting with 2025.02, the first release of every odd-numbered year is a long-term support (LTS) release supported for three years. In between, non-LTS releases are made every 3 months.' Timeline: '2027.02 3 Years Support (LTS)'

### E-04

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** Buildroot 2026.08 has these Raspberry Pi defconfigs for the target boards: configs/raspberrypi4_64_defconfig (Pi 4 B, 400, CM4, CM4S, 64-bit), configs/raspberrypicm4io_64_defconfig (CM4 on IO Board, 64-bit), configs/raspberrypi5_defconfig (Pi 5 B and 500) and configs/raspberrypicm5io_defconfig (CM5 on IO Board). All board/raspberrypi* directories other than board/raspberrypi itself are symlinks to board/raspberrypi.
- **Source:** Buildroot 2026.08 configs/ and board/raspberrypi/readme.txt — <https://gitlab.com/buildroot.org/buildroot/-/tree/2026.08/configs>
- **Evidence:** readme.txt: 'For model 4 B, 400, CM4 and CM4s (64 bit): $ make raspberrypi4_64_defconfig' ... 'For model CM5 (on IO Board): $ make raspberrypicm5io_defconfig'; 'raspberrypi5 -> raspberrypi' symlink

### E-05

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, CM5
- **Fact:** Buildroot LTS 2025.02.18 has raspberrypi4_64_defconfig, raspberrypicm4io_64_defconfig and raspberrypi5_defconfig, but no raspberrypicm5io_defconfig. CM5 defconfig support exists in 2026.08.
- **Source:** Buildroot 2025.02.18 configs/ directory listing — <https://gitlab.com/buildroot.org/buildroot/-/tree/2025.02.18/configs>
- **Evidence:** ls configs | grep raspberry at tag 2025.02.18 lists raspberrypi4_64, raspberrypi5, raspberrypicm4io_64 ... and no raspberrypicm5io_defconfig

### E-06

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** In Buildroot 2026.08 (also 2026.05.3 and master on 2026-10-06), all four Pi 4/CM4/Pi 5/CM5 defconfigs build the kernel from the raspberrypi/linux commit 21b410140c47ffab5668399f6f143c7d7b935c8b tarball through BR2_LINUX_KERNEL_CUSTOM_TARBALL. That commit is Linux 6.12.61, dated 2025-12-09.
- **Source:** Buildroot 2026.08 configs/raspberrypi5_defconfig; raspberrypi/linux Makefile @21b41014 — <https://github.com/raspberrypi/linux/blob/21b410140c47ffab5668399f6f143c7d7b935c8b/Makefile>
- **Evidence:** BR2_LINUX_KERNEL_CUSTOM_TARBALL_LOCATION="$(call github,raspberrypi,linux,21b410140c47ffab5668399f6f143c7d7b935c8b)/linux-21b41014...tar.gz"; Makefile: VERSION = 6 PATCHLEVEL = 12 SUBLEVEL = 61

### E-07

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** The Buildroot Pi 4 and CM4 64-bit defconfigs set BR2_LINUX_KERNEL_DEFCONFIG="bcm2711". The Pi 5 and CM5 defconfigs set BR2_LINUX_KERNEL_DEFCONFIG="bcm2712". All of them use BR2_PACKAGE_RPI_FIRMWARE=y and BR2_PACKAGE_KMOD=y.
- **Source:** Buildroot 2026.08 configs/raspberrypi{4_64,cm4io_64,5,cm5io}_defconfig — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypi5_defconfig>
- **Evidence:** raspberrypi4_64_defconfig:15 BR2_LINUX_KERNEL_DEFCONFIG="bcm2711"; raspberrypi5_defconfig:14 BR2_LINUX_KERNEL_DEFCONFIG="bcm2712"

### E-08

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** In-tree DTBs built by Buildroot 2026.08 (BR2_LINUX_KERNEL_INTREE_DTS_NAME): Pi4_64 builds broadcom/bcm2711-rpi-4-b, bcm2711-rpi-400, bcm2711-rpi-cm4 and bcm2711-rpi-cm4s. CM4IO_64 builds broadcom/bcm2711-rpi-cm4. Pi5 builds broadcom/bcm2712-rpi-5-b, bcm2712d0-rpi-5-b and bcm2712-rpi-500. CM5IO builds broadcom/bcm2712-rpi-cm5-cm5io and bcm2712-rpi-cm5l-cm5io.
- **Source:** Buildroot 2026.08 RPi defconfigs — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypicm5io_defconfig>
- **Evidence:** raspberrypicm5io_defconfig:17 BR2_LINUX_KERNEL_INTREE_DTS_NAME="broadcom/bcm2712-rpi-cm5-cm5io broadcom/bcm2712-rpi-cm5l-cm5io"

### E-09

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi5, CM5, Linux
- **Fact:** raspberrypi5_defconfig and raspberrypicm5io_defconfig set BR2_LINUX_KERNEL_CONFIG_FRAGMENT_FILES="board/raspberrypi/linux-4k-page-size.fragment". That fragment contains CONFIG_ARM64_4K_PAGES=y, which overrides CONFIG_ARM64_16K_PAGES=y in the Raspberry Pi bcm2712_defconfig.
- **Source:** Buildroot 2026.08 raspberrypi5_defconfig + board/raspberrypi/linux-4k-page-size.fragment; raspberrypi/linux bcm2712_defconfig — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/board/raspberrypi/linux-4k-page-size.fragment>
- **Evidence:** fragment content: 'CONFIG_ARM64_4K_PAGES=y'; bcm2712_defconfig:50 'CONFIG_ARM64_16K_PAGES=y' (6.12.61 and rpi-6.18.y)

### E-10

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** Buildroot 2026.08 pins rpi-firmware at RPI_FIRMWARE_VERSION = 063bcab6c8a90efb0d19f69d88cbbc7ec79cab68 (raspberrypi/firmware, committed 2025-12-08). That commit's extra/uname_string8 reports kernel 6.12.61-v8+, which matches the Buildroot kernel commit.
- **Source:** Buildroot 2026.08 package/rpi-firmware/rpi-firmware.mk; raspberrypi/firmware extra/uname_string8 — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/rpi-firmware/rpi-firmware.mk>
- **Evidence:** rpi-firmware.mk:8 'RPI_FIRMWARE_VERSION = 063bcab6c8a90efb0d19f69d88cbbc7ec79cab68'; uname_string8 @063bcab6: 'Linux version 6.12.61-v8+ ... Mon Dec 8 17:47:47 GMT 2025'

### E-11

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux, Pi4, CM4, Pi5
- **Fact:** Buildroot LTS 2025.02.18 RPi defconfigs use raspberrypi/linux commit 576cc10e1ed50a9eacffc7a05c796051d7343ea4 (Linux 6.6.28, 2024-04-18) and rpi-firmware 5476720d52cf579dc1627715262b30ba1242525e (commit message 'kernel: Bump to 6.6.28').
- **Source:** Buildroot 2025.02.18 configs + rpi-firmware.mk; raspberrypi/linux Makefile @576cc10e — <https://gitlab.com/buildroot.org/buildroot/-/blob/2025.02.18/package/rpi-firmware/rpi-firmware.mk>
- **Evidence:** RPI_FIRMWARE_VERSION = 5476720d52cf...; TARBALL_LOCATION ...576cc10e1ed5...; kernel Makefile 'VERSION = 6 PATCHLEVEL = 6 SUBLEVEL = 28'

### E-12

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** All four Buildroot 2026.08 Pi 4/CM4/Pi 5/CM5 defconfigs use the external toolchain BR2_TOOLCHAIN_EXTERNAL_BOOTLIN_AARCH64_GLIBC_STABLE ('aarch64 glibc stable 2025.08-1'). It selects BR2_TOOLCHAIN_GCC_AT_LEAST_14 and BR2_TOOLCHAIN_HEADERS_AT_LEAST_5_4.
- **Source:** Buildroot 2026.08 toolchain-external-bootlin/Config.in.options — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/toolchain/toolchain-external/toolchain-external-bootlin/Config.in.options>
- **Evidence:** config BR2_TOOLCHAIN_EXTERNAL_BOOTLIN_AARCH64_GLIBC_STABLE bool "aarch64 glibc stable 2025.08-1" ... select BR2_TOOLCHAIN_GCC_AT_LEAST_14 select BR2_TOOLCHAIN_HEADERS_AT_LEAST_5_4

### E-13

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** The rpi-firmware package options are: BR2_PACKAGE_RPI_FIRMWARE_BOOTCODE_BIN; independent bools _VARIANT_PI, _VARIANT_PI_X, _VARIANT_PI_CD, _VARIANT_PI_DB, _VARIANT_PI4 (start4.elf/fixup4.dat), _VARIANT_PI4_X, _VARIANT_PI4_CD and _VARIANT_PI4_DB; _CONFIG_FILE; _CMDLINE_FILE (default "board/raspberrypi/cmdline.txt"); _INSTALL_DTBS (default y, depends on !BR2_LINUX_KERNEL_DTS_SUPPORT); _INSTALL_DTB_OVERLAYS (default y); and _EXTRA_FILES. There is no Pi 5 firmware variant.
- **Source:** Buildroot 2026.08 package/rpi-firmware/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/rpi-firmware/Config.in>
- **Evidence:** Config.in:43 'config BR2_PACKAGE_RPI_FIRMWARE_VARIANT_PI4 bool "rpi 4 (default)"'; :72-74 CMDLINE_FILE default "board/raspberrypi/cmdline.txt"; :91-97 INSTALL_DTB_OVERLAYS default y; :103 EXTRA_FILES

### E-14

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS copies the prebuilt boot/overlays/*.dtbo and boot/overlays/overlay_map.dtb from the raspberrypi/firmware tarball, not from the kernel build, into $(BINARIES_DIR)/rpi-firmware/overlays/. With kernel DTS support it also selects BR2_LINUX_KERNEL_DTB_OVERLAY_SUPPORT, which adds DTC_FLAGS=-@ so the base DTBs carry symbols.
- **Source:** Buildroot 2026.08 package/rpi-firmware/rpi-firmware.mk, Config.in, linux/linux.mk — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/rpi-firmware/rpi-firmware.mk>
- **Evidence:** rpi-firmware.mk:57-60 '$(wildcard $(@D)/boot/overlays/*.dtbo) ... $(BINARIES_DIR)/rpi-firmware/overlays/ ... overlay_map.dtb'; Config.in:96 'select BR2_LINUX_KERNEL_DTB_OVERLAY_SUPPORT if BR2_LINUX_KERNEL_DTS_SUPPORT'; linux.mk:192-193 'LINUX_MAKE_ENV += DTC_FLAGS=-@'

### E-15

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi5, CM5, Pi4, CM4
- **Fact:** raspberrypi5_defconfig explicitly disables overlay installation ('# BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS is not set') in both 2026.08 and 2025.02.18. raspberrypicm5io_defconfig sets it to y, and the Pi 4/CM4 64-bit defconfigs leave it at its default of y.
- **Source:** Buildroot 2026.08 configs/raspberrypi5_defconfig, raspberrypicm5io_defconfig — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypi5_defconfig>
- **Evidence:** raspberrypi5_defconfig:24 '# BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS is not set'; raspberrypicm5io_defconfig:24 'BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS=y'

### E-16

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, CM4, Pi5, CM5
- **Fact:** config.txt source files are referenced by BR2_PACKAGE_RPI_FIRMWARE_CONFIG_FILE and installed as $(BINARIES_DIR)/rpi-firmware/config.txt. The sources are board/raspberrypi4-64/config_4_64bit.txt (Pi 4), board/raspberrypicm4io-64/config_cm4io_64bit.txt (CM4IO), board/raspberrypi5/config_5.txt (Pi 5) and board/raspberrypicm5io/config_cm5io.txt (CM5IO). Pi 5 uses cmdline_5.txt (console=ttyAMA10,115200), and the others use cmdline.txt (console=ttyAMA0,115200).
- **Source:** Buildroot 2026.08 RPi defconfigs and board/raspberrypi/ — <https://gitlab.com/buildroot.org/buildroot/-/tree/2026.08/board/raspberrypi>
- **Evidence:** BR2_PACKAGE_RPI_FIRMWARE_CONFIG_FILE="board/raspberrypi5/config_5.txt"; rpi-firmware.mk:34-35 '$(INSTALL) -D -m 0644 $(RPI_FIRMWARE_CONFIG_FILE) $(BINARIES_DIR)/rpi-firmware/config.txt'; cmdline_5.txt 'root=/dev/mmcblk0p2 rootwait console=tty1 console=ttyAMA10,115200'

### E-17

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Pi4, Pi5, TC358743
- **Fact:** The sample config_4_64bit.txt contains start_file=start4.elf, fixup_file=fixup4.dat, kernel=Image, disable_overscan=1, gpu_mem_256/512/1024=100, dtoverlay=miniuart-bt and arm_64bit=1. config_5.txt contains only kernel=Image and disable_overscan=1. Neither file contains a camera or tc358743 overlay, and both say they are samples to be overridden.
- **Source:** Buildroot 2026.08 board/raspberrypi/config_4_64bit.txt, config_5.txt — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/board/raspberrypi/config_4_64bit.txt>
- **Evidence:** '# Please note that this is only a sample ... You should override this file using BR2_PACKAGE_RPI_FIRMWARE_CONFIG_FILE.' ... 'gpu_mem_1024=100' 'dtoverlay=miniuart-bt' 'arm_64bit=1'

### E-18

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** board/raspberrypi/post-image.sh (the BR2_ROOTFS_POST_IMAGE_SCRIPT) uses board-specific genimage-${BOARD_NAME}.cfg if it exists. Otherwise it generates ${BINARIES_DIR}/genimage.cfg from genimage.cfg.in, listing all *.dtb, everything in rpi-firmware/ and the kernel file named by 'kernel=' in config.txt. It then runs genimage.
- **Source:** Buildroot 2026.08 board/raspberrypi/post-image.sh — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/board/raspberrypi/post-image.sh>
- **Evidence:** GENIMAGE_CFG="${BOARD_DIR}/genimage-${BOARD_NAME}.cfg" ... for i in "${BINARIES_DIR}"/*.dtb "${BINARIES_DIR}"/rpi-firmware/* ... KERNEL=$(sed -n 's/^kernel=//p' ...config.txt) ... genimage --rootpath ... --config "${GENIMAGE_CFG}"

### E-19

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** genimage.cfg.in defines boot.vfat (size = 32M) and sdcard.img (hdimage). sdcard.img has partition 'boot' (partition-type 0xC, bootable, image boot.vfat) and partition 'rootfs' (partition-type 0x83, image rootfs.ext4). The defconfigs set BR2_TARGET_ROOTFS_EXT2_4=y with BR2_TARGET_ROOTFS_EXT2_SIZE="120M".
- **Source:** Buildroot 2026.08 board/raspberrypi/genimage.cfg.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/board/raspberrypi/genimage.cfg.in>
- **Evidence:** image boot.vfat { vfat {...} size = 32M } image sdcard.img { hdimage {} partition boot { partition-type = 0xC bootable = "true" ...} partition rootfs { partition-type = 0x83 image = "rootfs.ext4" } }

### E-20

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux
- **Fact:** Buildroot's linux package builds and installs to BINARIES_DIR only the DTBs in LINUX_DTBS. That list comes from BR2_LINUX_KERNEL_INTREE_DTS_NAME, BR2_LINUX_KERNEL_INTREE_DTSO_NAMES and the custom DTS path or dir. Overlays built by the Raspberry Pi kernel tree are therefore not installed by the linux package.
- **Source:** Buildroot 2026.08 linux/linux.mk — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/linux/linux.mk>
- **Evidence:** linux.mk: 'LINUX_DTBS = $(addsuffix .dtb,$(LINUX_DTS_NAME)) $(addsuffix .dtbo,$(LINUX_DTSO_NAMES))'; LINUX_INSTALL_DTB: '$(foreach dtb,$(LINUX_DTBS), install -D ...)'

### E-21

- **Verdict:** `CORRECTED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** The RPi defconfigs set BR2_GLOBAL_PATCH_DIR="board/raspberrypi/patches" and BR2_DOWNLOAD_FORCE_CHECK_HASHES=y. That directory contains two entries: linux/linux.hash ('# Locally calculated' plus the sha256 7c31df8061aae748a6e72417bfe743a54198fb5bdc96e229ecc605dc621d32ef of linux-21b410140c47ffab5668399f6f143c7d7b935c8b.tar.gz) and linux-headers/linux-headers.hash, a symlink to ../linux/linux.hash.
- **Source:** Buildroot 2026.08 board/raspberrypi/patches/linux/linux.hash — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/board/raspberrypi/patches/linux/linux.hash>
- **Verifier's best source:** <https://gitlab.com/buildroot.org/buildroot/-/tree/2026.08/board/raspberrypi/patches>
- **Evidence:** 'sha256  7c31df8061aae748a6e72417bfe743a54198fb5bdc96e229ecc605dc621d32ef  linux-21b410140c47ffab5668399f6f143c7d7b935c8b.tar.gz'; defconfig: BR2_GLOBAL_PATCH_DIR="board/raspberrypi/patches", BR2_DOWNLOAD_FORCE_CHECK_HASHES=y
- **Original claim (before verification):** The RPi defconfigs set BR2_GLOBAL_PATCH_DIR="board/raspberrypi/patches" and BR2_DOWNLOAD_FORCE_CHECK_HASHES=y. The only file in that directory is linux/linux.hash, which holds the sha256 of linux-21b410140c47ffab5668399f6f143c7d7b935c8b.tar.gz (7c31df80...d32ef).
- **Verifier note:** find on board/raspberrypi/patches in the 2026.08 clone shows linux/linux.hash (149 bytes) and linux-headers/linux-headers.hash -> ../linux/linux.hash. The claim that linux.hash is the only file is wrong. The hash value is correct. Any project that changes the kernel tarball must provide a new hash for both linux and linux-headers, or the FORCE_CHECK_HASHES build fails.

### E-22

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** Buildroot manual §9.2 'Keeping customizations outside of Buildroot': a br2-external tree is selected with 'make BR2_EXTERNAL=/path/to/foo menuconfig' (space-separated for several trees). It must contain at least external.desc, Config.in and external.mk. Its path is available as BR2_EXTERNAL_$(NAME)_PATH, for example BR2_GLOBAL_PATCH_DIR=$(BR2_EXTERNAL_BAR_42_PATH)/patches/.
- **Source:** The Buildroot user manual (2026.08), §9.2 / §9.2.1 — <https://buildroot.org/downloads/manual/manual.html#outside-br-custom>
- **Evidence:** 'buildroot/ $ make BR2_EXTERNAL=/path/to/foo menuconfig' ... '9.2.1. Layout of a br2-external tree: A br2-external tree must contain at least those three files' ... 'BR2_ROOTFS_OVERLAY=$(BR2_EXTERNAL_BAR_42_PATH)/board/<boardname>/overlay/'

### E-23

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** Buildroot manual §9.5 names root filesystem overlays (BR2_ROOTFS_OVERLAY, space-separated list copied over the target filesystem after build) and post-build scripts (BR2_ROOTFS_POST_BUILD_SCRIPT, run before the rootfs images are assembled) as the two recommended ways to customize the target filesystem.
- **Source:** The Buildroot user manual (2026.08), §9.5 Customizing the generated target filesystem — <https://buildroot.org/downloads/manual/manual.html#rootfs-custom>
- **Evidence:** 'The two recommended methods, which can co-exist, are root filesystem overlay(s) and post build script(s). Root filesystem overlays (BR2_ROOTFS_OVERLAY) A filesystem overlay is a tree of files that is copied directly over the target filesystem after it has been built.'

### E-24

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** Buildroot manual §9.7: post-image scripts (BR2_ROOTFS_POST_IMAGE_SCRIPT) run after all images are created. They get the images output directory as their first argument and can use BR2_CONFIG, HOST_DIR, STAGING_DIR, TARGET_DIR, BUILD_DIR, BINARIES_DIR, CONFIG_DIR, BASE_DIR and PARALLEL_JOBS.
- **Source:** The Buildroot user manual (2026.08), §9.7 Customization after the images have been created — <https://buildroot.org/downloads/manual/manual.html>
- **Verifier's best source:** <https://buildroot.org/downloads/manual/manual.html#rootfs-custom>
- **Evidence:** 'specify a space-separated list of post-image scripts in config option BR2_ROOTFS_POST_IMAGE_SCRIPT ... The path to the images output directory is passed as the first argument to each script.'

### E-25

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** Buildroot manual §9.8: BR2_GLOBAL_PATCH_DIR is a space-separated list of directories. Patches are looked up in <global-patch-dir>/<packagename>/<packageversion>/ (falling back to <packagename>/), and extra download hashes in <global-patch-dir>/<packagename>/<packageversion>/<packagename>.hash or <global-patch-dir>/<packagename>/<packagename>.hash (§9.8.2). The manual calls it the preferred method for project patches.
- **Source:** The Buildroot user manual (2026.08), §9.8.1/§9.8.2 — <https://buildroot.org/downloads/manual/manual.html#customize-patches>
- **Evidence:** 'The BR2_GLOBAL_PATCH_DIR configuration option can be used to specify a space separated list of one or more directories containing package patches.' ... '<global-patch-dir>/<packagename>/<packagename>.hash'; 'The BR2_GLOBAL_PATCH_DIR option is the preferred method'

### E-26

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux
- **Fact:** Buildroot manual §9.4: a full or defconfig kernel configuration is stored with BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE and saved with 'make linux-update-defconfig'. Manual §18.13 says the -update-config and -update-defconfig targets cannot be used when kconfig fragment files are set; '<pkg>-diff-config' exists to find changes to move into fragments.
- **Source:** The Buildroot user manual (2026.08), §9.4 and §18.13 — <https://buildroot.org/downloads/manual/manual.html>
- **Evidence:** 'make linux-update-defconfig saves the linux configuration to the path specified by BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE'; 'foo-update-defconfig ... It is not possible to use this target when fragment files are set.'

### E-27

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux
- **Fact:** BR2_LINUX_KERNEL_CONFIG_FRAGMENT_FILES is a string option: 'A space-separated list of kernel configuration fragment files, that will be merged to the main kernel configuration file'. It can be combined with BR2_LINUX_KERNEL_DEFCONFIG="bcm2711" or "bcm2712", as the Pi 5 defconfig does.
- **Source:** Buildroot 2026.08 linux/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/linux/Config.in>
- **Evidence:** linux/Config.in:224 'config BR2_LINUX_KERNEL_CONFIG_FRAGMENT_FILES string "Additional configuration fragment files"'

### E-28

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Linux
- **Fact:** In Buildroot 2026.08, BR2_LINUX_KERNEL_CUSTOM_DTS_PATH is documented as deprecated because of a kernel build-system change in 6.12, and is replaced by BR2_LINUX_KERNEL_CUSTOM_DTS_DIR. CUSTOM_DTS_DIR copies directories over arch/<arch>/boot/dts/ and should contain the vendor subdirectory (for example broadcom/). Both options also exist in 2025.02.18.
- **Source:** Buildroot 2026.08 linux/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/linux/Config.in>
- **Evidence:** 'Due to a kernel build system changes in 6.12, BR2_LINUX_KERNEL_CUSTOM_DTS_PATH is now deprecated and replaced by BR2_LINUX_KERNEL_CUSTOM_DTS_DIR' ... 'Since the 6.12 release, each out-of-tree Device Tree Source file must be copied into their corresponding sub-directory.'

### E-29

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** v4l-utils is packaged in Buildroot as 'libv4l' (BR2_PACKAGE_LIBV4L, version 1.32.0 in 2026.08 and 1.28.1 in 2025.02.18). The tools, including media-ctl, v4l2-ctl and v4l2-compliance, need the sub-option BR2_PACKAGE_LIBV4L_UTILS.
- **Source:** Buildroot 2026.08 package/libv4l/Config.in and libv4l.mk — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/libv4l/Config.in>
- **Evidence:** 'config BR2_PACKAGE_LIBV4L_UTILS bool "v4l-utils tools"' ... '- media-ctl - v4l2-compliance - v4l2-ctl, cx18-ctl, ivtv-ctl'; libv4l.mk:7 'LIBV4L_VERSION = 1.32.0'

### E-30

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** i2c-tools is BR2_PACKAGE_I2C_TOOLS (version 4.4). It depends on BR2_PACKAGE_BUSYBOX_SHOW_OTHERS, which all four RPi defconfigs already set to y.
- **Source:** Buildroot 2026.08 package/i2c-tools/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/i2c-tools/Config.in>
- **Evidence:** 'config BR2_PACKAGE_I2C_TOOLS bool "i2c-tools" depends on BR2_PACKAGE_BUSYBOX_SHOW_OTHERS'; i2c-tools.mk 'I2C_TOOLS_VERSION = 4.4'; defconfigs 'BR2_PACKAGE_BUSYBOX_SHOW_OTHERS=y'

### E-31

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Streaming
- **Fact:** Buildroot 2026.08 and 2025.02.18 both package GStreamer 1.24.13 (gstreamer1, gst1-plugins-base/good/bad/ugly, gst1-libav, gst1-rtsp-server). The V4L2 elements (v4l2src etc.) come from BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2.
- **Source:** Buildroot 2026.08 package/gstreamer1/* — <https://gitlab.com/buildroot.org/buildroot/-/tree/2026.08/package/gstreamer1>
- **Evidence:** gstreamer1.mk:7 'GSTREAMER1_VERSION = 1.24.13'; gst1-plugins-good/Config.in:311 'config BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2 bool "v4l2"'

### E-32

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Streaming
- **Fact:** Buildroot passes -Dv4l2-probe=true to gst1-plugins-good only when BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y ('v4l2-probe (m2m)'), and passes -Dv4l2-probe=false otherwise. The option has no 'default y'.
- **Source:** Buildroot 2026.08 gst1-plugins-good.mk / Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/gstreamer1/gst1-plugins-good/gst1-plugins-good.mk>
- **Evidence:** gst1-plugins-good.mk:394-398 'ifeq ($(BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE),y) ... -Dv4l2-probe=true else ... -Dv4l2-probe=false'

### E-33

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, Linux
- **Fact:** In GStreamer 1.24.13, the V4L2 mem2mem encoder and decoder elements (v4l2h264enc, v4l2h264dec-style registrations, v4l2convert) are registered only inside gst_v4l2_probe_and_register(), which is compiled only under GST_V4L2_ENABLE_PROBE (set from the meson option 'v4l2-probe'). Upstream the option defaults to true.
- **Source:** GStreamer 1.24.13 gst-plugins-good sys/v4l2/gstv4l2.c, meson.build, meson_options.txt — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/1.24.13/subprojects/gst-plugins-good/sys/v4l2/gstv4l2.c>
- **Evidence:** gstv4l2.c:214-216 '#ifdef GST_V4L2_ENABLE_PROBE ret |= gst_v4l2_probe_and_register (plugin); #endif'; inside probe: 'if (gst_v4l2_is_h264_enc (sink_caps, src_caps)) gst_v4l2_h264_enc_register (...)'; meson_options.txt: option('v4l2-probe', type : 'boolean', value : true, ...)

### E-34

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Streaming
- **Fact:** RTMP in GStreamer under Buildroot: BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2 provides rtmp2sink/rtmp2src with no external library. The older BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP selects BR2_PACKAGE_RTMPDUMP (librtmp). FLV muxing is BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV and MP4 muxing is BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_ISOMP4.
- **Source:** Buildroot 2026.08 gst1-plugins-bad/Config.in, gst1-plugins-good/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/gstreamer1/gst1-plugins-bad/Config.in>
- **Evidence:** bad/Config.in:252 'config BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2 bool "rtmp2" help RTMP sink/source (rtmp2sink, rtmp2src)'; :262 '..._RTMP ... select BR2_PACKAGE_RTMPDUMP'

### E-35

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Streaming
- **Fact:** BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_WEBRTC (webrtcbin) depends on !BR2_STATIC_LIBS and selects BR2_PACKAGE_GST1_PLUGINS_BASE, BR2_PACKAGE_LIBNICE and the bad plugins DTLS (which selects OpenSSL), SCTP and SRTP (which selects libsrtp). libnice 0.1.21 builds its GStreamer elements (-Dgstreamer=enabled) only when BR2_PACKAGE_GST1_PLUGINS_BASE=y.
- **Source:** Buildroot 2026.08 gst1-plugins-bad/Config.in; package/libnice/libnice.mk — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/gstreamer1/gst1-plugins-bad/Config.in>
- **Evidence:** bad/Config.in:655-663 'select BR2_PACKAGE_LIBNICE select ..._PLUGIN_DTLS select ..._PLUGIN_SCTP select ..._PLUGIN_SRTP'; libnice.mk:32-33 'ifeq ($(BR2_PACKAGE_GST1_PLUGINS_BASE),y) LIBNICE_CONF_OPTS += -Dgstreamer=enabled'

### E-36

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot, Streaming
- **Fact:** x264 is BR2_PACKAGE_X264 (git commit baee400f..., optional BR2_PACKAGE_X264_CLI). The GStreamer x264enc element is BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264, which selects X264. FFmpeg (BR2_PACKAGE_FFMPEG, version 6.1.5) is configured with --enable-libx264 only when both BR2_PACKAGE_X264 and BR2_PACKAGE_FFMPEG_GPL are y.
- **Source:** Buildroot 2026.08 package/x264, gst1-plugins-ugly/Config.in, package/ffmpeg/ffmpeg.mk — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/ffmpeg/ffmpeg.mk>
- **Evidence:** ffmpeg.mk:7 'FFMPEG_VERSION = 6.1.5'; :439-441 'ifeq ($(BR2_PACKAGE_X264)$(BR2_PACKAGE_FFMPEG_GPL),yy) FFMPEG_CONF_OPTS += --enable-libx264'; ugly/Config.in:52-54 'config BR2_PACKAGE_GST1_PLUGINS_UGLY_PLUGIN_X264 ... select BR2_PACKAGE_X264'

### E-37

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux
- **Fact:** The default branch of github.com/raspberrypi/linux on 2026-10-06 is rpi-6.18.y. Its tip is af73e0836bf0 ('configs: Regenerate the defconfigs', 2026-10-06), and its Makefile reports 6.18.55. raspberrypi/firmware master extra/uname_string8 reports 6.18.55-v8+ (built 2026-10-05).
- **Source:** GitHub API repos/raspberrypi/linux; raspberrypi/linux rpi-6.18.y Makefile — <https://github.com/raspberrypi/linux/tree/rpi-6.18.y>
- **Evidence:** API default_branch: 'rpi-6.18.y'; Makefile: 'VERSION = 6 PATCHLEVEL = 18 SUBLEVEL = 55'; firmware master uname_string8: 'Linux version 6.18.55-v8+ ... Mon Oct 5 16:54:54 BST 2026'

### E-38

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux
- **Fact:** Kconfig in drivers/media/i2c/Kconfig (rpi-6.18.y): VIDEO_TC358743 is tristate 'Toshiba TC358743 decoder' (depends on VIDEO_DEV && I2C; selects MEDIA_CONTROLLER, VIDEO_V4L2_SUBDEV_API, HDMI and V4L2_FWNODE; module tc358743). VIDEO_TC358743_CEC is bool, depends on VIDEO_TC358743 and selects CEC_CORE.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/Kconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/Kconfig>
- **Evidence:** Kconfig:1415 'config VIDEO_TC358743 tristate "Toshiba TC358743 decoder" depends on VIDEO_DEV && I2C select MEDIA_CONTROLLER ...'; :1428 'config VIDEO_TC358743_CEC bool ... depends on VIDEO_TC358743 select CEC_CORE'

### E-39

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Linux, Pi4, CM4, Pi5, CM5
- **Fact:** The Raspberry Pi arm64 bcm2711_defconfig and bcm2712_defconfig set CONFIG_VIDEO_TC358743=m but do not set CONFIG_VIDEO_TC358743_CEC, in both rpi-6.18.y and the 6.12.61 commit used by Buildroot 2026.08. Both also set CONFIG_I2C_BCM2835=m and CONFIG_I2C_MUX_PINCTRL=m.
- **Source:** raspberrypi/linux arch/arm64/configs/bcm2711_defconfig, bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2711_defconfig>
- **Evidence:** rpi-6.18.y bcm2711_defconfig:1077 'CONFIG_VIDEO_TC358743=m', bcm2712_defconfig:1079 same; grep -c TC358743_CEC = 0 in both files on both branches

### E-40

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi4, CM4, TC358743
- **Fact:** Unicam in rpi-6.18.y and in 6.12.61 has two drivers. (1) VIDEO_BCM2835_UNICAM_LEGACY (drivers/media/platform/bcm2835/, Makefile builds bcm2835-unicam-legacy.ko, although its Kconfig help text says 'bcm2835-unicam') is the downstream driver. It matches "brcm,bcm2835-unicam" (forces Media Controller mode) and "brcm,bcm2835-unicam-legacy" (video-node mode unless the media_controller module parameter is set or the DT node has the 'brcm,media-controller' property). (2) VIDEO_BCM2835_UNICAM (drivers/media/platform/broadcom/, module bcm2835-unicam.ko) is the mainline Media-Controller-only driver and matches only "brcm,bcm2835-unicam-upstream". Both are =m in bcm2711 and bcm2712 defconfigs. The base DT csi0/csi1 nodes (bcm283x.dtsi) use compatible "brcm,bcm2835-unicam", so by default the downstream driver binds, in MC mode.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/platform/bcm2835 and broadcom (Kconfig, Makefile, of_match) — <https://github.com/raspberrypi/linux/tree/rpi-6.18.y/drivers/media/platform/bcm2835>
- **Evidence:** bcm2835/Makefile 'obj-$(CONFIG_VIDEO_BCM2835_UNICAM_LEGACY) += bcm2835-unicam-legacy.o'; bcm2835-unicam.c:3425 '{ .compatible = "brcm,bcm2835-unicam", .data = (void *)1 }, { .compatible = "brcm,bcm2835-unicam-legacy", .data = 0 }'; broadcom/bcm2835-unicam.c:2764 '{ .compatible = "brcm,bcm2835-unicam-upstream", }'; defconfig 'CONFIG_VIDEO_BCM2835_UNICAM_LEGACY=m CONFIG_VIDEO_BCM2835_UNICAM=m'
- **Original claim (before verification):** Unicam in rpi-6.18.y and in 6.12.61 has two drivers. (1) VIDEO_BCM2835_UNICAM_LEGACY (drivers/media/platform/bcm2835/, module bcm2835-unicam-legacy.ko) is the downstream driver: it matches "brcm,bcm2835-unicam" (Media Controller mode) and "brcm,bcm2835-unicam-legacy" (video-node mode unless the media_controller module parameter is set). (2) VIDEO_BCM2835_UNICAM (drivers/media/platform/broadcom/, module bcm2835-unicam.ko) is the mainline Media-Controller-only driver and matches only "brcm,bcm2835-unicam-upstream". Both are =m in bcm2711 and bcm2712 defconfigs.
- **Verifier note:** The claim omitted the 'brcm,media-controller' DT property. Probe code at rpi-6.18.y bcm2835-unicam.c:3300-3307: mc_api = media_controller param, forced true if match data is set, and forced true if of_property_read_bool('brcm,media-controller'). of_match is at 3425-3426, and broadcom/bcm2835-unicam.c:2764 is 'brcm,bcm2835-unicam-upstream'. 6.12.61 has the same at 3508-3509 and 2714. The defconfigs set UNICAM_LEGACY=m and UNICAM=m. bcm283x.dtsi:458 and :470 use 'brcm,bcm2835-unicam'.

### E-41

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Buildroot, Pi4, CM4
- **Fact:** In Linux 6.6.28 (the kernel of Buildroot LTS 2025.02.18), VIDEO_BCM2835_UNICAM is the downstream driver in drivers/media/platform/bcm2835/ and matches "brcm,bcm2835-unicam". That tree has no drivers/media/platform/broadcom/ directory and no VIDEO_BCM2835_UNICAM_LEGACY symbol.
- **Source:** raspberrypi/linux @576cc10e (6.6.28) drivers/media/platform/bcm2835/Kconfig — <https://github.com/raspberrypi/linux/tree/576cc10e1ed50a9eacffc7a05c796051d7343ea4/drivers/media/platform/bcm2835>
- **Evidence:** drivers/media/platform/bcm2835/Kconfig: 'config VIDEO_BCM2835_UNICAM'; of_match '{ .compatible = "brcm,bcm2835-unicam", }'; 'drivers/media/platform/broadcom: No such file or directory'

### E-42

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** TC358743, Pi4, CM4, Linux
- **Fact:** The rpi-6.18.y tc358743-overlay.dts includes tc358743.dtsi (I2C address 0x0f, compatible "toshiba,tc358743", 27 MHz cam1_clk refclk, link-frequencies 486000000, 2 data lanes by default). By default, fragment@100 changes csi1's compatible to "brcm,bcm2835-unicam-legacy". The 'media-controller' override removes that fragment, and the overlays README describes it as 'Configure use of Media Controller API for configuring the sensor (default off)'.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743-overlay.dts, tc358743.dtsi, README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-overlay.dts>
- **Evidence:** overlay:14 'compatible = "brcm,bcm2835-unicam-legacy";' :19 'media-controller = <0>,"!100";'; dtsi: 'reg = <0x0f>;' 'link-frequencies = /bits/ 64 <486000000>;' 'clock-frequency = <27000000>;'; README: 'media-controller Configure use of Media Controller API ... (default off)'

### E-43

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** TC358743, Pi5, CM5, Linux
- **Fact:** In rpi-6.18.y, overlay_map.dts maps 'tc358743' to 'tc358743-pi5' on bcm2712. tc358743-pi5-overlay.dts includes the same tc358743.dtsi without the legacy-compatible fragment, and on Pi 5 and CM5 the csi1 label points to &rp1_csi1 (compatible "raspberrypi,rp1-cfe").
- **Source:** raspberrypi/linux rpi-6.18.y overlays/overlay_map.dts, tc358743-pi5-overlay.dts, bcm2712-rpi-5-b.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/overlay_map.dts>
- **Evidence:** overlay_map.dts: 'tc358743 { bcm2835; bcm2711; bcm2712 = "tc358743-pi5"; };'; bcm2712-rpi-5-b.dts:225 'csi1: &rp1_csi1 { };'; rp1.dtsi 'rp1_csi1: csi@128000 { compatible = "raspberrypi,rp1-cfe";'

### E-44

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi5, CM5
- **Fact:** rpi-6.18.y has two RP1 CFE drivers. VIDEO_RP1_CFE (drivers/media/platform/raspberrypi/rp1-cfe/, the mainline driver, module rp1-cfe.ko) matches only "raspberrypi,rp1-cfe-upstream". VIDEO_RP1_CFE_DOWNSTREAM (drivers/media/platform/raspberrypi/rp1_cfe/, module rp1-cfe-downstream.ko) matches "raspberrypi,rp1-cfe", which is the compatible used by the rp1.dtsi csi nodes. Both are =m in bcm2711 and bcm2712 defconfigs.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/platform/raspberrypi/rp1-cfe and rp1_cfe — <https://github.com/raspberrypi/linux/tree/rpi-6.18.y/drivers/media/platform/raspberrypi>
- **Evidence:** rp1_cfe/Kconfig 'config VIDEO_RP1_CFE_DOWNSTREAM ... module will be called rp1-cfe-downstream'; rp1_cfe/cfe.c:2470 '{ .compatible = "raspberrypi,rp1-cfe" }'; rp1-cfe/cfe.c:2504 '{ .compatible = "raspberrypi,rp1-cfe-upstream" }'; bcm2712_defconfig:1040-1041 'CONFIG_VIDEO_RP1_CFE=m CONFIG_VIDEO_RP1_CFE_DOWNSTREAM=m'

### E-45

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Buildroot, Pi5, CM5
- **Fact:** In the 6.12.61 commit used by Buildroot 2026.08 (21b41014), only drivers/media/platform/raspberrypi/rp1_cfe/ exists. There, VIDEO_RP1_CFE builds module rp1-cfe.ko from the downstream driver, which matches "raspberrypi,rp1-cfe". The meaning of the symbol VIDEO_RP1_CFE therefore differs between rpi-6.12.y and rpi-6.18.y.
- **Source:** raspberrypi/linux @21b41014 drivers/media/platform/raspberrypi/rp1_cfe/{Kconfig,Makefile,cfe.c} — <https://github.com/raspberrypi/linux/tree/21b410140c47ffab5668399f6f143c7d7b935c8b/drivers/media/platform/raspberrypi>
- **Evidence:** rp1_cfe/Kconfig:3 'config VIDEO_RP1_CFE'; Makefile 'obj-$(CONFIG_VIDEO_RP1_CFE) += rp1-cfe.o'; cfe.c:2451 '{ .compatible = "raspberrypi,rp1-cfe" }'; raspberrypi/ contains only hevc_dec pisp_be rp1_cfe

### E-46

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi4, CM4, Pi5, CM5
- **Fact:** VIDEO_CODEC_BCM2835 (drivers/staging/vc04_services/bcm2835-codec, module bcm2835-codec, a V4L2 mem2mem codec over VCHIQ to VideoCore) and VIDEO_ISP_BCM2835 (module bcm2835-isp) both select BCM2835_VCHIQ_MMAL, which selects BCM_VC_SM_CMA. Both are =m in the arm64 bcm2711_defconfig and bcm2712_defconfig (rpi-6.18.y and 6.12.61), alongside CONFIG_BCM2835_VCHIQ=y.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/staging/vc04_services/{bcm2835-codec,bcm2835-isp,vchiq-mmal}/Kconfig; arm64 defconfigs — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/staging/vc04_services/bcm2835-codec/Kconfig>
- **Evidence:** 'config VIDEO_CODEC_BCM2835 tristate "BCM2835 Video codec support" ... select BCM2835_VCHIQ_MMAL ... select V4L2_MEM2MEM_DEV'; vchiq-mmal/Kconfig 'select BCM_VC_SM_CMA'; bcm2711_defconfig:1556-1557 'CONFIG_VIDEO_CODEC_BCM2835=m CONFIG_VIDEO_ISP_BCM2835=m'

### E-47

- **Verdict:** `CORRECTED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi4, CM4, Pi5, CM5
- **Fact:** The RPi arm64 defconfigs set CONFIG_CMA=y, CONFIG_DMA_CMA=y and CONFIG_CMA_SIZE_MBYTES=5. The CMA region in use comes from the device tree 'cma: linux,cma' node (shared-dma-pool, reusable, linux,cma-default, size 0x4000000 = 64 MB) in bcm283x.dtsi and bcm2712.dtsi. bcm2711.dtsi sets alloc-ranges to the lower 1 GB, but bcm2711-rpi-ds.dtsi, which bcm2711-rpi-4-b.dts and bcm2711-rpi-cm4.dts include afterwards, overrides it to <0x0 0x00000000 0x30000000>, the lower 768 MB. bcm2712.dtsi sets alloc-ranges <0x0 0x00000000 0x0 0x40000000> (lower 1 GB). The 'cma' overlay offers cma-64 through cma-512, cma-size (bytes, 4 MB aligned) and cma-default.
- **Source:** raspberrypi/linux rpi-6.18.y defconfigs; bcm283x.dtsi; bcm2711.dtsi; bcm2712.dtsi; overlays/README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712.dtsi>
- **Verifier's best source:** <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm2711-rpi-ds.dtsi>
- **Evidence:** bcm2711_defconfig:1775-1776 'CONFIG_DMA_CMA=y CONFIG_CMA_SIZE_MBYTES=5'; bcm283x.dtsi:38-40 'cma: linux,cma { compatible = "shared-dma-pool"; size = <0x4000000>; /* 64MB */'; bcm2711.dtsi:1118 '&cma { ... alloc-ranges = <0x0 0x00000000 0x40000000>;'; README 'Name: cma ... cma-512 CMA is 512MB (needs 1GB) ... cma-size CMA size in bytes, 4MB aligned'
- **Original claim (before verification):** The RPi arm64 defconfigs set CONFIG_CMA=y, CONFIG_DMA_CMA=y and CONFIG_CMA_SIZE_MBYTES=5. The CMA region in use is set in the device tree: 'cma: linux,cma' (shared-dma-pool, reusable, linux,cma-default) with size 0x4000000 (64 MB) in bcm283x.dtsi, which bcm2711.dtsi includes and restricts to below 1 GB, and in bcm2712.dtsi. The 'cma' overlay offers cma-64 through cma-512 and cma-size parameters.
- **Verifier note:** The claim implied 1 GB was the effective limit on Pi 4/CM4, but the DT includes override it to 768 MB. rpi-6.18.y bcm283x.dtsi:38-43 and bcm2711.dtsi:1118-1124 (alloc-ranges 0x40000000) are as cited. bcm2711-rpi-ds.dtsi:79-81 has '&cma { /* Limit cma to the lower 768MB to allow room for HIGHMEM on 32-bit */ alloc-ranges = <0x0 0x00000000 0x30000000>; }', and bcm2711-rpi-4-b.dts:299 and bcm2711-rpi-cm4.dts:272 include it after bcm2711.dtsi. bcm2712.dtsi:180-185 has size <0x0 0x4000000> and alloc-ranges lower 1 GB. Defconfig lines 88 (CMA=y), 1775-1776 (DMA_CMA, CMA_SIZE_MBYTES=5) and CMA_AREAS=7 confirmed. Not verified: whether the firmware or other overlays such as vc4-kms-v3d change CMA at boot.

### E-48

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Pi4, CM4, Pi5, CM5
- **Fact:** The RPi arm64 defconfigs set CONFIG_DMABUF_HEAPS=y, CONFIG_DMABUF_HEAPS_SYSTEM=y and CONFIG_DMABUF_HEAPS_CMA=y. In rpi-6.18.y, cma_heap.c registers the default CMA heap as "default_cma_region" and, when DMABUF_HEAPS_CMA_LEGACY (default y) is set, a second heap named after the CMA area. In 6.12.61 the heap is registered only under cma_get_name(cma).
- **Source:** raspberrypi/linux rpi-6.18.y drivers/dma-buf/heaps/{Kconfig,cma_heap.c}; @21b41014 cma_heap.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/dma-buf/heaps/cma_heap.c>
- **Evidence:** 6.18 cma_heap.c:28 '#define DEFAULT_CMA_NAME "default_cma_region"' :411-418 'if (IS_ENABLED(CONFIG_DMABUF_HEAPS_CMA_LEGACY)) { legacy_cma_name = cma_get_name(default_cma); ...'; Kconfig 'config DMABUF_HEAPS_CMA_LEGACY ... default y'; 6.12.61 cma_heap.c:379 'exp_info.name = cma_get_name(cma);'

### E-49

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Linux, Buildroot
- **Fact:** The RPi arm64 defconfigs build the media subsystem as modules (CONFIG_MEDIA_SUPPORT=m) and compress modules with XZ (CONFIG_MODULE_COMPRESS=y, CONFIG_MODULE_COMPRESS_XZ=y). The Buildroot RPi defconfigs enable BR2_PACKAGE_KMOD, BR2_PACKAGE_KMOD_TOOLS, BR2_PACKAGE_XZ and BR2_PACKAGE_HOST_KMOD_XZ.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2711_defconfig; Buildroot 2026.08 raspberrypi4_64_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2711_defconfig>
- **Evidence:** bcm2711_defconfig:80-81 'CONFIG_MODULE_COMPRESS=y CONFIG_MODULE_COMPRESS_XZ=y', :905 'CONFIG_MEDIA_SUPPORT=m'; Buildroot defconfig 'BR2_PACKAGE_KMOD=y BR2_PACKAGE_KMOD_TOOLS=y ... BR2_PACKAGE_HOST_KMOD_XZ=y'

### E-50

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Buildroot
- **Fact:** Buildroot's default /dev management is 'Dynamic using devtmpfs only' (BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_DEVTMPFS). Manual §6.2 says the devtmpfs + mdev option is what 'can be used to automatically load kernel modules when devices appear'. With systemd, udev handles /dev.
- **Source:** Buildroot 2026.08 system/Config.in; manual §6.2 /dev management — <https://buildroot.org/downloads/manual/manual.html>
- **Evidence:** system/Config.in:276-277 'prompt "/dev management" ... default BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_DEVTMPFS'; manual: 'mdev can for example be used to automatically load kernel modules when devices appear on the system.'

### E-51

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Linux, Pi4, CM4, Pi5, CM5
- **Fact:** Official Raspberry Pi kernel build documentation: 64-bit bcm2711_defconfig (KERNEL=kernel8) covers Pi 3, 4, 400, CM4, CM4S and Pi 5/CM5. bcm2712_defconfig builds kernel_2712.img, which is optimised for BCM2712 (Pi 5, 500, 500+, CM5) and uses a 16K page size.
- **Source:** Raspberry Pi documentation - The Linux kernel — <https://www.raspberrypi.com/documentation/computers/linux_kernel.html>
- **Evidence:** 'bcm2712_defconfig builds kernel_2712.img, which is optimised for BCM2712 devices (Raspberry Pi 5, 500, 500+, CM5) and uses a 16K page size.'

### E-52

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi5, CM5, Streaming
- **Fact:** Official Raspberry Pi camera software documentation says Raspberry Pi 5 uses software video encoders rather than the hardware H.264 encoder of earlier models, and notes they generally output frames with longer latency.
- **Source:** Raspberry Pi documentation - Camera software — <https://www.raspberrypi.com/documentation/computers/camera_software.html>
- **Evidence:** 'Raspberry Pi 5 uses software video encoders' ... 'These generally output frames with a longer latency than the old hardware encoders'

### E-53

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Buildroot, TC358743, Pi5, CM5
- **Fact:** The rpi-firmware commit used by Buildroot 2026.08 (063bcab6) contains boot/overlays/tc358743.dtbo, boot/overlays/tc358743-pi5.dtbo and boot/overlays/overlay_map.dtb. The LTS 2025.02.18 firmware commit (5476720d) has tc358743.dtbo and overlay_map.dtb but no tc358743-pi5.dtbo (HTTP 404).
- **Source:** raspberrypi/firmware boot/overlays at 063bcab6 and 5476720d — <https://github.com/raspberrypi/firmware/tree/063bcab6c8a90efb0d19f69d88cbbc7ec79cab68/boot/overlays>
- **Evidence:** HTTP HEAD raw.githubusercontent.com/raspberrypi/firmware/063bcab6.../boot/overlays/tc358743-pi5.dtbo -> 200; .../5476720d.../boot/overlays/tc358743-pi5.dtbo -> 404

---

## Topic F

**Blackmagic ATEM integration and streaming protocols** — <a id="topic-f"></a>46 claims.

### F-01

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Linux
- **Fact:** The official Blackmagic ATEM Switchers SDK supports only Microsoft Windows and macOS. Its manual names no Linux host.
- **Source:** ATEM Switchers SDK manual, December 2025 (Introduction, p.42) — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "The Switcher SDK supports Microsoft Windows and macOS OS." (cover page: "Windows / Mac OS")

### F-02

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM
- **Fact:** The latest ATEM SDK listed by Blackmagic is 'ATEM Switchers 10.4.1 SDK', dated 03 Sep 2026, for platforms 'Mac OS X' and 'Windows' only. Downloading it requires registration and acceptance of the terms 'bmd-standard-sdk'.
- **Source:** Blackmagic Design support downloads API (downloads.json), entry id ad86a2836b4b46adbba3c5f15fd2d590 — <https://www.blackmagicdesign.com/api/support/us/downloads.json>
- **Evidence:** name: ATEM Switchers 10.4.1 SDK | date: 03 Sep 2026 | platforms: ['Mac OS X', 'Windows'] | requiresRegistration: True | termsAndConditions: bmd-standard-sdk

### F-03

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM
- **Fact:** The ATEM SDK API is modelled on Microsoft COM. On Windows it is a native COM interface: CoCreateInstance(CLSID_CBMDSwitcherDiscovery, ..., IID_IBMDSwitcherDiscovery). On macOS the C++ entry point is CreateBMDSwitcherDiscoveryInstance(). The include files are 'Switchers X.Y\Win\Include\BMDSwitcherAPI.idl' and 'Switchers X.Y/macOS/Include/BMDSwitcherAPI.h'.
- **Source:** ATEM Switchers SDK manual, December 2025, sections 1.2, 1.7, 1.7.2 — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "The SDK interface is modeled on Microsoft's Component Object Model (COM)." ... "IBMDSwitcherDiscovery* switcherDiscovery = CreateBMDSwitcherDiscoveryInstance();"

### F-04

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Linux
- **Fact:** The ATEM SDK runtime libraries ship inside the ATEM product installers, which exist for Mac and Windows only. Applications link dynamically against the library installed on the end user's system.
- **Source:** ATEM Switchers SDK manual, December 2025, section 1.1 — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "The libraries supporting the Blackmagic SDK are shipped as part of the product installers ... Applications built against the interfaces shipped in the SDK will dynamically link against the library installed on the end-user's system." (ATEM Switchers 10.4.1 Update platforms: ['Mac OS X','Windows'])

### F-05

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM
- **Fact:** IBMDSwitcherDiscovery::ConnectTo(deviceAddress, switcherDevice, failReason) makes a synchronous network connection to a hostname or IP address. If no network connection can be made, it tries USB, but only if the switcher supports USB. An empty deviceAddress connects over USB only.
- **Source:** ATEM Switchers SDK manual, December 2025, section 2.3.1.1 — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "ConnectTo performs a synchronous network connection ... If a network connection cannot be established, ConnectTo will attempt to connect via USB"; deviceAddress: "Network hostname or IP address of switcher ... Set this empty to only connect via USB."
- **Original claim (before verification):** IBMDSwitcherDiscovery::ConnectTo(deviceAddress, ...) makes a synchronous network connection to a hostname or IP address. If the network connection fails it falls back to USB, and an empty deviceAddress connects over USB only.
- **Verifier note:** Section 2.3.1.1 says: 'ConnectTo performs a synchronous network connection. This may take several seconds ... If a network connection cannot be established, ConnectTo will attempt to connect via USB if the switcher supports it.' The deviceAddress parameter is described as: 'Set this empty to only connect via USB.' The original claim left out the qualifier 'if the switcher supports it'.

### F-06

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Streaming
- **Fact:** The ATEM SDK Streaming API (IBMDSwitcherStreamRTMP) streams H.264 or H.265 video with AAC audio over RTMP or SRT. Its states are bmdSwitcherStreamRTMPStateIdle, Connecting, Streaming and Stopping. Its methods include SetUrl, SetKey, CanStreamSRT and SetProfileXml.
- **Source:** ATEM Switchers SDK manual, December 2025, Section 11 Streaming — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "The switcher will stream H.264 or H.265 encoded video and AAC encoded audio via the RTMP or SRT streaming protocols to a selected streaming server."

### F-07

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM
- **Fact:** The ATEM SDK Recording API (IBMDSwitcherRecordAV) records H.264 video and AAC audio as MP4 to an externally connected disk. Its states are bmdSwitcherRecordAVStateIdle, Recording and Stopping. Its errors include bmdSwitcherRecordAVErrorNoMedia, MediaFull and DroppingFrames.
- **Source:** ATEM Switchers SDK manual, December 2025, Section 12 Recording — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "Recordings from the switcher are stored as H.264 video and AAC audio in MP4 file format onto any externally connected disk (USB disk. flash drive, or Blackmagic Design MultiDock)."

### F-08

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM
- **Fact:** In the ATEM SDK, per-input tally state is read with IBMDSwitcherInput::IsProgramTallied(bool*) and IBMDSwitcherInput::IsPreviewTallied(bool*). Changes are signalled by bmdSwitcherInputEventTypeIsProgramTalliedChanged and IsPreviewTalliedChanged.
- **Source:** ATEM Switchers SDK manual, December 2025, sections 2.3.5.12-2.3.5.13 — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "The IsProgramTallied method determines whether this switcher input is currently program tallied. ... HRESULT IsProgramTallied(bool* isTallied)"

### F-09

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Linux
- **Fact:** Per the ATEM SDK manual, uncompressed USB 3 video capture and H.264 streaming over USB from switchers are accessed through the separate DeckLink SDK, not the Switcher SDK. The DeckLink SDK ('Desktop Video 16.0 SDK', 08 Apr 2026) does list Linux as a platform.
- **Source:** ATEM Switchers SDK manual (Overview) + Blackmagic downloads.json — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** "Some Switcher capabilities, such as uncompressed USB 3 video capture and H.264 video streaming, must be accessed using the separate DeckLink SDK."; downloads.json: "Desktop Video 16.0 SDK | 08 Apr 2026 | ['Mac OS X', 'Windows', 'Linux']"

### F-10

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM
- **Fact:** The official ATEM SDK manual does not document the ATEM wire protocol. A text search of the December 2025 manual finds no occurrence of '9910' or 'UDP'.
- **Source:** ATEM Switchers SDK manual, December 2025 (full-text search of 822 pages) — <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** grep -i '9910|udp' on extracted text: no matches (only 'protocol' hits are VISCA, Camera Control Protocol and streaming protocols)

### F-11

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** ATEM Software Control talks to the switcher over a custom UDP protocol on port 9910. The protocol has sequence numbers, retransmissions, acknowledgements and a TCP-like 3-way handshake. The OpenSwitcher project documents it as reverse-engineered, not as an official specification.
- **Source:** OpenSwitcher docs - The low level UDP protocol — <https://docs.openswitcher.org/udptransport.html>
- **Evidence:** "The ATEM Software Control application communicates with the mixer over a custom UDP protocol on port 9910. ... re-invention of TCP over UDP"; index: "documentation for the reverse-engineered UDP protocol"

### F-12

- **Verdict:** `CORRECTED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** An ATEM UDP packet has a fixed 12-byte header. In order it holds: a 5-bit flags field, an 11-bit packet length (which includes the 12-byte header), a 16-bit session ID, a 16-bit acknowledgement number, a 16-bit field of unknown purpose, a 16-bit remote sequence number and a 16-bit local sequence number. The flag bits are 0=Reliable, 1=SYN, 2=Retransmission, 3=Request retransmission, 4=ACK. The payload is a series of commands. Each command has a 16-bit length (header included), 2 padding/unknown bytes, a 4-ASCII-character name, and then length-8 bytes of data.
- **Source:** OpenSwitcher docs - The low level UDP protocol — <https://docs.openswitcher.org/udptransport.html>
- **Evidence:** "This is the header for a packet in the UDP protocol, it's always 12 bytes long." ... "sent as a 16 bit length of the field/command, 2 unknown bytes ... and 4 ASCII characters signifying the field/command type."
- **Original claim (before verification):** An ATEM UDP packet has a fixed 12-byte header containing flags, packet length, a 16-bit session, an acknowledgement number, a remote sequence number and a local sequence number. The flag bits are 0=Reliable, 1=SYN, 2=Retransmission, 3=Request retransmission, 4=ACK. The payload is a series of commands, each with a 16-bit length, 2 padding bytes and a 4-ASCII-character command name.
- **Verifier note:** In the OpenSwitcher header table HTML, flags has colspan=5 and packet length colspan=11 on the first row, then session (16 bits), acknowledgement number (16), 'unknown' (16), remote sequence number (16) and local sequence number (16). The original claim left out the 16-bit 'unknown' field and the bit widths. The flag bit table matches. The payload text says: '16 bit length of the field/command, 2 unknown bytes which are most likely padding and 4 ASCII characters'.

### F-13

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** ATEM connection handshake: the client sends SYN with payload 01 00 00 00 00 00 00 00. The switcher replies with SYN and status byte 0x02 on success, or 0x04 meaning the client must restart. After the client's ACK, the switcher sends its full state, ending with an empty packet. Reliable packets must then be ACKed.
- **Source:** OpenSwitcher docs - Starting a connection — <https://docs.openswitcher.org/udptransport.html>
- **Evidence:** "The payload for this packet should be 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00." ... "On a successful connection this should be 0x02. If the switcher responds with 0x04 you should restart" ... "the switcher will start dumping the full state of all fields"

### F-14

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM, Linux
- **Fact:** atem-connection (Sofie, originally NRK) is a TypeScript library under the MIT licence (Copyright 2023 Norsk rikskringkasting AS). Its latest npm release is 3.10.3, published 2026-10-01. The repository is now github.com/Sofie-Automation/sofie-atem-connection. It sets DEFAULT_PORT = 9910 and opens a Node 'dgram' 'udp4' socket. It requires Node '^14.18 || ^16.14 || >=18.0'.
- **Source:** sofie-atem-connection: LICENSE, src/atem.ts, src/lib/atemSocketChild.ts, package.json; npm registry — <https://github.com/Sofie-Automation/sofie-atem-connection>
- **Evidence:** src/atem.ts:71 "export const DEFAULT_PORT = 9910"; atemSocketChild.ts:241 "createSocket('udp4')"; LICENSE "MIT License Copyright (c) 2023 Norsk rikskringkasting AS"; npm latest 3.10.3 2026-10-01T11:50:38Z

### F-15

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** atem-connection officially targets ATEM firmware v8.0 to latest. v7.2 should still work, and v7.3-v7.5.2 are community tested only. It does not support USB control. The project warns that new firmware will likely require library updates.
- **Source:** sofie-atem-connection README (Device support) — <https://github.com/Sofie-Automation/sofie-atem-connection/blob/main/README.md>
- **Evidence:** "v8.0 - Latest | Primary focus" ... "Due to the nature of the ATEM firmware and its tendency to break things, it is likely that new firmwares will require updates to the library" ... "Note: USB control of devices is not supported by this library."

### F-16

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM, Streaming
- **Fact:** atem-connection implements recording and streaming status and control. Recording uses commands 'RcTM' (set recording) and 'RTMS' (status). Streaming uses 'StrR' (set streaming) and 'StRS' (status). The streaming service is set with 'CRSS' (serviceName 64 bytes, url 512 bytes, key 512 bytes) and read back with 'SRSU'. All four status commands require minimumVersion ProtocolVersion.V8_1_1 (0x0002001e, protocol 2.30).
- **Source:** sofie-atem-connection src/commands/Recording/RecordingStatusCommand.ts, Streaming/StreamingStatusCommand.ts, Streaming/StreamingServiceCommand.ts, src/enums/index.ts — <https://github.com/Sofie-Automation/sofie-atem-connection/tree/main/src/commands>
- **Evidence:** RecordingStatusCommand rawName = 'RcTM', minimumVersion = ProtocolVersion.V8_1_1; RecordingStatusUpdateCommand rawName = 'RTMS'; StreamingStatusCommand 'StrR'; StreamingStatusUpdateCommand 'StRS'; enums: "V8_1_1 = 0x0002001e, // 2.30"

### F-17

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** atem-connection decodes tally with 'TlSr' (TallyBySourceCommand) and controls macros with 'MAct' (MacroActionCommand: Run=0, Stop=1, StopRecord=2, InsertUserWait=3, Continue=4, Delete=5). It routes aux outputs with 'CAuS' (AuxSourceCommand) and reads them with 'AuxS'. Program input appears at state path video.mixEffects.0.programInput.
- **Source:** sofie-atem-connection src/commands/TallyBySourceCommand.ts, Macro/MacroActionCommand.ts, AuxSourceCommand.ts, src/enums/index.ts, README — <https://github.com/Sofie-Automation/sofie-atem-connection>
- **Evidence:** TallyBySourceCommand rawName = 'TlSr'; MacroActionCommand rawName = 'MAct'; enum MacroAction { Run = 0, Stop = 1, StopRecord = 2, InsertUserWait = 3, Continue = 4, Delete = 5 }; AuxSourceCommand rawName = 'CAuS'

### F-18

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** The highest protocol version in atem-connection's ProtocolVersion enum is V9_6 = 0x00020020 (protocol 2.32). The enum has no 10.x entry, although Blackmagic has shipped ATEM 10.x software (10.4.1 on 03 Sep 2026).
- **Source:** sofie-atem-connection src/enums/index.ts; Blackmagic downloads.json — <https://github.com/Sofie-Automation/sofie-atem-connection/blob/main/src/enums/index.ts>
- **Evidence:** "V9_4 = 0x0002001f, // 2.31", "V9_6 = 0x00020020, // 2.32" (last entry); downloads.json "ATEM Switchers 10.4.1 Update | 03 Sep 2026"

### F-19

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM, Linux
- **Fact:** PyATEMMax is a Python 3 library under GPL-3.0 (LICENSE.md). It is a port of Kasper Skårhøj's ATEMmax Arduino library and is based on the protocol 'as reverse engineered by Skårhøj'. It sets UDPPort = 9910 in ATEMProtocol.py. Its latest PyPI release is 1.0b9, uploaded 2022-09-16.
- **Source:** clvLabs/PyATEMMax README, LICENSE.md, PyATEMMax/ATEMProtocol.py, docs site; PyPI JSON — <https://github.com/clvLabs/PyATEMMax>
- **Evidence:** "It's a port of the great ATEMmax Arduino library by Kasper Skårhøj."; LICENSE.md "GNU GENERAL PUBLIC LICENSE Version 3"; ATEMProtocol.py:28-29 "# ATEM protocol standard UDP port / UDPPort: int = 9910"; PyPI 1.0b9 2022-09-16

### F-20

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM
- **Fact:** PyATEMMax's command table includes tally ('TlIn' Tally By Index, 'TlSr' Tally By Source) and macro status ('MRcS'). Searching its protocol and handler sources finds no recording-status ('RTMS') or streaming-status ('StRS') commands.
- **Source:** clvLabs/PyATEMMax PyATEMMax/ATEMProtocol.py, ATEMCommandHandlers.py — <https://github.com/clvLabs/PyATEMMax/blob/master/PyATEMMax/ATEMProtocol.py>
- **Evidence:** ATEMProtocol.py:162-163 "\"TlIn\": 'Tally By Index', \"TlSr\": 'Tally By Source'"; grep -i 'stream|record|RTMS|StRS' only matched 'MRcS': 'Macro Recording Status' and downstream-keyer names

### F-21

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM, Linux
- **Fact:** In pyatem/OpenSwitcher, the pyatem protocol library is LGPL-3.0-only, while the gtk_switcher, openswitcher_proxy and bmd_setup programs are GPL-3.0-only. pyatem supports network control and USB (via pyusb), and OpenSwitcher offers livestream and recording controls for the ATEM Mini series.
- **Source:** pyatem LICENSE and README (sourcehut); openswitcher.org — <https://git.sr.ht/~martijnbraam/pyatem>
- **Evidence:** "The pyatem library is LGPL-3.0-only / The gtk_switcher, openswitcher_proxy and bmd_setup programs are GPL-3.0-only"; README: "The only external dependency for pyatem is pyusb for the USB protocol support"

### F-22

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** ATEM, Linux
- **Fact:** LibAtem is a C# library for .NET Core 3.0 under LGPL-3.0. It has been tested on Windows and Linux, including Raspberry Pi, with primary support for ATEM firmware 8.0+. Its README calls it incomplete: some macro operations, much of the audio mixer and camera control are missing.
- **Source:** LibAtem README and LICENSE — <https://github.com/LibAtem/LibAtem>
- **Evidence:** "It is written for .Net Core 3.0 and has been tested on Windows and Linux (including Raspberry Pi)"; LICENSE: "GNU LESSER GENERAL PUBLIC LICENSE Version 3"

### F-23

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, TC358743
- **Fact:** ATEM Mini Pro has 1 HDMI program output. Its HD video output standards are 1080p23.98, 1080p24, 1080p25, 1080p29.97, 1080p30, 1080p50, 1080p59.94 and 1080p60. There are no 720p, 1080i or Ultra HD output standards. Video sampling is 4:2:2 YUV, 10-bit, Rec 709.
- **Source:** Blackmagic ATEM Mini tech specs - ATEM Mini Pro — <https://www.blackmagicdesign.com/products/atemmini/techspecs/W-APS-14>
- **Evidence:** "HDMI Program Outputs 1" ... "HD Video Output Standards 1080p23.98, 1080p24, 1080p25, 1080p29.97, 1080p30, 1080p50, 1080p59.94, 1080p60" ... "Ultra HD Video Standards None" "Video Sampling 4:2:2 YUV"

### F-24

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, TC358743
- **Fact:** ATEM Mini Pro and ATEM Mini Pro ISO have no dedicated audio output connector ('Total Audio Outputs: None, embedded audio only'). Mixed program audio is output as embedded digital audio on the HDMI output and the USB-C webcam output. It is also carried in the RTMP/SRT stream and in USB recordings. The HDMI inputs carry 2-channel embedded audio.
- **Source:** Blackmagic ATEM Mini tech specs - ATEM Mini Pro — <https://www.blackmagicdesign.com/products/atemmini/techspecs/W-APS-14>
- **Evidence:** "Total Audio Outputs None, embedded audio only."; "USB Audio Output Supports digital audio output over USB-C."
- **Original claim (before verification):** ATEM Mini Pro (and Pro ISO) have no separate audio output. Program audio leaves only as audio embedded in the HDMI or USB output, and the HDMI inputs carry 2-channel embedded audio.
- **Verifier note:** Both the Mini Pro and Pro ISO blocks say 'Total Audio Outputs: None, embedded audio only' and 'HDMI Video Inputs ... 2 channel embedded audio'. The quoted evidence line 'USB Audio Output: Supports digital audio output over USB-C' does NOT appear in the ATEM Mini Pro block. It appears only in the Pro ISO, Extreme and Extreme ISO G2 blocks. The manual (p.175) says audio is 'output over USB webcam and HDMI outputs as embedded digital audio'. The HDMI-out audio channel count and sample rate are not specified for the Mini Pro.

### F-25

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, TC358743
- **Fact:** The ATEM Mini Pro's HDMI output defaults to multiview, not program. On ATEM Mini Extreme, HDMI out 1 defaults to program and out 2 to multiview. The source is changed with the front-panel VIDEO OUT buttons or the 'output' menu in ATEM Software Control. Selectable sources include inputs, program, preview and camera 1 direct.
- **Source:** ATEM Mini Installation and Operation Manual, June 2026 (sections 'Setting the HDMI Output using the Video Out Buttons' and 'Setting the HDMI Output Source') — <https://documents.blackmagicdesign.com/UserManuals/ATEM_Mini_Manual.pdf>
- **Evidence:** "The default output source for ATEM Mini Pro is the multiview ... ATEM Mini Extreme's default output is program for output 1 and multiview for output 2."

### F-26

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Streaming
- **Fact:** ATEM Mini Pro streams directly over RTMP and SRT, either via its Ethernet port (10/100/1000 BaseT) or a shared internet connection over USB-C. It records directly to USB-C media as .mp4 H.264 with AAC audio at the ATEM video standard. Supported media formats are ExFAT and HFS+.
- **Source:** Blackmagic ATEM Mini tech specs - ATEM Mini Pro — <https://www.blackmagicdesign.com/products/atemmini/techspecs/W-APS-14>
- **Evidence:** "Supports direct live streaming using Real Time Messaging Protocol (RTMP) and Secure Reliable Transport (SRT) over ethernet or a shared internet connection over USB-C." ... "1 x USB-C 3.1 Gen 1 expansion port for external media to direct record a .mp4 H.264 with AAC audio"

### F-27

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Linux
- **Fact:** The ATEM Mini USB-C port also acts as a webcam output: a computer connected to it recognises the ATEM Mini as a webcam. Blackmagic documents Mac and Windows (Teams, Zoom, OBS) use and makes no statement about Linux or UVC.
- **Source:** ATEM Mini Installation and Operation Manual, June 2026 (Getting Started / webcam) — <https://documents.blackmagicdesign.com/UserManuals/ATEM_Mini_Manual.pdf>
- **Evidence:** "Plug ATEM Mini's webcam output into your computer's USB input. Your computer will recognize ATEM Mini as a webcam"

### F-28

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Streaming
- **Fact:** ATEM Streaming Bridge receives an H.264 stream over Ethernet and outputs it on HDMI (1080p23.98-1080p60, 2-channel embedded audio) and 2x 3G-SDI. Its documented streaming input protocols are RTMP (from Web Presenter, ATEM Mini and ATEM SDI) and SRT (from Web Presenter). The bridge itself is configured with the ATEM Setup utility over USB, or with its mini switches. That utility exports an XML settings file, which is loaded on the remote ATEM Mini Pro/Extreme so the bridge appears in that switcher's streaming 'platform' menu. For internet links, the bridge needs TCP port 1935 forwarded to it.
- **Source:** Blackmagic ATEM Mini tech specs - ATEM Streaming Bridge; ATEM Mini manual June 2026 — <https://www.blackmagicdesign.com/products/atemmini/techspecs/W-APS-14>
- **Evidence:** "Streaming Input: ATEM Streaming Bridge supports video input streamed over ethernet from ATEM Mini and ATEM SDI models. Protocols: RTMP (Web Presenter, ATEM Mini, ATEM SDI) SRT (Web Presenter)"; "HDMI Video Standards 1080p23.98 ... 1080p60"
- **Original claim (before verification):** ATEM Streaming Bridge receives an H.264 stream over Ethernet and outputs it as HDMI (1080p23.98-1080p60) and 2x 3G-SDI. Its documented streaming input protocols are RTMP from Web Presenter, ATEM Mini and ATEM SDI, and SRT from Web Presenter. It is set up from the ATEM switcher's platform menu or via XML setup files.
- **Verifier note:** The tech specs confirm the I/O, the HDMI standards and the protocols. They also say 'Settings Control: Mini Switches or setup utility software when connected by USB.' The manual (pp.165-168) describes TCP port 1935 forwarding, and saving the XML file from 'streaming source settings' in ATEM Setup. Once loaded, 'The name you give the service will be the name listed in the platform menu in the remote ATEM Mini Pro'. The original wording, 'set up from the ATEM switcher's platform menu', conflated configuring the bridge with selecting it on the sending switcher. Whether third-party RTMP encoders such as FFmpeg are accepted by the bridge is not documented.

### F-29

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Streaming
- **Fact:** ATEM Mini Extreme ISO G2 accepts up to 8 streaming sources. These are RTMP/SRT video with audio from compatible Blackmagic Design cameras set to HyperDeck High/Medium/Low or Streaming High/Medium/Low quality. Receiving remote sources over the internet requires forwarding TCP port 1935 to the switcher.
- **Source:** Blackmagic ATEM Mini tech specs - ATEM Mini Extreme ISO G2; ATEM Mini manual June 2026 (Remote Sources) — <https://www.blackmagicdesign.com/products/atemmini/techspecs/W-APS-14>
- **Evidence:** "Total Streaming Sources Up to 8" ... "RTMP/SRT video with audio ... from compatible Blackmagic Design cameras outputting HyperDeck High, ..."; manual: "set up port forwarding on your Internet connection to 'TCP port 1935'"

### F-30

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** ATEM, Streaming
- **Fact:** Extra streaming services and low-level encoder settings on ATEM are configured through an XML file. The SDK's IBMDSwitcherStreamRTMP::SetProfileXml takes a <profile> with <config resolution fps codec> elements holding <bitrate>, <audio-bitrate> and <keyframe-interval>. The SDK example shows keyframe-interval 2 and codec 'H264'.
- **Source:** ATEM Mini manual June 2026 (Streaming settings); ATEM SDK manual section 11.3.1.33 — <https://documents.blackmagicdesign.com/UserManuals/ATEM_Mini_Manual.pdf>
- **Verifier's best source:** <https://documents.blackmagicdesign.com/DeveloperManuals/ATEMSDKManual.pdf>
- **Evidence:** Manual: "there is an XML file that has additional settings that knowledgeable users could take advantage of to add other streaming services"; SDK: "<config resolution="720p" fps="60" codec="H264"> <bitrate>6000000</bitrate> <audio-bitrate>128000</audio-bitrate> <keyframe-interval>2</keyframe-interval>"

### F-31

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming
- **Fact:** In legacy (pre-2023) FLV/RTMP, the VideoTagHeader CodecID 7 = AVC, which is the only modern video codec, and the AudioTagHeader SoundFormat 10 = AAC (AACPacketType 0 = sequence header, 1 = raw). HEVC, AV1, VP9, VP8 and Opus need Enhanced RTMP (E-RTMP) FourCC signalling.
- **Source:** Veovera Enhanced RTMP v2 specification (document version v2-2026-01-31-r2), legacy tag header tables citing Adobe FLV spec v10.1 — <https://github.com/veovera/enhanced-rtmp/blob/main/docs/enhanced/enhanced-rtmp-v2.md>
- **Evidence:** "CodecID UB[4] ... 7 = AVC"; "SoundFormat UB[4] ... 10 = AAC"; README: "Introduction of codecs such as VP8, VP9, HEVC and AV1 ... FourCC Signaling"

### F-32

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, Linux
- **Fact:** In FFmpeg, RTMP URLs take the form rtmp://[username:password@]server[:port][/app][/instance][/playpath], with a default TCP port of 1935. The 'listen' option makes FFmpeg act as an RTMP server, and 'rtmp_enhanced_codecs' advertises E-RTMP FourCCs such as hvc1,av01,vp09. The documented publish example is 'ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream'.
- **Source:** FFmpeg Protocols Documentation - rtmp, librtmp — <https://ffmpeg.org/ffmpeg-protocols.html#rtmp>
- **Evidence:** "port: The number of the TCP port to use (by default is 1935)." ... "listen: Act as a server, listening for an incoming connection." ... "ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream"

### F-33

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, Linux, Buildroot
- **Fact:** GStreamer's rtmp2sink (plugin rtmp2, GStreamer Bad Plug-ins) accepts video/x-flv on its sink pad and supports RTMP and RTMPS. The documented pipeline is 'x264enc ! flvmux ! rtmp2sink location=rtmp://...'. rtmp2src only connects to an RTMP server as a client and cannot accept incoming publish connections. In Buildroot the plugin is enabled with BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2.
- **Source:** GStreamer docs rtmp2sink / rtmp2src; Buildroot package/gstreamer1/gst1-plugins-bad/Config.in — <https://gstreamer.freedesktop.org/documentation/rtmp2/rtmp2sink.html>
- **Evidence:** "The rtmp2sink element sends audio and video streams to an RTMP server." sink caps video/x-flv; rtmp2src: "receives input streams from an RTMP server"; Buildroot: "config BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2 bool \"rtmp2\" help RTMP sink/source (rtmp2sink, rtmp2src)"

### F-34

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, Linux, Buildroot
- **Fact:** GStreamer's flvmux (GStreamer Good Plug-ins; Buildroot BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV) needs H.264 video as video/x-h264,stream-format=avc and AAC audio as audio/mpeg,mpegversion={4,2},stream-format=raw, and outputs video/x-flv. Its 'streamable' property drops indexes and duration for live streaming. The separate eflvmux element adds E-RTMP v2 FourCC muxing for H.265 (hvc1) and AV1. eflvmux first appears in the GStreamer 1.28 branch: it is absent from 1.24 and 1.26, and Buildroot master ships GStreamer 1.24.13, so it is not available in a stock Buildroot build.
- **Source:** GStreamer docs flvmux, eflvmux; Buildroot gst1-plugins-good Config.in — <https://gstreamer.freedesktop.org/documentation/flv/flvmux.html>
- **Evidence:** flvmux sink caps "video/x-h264 stream-format: avc", "audio/mpeg: mpegversion: { (int)4, (int)2 } stream-format: raw"; eflvmux: "capable of ... signalling advanced codecs in FOURCC format as per Enhanced RTMP (V2) specification"
- **Original claim (before verification):** GStreamer's flvmux (GStreamer Good Plug-ins; Buildroot BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV) needs H.264 video as video/x-h264,stream-format=avc and AAC audio as audio/mpeg,mpegversion={4,2},stream-format=raw, and outputs video/x-flv. Its 'streamable' property drops indexes and duration for live streaming. The separate eflvmux element adds E-RTMP v2 FourCC muxing for H.265 (hvc1) and AV1.
- **Verifier note:** The flvmux docs pad templates match. eflvmux docs show video caps video/x-h265 stream-format hvc1 and video/x-av1 obu-stream. The GitLab tree API shows subprojects/gst-plugins-good/gst/flv contains gsteflvmux.c on refs 1.28 and main, but not on 1.24 or 1.26. Buildroot master (commit 8cfad4f0, 2026-10-06) has GST1_PLUGINS_GOOD_VERSION = 1.24.13. Its Config.in has BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV ('FLV muxing and demuxing plugin').

### F-35

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Streaming, Linux, Pi4, CM4
- **Fact:** GStreamer's V4L2 stateful encoder element v4l2h264enc outputs H.264 as stream-format=byte-stream, alignment=au. flvmux needs stream-format=avc (F-34), so an h264parse element (or equivalent conversion) is needed between v4l2h264enc and flvmux.
- **Source:** GStreamer source subprojects/gst-plugins-good/sys/v4l2/gstv4l2h264enc.c (main) + flvmux docs — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/main/subprojects/gst-plugins-good/sys/v4l2/gstv4l2h264enc.c>
- **Evidence:** Inputs: gstv4l2h264enc.c:44 GST_STATIC_CAPS("video/x-h264, stream-format=(string) byte-stream, alignment=(string) au") vs flvmux sink caps stream-format: avc (F-34) => caps conversion required

### F-36

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming
- **Fact:** RFC 7742 requires WebRTC browsers and video-capable non-browsers to implement VP8 and H.264 Constrained Baseline. H.264 endpoints MUST support Constrained Baseline Level 1.2 and SHOULD support Constrained High Level 1.3. packetization-mode 1 MUST be supported, profile-level-id MUST appear in SDP, and SPS/PPS MUST be sent in-band: sprop-parameter-sets MUST NOT be in SDP.
- **Source:** RFC 7742 WebRTC Video Processing and Codec Requirements, sections 5 and 6.2 — <https://www.rfc-editor.org/rfc/rfc7742.txt>
- **Evidence:** "WebRTC Browsers MUST implement the VP8 video codec ... and H.264 Constrained Baseline" ... "they MUST support Constrained Baseline Profile Level 1.2 and SHOULD support H.264 Constrained High Profile Level 1.3" ... "WebRTC implementations MUST NOT include this parameter [sprop-parameter-sets]"

### F-37

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming
- **Fact:** RFC 6184 defines profile-level-id as the hex encoding of three SPS bytes: profile_idc, profile-iop (constraint_set0..5 flags plus reserved_zero_2bits, MSB first) and level_idc. Constrained Baseline can be signalled as profile_idc 0x42 with profile-iop x1xx0000 (constraint_set1_flag=1). Per Table 5 it can equally be signalled as 0x4D with 1xxx0000 or 0x58 with 11xx0000. If level-asymmetry-allowed is absent it is inferred as 0, and both directions must then use the same level, namely the lower of the two default levels.
- **Source:** RFC 6184 RTP Payload Format for H.264 Video, section 8.1 and Table 5 — <https://www.rfc-editor.org/rfc/rfc6184.txt>
- **Evidence:** "A base16 ... representation of ... 1) profile_idc, 2) ... profile-iop ... 3) level_idc"; Table 5 "CB 42 (B) x1xx0000"; "When the parameter is not present, the value MUST be inferred to be equal to 0."
- **Original claim (before verification):** RFC 6184 defines profile-level-id as the hex encoding of three SPS bytes: profile_idc, profile-iop (constraint_set0..5 flags plus reserved_zero_2bits, MSB first) and level_idc. Constrained Baseline is profile_idc 0x42 with profile-iop x1xx0000 (constraint_set1_flag=1). If level-asymmetry-allowed is absent it is inferred as 0, and both directions must then use the same level.
- **Verifier note:** Section 8.1 of rfc6184.txt gives the definition. Table 5 lists 'CB 42 (B) x1xx0000 / same as: 4D (M) 1xxx0000 / same as: 58 (E) 11xx0000'. The original claim gave only the 0x42 form. level-asymmetry-allowed: 'When the parameter is not present, the value MUST be inferred to be equal to 0.' Section 8.2.2 adds that the common level is 'the lower value of the default level in the offer and the default level in the answer'.

### F-38

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Streaming
- **Fact:** profile-level-id 42e01f decodes as profile_idc 0x42 (Baseline), profile-iop 0xE0 (binary 11100000, so constraint_set1_flag=1, which is Constrained Baseline) and level_idc 0x1F = 31, i.e. Constrained Baseline Level 3.1.
- **Source:** Derived from RFC 6184 section 8.1 / Table 5 — <https://www.rfc-editor.org/rfc/rfc6184.txt>
- **Evidence:** Inputs: RFC 6184 byte order profile_idc|profile-iop|level_idc; 0xE0 = 1110 0000 matches CB pattern x1xx0000; 0x1F = 31 => Level 3.1

### F-39

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming
- **Fact:** libwebrtc, the WebRTC engine used by Chromium, defines kH264ProfileLevelConstrainedBaseline = "42e01f" and kH264ProfileLevelConstrainedHigh = "640c1f". It assumes Constrained Baseline Level 3.1 when profile-level-id is absent. Its built-in H.264 encoder encodes only Constrained Baseline, and it advertises Baseline, Constrained Baseline and Main formats at Level 3.1.
- **Source:** libwebrtc media/base/media_constants.h, api/video_codecs/h264_profile_level_id.cc, modules/video_coding/codecs/h264/h264.cc (refs/heads/main) — <https://webrtc.googlesource.com/src/+/refs/heads/main/media/base/media_constants.h>
- **Evidence:** kH264ProfileLevelConstrainedBaseline = "42e01f"; "static const H264ProfileLevelId kDefaultProfileLevelId(H264Profile::kProfileConstrainedBaseline, H264Level::kLevel3_1);"; "We only support encoding Constrained Baseline Profile (CBP)"

### F-40

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Streaming, TC358743
- **Fact:** Per H.264 Table A-1 as encoded in FFmpeg: Level 3.1 has MaxFS 3600 MBs and MaxMBPS 108000; Level 4 has MaxFS 8192 and MaxMBPS 245760; Level 4.2 has MaxFS 8704 and MaxMBPS 522240. A 1920x1080 frame is 120x68 = 8160 MBs, which exceeds Level 3.1's MaxFS. So 1080p30 (244800 MB/s) needs at least Level 4 (level_idc 0x28), and 1080p60 (489600 MB/s) needs Level 4.2 (0x2A). A strict 42e01f negotiation does not cover 1080p.
- **Source:** FFmpeg libavcodec/h264_levels.c ('H.264 table A-1') + calculation — <https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/h264_levels.c>
- **Evidence:** h264_levels.c: { "3.1", 31, 0, 108000, 3600, ...}, { "4", 40, 0, 245760, 8192, ...}, { "4.2", 42, 0, 522240, 8704, ...}; 1080 rows coded as 68 MB rows (1088/16); 8160*30=244800; 8160*60=489600

### F-41

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, ATEM
- **Fact:** RFC 7874 requires WebRTC endpoints to implement the Opus and G.711 PCMA/PCMU audio codecs. AAC is not a required WebRTC codec, so AAC audio such as ATEM's RTMP audio (F-06) has to be transcoded, typically to Opus, for browser WebRTC playback.
- **Source:** RFC 7874 WebRTC Audio Codec and Processing Requirements, section 3 — <https://www.rfc-editor.org/rfc/rfc7874.txt>
- **Evidence:** "WebRTC endpoints are REQUIRED to implement the following audio codecs: o Opus [RFC6716] ... o PCMA and PCMU (as specified in ITU-T Recommendation G.711 ...)"

### F-42

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, Linux, Buildroot
- **Fact:** GStreamer webrtcbin is plugin 'webrtc' in gst-plugins-bad and is declared with licence "LGPL". It implements most of the W3C RTCPeerConnection API, takes and produces application/x-rtp, and has no built-in signalling transport. It will not build without libgstwebrtcnice (libnice). In Buildroot, BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_WEBRTC selects LIBNICE plus the DTLS, SCTP and SRTP plugins and depends on !BR2_STATIC_LIBS. The Buildroot master gst1-plugins-bad version is 1.24.13.
- **Source:** GStreamer webrtc docs; gst-plugins-bad ext/webrtc/gstwebrtc.c and meson.build (main); Buildroot package/gstreamer1/gst1-plugins-bad/Config.in and .mk (master) — <https://gstreamer.freedesktop.org/documentation/webrtc/index.html>
- **Evidence:** gstwebrtc.c: GST_PLUGIN_DEFINE(..., webrtc, "WebRTC plugins", plugin_init, VERSION, "LGPL", ...); meson: "webrtc plugin requires libgstwebrtcnice."; Buildroot: "select BR2_PACKAGE_LIBNICE ... select BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_SRTP"; "GST1_PLUGINS_BAD_VERSION = 1.24.13"

### F-43

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Streaming, Linux, Buildroot
- **Fact:** GStreamer's webrtcsink (gst-plugins-rs crate gst-plugin-webrtc, plugin rswebrtc) is licensed MPL-2.0. It includes a simple signalling server, encodes internally (VP8, H.264, VP9, H.265, AV1; Opus audio) and offers congestion control (Google Congestion Control). Buildroot master has no gst1-plugins-rs package.
- **Source:** gst-plugins-rs net/webrtc/Cargo.toml (main); GStreamer webrtcsink docs; Buildroot tree check — <https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs/-/blob/main/net/webrtc/Cargo.toml>
- **Evidence:** Cargo.toml: name = "gst-plugin-webrtc", license = "MPL-2.0", description = "GStreamer plugin for high level WebRTC elements and a simple signaling server"; raw.githubusercontent.com/buildroot/buildroot/master/package/gst1-plugins-rs/Config.in -> HTTP 404

### F-44

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Streaming, Linux, Buildroot
- **Fact:** MediaMTX is an MIT-licensed (Copyright 2019 aler9), zero-dependency single-executable media server for Linux, Windows and macOS. It converts between protocols including RTMP, WebRTC, SRT, RTSP and HLS. Default ports in mediamtx.yml are rtmpAddress :1935, webrtcAddress :8889, webrtcLocalUDPAddress :8189, rtspAddress :8554, srtAddress :8890, hlsAddress :8888 and apiAddress :9997 (api: false). Buildroot master has no package/mediamtx.
- **Source:** bluenviron/mediamtx LICENSE, README.md, mediamtx.yml (main); Buildroot tree check — <https://github.com/bluenviron/mediamtx>
- **Evidence:** LICENSE "MIT License Copyright (c) 2019 aler9"; README "Streams are automatically converted from a protocol to another" ... "it's a single executable"; mediamtx.yml: "rtmpAddress: :1935", "webrtcAddress: :8889", "webrtcLocalUDPAddress: :8189"; buildroot master package/mediamtx/Config.in -> HTTP 404

### F-45

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** Streaming
- **Fact:** MediaMTX accepts RTMP publishers using AV1, VP9, H265 or H264 video (Enhanced RTMP) and Opus, FLAC, AAC, MP3, AC-3, G711 or LPCM audio. WebRTC readers get AV1, VP9, VP8, H265 or H264 video and only Opus, G722 or G711 audio, via a browser page at :8889/<path> or WHEP at /<path>/whep. MediaMTX states that browsers deliberately do not support H.264 B-frames in WebRTC, and recommends re-encoding to H.264 baseline plus Opus.
- **Source:** MediaMTX docs: RTMP clients, Read with WebRTC, WebRTC-specific features — <https://mediamtx.org/docs/features/webrtc-specific-features>
- **Evidence:** WebRTC read codecs: "Video: AV1, VP9, VP8, H265, H264 / Audio: Opus, G722, G711 (PCMA, PCMU)"; "H264, when the stream contains B-frames. These are not part of the WebRTC specification and support for them has been intentionally left out by every browser."

### F-46

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** ATEM, Streaming, Linux
- **Fact:** An ATEM streaming over RTMP acts as an RTMP client that publishes to a configured server URL and key (F-06, F-16). For PACSCORDER to receive an ATEM RTMP stream it must run a listening RTMP server, such as MediaMTX on :1935 or FFmpeg with '-listen 1'. GStreamer rtmp2src cannot do this because it is client-only.
- **Source:** Derived from ATEM SDK SetUrl/SetKey, FFmpeg rtmp 'listen', GStreamer rtmp2src docs, MediaMTX rtmpAddress — <https://gstreamer.freedesktop.org/documentation/rtmp2/rtmp2src.html>
- **Evidence:** Inputs: SDK "The SetUrl method sets the streaming server URL"; FFmpeg "listen: Act as a server"; rtmp2src "receives input streams from an RTMP server" (client); mediamtx.yml "rtmp: true / rtmpAddress: :1935"

---

## Topic G

**Raspberry Pi OS and official image tooling** — <a id="topic-g"></a>71 claims.

### G-01

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** The newest Raspberry Pi OS Lite (64-bit) image as of 2026-10-06 is 2026-10-06-raspios-trixie-arm64-lite.img.xz, based on Debian 13 'trixie'. https://downloads.raspberrypi.com/raspios_lite_arm64_latest returns HTTP 302 to it.
- **Source:** downloads.raspberrypi.com raspios_lite_arm64 image index + raspios_lite_arm64_latest redirect — <https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2026-10-06/>
- **Evidence:** The directory lists 2026-10-06-raspios-trixie-arm64-lite.img.xz plus .bmap, .sha256, .sig, .info and .sbom.xz. 'curl -I raspios_lite_arm64_latest' returns 'location: .../raspios_lite_arm64-2026-10-06/2026-10-06-raspios-trixie-arm64-lite.img.xz'. When fetched, the www.raspberrypi.com/software/operating-systems page still showed the 2026-09-15 release (kernel 6.18, Debian 13), probably because of page caching lag.

### G-02

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** 'Trixie — the new version of Raspberry Pi OS' was published on raspberrypi.com on 2025-10-02 (author Simon Long). The first trixie Lite arm64 image directory is raspios_lite_arm64-2025-10-02, but the image file inside it is 2025-10-01-raspios-trixie-arm64-lite.img.xz and the release-notes entry is dated 2025-10-01.
- **Source:** Trixie — the new version of Raspberry Pi OS — <https://www.raspberrypi.com/news/trixie-the-new-version-of-raspberry-pi-os/>
- **Evidence:** The news post is dated October 2, 2025. The image index contains raspios_lite_arm64-2025-10-02.

### G-03

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** Debian 13 'trixie' was first released on 2025-08-09. Point release 13.7 came out on 2026-09-12. Full support runs to 2028-08-09 and LTS to 2030-06-30.
- **Source:** Debian -- Debian 'trixie' Release Information — <https://www.debian.org/releases/trixie/>
- **Evidence:** Page text: 'Debian 13.0 was initially released on August 9th, 2025.' and 'Debian 13.7 was released on September 12th, 2026.' It also gives three years of standard support to August 9th, 2028 and LTS to June 30th, 2030.

### G-04

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The 2026-10-06 Raspberry Pi OS release notes list Raspberry Pi firmware 6f0881cba8bea8ec24956a5718714730e9936ec5 and Linux kernel 6.18.50 (cff533aec2fa601846766b32ff57204e0a61bed7). The 2026-09-15 entry lists the same kernel.
- **Source:** Raspberry Pi OS Lite arm64 release_notes.txt — <https://downloads.raspberrypi.com/raspios_lite_arm64/release_notes.txt>
- **Evidence:** The '2026-10-06:' entry contains '* Raspberry Pi firmware 6f0881cb...' and '* Linux kernel 6.18.50 - cff533ae...'. The 2026-09-15 entry also lists kernel 6.18.50.

### G-05

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** Raspberry Pi OS Trixie moved its default kernel from 6.12 to 6.18 partway through the release. The release-notes entries are 2026-04-13 for 6.12.75 and 2026-06-18 for 6.18.34. The re-spun 2026-04-21 image still had 6.12.75, so the switch landed between the 2026-04-21 and 2026-06-18 images.
- **Source:** Raspberry Pi OS Lite arm64 release_notes.txt — <https://downloads.raspberrypi.com/raspios_lite_arm64/release_notes.txt>
- **Evidence:** The 2026-04-13 entry says 'Linux kernel 6.12.75 - 89050b10...'. The 2026-06-18 entry says 'Linux kernel 6.18.34 - c8c74941...'. The 2025-12-04 entry says 6.12.47.

### G-06

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** In the 2026-10-06 Lite image the kernel ships as Debian packages. The meta-packages linux-image-rpi-v8 and linux-image-rpi-2712 (1:6.18.50-1+rpt1) pull in linux-image-6.18.50+rpt-rpi-v8 and linux-image-6.18.50+rpt-rpi-2712, and matching linux-headers are also installed. GPU firmware and bootloader files come from raspi-firmware 1:1.20260915-1 ('Raspberry Pi family GPU firmware and bootloaders'), and the EEPROM updater is rpi-eeprom 28.33-1.
- **Source:** 2026-10-06-raspios-trixie-arm64-lite.info (package manifest) — <https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2026-10-06/2026-10-06-raspios-trixie-arm64-lite.info>
- **Evidence:** Manifest lines: 'linux-image-rpi-2712 1:6.18.50-1+rpt1 ... Linux for Raspberry Pi 2712 (meta-package)', 'linux-image-rpi-v8 1:6.18.50-1+rpt1', 'raspi-firmware 1:1.20260915-1 ... Raspberry Pi family GPU firmware and bootloaders', 'rpi-eeprom 28.33-1'.

### G-07

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The archive.raspberrypi.com trixie main/binary-arm64 index (Release dated Tue, 06 Oct 2026 02:48 UTC) has no raspberrypi-kernel package. It carries linux-image-rpi-v8, linux-image-rpi-2712 and linux-image-rpi-v8-rt (all 1:6.18.50-1+rpt1), matching headers, and versioned images 6.12.25, 6.12.34, 6.12.47, 6.12.62, 6.12.75, 6.18.29, 6.18.33, 6.18.34, 6.18.39 and 6.18.50.
- **Source:** archive.raspberrypi.com debian trixie main binary-arm64 Packages.gz — <https://archive.raspberrypi.com/debian/dists/trixie/main/binary-arm64/Packages.gz>
- **Evidence:** Parsed the Packages index (Release dated Tue, 06 Oct 2026). Lookups returned raspberrypi-kernel None, linux-image-rpi-2712 ['1:6.18.50-1+rpt1'], linux-image-rpi-v8-rt ['1:6.18.50-1+rpt1'], linux-headers-rpi-2712 ['1:6.18.50-1+rpt1'], plus versioned images such as linux-image-6.12.75+rpt-rpi-2712 and linux-image-6.18.33+rpt-rpi-v8.

### G-08

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** Official documentation says to update with 'sudo apt update' and then 'sudo apt full-upgrade', 'because Raspberry Pi OS changes package dependencies more often that Debian'. Updating 'also updates your Linux kernel and firmware', and 'Normal firmware updates are included in APT updates (raspi-firmware package)'.
- **Source:** Raspberry Pi Documentation - Raspberry Pi OS — <https://www.raspberrypi.com/documentation/computers/os.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/os/updating.adoc>
- **Evidence:** Quotes: full-upgrade is recommended because 'Raspberry Pi OS changes package dependencies more often that Debian'. Updating 'also updates your Linux kernel and firmware'. 'Normal firmware updates are included in APT updates (raspi-firmware package).'

### G-09

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** rpi-update downloads the latest pre-release Linux kernel, its modules, device tree files and VideoCore firmware. On Raspberry Pi 4 and 5 it also updates the EEPROM bootloader. The documentation warns that pre-release firmware 'can introduce instability, rendering your system unreliable or unbootable', and says rpi-update is for developers and engineers doing testing, development or specific bug fixes.
- **Source:** Raspberry Pi Documentation - Raspberry Pi OS (rpi-update) — <https://www.raspberrypi.com/documentation/computers/os.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/os/updating.adoc>
- **Evidence:** Quotes: 'Pre-release firmware isn't guaranteed to work and can introduce instability, rendering your system unreliable or unbootable.' and 'rpi-update is intended for use by developers and engineers for testing, development, and specific bug fixes. In most cases, you don't need to use rpi-update.'

### G-10

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The 2026-10-06 Lite image installs rpi-update 20230904 by default.
- **Source:** 2026-10-06-raspios-trixie-arm64-lite.info — <https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2026-10-06/2026-10-06-raspios-trixie-arm64-lite.info>
- **Evidence:** The manifest contains 'rpi-update 20230904'.

### G-11

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** Raspberry Pi OS reads config.txt from the boot partition, which is mounted at /boot/firmware/. The current config.txt documentation does not mention the pre-Bookworm path. raspi-config (trixie branch) uses /boot/firmware/config.txt when that file exists and otherwise falls back to /boot/config.txt, the legacy location used before Bookworm.
- **Source:** Raspberry Pi Documentation - config.txt — <https://www.raspberrypi.com/documentation/computers/config_txt.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/config_txt/what_is_config_txt.adoc>
- **Evidence:** Quote: 'Raspberry Pi OS looks for this file in the boot partition, located at /boot/firmware/.' The page also says the path was /boot/config.txt before Bookworm. raspi-config (trixie branch) sets CONFIG=/boot/firmware/config.txt when that file exists.
- **Original claim (before verification):** Raspberry Pi OS reads config.txt from the boot partition at /boot/firmware/. Before Bookworm the path was /boot/config.txt.
- **Verifier note:** The doc quote on /boot/firmware/ is correct. The 'before Bookworm the path was /boot/config.txt' part is not in the cited page: grep for /boot/config.txt and Bookworm in the docs source found nothing. The only support I found is the raspi-config fallback logic (lines 11-16) and community comments on the Bookworm announcement.

### G-12

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4
- **Fact:** In rpi-6.18.y, the tc358743 overlay ('Toshiba TC358743 HDMI to CSI-2 bridge chip. Uses Unicam 1') is loaded with 'dtoverlay=tc358743,<param>=<val>'. Its parameters are 4lane (Compute Module CAM1 only), link-frequency (only 297000000 or 486000000; 486000000 is the default), media-controller (default off) and cam0. By default the overlay sets the csi1 compatible to 'brcm,bcm2835-unicam-legacy', which uses the video-node (non-MC) mode. The media-controller parameter turns that fragment off.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** README text: 'Name: tc358743 / Info: Toshiba TC358743 HDMI to CSI-2 bridge chip. Uses Unicam 1... Load: dtoverlay=tc358743,<param>=<val> / Params: 4lane Use 4 lanes (only applicable to Compute Modules CAM1 connector). link-frequency ... Only values of 297000000 (574Mbit/s) and 486000000 (972Mbit/s - default) are supported by the driver. media-controller Configure use of Media Controller API ... (default off) cam0 Adopt the default configuration for CAM0 on a Compute Module (CSI0, i2c_vc, and cam0_reg).'

### G-13

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 5, CM5
- **Fact:** The separate 'tc358743-pi5' overlay ('for Pi5', compatible brcm,bcm2712) takes the parameters 4lane, link-frequency (297000000 or 486000000) and cam0. It has no media-controller parameter.
- **Source:** raspberrypi/linux rpi-6.18.y overlays README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** README text: 'Name: tc358743-pi5 / Info: Toshiba TC358743 HDMI to CSI-2 bridge chip for Pi5... Load: dtoverlay=tc358743-pi5,<param>=<val> / Params: 4lane ... link-frequency ... cam0'.

### G-14

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** The 'tc358743-audio' overlay routes TC358743 audio to the Pi over I2S, wired LRCK/WFS to GPIO19, BCK/SCK to GPIO18 and DATA/SD to GPIO20. Its only parameter is card-name, which defaults to 'tc358743'.
- **Source:** raspberrypi/linux rpi-6.18.y overlays README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** README text: 'tc358743-audio Info: Used in combination with the tc358743-fast overlay to route the audio from the TC358743 over I2S to the Pi. Wiring is LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18, and DATA/SD to GPIO 20.'

### G-15

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The official camera documentation says that to use the listed camera-sensor overlays 'you must disable automatic camera detection', by setting camera_auto_detect=0 in /boot/firmware/config.txt.
- **Source:** Raspberry Pi Documentation - Camera software — <https://www.raspberrypi.com/documentation/computers/camera_software.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/rpicam_configuration.adoc>
- **Evidence:** Quote (via page fetch): 'To disable automatic detection, set camera_auto_detect=0 in /boot/firmware/config.txt.' The page also says that to use an overlay 'you must disable automatic camera detection.'

### G-16

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** In raspberrypi/linux's default branch (rpi-6.18.y), both arm64 bcm2711_defconfig and bcm2712_defconfig set CONFIG_VIDEO_TC358743=m. The 2026-10-06 image actually ships tc358743.ko.xz for both the rpi-v8 and rpi-2712 kernels.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/configs/bcm2711_defconfig and bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2712_defconfig>
- **Evidence:** The GitHub API reports default_branch rpi-6.18.y. grep found 'CONFIG_VIDEO_TC358743=m' at line 1077 of bcm2711_defconfig and line 1079 of bcm2712_defconfig.

### G-17

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** Both rpi-6.18.y arm64 defconfigs set CONFIG_VIDEO_BCM2835_UNICAM_LEGACY=m, CONFIG_VIDEO_BCM2835_UNICAM=m, CONFIG_VIDEO_RPI_HEVC_DEC=m, CONFIG_VIDEO_RASPBERRYPI_PISP_BE=m, CONFIG_VIDEO_RP1_CFE=m and CONFIG_VIDEO_RP1_CFE_DOWNSTREAM=m.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2711_defconfig / bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2711_defconfig>
- **Evidence:** bcm2711_defconfig lines 1034-1039 and bcm2712_defconfig lines 1036-1041 contain exactly these six =m entries.

### G-18

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** Both rpi-6.18.y arm64 defconfigs set CONFIG_BCM2835_VCHIQ=y, CONFIG_VIDEO_BCM2835=m, CONFIG_VIDEO_CODEC_BCM2835=m (bcm2835-codec) and CONFIG_VIDEO_ISP_BCM2835=m. Building the module into bcm2712 does not give Pi 5 a hardware H.264 encoder (see G-22).
- **Source:** raspberrypi/linux rpi-6.18.y bcm2711_defconfig / bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2711_defconfig>
- **Evidence:** bcm2711_defconfig lines 1553-1557 and bcm2712_defconfig lines 1555-1559 contain 'CONFIG_BCM2835_VCHIQ=y', 'CONFIG_VIDEO_BCM2835=m', 'CONFIG_VIDEO_CODEC_BCM2835=m' and 'CONFIG_VIDEO_ISP_BCM2835=m'.

### G-19

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** Both rpi-6.18.y arm64 defconfigs set CONFIG_DMABUF_HEAPS=y, CONFIG_DMABUF_HEAPS_SYSTEM=y, CONFIG_DMABUF_HEAPS_CMA=y, CONFIG_CMA=y, CONFIG_DMA_CMA=y, CONFIG_CMA_SIZE_MBYTES=5 and CONFIG_PREEMPT=y.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2711_defconfig / bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2712_defconfig>
- **Evidence:** grep output from both files includes CONFIG_PREEMPT=y (line 10), CONFIG_DMABUF_HEAPS_CMA=y, CONFIG_DMA_CMA=y and CONFIG_CMA_SIZE_MBYTES=5.

### G-20

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 5, CM5, Pi 4 Model B, CM4
- **Fact:** bcm2712_defconfig sets CONFIG_LOCALVERSION="-v8-16k" and CONFIG_ARM64_16K_PAGES=y, and bcm2711_defconfig sets CONFIG_LOCALVERSION="-v8". Raspberry Pi documents that kernel8.img (4K pages) also runs on BCM2712 devices, and the firmware falls back to it when kernel_2712.img is absent.
- **Source:** raspberrypi/linux rpi-6.18.y bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/configs/bcm2712_defconfig>
- **Evidence:** bcm2712_defconfig line 1: CONFIG_LOCALVERSION="-v8-16k"; line 50: CONFIG_ARM64_16K_PAGES=y. bcm2711_defconfig line 1: CONFIG_LOCALVERSION="-v8".

### G-21

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4
- **Fact:** In rpi-6.18.y the downstream Unicam driver (Kconfig VIDEO_BCM2835_UNICAM_LEGACY) builds as bcm2835-unicam-legacy.ko. It matches 'brcm,bcm2835-unicam' (.data=1, which forces Media Controller mode) and 'brcm,bcm2835-unicam-legacy' (.data=0, where mode follows the media_controller module parameter, default 0). It can be driven from the video node. The mainline driver matches only 'brcm,bcm2835-unicam-upstream' and supports only Media Controller.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/platform/bcm2835 (Kconfig, Makefile, bcm2835-unicam.c) and drivers/media/platform/broadcom/bcm2835-unicam.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/bcm2835/Kconfig>
- **Evidence:** Kconfig help: 'This is the downstream version of this driver that still supports being driven from the video node for simple devices. The mainline driver only supports using Media Controller.' Makefile: 'bcm2835-unicam-legacy-y := bcm2835-unicam.o'. Legacy driver: '{ .compatible = "brcm,bcm2835-unicam", .data = (void *)1 }, { .compatible = "brcm,bcm2835-unicam-legacy", .data = 0 }' and 'static int media_controller; module_param(media_controller...'. Mainline driver: '{ .compatible = "brcm,bcm2835-unicam-upstream", }'.

### G-22

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 5, CM5
- **Fact:** Raspberry Pi's camera documentation says 'Raspberry Pi 5 uses software video encoders', which have longer latency than 'the old hardware encoders'. The BCM2712 processor page says 'Other CODECs run in software' (only HEVC decode is in hardware) and gives 'H264 1080p30 encode (from ISP) ~30–40% CPU'.
- **Source:** Raspberry Pi Documentation - Camera software — <https://www.raspberrypi.com/documentation/computers/camera_software.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/rpicam_vid.adoc>
- **Evidence:** Quote: 'Raspberry Pi 5 uses software video encoders.' Raspberry Pi forum replies in search results say the same: 'There is no hardware H264 or H265 encoder on Pi5'.

### G-23

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Pi 4 Model B, CM4
- **Fact:** v4l2h264enc ('V4L2 H.264 Encoder') is in gst-plugins-good sys/v4l2. It is registered dynamically by gst_v4l2_probe_and_register(), which calls gst_v4l2_h264_enc_register for M2M devices whose caps pass gst_v4l2_is_h264_enc. The probe is compiled in only with GST_V4L2_ENABLE_PROBE, set by the meson option v4l2-probe (upstream default true).
- **Source:** GStreamer 1.26 gst-plugins-good sys/v4l2/gstv4l2h264enc.c — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/1.26/subprojects/gst-plugins-good/sys/v4l2/gstv4l2h264enc.c>
- **Evidence:** The source contains 'GST_DEBUG_CATEGORY_INIT (gst_v4l2_h264_enc_debug, "v4l2h264enc", 0, "V4L2 H.264 Encoder")', 'gst_v4l2_is_h264_enc (GstCaps * sink_caps, GstCaps * src_caps)' and 'gst_v4l2_h264_enc_register (GstPlugin * plugin, ...)'.

### G-24

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** v4l-utils in Debian trixie is 1.30.1-1 (arm64; forky has 1.32.0-5). It ships /usr/bin/v4l2-ctl, /usr/bin/media-ctl, /usr/bin/v4l2-compliance and /usr/bin/cec-ctl, and it is installed in the 2026-10-06 Lite image.
- **Source:** Debian madison + packages.debian.org trixie/arm64/v4l-utils filelist + Lite .info — <https://packages.debian.org/trixie/arm64/v4l-utils/filelist>
- **Evidence:** madison: 'v4l-utils | 1.30.1-1 | trixie | arm64' (forky has 1.32.0-5). The filelist includes /usr/bin/media-ctl, /usr/bin/v4l2-ctl and /usr/bin/cec-ctl. The Lite manifest has 'v4l-utils 1.30.1-1'.

### G-25

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** i2c-tools in Debian trixie is 4.4-2 (arm64). It ships /usr/sbin/i2cdetect, i2cget, i2cset, i2cdump and i2ctransfer, and it is not installed in the 2026-10-06 Lite image.
- **Source:** Debian madison + packages.debian.org trixie/arm64/i2c-tools filelist — <https://packages.debian.org/trixie/arm64/i2c-tools/filelist>
- **Evidence:** madison: 'i2c-tools | 4.4-2 | trixie | arm64'. The filelist includes /usr/sbin/i2cdetect, i2cget, i2cset and i2ctransfer. grep for 'i2c' in the Lite .info manifest found nothing.

### G-26

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** gstreamer1.0-plugins-good in Debian trixie and trixie-security is 1.26.2-1+deb13u2. It ships libgstvideo4linux2.so (v4l2src and the probed v4l2 M2M elements), plus libgstrtp, libgstrtpmanager, libgstisomp4, libgstmatroska and libgstflv. archive.raspberrypi.com trixie main does not override it.
- **Source:** Debian madison + packages.debian.org trixie/arm64/gstreamer1.0-plugins-good filelist + RPi archive index — <https://packages.debian.org/trixie/arm64/gstreamer1.0-plugins-good/filelist>
- **Evidence:** madison: 'gstreamer1.0-plugins-good | 1.26.2-1+deb13u2 | trixie | arm64'. The filelist includes /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstvideo4linux2.so, libgstrtp.so, libgstrtpmanager.so, libgstisomp4.so, libgstmatroska.so and libgstflv.so. The RPi archive index lookup for gstreamer1.0-plugins-good returned None.

### G-27

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** gstreamer1.0-plugins-bad in Debian trixie is 1.26.2-3+deb13u3. It ships libgstrtmp2, libgstwebrtc, libgstwebrtcdsp, libgstsrtp, libgstdtls, libgstsctp, libgstv4l2codecs and libgstkms. archive.raspberrypi.com trixie carries its own build, 1.26.2-3+rpt4+deb13u3, which sorts higher than Debian's.
- **Source:** Debian madison + packages.debian.org trixie/arm64/gstreamer1.0-plugins-bad filelist + RPi archive index — <https://packages.debian.org/trixie/arm64/gstreamer1.0-plugins-bad/filelist>
- **Evidence:** madison: 'gstreamer1.0-plugins-bad | 1.26.2-3+deb13u3 | trixie | arm64'. The filelist includes libgstrtmp2.so, libgstwebrtc.so, libgstwebrtcdsp.so, libgstsrtp.so, libgstdtls.so, libgstsctp.so, libgstv4l2codecs.so and libgstkms.so. The RPi archive Packages lists gstreamer1.0-plugins-bad ['1.26.2-3+rpt4+deb13u3'].

### G-28

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** gstreamer1.0-nice in Debian trixie is 0.1.22-1 and ships libgstnice.so. libnice10 is also 0.1.22-1. webrtcbin's ICE transport creates nicesrc and nicesink elements, so it needs this plugin at runtime.
- **Source:** Debian madison + packages.debian.org trixie/arm64/gstreamer1.0-nice filelist — <https://packages.debian.org/trixie/arm64/gstreamer1.0-nice/filelist>
- **Evidence:** madison: 'gstreamer1.0-nice | 0.1.22-1 | trixie | arm64' and 'libnice10 | 0.1.22-1 | trixie | arm64'. The filelist includes /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstnice.so.

### G-29

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** Debian trixie and trixie-security ship ffmpeg 7:7.1.5-0+deb13u1. archive.raspberrypi.com trixie ships ffmpeg and libavcodec61 8:7.1.5-0+deb13u1+rpt2, and its higher epoch makes it win on Raspberry Pi OS.
- **Source:** Debian madison ffmpeg + archive.raspberrypi.com trixie Packages.gz — <https://archive.raspberrypi.com/debian/dists/trixie/main/binary-arm64/Packages.gz>
- **Evidence:** madison: 'ffmpeg | 7:7.1.5-0+deb13u1 | trixie / trixie-security'. RPi index: ffmpeg ['8:7.1.5-0+deb13u1+rpt2'], libavcodec61 ['8:7.1.5-0+deb13u1+rpt2'].

### G-30

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** x264 in Debian trixie is 2:0.164.3108+git31e19f9-2+b1 (arm64). archive.raspberrypi.com trixie has no x264 or libx264-164 package.
- **Source:** Debian madison x264 — <https://qa.debian.org/madison.php?package=x264&table=debian&a=arm64&s=trixie>
- **Evidence:** madison: 'x264 | 2:0.164.3108+git31e19f9-2+b1 | trixie | arm64'. The RPi index lookups for x264 and libx264-164 returned None.

### G-31

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** GStreamer core in trixie is libgstreamer1.0-0 1.26.2-2. archive.raspberrypi.com trixie overrides gstreamer1.0-plugins-base (1.26.2-1+rpt3+deb13u2) and provides gstreamer1.0-libcamera (0.7.2+rpt20260817-1).
- **Source:** Debian madison + archive.raspberrypi.com trixie Packages.gz — <https://archive.raspberrypi.com/debian/dists/trixie/main/binary-arm64/Packages.gz>
- **Evidence:** madison: 'libgstreamer1.0-0 | 1.26.2-2 | trixie'. RPi index: gstreamer1.0-plugins-base ['1.26.2-1+rpt3+deb13u2'] and gstreamer1.0-libcamera ['0.7.2+rpt20260817-1'].

### G-32

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The 2026-10-06 Lite manifest lists 633 installed ('ii') packages. They include cloud-init 25.2-1~bpo13+1+rpt20, netplan.io 1.1.2-7+rpt1, network-manager 1.52.1-1+rpt4, openssh-server 1:10.0p1-7+deb13u4, systemd 257.13-1~deb13u1, libcamera0.7, rpicam-apps-core 1.13.0-1 and rpi-connect-lite 2.13.0. No GStreamer packages are installed.
- **Source:** 2026-10-06-raspios-trixie-arm64-lite.info — <https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2026-10-06/2026-10-06-raspios-trixie-arm64-lite.info>
- **Evidence:** 'grep -c ^ii' returned 633. The named packages appear in the manifest. grep for 'libgst' returned nothing.

### G-33

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** The 2026-10-06 Raspberry Pi OS Lite (64-bit) download is 550,466,056 bytes (.img.xz) and expands to 3,078,619,136 bytes. Imager lists it for pi5-64bit, pi4-64bit and pi3-64bit. These device tags do not name CM4 or CM5 separately, though the image includes CM4 and CM5 device trees.
- **Source:** Raspberry Pi Imager OS list (os_list_imagingutility_v4.json) — <https://downloads.raspberrypi.com/os_list_imagingutility_v4.json>
- **Evidence:** Entry: {'name': 'Raspberry Pi OS Lite (64-bit)', 'release_date': '2026-10-06', 'extract_size': 3078619136, 'image_download_size': 550466056, 'devices': ['pi5-64bit','pi4-64bit','pi3-64bit']}.

### G-34

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The 2026-10-06 Lite image is published with an SPDX-2.3 SBOM (.sbom.xz) generated by syft-1.54.0 (creator 'Organization: Anchore, Inc'). Line 2 of its .info reads 'Generated using pi-gen, https://github.com/RPi-Distro/pi-gen, b2cd98a9ee08a31ef7d0110ed09f2492bbd972ec, stage2'.
- **Source:** 2026-10-06 Lite .sbom.xz and .info — <https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2026-10-06/2026-10-06-raspios-trixie-arm64-lite.sbom.xz>
- **Evidence:** SBOM header: '"spdxVersion":"SPDX-2.3" ... "creators":["Organization: Anchore, Inc","Tool: syft-1.54.0"]'. The .info line 2 reads 'Generated using pi-gen, https://github.com/RPi-Distro/pi-gen, b2cd98a9ee08a31ef7d0110ed09f2492bbd972ec, stage2'.

### G-35

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** pi-gen's repository description is 'Tool used to create the official Raspberry Pi OS images'. 64-bit images should be built from its arm64 branch. Stages 0 to 5 run in order, and stage 2 'produces the Raspberry Pi OS Lite image'. Builds are configured by a 'config' file (IMG_NAME, RELEASE defaulting to trixie, DEPLOY_COMPRESSION and so on) and can run natively or via build-docker.sh.
- **Source:** RPi-Distro/pi-gen README — <https://github.com/RPi-Distro/pi-gen>
- **Evidence:** README (via fetch): 'Tool used to create the official Raspberry Pi OS images'. It notes 64-bit images build from the arm64 branch, Stage 2 is the 'Lite system', config variables include IMG_NAME, RELEASE and DEPLOY_COMPRESSION, and Docker builds are supported.

### G-36

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** rpi-image-gen was announced on raspberrypi.com on 2025-03-21 by Matt Lear. The post says it 'is designed to generate highly customised software images for Raspberry Pi devices, and offers a very granular level of control over file system construction and software image creation', and calls it 'an alternative to pi-gen'. Like pi-gen, it installs a Debian system from Raspberry Pi OS and Debian binary packages, and the post says it produces an SBOM for every build.
- **Source:** Introducing rpi-image-gen: build highly customised Raspberry Pi software images — <https://www.raspberrypi.com/news/introducing-rpi-image-gen-build-highly-customised-raspberry-pi-software-images/>
- **Evidence:** Post dated March 21, 2025, by Matt Lear. Quote: 'rpi-image-gen is designed to generate highly customised software images for Raspberry Pi devices, and offers a very granular level of control over file system construction and software image creation.' The post says rpi-image-gen produces an SBOM for every build.
- **Original claim (before verification):** rpi-image-gen was announced on raspberrypi.com on 2025-03-21 (author Matt Lear) as a tool for building highly customised Raspberry Pi software images. It is positioned as an alternative to pi-gen that assembles images from pre-built Debian packages instead of compiling.
- **Verifier note:** The date, author and quote are correct. The post does not contrast 'pre-built packages instead of compiling'. pi-gen also installs pre-built Debian packages, so that framing misrepresents the difference. The post says 'Similar to pi-gen, rpi-image-gen leverages ... installing a Debian Linux system'. The 'slim' example used in the comparison is in the post.

### G-37

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** github.com/raspberrypi/rpi-image-gen was created on 2024-12-09 and is BSD-3-Clause licensed (default branch master). Releases: v1.0.0 (2025-09-01), v2.0.0-rc.1 (2025-09-05; no final v2.0.0 release exists), v2.1.0 (2026-01-22), v2.2.0 (2026-02-05), v2.3.0 (2026-03-04), v2.4.0 (2026-03-09), v2.5.0 (2026-04-28), v2.6.0 (2026-05-22), v2.7.0 (2026-06-26) and v2.8.0 (2026-08-13, latest).
- **Source:** GitHub API: raspberrypi/rpi-image-gen repo + releases — <https://github.com/raspberrypi/rpi-image-gen/releases>
- **Evidence:** API output: created_at 2024-12-09T11:18:43Z, license bsd-3-clause, default_branch master, pushed_at 2026-10-02. Release tags and published_at values are as listed.

### G-38

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** rpi-image-gen v2 builds from declarative YAML configs and composable layers. Layers carry embedded X-Env metadata (variables, validation, dependencies, Provides and Requires) and use shell hooks. v2.0.0-rc.1 introduced the single YAML format, layer dependencies and automatic variable validation. v2.7.0 made X-Env-Layer-Version mandatory. v2.8.0 added a trait registry (for example hw:soc:bcm2712) with conditional layer Requires (when=has(...)) and dropped INI configs.
- **Source:** rpi-image-gen release notes v2.0.0-rc.1, v2.7.0, v2.8.0 — <https://github.com/raspberrypi/rpi-image-gen/releases>
- **Evidence:** v2.0.0-rc.1: 'Single format for all declarative files (YAML)', 'Layer dependencies', 'Automatic configuration variable validation'. v2.7.0: 'X-Env-Layer-Version is now mandatory'. v2.8.0: 'introduces traits - a new system for describing hardware, boot mechanism, and system facts (hw:*, boot:*, etc)'. README: Configuration, Layers and Hooks.

### G-39

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The rpi-image-gen README calls it a tool 'designed to create reproducible operating system artefacts' that can 'Generate Software Bill of Materials and CVE reports' and 'Integrate with rpi-sb-provisioner to automatically set up signed boot and encrypted filesystems'. It names native Debian Bookworm and Trixie arm64 as the only supported hosts. Containers and non-arm64 (QEMU) hosts are 'not formally supported'. It uses bdebstrap, mmdebstrap, genimage and podman unshare, and needs CAP_SYS_ADMIN.
- **Source:** raspberrypi/rpi-image-gen README.adoc — <https://github.com/raspberrypi/rpi-image-gen/blob/master/README.adoc>
- **Evidence:** Quotes: 'rpi-image-gen is a versatile image generation and build automation tool designed to create reproducible operating system artefacts', 'Generate Software Bill of Materials and CVE reports', 'Integrate with rpi-sb-provisioner to automatically set up signed boot and encrypted filesystems', 'developed on Raspberry Pi OS, with Debian Bookworm and Trixie arm64 as the supported native hosts'. It depends on bdebstrap, mmdebstrap, genimage and podman.

### G-40

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** rpi-image-gen's layer reference lists device layers rpi-cm4, rpi-cm5, rpi3, rpi4, rpi5 and rpizero2w, kernel layers rpi-linux-2712, rpi-linux-v8 and rpi-linux-v7, the sbom-base layer, the image-rpios layout and the image-rota layout ('Immutable GPT A/B layout for rotational OTA updates, boot/system redundancy, and a shared persistent data partition').
- **Source:** rpi-image-gen documentation - layer reference — <https://raspberrypi.github.io/rpi-image-gen/layer_reference.html>
- **Evidence:** The page lists device layers rpi-cm4, rpi-cm5, rpi3, rpi4, rpi5 and rpizero2w, kernel layers rpi-linux-2712, rpi-linux-v7, rpi-linux-v7l and rpi-linux-v8, and sbom-base. image-rota is described as 'Immutable GPT A/B layout for rotational OTA updates, boot/system redundancy, and a shared…'.

### G-41

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** image-rota provides immutable A/B system slots with a single persistent partition. Since v2.3.0 (2026-03-04), slots default to EROFS with zstd compression, tuned to the kernel page size (16K on Pi 5/CM5, 4K otherwise; ext4 still selectable). v2.4.0 added dm-verity hash generation. v2.5.0 declared LUKS v2 encryption production-ready for image-rpios and image-rota. v2.6.0 added the slot-shared framework for paths that persist across slot rotation.
- **Source:** rpi-image-gen release notes v2.3.0–v2.6.0 + image-rota doc — <https://github.com/raspberrypi/rpi-image-gen/releases>
- **Evidence:** v2.3.0: 'AB system slots now default to EROFS with zstd compression, automatically tuned to the target kernel's page size (16K on Pi 5/CM5, 4K otherwise)'. v2.4.0: 'image-rota: dm-verity hash generation'. v2.5.0: 'LUKS v2 disk encryption is now production-ready for both image-rpios and image-rota layers'. v2.6.0: 'slot-shared framework: layers declare filesystem paths to persist across slot rotations'. image-rota doc: 'Reliable rollbacks: slot can be flipped if a new root fails health checks'.

### G-42

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, Pi 5
- **Fact:** rpi-image-gen v2.2.0 (2026-02-05) added support for building device images for Raspberry Pi Connect Remote Update, with an example in examples/ota. The workflow is to build a Connect-enabled image, boot it so it auto-registers, build an update, host it over HTTP and deploy it from Connect.
- **Source:** rpi-image-gen v2.2.0 release notes — <https://github.com/raspberrypi/rpi-image-gen/releases/tag/v2.2.0>
- **Evidence:** Quote: 'This release adds support for building device images suitable for field deployment with Raspberry Pi Connect Remote Update... roll out updates over-the-air (OTA) using images built with rpi-image-gen.' The workflow is to build a Connect-enabled image (examples/ota), host the update image over HTTP, and deploy it from Connect.

### G-43

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, Pi 5
- **Fact:** Raspberry Pi Connect Remote Update was announced as a beta in a Raspberry Pi engineer's forum post on 2026-01-29. That post required a Pi 4 or 5 on Ethernet with an SD card of at least 16 GB, and allowed deployment only to online devices. The documentation as of 2026-10-05 is broader. A/B boot updates (image and update.tar.zst built with rpi-image-gen) need a Raspberry Pi 4 or later that is connected to the internet, opted in and signed in to Connect, plus a storage device (typically microSD) of at least 16 GB. Personal-account deployments still need the device online, but Connect for Organisations devices are updated the next time they sign in. The current docs do not call the feature beta, and Connect also supports script artefacts built with otamaker.
- **Source:** Raspberry Pi Forums: Raspberry Pi Connect: Remote Update — <https://forums.raspberrypi.com/viewtopic.php?t=395798>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/services/connect/ab-boot-update.adoc>
- **Evidence:** Fetched summary: posted January 29, 2026, labelled beta. It describes an A/B partition scheme that downloads update.tar.zst, verifies SHA256 and switches slots. Requirements are 'Raspberry Pi 4 or 5', Ethernet and SD ≥16 GB. Known limitation: deployments only work when the device is online. The thread is linked from the rpi-image-gen v2.2.0 notes.
- **Original claim (before verification):** Raspberry Pi Connect Remote Update was announced on 2026-01-29 as a beta. It performs image-based A/B OTA updates using images built by rpi-image-gen, and requires a Raspberry Pi 4 or 5 on Ethernet with an SD card of at least 16 GB. Deployments only work while the device is online.
- **Verifier note:** The forum post (fetched through WebFetch) matches the original claim, but it is outdated. The current docs (ab-boot-update.adoc lines 1-45 and remote-update.adoc lines 1-27) replace 'Ethernet' with 'connected to the internet' and 'SD card' with 'storage device (typically, microSD)', and add offline deployment for Organisations. Whether Compute Modules are supported is not stated: the docs say 'Raspberry Pi 4 or later', and Imager only offers Pi 4 or Pi 5 as device types.

### G-44

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** Minor rpi-image-gen releases include breaking changes. v2.4.0 disabled passwordless sudo by default (set IGconf_device_user1sudo=nopasswd to restore it). v2.7.0 renamed rpi-user-credentials to device-user-admin and stopped hardcoding user1's supplementary groups (IGconf_device_user1groups). v2.8.0 removed INI config support. v2.3.0 changed the A/B default rootfs to EROFS.
- **Source:** rpi-image-gen release notes v2.4.0, v2.7.0, v2.8.0 — <https://github.com/raspberrypi/rpi-image-gen/releases>
- **Evidence:** v2.4.0 'Breakages - rpi-user-credentials - sudo access is now disabled by default'. v2.7.0 'Breakages - rpi-user-credentials has been renamed to device-user-admin'. v2.8.0 'Dropped INI config support'.

### G-45

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5, Pi 4 Model B, Pi 5
- **Fact:** rpiboot (usbboot) 'provides a file server for loading software into memory on a Raspberry Pi for provisioning'. By default it boots the device so it appears as USB mass storage, using the Linux mass-storage-gadget on Zero 2 W, 3A+, CM3/3+/3E, Pi 4B, CM4/4S, Pi 400, Pi 5, Pi 500/500+ and CM5. Pi 4B (and 400/500) must first have rpiboot enabled, and on Pi 4B that means permanently programming an OTP GPIO. The secure-boot-recovery and secure-boot-recovery5 extensions do Pi 4 and Pi 5 secure-boot bootloader flashing and OTP provisioning.
- **Source:** raspberrypi/usbboot README — <https://github.com/raspberrypi/usbboot>
- **Evidence:** README: 'a file server for loading software into memory on a Raspberry Pi for provisioning'. The mass-storage-gadget device list includes 4B, 5, 400 and CM3/3+/3E/4/4S/5. secure-boot-recovery handles 'Pi4 secure-boot bootloader and OTP provisioning' and secure-boot-recovery5 the Pi 5 equivalent, using CUSTOMER_KEY_HASH.

### G-46

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5, Pi 4 Model B, Pi 5
- **Fact:** rpi-sb-provisioner is 'A minimal-input automatic secure boot provisioning system for Raspberry Pi devices'. It supports Pi 5, Pi 4, CM5, CM4 and Zero 2 W, and has three modes: secure-boot, fde-only and naked. It installs with 'sudo apt install -y rpi-sb-provisioner' (2.3.4 in the trixie archive, with rpiboot 20261005~110850). The host must be a Raspberry Pi 5 or other 64-bit Pi on Bookworm or newer, with at least 32 GB free and an official 27W supply.
- **Source:** raspberrypi/rpi-sb-provisioner README + archive.raspberrypi.com trixie index — <https://github.com/raspberrypi/rpi-sb-provisioner>
- **Evidence:** README: 'A minimal-input automatic secure boot provisioning system for Raspberry Pi devices'. It lists Pi 5, Pi 4, CM5, CM4 and Zero 2 W, the modes secure-boot, fde-only and naked, and 'sudo apt install -y rpi-sb-provisioner'. The RPi archive index lists rpi-sb-provisioner 2.3.4 and rpiboot 20261005~110850.

### G-47

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5
- **Fact:** The Compute Module documentation flashes eMMC by fitting nRPI_BOOT (J2), running 'sudo apt install rpiboot' and 'sudo rpiboot', and then writing with Imager or 'sudo dd ... of=/dev/sdX'. Its TIP says to use the Raspberry Pi Secure Boot Provisioner to flash the same image to multiple Compute Modules, and pi-gen (not rpi-image-gen) to customise the OS image.
- **Source:** Raspberry Pi Documentation - Compute Module hardware — <https://www.raspberrypi.com/documentation/computers/compute-module.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/compute-module/cm-emmc-flashing.adoc>
- **Evidence:** Quote (via fetch): 'To flash the same image to multiple Compute Modules, use the Raspberry Pi Secure Boot Provisioner'. It also gives the steps sudo apt install rpiboot, sudo rpiboot and dd to /dev/sdX.

### G-48

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** On Trixie, raspi-config's 'P2 Overlay File System' ('Enable/disable read-only file system') refuses when MemTotal is 262144 kB or less ('At least 512MB of RAM is recommended'). Otherwise it apt-installs overlayroot if missing and prepends 'overlayroot=tmpfs ' to cmdline.txt. A separate option makes the boot partition read-only by adding ',ro' to its fstab entry.
- **Source:** RPi-Distro/raspi-config (trixie branch) raspi-config script — <https://github.com/RPi-Distro/raspi-config/blob/trixie/raspi-config>
- **Evidence:** Menu: '"P2 Overlay File System" "Enable/disable read-only file system"'. enable_overlayfs(): checks MemTotal -le 262144 and reports 'At least 512MB of RAM is recommended for overlay filesystem'. It runs 'is_installed overlayroot || apt-get install -y overlayroot' and 'sed -i $CMDLINE -e "s/^/overlayroot=tmpfs /"'. enable_bootro edits /etc/fstab to add ',ro'.

### G-49

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** all
- **Fact:** Debian trixie has overlayroot 0.18.debian14, unattended-upgrades 2.12, rauc 1.13-3+deb13u1, swupdate 2024.12.1+dfsg-3+deb13u2 (2026.05.1+dfsg-1~bpo13+1 in trixie-backports) and mender-client 3.4.0+ds1-5+b15.
- **Source:** Debian madison (trixie) — <https://qa.debian.org/madison.php?package=rauc&table=debian&s=trixie>
- **Evidence:** madison output: 'overlayroot | 0.18.debian14 | trixie | all', 'unattended-upgrades | 2.12 | trixie', 'rauc | 1.13-3+deb13u1 | trixie | arm64', 'swupdate | 2024.12.1+dfsg-3+deb13u2 | trixie', 'swupdate | 2026.05.1+dfsg-1~bpo13+1 | trixie-backports', 'mender-client | 3.4.0+ds1-5+b15 | trixie'.

### G-50

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** autoboot.txt is an optional file in the first boot partition that sets boot_partition. It accepts the [all], [none] and [tryboot] filters, and supports tryboot_a_b=1, which loads the normal config.txt and boot.img instead of tryboot.txt and tryboot.img. Combined with tryboot, it is the firmware mechanism for fail-safe A/B OS updates. The docs note it only picks the boot partition, and that a production A/B system also needs redundant partitions and update tooling (they point to Connect A/B and rpi-image-gen).
- **Source:** Raspberry Pi Documentation - config.txt (autoboot.txt) — <https://www.raspberrypi.com/documentation/computers/config_txt.html#autoboot-txt>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/config_txt/autoboot.adoc>
- **Evidence:** Quote: 'autoboot.txt is an optional configuration file placed in the initial boot partition that can be used to specify the boot_partition number.' The file 'supports the [all], [none], and [tryboot] conditional filters'. The [tryboot] filter 'is intended for use in autoboot.txt to select a different boot_partition in tryboot mode for fail-safe OS updates.' The fetched summary listed support on Pi 4, 400, CM4/CM4S and Pi 5; the exact device list was not quoted verbatim.

### G-51

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** tryboot is set by adding 'tryboot' after the partition number in the reboot command, for example sudo reboot '0 tryboot'. It is documented under 'Fail-safe OS updates (tryboot)'. The one-shot flag loads tryboot.txt, or with tryboot_a_b the alternate partition's config.txt. Because the flag is cleared before the firmware starts, a crash or reset reverts to the original configuration. All models support tryboot, but Pi 4B rev 1.0/1.1 need an EEPROM that is not write-protected. With secure boot enabled, tryboot mode loads tryboot.img instead of boot.img.
- **Source:** Raspberry Pi Documentation - Raspberry Pi hardware (EEPROM boot flow) — <https://www.raspberrypi.com/documentation/computers/raspberry-pi.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/raspberry-pi/bootflow-eeprom.adoc>
- **Evidence:** Quotes: 'To set the tryboot flag, add tryboot after the partition number in the reboot command.' 'This is useful for unattended or remote systems to ensure recovery from failed boots.' 'If secure-boot is enabled, then tryboot mode will cause tryboot.img to be loaded instead of boot.img.'
- **Original claim (before verification):** tryboot is triggered by adding 'tryboot' after the partition number in the reboot command. Raspberry Pi documents it as 'Fail-safe OS updates (tryboot)' for unattended or remote systems. When secure boot is enabled, tryboot mode loads tryboot.img instead of boot.img.
- **Verifier note:** The quote 'This is useful for unattended or remote systems to ensure recovery from failed boots' is in eeprom-bootloader.adoc line 162. It describes BOOT_WATCHDOG_TIMEOUT, not tryboot, so the claim attaches it to the wrong feature. BOOT_WATCHDOG_TIMEOUT is itself relevant to the appliance.

### G-52

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 5, CM5
- **Fact:** A/B updates of the bootloader EEPROM itself are 'compatible only with the Raspberry Pi 5, Raspberry Pi Compute Module 5, and Raspberry Pi 5 keyboard computer devices'. They can be enabled with raspi-config, and while they are on, direct EEPROM writers such as flashrom no longer work.
- **Source:** Raspberry Pi Documentation - Raspberry Pi hardware (boot EEPROM) — <https://www.raspberrypi.com/documentation/computers/raspberry-pi.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/raspberry-pi/boot-eeprom.adoc>
- **Evidence:** Fetched text says A/B bootloader updates are 'compatible only with the Raspberry Pi 5, Raspberry Pi Compute Module 5, and Raspberry Pi 5 keyboard computer devices'. The section is headed 'A/B firmware updates with raspi-config'.

### G-53

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, Pi 5
- **Fact:** The Raspberry Pi trixie arm64 archive has rpi-connect 2.13.0, rpi-connect-lite 2.13.0 and rpi-connect-ota 1.3.16. rpi-connect-lite 2.13.0 is preinstalled in the 2026-10-06 Lite image.
- **Source:** archive.raspberrypi.com trixie Packages.gz — <https://archive.raspberrypi.com/debian/dists/trixie/main/binary-arm64/Packages.gz>
- **Evidence:** Parsed index: 'rpi-connect 2.13.0', 'rpi-connect-lite 2.13.0', 'rpi-connect-ota 1.3.16'.

### G-54

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** Several documented settings affect boot time. initial_turbo enables turbo from boot for up to 60 seconds or until cpufreq sets a frequency, and the November 2024 firmware already changed its default from 0 to 60 'to reduce boot time', so on current firmware it is on by default rather than a tuning lever. disable_splash=1 hides the rainbow splash (default 0). Network-install keyboard detection adds about 1 second to boot. NET_INSTALL_ENABLED defaults to 1 on flagship boards since Pi 4B and to 0 on Compute Modules since CM4. BOOT_WATCHDOG_TIMEOUT can reset a unit whose OS never starts.
- **Source:** Raspberry Pi Documentation - config.txt; Raspberry Pi hardware (bootloader configuration) — <https://www.raspberrypi.com/documentation/computers/config_txt.html>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/config_txt/overclocking.adoc>
- **Evidence:** Quotes: initial_turbo 'Enables turbo mode from boot for the given value in seconds, or until cpufreq sets a frequency. The maximum value is 60.' disable_splash: 'the rainbow splash screen will not be shown on boot'. Bootloader docs: 'network install must initialise the USB controller and enumerate devices. This increases boot time by approximately 1 second'.
- **Original claim (before verification):** Some documented config.txt and bootloader settings affect boot time. initial_turbo runs the CPU in turbo from boot for up to 60 seconds or until cpufreq takes over. disable_splash=1 hides the rainbow splash. Network install's keyboard detection adds about 1 second to boot.
- **Verifier note:** overclocking.adoc lines 62-66 give the default change. A later table (line 290) still lists the default as 0, so the docs are internally inconsistent. The other quotes are in boot.adoc line 94 and eeprom-bootloader.adoc lines 495-503.

### G-55

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The Trixie announcement says: 'As with all major version upgrades, we do not recommend or support attempting to upgrade a running Bookworm image.' The documentation recommends a clean install. The promise of continued Bookworm kernels is not in the article body. It comes from a comment by Raspberry Pi staff member Gordon Hollingworth on 2025-10-02: 'We will release new Linux kernels for the legacy OS for critical vulnerabilities. But otherwise there won't be any updates to Raspberry Pi specific packages.' Legacy Bookworm images were still being refreshed on 2026-10-06 with kernel 6.12.109.
- **Source:** Trixie — the new version of Raspberry Pi OS — <https://www.raspberrypi.com/news/trixie-the-new-version-of-raspberry-pi-os/>
- **Evidence:** Quote: 'As with all major version upgrades, we do not recommend or support attempting to upgrade a running Bookworm image.' The OS documentation also says to install a fresh image instead of upgrading in place.
- **Original claim (before verification):** Raspberry Pi does not recommend or support upgrading a running Bookworm image to Trixie in place. It will keep releasing Bookworm kernels to fix critical vulnerabilities.
- **Verifier note:** The source for the kernel commitment is a staff blog comment, not formal policy. updating.adoc line 155 says 'we strongly recommend that you use a clean install'. The oldstable release_notes show 6.12.109 on 2026-09-15 and 2026-10-06.

### G-56

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The Trixie announcement says: 'it's an odd-numbered year, which means there is a new major release of Debian Linux, which in turn means there is a new major release of Raspberry Pi OS.'
- **Source:** Trixie — the new version of Raspberry Pi OS — <https://www.raspberrypi.com/news/trixie-the-new-version-of-raspberry-pi-os/>
- **Evidence:** Quote: 'it's an odd-numbered year, which means there is a new major release of Debian Linux, which in turn means there is a new major release of Raspberry Pi OS.'

### G-57

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** The 2026-04-13 Raspberry Pi OS release notes say 'Passwordless sudo now disabled by default' and 'Switch to enable passwordless sudo added to Control Centre System tab and to raspi-config'.
- **Source:** Raspberry Pi OS Lite arm64 release_notes.txt — <https://downloads.raspberrypi.com/raspios_lite_arm64/release_notes.txt>
- **Evidence:** 2026-04-13 entry: '* Passwordless sudo now disabled by default' and '* Switch to enable passwordless sudo added to Control Centre System tab and to raspi-config'.

### G-58

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** Buildroot status on 2026-10-06: stable 2026.08 (released 2026-09-04, series EOL December 2026); old stable 2026.05.3 (2026-09-10, series EOL September 2026, so already end of life); long-term support 2025.02.x (latest 2025.02.18 on 2026-09-10, EOL March 2028).
- **Source:** Buildroot - Download — <https://buildroot.org/download.html>
- **Evidence:** Download page rows: 'Stable 2026.08.x December 2026 2026.08 2026-09-04', 'Old stable 2026.05.x September 2026 2026.05.3 2026-09-10', 'Long-term support 2025.02.x March 2028 2025.02.18 2026-09-10'.

### G-59

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** Buildroot 2026.08's raspberrypi4_64, raspberrypicm4io_64, raspberrypi5 and raspberrypicm5io defconfigs pin raspberrypi/linux commit 21b410140c47ffab5668399f6f143c7d7b935c8b (Linux 6.12.61), with defconfig bcm2711 (Pi 4/CM4) or bcm2712 (Pi 5/CM5). The 2026-10-06 Raspberry Pi OS ships 6.18.50.
- **Source:** Buildroot 2026.08 configs/raspberrypi5_defconfig (and siblings) + raspberrypi/linux Makefile at pinned commit — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypi5_defconfig>
- **Evidence:** BR2_LINUX_KERNEL_CUSTOM_TARBALL_LOCATION="$(call github,raspberrypi,linux,21b410140c47...)". The Makefile at that commit reads 'VERSION = 6 / PATCHLEVEL = 12 / SUBLEVEL = 61'. BR2_LINUX_KERNEL_DEFCONFIG is "bcm2712" (Pi 5/CM5) or "bcm2711" (Pi 4/CM4).

### G-60

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Pi 5, CM5
- **Fact:** Buildroot 2026.08's raspberrypi5_defconfig and raspberrypicm5io_defconfig apply board/raspberrypi/linux-4k-page-size.fragment (CONFIG_ARM64_4K_PAGES=y), so their Pi 5/CM5 kernels use 4K pages. Raspberry Pi OS's default bcm2712 kernel uses 16K pages, although Raspberry Pi documents that the 4K kernel8.img also runs on BCM2712.
- **Source:** Buildroot 2026.08 raspberrypi5_defconfig + board/raspberrypi/linux-4k-page-size.fragment — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/board/raspberrypi/linux-4k-page-size.fragment>
- **Evidence:** BR2_LINUX_KERNEL_CONFIG_FRAGMENT_FILES="board/raspberrypi/linux-4k-page-size.fragment". The fragment contains CONFIG_ARM64_4K_PAGES=y.

### G-61

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Pi 5, CM5
- **Fact:** Buildroot 2026.08's raspberrypi5_defconfig explicitly unsets BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS, so no DT overlays (including tc358743-pi5) are installed by default. raspberrypicm5io_defconfig sets it to y, and the Config.in default is y, so the Pi 4 defconfigs install them too. The overlays are copied from the pinned raspberrypi/firmware tree (boot/overlays), not built from the pinned kernel.
- **Source:** Buildroot 2026.08 configs/raspberrypi5_defconfig, raspberrypicm5io_defconfig, package/rpi-firmware/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypi5_defconfig>
- **Evidence:** raspberrypi5_defconfig: '# BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS is not set'. raspberrypicm5io_defconfig: 'BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS=y'. Config.in: 'config BR2_PACKAGE_RPI_FIRMWARE_INSTALL_DTB_OVERLAYS bool "Install DTB overlays" default y'.

### G-62

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** All four Buildroot 2026.08 Raspberry Pi defconfigs build a 120M ext4 rootfs with the external Bootlin aarch64 glibc stable toolchain and BR2_DOWNLOAD_FORCE_CHECK_HASHES=y. The cm4io_64 and cm5io defconfigs also build the host rpiboot (BR2_PACKAGE_HOST_RASPBERRYPI_USBBOOT=y).
- **Source:** Buildroot 2026.08 Raspberry Pi defconfigs — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypicm4io_64_defconfig>
- **Evidence:** All four contain 'BR2_TARGET_ROOTFS_EXT2_SIZE="120M"' and 'BR2_TOOLCHAIN_EXTERNAL_BOOTLIN_AARCH64_GLIBC_STABLE=y'. cm4io_64 and cm5io contain 'BR2_PACKAGE_HOST_RASPBERRYPI_USBBOOT=y'.

### G-63

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** At raspberrypi/linux commit 21b410140c47 (6.12.61), both arm64 bcm2711_defconfig and bcm2712_defconfig contain CONFIG_VIDEO_TC358743=m, CONFIG_VIDEO_BCM2835_UNICAM=m (and _LEGACY=m), CONFIG_VIDEO_RP1_CFE=m and CONFIG_VIDEO_CODEC_BCM2835=m. bcm2712 also sets CONFIG_ARM64_16K_PAGES=y, which Buildroot overrides to 4K.
- **Source:** raspberrypi/linux @21b410140c47 arch/arm64/configs/bcm2711_defconfig, bcm2712_defconfig — <https://github.com/raspberrypi/linux/blob/21b410140c47ffab5668399f6f143c7d7b935c8b/arch/arm64/configs/bcm2712_defconfig>
- **Evidence:** grep at that commit returned CONFIG_VIDEO_BCM2835_UNICAM=m, CONFIG_VIDEO_RP1_CFE=m, CONFIG_VIDEO_TC358743=m and CONFIG_VIDEO_CODEC_BCM2835=m in both files, plus CONFIG_ARM64_16K_PAGES=y in bcm2712.

### G-64

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** Buildroot 2026.08 package versions: gstreamer1, gst1-plugins-good and gst1-plugins-bad 1.24.13; libnice 0.1.21; ffmpeg 6.1.5; libv4l 1.32.0; i2c-tools 4.4; x264 at git baee400fa9ced6f5481a728138fed6e867b0ff7f; rauc 1.15.2; swupdate 2026.05.1; mender 3.5.3.
- **Source:** Buildroot 2026.08 package/*.mk — <https://gitlab.com/buildroot.org/buildroot/-/tree/2026.08/package>
- **Evidence:** From the .mk files: GST1_PLUGINS_GOOD_VERSION = 1.24.13, GST1_PLUGINS_BAD_VERSION = 1.24.13, LIBNICE_VERSION = 0.1.21, FFMPEG_VERSION = 6.1.5, LIBV4L_VERSION = 1.32.0, I2C_TOOLS_VERSION = 4.4, X264_VERSION = baee400fa9ced6f5481a728138fed6e867b0ff7f, RAUC_VERSION = 1.15.2, SWUPDATE_VERSION = 2026.05.1, MENDER_VERSION = 3.5.3.

### G-65

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** Buildroot 2026.08 can build the GStreamer plugins this pipeline needs. gst1-plugins-bad has RTMP2, WEBRTC, DTLS, SRTP, SCTP and V4L2CODECS options, and gst1-plugins-good has V4L2 and V4L2_PROBE ('v4l2-probe (m2m)'). V4L2_PROBE is not on by default, and v4l2h264enc needs it.
- **Source:** Buildroot 2026.08 package/gstreamer1/gst1-plugins-bad/Config.in and gst1-plugins-good/Config.in — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/gstreamer1/gst1-plugins-bad/Config.in>
- **Evidence:** Option names found: BR2_PACKAGE_GST1_PLUGINS_BAD_PLUGIN_RTMP2, ..._WEBRTC, ..._DTLS, ..._SRTP, ..._SCTP, ..._V4L2CODECS; BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2 and ..._V4L2_PROBE 'v4l2-probe (m2m)'.

### G-66

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** BR2_REPRODUCIBLE is labelled 'Make the build reproducible (experimental)'. It aims for identical binaries for a given configuration, including on different machines, but 'The current implementation is restricted to builds with the same output directory', and it is experimental because 'not all packages behave properly'.
- **Source:** Buildroot 2026.08 Config.in (BR2_REPRODUCIBLE) — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/Config.in>
- **Evidence:** 'config BR2_REPRODUCIBLE bool "Make the build reproducible (experimental)" ... The current implementation is restricted to builds with the same output directory... If you build with the same O=... path, however, the result is identical.'

### G-67

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** Buildroot manual §11.7 ('Why doesn't Buildroot generate binary packages (.deb, .ipkg…)?') recommends complete system upgrades of the whole root filesystem image at once, so that the image deployed 'is guaranteed to really be the one that has been tested and validated'. The manual also says 'Buildroot is not meant to be a distribution'.
- **Source:** The Buildroot user manual, §11.7 'Why doesn't Buildroot generate binary packages' — <https://buildroot.org/downloads/manual/manual.html>
- **Evidence:** Quote: 'by doing complete system upgrades by upgrading the entire root filesystem image at once, the image deployed to the embedded system is guaranteed to really be the one that has been tested and validated.' Also: 'Buildroot is not meant to be a distribution'.

### G-68

- **Verdict:** `CONFIRMED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** 'make legal-info' collects legally relevant material under legal-info/: a README, the config, sources, patches, a manifest with licences, and licence texts. It does not produce some material, such as some external toolchains' source and Buildroot's own source, and 'You (or your legal department) have to check the output of make legal-info before using it as your own compliance delivery.'
- **Source:** The Buildroot user manual, legal-info section — <https://buildroot.org/downloads/manual/manual.html>
- **Evidence:** Quotes: 'Buildroot will collect legally-relevant material in your output directory, under the legal-info/ subdirectory.' 'Buildroot does not produce some material that you will or may need, such as the toolchain source code for some of the external toolchains and the Buildroot source code itself.' 'You (or your legal department) have to check the output of make legal-info'.

### G-69

- **Verdict:** `CORRECTED`
- **Tier:** `buildroot` (Rule 23 priority 6 — Buildroot documentation/source)
- **Applies to:** all
- **Fact:** Buildroot 2026.08's rpi-firmware package pins raspberrypi/firmware commit 063bcab6c8a90efb0d19f69d88cbbc7ec79cab68 (bundled kernel 6.12.61, built 2025-12-08) and declares RPI_FIRMWARE_LICENSE = BSD-3-Clause with licence file boot/LICENCE.broadcom. That file is a binary-only, no-modification licence restricted to use 'for the purposes of developing for, running or using a Raspberry Pi device', so the BSD-3-Clause label is misleading. Raspberry Pi OS installs the equivalent, newer, proprietary GPU firmware and bootloader files through raspi-firmware 1:1.20260915-1.
- **Source:** Buildroot 2026.08 package/rpi-firmware/rpi-firmware.mk — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/package/rpi-firmware/rpi-firmware.mk>
- **Evidence:** 'RPI_FIRMWARE_VERSION = 063bcab6c8a90efb0d19f69d88cbbc7ec79cab68', 'RPI_FIRMWARE_LICENSE = BSD-3-Clause', 'RPI_FIRMWARE_LICENSE_FILES = boot/LICENCE.broadcom'.
- **Original claim (before verification):** Buildroot's rpi-firmware package pins raspberrypi/firmware commit 063bcab6c8a90efb0d19f69d88cbbc7ec79cab68. Its licence is declared as BSD-3-Clause with licence file boot/LICENCE.broadcom. Raspberry Pi OS installs the same proprietary GPU firmware and bootloader blobs through raspi-firmware.
- **Verifier note:** The .mk values were checked, and LICENCE.broadcom was read at the pinned commit. 'Same blobs' was too strong: they are the same kind of files at different versions. The licence mislabel is exactly the sort of thing a legal-info review must catch.

### G-70

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** all
- **Fact:** archive.raspberrypi.com publishes main/source/Sources (431197 bytes) and Sources.gz (120496 bytes) for trixie, and Sources.gz returns HTTP 200. Source for Raspberry Pi (+rpt) packages can therefore be fetched with apt deb-src.
- **Source:** archive.raspberrypi.com debian trixie Release — <https://archive.raspberrypi.com/debian/dists/trixie/Release>
- **Evidence:** The Release file lists 'main/source/Sources' (431197 bytes) and 'main/source/Sources.gz' (120496 bytes). Fetching Sources.gz returned HTTP 200.

### G-71

- **Verdict:** `CORRECTED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** Pi 4 Model B, CM4, Pi 5, CM5
- **Fact:** This is reasoning built on verified inputs. The 2026-10-06 Lite image ships both the rpi-v8 (bcm2711, 4K pages) and rpi-2712 (16K pages) kernels, with module trees that include tc358743, unicam, rp1-cfe and bcm2835-codec, and device trees for Pi 4B, CM4, Pi 5, CM5 and CM5 Lite. The firmware loads kernel_2712.img on BCM2712 and kernel8.img otherwise, so one image can boot all four candidates. More than overlays and storage differs per board. The overlay differs (tc358743 vs tc358743-pi5), as do the cam0 and 4lane parameters. The capture model differs: Pi 4/CM4 default to legacy Unicam in video-node mode, while Pi 5/CM5 use CFE with Media Controller only. The encode path differs: hardware bcm2835-codec on Pi 4/CM4, software only on Pi 5/CM5. Lane count differs: the Pi 4B connector is 2-lane, which caps TC358743 at 1080p50 YUV422 or 1080p30 RGB888, while 4 lanes on a Compute Module give 1080p60. Storage and provisioning differ too (SD vs eMMC with rpiboot). config.txt [pi4]/[pi5]/[cm4]/[cm5] filters let one file hold the per-model settings.
- **Source:** Inference from G-06, G-12, G-13, G-59 (Buildroot bcm2711 DTS list includes bcm2711-rpi-cm4; bcm2712 used for CM5) — <https://gitlab.com/buildroot.org/buildroot/-/blob/2026.08/configs/raspberrypi4_64_defconfig>
- **Verifier's best source:** <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/camera/csi-2-usage.adoc>
- **Evidence:** Buildroot raspberrypi4_64_defconfig builds 'broadcom/bcm2711-rpi-4-b ... broadcom/bcm2711-rpi-cm4' from bcm2711_defconfig. The cm5io defconfig uses bcm2712 with 'bcm2712-rpi-cm5-cm5io'. The Lite image ships both rpi-v8 and rpi-2712 kernels.
- **Original claim (before verification):** Reasoning: the Pi 4 and CM4 run the same arm64 bcm2711 kernel (linux-image-rpi-v8), and the Pi 5 and CM5 run the same bcm2712 kernel (linux-image-rpi-2712), so a single Raspberry Pi OS image covers all four candidate targets. Only config.txt overlays (tc358743 vs tc358743-pi5, cam0/4lane) and storage layout (SD vs eMMC) change between them.
- **Verifier note:** The SBOM lists bcm2711-rpi-cm4*.dtb and bcm2712-rpi-cm5*/cm5l*.dtb and both module trees. boot.adoc line 24 covers the kernel fallback, csi-2-usage.adoc the TC358743 lane limits, and conditional.adoc lines 31-61 the filters. The original 'only overlays and storage change' understated the differences.

---

## Topic H

**H.265/HEVC software encoding and transport (research of 2026-10-08)** — <a id="topic-h"></a>43 claims. Added 2026-10-08 (workflow wf_94a0b1b4-f2b; same method: researcher + independent adversarial verifier).

### H-01

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5, Raspberry Pi OS trixie
- **Fact:** Debian 13 trixie ships x265 version 4.1-2 (CLI package 'x265', shared library package libx265-215), built for arm64 as well as amd64, armel, armhf, i386, ppc64el, riscv64 and s390x.
- **Source:** Debian trixie package: x265 — <https://packages.debian.org/trixie/x265>
- **Evidence:** packages.debian.org/trixie/x265: Version 4.1-2, 'H.265/HEVC video stream encoder', library libx265-215, architectures include arm64.

### H-02

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5, Raspberry Pi OS trixie
- **Fact:** archive.raspberrypi.com does not override x265. Its pool/main/x/ directory has no x265 source package (the only video-encoder entry is x264/, dated 2019-06-17), and the trixie arm64 Packages indices for the 'main' and 'beta' components contain no x265 or libx265 package. Raspberry Pi OS therefore uses Debian's x265 4.1-2 / libx265-215 unchanged.
- **Source:** Raspberry Pi APT archive pool listing /debian/pool/main/x/ — <https://archive.raspberrypi.com/debian/pool/main/x/>
- **Evidence:** The directory listing fetched on 2026-10-08 shows x11-xserver-utils, x264 (2019-06-17), xarchiver, ... xwayland. There is no x265/ entry.

### H-03

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5, Raspberry Pi OS trixie
- **Fact:** Debian's x265 4.1-2 build enables assembly on arm64 (-DENABLE_ASSEMBLY=ON for amd64 and arm64). It also builds separate 10-bit and 12-bit static libraries and links them into the single 8-bit libx265 shared library (-DEXTRA_LIB="x265_main10.a;x265_main12.a" -DLINKED_10BIT=ON -DLINKED_12BIT=ON), so one libx265-215 provides Main, Main10 and Main12.
- **Source:** Debian x265 4.1-2 debian/rules (salsa) — <https://salsa.debian.org/multimedia-team/x265/-/raw/debian/4.1-2/debian/rules>
- **Evidence:** Quote: '# enable assembly builds on amd64 and arm64 / ifneq (,$(filter $(DEB_HOST_ARCH),amd64 arm64)) FLAGS += -DENABLE_ASSEMBLY=ON'. Also '-DEXTRA_LIB="x265_main10.a;x265_main12.a" -DLINKED_10BIT=ON -DLINKED_12BIT=ON'.

### H-04

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** x265 4.1 on Linux/AArch64 enables runtime CPU feature detection by default (AARCH64_RUNTIME_CPU_DETECT ON; ENABLE_NEON_DOTPROD, ENABLE_NEON_I8MM, ENABLE_SVE and ENABLE_SVE2 all default ON). It turns on the Neon DotProd kernels only when getauxval(AT_HWCAP) reports ASIMDDP (bit 20) and SVE only from AT_HWCAP bit 22. I8MM and SVE2 come from AT_HWCAP2. aarch64_cpu_detect() also masks I8MM/SVE off when DotProd is absent, and SVE2 off when SVE is absent.
- **Source:** x265 4.1 source/CMakeLists.txt and source/common/aarch64/cpu.h — <https://bitbucket.org/multicoreware/x265_git/raw/4.1/source/common/aarch64/cpu.h>
- **Evidence:** CMakeLists.txt: option(AARCH64_RUNTIME_CPU_DETECT ... ON); option(ENABLE_NEON_DOTPROD ... ON) ... cpu.h: '#define X265_AARCH64_HWCAP_ASIMDDP (1 << 20)' ... 'if (hwcap & X265_AARCH64_HWCAP_ASIMDDP) flags |= X265_CPU_NEON_DOTPROD;'

### H-05

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** GCC 14 defines cortex-a72 (the BCM2711 core) as Armv8-A + CRC. It defines cortex-a76 (the BCM2712 core) as Armv8.2-A + F16, RCPC and DOTPROD. So x265's Neon DotProd paths can apply only on CM5; I8MM, SVE and SVE2 paths apply on neither board.
- **Source:** GCC 14 gcc/config/aarch64/aarch64-cores.def — <https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-14/gcc/config/aarch64/aarch64-cores.def>
- **Evidence:** AARCH64_CORE("cortex-a72", cortexa72, cortexa57, V8A, (CRC), ...); AARCH64_CORE("cortex-a76", cortexa76, cortexa57, V8_2A, (F16, RCPC, DOTPROD), neoversen1, ...)

### H-06

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** x265's AArch64 SIMD work across recent releases: 3.6 (4 April 2024) added ARM64 NEON optimisations ('overall performance increased by around 20%') and SVE/SVE2. 4.0 (13 September 2024) added Arm SIMD that gives 'up to 57% faster encoding compared to release 3.6', using Armv8.4 DotProd, Armv8.6 I8MM and Armv9 SVE2. 4.1 (22 November 2024) lists no new Arm SIMD work; its Optimizations are lowresMC pointer copies and MCSTF.
- **Source:** x265 Release Notes — <https://x265.readthedocs.io/en/master/releasenotes.html>
- **Evidence:** Version 4.0 Optimizations: 'Arm SIMD optimizations ... up to 57% faster encoding compared to release 3.6. Arm SIMD optimizations include use of Armv8.4 DotProd, Armv8.6 I8MM, and Armv9 SVE2 ...'. Version 3.6: 'ARM64 NEON optimizations ... around 20%'. Version 4.1 Optimizations list only lowresMC and MCSTF items.

### H-07

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** Newer x265 releases have further AArch64 speed-ups that trixie's 4.1-2 lacks. 4.2 (19 April 2026) adds NEON/SVE optimisations giving '8% faster encoding speed compared to v4.1'. 4.3 (31 July 2026) improves the Neon and SVE DCT16/DCT32, adds new SVE2 psyCost/sa8d/satd, and makes the Neon sa8d, satd and psyCost faster. Only the Neon parts can help Cortex-A72/A76.
- **Source:** x265 Release Notes — <https://x265.readthedocs.io/en/master/releasenotes.html>
- **Evidence:** Version 4.2: 'ARM SIMD optimizations including the use of NEON and SVE ... resulting in 8% faster encoding speed compared to v4.1'. Version 4.3 (Release date - 31st July 2026): 'AArch64 SIMD optimizations: improved Neon and SVE DCT16/DCT32 ... faster Neon sa8d, satd and psyCost'.

### H-08

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5, FFmpeg 7.1.5 (RPi build)
- **Fact:** The Raspberry Pi FFmpeg source package 8:7.1.5-0+deb13u1+rpt2 (changelog entry 'HW accel patch 30', Serge Schneider, 27 August 2026) is configured with --enable-libx265 in the common CONFIG shared by every flavour (standard, static, extra). The full (non-stage1) build also has --enable-libx264 and --enable-libsrt.
- **Source:** Raspberry Pi archive: ffmpeg_7.1.5-0+deb13u1+rpt2.debian.tar.xz (debian/rules, debian/changelog) — <https://archive.raspberrypi.com/debian/pool/main/f/ffmpeg/ffmpeg_7.1.5-0+deb13u1+rpt2.debian.tar.xz>
- **Evidence:** debian/rules line 66: '--enable-libx265 \' inside 'CONFIG := ... --enable-gpl ...'. Lines 216 and 219: '--enable-libsrt', '--enable-libx264' in the non-stage1 branch. debian/control: '# --enable-libx265 / libx265-dev (>= 1.8)'.

### H-09

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5, FFmpeg 7.1.5 (RPi build)
- **Fact:** The Raspberry Pi arm64 binary libavcodec61 8:7.1.5-0+deb13u1+rpt2 declares Depends on libx265-215 (>= 4.1) and libx264-164 (>= 2:0.164.3108+git31e19f9). Its libavcodec.so.61.19.101 contains the encoder long name 'libx265 H.265 / HEVC', the symbol reference x265_api_get_215 and the configure string --enable-libx265, so the libx265 encoder is built into the distro FFmpeg.
- **Source:** Raspberry Pi archive: libavcodec61_7.1.5-0+deb13u1+rpt2_arm64.deb control file — <https://archive.raspberrypi.com/debian/pool/main/f/ffmpeg/libavcodec61_7.1.5-0+deb13u1+rpt2_arm64.deb>
- **Evidence:** control: 'Package: libavcodec61 / Version: 8:7.1.5-0+deb13u1+rpt2 / Depends: ... libx264-164 (>= 2:0.164.3108+git31e19f9), libx265-215 (>= 4.1) ...'

### H-10

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** FFmpeg 7.1.5
- **Fact:** FFmpeg 7.1.5's libx265 wrapper accepts only planar (or gray) inputs, never packed uyvy422, yuyv422 or nv12. Which list it advertises depends on the linked libx265: x265_csp_eight (yuv420p, yuvj420p, yuv422p, yuvj422p, yuv444p, yuvj444p, gbrp, gray8) when only 8-bit is available. With Debian's multi-depth libx265-215 (H-03), x265_api_get(12) succeeds, so the twelve-bit list is used, which adds yuv420p10/12, yuv422p10/12, yuv444p10/12, gbrp10/12 and gray10/12. The wrapper copies avctx->thread_count into x265's frameNumThreads after applying preset/tune.
- **Source:** FFmpeg n7.1.5 libavcodec/libx265.c — <https://raw.githubusercontent.com/FFmpeg/FFmpeg/n7.1.5/libavcodec/libx265.c>
- **Evidence:** static const enum AVPixelFormat x265_csp_eight[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUVJ422P, AV_PIX_FMT_YUV444P, AV_PIX_FMT_YUVJ444P, AV_PIX_FMT_GBRP, AV_PIX_FMT_GRAY8, ...}. Line 281: 'ctx->params->frameNumThreads = avctx->thread_count;'
- **Original claim (before verification):** FFmpeg 7.1.5's libx265 wrapper accepts only planar 8-bit inputs: yuv420p, yuvj420p, yuv422p, yuvj422p, yuv444p, yuvj444p, gbrp and gray8. It does not accept packed uyvy422. It also copies avctx->thread_count (-threads) into x265's frameNumThreads.
- **Verifier note:** libx265_get_supported_config() (lines ~959-968) picks x265_csp_twelve when x265_api_get(12) is non-NULL. 'Only planar 8-bit' is wrong for the Debian/RPi build. The frameNumThreads assignment (line 281) runs after param_default_preset(), so -tune zerolatency's frameNumThreads=1 is overwritten by -threads. The ffmpeg CLI defaults to threads=auto (0), which re-enables x265 auto frame threading.

### H-11

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5, GStreamer 1.26.2
- **Fact:** GStreamer's x265enc element lives in the 'x265' plugin of gst-plugins-bad ('GStreamer Bad Plug-ins'). Debian trixie's gstreamer1.0-plugins-bad 1.26.2-3+deb13u3 (arm64) ships it as /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstx265.so.
- **Source:** Debian trixie arm64 gstreamer1.0-plugins-bad file list; GStreamer x265 plugin docs — <https://packages.debian.org/trixie/arm64/gstreamer1.0-plugins-bad/filelist>
- **Evidence:** File list contains /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstx265.so. The GStreamer docs page for x265enc names the package as 'GStreamer Bad Plug-ins'.

### H-12

- **Verdict:** `CORRECTED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5, GStreamer 1.26.2
- **Fact:** Raspberry Pi's archive overrides gst-plugins-bad1.0 with 1.26.2-3+rpt4+deb13u3 (Serge Schneider, 25 Sep 2026). The rpt changelog entries are v4l2codecs patches, a wayland dmabuf column-mode stride patch, and 'armhf: remove libonnxruntime-dev dependency'. The package still Build-Depends on libx265-dev and still installs usr/lib/*/gstreamer-1.0/libgstx265.so in gstreamer1.0-plugins-bad.
- **Source:** Raspberry Pi archive: gst-plugins-bad1.0_1.26.2-3+rpt4+deb13u3.debian.tar.xz — <https://archive.raspberrypi.com/debian/pool/main/g/gst-plugins-bad1.0/gst-plugins-bad1.0_1.26.2-3+rpt4+deb13u3.debian.tar.xz>
- **Evidence:** changelog top entry is 1.26.2-3+rpt4+deb13u3 (Serge Schneider, 25 Sep 2026) with v4l2codecs changes. debian/control line 95: 'libx265-dev,'. debian/gstreamer1.0-plugins-bad.install line 117: 'usr/lib/*/gstreamer-1.0/libgstx265.so'.
- **Original claim (before verification):** Raspberry Pi's archive overrides gst-plugins-bad1.0 with 1.26.2-3+rpt4+deb13u3. The rpt changes are v4l2codecs and wayland dmabuf patches. The package still Build-Depends on libx265-dev and still installs libgstx265.so in gstreamer1.0-plugins-bad.
- **Verifier note:** Small correction: the rpt changes also include the armhf onnxruntime build-dependency removal. Checked debian/control line 95 'libx265-dev,' and .install line 117. None of the patches touch ext/x265, so the hard-coded latency in H-15 is unchanged. debian/rules sets -Dsvthevcenc=disabled. The RPi trixie arm64 Packages index lists gstreamer1.0-plugins-bad 1.26.2-3+rpt4+deb13u3.

### H-13

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5, GStreamer 1.26.2
- **Fact:** GStreamer 1.26.2 x265enc accepts only planar formats on its sink pad: Y444, Y42B and I420 at 8-bit, plus Y444/I422/I420 at 10-bit and 12-bit LE when the linked libx265 provides those depths (Debian's does, see H-03). It does not accept packed UYVY, YUY2 or NV12. Its source caps are video/x-h265, stream-format=byte-stream, alignment=au.
- **Source:** GStreamer 1.26.2 gst-plugins-bad ext/x265/gstx265enc.c; x265enc documentation — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/1.26.2/subprojects/gst-plugins-bad/ext/x265/gstx265enc.c>
- **Evidence:** gst_x265_enc_add_x265_chroma_format() appends only "Y444","Y42B","I420","Y444_10LE","I422_10LE","I420_10LE","Y444_12LE","I422_12LE","I420_12LE" (BE on big-endian). Src: 'stream-format = (string) byte-stream, alignment = (string) au'. The docs page lists the same sink formats.

### H-14

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** GStreamer 1.26.2
- **Fact:** x265enc's tuning properties are: speed-preset (ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow, placebo, plus a 0 'No preset' value; default medium); tune (psnr, ssim, grain, zerolatency, fastdecode, animation, plus a 0 'No tunning' value; default ssim); bitrate in kbit/s (default 2048, max 102400); key-int-max (default 0 = x265 default); and option-string for raw x265 'key=value:...' options.
- **Source:** GStreamer x265enc docs; x265 4.1 source/x265.h — <https://gstreamer.freedesktop.org/documentation/x265/index.html>
- **Evidence:** x265.h: 'x265_preset_names[] = { "ultrafast", ... "placebo", 0 }', 'x265_tune_names[] = { "psnr", "ssim", "grain", "zerolatency", "fastdecode", "animation", 0 }'. gstx265enc.c: PROP_SPEED_PRESET_DEFAULT 6 /* Medium */, PROP_TUNE_DEFAULT 2 /* SSIM */.

### H-15

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** GStreamer 1.26.2
- **Fact:** In GStreamer 1.26.2, x265enc reports a hard-coded latency of 5 frames unless tune=zerolatency, in which case it reports 0 frames (it assumes 25 fps if the framerate is unknown). Computing latency from the actual encoder parameters only arrived in 1.26.8, after Debian's 1.26.2, and neither Debian's nor Raspberry Pi's patches backport it.
- **Source:** gstx265enc.c (1.26.2) gst_x265_enc_set_latency; GStreamer 1.26 release notes — <https://gstreamer.freedesktop.org/releases/1.26/>
- **Evidence:** 1.26.2 source: '/* FIXME get a real value from the encoder ... */ if (... "zerolatency") max_delayed_frames = 0; else max_delayed_frames = 5;'. Release notes, 'Highlighted bugfixes in 1.26.8' section: 'x265enc: advertise latency based on encoder parameters instead of hard-coding it to 5 frames'.

### H-16

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** In x265 4.1, tune=zerolatency sets bframes=0, b-adapt off, lookahead depth 0, scenecut off, histogram scenecut off, cuTree off and frameNumThreads=1. Wavefront (WPP) row parallelism stays at its default (on), so frame-level parallelism is disabled and only WPP/pool threading remains.
- **Source:** x265 4.1 source/common/param.cpp — <https://bitbucket.org/multicoreware/x265_git/raw/4.1/source/common/param.cpp>
- **Evidence:** 'else if (!strcmp(tune, "zerolatency") ...) { param->bFrameAdaptive = 0; param->bframes = 0; param->lookaheadDepth = 0; param->scenecutThreshold = 0; param->bHistBasedSceneCut = 0; param->rc.cuTree = 0; param->frameNumThreads = 1; }'. Defaults: 'param->bEnableWavefront = 1; param->frameNumThreads = 0;'

### H-17

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** x265 4.1's ultrafast preset sets: max CU 32 and min CU 16, 3 B-frames (b-adapt off), lookahead 5, scenecut off, rdLevel 2, 1 reference, DIA motion search, subme 0, SAO off, sign hiding off, weighted prediction off, and AQ off.
- **Source:** x265 4.1 source/common/param.cpp — <https://bitbucket.org/multicoreware/x265_git/raw/4.1/source/common/param.cpp>
- **Evidence:** if (!strcmp(preset, "ultrafast")) { ... lookaheadDepth = 5; ... maxCUSize = 32; minCUSize = 16; bframes = 3; ... subpelRefine = 0; searchMethod = X265_DIA_SEARCH; bEnableSAO = 0; ... rdLevel = 2; maxNumReferences = 1; ... aqMode = X265_AQ_NONE; ...}

### H-18

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5, Raspberry Pi OS trixie
- **Fact:** Besides x265, trixie packages two other HEVC software encoders: kvazaar 2.3.1-2 (with libkvazaar7, arm64 included) and the HM reference software 'hm' / 'hm-highbitdepth' 18.0-2 (18.0-2+b1 on arm64), which provides TAppEncoderStatic. HM is a reference encoder and is not suitable for real-time use. Neither is wired into the distro media stacks: the RPi FFmpeg rpt2 rules/control have no --enable-libkvazaar, the RPi gst-plugins-bad control has no kvazaar, and gst-plugins-bad is built with -Dsvthevcenc=disabled. SVT-HEVC is not packaged in trixie (svt-hevc / libsvthevc1: 'No such package').
- **Source:** Debian trixie packages: kvazaar, svt-hevc — <https://packages.debian.org/trixie/kvazaar>
- **Verifier's best source:** <https://packages.debian.org/search?keywords=hevc&searchon=descriptions&suite=trixie&section=all>
- **Evidence:** packages.debian.org/trixie/kvazaar: 'Package: kvazaar (2.3.1-2) ... HEVC encoder - application', arm64 present. packages.debian.org/trixie/svt-hevc and /libsvthevc1: 'No such package'. grep -c kvazaar finds 0 matches in the RPi ffmpeg rpt2 debian/rules and the RPi gst-plugins-bad debian/control.
- **Original claim (before verification):** The only other HEVC software encoder packaged in trixie is kvazaar 2.3.1-2 (with libkvazaar7). It is not wired into the distro media stacks: the RPi FFmpeg rules have no --enable-libkvazaar, and the RPi gst-plugins-bad control file does not reference kvazaar. SVT-HEVC is not packaged in trixie.
- **Verifier note:** 'The only other HEVC software encoder is kvazaar' is wrong: packages.debian.org lists hm 18.0-2 ('Reference software for HEVC'), and its arm64 file list includes /usr/bin/TAppEncoderStatic. The kvazaar facts and the grep count of 0 were re-checked.

### H-19

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** CM5
- **Fact:** No raspberrypi.com documentation or product brief found gives a figure for HEVC software encode cost (based on a site search; absence cannot be proven exhaustively). The nearest statement comes from Raspberry Pi engineer 6by9 on the official forum (24 October 2024): 'Software H265 (HEVC) encode is too intensive an operation to perform at any significant resolution.'
- **Source:** Raspberry Pi Forums: 'RPI5 h264/h265 video encoding' — <https://forums.raspberrypi.com/viewtopic.php?t=378329>
- **Evidence:** 6by9 (Raspberry Pi Engineer & Forum Moderator), 24 Oct 2024: 'Software H265 (HEVC) encode is too intensive an operation to perform at any significant resolution.' The original poster reported that libx265 made the Pi 5 'unresponsive' and the video 'laggy'.

### H-20

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** CM5
- **Fact:** Community benchmark (Phoronix/OpenBenchmarking result 2309281-NE-RASPBERRY47, 28 September 2023; Pi 5, Cortex-A76 @ 2.40 GHz, Debian 12, kernel 6.1.0-rpi3-rpi-2712, GCC 12.2.0; PTS ffmpeg-6.0.0 profile built with an x265 git snapshot from 2022-10-28). Results: libx265 Live 10.00 FPS, Upload 1.68 FPS, Platform 3.26 FPS, Video On Demand 3.27 FPS. For comparison, libx264 Live was 66.17 FPS.
- **Source:** OpenBenchmarking.org: Raspberry Pi 5 Benchmarks (2309281-NE-RASPBERRY47) — <https://openbenchmarking.org/result/2309281-NE-RASPBERRY47>
- **Evidence:** FFmpeg 6.0 'Encoder: libx265 - Scenario: Live' 10.00 FPS; 'libx264 - Live' 66.17 FPS; 'libx265 - Upload' 1.68; 'libx265 - Platform' 3.26; 'libx265 - Video On Demand' 3.27. Read via the mail.openbenchmarking.org mirror.

### H-21

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** CM4, CM5
- **Fact:** Community benchmark (OpenBenchmarking 2309273-NE-2303239NE28, same FFmpeg 6.0 libx265 'Live' test). The 'Raspberry Pi 4' entry is actually a Raspberry Pi 400 (BCM2711, Cortex-A72 @ 1.80 GHz, Debian 11, kernel 5.15.84-v8+): 4.33 FPS. Orange Pi 5 (RK3588, 'Cortex-A76 @ 1.80GHz (4 Cores / 8 Threads)', Ubuntu 22.04): 8.87 FPS. Raspberry Pi 5: 9.99 FPS.
- **Source:** OpenBenchmarking.org: Raspberry Pi 5 Benchmarks (2309273-NE-2303239NE28) — <https://openbenchmarking.org/result/2309273-NE-2303239NE28>
- **Evidence:** 'FFmpeg 6.0 Encoder: libx265 - Scenario: Live' FPS: Raspberry Pi 4 4.33, Orange Pi 5 8.87, Raspberry Pi 5 9.99. The system table shows 'ARMv8 Cortex-A72 @ 1.80GHz (4 Cores), BCM2835 Raspberry Pi 400 Rev 1.0, Broadcom BCM2711', Debian 11, kernel 5.15.84-v8+.

### H-22

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** CM4, CM5
- **Fact:** The PTS FFmpeg 6.0 test behind H-20 and H-21 is not a 1080p60 live measurement. It runs vbench clips (from videos/crf18), calls ffmpeg with '-threads 1' (which libx265 turns into frameNumThreads=1; x265's WPP thread pool is still used), and its Live scenario uses a fixed bits-per-pixel target bitrate with '-preset veryfast -tune zerolatency' in the visible branch (the other branch is not visible in the patch). It builds x265 from a 2022-10-28 git snapshot, which predates x265 4.0's Arm optimisations.
- **Source:** phoronix-test-suite test-profiles pts/ffmpeg-6.0.0 install.sh — <https://github.com/phoronix-test-suite/test-profiles/blob/master/pts/ffmpeg-6.0.0/install.sh>
- **Evidence:** install.sh: 'tar -xf x265-20221028.tar.xz'. Patched vbench: 'cmd = [ffmpeg,"-i",video,"-c:v",encoder,"-threads",str(1)]+settings'. Live: 'settings += [ "-preset","veryfast","-tune","zerolatency" ]'. Videos come from 'videos/crf18'.

### H-23

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** CM5
- **Fact:** Reasoning: in the same PTS/vbench Live harness on Pi 5, libx265 was about 6.6 times slower than libx264 (66.17 / 10.00 = 6.62). This only gives a relative cost; it is not a prediction of PACSCORDER's 1080p30/60 throughput.
- **Source:** Calculation from H-20 — <https://openbenchmarking.org/result/2309281-NE-RASPBERRY47>
- **Evidence:** Inputs: libx264 Live 66.17 FPS and libx265 Live 10.00 FPS on Pi 5. 66.17 / 10.00 = 6.6.

### H-24

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** RTMP streaming
- **Fact:** The Enhanced RTMP specification (Veovera, enhanced-rtmp-v2.md on main) is at document version v2-2026-01-31-r2, marked as a Release Version. It defines the HEVC video FourCC as 'hvc1' (VideoFourCc.Hevc = makeFourCc("hvc1")) and lists 'hvc1' among the fourCcList connect-command values.
- **Source:** veovera/enhanced-rtmp: enhanced-rtmp-v2.md — <https://github.com/veovera/enhanced-rtmp/blob/main/docs/enhanced/enhanced-rtmp-v2.md>
- **Evidence:** '**Document Version:** **v2-2026-01-31-r2**'; 'This document represents a **Release Version** ...'; 'enum VideoFourCc { ... Hevc = makeFourCc("hvc1"), ...'; the fourCcList example includes "av01", "vp09", "vp08", "hvc1".

### H-25

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** FFmpeg 7.1.5, RTMP streaming
- **Fact:** FFmpeg 6.1 was the first release to mux HEVC into FLV and signal it over RTMP: its changelog has 'Support HEVC,VP9,AV1 codec in enhanced flv format' and 'Support HEVC,VP9,AV1 codec fourcclist in enhanced rtmp protocol'. Enhanced FLV v2 (multitrack audio/video, more codecs) arrived later, in FFmpeg 8.0.
- **Source:** FFmpeg Changelog (master) — <https://raw.githubusercontent.com/FFmpeg/FFmpeg/master/Changelog>
- **Evidence:** version 6.1: '- Support HEVC,VP9,AV1 codec in enhanced flv format' ... '- Support HEVC,VP9,AV1 codec fourcclist in enhanced rtmp protocol'. version 8.0: '- Enhanced FLV v2: Multitrack audio/video, modern codec support'.

### H-26

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** FFmpeg 7.1.5, RTMP streaming
- **Fact:** FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP publishing. Its FLV muxer maps AV_CODEC_ID_HEVC to FourCC 'hvc1' and writes enhanced video headers. The rtmp_enhanced_codecs option writes a fourCcList in the connect command but accepts only hvc1, av01 and vp09; any other FourCC fails with AVERROR_PATCHWELCOME. The 7.1 FLV audio table has no Opus; its entries are MP3, PCM (U8/S16BE/S16LE), ADPCM_SWF, AAC, Nellymoser, G.711 mu-law/A-law and Speex.
- **Source:** FFmpeg n7.1.5 libavformat/flvenc.c and rtmpproto.c — <https://raw.githubusercontent.com/FFmpeg/FFmpeg/n7.1.5/libavformat/flvenc.c>
- **Evidence:** flvenc.c: '{ AV_CODEC_ID_HEVC, MKBETAG('h', 'v', 'c', '1') }'; flv_audio_codec_ids has no OPUS. rtmpproto.c: 'ff_amf_write_field_name(&p, "fourCcList")', 'if (!strncmp(fourcc_data, "hvc1", 4) || ... "av01" ... "vp09")', option 'rtmp_enhanced_codecs'.

### H-27

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** GStreamer 1.26.2, RTMP streaming
- **Fact:** GStreamer 1.26.2's flvmux has no H.265 on its video sink pad (only video/x-flash-video, video/x-flash-screen, video/x-vp6-flash, video/x-vp6-alpha and video/x-h264 stream-format=avc). The 1.26 distro GStreamer therefore cannot put HEVC into FLV/RTMP; see [F-34] for eflvmux arriving in 1.28.
- **Source:** GStreamer 1.26.2 gst-plugins-good gst/flv/gstflvmux.c — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/1.26.2/subprojects/gst-plugins-good/gst/flv/gstflvmux.c>
- **Evidence:** GST_STATIC_CAPS ("video/x-flash-video; video/x-flash-screen; video/x-vp6-flash; video/x-vp6-alpha; video/x-h264, stream-format=avc;"). grep -i 'h265|hevc|hvc1' finds 0 matches in the file.

### H-28

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** RTMP streaming, SRT, WebRTC
- **Fact:** MediaMTX's docs (main branch; latest release v1.21.1, published 2026-09-20) list these codecs. RTMP publish/read video: AV1, VP9, H265, H264; audio: Opus, FLAC, AAC, MP3, AC-3, G711, LPCM. RTMP is described as 'expanded to support modern codecs (Enhanced RTMP)'. SRT publish video: H265, H264, MPEG-4 Video, MPEG-1/2 Video. WebRTC read video: AV1, VP9, VP8, H265, H264.
- **Source:** MediaMTX docs: RTMP clients / SRT clients / WebRTC read — <https://github.com/bluenviron/mediamtx/blob/main/docs/3-publish/09-rtmp-clients.md>
- **Evidence:** 09-rtmp-clients.md: '| **video** | AV1, VP9, H265, H264 |' and 'It has been expanded to support modern codecs (Enhanced RTMP)'. 03-srt-clients.md: '| **video** | H265, H264, MPEG-4 Video ...'. 4-read/03-webrtc.md: '| **video** | AV1, VP9, VP8, H265, H264 |'. GitHub API: tag v1.21.1, published 2026-09-20.

### H-29

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** RTMP streaming
- **Fact:** YouTube Live's official encoder settings page lists RTMP/RTMPS with video codecs H.264, H.265 (HEVC) and AV1, and audio AAC or MP3. Other settings: up to 60 fps, 2 s keyframes recommended (do not exceed 4 s), CBR, and HEVC for HDR ('we recommend using H.265 over RTMP(S)'). Recommended 1080p60 bitrate is 4 Mbps minimum / 12 Mbps recommended for AV1 and H.265, versus 6 / 17 Mbps for H.264. The page does not use the term 'Enhanced RTMP'.
- **Source:** YouTube Help: Choose live encoder settings, bitrates, and resolutions — <https://support.google.com/youtube/answer/2853702?hl=en>
- **Evidence:** 'Protocol: RTMP/RTMPS Streaming / Video codec: H.264 / H.265 (HEVC) / AV1 / Frame rate: up to 60 fps / Keyframe frequency: Recommended 2 seconds, Do not exceed 4 seconds / Audio codec: AAC or MP3'. Table row '1080p @60fps: 4 Mbps, 12 Mbps, 6 Mbps, 17 Mbps'. Also: 'If you want to stream in HDR, we recommend using H.265 over RTMP(S).'

### H-30

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** SRT, GStreamer 1.26.2, FFmpeg 7.1.5 (RPi build)
- **Fact:** The distro stacks have the components for HEVC over SRT in MPEG-TS. GStreamer 1.26.2 mpegtsmux accepts video/x-h265 stream-format=byte-stream (alignment au or nal), and trixie's gstreamer1.0-plugins-bad ships libgstsrt.so and libgstmpegtsmux.so. The RPi FFmpeg build has --enable-libsrt.
- **Source:** GStreamer 1.26.2 gstmpegtsmux.c; Debian trixie plugins-bad file list — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/1.26.2/subprojects/gst-plugins-bad/gst/mpegtsmux/gstmpegtsmux.c>
- **Evidence:** gstmpegtsmux.c line 109: '"video/x-h265,stream-format=(string)byte-stream,"'. The trixie arm64 file list contains libgstsrt.so and libgstmpegtsmux.so. RPi ffmpeg rules: '--enable-libsrt'.

### H-31

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** WebRTC, GStreamer 1.26.2
- **Fact:** RFC 7798 (March 2016, Standards Track) defines the RTP payload format for HEVC. GStreamer's rtph265pay implements it ('Payload-encode H265 video into RTP packets (RFC 7798)'). Adding profile-id, tier-flag and level-id to rtph265pay output caps came in 1.26.4, so the distro's 1.26.2 does not have it.
- **Source:** RFC 7798; GStreamer 1.26.2 gstrtph265pay.c; GStreamer 1.26 release notes — <https://www.rfc-editor.org/rfc/rfc7798>
- **Evidence:** RFC 7798 header: 'Request for Comments: 7798, Category: Standards Track, March 2016, RTP Payload Format for High Efficiency Video Coding (HEVC)'. rtph265pay.c line 229: 'Payload-encode H265 video into RTP packets (RFC 7798)'. Release notes, 'Highlighted bugfixes in 1.26.4': 'rtph265pay: add profile-id, tier-flag, and level-id to output rtp caps'.

### H-32

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** WebRTC
- **Fact:** RFC 7742 requires WebRTC browsers to implement VP8 and H.264 Constrained Baseline. It mentions H.265 only as a reference for SEI 'Display Orientation' messages in the CVO discussion, not as a required codec.
- **Source:** RFC 7742: WebRTC Video Processing and Codec Requirements — <https://www.rfc-editor.org/rfc/rfc7742>
- **Evidence:** 'WebRTC Browsers MUST implement the VP8 video codec ... and H.264 Constrained Baseline'. The only H.265 mention: 'the SEI "Display Orientation" messages in H.264 and H.265 [H265]' and the [H265] reference entry.

### H-33

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** WebRTC
- **Fact:** Chrome turned on H.265 in WebRTC by default in Chrome 136 on desktop, Android and WebView, but only where the platform provides it in hardware; Chrome has no software fallback. Chromestatus records Safari as 'Shipped/Shipping' and Firefox as 'No signal'.
- **Source:** Chrome Platform Status: H265 (HEVC) codec support in WebRTC (feature 5153479456456704) — <https://chromestatus.com/feature/5153479456456704>
- **Evidence:** API JSON: status 'Enabled by default', milestone '136', desktop 136, android 136, webview 136. Summary: 'we should support it in WebRTC when provided by the platform, i.e., if it is available in hardware (we will not provide a software implementation)'. safari: 'Shipped/Shipping'; ff: 'No signal'. Feature notes: '--enable-features=WebRtcAllowH265Send,WebRtcAllowH265Receive'.

### H-34

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** WebRTC
- **Fact:** WebKit's Safari 18.0 post says Safari 18.0 added the standard RFC HEVC RTP payload format for WebRTC, replacing the earlier generic packetization. The post's text writes 'RFC 7789', but the HEVC RTP payload RFC is 7798.
- **Source:** WebKit Features in Safari 18.0 — <https://webkit.org/blog/15865/webkit-features-in-safari-18-0/>
- **Evidence:** 'WebKit for Safari 18.0 adds support for the WebRTC HEVC RFC 7789 RTP Payload Format. Previously, the WebRTC HEVC used generic packetization instead of RFC 7789 packetization.'

### H-35

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** WebRTC
- **Fact:** No evidence was found that Firefox supports H.265 in WebRTC. Mozilla's standards-positions issue #1188 ('H265 (HEVC) codec support in WebRTC', opened 3 March 2025) is still open with only 'venue: W3C' and 'topic: API' labels and no position. Chromestatus records Firefox as 'No signal'.
- **Source:** mozilla/standards-positions issue #1188 — <https://github.com/mozilla/standards-positions/issues/1188>
- **Evidence:** GitHub API: state 'open', created 2025-03-03, labels ['venue: W3C','topic: API'], no position label. A user comment from 2025-08-30 says Firefox supports HEVC outside WebRTC 'however through webrtc it does not'.

### H-36

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** WebRTC
- **Fact:** Microsoft Edge 147 (Windows 11) had not enabled H.265 in WebRTC by default as of May 2026, even though it is Chromium-based. This comes from a Microsoft Q&A answer (Thomas4-N, 'Microsoft External Staff • Moderator', 2026-05-05): Edge 'hasn't enabled those flags by default yet, so H.265 simply isn't advertised in the SDP offer'. The workaround given is to launch with --enable-features=WebRtcAllowH265Send,WebRtcAllowH265Receive.
- **Source:** Microsoft Q&A: H.265 (HEVC) not published/sent via WebRTC - Chrome supports it, Edge does not — <https://learn.microsoft.com/en-in/answers/questions/5880331/h-265-hevc-not-published-sent-via-webrtc-chrome-su>
- **Evidence:** Answer of 2026-05-05: Edge 'has NOT enabled these flags [WebRtcAllowH265Send/Receive] by default yet ... H.265 is not advertised in SDP offers'. It points to tracking issue MicrosoftEdge/MSEdgeExplainers #1273.

### H-37

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Recording, GStreamer 1.26.2
- **Fact:** GStreamer 1.26.2 can record HEVC to MP4 or Matroska. qtmux and mp4mux accept video/x-h265 stream-format {hvc1, hev1}, alignment=au, and write an hvc1 or hev1 sample entry with an hvcC box. matroskamux accepts video/x-h265 {hvc1, hev1} but warns that hev1 'is not officially supported, only use this format for smart encoding'. x265enc outputs byte-stream, so h265parse is needed before these muxers.
- **Source:** GStreamer 1.26.2 gst-plugins-good isomp4/gstqtmuxmap.c, isomp4/gstqtmux.c, matroska/matroska-mux.c — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/1.26.2/subprojects/gst-plugins-good/gst/isomp4/gstqtmuxmap.c>
- **Evidence:** gstqtmuxmap.c: '#define H265_CAPS "video/x-h265, stream-format = (string) { hvc1, hev1 }, alignment = (string) au, "', used in both the qtmux and mp4mux templates. gstqtmux.c: 'entry.fourcc = FOURCC_hvc1' / 'FOURCC_hev1', build_codec_data_extension(FOURCC_hvcC, ...). matroska-mux.c line 121: 'video/x-h265, stream-format = (string) { hvc1, hev1 }, alignment=au'; line 1354 warning text.

### H-38

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Recording, FFmpeg 7.1.5
- **Fact:** FFmpeg 7.1.5's MP4 muxer tag table lists HEVC as 'hev1', 'hvc1' and 'dvh1'. isom_tags.c notes that 'hev1' means parameter sets may be in the elementary stream and 'hvc1' means they shall not be. FFmpeg's Matroska muxer also handles AV_CODEC_ID_HEVC (hvcC CodecPrivate via ff_isom_write_hvcc).
- **Source:** FFmpeg n7.1.5 libavformat/movenc.c, isom_tags.c, matroskaenc.c — <https://raw.githubusercontent.com/FFmpeg/FFmpeg/n7.1.5/libavformat/movenc.c>
- **Evidence:** movenc.c codec_mp4_tags: '{ AV_CODEC_ID_HEVC, MKTAG('h','e','v','1') }, { AV_CODEC_ID_HEVC, MKTAG('h','v','c','1') }, { AV_CODEC_ID_HEVC, MKTAG('d','v','h','1') }'. isom_tags.c: 'hev1 ... /* HEVC/H.265 which indicates parameter sets may be in ES */', 'hvc1 ... /* ... parameter sets shall not be in ES */'. matroskaenc.c references AV_CODEC_ID_HEVC at lines 1141 and 3443.

### H-39

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Licensing
- **Fact:** x265 is copyright MulticoreWare. It is licensed under GPL version 2 'or (at your option) any later version', and also under a commercial proprietary licence (contact license @ x265.com). The x265 docs state that neither the GPL nor the commercial licence covers HEVC patents.
- **Source:** x265 4.1 source/x265.h header; doc/reST/introduction.rst — <https://bitbucket.org/multicoreware/x265_git/raw/4.1/doc/reST/introduction.rst>
- **Evidence:** x265.h: 'either version 2 of the License, or (at your option) any later version ... This program is also available under a commercial proprietary license. For more information, contact us at license @ x265.com.' introduction.rst: 'The GNU GPL v2 license or the x265 commercial license agreement govern your rights to access the copyrighted x265 software source code, but do not cover any patents that may be applicable ... You are responsible for ... licensing all applicable patent rights'.

### H-40

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Licensing
- **Fact:** Access Advance acquired the former Via LA HEVC/VVC program as of 15 December 2025; the page is now managed by Video Codec Licensing LLC, an Access Advance subsidiary (contact VCL Advance). Listed rates (per unit, annual reset): units 1–100,000 cost $0.00 (available to one Legal Entity in an affiliated group); from unit 100,001, $0.30 each in R1 and $0.20 each in R2. The maximum annual royalty per Enterprise (Legal Entity and Affiliates) is $30,000,000. Coverage and royalties apply from 1 May 2013, and the licence 'extends to devices implementing the technology'.
- **Source:** VCL Advance (formerly Via LA) HEVC/VVC licensing program page — <https://www.via-la.com/licensing-programs/hevc-vvc/>
- **Evidence:** 'Please note that as of December 15, 2025, Access Advance has acquired this HEVC/VVC program ... managed by Video Codec Licensing LLC, a subsidiary of Access Advance'. 'For the first 1 to 100,000 units $0.00* ... For units 100,001 and more $ 0.30 ea. for sales in R1** $0.20 ea. for sales in R2** ... Maximum annual royalty payable by an Enterprise ...: $30,000,000 ... * available to one Legal Entity in an affiliated group'.

### H-41

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Licensing
- **Fact:** Access Advance (HEVC Advance pool) says a licence is 'most likely' needed for any product that can encode and/or decode HEVC. The royalty falls due when a Consumer HEVC Product (or HEVC content on digital media storage) is sold to an End User, if an HEVC Essential Patent on its list is in force in the country of manufacture or of sale/distribution. Software that users download for free 'in general' needs a licence too, with case-by-case exceptions.
- **Source:** Access Advance FAQ; 'Where and When is a Royalty Due?' — <https://accessadvance.com/faq/>
- **Evidence:** FAQ: 'You most likely need a license if you sell any products that have HEVC/H.265 encoding and/or decoding capability/functionality.' 'In general, HEVC software downloaded by users requires a license. However, there are some situations wherein a license is not needed.' Royalty page: 'A royalty is due upon the Sale of a Consumer HEVC Product ... for which an HEVC Standard Essential Patent ... is in force in either the country/territory of Manufacture or the country/territory of Sale.'

### H-42

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** Licensing
- **Fact:** Access Advance's published HEVC Advance rate table (page header 'For Licensees Having a PPL With Effective Date On or After July 1, 2026') lists 'Connected Home & Other Devices', with examples including surveillance cameras, conferencing products, digital signage and HEVC software. For Devices >$80 and All HEVC Software, the in-compliance rate without trademark discount is $1.111 (Region 1) / $0.555 (Region 2) per unit, with a $30MM category cap, $60M annual enterprise cap and $25,000 annual enterprise credit. With the trademark discount the rate is $1.00 / $0.50. The standard (non-compliant) rate is $1.333 / $0.667 with no caps or credit.
- **Source:** Access Advance: HEVC Advance Patent Pool detailed royalty rates (rate-table images) — <https://accessadvance.com/hevc-advance-patent-pool-detailed-royalty-rates/>
- **Evidence:** Image 'HEVC-Advance-Rates-Only-without-trademark-discount-on-or-after-1.1.26-Nov-2025': 'Devices >$80.00 / All HEVC Software: $1.111/$0.555'; category cap $30MM; 'Annual Enterprise Cap $60 million / Annual Enterprise Credit $25,000'. Standard-rate image: '$1.333/$0.667', 'No Cap Applies'. The page header reads 'For Licensees Having a PPL With Effective Date On or After July 1, 2026'.

### H-43

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** CM4, CM5
- **Fact:** Reasoning: if x265 is fed from the TC358743's UYVY output, each 1080p60 frame must first be converted to planar I420 (or Y42B). Done on the CPU at 1080p60, that conversion reads about 249 MB/s and writes about 187 MB/s, before x265 starts.
- **Source:** Calculation from H-10, H-13 — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/1.26.2/subprojects/gst-plugins-bad/ext/x265/gstx265enc.c>
- **Evidence:** UYVY 1920x1080x2 B = 4,147,200 B/frame x 60 = 248.8 MB/s read. I420 1920x1080x1.5 B = 3,110,400 B/frame x 60 = 186.6 MB/s written. Neither x265enc (H-13) nor FFmpeg libx265 (H-10) accepts UYVY.

---

## Topic I

**HDMI audio capture path: TC358743 → I2S → ALSA → AAC/Opus (research of 2026-10-08)** — <a id="topic-i"></a>47 claims. Added 2026-10-08 (workflow wf_94a0b1b4-f2b; same method: researcher + independent adversarial verifier).

### I-01

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** In rpi-6.18.y, tc358743-audio-overlay.dts declares compatible = "brcm,bcm2835". Its fragment@0 targets <&i2s_clk_consumer> and only sets status = "okay". The overlay never references the plain <&i2s> label.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts>
- **Evidence:** Verbatim: 'compatible = "brcm,bcm2835"; fragment@0 { target = <&i2s_clk_consumer>; __overlay__ { status = "okay"; }; };'

### I-02

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** tc358743-audio fragment@1 (target-path "/") adds the node tc358743_codec: tc358743-codec with #sound-dai-cells = <0>, compatible = "linux,spdif-dir" and status = "okay". TC358743 has no ASoC codec driver of its own.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743-audio-overlay.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts>
- **Evidence:** 'tc358743_codec: tc358743-codec { #sound-dai-cells = <0>; compatible = "linux,spdif-dir"; status = "okay"; };'

### I-03

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** tc358743-audio fragment@2 makes <&sound> a simple-audio-card with format "i2s" and name "tc358743". Both bitclock-master and frame-master point at the codec subnode (dailink0_master), so TC358743 drives BCK and LRCK. The CPU DAI is <&i2s_clk_consumer> with dai-tdm-slot-num = <2> and dai-tdm-slot-width = <32>. The only override is card-name.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743-audio-overlay.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts>
- **Evidence:** 'simple-audio-card,format = "i2s"; simple-audio-card,name = "tc358743"; simple-audio-card,bitclock-master = <&dailink0_master>; simple-audio-card,frame-master = <&dailink0_master>; ... simple-audio-card,cpu { sound-dai = <&i2s_clk_consumer>; dai-tdm-slot-num = <2>; dai-tdm-slot-width = <32>; }; dailink0_master: simple-audio-card,codec { sound-dai = <&tc358743_codec>; }; __overrides__ { card-name = <&sound_overlay>,"simple-audio-card,name"; }'

### I-04

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5
- **Fact:** The overlays README entry for tc358743-audio (lines 5609-5615) reads: 'Used in combination with the tc358743-fast overlay to route the audio from the TC358743 over I2S to the Pi. Wiring is LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18, and DATA/SD to GPIO 20.' Its only parameter is card-name (default "tc358743"). 'tc358743-fast' is a stale name: the overlays Makefile (lines 326-328) builds only tc358743.dtbo, tc358743-audio.dtbo and tc358743-pi5.dtbo, and the directory has only those overlays plus tc358743.dtsi.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/README and Makefile — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** README lines 5609-5615: 'Name: tc358743-audio / Info: Used in combination with the tc358743-fast overlay ... / Load: dtoverlay=tc358743-audio,<param>=<val> / Params: card-name Override the default, "tc358743", card name.' Makefile lines 326-328 list only tc358743.dtbo, tc358743-audio.dtbo, tc358743-pi5.dtbo.

### I-05

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** overlay_map.dts in rpi-6.18.y has no tc358743-audio node. Its only tc358743 node (lines 457-461) is 'tc358743 { bcm2835; bcm2711; bcm2712 = "tc358743-pi5"; };'. On CM5, dtoverlay=tc358743 therefore loads tc358743-pi5, and tc358743-audio loads under its own name.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/overlay_map.dts — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/overlay_map.dts>
- **Evidence:** grep -i tc358 finds only 'tc358743 { bcm2835; bcm2711; bcm2712 = "tc358743-pi5"; };' (lines 457-461). There is no tc358743-audio node.

### I-06

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5
- **Fact:** Raspberry Pi documentation says: 'Any overlay not mentioned in the map is assumed to be compatible with all platforms', where bcm2712 covers Raspberry Pi 5, CM5, 500 and 500+. The firmware therefore does not block tc358743-audio on CM5. Whether it works depends on its labels (i2s_clk_consumer, sound) resolving in the bcm2712 base DT.
- **Source:** Raspberry Pi documentation, configuration/reference.adoc 'The overlay map file' — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/configuration/reference.adoc>
- **Evidence:** 'Any platform not included in an overlay's node is not compatible with that overlay. Any overlay not mentioned in the map is assumed to be compatible with all platforms.' and 'bcm2712 for Raspberry Pi 5, CM5, 500, and 500+'

### I-07

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM5
- **Fact:** bcm2712-rpi.dtsi defines 'i2s: &rp1_i2s0', 'i2s_clk_producer: &rp1_i2s0' and 'i2s_clk_consumer: &rp1_i2s1' (lines 385-387). It gives &i2s_clk_consumer pinctrl-0 = <&rp1_i2s1_18_21> (lines 451-454) and defines 'sound: sound { status = "disabled"; }' (line 373). bcm2712-rpi-cm5.dtsi includes bcm2712-rpi.dtsi (line 196), so both labels the overlay needs exist on CM5.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/boot/dts/broadcom/bcm2712-rpi.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi.dtsi>
- **Evidence:** Lines 385-387: 'i2s: &rp1_i2s0 { }; i2s_clk_producer: &rp1_i2s0 { }; i2s_clk_consumer: &rp1_i2s1 { };'. Lines 451-454: '&i2s_clk_consumer { pinctrl-names = "default"; pinctrl-0 = <&rp1_i2s1_18_21>; };'. Line 373: 'sound: sound { status = "disabled"; };'. CM5 dtsi line 196: '#include "bcm2712-rpi.dtsi"'.

### I-08

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM5
- **Fact:** In rp1.dtsi, rp1_i2s1 is node i2s@a4000 (reg <0xc0 0x400a4000 0x0 0x1000>). It has compatible "snps,designware-i2s", DMA channels RP1_DMA_I2S1_TX/RX, dma-maxburst = <4> and status "disabled", and its interrupt is commented out ('Providing an interrupt disables DMA'). Pin group rp1_i2s1_18_21 uses function "i2s1" on gpio18, gpio19, gpio20 and gpio21 with bias-disable.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/boot/dts/broadcom/rp1.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/rp1.dtsi>
- **Evidence:** 'rp1_i2s1: i2s@a4000 { reg = <0xc0 0x400a4000 0x0 0x1000>; compatible = "snps,designware-i2s"; ... dmas = <&rp1_dma RP1_DMA_I2S1_TX>,<&rp1_dma RP1_DMA_I2S1_RX>; ... status = "disabled"; }' and 'rp1_i2s1_18_21: rp1_i2s1_18_21 { function = "i2s1"; pins = "gpio18", "gpio19", "gpio20", "gpio21"; bias-disable; };'

### I-09

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM5
- **Fact:** For DT-probed instances, dwc-i2s.c sets DW_I2S_MASTER or DW_I2S_SLAVE from the hardware COMP_PARAM_1 MODE_EN bit (bit 4). dw_i2s_set_fmt() accepts SND_SOC_DAIFMT_BC_FC only if DW_I2S_SLAVE is set, accepts BP_FP only if DW_I2S_MASTER is set, and always rejects BC_FP and BP_FC with -EINVAL. The RP1 datasheet says I2S0 is a clock producer (master) and I2S1 a clock consumer (slave). A codec-master link such as tc358743-audio therefore has to use rp1_i2s1, which is what the i2s_clk_consumer label points to.
- **Source:** raspberrypi/linux rpi-6.18.y sound/soc/dwc/dwc-i2s.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/dwc/dwc-i2s.c>
- **Evidence:** dw_configure_dai(): 'if (COMP1_MODE_EN(comp1)) { ... dev->capability |= DW_I2S_MASTER; } else { ... dev->capability |= DW_I2S_SLAVE; }'. dw_i2s_set_fmt(): 'case SND_SOC_DAIFMT_BC_FC: if (dev->capability & DW_I2S_SLAVE) ret = 0; else ret = -EINVAL; ... case SND_SOC_DAIFMT_BC_FP: case SND_SOC_DAIFMT_BP_FC: ret = -EINVAL;'

### I-10

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4
- **Fact:** On BCM2711 (CM4), bcm270x-rpi.dtsi (lines 151-152) points both i2s_clk_producer and i2s_clk_consumer at the single &i2s node (i2s@7e203000, compatible "brcm,bcm2835-i2s", status disabled in bcm283x.dtsi). bcm2711-rpi-cm4.dts sets &i2s pinctrl-0 = <&i2s_pins>, and bcm2711-rpi-ds.dtsi defines i2s_pins as brcm,pins = <18 19 20 21> in BCM2835_FSEL_ALT0 (bcm2711-rpi-ds.dtsi also includes bcm270x-rpi.dtsi). Setting status=okay on i2s_clk_consumer flips the same node as dtparam=i2s=on ('i2s = <&i2s>,"status"'), which defaults to off.
- **Source:** raspberrypi/linux rpi-6.18.y bcm270x-rpi.dtsi, bcm283x.dtsi, bcm2711-rpi-cm4.dts, bcm2711-rpi-ds.dtsi, overlays README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/broadcom/bcm270x-rpi.dtsi>
- **Evidence:** bcm270x-rpi.dtsi lines 151-152: 'i2s_clk_producer: &i2s {}; i2s_clk_consumer: &i2s {};'. bcm283x.dtsi: 'i2s: i2s@7e203000 { compatible = "brcm,bcm2835-i2s";'. bcm2711-rpi-cm4.dts: '&i2s { pinctrl-names = "default"; pinctrl-0 = <&i2s_pins>; };'. bcm2711-rpi-ds.dtsi: 'i2s_pins: i2s { brcm,pins = <18 19 20 21>; brcm,function = <BCM2835_FSEL_ALT0>; };'. README: 'i2s Set to "on" to enable the i2s interface (default "off")'.

### I-11

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** The 'linux,spdif-dir' stub codec (sound/soc/codecs/spdif_receiver.c, module snd-soc-spdif-rx) has one DAI, "dir-hifi", and it is capture only. It allows 1-384 channels, rates SNDRV_PCM_RATE_8000_768000 | SNDRV_PCM_RATE_128000, and formats S16_LE, S20_3LE, S24_LE, S32_LE and IEC958_SUBFRAME_LE. It has no DAI ops (no hw_params), no ALSA controls, and no reference to the TC358743 driver. The component has only one DAPM input widget (spdif-in) and one route.
- **Source:** raspberrypi/linux rpi-6.18.y sound/soc/codecs/spdif_receiver.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/codecs/spdif_receiver.c>
- **Evidence:** '#define STUB_RATES (SNDRV_PCM_RATE_8000_768000 | SNDRV_PCM_RATE_128000)'; '#define STUB_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_LE)'; 'static struct snd_soc_dai_driver dir_stub_dai = { .name = "dir-hifi", .capture = { .stream_name = "Capture", .channels_min = 1, .channels_max = 384, ...'. The component driver has only DAPM widgets and routes. Makefile: 'snd-soc-spdif-rx-y := spdif_receiver.o'.

### I-12

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** Both arm64 defconfigs (bcm2711_defconfig for CM4, bcm2712_defconfig for CM5) set CONFIG_SND_SIMPLE_CARD=m, CONFIG_SND_BCM2835_SOC_I2S=m, CONFIG_SND_DESIGNWARE_I2S=m, CONFIG_SND_DESIGNWARE_PCM=y and CONFIG_VIDEO_TC358743=m. CONFIG_SND_SOC_SPDIF is not set directly. It is pulled in by CONFIG_SND_RP1_AUDIO_OUT=m, whose Kconfig entry does 'select SND_SOC_SPDIF'. The packaged Raspberry Pi kernels (linux-headers-6.18.50+rpt-rpi-v8 and -rpi-2712 .config) both contain CONFIG_SND_SOC_SPDIF=m.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/configs/bcm2711_defconfig, bcm2712_defconfig, sound/soc/raspberrypi/Kconfig — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/raspberrypi/Kconfig>
- **Evidence:** sound/soc/raspberrypi/Kconfig: 'config SND_RP1_AUDIO_OUT tristate "PWM Audio Out from RP1" select SND_SOC_GENERIC_DMAENGINE_PCM select SND_SOC_SPDIF'. Both defconfigs contain CONFIG_SND_RP1_AUDIO_OUT=m, CONFIG_SND_SIMPLE_CARD=m, CONFIG_SND_BCM2835_SOC_I2S=m, CONFIG_SND_DESIGNWARE_I2S=m, CONFIG_VIDEO_TC358743=m.

### I-13

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4
- **Fact:** bcm2835-i2s (the CM4 CPU DAI, driver name "bcm2835-i2s") supports capture at exactly 2 channels, SNDRV_PCM_RATE_CONTINUOUS from 8000 to 384000 Hz, in S16_LE, S24_LE or S32_LE. hw_params calls clk_set_rate() only when the CPU is the bit-clock provider (BP_FP/BP_FC). In the overlay's BC_FC mode it sets the CLKM and FSM slave bits, and the requested rate never reaches hardware. Intersected with spdif-dir, the usable capture space on CM4 is 2 ch x {S16_LE, S24_LE, S32_LE} at any rate from 8-384 kHz, and the rate actually on the wire is set by the TC358743.
- **Source:** raspberrypi/linux rpi-6.18.y sound/soc/bcm/bcm2835-i2s.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/bcm/bcm2835-i2s.c>
- **Evidence:** '.capture = { .channels_min = 2, .channels_max = 2, .rates = SNDRV_PCM_RATE_CONTINUOUS, .rate_min = 8000, .rate_max = 384000, .formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE }'; '/* Clock should only be set up here if CPU is clock master */ if (bit_clock_provider && ...) { ... clk_set_rate(dev->clk, bclk_rate);'

### I-14

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM5
- **Fact:** On RP1, dw_configure_dai_by_dt() declares SNDRV_PCM_RATE_8000_768000. Capture channels_max (2*(COMP1_RX_CHANNELS+1)) and formats (formats[] indexed by COMP2 RX word size) are read from the hardware COMP_PARAM registers, so they are not visible in source. dw_i2s_set_tdm_slot() accepts only slot_width == 32 with 0-16 slots, and requires rx_mask == tx_mask with a non-zero mask. The overlay's 2 x 32-bit setting passes, because ASoC defaults both masks to 0x3. hw_params also accepts only 2, 4, 6 or 8 channels and S16/S24/S32_LE.
- **Source:** raspberrypi/linux rpi-6.18.y sound/soc/dwc/dwc-i2s.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/dwc/dwc-i2s.c>
- **Evidence:** dw_configure_dai_by_dt(): 'ret = dw_configure_dai(dev, dw_i2s_dai, SNDRV_PCM_RATE_8000_768000);'. dw_configure_dai(): 'dw_i2s_dai->capture.channels_max = 2 * (COMP1_RX_CHANNELS(comp1) + 1); dw_i2s_dai->capture.formats = formats[idx];'. dw_i2s_set_tdm_slot(): 'if (slot_width != 32) return -EINVAL; if (slots < 0 || slots > 16) return -EINVAL;'

### I-15

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** The ALSA card id is "tc358743" (from simple-audio-card,name), so the capture device can be opened as hw:CARD=tc358743,DEV=0 (or plughw:/sysdefault:CARD=tc358743) whatever card index is assigned. ASoC names the PCM '<link stream_name> <codec-dai>-<id>', where simple-card builds the stream name as '<cpu dai_name>-<codec dai_name>'. On CM4 the CPU DAI driver name is "bcm2835-i2s", giving 'bcm2835-i2s-dir-hifi dir-hifi-0'.
- **Source:** raspberrypi/linux rpi-6.18.y sound/soc/generic/simple-card.c, sound/soc/soc-core.c, sound/soc/soc-pcm.c, bcm2835-i2s.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/soc-pcm.c>
- **Evidence:** simple-card.c line 350: link name '"%s-%s", cpus->dai_name, codecs->dai_name'. soc-core.c snd_soc_dai_name_get(): 'if (dai->driver->name) return dai->driver->name;'. bcm2835-i2s.c: 'static struct snd_soc_dai_driver bcm2835_i2s_dai = { .name = "bcm2835-i2s",'. soc-pcm.c: 'snprintf(new_name, sizeof(new_name), "%s %s-%d", rtd->dai_link->stream_name, soc_codec_dai_name(rtd), rtd->id);'

### I-16

- **Verdict:** `CONFIRMED`
- **Tier:** `community` (not ranked by Rule 23 — community source (forum, issue tracker, third-party project))
- **Applies to:** CM4
- **Fact:** Raspberry Pi Forums thread t=258742 (Dec 2019, BCM2835-I2S Pi on a 2019 kernel) shows 'card 0: tc358743 [tc358743], device 0: bcm2835-i2s-dir-hifi dir-hifi-0' in 6by9's test. The original poster saw the same device as card 1, so the index varies. Capture was run with 'arecord -vv -d 20 -r 48000 -c 2 -f dat -t wav -D sysdefault:CARD=tc358743 out.wav'. 6by9 stated: 'dtoverlay=tc358743-audio *requires* dtoverlay=tc358743 to be loaded too. It can't be used independently as the tc358743 has to be configured appropriately.' The thread also shows audio_sampling_rate 0x00981980 value=48000 read-only and audio_present 0x00981981 value=1.
- **Source:** Raspberry Pi Forums: HDMI to CSI-2 TC358743 I2S Audio (t=258742) — <https://forums.raspberrypi.com/viewtopic.php?t=258742>
- **Evidence:** Forum quotes: 'card 0: tc358743 [tc358743], device 0: bcm2835-i2s-dir-hifi dir-hifi-0'; 6by9: 'dtoverlay=tc358743-audio *requires* dtoverlay=tc358743 to be loaded too. It can't be used independently as the tc358743 has to be configured appropriately.' Control readout shown: 'audio_sampling_rate: value=48000 flags=read-only / audio_present: value=1'.

### I-17

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** CM5
- **Fact:** On CM5 the PCM name will differ from CM4. dwc-i2s allocates its DAI driver with devm_kzalloc and never sets .name, and dw_i2s_component sets legacy_dai_naming = 1. The DAI name therefore falls back to the platform-device name via fmt_single_name(), which for RP1 I2S1 is '1f000a4000.i2s'. The expected PCM name is '1f000a4000.i2s-dir-hifi dir-hifi-0'. Software should select the device by card id 'tc358743', not by PCM name.
- **Source:** raspberrypi/linux rpi-6.18.y sound/soc/dwc/dwc-i2s.c and sound/soc/soc-core.c (fmt_single_name) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/dwc/dwc-i2s.c>
- **Verifier's best source:** <https://forums.raspberrypi.com/viewtopic.php?t=391090>
- **Evidence:** dwc-i2s.c: 'dw_i2s_dai = devm_kzalloc(...)' with no name assignment, and 'static const struct snd_soc_component_driver dw_i2s_component = { .name = "dw-i2s", ... .legacy_dai_naming = 1, };'. soc-core.c fmt_single_name() returns dev_name(dev) when the driver name is not a substring and the name is not '%x-%x'. The exact RP1 device name was not verified on hardware.

### I-18

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** CM4, CM5
- **Fact:** The kernel has no path that carries HDMI sample-rate changes into ALSA. spdif-dir has no controls or hw_params and accepts 8-768 kHz. The CPU I2S (bcm2835-i2s or RP1 dwc-i2s) runs as clock consumer and ignores the requested rate. Nothing outside tc358743.c uses TC358743_CID_AUDIO_SAMPLING_RATE. If the application opens the card at 48000 Hz while the source sends 44100 Hz, frames arrive at 44.1 kHz but are labelled 48 kHz. Played at 48 kHz they run 48000/44100 = 1.0884x fast (+8.84 %, about +1.47 semitones), and the audio timeline is 44100/48000 = 0.919 of real time (8.1 % short), so A/V drift accumulates.
- **Source:** Derived from spdif_receiver.c, bcm2835-i2s.c, dwc-i2s.c, tc358743.c (rpi-6.18.y) — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/sound/soc/codecs/spdif_receiver.c>
- **Evidence:** Inputs: no rate control in spdif-dir; bcm2835-i2s programs a clock only when bit_clock_provider; TC358743 is I2S master per datasheet. Calculation: 48000/44100 = 1.0884.

### I-19

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** tc358743.c get_audio_sampling_rate() decodes FS_SET (0x8621) & MASK_FS (0x0f) through code_to_rate[] = {44100, 0, 48000, 32000, 22050, 384000, 24000, 352800, 88200, 768000, 96000, 705600, 176400, 0, 192000, 0}. It returns 0 when no_signal() is true, i.e. when SYS_STATUS lacks MASK_S_TMDS, with the comment 'Register FS_SET is not cleared when the cable is disconnected'. audio_present() reads AU_STATUS0 (0x8523) & MASK_S_A_SAMPLE (0x01).
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c and tc358743_regs.h — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** 'static const int code_to_rate[] = { 44100, 0, 48000, 32000, 22050, 384000, 24000, 352800, 88200, 768000, 96000, 705600, 176400, 0, 192000, 0 }; /* Register FS_SET is not cleared when the cable is disconnected */ if (no_signal(sd)) return 0; return code_to_rate[i2c_rd8(sd, FS_SET) & MASK_FS];'. regs: '#define FS_SET 0x8621', '#define MASK_FS 0x0f', '#define AU_STATUS0 0x8523', '#define MASK_S_A_SAMPLE 0x01'.

### I-20

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** TC358743_CID_AUDIO_SAMPLING_RATE = V4L2_CID_USER_TC358743_BASE + 0 and TC358743_CID_AUDIO_PRESENT = base + 1. The base is V4L2_CID_USER_BASE + 0x1080, and V4L2_CID_USER_BASE = V4L2_CID_BASE = (V4L2_CTRL_CLASS_USER | 0x900) = 0x00980900, so the IDs are 0x00981980 and 0x00981981. 'Audio sampling rate' is a read-only INTEGER, range 0-768000, step 1. 'Audio present' is a read-only BOOLEAN.
- **Source:** raspberrypi/linux rpi-6.18.y include/media/i2c/tc358743.h, include/uapi/linux/v4l2-controls.h, drivers/media/i2c/tc358743.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/include/media/i2c/tc358743.h>
- **Evidence:** tc358743.h: '#define TC358743_CID_AUDIO_SAMPLING_RATE (V4L2_CID_USER_TC358743_BASE + 0)', '#define TC358743_CID_AUDIO_PRESENT (V4L2_CID_USER_TC358743_BASE + 1)'. v4l2-controls.h: '#define V4L2_CID_USER_TC358743_BASE (V4L2_CID_USER_BASE + 0x1080)'. tc358743.c: '.name = "Audio sampling rate", .type = V4L2_CTRL_TYPE_INTEGER, .min = 0, .max = 768000, ... .flags = V4L2_CTRL_FLAG_READ_ONLY'. Hex arithmetic: 0x980900 + 0x1080 = 0x981980.

### I-21

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** The tc358743 driver updates the sampling-rate control when the CBIT interrupt has MASK_I_CBIT_FS set, and the audio-present control on MASK_I_AF_LOCK or MASK_I_AF_UNLOCK. These CBIT interrupts are unmasked only while +5V/cable is detected. tc358743_subscribe_event() accepts V4L2_EVENT_CTRL (and V4L2_EVENT_SOURCE_CHANGE). Userspace can therefore get a control-change event on a rate change and reopen the ALSA stream at the new rate.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** 'if (cbit_int & MASK_I_CBIT_FS) { v4l2_dbg(1, debug, sd, "%s: Audio sample rate changed\n", __func__); tc358743_s_ctrl_audio_sampling_rate(sd);' ... 'if (cbit_int & (MASK_I_AF_LOCK | MASK_I_AF_UNLOCK)) { ... tc358743_s_ctrl_audio_present(sd);'. tc358743_subscribe_event(): 'case V4L2_EVENT_CTRL: return v4l2_ctrl_subdev_subscribe_event(sd, fh, sub);'

### I-22

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** The shared tc358743.dtsi used by tc358743 and tc358743-pi5 has no 'interrupts' property, so the driver uses a poll timer every POLL_INTERVAL_MS = 1000 ms. It polls every 10 ms (POLL_INTERVAL_CEC_MS) only when a CEC adapter exists, and CONFIG_VIDEO_TC358743_CEC is not set in either defconfig or in the packaged 6.18.50 rpi-v8 and rpi-2712 kernels. A source sample-rate change can therefore take up to about 1 s, plus I2C time, to reach the V4L2 control.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/tc358743.dtsi, drivers/media/i2c/tc358743.c, arm64 defconfigs — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743.dtsi>
- **Evidence:** tc358743.dtsi node 'tc358743@f { compatible = "toshiba,tc358743"; reg = <0x0f>; clocks = <&cam1_clk>; ... }' has no interrupts. tc358743.c: '#define POLL_INTERVAL_CEC_MS 10', '#define POLL_INTERVAL_MS 1000', 'if (state->i2c_client->irq) { ... } else { ... timer_setup(&state->timer, tc358743_irq_poll_timer, 0);', 'msecs = state->cec_adap ? POLL_INTERVAL_CEC_MS : POLL_INTERVAL_MS;'. grep finds no TC358743_CEC in bcm2711/bcm2712 defconfigs.

### I-23

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** Where the audio controls appear depends on the board. On CM4, the tc358743 overlay's fragment@100 sets csi1 compatible to "brcm,bcm2835-unicam-legacy" unless media-controller=on (default off). That gives mc_api = false, and unicam then calls v4l2_ctrl_add_handler() to copy the sensor's controls onto /dev/videoN. On CM5, the rp1-cfe driver bound by DT (compatible "raspberrypi,rp1-cfe") never calls v4l2_ctrl_add_handler(). It registers subdev nodes, so the tc358743 controls are only on the tc358743 /dev/v4l-subdevN.
- **Source:** raspberrypi/linux rpi-6.18.y tc358743-overlay.dts, drivers/media/platform/bcm2835/bcm2835-unicam.c, drivers/media/platform/raspberrypi/rp1_cfe/cfe.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/bcm2835/bcm2835-unicam.c>
- **Evidence:** tc358743-overlay.dts: 'legacy_frag: fragment@100 { target = <&csi1>; __overlay__ { compatible = "brcm,bcm2835-unicam-legacy"; }; }; __overrides__ { media-controller = <0>,"!100";'. unicam: 'if (!unicam->mc_api) { /* Add controls from the subdevice */ ret = v4l2_ctrl_add_handler(&unicam->ctrl_handler, unicam->sensor->ctrl_handler, NULL, true);'. cfe.c has no v4l2_ctrl_add_handler call (grep).

### I-24

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** tc358743_set_hdmi_audio() is called only from tc358743_initial_setup(), which runs once at probe (line 2267). It hard-codes I2S output: SDO_MODE1 = MASK_SDO_FMT_I2S; CONFCTL |= MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S | MASK_AUTOINDEX; FS_IMODE = MASK_NLPCM_SMODE | MASK_FS_SMODE; ACR_MODE = MASK_CTS_MODE; BUFINIT_START = 500 ms; FS_MUTE = 0x00. It also sets auto-mute/auto-play masks, ACR_MDF0/1 limits and DIV_MODE delay 100 ms. tc358743_regs.h also defines MASK_AUDOUTSEL_TDM (0x18), MASK_AUDOUTSEL_CSI (0x00) and MASK_AUDCHNUM_4/6/8, but the driver never selects them.
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/i2c/tc358743.c and tc358743_regs.h — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/i2c/tc358743.c>
- **Evidence:** 'i2c_wr8(sd, BUFINIT_START, SET_BUFINIT_START_MS(500)); ... i2c_wr8(sd, FS_IMODE, MASK_NLPCM_SMODE | MASK_FS_SMODE); i2c_wr8(sd, ACR_MODE, MASK_CTS_MODE); ... i2c_wr8(sd, SDO_MODE1, MASK_SDO_FMT_I2S); ... i2c_wr16_and_or(sd, CONFCTL, 0xffff, MASK_AUDCHNUM_2 | MASK_AUDOUTSEL_I2S | MASK_AUTOINDEX);'. regs: MASK_AUDCHNUM_8 0x0000, _6 0x0400, _4 0x0800, _2 0x0c00; MASK_AUDOUTSEL_CSI 0x0000, _I2S 0x0010, _TDM 0x0018. tc358743_initial_setup() is called at probe (line 2267).

### I-25

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** CM4, CM5
- **Fact:** The TC358743XBG public datasheet (Rev. 1.0, 2017-10-26, Features, page 7/18) lists the I2S output as a single data lane for stereo, 'Support Master Clock mode only', 16/18/20/24-bit data 'depend on HDMI input stream', 'Left or Right-justify with MSB first', 'Support 32 bit-wide time-slot only', and a 256fs oversampling clock output. The TDM output is 'Fixed to 8 channels', 32-bit slots, master-clock only, with 16/18/20/24-bit PCM. I2S and TDM pins are multiplexed.
- **Source:** Toshiba TC358743XBG datasheet Rev. 1.0, 2017-10-26 (Features) — <https://jlcpcb.com/api/file/downloadByFileSystemAccessId/8588919017658585088>
- **Evidence:** Page 7: 'Audio Output Interface: Either I2S or TDM Audio interface available (pins are multiplexed). I2S Audio Interface: Single data lane for stereo data; Support Master Clock mode only; Support 16, 18, 20 or 24-bit data (depend on HDMI input stream); Support Left or Right-justify with MSB first; Support 32 bit-wide time-slot only; Output Audio Over sampling clock (256fs). TDM ...: Fixed to 8 channels (depend on HDMI input stream); Support 32 bit-wide time slot only; Support Master Clock mode only ...'

### I-26

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** CM4, CM5
- **Fact:** Per its datasheet, the TC358743XBG has an 'Internal Audio PLL to track N/CTS value transmitted by the ACR packet', so its I2S clocks follow the HDMI source's audio clock. The datasheet also says 'Video, Audio and InfoFrame data can be transmit over MIPI CSI-2', but the Linux driver selects I2S output (MASK_AUDOUTSEL_I2S), not CSI.
- **Source:** Toshiba TC358743XBG datasheet Rev. 1.0, 2017-10-26 (Features) — <https://jlcpcb.com/api/file/downloadByFileSystemAccessId/8588919017658585088>
- **Evidence:** Page 7: 'Audio Supports: Internal Audio PLL to track N/CTS value transmitted by the ACR packet.' and 'CSI-2 TX Interface ... Video, Audio and InfoFrame data can be transmit over MIPI CSI-2'.

### I-27

- **Verdict:** `CONFIRMED`
- **Tier:** `datasheet` (Rule 23 priority 2 — official hardware datasheet)
- **Applies to:** CM4, CM5
- **Fact:** TC358743XBG has four audio pins, all outputs: A_SCK (I2S/TDM bit clock), A_WFS (I2S word clock or TDM frame sync), A_SD (I2S/TDM data) and A_OSCK (oversampling clock). They are powered from VDDIO2, which is rated 1.8-3.3 V (recommended 1.65-3.6 V). The balls are F7 = A_SCK, F8 = A_SD, G7 = A_WFS and G8 = A_OSCK.
- **Source:** Toshiba TC358743XBG datasheet Rev. 1.0, Table 3.1 and Figure 3.1 — <https://jlcpcb.com/api/file/downloadByFileSystemAccessId/8588919017658585088>
- **Evidence:** Table 3.1: 'Audio (4) A_SCK O L N I2S/TDM Bit Clock signal VDDIO2 1.8V-3.3V; A_WFS O L N I2S Word Clock or TDM Frame Sync signal VDDIO2 1.8V-3.3V; A_SD O L N I2S/TDM data signal VDDIO2 1.8V-3.3V; A_OSCK O L N Audio Oversampling Clock VDDIO2 1.8V-3.3V'. Pin layout row F: '... A_SCK A_SD'; row G: '... A_WFS A_OSCK'.

### I-28

- **Verdict:** `CONFIRMED`
- **Tier:** `reasoning` (Rule 23 priority 8 — reasoning/calculation from cited inputs)
- **Applies to:** CM4, CM5
- **Fact:** The overlay and the datasheet agree on clock roles. The datasheet makes the TC358743 the I2S clock master only. The overlay sets bitclock-master and frame-master to the codec link, which makes the Pi the clock consumer (BC_FC), and binds the CPU DAI through i2s_clk_consumer. On CM4 that label is the single bidirectional bcm2835-i2s, which switches to slave via CLKM/FSM. On CM5 it is RP1 I2S1, which the RP1 datasheet calls the 'clock-consumer (slave)' instance. The overlay's 2 x 32-bit slots match the datasheet's '32 bit-wide time-slot only': 64 BCK per LRCK frame (64fs).
- **Source:** Comparison of tc358743-audio-overlay.dts, bcm270x-rpi.dtsi, bcm2712-rpi.dtsi and the TC358743XBG datasheet — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts>
- **Evidence:** Inputs: I-03 (bitclock-master/frame-master = codec; tdm 2 x 32), I-07 / I-10 (label mapping), I-09 (dwc BC_FC needs slave HW), I-25 (Master Clock mode only; 32-bit slots). Calculation: 2 slots x 32 bits = 64 BCK/frame = 64fs.

### I-29

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5
- **Fact:** The CM4 IO Board and the CM5 IO Board documentation both list a 'HAT footprint with 40-pin GPIO connector' and 'Selectable 1.8 V or 3.3 V GPIO voltage'. The TC358743's VDDIO2 (I-27) should match the selected GPIO bank voltage, or the audio lines need level shifting.
- **Source:** Raspberry Pi documentation, compute-module/introduction.adoc (CM5IO and CM4IO feature lists) — <https://github.com/raspberrypi/documentation/blob/master/documentation/asciidoc/computers/compute-module/introduction.adoc>
- **Evidence:** CM5IO and CM4IO lists both contain '** HAT footprint with 40-pin GPIO connector.' and '** Selectable 1.8 V or 3.3 V GPIO voltage.'

### I-30

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** GPIO18-GPIO21 are named lines on the CM4 SoC GPIO bank (bcm2711-rpi-cm4.dts gpio-line-names) and on the CM5 RP1 bank (bcm2712-rpi-cm5.dtsi &rp1_gpio). When tc358743-audio is enabled, the I2S pinctrl claims all four, including GPIO21 (DOUT/SDO0), although the TC358743 path uses only 18, 19 and 20. On CM4 the pins are set to ALT0 (i2s_pins <18 19 20 21>). On CM5 they use function "i2s1" (rp1_i2s1_18_21).
- **Source:** raspberrypi/linux rpi-6.18.y bcm2711-rpi-cm4.dts, bcm2711-rpi-ds.dtsi, bcm2712-rpi-cm5.dtsi, rp1.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5.dtsi>
- **Evidence:** CM4 gpio-line-names include "GPIO18" ... "GPIO21"; i2s_pins brcm,pins = <18 19 20 21> ALT0. CM5 '&rp1_gpio { gpio-line-names = ... "GPIO18", // GPIO18 "GPIO19", ... "GPIO21"'. rp1_i2s1_18_21 pins gpio18-21.

### I-31

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5
- **Fact:** Several overlays default to GPIO 18-21 and conflict with I2S. The pwm and pwm-2chan overlays default to pin 18, and the README notes that pin 18 'is the one used by the I2S audio interface'. gpio-ir defaults to gpio_pin 18. audremap offers pins_18_19 on bcm2835/bcm2711, but on bcm2712 overlay_map redirects audremap to audremap-pi5, where the README says 'pins_18_19 Not available; this will not enable audio out'. gpio-fan defaults to gpiopin 12 and does not conflict.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm/boot/dts/overlays/README — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm/boot/dts/overlays/README>
- **Evidence:** pwm: 'Pin 18 is the only one available on all platforms, and it is the one used by the I2S audio interface' / 'pin Output pin (default 18)'. gpio-ir: 'gpio_pin Input pin number. Default is 18.' audremap: 'pins_18_19 Select GPIOs 18 & 19'. audremap-pi5: 'pins_18_19 Not available; this will not enable audio out'. gpio-fan: 'gpiopin GPIO used to control the fan (default 12)'.

### I-32

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM5
- **Fact:** On CM5, the base DT's GPIO-20 and fan entries do not touch header GPIO 18-21. The power button is gpio-keys on '&gio 20' with pwr_button_pins in &pinctrl, both on the BCM2712 SoC GPIO controller, not RP1 header GPIO20. The cooling_fan node (status disabled by default in the CM5 dtsi) uses pwms = <&rp1_pwm1 3 41566 PWM_POLARITY_INVERTED>. The rp1_gpio line names put FAN_TACH on GPIO29 and FAN_PWM on GPIO45.
- **Source:** raspberrypi/linux rpi-6.18.y arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5.dtsi — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5.dtsi>
- **Evidence:** '&pinctrl { pwr_button_pins: pwr_button_pins { function = "gpio"; pins = "gpio20"; bias-pull-up; };' and 'pwr_key: pwr { ... gpios = <&gio 20 GPIO_ACTIVE_LOW>;'. 'fan: cooling_fan { ... pwms = <&rp1_pwm1 3 41566 PWM_POLARITY_INVERTED>;'. rp1_gpio line names: '"FAN_TACH", // GPIO29', '"FAN_PWM", // GPIO45'.

### I-33

- **Verdict:** `CONFIRMED`
- **Tier:** `kernel-source` (Rule 23 priority 4 — Linux kernel source)
- **Applies to:** CM4, CM5
- **Fact:** The Raspberry Pi CSI receiver drivers in rpi-6.18.y all set q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC: legacy/MC bcm2835-unicam, the upstream-style broadcom/bcm2835-unicam, the DT-bound downstream rp1_cfe ('raspberrypi,rp1-cfe') and the upstream rp1-cfe ('raspberrypi,rp1-cfe-upstream'). Each stamps the buffer with ktime_get_ns() (CLOCK_MONOTONIC) in its frame-start interrupt handler (UNICAM_FSI or cfe_sof_isr_handler).
- **Source:** raspberrypi/linux rpi-6.18.y drivers/media/platform/bcm2835/bcm2835-unicam.c, drivers/media/platform/broadcom/bcm2835-unicam.c, drivers/media/platform/raspberrypi/rp1_cfe/cfe.c — <https://github.com/raspberrypi/linux/blob/rpi-6.18.y/drivers/media/platform/raspberrypi/rp1_cfe/cfe.c>
- **Evidence:** bcm2835-unicam.c: 'if (ista & UNICAM_FSI) { /* Timestamp is to be when the first data byte was captured, aka frame start. */ ts = ktime_get_ns(); ... cur_frm->vb.vb2_buf.timestamp = ts;' and 'q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;'. cfe.c: 'node->ts = ktime_get_ns(); ... node->cur_frm->vb.vb2_buf.timestamp = node->ts;' and 'q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;'

### I-34

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** ALSA PCM status reports avail/delay with a system-time snapshot whose clock the application picks through sw_params: CLOCK_REALTIME, CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW. alsa-lib's hw plugin (v1.2.14, the version in trixie, and master) switches each newly opened PCM to SNDRV_PCM_TSTAMP_TYPE_MONOTONIC via the SNDRV_PCM_IOCTL_TTSTAMP ioctl when the kernel PCM protocol is 2.0.9 or later. Opens in SND_PCM_APPEND mode are not switched.
- **Source:** Linux Documentation/sound/designs/timestamping.rst (rpi-6.18.y); alsa-lib src/pcm/pcm_hw.c — <https://github.com/alsa-project/alsa-lib/blob/master/src/pcm/pcm_hw.c>
- **Verifier's best source:** <https://github.com/alsa-project/alsa-lib/blob/v1.2.14/src/pcm/pcm_hw.c>
- **Evidence:** timestamping.rst: 'Applications can select from CLOCK_REALTIME (NTP corrections including going backwards), CLOCK_MONOTONIC (NTP corrections but never going backwards), CLOCK_MONOTIC_RAW (without NTP corrections) and change the mode dynamically with sw_params'. pcm_hw.c: 'if (SNDRV_PROTOCOL_VERSION(2, 0, 9) <= ver) { ... int on = SNDRV_PCM_TSTAMP_TYPE_MONOTONIC; if (ioctl(fd, SNDRV_PCM_IOCTL_TTSTAMP, &on) < 0) ... tstamp_type = SND_PCM_TSTAMP_TYPE_MONOTONIC;' (alsa-lib master; trixie's alsa-lib version not checked).

### I-35

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** GStreamer 1.26.2 alsasrc sets tstamp mode SND_PCM_TSTAMP_MMAP. On PAUSED->PLAYING it uses driver timestamps only if use-driver-timestamps is TRUE (the default) and the element clock's exact GType is GstSystemClock with clock-type MONOTONIC. In that case each timestamp is snd_pcm_status htstamp minus avail/rate minus one period_time. GstAudioBaseSrc defaults are provide-clock=TRUE, slave-method=skew, buffer-time 200 ms and latency-time 10 ms.
- **Source:** GStreamer 1.26.2 subprojects/gst-plugins-base/ext/alsa/gstalsasrc.c and gst-libs/gst/audio/gstaudiobasesrc.c — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/1.26.2/subprojects/gst-plugins-base/ext/alsa/gstalsasrc.c>
- **Evidence:** gstalsasrc.c: 'if (G_OBJECT_TYPE (clk) == GST_TYPE_SYSTEM_CLOCK) { ... if (clocktype == GST_CLOCK_TYPE_MONOTONIC && alsa->use_driver_timestamps) { GST_INFO ("Using driver timestamps !"); alsa->driver_timestamps = TRUE;'; 'snd_pcm_status_get_htstamp (status, &tstamp); ... timestamp -= gst_util_uint64_scale_int (avail, GST_SECOND, asrc->rate); timestamp -= asrc->period_time * 1000;'. gstaudiobasesrc.c: '#define DEFAULT_BUFFER_TIME ((200 * GST_MSECOND) / GST_USECOND)', '#define DEFAULT_LATENCY_TIME ((10 * GST_MSECOND) / GST_USECOND)', '#define DEFAULT_PROVIDE_CLOCK TRUE', '#define DEFAULT_SLAVE_METHOD GST_AUDIO_BASE_SRC_SLAVE_SKEW'.

### I-36

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** GStreamer documentation says that on PLAYING the pipeline asks elements from sink to source whether they can provide a clock, and uses the last one that can. This 'prefers ... a clock from source elements in a typical capture pipeline'. v4l2src does not provide a clock, so in a v4l2src + alsasrc pipeline alsasrc's GstAudioClock (provide-clock=TRUE) normally becomes the pipeline clock. alsasrc then does not use ALSA driver timestamps, because the clock is not a GstSystemClock (I-35).
- **Source:** GStreamer Application Development Manual: Clocks and synchronization — <https://gstreamer.freedesktop.org/documentation/application-development/advanced/clocks.html>
- **Evidence:** 'When the pipeline goes to the PLAYING state, it will go over all elements in the pipeline from sink to source and ask each element if they can provide a clock. The last element that can provide a clock will be used as the clock provider in the pipeline. This algorithm prefers a clock from an audio sink in a typical playback pipeline and a clock from source elements in a typical capture pipeline.'

### I-37

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** GStreamer 1.26.2 v4l2src computes delay = CLOCK_MONOTONIC now minus the V4L2 buffer timestamp. If the timestamp is in the future or more than 10 s old, it compares against g_get_real_time() instead. It then sets PTS = (pipeline clock time - base_time) - delay. If the driver timestamp is in the future, goes backwards, or the delay exceeds the timestamp, it sets has_bad_timestamp and assumes a one-frame delay for the rest of the session; start() resets the flag.
- **Source:** GStreamer 1.26.2 subprojects/gst-plugins-good/sys/v4l2/gstv4l2src.c — <https://gitlab.freedesktop.org/gstreamer/gstreamer/-/blob/1.26.2/subprojects/gst-plugins-good/sys/v4l2/gstv4l2src.c>
- **Evidence:** 'clock_gettime (CLOCK_MONOTONIC, &now); gstnow = GST_TIMESPEC_TO_TIME (now); if (timestamp > gstnow || (gstnow - timestamp) > (10 * GST_SECOND)) { /* very large diff, fall back to system time */ gstnow = g_get_real_time () * GST_USECOND; } ... delay = gstnow - timestamp; ... timestamp = abs_time - base_time; /* adjust for delay in the device */ if (timestamp > delay) timestamp -= delay;'

### I-38

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** In FFmpeg n7.1 the two capture devices use different clocks. The alsa input device stamps packets with av_gettime() (wall clock) minus (snd_pcm_delay + frames read)/sample_rate, smoothed by ff_timefilter, in microseconds. The v4l2 input device by default (-ts/-timestamps default) passes the kernel's monotonic buffer timestamps through unchanged; 'abs' autodetects and converts to wall clock, and 'mono2abs' forces the monotonic-to-wall-clock conversion. Mixing -f v4l2 and -f alsa without -ts abs/mono2abs therefore mixes clock bases. The ffmpeg CLI hides this by default by shifting each input to start at 0 (ts_offset = -start_time unless -copyts), which also discards the real start offset between the two inputs.
- **Source:** FFmpeg n7.1 libavdevice/alsa_dec.c and libavdevice/v4l2.c — <https://github.com/FFmpeg/FFmpeg/blob/n7.1/libavdevice/alsa_dec.c>
- **Evidence:** alsa_dec.c: 'dts = av_gettime(); snd_pcm_delay(s->h, &delay); dts -= av_rescale(delay + res, 1000000, s->sample_rate); pkt->pts = ff_timefilter_update(s->timefilter, dts, s->last_period);'. v4l2.c: '{ "timestamps", ... {.i64 = 0 } ...}', '{ "default", "use timestamps from the kernel" ...}', '{ "abs", "use absolute timestamps (wall clock)" ...}', '{ "mono2abs", "force conversion from monotonic to absolute timestamps" ...}'.

### I-39

- **Verdict:** `CONFIRMED`
- **Tier:** `official-rpi` (Rule 23 priority 3 — official Raspberry Pi documentation)
- **Applies to:** CM4, CM5
- **Fact:** In the Raspberry Pi trixie arm64 archive (Release dated Wed, 07 Oct 2026 19:33:14 UTC), ffmpeg and libavcodec61 are both 8:7.1.5-0+deb13u1+rpt2. libavcodec61 depends on libopus0 (>= 1.1), libmp3lame0, libx264-164 and libx265-215, and not on libfdk-aac. No fdk package exists in that archive, and libavcodec-extra61 has no fdk-aac either. The Pi build therefore has the libopus wrapper and the native 'aac' encoder but no libfdk_aac. Linking libx264 and libx265, which are on FFmpeg's EXTERNAL_LIBRARY_GPL_LIST, makes it a GPL build.
- **Source:** archive.raspberrypi.com debian dists/trixie/main/binary-arm64/Packages — <http://archive.raspberrypi.com/debian/dists/trixie/main/binary-arm64/Packages.gz>
- **Evidence:** 'Package: libavcodec61 / Source: ffmpeg / Version: 8:7.1.5-0+deb13u1+rpt2 / Depends: ... libmp3lame0 (>= 3.100), ... libopus0 (>= 1.1), ... libx264-164 (>= 2:0.164.3108+git31e19f9), libx265-215 (>= 4.1), ...'. No libfdk-aac in the Depends list. 'Package: ffmpeg / Version: 8:7.1.5-0+deb13u1+rpt2'.

### I-40

- **Verdict:** `CORRECTED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** FFmpeg 7.1's native 'aac' encoder is documented as the default AAC encoder and is not flagged AV_CODEC_CAP_EXPERIMENTAL. It takes AV_SAMPLE_FMT_FLTP at the MPEG-4 audio rates (ff_mpeg4audio_sample_rates). Its codec default is b=0. When -b is not given, aac_encode_init() sums a bitrate per channel element: 128000 per channel pair, 69000 per single channel, 16000 per LFE. Stereo capture therefore defaults to 128 kb/s and mono to 69 kb/s. encoders.texi says an explicit -b 'automatically activates constant bit rate (CBR) mode'.
- **Source:** FFmpeg n7.1 libavcodec/aacenc.c and doc/encoders.texi — <https://github.com/FFmpeg/FFmpeg/blob/n7.1/doc/encoders.texi>
- **Verifier's best source:** <https://github.com/FFmpeg/FFmpeg/blob/n7.1/libavcodec/aacenc.c>
- **Evidence:** encoders.texi: '@section aac / Advanced Audio Coding (AAC) encoder. This encoder is the default AAC encoder, natively implemented into FFmpeg. ... @item b Set bit rate in bits/s. Setting this automatically activates constant bit rate (CBR) mode. If this option is unspecified it is set to 128kbps.' aacenc.c: '.p.capabilities = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_DELAY | AV_CODEC_CAP_SMALL_LAST_FRAME, ... .p.supported_samplerates = ff_mpeg4audio_sample_rates, ... .p.sample_fmts = { AV_SAMPLE_FMT_FLTP, ...'
- **Original claim (before verification):** FFmpeg 7.1's native 'aac' encoder is the default AAC encoder and is not flagged experimental. It takes AV_SAMPLE_FMT_FLTP input at the MPEG-4 audio sample rates (ff_mpeg4audio_sample_rates), and its bitrate defaults to 128 kbps CBR when -b is not given.
- **Verifier note:** encoders.texi line 40 says 'If this option is unspecified it is set to 128kbps', but code lines 1279-1285 and 1414-1416 show the default depends on the channel layout. For PACSCORDER's 2-channel capture the result is still 128 kb/s.

### I-41

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** FFmpeg 7.1's native 'opus' encoder is flagged AV_CODEC_CAP_EXPERIMENTAL, implements only the CELT part of Opus, supports only 48000 Hz, and supports mono or stereo only. Production Opus encoding should use the 'libopus' wrapper, which needs --enable-libopus at build time, accepts 48000/24000/16000/12000/8000 Hz, and is present in the Raspberry Pi build because libavcodec61 depends on libopus0.
- **Source:** FFmpeg n7.1 libavcodec/opus/enc.c and doc/encoders.texi — <https://github.com/FFmpeg/FFmpeg/blob/n7.1/libavcodec/opus/enc.c>
- **Evidence:** enc.c: '.p.capabilities = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_DELAY | AV_CODEC_CAP_SMALL_LAST_FRAME | AV_CODEC_CAP_EXPERIMENTAL, ... .p.supported_samplerates = (const int []){ 48000, 0 },'. encoders.texi: 'This is a native FFmpeg encoder for the Opus format. Currently, it's in development and only implements the CELT part of the codec. Its quality is usually worse and at best is equal to the libopus encoder.' ffmpeg-codecs: 'libopus ... You need to explicitly configure the build with --enable-libopus.'

### I-42

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** In FFmpeg 7.1's configure, libfdk_aac is on EXTERNAL_LIBRARY_NONFREE_LIST (with decklink and libtls), and 'enabled gpl && map "die_license_disabled_gpl nonfree"' applies to that list. A --enable-gpl build (needed for x264/x265) therefore needs --enable-nonfree to enable libfdk_aac. --enable-nonfree is described as making 'the resulting libs and binaries ... unredistributable', and configure then reports the license as 'nonfree and unredistributable'. An LGPL build (no --enable-gpl) can enable libfdk_aac without --enable-nonfree.
- **Source:** FFmpeg n7.1 configure; FFmpeg Codecs Documentation (libfdk_aac) — <https://github.com/FFmpeg/FFmpeg/blob/n7.1/configure>
- **Evidence:** configure: 'EXTERNAL_LIBRARY_NONFREE_LIST=" decklink libfdk_aac libtls "'; 'enabled gpl && map "die_license_disabled_gpl nonfree" $EXTERNAL_LIBRARY_NONFREE_LIST'; '--enable-nonfree allow use of nonfree code, the resulting libs and binaries will be unredistributable [no]'. Docs: 'The library is also incompatible with GPL, so if you allow the use of GPL, you should configure with --enable-gpl --enable-nonfree --enable-libfdk-aac.'

### I-43

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** Debian trixie ships fdk-aac 2.0.3-1 in non-free (Section non-free/libs), with binary packages libfdk-aac2t64 (arm64 565.3 kB, also armhf), libfdk-aac-dev and aac-enc. The licence is 'Fraunhofer-FDK-AAC-for-Android'. Debian's copyright file says it 'is incompatible with any version of the GNU GPL'. The licence's clause 3 grants 'NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS' and points to Via Licensing (now Via LA) or the patent owners for patent licences.
- **Source:** packages.debian.org trixie fdk-aac / libfdk-aac2t64; Debian copyright file fdk-aac_2.0.3-1 — <https://packages.debian.org/source/trixie/fdk-aac>
- **Evidence:** 'Source Package: fdk-aac (2.0.3-1) [non-free] ... binary packages: aac-enc, libfdk-aac-dev, libfdk-aac2t64'. 'Package: libfdk-aac2t64 (2.0.3-1) [non-free]' with arm64 565.3 kB download. Copyright: 'License: Fraunhofer-FDK-AAC-for-Android'; 'It is incompatible with any version of the GNU GPL'; '3. NO PATENT LICENSE ... NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS ... ARE GRANTED'; 'Patent licenses ... may be obtained through Via Licensing'.

### I-44

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** The Raspberry Pi archive's gstreamer1.0-plugins-bad 1.26.2-3+rpt4+deb13u3 (arm64) depends on libvo-aacenc0 (>= 0.1.3) and libopus0 (>= 1.1), and not on libfdk-aac. Its .deb, like Debian's 1.26.2-3+deb13u3, contains libgstvoaacenc.so and libgstopusparse.so but no libgstfdkaac.so. voaacenc is therefore available; fdkaacenc is not shipped and would need a custom build linked against non-free libfdk-aac.
- **Source:** archive.raspberrypi.com trixie Packages; packages.debian.org trixie/arm64 gstreamer1.0-plugins-bad file list — <https://packages.debian.org/trixie/arm64/gstreamer1.0-plugins-bad/filelist>
- **Evidence:** RPi Packages: 'Package: gstreamer1.0-plugins-bad / Source: gst-plugins-bad1.0 / Version: 1.26.2-3+rpt4+deb13u3 / Depends: ... libopus0 (>= 1.1), ... libvo-aacenc0 (>= 0.1.3), ...'. Debian file list includes '/usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstvoaacenc.so' and no fdkaac entry. Debian: 'Package: libvo-aacenc0 (0.1.3-3)' in main.

### I-45

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** The GStreamer Opus encoder (opusenc, plugin 'opus') is in gst-plugins-base. The Raspberry Pi archive ships gstreamer1.0-plugins-base 1.26.2-1+rpt3+deb13u2, which depends on libopus0 (>= 1.1) and contains /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstopus.so. alsasrc comes from gstreamer1.0-alsa 1.26.2-1+rpt3+deb13u2. Debian trixie's libopus0 is 1.5.2-2, and Raspberry Pi does not rebuild it.
- **Source:** archive.raspberrypi.com trixie Packages; packages.debian.org trixie gstreamer1.0-plugins-base, libopus0 — <https://packages.debian.org/trixie/libopus0>
- **Evidence:** RPi: 'Package: gstreamer1.0-plugins-base / Version: 1.26.2-1+rpt3+deb13u2 / Depends: ... libopus0 (>= 1.1) ...'; 'Package: gstreamer1.0-alsa / Version: 1.26.2-1+rpt3+deb13u2'. Debian file list: '/usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstopus.so'. 'Package: libopus0 (1.5.2-2)'. GStreamer docs: 'opusenc ... Plugin – opus, Package – GStreamer Base Plug-ins'.

### I-46

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** Raspberry Pi does not rebuild gstreamer1.0-libav, which provides avenc_aac wrapping FFmpeg's native AAC encoder. The package comes from Debian trixie as 1.26.2-1+deb13u1, ships libgstlibav.so, and depends on libavcodec61 (>= 7:7.1.4). The Raspberry Pi libavcodec61 8:7.1.5-0+deb13u1+rpt2 satisfies that and wins on epoch (8 > 7).
- **Source:** packages.debian.org trixie gstreamer1.0-libav; archive.raspberrypi.com trixie Packages — <https://packages.debian.org/trixie/gstreamer1.0-libav>
- **Evidence:** Debian: 'Package: gstreamer1.0-libav (1.26.2-1+deb13u1)'; deps 'libavcodec61 (>= 7:7.1.4)'; file list '/usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstlibav.so'. No gstreamer1.0-libav package in the RPi trixie arm64 Packages index. GStreamer docs: avenc_aac 'Plugin – libav, Package – GStreamer FFMPEG Plug-ins'.

### I-47

- **Verdict:** `CONFIRMED`
- **Tier:** `vendor-other` (not ranked by Rule 23 — official documentation of another vendor or standards body)
- **Applies to:** CM4, CM5
- **Fact:** GStreamer encoders accept different sample rates on their sink pads. opusenc accepts only 48000, 24000, 16000, 12000 or 8000 Hz (F32LE/S16LE, 1-255 channels) and defaults to bitrate 64000 with bitrate-type constrained-vbr. avenc_aac accepts F32LE at 7350-96000 Hz including 44100 and 48000, with 1-16 channels. voaacenc accepts S16LE at 8000-96000 Hz with 1 or 2 channels, rank secondary. fdkaacenc (plugin fdkaac, gst-plugins-bad) accepts S16LE at 8000-96000 Hz and offers profiles lc, he-aac-v1, he-aac-v2 and ld. A 44.1 kHz HDMI source needs audioresample before opusenc.
- **Source:** GStreamer documentation: opusenc, avenc_aac, voaacenc, fdkaacenc — <https://gstreamer.freedesktop.org/documentation/opus/opusenc.html>
- **Evidence:** opusenc sink: 'format: { F32LE, S16LE } ... rate: { (int)48000, (int)24000, (int)16000, (int)12000, (int)8000 } channels: [ 1, 255 ]'; bitrate 'Default value : 64000'; bitrate-type 'Default value : constrained-vbr (2)'. avenc_aac sink: 'rate: { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 } format: F32LE'. voaacenc sink: 'format: S16LE ... rate: {8000 ... 96000} channels: 1 / channels: 2', 'Rank – secondary'. fdkaacenc: 'profile: { lc, he-aac-v1, he-aac-v2, ld }'.
