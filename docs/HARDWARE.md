# PACSCORDER Hardware

| | |
|---|---|
| Document status | DRAFT — no PACSCORDER hardware exists. Every PACSCORDER-specific value is `UNKNOWN — VERIFICATION REQUIRED`. |
| Last updated | 2026-10-09 |
| Applies to | PACSCORDER hardware, all revisions (none defined yet). Candidate platforms: Raspberry Pi 4 Model B, Compute Module 4 (CM4), Raspberry Pi 5, Compute Module 5 (CM5). The product needs both a 2-lane and a 4-lane CSI-2 configuration (REQ-CAP-007, owner 2026-10-07; §4.11). Bring-up evaluates **CM4 and CM5 side by side** (owner, 2026-10-07); ADR-004 stays OPEN until measured. Pi 4 Model B and Pi 5 facts are kept. HDMI audio is required (REQ-CAP-006; §4.6.1). Recording storage is a PCIe NVMe SSD mirrored with a USB-to-SATA HDD in a self-powered enclosure (ADR-009, ACCEPTED 2026-10-08; §4.13.1, §4.14.1); no storage part has been chosen. |
| Verification | Source research of 2026-10-06, plus research topics H (H.265 encoding) and I (HDMI audio path) of 2026-10-08, and research topics J (recording storage and power loss) and K (live latency) of 2026-10-08 ([REFERENCES.md](REFERENCES.md)). Nothing has been verified on PACSCORDER hardware: no hardware exists as of 2026-10-08 (still none on 2026-10-09). |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 8 (hardware documentation), Rule 22 (unknown information), Rule 23 (source priority), Rule 25 (new-engineer questions) |

This document describes the **actual** PACSCORDER hardware (Rule 8). As of 2026-10-06 no PACSCORDER hardware exists, the target platform is undecided, and nothing has been tested (owner, 2026-10-06). On 2026-10-07 the owner required both a 2-lane and a 4-lane CSI-2 configuration (REQ-CAP-007) and named ATEM switchers and directly connected cameras as the HDMI sources (REQ-CAP-008). The platform for each lane configuration is still undecided (ADR-004, OPEN). In a second set of answers on 2026-10-07 the owner added four points: bring-up evaluates CM4 and CM5 side by side, and the product platform is decided from measurements (ADR-004 stays OPEN until then); HDMI audio is required (REQ-CAP-006, DRAFT; OQ-004 ANSWERED); video is encoded in both H.264 and H.265 (REQ-ENC-001; which outputs use H.265 is OQ-103); and the HDMI sources are any HDMI camera, with no model list, plus ATEM switcher outputs (OQ-102 ANSWERED). *(Superseded in part 2026-10-08: the owner answered OQ-103 with "H.264 only for now". Video is encoded in H.264 only, for all outputs (REQ-ENC-001); H.265 is deferred (REQ-ENC-002, `DEFERRED`) and is not in current scope. The other three points are unchanged.)* *(Added 2026-10-09; research topic J / ADR-009.)* On 2026-10-08 the owner also decided the recording storage: every recording is written as fragmented MP4 and mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD, and the HDD sits in a self-powered enclosure, never powered from the board's USB VBUS (ADR-009, ACCEPTED). ext4 on the recording volumes is Claude's proposal inside ADR-009, not an owner decision (OQ-120). The same day the owner set the live latency target, under 1 second camera-to-viewer for WebRTC viewers only, with RTMP outputs best-effort (REQ-STR-002; OQ-116 ANSWERED). *(Added 2026-10-09, later: on 2026-10-09 the owner set that target's criterion — judged at the 95th percentile, 95 % of samples under 1 s over a sustained run with the recording running — and limited WebRTC viewers to the LAN; internet viewers are not in current scope (OQ-008; OQ-128, RISK-033). The owner also set no fixed recording duration limit: recordings run until stopped or the disk is full (OQ-006 ANSWERED); one-drive-full or failure behaviour and file splitting: OQ-129.)* *(Added 2026-10-09, owner decisions on bitrate and drive failure: the recording encode is 25 Mbit/s VBR and the live encode CBR 17 Mbit/s (OQ-005). Reasoning from [J-36]: each recording drive then receives about 3.15 MB/s, so drive capacity sets the recording time at about 88 h per 1 TB (§4.14). If one mirrored drive fills, is absent or fails during a recording, recording continues on the drive that still works and the operator is alerted (OQ-129; file splitting, the alert method and drive return are still open).)* *(Superseded in part 2026-10-09, later still: owner decisions — a recording started with one drive missing runs on the available drive with an operator alert, and recordings are split into a new file every 30 minutes, about 5.67 GB per file per drive at 25 Mbit/s (reasoning from [J-36]; OQ-129; §4.14). Still open: drive return (OQ-129); alert method (OQ-091).)* No SSD, adaptor, HDD, enclosure or bridge has been chosen (OQ-121, OQ-122). Every subsection below therefore has two parts:

- **PACSCORDER value** — what PACSCORDER's own hardware is. It is `UNKNOWN — VERIFICATION REQUIRED` unless a decision or a measurement exists. The marker and the open question (`OQ-NNN`, see [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md)) that resolve it are named.
- **Reference facts from sources** — what the cited sources say about the TC358743 and the Raspberry Pi side. These are design constraints. They are **not** a description of PACSCORDER.

> **Rule for editors.** Do not copy a reference fact into a "PACSCORDER value" cell until it has been confirmed for the actual board, by schematic, BOM or measurement. Under Rule 23 a measurement on PACSCORDER hardware overrides every source fact. Record each confirmation with its evidence and the hardware revision (§8).

Fact IDs such as `[A-36]` point to [REFERENCES.md](REFERENCES.md). Facts from the `community` tier are worded as reports. Facts from the `reasoning` tier, and Claude's own calculations, are labelled as reasoning. `CORRECTED` entries are used in their corrected wording only.

Related documents: [DEVICE_TREE.md](DEVICE_TREE.md) describes how this hardware is declared to Linux. [CSI_PIPELINE.md](CSI_PIPELINE.md) covers the CSI-2 link, [TC358743_DRIVER.md](TC358743_DRIVER.md) the driver, and [TESTING.md](TESTING.md) the test procedures.

---

## 1. Current state

| Item | State (2026-10-07; updated 2026-10-08 and 2026-10-09) |
|---|---|
| PACSCORDER hardware | NOT STARTED. No board, PCB, bridge board or enclosure exists. |
| Target platform | Undecided among Pi 4 Model B, CM4, Pi 5 and CM5 (owner, 2026-10-06: "keep all four"). The product needs both a 2-lane and a 4-lane CSI-2 configuration (REQ-CAP-007, owner 2026-10-07), so ADR-004 is now a choice per configuration. 2-lane candidates: Pi 4 Model B, CM4 CAM0, or a 2-lane bridge board on any port. 4-lane candidates: CM4 CAM1, Pi 5, CM5 (§4.11). ADR-004 is OPEN; OQ-011. *(Owner, 2026-10-07, second answer: bring-up evaluates CM4 and CM5 side by side; the product platform is decided from the TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 results. ADR-004 stays OPEN until then. Pi 4 Model B and Pi 5 stay documented as candidates.)* |
| HDMI audio hardware path | Required (REQ-CAP-006, DRAFT; owner 2026-10-07, OQ-004 ANSWERED). NOT STARTED. The I2S wiring, the VDDIO2 versus GPIO bank voltage and the use of GPIO 18–21 are UNKNOWN — VERIFICATION REQUIRED (OQ-025, OQ-024, OQ-114). The CM5 audio path is unconfirmed (OQ-054). See §4.6.1. |
| Recording storage hardware *(added 2026-10-09)* | Architecture decided by the owner on 2026-10-08 (ADR-009, ACCEPTED): a PCIe NVMe SSD and a USB-to-SATA HDD, every recording mirrored to both, the HDD in a self-powered enclosure. NOT STARTED. SSD, CM4 PCIe adaptor, HDD, enclosure and USB-to-SATA bridge are UNKNOWN — VERIFICATION REQUIRED (OQ-121, OQ-122). CM4 PCIe interrupt mode: OQ-123; CM5 M.2 enablement and NVMe boot: OQ-124; filesystem: OQ-120. See §4.13.1 and §4.14.1. |
| TC358743 bridge | Undecided: third-party board or custom PCB. OQ-018. Whether one board design can serve both lane configurations is UNKNOWN (OQ-021). |
| Hardware revisions | None defined (§8). |
| Hardware-dependent tests | TEST-HW-001, TEST-DRV-001, TEST-DRV-002, TEST-PLT-001, TEST-CAP-001 to TEST-CAP-004 and TEST-AUD-001 are all `BLOCKED — HARDWARE REQUIRED`. |

## 2. Rule 8 summary

| Rule 8 item | PACSCORDER value | Resolution marker | Open questions | Section |
|---|---|---|---|---|
| Raspberry Pi / CM | UNKNOWN — VERIFICATION REQUIRED (one choice per lane configuration, REQ-CAP-007; bring-up evaluates CM4 and CM5 side by side, owner 2026-10-07) | OWNER DECISION REQUIRED | OQ-011, OQ-018 | [§4.1](#41-raspberry-pi--cm) |
| TC358743 | UNKNOWN — VERIFICATION REQUIRED (part suffix, silicon revision, board) | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-018, OQ-029, OQ-034, OQ-085 | [§4.2](#42-tc358743) |
| HDMI connector | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED | OQ-018, OQ-024 | [§4.3](#43-hdmi-connector) |
| CSI connector | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED | OQ-021 | [§4.4](#44-csi-connector) |
| I2C | UNKNOWN — VERIFICATION REQUIRED | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-026, OQ-031, OQ-043 | [§4.5](#45-i2c) |
| GPIO | UNKNOWN — VERIFICATION REQUIRED (no allocation exists; includes the HDMI audio I2S lines, required by REQ-CAP-006) | VENDOR CONFIRMATION REQUIRED; OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (CM5 audio path) | OQ-020, OQ-022, OQ-025; added 2026-10-08: OQ-024, OQ-054, OQ-114 | [§4.6](#46-gpio), [§4.6.1](#461-hdmi-audio-path-i2s--req-cap-006) |
| RESET | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; DATASHEET REQUIRED | OQ-020, OQ-031 | [§4.7](#47-reset) |
| INT | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED | OQ-020, OQ-026 | [§4.8](#48-int) |
| POWER | UNKNOWN — VERIFICATION REQUIRED *(2026-10-09: the HDD is powered by a self-powered enclosure, never board VBUS — ADR-009; on the CM4 IO Board the PCIe slot, and so the NVMe SSD, needs the 12 V barrel input [J-05], RISK-026)* | OWNER DECISION REQUIRED; DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-023, OQ-024, OQ-031; added 2026-10-09: OQ-121, OQ-122 | [§4.9](#49-power) |
| CLOCK | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-019 | [§4.10](#410-clock) |
| CSI LANES | UNKNOWN — VERIFICATION REQUIRED (required: a 2-lane and a 4-lane configuration, REQ-CAP-007) | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-021, OQ-002 (OQ-001 ANSWERED 2026-10-07) | [§4.11](#411-csi-lanes) |
| ETHERNET | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; DATASHEET REQUIRED | OQ-018, OQ-098 | [§4.12](#412-ethernet) |
| USB | UNKNOWN — VERIFICATION REQUIRED *(2026-10-09: one USB port carries the recording HDD's USB-to-SATA bridge — ADR-009; bridge and port UNKNOWN)* | OWNER DECISION REQUIRED; DATASHEET REQUIRED; added 2026-10-09: VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED (bridge) | OQ-018, OQ-098; added 2026-10-09: OQ-122 | [§4.13](#413-usb), [§4.13.1](#4131-usb-for-the-recording-hdd-adr-009-added-2026-10-09) |
| STORAGE | UNKNOWN — VERIFICATION REQUIRED *(2026-10-09: recording media decided — NVMe SSD + USB-to-SATA HDD, mirrored, ADR-009; every part model, capacity and the boot medium still UNKNOWN)* | OWNER DECISION REQUIRED; DATASHEET REQUIRED (boot-storage options per board); added 2026-10-09: VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED (SSD, adaptor, NVMe boot) | OQ-006, OQ-018, OQ-098; added 2026-10-09: OQ-120, OQ-121, OQ-123, OQ-124 | [§4.14](#414-storage), [§4.14.1](#4141-recording-storage-per-adr-009-added-2026-10-09) |

## 3. Intended signal path

The diagram shows the architecture mandated by Rule 5 at board level. **It is not a schematic.** Every PACSCORDER-specific element is unknown.

```text
HDMI source: ATEM switcher output or camera (REQ-CAP-008; models OQ-102)
             (OQ-102 ANSWERED 2026-10-07: any HDMI camera, no model list,
              plus ATEM switcher outputs)
   │  TMDS, DDC, HPD, +5V (and CEC)
   ▼
[HDMI connector] ......................... PACSCORDER: UNKNOWN (OQ-018, OQ-024)
   │
   ▼
[TC358743XBG]  HDMI-RX → CSI-2-TX bridge [A-01]
   ├── REFCLK  ◄── oscillator on the bridge board, frequency UNKNOWN (OQ-019) [A-21], [A-45]
   ├── RESETN  ◄── source UNKNOWN: Pi GPIO, CAM_GPIO or reset circuit (OQ-020) [A-27]
   ├── INT     ──► Pi GPIO or not connected: UNKNOWN (OQ-020) [A-29]
   ├── I2C     ◄─► Pi camera I2C bus: UNKNOWN (OQ-026) [A-39]
   └── I2S     ──► Pi GPIOs, only if audio is required: UNKNOWN (OQ-004, OQ-025) [A-11], [A-12]
                   (Superseded 2026-10-07: audio is REQUIRED, REQ-CAP-006; OQ-004
                    ANSWERED. Overlay wiring GPIO 18/19/20 [A-47]; Pi side CM4
                    bcm2835-i2s [I-10], CM5 RP1 I2S1, unconfirmed [I-07] (OQ-054);
                    VDDIO2 vs GPIO bank voltage [I-29] (OQ-024); see §4.6.1)
   │
   │  CSI-2: 1 clock lane + N data lanes [A-05], [B-05]; N = 2 or 4 per
   │  configuration (REQ-CAP-007); lanes routed by the board UNKNOWN (OQ-021)
   ▼
[CSI connector + cable/adapter] .......... PACSCORDER: UNKNOWN (OQ-021)
   │
   ▼
[Raspberry Pi / CM CSI-2 receiver] ....... platform UNKNOWN per configuration (ADR-004, OQ-011)
                                           bring-up: CM4 and CM5 side by side (owner 2026-10-07)
```

*(Added 2026-10-09; research topic J / ADR-009.)* Recording storage attaches to the platform, not to the TC358743 path: the NVMe SSD on the module's single PCIe Gen 2 x1 link and the USB-to-SATA HDD on a USB port [J-01], [J-12], [J-18]. On the IO Boards that means the CM4 IO Board's PCIe socket (through a passive adaptor) or the CM5 IO Board's M.2 M-key slot [J-03], [J-11], [J-13]. Details: §4.13.1 and §4.14.1.

---

## 4. Rule 8 items

### 4.1 Raspberry Pi / CM

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Platform (Pi 4 Model B, CM4, Pi 5 or CM5) | UNKNOWN — VERIFICATION REQUIRED. One platform and connector is needed for each lane configuration, 2-lane and 4-lane (REQ-CAP-007); candidates per configuration in §4.11. Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07); that is an evaluation plan, not a platform choice. | OWNER DECISION REQUIRED — ADR-004 (OPEN, now a choice per configuration; decided from measurements), OQ-011 |
| RAM size | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED. It affects CMA sizing (OQ-061). |
| Board revision of the units used | UNKNOWN — VERIFICATION REQUIRED | HARDWARE TEST REQUIRED: record from each unit on receipt (§8). |
| Carrier board (CM4 or CM5 only): official IO board or custom | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018; for CM5, OQ-052 |
| Compute Module variant (with eMMC or without) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-006, OQ-018 |

**Reference facts from sources**

- **CSI-2 receiver.** Pi 4 Model B and CM4 (BCM2711) use the Unicam receiver [C-07], [C-09]. Pi 5 and CM5 use the RP1 CSI-2 receiver, driven by the RP1 CFE driver [C-29]. On Pi 5/CM5 the TC358743 overlay always uses Media Controller mode [C-11].
- **Lanes.** Pi 4 Model B has one 2-lane camera connector [C-01]. CM4 has CAM0 with 2 lanes and CAM1 with 4 lanes [C-02]. Pi 5 has two 4-lane ports [C-04]. CM5 has two 4-lane MIPI interfaces [C-05]. Details per platform are in §5.
- **Video encode hardware.** The BCM2711 (Pi 4B, CM4) is specified for H.264 1080p30 encode [D-10]. Pi 5 and CM5 have no hardware video encoder [D-31], [G-22]. See [VIDEO_ENCODER.md](VIDEO_ENCODER.md).
  - *(Added 2026-10-08, research topic H, when H.264 and H.265 were both required by REQ-ENC-001; deferred — REQ-ENC-002; not in current scope: the owner answered OQ-103 on 2026-10-08 with "H.264 only for now". Kept as evidence for REQ-ENC-002.)* No candidate has a hardware HEVC encoder: Pi 4/CM4's `bcm2835-codec` offers no HEVC format and the Raspberry Pi HEVC driver is a decoder only [D-24]; Pi 5/CM5 have no hardware video encoder at all [D-31]. H.265 is therefore encoded in software on every candidate (RISK-022). x265's Neon DotProd kernels can apply only on CM5's Cortex-A76 core; CM4's Cortex-A72 core has no DotProd, and the I8MM, SVE and SVE2 paths apply on neither board [H-04], [H-05]. Reasoning (inputs [H-05], [D-10], [I-06]): Pi 4 Model B has the same BCM2711 as CM4 and Pi 5 the same BCM2712 as CM5, so the same split applies to them. Software H.265 throughput on CM4 and CM5 is unmeasured (OQ-104, OQ-105); which outputs use H.265 is OQ-103. *(2026-10-08: OQ-103 ANSWERED — no output uses H.265 for now. OQ-104, OQ-105 and RISK-022 stay OPEN, not in current scope.)*
  - *(Added 2026-10-09; research topic K — live latency under 1 s for WebRTC viewers, REQ-STR-002; OQ-116 ANSWERED.)* CM4: the `bcm2835-codec` H.264 encoder never emits B-frames; its defaults are profile High, level 4.0 and a GOP of 60 [K-30]. A Raspberry Pi engineer stated that this encoder holds no extra buffers and that its latency depends on the macroblock count, about 10 ms for 720p on a Pi 4 (community source) [K-33]. CM5: Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39]; the per-frame x264 time on BCM2712 is unmeasured (OQ-059). Capture: Unicam (CM4) completes each buffer only at frame end, so a frame reaches userspace at least one frame's readout time after its first line [K-34]; RP1 CFE (CM5) also completes the buffer at frame end, so capture costs about one frame's readout time [K-35]. On CM4 the recording and live encodes would share the one hardware encoder; whether it runs both is UNKNOWN (OQ-115). Whether either module meets the < 1 s target is unmeasured (RISK-031, OQ-125).
- **HDMI audio receiver.** *(Added 2026-10-08, research topic I; audio is required, REQ-CAP-006.)* The `tc358743-audio` overlay binds the Pi side through the `i2s_clk_consumer` label [I-01], [I-03]. On CM4 that label is the single `bcm2835-i2s` controller [I-10]; on CM5 it resolves to RP1 I2S1, but capture there is unconfirmed [I-07], [I-08] (OQ-054). Details per platform: §4.6.1.
- **Software image.** One Raspberry Pi OS Lite image (2026-10-06) contains kernels and device trees for all four candidates (reasoning-tier entry, CORRECTED) [G-71]. The BCM2712-optimised kernel (`kernel_2712.img`, built from `bcm2712_defconfig`) uses 16K pages [E-51], [G-20]; `kernel8.img` (4K pages) also runs on BCM2712 devices [G-20]. The product runs its own project-built OS image (REQ-BLD-002, owner 2026-10-07); the build tool is ADR-003 (ACCEPTED 2026-10-07: `rpi-image-gen`). See [BUILD_SYSTEM.md](BUILD_SYSTEM.md).
- **RAM and CMA.** The device-tree CMA pool is limited to the lower 768 MB of RAM on Pi 4/CM4 and to the lower 1 GB by `bcm2712.dtsi` (Pi 5/CM5) (CORRECTED) [E-47]. The `cma-192` and larger CMA parameters need 1 GB of RAM [C-40].
- **Consequence (reasoning; inputs [C-01], [C-37], [C-48]).** Pi 4 Model B cannot carry 1080p60 from the TC358743; its limit is 1080p50 UYVY or 1080p30 RGB888. Under REQ-CAP-007 (owner, 2026-10-07; OQ-001 ANSWERED) 1080p60 is required on the 4-lane configuration, so Pi 4 Model B is excluded from the 4-lane configuration but remains a candidate for the 2-lane configuration (RISK-001; ADR-004 OPEN). The same applies to CM4 CAM0 [C-02].

### 4.2 TC358743

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Part number and ordering suffix | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-018. Which suffixes carry HDCP keys: OQ-028. Alternative parts TC358743AXBG and TC9590XBG: OQ-034. |
| Mounted on | UNKNOWN — VERIFICATION REQUIRED (third-party bridge board or custom PCB) | OWNER DECISION REQUIRED — OQ-018 |
| Silicon revision (CHIPID bits [7:0]) | UNKNOWN — VERIFICATION REQUIRED | HARDWARE TEST REQUIRED — OQ-029 (read during TEST-DRV-001) |
| Number of TC358743 devices (HDMI inputs) per unit | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018 |
| Design documentation held | Public datasheet Rev. 2.20 (2026-05-11) only [A-01]. NDA Functional Specification and register spreadsheet not held. | VENDOR CONFIRMATION REQUIRED — OQ-027 |
| Supply and lifecycle | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-085, RISK-004 |

**Reference facts from sources**

- **Function.** The TC358743XBG is an HDMI-RX to MIPI CSI-2-TX bridge. The current public datasheet is the combined TC358743XBG/TC9590XBG document, Rev. 2.20 of 2026-05-11, 20 pages [A-01]. The receiver is HDMI-RX 1.4, without Audio Return Channel or HDMI Ethernet Channel [A-02].
- **Input limits.** The maximum TMDS clock is 165 MHz; video input is supported up to 1080p60 [A-07]. The Linux driver's DV-timings capability covers 640–1920 × 350–1200 pixels and 13–165 MHz pixel clock, and advertises progressive timings without the interlaced capability [A-08].
- **Output.** CSI-2 with up to 4 data lanes and up to 1 Gbps per lane [A-05]; 1, 2, 3 or 4 lanes are configurable [A-06].
- **Audio.** Output is I2S or TDM on shared pins, in controller (master) clock mode only [A-11]. The silicon can also send audio over CSI-2 [A-05]. The Linux driver always configures 2-channel I2S output [A-13]. Reasoning: with the stock driver, audio therefore needs the I2S wiring in §4.6, separate from the CSI-2 connection.
  - *(Added 2026-10-08, research topic I; audio is required, REQ-CAP-006.)* The public datasheet Rev. 1.0 (2017-10-26) lists the I2S output as a single data lane for stereo, master-clock mode only, with 16/18/20/24-bit data that "depend on HDMI input stream", MSB first left- or right-justified, 32-bit time slots only, and a 256fs oversampling clock output. The TDM output is "Fixed to 8 channels". I2S and TDM share pins [I-25].
  - An internal audio PLL tracks the N/CTS values of the source's ACR packets, so the I2S clocks follow the HDMI source's audio clock, not a Pi clock [I-26]. Audio and video therefore arrive in separate clock domains (A/V synchronisation: OQ-112, RISK-024).
  - The driver programs audio once, at probe, and never selects the TDM, CSI or 4/6/8-channel settings that its register header defines [I-24]. Reasoning from [I-24], [I-13]: the stock hardware path is stereo I2S. What the chip puts on I2S for compressed or multichannel HDMI audio is OQ-110. The research found no list of supported sample rates in the public datasheet (research open question, topic I; OQ-033).
- **HDCP.** The datasheet lists "Support HDCP (optional)" without a version [A-03]. The Linux driver disables HDCP authentication on Device Tree platforms [A-04]. See RISK-008 and OQ-028.
- **Package.** TC358743XBG: P-TFBGA64, 64 balls, 6.0 × 6.0 mm, 0.65 mm pitch, 1.2 mm maximum height. TC9590XBG uses a different 7.0 × 7.0 mm, 0.80 mm pitch package [A-40].
- **Temperature and ESD.** TC358743XBG operating range is −30 to +70 °C ambient; TC9590XBG is −40 to +85 °C; storage is −40 to +125 °C. The datasheet notes that the product is weak against ESD [A-41].
- **Chip identification.** CHIPID is the 16-bit register 0x0000: bits [15:8] are the chip ID and bits [7:0] the revision [A-18]. The driver requires the chip-ID byte to read 0x00 and does not check the revision [A-19].
- **Documentation gap.** Only the 20-page summary datasheet is public. It has no register map, no I2C address and no AC timing. The Linux driver was written against non-public Toshiba documents [A-42] (RISK-005, OQ-027).
  - *(Added 2026-10-09; research topic K.)* The `rpi-6.18.y` driver sets the bridge's FIFOCTL trigger level to 374 and states no latency figure for the bridge; no public source checked documents the TC358743's internal buffering latency [K-38]. That term of the < 1 s WebRTC budget is UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED; HARDWARE TEST REQUIRED; OQ-125, OQ-027).
- **Other blocks.** An infrared input exists but the driver holds it in reset [A-49]. A CEC pin exists; Linux CEC support is a separate kernel option [A-34] (OQ-016).
- **Lifecycle (research notes, not register facts).** The research recorded a Raspberry Pi engineer's belief that the part is end-of-life, and that Toshiba's product page showed no EOL or NRND flag when fetched (topic C open question). It also recorded that the product page listed the TC358743XBG as mass production on 2026-10-06 (topic A gap). VENDOR CONFIRMATION REQUIRED — OQ-085.

### 4.3 HDMI connector

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Connector type and position | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018 |
| Number of HDMI inputs | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018 |
| HPD output circuit and +5V sensing | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED (schematic); DATASHEET REQUIRED — OQ-024 |
| ESD protection on the HDMI lines | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-018 |
| CEC line connected | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-016 |

**Sources the connector must accept (requirement, not a hardware value).** HDMI sources are the HDMI outputs of Blackmagic ATEM switchers and cameras connected directly (REQ-CAP-008, DRAFT; owner, 2026-10-07; OQ-009 ANSWERED). Which models is OPEN (OQ-102). *(Superseded 2026-10-07: OQ-102 ANSWERED by the owner — no model list; any HDMI camera (generic), plus ATEM switcher outputs. What is accepted is defined by the supported-mode matrix and the EDID (OQ-002), and unsupported modes are rejected per REQ-CAP-005. Representative cameras and an ATEM are still needed for TEST-CAP-001 and TEST-CAP-004.)* The ATEM Mini Pro has one HDMI program output with 1080p23.98 to 1080p60 output standards and no 720p, 1080i or Ultra HD [F-23]. Camera HDMI output modes and HDCP behaviour are UNKNOWN — VERIFICATION REQUIRED per model (OQ-102); the driver disables HDCP [A-04] (RISK-008). *(2026-10-08: with no model list, this applies to every camera used in testing.)* Which audio formats the ATEM outputs and the cameras send is also unverified (research open question, topic I; OQ-110, OQ-083).

**Reference facts from sources**

- **Signal levels.** DDC_SCL, DDC_SDA and HPDI are 5 V tolerant and sit in the VDDIO1 (3.3 V) domain. HPDO (hot-plug detect output) is in the same VDDIO1 domain but is not in that 5 V tolerant list [A-39]. How the bridge board drives HPD toward the source and senses source +5V is unknown (OQ-024).
- **CEC.** The CEC pin (ball G1) is in the VDDIO1 3.3 V domain. Linux CEC support needs `CONFIG_VIDEO_TC358743_CEC` [A-34], which the Raspberry Pi defconfigs do not enable [B-20].
- **Hot-plug behaviour.** The driver asserts HPD only after userspace has written an EDID and source +5V is present. With an EDID and +5V present, HPD rises after HZ/7 jiffies, 140 ms at the Raspberry Pi default HZ=250 (CORRECTED) [A-33], [B-21]. When +5V disappears the driver drops HPD and clears the stored timings [B-23]. See RISK-010 and REQ-CAP-003.
- **EDID memory.** 1 KB of embedded EDID SRAM; EDID 1.3 base block plus one CEA-861-D extension [A-31].
- **HDMI PHY support components.** REXT must connect to AVDD33 through a 2 kΩ ±1 % resistor. VPGM, the eFuse programming supply, must be tied to ground [A-37].
- **ESD (reasoning).** The datasheet says the part is weak against ESD [A-41], so ESD protection at the HDMI connector is a design item for any custom PCB (OQ-018).

### 4.4 CSI connector

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Bridge-side connector (15-pin 1.0 mm or 22-pin 0.5 mm) | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-021 |
| Cable or adapter, contact sides, length | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-021 |
| Pi-side connector used (Pi 4B camera port, CM CAM0/CAM1, Pi 5 CAM/DISP0/1, CM5 MIPI0/1) | UNKNOWN — VERIFICATION REQUIRED, one per lane configuration (REQ-CAP-007); connector-to-configuration map in §4.11 | OWNER DECISION REQUIRED — ADR-004, OQ-011, OQ-021 |

**Reference facts from sources**

- **Pi 4 Model B:** one standard 15-pin, 1.0 mm pitch, 16 mm wide CSI connector [C-01].
- **CM4 IO Board:** both CSI-2 interfaces go to separate 22-pin, 0.5 mm pitch connectors. CSI0 (2 lanes) needs both J6 jumpers fitted to route I2C to its connector [C-03].
- **Pi 5:** two mini 22-pin, 0.5 mm pitch, 11.5 mm wide combined CSI/DSI ports [C-04].
- **CM5:** MIPI0 is on the CM4 CAM1 pins 115–141; MIPI1 is on the CM4 DSI1 pins 175–196. The CM4 CAM0 pins 128–142 carry USB 3.0 on CM5 [C-05].
- **CM5 IO Board:** two dual-purpose 22-pin CAM/DISP connectors. CAM/DISP 0 has a camera power-down signal. CAM/DISP 1 needs two J6 jumpers to route I2C, and a camera on it cannot be powered down [C-06].
- **Cable hazard (community).** A Raspberry Pi engineer (6by9) reported that the Auvidea B101 uses a 15-pin FFC with contacts on the same side, while Pi 5 has 22-pin connectors. He reported that a wrongly sided 22-to-15 adapter swaps pin 1 (GND) with pin 15 (3V3) and can damage either board [C-45] (RISK-021).
- **Connector selection in software.** On boards with two connectors the overlay defaults to connector 1; appending `,cam0` selects connector 0 [C-39]. See [DEVICE_TREE.md](DEVICE_TREE.md).

### 4.5 I2C

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Pi I2C controller and Linux bus | UNKNOWN — VERIFICATION REQUIRED (depends on platform and connector) | OQ-011, OQ-021; runtime bus number OQ-043 (TEST-PLT-001) |
| TC358743 7-bit address | UNKNOWN — VERIFICATION REQUIRED. The kernel binding example and the Raspberry Pi overlay use 0x0f [A-15]; not confirmed for PACSCORDER. | DATASHEET REQUIRED; HARDWARE TEST REQUIRED — OQ-026 (TEST-HW-001) |
| Pull-up resistors (value, voltage, location) | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-024 |
| Bus clock | UNKNOWN — VERIFICATION REQUIRED | DATASHEET REQUIRED — OQ-031 |
| Other devices on the same bus | UNKNOWN — VERIFICATION REQUIRED | HARDWARE TEST REQUIRED — OQ-026 |

**Reference facts from sources**

- **Address.** The kernel binding example and the Raspberry Pi `tc358743.dtsi` place the TC358743 at 7-bit address 0x0f. The public datasheet states no address and no address-select strap [A-15]. The driver prints addresses in 8-bit form, so 0x0f appears as 0x1e in kernel messages [A-16].
- **Possible strap.** The sister part TC358749XBG documents addresses 0x0F and 0x1F, selected by the INT pin at reset. No equivalent statement exists for TC358743XBG [A-50] (OQ-026). The research notes advise against driving or pulling INT strongly during reset until this is resolved (research gap, topic A; not a register fact).
- **Speed.** The datasheet gives 100 kHz and 400 kHz. An older product brief also lists 2 MHz. The two documents conflict [A-14] (OQ-031).
- **Protocol.** 16-bit register addresses sent MSB first, little-endian register values, at most 130 bytes per driver write [A-17]. The I2C adapter must support SMBus byte-data transfers [A-20].
- **Voltage domain.** The host I2C pins are in VDDIO2, which is 1.8 V or 3.3 V [A-39], [A-36].
- **Pi-side buses.** Per-platform buses and pins are in §5 [C-24]–[C-27], [B-44]. A Raspberry Pi engineer reported that Pi 5 camera I2C bus numbers changed between kernels (CORRECTED) [C-28].
- **Bus sharing.** With CM5 on the CM4 IO Board, the `i2c_csi_dsi1` bus also serves DISP1, the on-board RTC and the fan controller [C-27]. Buildroot's CM4IO/CM5IO sample configurations also place an RTC on the camera bus (research gap, topic E; not a register fact). See [DEVICE_TREE.md](DEVICE_TREE.md).
- **Reasoning (input [A-15]).** Two TC358743 devices at the same fixed address cannot share one bus. A multi-input design needs separate buses, or a second address confirmed by datasheet and test (OQ-018, OQ-026).

### 4.6 GPIO

**PACSCORDER value:** no GPIO allocation exists. UNKNOWN — VERIFICATION REQUIRED (OQ-020, OQ-022, OQ-025). *(2026-10-08: HDMI audio is required — REQ-CAP-006, OQ-004 ANSWERED 2026-10-07 — so the I2S rows below must be wired; GPIO 18–21 allocation is OQ-114. See §4.6.1.)*

| Signal | Direction and type (TC358743 side) | PACSCORDER Pi GPIO | Reference |
|---|---|---|---|
| RESETN | Input, active low, Schmitt [A-27] | UNKNOWN — VERIFICATION REQUIRED (OQ-020) | §4.7 |
| INT | Output, active high, level [A-29] | UNKNOWN — VERIFICATION REQUIRED (OQ-020) | §4.8 |
| Camera-connector CAM_GPIO (power enable) | Pi output [C-21], [C-22] | UNKNOWN whether the board uses it (OQ-022) | below |
| A_SCK (I2S bit clock) | Output, VDDIO2 [A-12]; ball F7 [I-27] | UNKNOWN (OQ-025). The `tc358743-audio` overlay expects GPIO 18 [A-47]. | below, §4.6.1 |
| A_WFS (I2S word clock) | Output, VDDIO2 [A-12]; ball G7 [I-27] | UNKNOWN (OQ-025). The overlay expects GPIO 19 [A-47]. | below, §4.6.1 |
| A_SD (I2S data) | Output, VDDIO2 [A-12]; ball F8 [I-27] | UNKNOWN (OQ-025). The overlay expects GPIO 20 [A-47]. | below, §4.6.1 |
| A_OSCK (256fs oversampling clock) | Output, VDDIO2 [A-11], [A-12]; ball G8 [I-27] | UNKNOWN. Not part of the overlay's documented wiring [A-47]. Whether it is needed or can stay unconnected is unknown (research open question, topic I; OQ-025). | — |
| Pi GPIO 21 (no TC358743 signal) | — | Claimed by the I2S pin group when `tc358743-audio` is enabled, although the TC358743 path uses only GPIO 18, 19 and 20 [I-30]. Freeing it would need a custom overlay with a pin group for GPIO 18–20 only (research gap, topic I; OQ-114). | §4.6.1 |

**Reference facts from sources**

- **Camera power-enable lines in the stock device trees.**
  - Pi 4B: `cam1_reg` is a fixed regulator on expander GPIO 5 ("CAM_GPIO", firmware-controlled); `cam0_reg` is a dummy. CM4: both aliases share expander GPIO 5 [C-21].
  - Pi 5: `cam0_reg` is RP1 GPIO 34 (MIPI 0 connector) and `cam1_reg` is RP1 GPIO 46 (MIPI 1 connector). CM5: RP1 GPIO 34 (CAM_GPIO0), shared by both [C-22].
- **CAM_GPIO with the TC358743.** Neither the overlay nor the driver uses these regulators [C-23]. Reasoning from the regulator code: the line is expected to stay low while the TC358743 is in use [C-51]. A board that needs CAM_GPIO high to power up or leave reset would stay off with the stock overlay (OQ-022; [DEVICE_TREE.md](DEVICE_TREE.md)).
- **Audio wiring.** The `tc358743-audio` overlay routes LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18 and DATA/SD to GPIO 20 [A-47], [G-14]. The Pi is the I2S clock consumer [B-46]; the TC358743 drives the clocks [A-11]. Audio wiring is needed only if audio is required (OQ-004). *(Superseded 2026-10-07: audio is required — REQ-CAP-006, OQ-004 ANSWERED — so this wiring is needed.)* Whether this overlay works on Pi 5/CM5 is unverified (OQ-054). *(2026-10-08: on CM5 its labels resolve to RP1 I2S1 on GPIO 18–21, but operation is still unconfirmed [I-07], [I-08]; §4.6.1.)*
- **Voltage domain.** REFCLK, RESETN, INT, the host I2C pins and the audio pins are all in VDDIO2 [A-21], [A-27], [A-29], [A-39], [A-12]. VDDIO2 accepts 1.65–3.6 V [A-36]. The Raspberry Pi GPIO voltage is not in the source register: DATASHEET REQUIRED. The research notes say that the I2S pin voltage equals VDDIO2, which must be 3.3 V to interface with Pi GPIO 18/19/20 (research gap, topic A; not a register fact) (OQ-024). *(Partly superseded 2026-10-08: the CM4 IO Board and the CM5 IO Board both offer a selectable 1.8 V or 3.3 V GPIO voltage, and the TC358743's VDDIO2 should match the selected voltage, or the audio lines need level shifting [I-29]. On those carriers 3.3 V is therefore not the only option. The GPIO voltage of Pi 4 Model B, Pi 5 and any custom carrier is still not in the register: DATASHEET REQUIRED.)*

#### 4.6.1 HDMI audio path (I2S) — REQ-CAP-006

*Added 2026-10-08 from research topic I.* HDMI audio is required in recordings and streams (REQ-CAP-006, DRAFT; owner 2026-10-07, OQ-004 ANSWERED). Channels, sample rates and the A/V tolerance are not yet specified (OQ-004, OQ-110, OQ-111, OQ-112). With the stock driver the audio leaves the TC358743 on I2S, on wiring separate from the CSI-2 cable (reasoning, §4.2) (RISK-014).

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| TC358743 audio pins brought out by the bridge board (A_SCK, A_WFS, A_SD; A_OSCK) | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-025, OQ-018 |
| Wiring to Pi GPIO 18/19/20, or to the carrier's I2S pins | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED — OQ-025 (TEST-AUD-001) |
| VDDIO2 voltage, and the carrier's GPIO bank voltage selection | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-024 |
| Other users of GPIO 18–21 on the carrier or in software | UNKNOWN — VERIFICATION REQUIRED (no carrier pin map exists) | OWNER DECISION REQUIRED — OQ-114, OQ-018 |
| Audio capture through RP1 I2S1 on CM5 | UNKNOWN — VERIFICATION REQUIRED (unconfirmed by any source) | HARDWARE TEST REQUIRED — OQ-054 (TEST-AUD-001) |

**Reference facts from sources**

- **Pins.** The four audio pins are outputs: A_SCK (I2S/TDM bit clock, ball F7), A_WFS (I2S word clock or TDM frame sync, ball G7), A_SD (data, ball F8) and A_OSCK (oversampling clock, ball G8). They are powered from VDDIO2, rated 1.8–3.3 V (recommended 1.65–3.6 V) [I-27].
- **Clock roles.** The datasheet makes the TC358743 the I2S clock master only [I-25]. The overlay makes the codec link bit-clock and frame master and binds the Pi through `i2s_clk_consumer`, with two 32-bit slots [I-03]. Reasoning: the overlay and the datasheet agree on clock roles; 2 slots × 32 bits = 64 bit clocks per frame (64fs) [I-28].
- **Voltage.** The CM4 IO Board and the CM5 IO Board both list a 40-pin GPIO header and a selectable 1.8 V or 3.3 V GPIO voltage; VDDIO2 should match the selected voltage, or the audio lines need level shifting [I-29] (OQ-024).
- **Pin conflicts.** With `tc358743-audio` enabled, the I2S pin group claims GPIO 18–21, including GPIO 21, which the TC358743 path does not use [I-30]. The `pwm` and `pwm-2chan` overlays default to pin 18, which the README calls "the one used by the I2S audio interface"; `gpio-ir` defaults to GPIO 18; `audremap` offers `pins_18_19` on BCM2835/BCM2711, while on BCM2712 its `audremap-pi5` variant says `pins_18_19` is not available; `gpio-fan` defaults to GPIO 12 and does not conflict [I-31]. On CM5 the base Device Tree's power-button and fan entries do not use header GPIO 18–21 [I-32] (OQ-114).

Pi-side I2S per platform (sources cover CM4 and CM5; the platform focus of bring-up):

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Controller behind `i2s_clk_consumer` | Not covered by the topic I register entries: KERNEL SOURCE INSPECTION REQUIRED | The single `bcm2835-i2s` node (`i2s@7e203000`); `i2s_clk_producer` and `i2s_clk_consumer` both point at it, and enabling it flips the same node as `dtparam=i2s=on` [I-10] | `bcm2712-rpi.dtsi` maps the label to RP1 I2S1 [I-07]; whether the Pi 5 board DT includes that file is not in the register: KERNEL SOURCE INSPECTION REQUIRED (OQ-054) | RP1 I2S1 (`rp1_i2s1`, `i2s@a4000`, compatible `snps,designware-i2s`) [I-07], [I-08]; the clock-consumer instance a codec-master link needs [I-09] |
| Pins | GPIO 18/19/20 per the overlay README [A-47], [G-14] | GPIO 18–21 in ALT0 (`i2s_pins`) [I-10], [I-30] | As for the controller row | GPIO 18–21, function `i2s1` (`rp1_i2s1_18_21`) [I-08], [I-30] |
| Capture limits on the Pi side | Not covered by topic I | Exactly 2 channels, 8–384 kHz, S16_LE / S24_LE / S32_LE; the rate on the wire is set by the TC358743 [I-13] | Not covered by topic I | Channel count and formats are read from RP1 hardware registers, not visible in source; `hw_params` accepts 2, 4, 6 or 8 channels [I-14] |
| Evidence of capture in sources | — | A 2019 forum thread shows the `tc358743` card enumerated on a Pi with `bcm2835-i2s` (community source) [I-16] | — | None: labels resolve [I-05], [I-06], [I-07], but no source shows capture (research gap, topic I; OQ-054). Reasoning in ADR-004: a bring-up gate for CM5. |
| GPIO bank voltage | Not in the register: DATASHEET REQUIRED | CM4 IO Board: selectable 1.8 V or 3.3 V [I-29]; custom carrier UNKNOWN (OQ-018) | Not in the register: DATASHEET REQUIRED | CM5 IO Board: selectable 1.8 V or 3.3 V [I-29]; custom carrier UNKNOWN (OQ-018) |

The overlay and its Device Tree details are in [DEVICE_TREE.md](DEVICE_TREE.md) §3.5; the driver's audio setup and controls in [TC358743_DRIVER.md](TC358743_DRIVER.md) §11.7 and §22.1.

### 4.7 RESET

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| RESETN driven by | UNKNOWN — VERIFICATION REQUIRED (Pi GPIO, camera-connector CAM_GPIO, or power-on reset circuit) | VENDOR CONFIRMATION REQUIRED — OQ-020, OQ-022 |
| Pi GPIO number (if any) | UNKNOWN — VERIFICATION REQUIRED | OQ-020 |
| Pull resistor and default state | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-020 |
| Required pulse width and reset-to-I2C-ready time | UNKNOWN — not in the public datasheet | DATASHEET REQUIRED — OQ-031 |

**Reference facts from sources**

- RESETN (ball G5) is the system reset input: active low, Schmitt input, VDDIO2 domain [A-27].
- The DT binding makes `reset-gpios` optional; its example uses `GPIO_ACTIVE_LOW` [A-27], [B-05].
- When `reset-gpios` is present, the driver waits 5–10 ms, asserts reset for 1–2 ms, deasserts it, and waits 20 ms before reading CHIPID. These times are the driver's choice, not a Toshiba specification [A-28], [B-13].
- The stock Raspberry Pi overlay has no `reset-gpios` [B-41], [C-23]. Driver removal does not assert reset [B-40].

### 4.8 INT

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| INT connected to a Pi GPIO | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-020 |
| Pi GPIO number (if any) | UNKNOWN — VERIFICATION REQUIRED | OQ-020 |
| Pull resistor on INT | UNKNOWN — VERIFICATION REQUIRED. It matters if INT is also an address strap. | DATASHEET REQUIRED — OQ-026 |

**Reference facts from sources**

- INT (ball B3) is the interrupt output: active high, level-triggered, VDDIO2 domain, low at initialisation [A-29].
- The driver requests the IRQ with `IRQF_TRIGGER_HIGH | IRQF_ONESHOT` [A-29], [B-19].
- Without an IRQ the driver polls the interrupt status over I2C every 1000 ms, or every 10 ms when a CEC adapter is registered [A-30], [B-19]. The stock overlay has no `interrupts` property, so it runs in polling mode [A-30], [C-20]. See RISK-013 and REQ-CAP-004.
- On the sister part TC358749XBG, INT also selects the I2C address at reset [A-50] (OQ-026).

### 4.9 POWER

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Product power input (voltage, connector, source such as DC jack, USB-C or PoE) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-023 |
| Total power budget | UNKNOWN — VERIFICATION REQUIRED | DATASHEET REQUIRED; HARDWARE TEST REQUIRED (measure) — OQ-023 |
| Generation of the TC358743 rails (regulators) | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-018, OQ-024 |
| Rail power-up sequencing | UNKNOWN — not in the public datasheet | DATASHEET REQUIRED — OQ-031 |
| Bridge board powered from the camera-connector 3V3 or a separate supply | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-021, OQ-022 |
| Operating ambient temperature target | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-010 |
| HDD power *(added 2026-10-09)* | Owner decision (ADR-009, 2026-10-08): a **self-powered enclosure**, never the board's USB VBUS ("or hub" in ADR-009 decision 3 is Claude's addition, not an owner decision). The enclosure, its supply and its power-up behaviour are UNKNOWN — VERIFICATION REQUIRED. | VENDOR CONFIRMATION REQUIRED — OQ-122; power budget OQ-023 |
| NVMe SSD supply and peak current *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED (no SSD chosen). The rating of the CM4 IO Board's 12 V-to-3.3 V slot converter and of the CM5 IO Board's M.2 3.3 V supply is not in the register (research gap, topic J). | DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED — OQ-121 |
| Hold-up supply for a clean stop on power loss *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED. Not decided; depends on the loss the owner accepts after measurement (OQ-119; RISK-030). | OWNER DECISION REQUIRED — OQ-119, OQ-023 |

> **12 V is not a PACSCORDER specification.** The owner's Rule 9 test example lists "12V power" in its Setup section. That is an example in the rules. The actual PACSCORDER power input is `UNKNOWN — VERIFICATION REQUIRED` (OQ-023). *(Added 2026-10-09; research topic J.)* On the CM4 IO Board, however, the PCIe slot is powered only from the +12 V barrel input, so an NVMe SSD on that carrier needs 12 V; with a 5 V-only PoE HAT, PCIe cards do not work [J-05] (RISK-026). That is a fact about Raspberry Pi's reference carrier, not a PACSCORDER specification; a custom carrier's PCIe supply is UNKNOWN (OQ-018, OQ-023).

**Reference facts from sources**

TC358743 supply rails and recommended operating ranges [A-36], with supply-noise limits [A-37]:

| Rail | Function | Nominal | Range | Noise limit (peak-to-peak) |
|---|---|---|---|---|
| VDDC1 / VDDC2 | Core | 1.2 V | 1.1–1.3 V | 0.1 V (general limit) |
| VDD_MIPI | MIPI | 1.2 V | 1.1–1.3 V | 0.1 V (general limit) |
| AVDD12 | HDMI PHY | 1.2 V | 1.15–1.25 V | 0.04 V |
| AVDD33 | HDMI PHY | 3.3 V | 3.135–3.465 V | 0.08 V |
| VDDIO1 | HDMI digital IO | 3.3 V | 3.0–3.6 V | 0.1 V (general limit) |
| VDDIO2 | Digital IO (Pi-facing pins) | 1.8 V or 3.3 V | 1.65–3.6 V | 0.1 V (general limit) |
| AVDD25 | APLL | 2.5 V | 2.25–2.75 V | 0.1 V (general limit) |

- **Consumption.** Typical total power is 480.5 mW at 720p60 and 543.2 mW at 1080p60. Sleep mode draws 108.9 µW. VDDC1 is always on; VDDC2 can be shut off in deep sleep [A-38].
- **No supply control from Linux.** The DT binding defines no supply properties [B-04], [B-05], and the driver requests no regulator [C-23]. Reasoning: the rails must already be up, and RESETN released, before the driver probes. The stock driver cannot sequence them.
- **Camera connector supply (community).** A Raspberry Pi engineer reported that on the 15-pin camera connector pin 1 is GND and pin 15 is 3V3 [C-45].
- **Raspberry Pi power.** No Raspberry Pi board power figure was collected in the 2026-10-06 research. DATASHEET REQUIRED (OQ-023). *(Superseded in part 2026-10-09: research topic J — figures for the two IO Boards are below. Pi 4 Model B and Pi 5 power, and PACSCORDER's total budget, are still DATASHEET REQUIRED / HARDWARE TEST REQUIRED (OQ-023).)*
- **IO-board power and USB VBUS** *(added 2026-10-09; research topic J)*.
  - CM4 IO Board: the main input J19 is a +12 V DC barrel; it feeds the PCIe slot's +12 V pins directly, and an on-board +12 V-to-+3.3 V DC-DC converter serves only the PCIe slot. A typical PoE HAT provides +5 V only, so PCIe cards and the fan do not work with it. Raspberry Pi recommends budgeting 9 W for the CM4 [J-05]. USB VBUS for the hub's ports comes from one current-limit switch set to about 1.2 A [J-06].
  - CM5 IO Board: powered through USB-C (J11), negotiating 5 V at 5 A over USB PD by default; Raspberry Pi documents 5 V / 5 A (25 W), or 5 V / 3 A (15 W) with a 600 mA peripheral limit; `PSU_MAX_CURRENT=5000` in the EEPROM configuration suppresses the low-current warning [J-20]. The two USB 3.0 ports share about 1.2 A of VBUS through an internal current switch [J-19].
- **HDD power** *(added 2026-10-09; research topic J; the register's HDDs are datasheet examples, not PACSCORDER parts)*.
  - 2.5-inch: a Seagate BarraCuda 2.5-inch SATA HDD draws up to 1.0 A at +5 V during spin-up and averages 1.70 W (1-disk) or 1.80 W (2-disk) when writing; it takes +5 V only, through a native SATA power connector (CORRECTED) [J-30].
  - 3.5-inch: a Seagate BarraCuda 3.5-inch HDD takes +5 V and +12 V, with a 12 V startup current of 2.0 A or 2.5 A depending on capacity; because USB VBUS supplies only 5 V, the register entry concludes that a 3.5-inch HDD always needs an external 12 V supply, such as a self-powered dock [J-32].
  - Reasoning [J-31]: a bus-powered 2.5-inch HDD needing 1.0 A to spin up exceeds the 600 mA peripheral limit of a CM5 IO Board on a 3 A supply; it fits only nominally under the ~1.2 A limits of the CM4 IO Board (one switch for all hub ports) and of the CM5 IO Board on a 5 A supply (shared by both USB 3 ports), leaving about 0.2 A for the bridge and every other USB device. Raspberry Pi's documentation says HDDs typically need a powered USB hub, and that without one intermittent failures can occur even when everything appears to work [J-28]. ADR-009 therefore puts the HDD in a self-powered enclosure (the owner's words; ADR-009 decision 3 adds "or hub" as Claude's addition).
  - Whether the CM5 IO Board also applies a firmware USB current limit beyond the ~1.2 A switch is a research open question (topic J; HARDWARE TEST REQUIRED). How much the chosen self-powered enclosure's bridge still draws from board VBUS is UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED; OQ-122, OQ-023).
- **Thermal.** The TC358743XBG is rated −30 to +70 °C ambient [A-41] (OQ-010, REQ-PERF-001).

### 4.10 CLOCK

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| REFCLK oscillator frequency | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED (schematic, BOM); HARDWARE TEST REQUIRED (measure) — OQ-019 |
| Oscillator part, accuracy, jitter | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-019; Toshiba limits: DATASHEET REQUIRED — OQ-031 |
| Oscillator output level (VDDIO2) | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-024 |

**Reference facts from sources**

- **Allowed frequencies.** REFCLK (ball H5) is the reference clock input. The datasheet lists 27/26 MHz or 42 MHz, in the VDDIO2 domain [A-21]. The driver accepts only 26, 27 or 42 MHz [B-07].
- **Wrong value is not rejected cleanly (CORRECTED).** For any other rate the driver logs `unsupported refclk rate` but probe continues. If the chip then answers the CHIPID read, `tc358743_set_ref_clk()` hits `BUG_ON()`, a kernel BUG [A-22], [B-11] (RISK-007).
- **27 MHz preferred (reasoning).** Only 27 MHz gives the exact 594 and 972 Mbps lane rates of the driver's timing tables. 26 MHz gives 572/962 Mbps and 42 MHz gives 588/966 Mbps [A-23], [B-10].
- **The Pi does not generate REFCLK.** The Raspberry Pi overlay declares 27 MHz on a `fixed-clock` node; the bridge board must supply its own oscillator [A-45]. Reasoning from the base device trees gives the same result [B-47].
- **Missing specifications.** The public datasheet has no AC timing [A-42], so REFCLK tolerance, jitter and duty cycle are not available (OQ-031).
- **Device Tree consequence (reasoning; inputs [A-45], [B-07], [A-23]).** The overlay only declares the frequency and the driver derives its PLL settings from the declared value, so the DT `clock-frequency` must equal the measured oscillator frequency. See [DEVICE_TREE.md](DEVICE_TREE.md).

### 4.11 CSI LANES

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Data lanes routed from the TC358743 to the Pi | UNKNOWN — VERIFICATION REQUIRED. Required: one 2-lane and one 4-lane configuration (REQ-CAP-007). Whether one board design serves both, for example a 4-lane board run with 2 lanes, is UNKNOWN. | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED — OQ-021 |
| Lanes available on the chosen Pi connector | Depends on the platform and connector chosen for each configuration (ADR-004, OQ-011); map below and §5 | OWNER DECISION REQUIRED — OQ-011 |
| Lane order and polarity on PCB, connector and cable | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED — OQ-021 |
| Link rate | Not a hardware value; set in the Device Tree ([DEVICE_TREE.md](DEVICE_TREE.md)). ADR-008 (PROPOSED) proposes keeping the overlay default of 486 MHz (972 Mbit/s per lane). | OWNER DECISION REQUIRED — OQ-099 |

**Reference facts from sources**

- **Transmitter.** 1 to 4 data lanes, up to 1 Gbps per lane [A-05], [A-06].
- **Receivers.** Unicam lanes run at up to 1 Gbit/s each (maximum link frequency 500 MHz) [C-07]. RP1 runs at up to 1.5 Gbps per lane, with 8 Gbps in total across its two 4-lane D-PHYs [C-30].
- **Official capability.** With 2 lanes the maximum is 1080p30 RGB888 or 1080p50 YUV422. With 4 lanes on a Compute Module, 1080p60 can be received in either format [C-37].
- **Lanes the driver requests (reasoning).** At the default 972 Mbps per lane: 1080p60 UYVY needs 3 lanes and RGB888 needs 4; 1080p50 UYVY needs 2 and RGB888 needs 3; 1080p30 needs 2 in either format [C-47]. On 2 lanes, 1080p60 UYVY would need 102.4 % of the link [C-48]. All 1080p30/50/60 combinations fit on 4 lanes by bandwidth, but the driver activates only 2–4 lanes, so not every mode uses all four [C-49].
- **A 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (reasoning; inputs [C-47], [C-49]).** At 972 Mbit/s per lane the driver activates 3 of the 4 lanes for this mode. Capture on 3 of 4 configured lanes is unproven (OQ-038). ADR-008 (PROPOSED) proposes evaluating 297 MHz (594 Mbit/s, 4 active lanes) for this mode on a CM4 CAM1 4-lane link only, in TEST-CAP-002 (OQ-099).
- **No clamping.** The driver computes the lane count at runtime and does not clamp it to the DT `data-lanes` value (CORRECTED) [A-25]. Both receivers fail stream start when more lanes are requested than the DT configures [B-32], [C-16].
- **Mis-configuration.** Declaring 4 lanes on a 2-lane connector does not fail the probe: Unicam logs a message and adopts the endpoint count [C-17]. See the warning in [DEVICE_TREE.md](DEVICE_TREE.md).
- **Corruption near lane limits (community).** An open issue reports corrupted images at 1080p50 RGB888 on a 4-lane CM4 when the driver chose 3 lanes; a Raspberry Pi engineer attributed it to the fixed FIFO trigger level and to the lane formula using active height instead of total line time [C-43] (RISK-006).

#### Lane configurations required by REQ-CAP-007

The owner requires both a 2-lane and a 4-lane CSI-2 configuration, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; owner statement of 2026-10-07, OQ-001 ANSWERED). Which platform and connector serve each configuration is **not decided** (ADR-004, OPEN). The table maps each candidate connector to the configuration it can serve. It is not a selection.

| Candidate connector | Data lanes at the connector | Lane configuration it can serve | Notes |
|---|---|---|---|
| Pi 4 Model B camera connector | 2 [C-01] | 2-lane only | Never declare 4 lanes: not rejected at probe [C-17] |
| CM4 CAM0 | 2 [C-02] | 2-lane only | CM4 IO Board: J6 jumpers needed for I2C [C-03] |
| CM4 CAM1 | 4 [C-02] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | — |
| Pi 5 CAM/DISP0 and CAM/DISP1 | 4 per port [C-04] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | 4 lanes on CAM/DISP0 (`cam0`): NEEDS VERIFICATION (OQ-049) |
| CM5 MIPI0 and MIPI1 | 4 per interface [C-05] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | Carrier-dependent (OQ-052); no CM4-style CAM0 [C-05] |

**Supported-mode limits per configuration** at the default 972 Mbit/s per lane. These are bandwidth results (reasoning-tier entries and official documentation), not test results:

| Configuration | Fits by bandwidth | Does not fit | Sources |
|---|---|---|---|
| 2-lane | 1080p30 UYVY (51.2 %), 1080p30 RGB888 (76.8 %), 1080p50 UYVY (85.3 %); 720p60 needs 1 lane in UYVY and 2 in RGB888 | 1080p50 RGB888 (128 %), 1080p60 UYVY (102.4 %), 1080p60 RGB888 (153.6 %) | [C-37], [C-48], [B-33] |
| 4-lane | All six 1080p30/50/60 × UYVY/RGB888 combinations; the highest load is 1080p60 RGB888 at 76.8 % of 3.888 Gbit/s | None of those six | [C-37], [C-49] |

- Official Raspberry Pi documentation gives the same limits: 2 lanes carry at most 1080p30 RGB888 or 1080p50 YUV422, and 4 lanes on a Compute Module carry 1080p60 in either format [C-37]. "All frame rates" on a 2-lane configuration therefore means all rates up to that limit (REQ-CAP-007).
- The 4-lane results are necessary, not shown sufficient: 1080p60 UYVY activates 3 of the 4 lanes (OQ-038), and 1080p50 RGB888 on 3 of 4 lanes has the open corruption report above [C-43] (RISK-006).
- At 594 Mbit/s (`link-frequency=297000000`) only 1080p30 UYVY fits on 2 lanes [C-48]. On 4 lanes 1080p60 UYVY, 1080p50 UYVY and both 1080p30 formats fit, and 1080p50/1080p60 RGB888 are rejected [C-49]. The link rate is ADR-008 (PROPOSED; OQ-099).
- **EDID per configuration (reasoning; OQ-002).** The EDID is written by userspace [C-37], not by the hardware. The built-in `hdmi` EDID type of `v4l2-ctl` advertises up to 1080p60 [B-24], more than a 2-lane link carries [C-48]. Because the two configurations carry different mode sets, the EDID loaded at start may need to differ per lane configuration. Its content is OQ-002; see [V4L2.md](V4L2.md).

### 4.12 ETHERNET

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Ethernet interface (on-board Pi Ethernet, carrier-board PHY, none) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018; Raspberry Pi Ethernet facts: DATASHEET REQUIRED — OQ-098 |
| Speed, PoE | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018, OQ-023; DATASHEET REQUIRED — OQ-098 |
| Network requirements that size it | UNKNOWN — VERIFICATION REQUIRED *(2026-10-09: WebRTC reach decided — viewers are on the LAN only; internet viewers are not in current scope (owner, 2026-10-09; OQ-008; OQ-128, RISK-033). Viewer count, browsers and bitrates are still open (OQ-008, OQ-005), and Ethernet hardware is still not researched (OQ-098), so the network requirements remain UNKNOWN.)* *(2026-10-09, bitrate decision: the live encode's bitrate is now set — CBR 17 Mbit/s (owner; OQ-005). The viewer count and browsers are still open (OQ-008) and Ethernet hardware is still not researched (OQ-098), so the network requirements remain UNKNOWN.)* | OWNER DECISION REQUIRED — OQ-007, OQ-008, OQ-075. ATEM network integration is not in current scope (OQ-009 ANSWERED 2026-10-07: HDMI capture only, REQ-ATEM-001). |

**Reference facts from sources**

- The 2026-10-06 research collected **no** fact about Raspberry Pi or Compute Module Ethernet hardware (PHY, speed, PoE). DATASHEET REQUIRED (Raspberry Pi product documentation) before this section can describe candidates — OQ-098. *(2026-10-09: research topic J covered USB and storage only; Ethernet is still not researched, and OQ-098 stays OPEN for it. The one PoE-related fact topic J found is that a 5 V-only PoE HAT cannot power the CM4 IO Board's PCIe slot [J-05], §4.9.)*
- Facts that make network hardware relevant:
  - FFmpeg RTMP URLs use default TCP port 1935 [F-32].
  - *Not in current scope; kept as reference.* The owner's answer of 2026-10-07 limits the ATEM integration to HDMI capture of the ATEM output (REQ-ATEM-001, REQ-CAP-008; OQ-009 ANSWERED). Network tally/control and RTMP exchange were offered and not selected, so the next two facts size nothing unless the owner adds them:
    - ATEM control software uses a custom UDP protocol on port 9910, reported by the OpenSwitcher project as reverse-engineered [F-11]. The official ATEM SDK manual does not document the wire protocol [F-10].
    - The ATEM Mini Pro streams over RTMP or SRT, either through its 10/100/1000 BaseT Ethernet port or through a shared internet connection over USB-C [F-26]. Reasoning: for PACSCORDER to receive that stream it must run a listening RTMP server [F-46].

### 4.13 USB

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| USB host ports exposed by the product | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018; Raspberry Pi USB facts: DATASHEET REQUIRED — OQ-098 |
| USB device-mode path for factory provisioning (Compute Module carriers) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-018, OQ-071 |
| Port and USB speed used by the recording HDD (ADR-009) *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED. On CM4 only USB 2.0 exists [J-01], [J-08]; on CM5 a USB 3.0 port is available [J-18]. | OWNER DECISION REQUIRED — OQ-018; §4.13.1 |
| USB-to-SATA bridge (chipset, VID:PID, UAS or Bulk-Only) *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED (no enclosure chosen) | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED — OQ-122 (TEST-REC-001, TEST-PERF-001); RISK-027 |
| CM4 USB host controller on the product image (`otg_mode=1` XHCI or `dwc2`) *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED | HARDWARE TEST REQUIRED — OQ-122; configuration in [DEVICE_TREE.md](DEVICE_TREE.md) §6.2 |

**Reference facts from sources**

- The 2026-10-06 research collected no fact about Raspberry Pi USB host ports. DATASHEET REQUIRED — OQ-098. *(Superseded in part 2026-10-09: research topic J — CM4, CM5 and their IO Boards are covered in §4.13.1. USB on Pi 4 Model B and Pi 5 is still not researched: DATASHEET REQUIRED — OQ-098.)*
- On CM5, the pins that carry CAM0 on CM4 (128–142) carry USB 3.0 [C-05]. Reasoning from [C-05]: a CM4-style carrier therefore has no CAM0 camera port with CM5.
- **Provisioning.** `rpiboot` (usbboot) makes a device appear as USB mass storage for provisioning. It supports Pi 4B, CM4, Pi 5 and CM5, among others. On Pi 4B it must first be enabled by permanently programming an OTP GPIO [G-45]. The Compute Module documentation flashes eMMC by fitting nRPI_BOOT (J2) and running `rpiboot` [G-47]. Reasoning: a Compute Module carrier needs a USB device-mode path and a boot-select means for factory flashing (OQ-018, OQ-071).
- The ATEM Mini USB-C port acts as a webcam output. Blackmagic documents Mac and Windows use and makes no statement about Linux or UVC [F-27]. *Not in current scope; kept as reference:* OQ-009 was answered on 2026-10-07 with HDMI capture only (REQ-ATEM-001, REQ-CAP-008), so this matters only if the owner adds a USB path (OQ-082).
- USB storage is one of the recording-medium options in OQ-006. *(Superseded 2026-10-09: research topic J / ADR-009 — the owner chose a USB-to-SATA HDD as one of the two mirrored recording targets on 2026-10-08; see §4.13.1.)*

#### 4.13.1 USB for the recording HDD (ADR-009) (added 2026-10-09)

*Added 2026-10-09; research topic J / ADR-009.* Every recording is also written to a USB-to-SATA HDD in a self-powered enclosure (ADR-009). Topic J covered CM4, CM5 and their IO Boards; Pi 4 Model B and Pi 5 USB were not researched (OQ-098).

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| USB available to the HDD | Not researched in topic J: DATASHEET REQUIRED (OQ-098) | One USB 2.0 High-Speed port, 480 Mbit/s signalling [J-01]; no USB 3.0 controller, so USB 3 on a CM4 carrier needs an external xHCI (for example a VLI805) on the PCIe link [J-08] | Not researched in topic J: DATASHEET REQUIRED (OQ-098) | Two USB 3.0 SuperSpeed interfaces, each up to 5 Gb/s at the same time, plus one USB 2.0 interface (480 Mb/s), which needs `dtoverlay=dwc2,dr_mode=host` [J-18] |
| Host controller | — | USB is off by default. The CM4 IO Board datasheet enables it with `dtoverlay=dwc2,dr_mode=host`; Raspberry Pi OS enables `otg_mode=1` by default on CM4, which selects a "more capable XHCI USB 2.0 controller" [J-07] | — | RP1 has two identical USB 3.0 xHCI controllers (Synopsys dwc_usb3); every downstream port has independent, uncontended bandwidth [J-21]. Both RP1 USB controllers are enabled in the CM5 device tree [J-22]. |
| UAS binding | — | `uas` refuses to bind behind `dwc2`, which reports no scatter-gather, so a UASP bridge runs under `usb-storage` (Bulk-Only); under Raspberry Pi OS's default `otg_mode=1` controller `uas` can bind [J-24] | — | Not stated separately in the register for RP1; the kernel's bridge quirks apply on CM4 and CM5 [J-25] |
| On the IO Board | — | CM4 IO Board: an on-board USB 2.0 hub (USB2514B) on the single CM4 USB 2.0 port; two ports go to the stacked Type-A connector and two to an internal header; one current-limit switch of about 1.2 A supplies VBUS to the connectors; plugging in the micro-USB cable disables the hub [J-06] | — | CM5 IO Board: two USB 3.0 Type-A ports sharing about 1.2 A of VBUS, and one USB 2.0 Type-C port intended mainly for data transfer and `rpiboot` [J-19] |
| Path to the SoC | — | Reasoning (CORRECTED): the CM4 IO Board's single PCIe Gen 2 x1 socket is the only PCIe link, so with the NVMe SSD fitted there is no PCIe left for an xHCI unless a PCIe switch is added, and booting through a switch is not supported; without a switch the HDD runs at USB 2.0 through the on-board hub, shared with every other USB device [J-09], [J-04] (RISK-026) | — | RP1 connects to the BCM2712 over PCIe 2.0 x4, with a maximum unidirectional bandwidth of 14.7 Gbit/s [J-22]; the NVMe link is a separate controller, so the M.2 SSD and the RP1-attached HDD do not share a PCIe root port [J-17]. Reasoning (CORRECTED): CSI-2 capture, Gigabit Ethernet egress and the HDD writes together use about 20.5 % of the RP1 link with 1080p60 UYVY capture, or about 27 % with RGB888 [J-23]. |

- **Bandwidth (reasoning) [J-37].** Even the highest recording rate in the research, 25.192 Mbit/s, is about 5.2 % of USB 2.0's 480 Mbit/s signalling rate and about 0.63 % of the ~4 Gbit/s of a USB 3.0 or PCIe Gen 2 x1 link, so interface bandwidth is not the recording bottleneck on either module. On CM4 the USB 2.0 bus (60 MB/s raw) limits the HDD only for offload and copy times. Real sustained throughput and CPU cost on CM4's shared hub are unmeasured (research open question, topic J; OQ-122). *(2026-10-09, bitrate decision: 25 Mbit/s is now the owner's recording bitrate (OQ-005), so 25.192 Mbit/s — with the research's assumed 192 kbit/s AAC — is the rate each drive receives at the decided setting, not only the top of an example range (reasoning [J-37]).)*
- **USB-to-SATA bridges.** The kernel applies built-in bridge quirks before binding: for example, some ASMedia bridges below SuperSpeed get IGNORE_UAS, all Seagate enclosures (VID 0x0bc2) get NO_ATA_1X, and one RTL9210 enclosure gets IGNORE_UAS; user `usb-storage.quirks` entries are merged afterwards [J-25]. The parameter takes `VID:PID:Flags` entries, where `u` means IGNORE_UAS [J-26]. A sticky forum post by a Raspberry Pi engineer reports that some UAS devices that do not fully implement the UAS specification stop responding, or in rare cases throw write data away, which can corrupt the filesystem, and gives `usb-storage.quirks=…:u` in `cmdline.txt` as the workaround (community source, CORRECTED) [J-27]. Raspberry Pi's documentation warns that USB SATA adapters supported by the bootloader in mass-storage mode can fail if Linux selects UAS mode [J-28]. RISK-027; OQ-122. The kernel command-line item is in [BUILD_SYSTEM.md](BUILD_SYSTEM.md).
- **Reasoning from [J-24]:** UAS behaviour can differ between CM4 and CM5, so a bridge qualified on one board is not thereby qualified on the other (RISK-027).
- **Commands.** The `config.txt` and `cmdline.txt` settings in this subsection are quoted from the sources: NOT YET RUN ON PACSCORDER HARDWARE.

### 4.14 STORAGE

**PACSCORDER value**

| Attribute | Value | Resolution |
|---|---|---|
| Boot medium (SD card, eMMC, USB, NVMe) | UNKNOWN — VERIFICATION REQUIRED *(2026-10-09: still not decided; NVMe boot facts in §4.14.1; what NVMe boot needs on CM4 and CM5 is OQ-124)* | OWNER DECISION REQUIRED — OQ-006, OQ-018; boot-storage options per board: DATASHEET REQUIRED — OQ-098 |
| Recording medium and capacity | UNKNOWN — VERIFICATION REQUIRED *(2026-10-09: medium decided by the owner on 2026-10-08 — a PCIe NVMe SSD and a USB-to-SATA HDD, every recording mirrored to both (ADR-009, ACCEPTED). Capacity and models are still UNKNOWN; the maximum recording duration that sizes them is still OPEN, OQ-006.)* *(Superseded 2026-10-09, later: no fixed duration limit — recordings run until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED). Reasoning from [J-36]: the capacity therefore sets how long a recording can run rather than being sized from a duration — about 271 h per 1 TB at 8 Mbit/s and about 88 h at 25 Mbit/s, for the research's example bitrates (bitrate still OQ-005). Capacity and models are still UNKNOWN (OQ-121, OQ-122). One drive full, absent or failed before the other, and file splitting: OQ-129.)* *(Superseded in part 2026-10-09, bitrate and drive-failure decisions: the recording bitrate is 25 Mbit/s, VBR (owner; OQ-005). Reasoning from [J-36] (192 kbit/s AAC assumed there; decimal units; container overhead excluded): each drive receives about 3.15 MB/s, about 11.34 GB per hour, so capacity sets the recording time at about 88 h per 1 TB; both mirror copies together write about 6.30 MB/s. Drive capacities and models are still UNKNOWN (OQ-121, OQ-122). If one drive fills, is absent or fails during a recording, recording continues on the other drive and the operator is alerted (owner; OQ-129); file splitting, the alert method (OQ-091) and drive return are still open.)* *(Superseded in part 2026-10-09, later still: file splitting decided — a new file every 30 minutes, about 5.67 GB per file per drive at 25 Mbit/s (reasoning from [J-36], with its assumed 192 kbit/s AAC); a recording started with one drive missing runs on the available drive with an operator alert (owner; OQ-129). Reasoning: capacity still sets the total recording time; splitting changes only the file size. Still open: drive return (OQ-129); alert method (OQ-091).)* | OWNER DECISION REQUIRED — OQ-006 *(2026-10-09: OQ-006 ANSWERED; capacity and models OQ-121, OQ-122; follow-up OQ-129)* *(2026-10-09, later: recording bitrate decided, OQ-005; OQ-129 policy decided, remainder OPEN)* *(2026-10-09, later still: start on the available drive and 30-minute splitting decided; OQ-129 OPEN only for drive return; alert method OQ-091)* |
| Partition layout and update scheme | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED — OQ-068, OQ-069 |
| Behaviour on power loss during recording | UNKNOWN — not researched *(Superseded 2026-10-09: research topic J / ADR-009 — the owner chose fragmented MP4 as the power-loss strategy on 2026-10-08. How much is still lost on each drive and board is unmeasured: HARDWARE TEST REQUIRED, OQ-119; RISK-030.)* | OWNER DECISION REQUIRED — OQ-006 |
| NVMe SSD: model, form factor, firmware; CM4 PCIe-to-M.2 adaptor *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED (none chosen) | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED — OQ-121, OQ-123 (CM4 interrupt mode), OQ-124 (CM5 M.2 enablement) |
| HDD: model, size (2.5-inch or 3.5-inch), enclosure and its supply *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED (none chosen). The enclosure is self-powered (ADR-009). | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED — OQ-122 |
| Recording-volume filesystem and mount options *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED. ADR-009 proposes ext4; that is Claude's proposal within the ADR, not an owner decision. | OWNER DECISION REQUIRED — OQ-120 |
| Drive write caches (SSD and HDD) *(added 2026-10-09)* | UNKNOWN — VERIFICATION REQUIRED; not in the register | VENDOR CONFIRMATION REQUIRED — OQ-119, OQ-121, OQ-122 |

**Reference facts from sources**

- Storage and provisioning differ between the candidate boards: SD card or eMMC flashed with `rpiboot` (reasoning-tier entry, CORRECTED) [G-71]. Compute Module eMMC flashing uses `rpiboot` [G-47]. The boot-storage options of each candidate board (SD, eMMC, NVMe) were not otherwise researched: DATASHEET REQUIRED — OQ-098. *(Superseded in part 2026-10-09: research topic J — NVMe boot on CM4, CM5 and Pi 5 is documented [J-10], [J-11]; see §4.14.1. Other boot options of Pi 4 Model B and Pi 5 are still DATASHEET REQUIRED — OQ-098.)*
- The Raspberry Pi OS Lite (64-bit) 2026-10-06 download is 550,466,056 bytes compressed and 3,078,619,136 bytes expanded [G-33].
- Raspberry Pi Connect A/B updates need a storage device of at least 16 GB (CORRECTED) [G-43].
- The `rpi-image-gen` `image-rota` layout provides A/B slots and a shared persistent data partition [G-40].
- `raspi-config` offers a read-only root through `overlayroot=tmpfs` [G-48].
- See [RECORDING.md](RECORDING.md) and [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

#### 4.14.1 Recording storage per ADR-009 (added 2026-10-09)

*Added 2026-10-09; research topic J / ADR-009.* The owner decided on 2026-10-08 (ADR-009, ACCEPTED): MP4 written fragmented, every recording mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD, the HDD in a self-powered enclosure. ext4 on the recording volumes is Claude's proposal inside ADR-009, not an owner decision (OQ-120). The USB side is in §4.13.1 and the power side in §4.9. Topic J covered CM4, CM5 and their IO Boards; for Pi 4 Model B it found nothing, and for Pi 5 only NVMe boot and the Gen 3 warning.

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| PCIe link for the NVMe SSD | Not researched in topic J: DATASHEET REQUIRED (OQ-098) | One PCIe 2.0 x1 host (Gen 2, 5 Gbps), the module's only PCIe lane [J-01]. The PCIe host controller does not support 64-bit accesses from the ARM; the CM4 datasheet says kernels 5.10 and newer support MSI-X with up to 32 IRQs, while the CM4 IO Board datasheet says the interface does not support MSI-X and devices typically fall back to MSI [J-02] (OQ-123) | Raspberry Pi's PCIe documentation warns that Pi 5 is not certified for Gen 3.0 speeds and that Gen 3.0 connections might be unstable; Gen 3 is enabled with `dtparam=pciex1_gen=3` or through `raspi-config` [J-16] (quoted from the source, not proposed; NOT YET RUN ON PACSCORDER HARDWARE). Other Pi 5 PCIe facts were not researched (OQ-098) | One-lane PCIe Gen 2 (5 Gb/s) for NVMe and other peripherals; Gen 3 "is possible in some cases, but is unsupported and might not function reliably" [J-12] |
| Reference carrier slot | — | CM4 IO Board: one PCIe Gen 2 x1 socket for standard PC PCIe cards; Raspberry Pi states it has been used with an NVMe drive through a passive PCIe adaptor [J-03]. Raspberry Pi's NVMe boot documentation suggests searching for a "PCI-E 3.0 ×1 lane to M.2 NGFF M-Key SSD NVMe PCI Express adapter card" [J-11]. Booting through a PCIe switch is not supported [J-04]. Slot power only from the +12 V barrel input [J-05] (§4.9) | — | CM5 IO Board: M.2 M-key connector at PCIe Gen 2 ×1 by default, Gen 3 ×1 "experimental and therefore unsupported" [J-13]; 2230, 2242, 2260 and 2280 form factors [J-14] |
| Device Tree state of the link | — | Not in the register: KERNEL SOURCE INSPECTION REQUIRED | Not in the register for the Pi 5 board file | In `rpi-6.18.y` (6.18.55) the M.2 link `pcie1` is disabled, and neither the CM5 dtsi nor the CM5 IO Board files enable it; the `pciex1` dtparam (alias `nvme`) defaults to off and `pciex1_gen` to 2 [J-15], [J-17]. Whether firmware enables it at run time is unknown (OQ-124). Configuration: [DEVICE_TREE.md](DEVICE_TREE.md) §6.4. |
| Device name in Linux | — | The SSD appears as `/dev/nvme0`, namespace `/dev/nvme0n1` [J-11] | — | As CM4 [J-11] |
| NVMe boot | — | `BOOT_ORDER` nibble 0x6; for Compute Modules set `BOOT_ORDER=0xf6` in usbboot `recovery/boot.conf`, run `update-pieeprom.sh` and flash with `rpiboot`; CM4 Lite boots NVMe automatically when the SD slot is empty, and eMMC CM4s must put NVMe first [J-10]. Boot medium not decided (OQ-124) | `BOOT_ORDER` nibble 0x6 is documented for Pi 5 [J-10] | `BOOT_ORDER` nibble 0x6 is documented for CM5; Compute Module procedure as CM4 [J-10]. Whether CM5 uses usbboot `recovery5` and whether `PCIE_PROBE` is needed are research gaps (topic J; OQ-124) |

- **Bandwidth (reasoning).** Per destination, H.264 at 8 / 12 / 25 Mbit/s plus 192 kbit/s AAC writes about 1.02 / 1.52 / 3.15 MB/s; mirroring doubles the system total to about 2.05–6.30 MB/s; a 1 TB disk holds about 271 h at 8 Mbit/s or about 88 h at 25 Mbit/s [J-36]. The recording rate is about 0.63 % of a PCIe Gen 2 x1 link [J-37]. Research recommends staying at Gen 2, because Gen 3 would add risk without benefit (research design risk, topic J; OQ-124). *(2026-10-09, bitrate decision: 25 Mbit/s is now the owner's recording bitrate (OQ-005). At that rate, reasoning from [J-36]: about 3.15 MB/s and 11.34 GB per hour per drive, about 88 h per 1 TB, and about 6.30 MB/s for both mirror copies. Reasoning: with VBR the written rate varies around the setting, so measured sizes will differ (TEST-REC-001). 8 and 12 Mbit/s stay examples.)* *(2026-10-09, later still: recordings are split into a new file every 30 minutes (owner; OQ-129). Reasoning from [J-36]: at 25 Mbit/s plus the research's assumed 192 kbit/s AAC, each file is about 3.15 MB/s × 1,800 s ≈ 5.67 GB per drive.)*
- **HDD timing.** The register's example 2.5-inch HDD takes 2.5 s typical and 3.0 s maximum from standby to ready, and 2.8 / 3.3 s typical, 3.0 / 3.5 s maximum (1-disk / 2-disk) from power-on to ready; its maximum sustained OD read rate is 140 MB/s (CORRECTED) [J-30]. A stall of that length must not reach the NVMe copy or the live path (ADR-009 Consequences; RISK-028, OQ-117). Whether the chosen drive or bridge spins down on its own is UNKNOWN (VENDOR CONFIRMATION REQUIRED; OQ-117). *(2026-10-09, drive-failure decision: if a drive fills, is absent or fails, rather than stalls, the recording continues on the other drive and the operator is alerted (owner; OQ-129); RISK-028 concerns a slow drive, not a failed one.)* *(2026-10-09, later still: if one drive is already missing when a recording is started, the recording starts on the available drive and the operator is alerted (owner; OQ-129).)*
- **Commands.** The `BOOT_ORDER`, `boot.conf`, `update-pieeprom.sh`, `rpiboot` and `dtparam` settings in the table above are quoted from the sources: NOT YET RUN ON PACSCORDER HARDWARE.
- **Kernel support.** The `rpi-6.18.y` defconfigs for CM4 (`bcm2711_defconfig`) and CM5 (`bcm2712_defconfig`) build USB storage, UAS, NVMe, ext4 and VFAT into the kernel, and exFAT and NTFS3 as modules [J-29]. Image details: [BUILD_SYSTEM.md](BUILD_SYSTEM.md).
- **Not in the register (UNKNOWN — VERIFICATION REQUIRED):** the current rating of the CM4 IO Board's 12 V-to-3.3 V slot converter and of the CM5 IO Board's M.2 3.3 V supply, against the chosen SSD's peak current (research gap, topic J; DATASHEET REQUIRED; OQ-121); whether the chosen adaptor fits the CM4 IO Board socket mechanically (research open question, topic J; OQ-121); the drives' write-cache behaviour at power loss (OQ-119).

---

## 5. Per-platform reference table

Reference facts for each candidate platform. **None of these is a PACSCORDER value** until the platform for each lane configuration is chosen (ADR-004, OQ-011; REQ-CAP-007) and the hardware is checked.

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Camera connector(s) | One 15-pin, 1.0 mm pitch, 16 mm wide [C-01] | Carrier-dependent. CM4 IO Board: two 22-pin, 0.5 mm pitch [C-03] | Two mini 22-pin, 0.5 mm pitch, combined CSI/DSI [C-04] | Carrier-dependent. MIPI0 on CM4 CAM1 pins, MIPI1 on CM4 DSI1 pins [C-05]. CM5 IO Board: two 22-pin CAM/DISP [C-06] |
| CSI-2 data lanes | 2 [C-01]; DT limits csi1 to 2 [B-48] | CAM0: 2, CAM1: 4 [C-02], [B-48] | 4 per port, 1.5 Gbps per lane [C-04] | 4 per interface [C-05] |
| Lane configuration it can serve (REQ-CAP-007; §4.11) | 2-lane only [C-01] | CAM0: 2-lane only; CAM1: 4-lane [C-02] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | as Pi 5; carrier-dependent (OQ-052) |
| CSI-2 receiver | Unicam, csi1 [C-08], [C-09] | Unicam: csi0 (2-lane), csi1 (4-lane) [C-08] | RP1 CFE: `rp1_csi0`, `rp1_csi1` [C-29] | RP1 CFE [C-29], [B-44] |
| Camera I2C bus | `i2c_csi_dsi` = `/dev/i2c-10`, a pinctrl-mux channel of i2c0 on GPIO 44/45 [C-24] | CAM1: `i2c-10`; CAM0: `i2c-0`. CM4 IO Board: GPIO 44/45 and GPIO 0/1 [C-25], [C-24]. CAM0 on the IO Board needs the J6 jumpers [C-03] | CAM/DISP0: RP1 i2c6, GPIO 38/39, `/dev/i2c-10`. CAM/DISP1: RP1 i2c4, GPIO 40/41, `/dev/i2c-11` [C-26]; i2c4 runs at 100 kHz [B-44]. Earlier kernels used other numbers (reported) [C-28] | CM5 IO Board: CAM/DISP1 RP1 i2c0 on GPIO 0/1 (symlink `i2c-11`), CAM/DISP0 RP1 i2c6 on GPIO 38/39. CM4 IO Board: `i2c_csi_dsi1` (CAM1, DISP1, RTC, fan) is i2c6, `i2c_csi_dsi0` is i2c0 [C-27] |
| Camera power-enable GPIO (stock DT) | `cam1_reg` on expander GPIO 5; `cam0_reg` dummy [C-21] | Expander GPIO 5, shared by both ports [C-21] | MIPI0: RP1 GPIO 34; MIPI1: RP1 GPIO 46 [C-22] | RP1 GPIO 34 (CAM_GPIO0), shared [C-22]. CM5 IO Board: only CAM/DISP 0 can power down a camera [C-06] |
| Stock TC358743 overlay | `tc358743` [G-12] | `tc358743` [G-12] | `tc358743` redirected to `tc358743-pi5` [C-11] | as Pi 5 [C-11], [E-43] |
| 1080p60 capture by bandwidth (a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY: 3 of 4 lanes at 972 Mbit/s, OQ-038; ADR-008) | No; limit 1080p50 UYVY or 1080p30 RGB888, the 2-lane configuration ceiling [C-37], [C-48] | By bandwidth on CAM1 [C-37], [C-49] | By bandwidth (reasoning) [C-49]. On CAM/DISP0 (`cam0`), whether `4lane` gives 4 lanes on `csi0` is NEEDS VERIFICATION (OQ-049). No official TC358743 documentation [C-38] | By bandwidth (reasoning) [C-49]; carrier-dependent (OQ-052). No official TC358743 documentation [C-38] |
| Hardware H.264 encode | 1080p30 specified [D-10] | 1080p30 specified [D-10] | None [D-31], [G-22] | None [D-31] |
| Hardware H.265 encode (added 2026-10-08, when H.265 was required by REQ-ENC-001; deferred — REQ-ENC-002; not in current scope) | None [D-24] | None [D-24] | None [D-31] | None [D-31] |
| x265 Neon DotProd kernels for software H.265 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | Cannot apply: Cortex-A72 (reasoning: same BCM2711 as CM4 [D-10], [H-05]) | Cannot apply: Cortex-A72 [H-05] | Can apply: Cortex-A76 (reasoning: same BCM2712 as CM5 [I-06], [H-05]) | Can apply: Cortex-A76 [H-04], [H-05] |
| HDMI audio I2S (`tc358743-audio`; audio required, REQ-CAP-006; added 2026-10-08; §4.6.1) | Not covered by topic I: KERNEL SOURCE INSPECTION REQUIRED | `bcm2835-i2s`, GPIO 18–21 [I-10], 2 channels [I-13] | Labels in `bcm2712-rpi.dtsi` [I-07]; Pi 5 inclusion not in the register (OQ-054) | RP1 I2S1, GPIO 18–21 [I-07], [I-08]; capture unconfirmed (OQ-054) |
| PCIe for the NVMe recording SSD (ADR-009; added 2026-10-09; §4.14.1) | Not researched in topic J (OQ-098) | One PCIe Gen 2 x1 lane [J-01]; MSI-X support disputed [J-02] (OQ-123). CM4 IO Board: one x1 socket, NVMe through a passive adaptor [J-03], slot powered only from 12 V [J-05] | Gen 3 not certified [J-16]; otherwise not researched (OQ-098) | One PCIe Gen 2 lane; Gen 3 unsupported [J-12]. CM5 IO Board: M.2 M-key, 2230–2280 [J-13], [J-14]; link disabled by default in the `rpi-6.18.y` device tree [J-15], [J-17] (OQ-124) |
| USB for the recording HDD (ADR-009; added 2026-10-09; §4.13.1) | Not researched in topic J (OQ-098) | USB 2.0 only [J-01], [J-08]. CM4 IO Board: USB2514B hub, one ~1.2 A VBUS switch for all ports [J-06]; HDD shares USB 2.0 with every other USB device (reasoning, CORRECTED) [J-09] | Not researched in topic J (OQ-098) | Two USB 3.0 interfaces [J-18] on RP1 xHCI controllers [J-21]. CM5 IO Board: two USB 3.0 ports sharing ~1.2 A VBUS [J-19] |
| NVMe boot (`BOOT_ORDER` 0x6; added 2026-10-09) | Not among the models [J-10] documents it for (CM4, CM5, Pi 5 and 500+ only) | Documented [J-10] | Documented [J-10] | Documented [J-10]; CM5 specifics OQ-124 |
| Bring-up role (owner, 2026-10-07; ADR-004 OPEN) | Documented candidate | Evaluated side by side with CM5 | Documented candidate | Evaluated side by side with CM4 |
| Platform-specific hazards | 2 lanes only; never declare 4 lanes [C-17] | CAM0 is 2-lane [C-02] | Stale README text for `tc358743-pi5` [C-13] | CM4 CAM0 pins are USB 3.0 on CM5 [C-05]; CAM/DISP 1 on the CM5 IO Board needs J6 jumpers [C-06] |

## 6. Candidate boards named in sources (not selected)

No board has been selected. The entries below appear in the sources. They are listed so that a new engineer knows what the research looked at. **Being listed here is not a recommendation.**

| Board | What the source says | Tier | Status |
|---|---|---|---|
| Auvidea B101 (TC358743 bridge) | A Raspberry Pi engineer reported that it uses a 15-pin FFC with contacts on the same side, and warned about wrongly sided adapters on Pi 5 [C-45] | community | Named in sources, not selected |
| Auvidea B102 (TC358743 bridge) | Raspberry Pi engineers reported a B102 probing on Pi 5 in December 2023 [C-41] | community | Named in sources, not selected |
| Raspberry Pi CM4 IO Board (carrier) | Connectors, lanes and I2C mapping [C-03], [C-25]. *(Added 2026-10-09.)* PCIe Gen 2 x1 socket [J-03], 12 V-only slot power [J-05], USB 2.0 hub with one ~1.2 A VBUS switch [J-06] | official-rpi; datasheet | Named in sources, not selected |
| Raspberry Pi CM5 IO Board (carrier) | Connectors, J6 jumpers, power-down signal [C-06]; I2C mapping [C-27]. *(Added 2026-10-09.)* M.2 M-key PCIe Gen 2 ×1 slot, 2230–2280 [J-13], [J-14]; two USB 3.0 ports sharing ~1.2 A [J-19]; USB-C PD power [J-20] | official-rpi / kernel-source; datasheet | Named in sources, not selected |
| Recording storage parts: NVMe SSD, CM4 PCIe-to-M.2 adaptor, HDD, self-powered USB-to-SATA enclosure *(added 2026-10-09)* | None chosen (OQ-121, OQ-122). The Seagate BarraCuda 2.5-inch and 3.5-inch HDD families appear in the register only as datasheet examples of spin-up current, timing and supply voltage [J-30], [J-32]. The bridge chipsets in [J-25] are named only as kernel-quirk examples. | datasheet; kernel-source | Named in sources, not selected |
| Geekworm C77x / C779 / C790, Waveshare, X1301 | Named only in the research questions and gaps. There is **no** register entry, so nothing about them is stated here. | — | Named in research notes only, not selected |

For every board above, oscillator frequency, routed lanes, connector pinout, INT/RESETN wiring and CAM_GPIO use are UNKNOWN — VERIFICATION REQUIRED (OQ-018 to OQ-022). Whether one bridge-board design can serve both the 2-lane and the 4-lane configuration (REQ-CAP-007) is also UNKNOWN (OQ-021).

## 7. Source-derived hazards to check before first power-up

This is a list of facts to check against the chosen hardware. It is **not** a procedure and nothing in it has been done. Procedures belong in [TESTING.md](TESTING.md) and are NOT YET RUN ON PACSCORDER HARDWARE.

| # | Hazard | Evidence | Linked |
|---|---|---|---|
| 1 | A wrongly sided FFC or adapter can swap GND and 3V3 and damage either board (reported) | [C-45] | RISK-021, OQ-021 |
| 2 | A REFCLK value in the DT that is not 26, 27 or 42 MHz leads to a kernel BUG | [A-22], [B-11] | RISK-007, OQ-019 |
| 3 | Declaring 4 lanes on a 2-lane connector is not rejected at probe | [C-17] | OQ-021 |
| 4 | CM4 IO Board CAM0 and CM5 IO Board CAM/DISP 1 need J6 jumpers for I2C | [C-03], [C-06] | OQ-021, OQ-052 |
| 5 | CM5 on a CM4-style carrier: the CAM0 pins carry USB 3.0 | [C-05] | OQ-052 |
| 6 | A bridge board that depends on CAM_GPIO stays unpowered or in reset with the stock overlay (reasoning) | [C-51], [C-23] | OQ-022 |
| 7 | VDDIO2 sets the logic level of every Pi-facing TC358743 pin | [A-36], [A-39] | OQ-024 |
| 8 | The TC358743 is weak against ESD and its HPDO pin is not listed as 5 V tolerant | [A-41], [A-39] | OQ-024 |
| 9 | Ambient temperature at the TC358743 must stay within −30 to +70 °C | [A-41] | OQ-010 |
| 10 | Another device at 0x0f on the camera I2C bus would collide with the TC358743 (reasoning) | [A-15], [C-27] | OQ-026 |
| 11 | *(Added 2026-10-08.)* The TC358743 audio outputs run at VDDIO2; the CM4 and CM5 IO Boards have a selectable 1.8 V or 3.3 V GPIO voltage, which VDDIO2 should match, or the audio lines need level shifting | [I-27], [I-29] | OQ-024, OQ-025 |
| 12 | *(Added 2026-10-08.)* Loading `tc358743-audio` claims GPIO 18–21; the `pwm`, `pwm-2chan` and `gpio-ir` overlays default to GPIO 18, and `audremap` offers `pins_18_19` on BCM2711 | [I-30], [I-31] | OQ-114 |
| 13 | *(Added 2026-10-09.)* On the CM4 IO Board the PCIe slot, and so the NVMe SSD, is powered only from the +12 V barrel input; with a 5 V-only PoE HAT, PCIe cards do not function | [J-05] | RISK-026, OQ-023, OQ-121 |
| 14 | *(Added 2026-10-09.)* A bus-powered HDD can exceed the board's VBUS limits: spin-up up to 1.0 A for the register's example 2.5-inch drive (one Seagate family) against about 1.2 A shared by all ports (CM4 IO Board, CM5 IO Board on 5 A) or a 600 mA peripheral limit (CM5 IO Board on a 3 A supply) (reasoning); a 3.5-inch HDD needs 12 V. ADR-009 requires a self-powered enclosure (owner decision; "or hub" is Claude's addition) | [J-30], [J-32], [J-06], [J-19], [J-20], [J-31] | ADR-009, OQ-122 |
| 15 | *(Added 2026-10-09.)* On the CM4 IO Board, plugging in the micro-USB cable disables the USB hub, and with it the HDD | [J-06] | OQ-122 |
| 16 | *(Added 2026-10-09.)* A USB-to-SATA bridge can misbehave in UAS mode: stop responding or, rarely, lose written data (reported) | [J-27], [J-28] | RISK-027, OQ-122 |
| 17 | *(Added 2026-10-09.)* On CM5 the M.2 link is disabled by default in the `rpi-6.18.y` device tree, and the `pciex1` dtparam defaults to off; reasoning: an SSD in the CM5 IO Board's M.2 slot may not enumerate unless the link is enabled (whether firmware enables it at run time is unknown) | [J-15], [J-17] | OQ-124 |

## 8. Hardware revision history

Rule 8 requires a revision history and forbids assuming that revisions are identical. **No PACSCORDER hardware revision has been defined.**

| Revision | Status | Date | Platform | Bridge board / PCB | Differences from previous revision | Evidence |
|---|---|---|---|---|---|---|
| HW REV A | Not yet defined | — | UNKNOWN — VERIFICATION REQUIRED | UNKNOWN — VERIFICATION REQUIRED | — (first revision) | — |
| HW REV B | Not yet defined | — | — | — | — | — |
| HW REV C | Not yet defined | — | — | — | — | — |

**When a revision is defined**, add a subsection for it with every field below. A field may be `UNKNOWN — VERIFICATION REQUIRED`, but it may not be left out or copied from another revision without checking.

- Raspberry Pi / CM model, RAM size and board revision, read from each unit.
- Compute Module carrier: name, revision and schematic reference.
- Bridge board or PCB: name, revision, schematic and BOM reference.
- TC358743 marking, ordering suffix and CHIPID revision byte (OQ-029).
- REFCLK oscillator: part and measured frequency (OQ-019).
- CSI-2: connectors, cable, routed data lanes, lane order (OQ-021), and which lane configuration of REQ-CAP-007 (2-lane or 4-lane) the revision implements.
- I2C: bus, measured address, pull-ups, other devices on the bus (OQ-026).
- RESETN and INT wiring, with GPIO numbers (OQ-020). CAM_GPIO use (OQ-022).
- Power input, rail generation, measured consumption (OQ-023, OQ-024).
- HDMI connector, HPD/+5V interface, ESD protection (OQ-024).
- HDMI audio (required, REQ-CAP-006): which TC358743 audio pins are wired to which Pi GPIOs, whether A_OSCK is connected, VDDIO2 versus the selected GPIO bank voltage, and the use of GPIO 21 (OQ-025, OQ-024, OQ-114). *(Added 2026-10-08.)*
- Ethernet, USB and storage as fitted (OQ-018, OQ-006, OQ-098).
- Recording storage (ADR-009): NVMe SSD model, firmware and form factor, and on CM4 the PCIe adaptor; HDD model, enclosure, its supply, and the USB-to-SATA bridge VID:PID with the driver it binds to on this board; the USB port used; the recording-volume filesystem and mount options (OQ-120, OQ-121, OQ-122). How the carrier powers the PCIe slot (12 V on the CM4 IO Board [J-05]) (OQ-023). *(Added 2026-10-09.)*
- Device Tree configuration used, by entry in the change log of [DEVICE_TREE.md](DEVICE_TREE.md).
- Tests run on this revision, with results in [TESTING.md](TESTING.md). Every test record names the hardware revision (Rule 9).

---

## Verification status

### Verified from sources (fact IDs)

Every reference fact in this document cites an entry of [REFERENCES.md](REFERENCES.md) whose verdict is `CONFIRMED` or `CORRECTED`. This document cites these entries:

| Topic | Fact IDs |
|---|---|
| A — TC358743 hardware | A-01, A-02, A-03, A-04, A-05, A-06, A-07, A-08, A-11, A-12, A-13, A-14, A-15, A-16, A-17, A-18, A-19, A-20, A-21, A-22, A-23, A-25, A-27, A-28, A-29, A-30, A-31, A-33, A-34, A-36, A-37, A-38, A-39, A-40, A-41, A-42, A-45, A-47, A-49, A-50 |
| B — tc358743 Linux driver | B-04, B-05, B-07, B-10, B-11, B-13, B-19, B-20, B-21, B-23, B-24, B-32, B-33, B-40, B-41, B-44, B-46, B-47, B-48 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-07, C-08, C-09, C-11, C-13, C-16, C-17, C-20, C-21, C-22, C-23, C-24, C-25, C-26, C-27, C-28, C-29, C-30, C-37, C-38, C-39, C-40, C-41, C-43, C-45, C-47, C-48, C-49, C-51 |
| D — Encoders | D-10, D-24, D-31 |
| E — Buildroot and kernel configuration | E-43, E-47, E-51 |
| F — ATEM and streaming | F-10, F-11, F-23, F-26, F-27, F-32, F-46 |
| G — Raspberry Pi OS and image tooling | G-12, G-14, G-20, G-22, G-33, G-40, G-43, G-45, G-47, G-48, G-71 |
| H — H.265/HEVC software encoding (added 2026-10-08) | H-04, H-05 |
| I — HDMI audio path (added 2026-10-08) | I-01, I-03, I-05, I-06, I-07, I-08, I-09, I-10, I-13, I-14, I-16, I-24, I-25, I-26, I-27, I-28, I-29, I-30, I-31, I-32 |
| J — Recording storage and power loss (added 2026-10-09) | J-01, J-02, J-03, J-04, J-05, J-06, J-07, J-08, J-09, J-10, J-11, J-12, J-13, J-14, J-15, J-16, J-17, J-18, J-19, J-20, J-21, J-22, J-23, J-24, J-25, J-26, J-27, J-28, J-29, J-30, J-31, J-32, J-36, J-37 |
| K — Live latency (added 2026-10-09) | K-30, K-33, K-34, K-35, K-38, K-39 |

- `CORRECTED` entries, used in their corrected wording only: A-22, A-25, B-11, B-21, B-44, C-28, C-39, E-47, G-43, G-71; added 2026-10-09: J-09, J-23, J-27, J-30.
- `community` entries, worded as reports: C-28, C-41, C-43, C-45, F-11; added 2026-10-08: I-16; added 2026-10-09: J-27, K-33.
- `reasoning` entries, labelled as reasoning: A-23, B-10, B-11, B-33, B-47, C-47, C-48, C-49, C-51, F-46, G-71; added 2026-10-08: I-28; added 2026-10-09: J-09, J-23, J-31, J-36, J-37. The 12 V conclusion inside the datasheet entry J-32 is the entry's own reasoning and is attributed to the entry.
- Statements marked *research gap* or *research notes* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). They are not register facts and are recorded only to state what is unknown. Statements marked *research gap* or *research open question* for topics H and I (added 2026-10-08) come from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json), with the same status. Statements marked *research gap*, *research open question* or *research design risk* for topics J and K (added 2026-10-09) come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json), with the same status.
- All cited J and K entries have verdict `CONFIRMED` or `CORRECTED`. The Seagate drives in [J-30] and [J-32] are datasheet examples, not PACSCORDER parts.
- *(Added 2026-10-09, later still; owner decisions on OQ-129 — start on the available drive; split every 30 minutes.)* The notes in the intro, §4.14 and §4.14.1 cite only [J-36] (reasoning), already listed above; no entry is new. The 5.67 GB file size is reasoning; no hardware value changed.
- "Verified from sources" means only that the cited source says so. It says nothing about PACSCORDER hardware.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-08). Every PACSCORDER value in this document is `UNKNOWN — VERIFICATION REQUIRED`. The owner decisions of 2026-10-07 (REQ-CAP-007, REQ-CAP-008; second set: CM4 and CM5 side by side, HDMI audio required under REQ-CAP-006, H.264 and H.265 under REQ-ENC-001, any HDMI camera plus ATEM outputs under OQ-102) are requirements and plans, not hardware evidence. *(2026-10-08: the codec decision was narrowed to H.264 only — OQ-103 ANSWERED; H.265 deferred under REQ-ENC-002. That is also a requirement decision, not hardware evidence.)* *(2026-10-09: the owner decisions of 2026-10-08 on recording storage — ADR-009 — and on live latency — OQ-116 — are decisions, not hardware evidence. No storage part has been chosen or tested; still nothing is verified on PACSCORDER hardware as of 2026-10-09. The storage values of §4.13.1 and §4.14.1 would be confirmed by TEST-REC-001 and TEST-PERF-001, both `BLOCKED — HARDWARE REQUIRED`.)* The hardware tests that would confirm values in this document (TEST-HW-001, TEST-DRV-001, TEST-DRV-002, TEST-PLT-001, TEST-CAP-001 to TEST-CAP-004, TEST-AUD-001, TEST-PERF-001) are `BLOCKED — HARDWARE REQUIRED`, as is every other hardware-dependent test in [TESTING.md](TESTING.md).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review against REFERENCES.md: narrowed wording to what the cited facts state (A-08 timings capability, A-15 address sources, BCM2712 kernel page size per E-51/G-20, F-26 streaming path, C-43 attribution); added missing citations and reasoning labels (REFCLK specification gap, DT clock consequence, CM5 CAM0, CAM_GPIO, A_OSCK, clock lane); corrected research-note attributions (lifecycle, VDDIO2); completed the hardware-test status statement. No hardware facts added. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: OQ-098 (Raspberry Pi Ethernet, USB and storage facts) linked in the §2 summary and in §4.12 ETHERNET, §4.13 USB, §4.14 STORAGE and the §8 revision checklist; 4-lane port marked necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s, OQ-038; ADR-008) in §4.11 and §5, and Pi 5 CAM/DISP0 4-lane conditioned on OQ-049; link rate linked to ADR-008 (PROPOSED) and OQ-099; TC358743 audio note now records that the silicon can also send audio over CSI-2 [A-05] while the driver configures I2S [A-13], with the wiring consequence labelled reasoning. Added citation A-13. No hardware facts added; no status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: both 2-lane and 4-lane configurations required (REQ-CAP-007; OQ-001 ANSWERED) in the header, intro, §1, §2, §3, §4.1, §4.4, §4.11 and §5; new §4.11 subsection maps each candidate connector to the lane configuration it can serve and gives the per-configuration supported-mode limits [C-37], [C-48], [C-49], [B-33] and the per-configuration EDID note (OQ-002, reasoning [B-24]); "Pi 4 Model B excluded if 1080p60 is mandatory" replaced by "excluded from the 4-lane configuration, still a 2-lane candidate" (§4.1); HDMI sources = ATEM outputs and cameras (REQ-CAP-008, OQ-102 [F-23]) in §3 and §4.3; ATEM network/USB facts in §4.12 and §4.13 labelled "not in current scope" (OQ-009 ANSWERED; REQ-ATEM-001); one-board-for-both question (OQ-021) in §1, §4.11 and §6; §8 checklist records the lane configuration; §4.1 software-image note references REQ-BLD-002 (own OS image; ADR-003 still PROPOSED). Added citations B-24, B-33, F-23. No platform chosen; no ADR status changed; no hardware facts added. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §4.1 "Software image" bullet now says the build tool is ADR-003 (ACCEPTED 2026-10-07: `rpi-image-gen`). Not changed: evidence and citations, the status of every other ADR, Change-history rows. | Claude (session 2026-10-07) |
| 2026-10-08 | Owner decisions of 2026-10-07 (second set) and research topics H and I propagated. Header, intro, §1, §2, §3, §4.1 and §5: bring-up evaluates CM4 and CM5 side by side, ADR-004 stays OPEN until measured, Pi 4 Model B and Pi 5 kept as documented candidates. HDMI audio required (REQ-CAP-006, OQ-004 ANSWERED): "only if audio is required" statements in §3 and §4.6 marked superseded; new §1 row; new §4.6.1 "HDMI audio path (I2S)" with PACSCORDER-value table (OQ-025, OQ-024, OQ-114, OQ-054), pins and balls [I-27], clock roles [I-25], [I-03], [I-28] (reasoning), IO-board GPIO voltage [I-29], GPIO 18–21 conflicts [I-30], [I-31], [I-32], and a per-platform I2S table (CM4 `bcm2835-i2s` [I-10], [I-13]; CM5 RP1 I2S1 [I-05]–[I-09], [I-14], unconfirmed; Pi 4 Model B and Pi 5 marked KERNEL SOURCE INSPECTION REQUIRED where topic I does not cover them; community report [I-16]); §4.6 GPIO table adds ball numbers, A_OSCK question and a GPIO 21 row; §4.6 "VDDIO2 must be 3.3 V" research note marked partly superseded by [I-29]. §4.2 audio facts extended ([I-24], [I-25], [I-26]; OQ-110, OQ-112, OQ-033). §4.1 and §5: no hardware H.265 encoder on any candidate [D-24], [D-31]; x265 DotProd only on CM5's Cortex-A76 [H-04], [H-05], extended to Pi 4 Model B and Pi 5 by labelled reasoning (OQ-103, OQ-104, OQ-105; RISK-022); §5 rows for H.265 encode, DotProd, audio I2S and bring-up role. §3 and §4.3: OQ-102 ANSWERED (any HDMI camera, no model list, plus ATEM outputs) marked as superseding "Which models is OPEN"; source audio formats unverified (OQ-110, OQ-083). §7 hazards 11–12 added; §8 checklist adds the audio wiring item. Verification status: D-24, H-04, H-05 and topic I IDs added; community I-16 and reasoning I-28 listed; 2026-10-08 research JSON named. No platform chosen; no REQ or ADR status changed; no hardware facts added. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): intro "both H.264 and H.265" marked superseded in part (H.264 only; H.265 deferred, not in current scope); §4.1 H.265 encode bullet labelled "deferred — REQ-ENC-002; not in current scope" with an OQ-103 ANSWERED note (OQ-104, OQ-105, RISK-022 OPEN, not in current scope); §5 "Hardware H.265 encode" and "x265 Neon DotProd" rows labelled deferred; Verified-on-hardware paragraph notes the narrowed codec decision. Hardware facts [D-24], [D-31], [H-04], [H-05] kept unchanged as evidence; no citation added or removed; no platform chosen; no other decision or status changed. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header (Last updated; Applies to names the ADR-009 recording storage; Verification adds topics J and K); intro: dated sentence on the 2026-10-08 owner decisions (fragmented MP4 mirrored to NVMe SSD + self-powered USB-to-SATA HDD; ext4 is Claude's proposal, OQ-120; < 1 s for WebRTC viewers only, RTMP best-effort); §1 new "Recording storage hardware" row (no part chosen; OQ-120 to OQ-124); §2 POWER, USB and STORAGE rows annotated and linked to OQ-120 to OQ-124; §3 dated note on where storage attaches [J-01], [J-03], [J-11], [J-12], [J-13], [J-18]; §4.1 live-latency sub-bullet [K-30], [K-33] (community), [K-34], [K-35], [K-39] (OQ-059, OQ-115, RISK-031, OQ-125); §4.2 TC358743 internal buffering undocumented [K-38] (OQ-125); §4.9 three PACSCORDER-value rows (HDD power per ADR-009, SSD supply, hold-up supply), 12 V note for the CM4 IO Board PCIe slot [J-05] (RISK-026), "Raspberry Pi power" research gap marked superseded in part, new IO-board power and VBUS facts [J-05], [J-06], [J-19], [J-20] and HDD power facts [J-30] (CORRECTED), [J-32], reasoning [J-31], [J-28]; §4.12 note that Ethernet is still not researched (OQ-098); §4.13 three PACSCORDER-value rows (HDD port, bridge, CM4 host controller; OQ-122), two research-gap / option statements marked superseded, new §4.13.1 per-platform USB table for the HDD [J-01], [J-04], [J-06]–[J-09], [J-17]–[J-19], [J-21]–[J-25], bandwidth reasoning [J-37], bridge facts [J-25]–[J-28] (RISK-027); §4.14 table rows annotated (recording medium decided; power-loss strategy decided, loss OQ-119) and four rows added (SSD/adaptor, HDD/enclosure, filesystem, write caches), boot-storage research gap marked superseded in part, new §4.14.1 per-platform PCIe / NVMe table [J-01], [J-02], [J-03], [J-04], [J-05], [J-10]–[J-17], data rates [J-36], HDD timing [J-30], kernel support [J-29], unknowns (OQ-121); §5 rows for PCIe, USB and NVMe boot; §6 IO Board rows extended and a storage-parts row (none chosen); §7 hazards 13–17; §8 checklist storage item; Verification status lists topics J and K, CORRECTED J-09, J-23, J-27, J-30, community J-27, K-33, reasoning J-09, J-23, J-31, J-36, J-37, and the 2026-10-08 storage-latency research JSON. No hardware revision defined; no platform or part chosen; no REQ, ADR, RISK or OQ status changed. Verifier pass (same date): header Verification row keeps the original "as of 2026-10-08" with "(still none on 2026-10-09)" added (Rule 21); §4.1 "the recording and live encodes share the one hardware encoder" → "would share …; whether it runs both is UNKNOWN (OQ-115)"; §4.13.1 and §4.14.1 gain a "Commands" bullet (source settings NOT YET RUN ON PACSCORDER HARDWARE); §4.14.1 Pi 5 Gen 3 line marked quoted, not proposed, NOT YET RUN [J-16]; §7 hazard 14 spin-up figure tied to the register's example drive [J-30]. Main session, same date: "self-powered enclosure or hub" stated as the owner decision reworded to the owner's words, "self-powered enclosure"; where ADR-009 decision 3 is quoted, "or hub" is marked as Claude's addition (consistent with the DECISIONS.md correction of 2026-10-09). | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 (recording until stopped or disk full — OQ-006 ANSWERED, new OQ-129; latency judged at the 95th percentile; LAN-only WebRTC viewers): intro — dated note after the 2026-10-08 latency sentence (95th-percentile criterion with the recording running; LAN-only viewers, OQ-008, OQ-128, RISK-033; no fixed recording duration limit, OQ-129); §4.12 "Network requirements that size it" — dated note that WebRTC reach is LAN only, while viewer count, browsers, bitrates and Ethernet hardware (not researched, OQ-098) remain open, so the value stays UNKNOWN; §4.14 "Recording medium and capacity" — "maximum recording duration … still OPEN, OQ-006" marked superseded, with labelled reasoning from [J-36] that capacity now sets the recording time, and the resolution cell notes OQ-006 ANSWERED (capacity and models OQ-121, OQ-122; follow-up OQ-129). No fact ID added; no hardware value, REQ, ADR, RISK, OQ or test status changed here. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09, later (live CBR 17 Mbit/s and recording 25 Mbit/s VBR — OQ-005; continue on the remaining drive if one fails — OQ-129): intro — dated note (bitrates; reasoning from [J-36] that each drive then receives about 3.15 MB/s, so capacity sets the recording time at about 88 h per 1 TB; drive-failure policy; file splitting, alert method and drive return still open); §4.12 "Network requirements that size it" — live bitrate now set, value stays UNKNOWN (viewer count and browsers OQ-008; Ethernet OQ-098); §4.13.1 bandwidth bullet — 25.192 Mbit/s is the decided per-drive rate (reasoning [J-37]); §4.14 "Recording medium and capacity" — superseded in part (about 3.15 MB/s, 11.34 GB/h per drive, about 88 h per 1 TB, about 6.30 MB/s mirrored — reasoning from [J-36]; capacities and models still OQ-121/OQ-122; drive-failure policy, OQ-129 remainder open) and resolution cell annotated; §4.14.1 bandwidth bullet — figures at the decided 25 Mbit/s, VBR caveat (reasoning), 8 and 12 Mbit/s stay examples; §4.14.1 HDD-timing bullet — failed versus stalled drive (OQ-129; RISK-028). No fact ID added; no hardware value, REQ, ADR, RISK, OQ or test status changed here. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 on OQ-129 (start on the available drive when one is missing; split recordings every 30 minutes): intro — superseded-in-part note (about 5.67 GB per 30-minute file per drive at 25 Mbit/s, reasoning from [J-36]; drive return OQ-129 and alert method OQ-091 still open); §4.14 "Recording medium and capacity" — value cell superseded in part (file size per drive; capacity still sets total recording time, reasoning) and resolution cell annotated (OQ-129 OPEN only for drive return); §4.14.1 — "Bandwidth" bullet file-size note and "HDD timing" bullet note (drive missing at start); Verification status — note (no new entry). No fact ID added; no hardware value, REQ, ADR, RISK, OQ or test status changed here. | Claude (session 2026-10-09) |
