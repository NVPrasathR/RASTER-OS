# PACSCORDER Open Questions (OQ register)

| | |
|---|---|
| Document status | Active — 103 questions registered: 98 OPEN, 5 ANSWERED |
| Last updated | 2026-10-07 |
| Applies to | PACSCORDER product requirements, hardware, TC358743 bridge, Linux driver, all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5), build/OS, streaming, ATEM, licensing and supply |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)). Nothing has been tested on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 8 (never invent hardware information), Rule 22 (unknown information), Rule 23 (source priority), Rule 25 (new-engineer questions) |

This is the single register of everything PACSCORDER does not yet know and every decision the owner has not yet made (Rule 22). Other documents that write `UNKNOWN — VERIFICATION REQUIRED` or a resolution marker link the matching `OQ-NNN` from here. Identifiers used below are defined in [REQUIREMENTS.md](REQUIREMENTS.md) (`REQ-…`), [DECISIONS.md](DECISIONS.md) (`ADR-…`), [RISKS.md](RISKS.md) (`RISK-…`), [README.md](README.md) canonical test table (`TEST-…`) and [REFERENCES.md](REFERENCES.md) (fact IDs such as `[C-37]`).

## How to use this register

An OQ is closed only with evidence. To close one, add these fields under the entry without changing the original text: **Answer** (what was found or decided), **Evidence** (a fact ID, a datasheet section, an owner statement with date, or a TEST ID whose result is recorded in [TESTING.md](TESTING.md)), **Answered on** (date) and **Answered by**. Then change **Status** to `ANSWERED` and update every linked REQ, ADR and RISK and every document that carried the UNKNOWN marker. Never delete or renumber an entry (Rules 14 and 21). If an answer later proves wrong, add a dated note and set the status back to `OPEN`. A question made irrelevant by a decision (for example a platform that is dropped) is marked `ANSWERED` with that decision as its evidence. New questions take the next free number after the highest ID in use (OQ-090 to OQ-101 were added on 2026-10-06 after the cross-document review; OQ-102 and OQ-103 on 2026-10-07) and are placed in the matching category section, so numbers inside a category need not be contiguous.

Conventions used in every entry:

- **Status** is `OPEN` or `ANSWERED`. Implementation and test status words (Rule 10) are not used here; as of 2026-10-06 every test named below is `BLOCKED — HARDWARE REQUIRED` or `NOT STARTED`.
- **Resolution method** uses these markers: `OWNER DECISION REQUIRED`, `DATASHEET REQUIRED`, `HARDWARE TEST REQUIRED`, `KERNEL SOURCE INSPECTION REQUIRED`, `VENDOR CONFIRMATION REQUIRED`, `LEGAL CLARIFICATION REQUIRED`, `BUILD TEST REQUIRED`. `VENDOR CONFIRMATION REQUIRED` covers board-vendor schematics as well as statements from Toshiba, Raspberry Pi and Blackmagic.
- **Known so far** cites the source register. Facts of tier `community` are worded as reports ("reported by …"). Calculations are marked as reasoning and name their input facts. Text marked *research gap* or *research open question* comes from the lists in [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json); it is **not** a register fact.
- **Resolving test** is a canonical TEST ID from [README.md](README.md), or `—` when no test applies (decisions, documents, legal questions).
- Tools and commands named in an entry are not procedures. Every procedure lives in [TESTING.md](TESTING.md) and is **NOT YET RUN ON PACSCORDER HARDWARE**.

The register was built by merging every `open_questions` and `gaps` item of research topics A–G, the UNDEFINED items in [REQUIREMENTS.md](REQUIREMENTS.md), the OPEN and PROPOSED ADRs in [DECISIONS.md](DECISIONS.md) and the risks in [RISKS.md](RISKS.md). Duplicates raised by several topics were merged into one entry. [Appendix A](#appendix-a--research-items-to-oq-mapping) maps every research item to its OQ; [Appendix B](#appendix-b--requirements-decisions-and-risks-to-oq-mapping) maps requirements, decisions and risks.

## Summary

| ID | Title | Category | Resolution marker | Status |
|---|---|---|---|---|
| OQ-001 | Is 1920x1080@60 capture mandatory? | 1 Owner decisions | OWNER DECISION REQUIRED | ANSWERED |
| OQ-002 | Supported HDMI input modes and EDID content | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-003 | Accepted capture pixel format (ADR-005) | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-004 | Is HDMI audio required? | 1 Owner decisions | OWNER DECISION REQUIRED | ANSWERED |
| OQ-005 | Encoding parameters: codec, bitrate, latency, simultaneous encodes | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-006 | Recording: container, storage, duration, power-loss behaviour | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-007 | RTMP destinations and parameters | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-008 | WebRTC scope: reach, browsers, viewers, latency | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-009 | ATEM integration scope | 1 Owner decisions | OWNER DECISION REQUIRED | ANSWERED |
| OQ-010 | Sustained-operation envelope: temperature, soak duration, drop threshold | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-011 | Product target platform (ADR-004) | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-012 | OS and image build basis (ADR-003) | 1 Owner decisions | OWNER DECISION REQUIRED | ANSWERED |
| OQ-013 | TC358743 driver strategy (ADR-002) | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-014 | Capture control model (ADR-006) | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-015 | Userspace media framework (ADR-007) | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-016 | Is HDMI CEC required? | 1 Owner decisions | OWNER DECISION REQUIRED; KERNEL SOURCE INSPECTION REQUIRED | OPEN |
| OQ-017 | Acceptance of DRAFT and PROPOSED requirements | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-018 | PACSCORDER hardware composition (bridge board, carrier, inputs, Rule 8 items) | 2 Hardware & bridge board | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-019 | REFCLK oscillator frequency on the bridge board | 2 Hardware & bridge board | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-020 | INT and RESETN wiring to Pi GPIOs | 2 Hardware & bridge board | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED | OPEN |
| OQ-021 | CSI-2 lanes routed, connector type and cable | 2 Hardware & bridge board | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-022 | Use of the camera-connector CAM_GPIO pin | 2 Hardware & bridge board | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-023 | Product power input and power budget | 2 Hardware & bridge board | OWNER DECISION REQUIRED; DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-024 | Bridge-board I/O voltage and HDMI HPD/+5V interface | 2 Hardware & bridge board | VENDOR CONFIRMATION REQUIRED; DATASHEET REQUIRED | OPEN |
| OQ-025 | HDMI audio I2S wiring to the Pi | 2 Hardware & bridge board | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-026 | TC358743 I2C address, address strap and bus sharing | 2 Hardware & bridge board | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-027 | Access to Toshiba NDA documentation (REF_01, REF_02) | 3 TC358743 silicon | VENDOR CONFIRMATION REQUIRED; OWNER DECISION REQUIRED | OPEN |
| OQ-028 | HDCP version, keys, licensing and behaviour | 3 TC358743 silicon | DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED; LEGAL CLARIFICATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-029 | CHIPID revision byte of production silicon | 3 TC358743 silicon | HARDWARE TEST REQUIRED; DATASHEET REQUIRED | OPEN |
| OQ-030 | Input timing and CSI-2 rate limits of the silicon | 3 TC358743 silicon | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-031 | Electrical and timing specifications missing from the public datasheet | 3 TC358743 silicon | DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-032 | HPD state from power-on until an EDID is loaded | 3 TC358743 silicon | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-033 | Silicon capabilities the driver does not use | 3 TC358743 silicon | DATASHEET REQUIRED; KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-034 | Alternative parts: TC358743AXBG and TC9590XBG | 3 TC358743 silicon | DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-035 | FIFO level and D-PHY timing validity per mode and temperature | 3 TC358743 silicon | HARDWARE TEST REQUIRED; DATASHEET REQUIRED | OPEN |
| OQ-036 | Exact mainline commit used for the driver comparison | 4 Linux driver & kernel | KERNEL SOURCE INSPECTION REQUIRED | OPEN |
| OQ-037 | Effective CSI-2 clock mode (continuous or non-continuous) | 4 Linux driver & kernel | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-038 | Capture with 3 active lanes of 4 configured | 4 Linux driver & kernel | HARDWARE TEST REQUIRED | OPEN |
| OQ-039 | Behaviour after driver unload; module reload as recovery | 4 Linux driver & kernel | HARDWARE TEST REQUIRED | OPEN |
| OQ-040 | Distinguishing fractional frame rates (59.94 vs 60 Hz) | 4 Linux driver & kernel | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-041 | Colourimetry and quantisation range for the encoder | 4 Linux driver & kernel | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED | OPEN |
| OQ-042 | Driver features not covered by the source register | 4 Linux driver & kernel | KERNEL SOURCE INSPECTION REQUIRED; DATASHEET REQUIRED | OPEN |
| OQ-043 | Runtime device nodes, I2C bus numbers and media entity names | 4 Linux driver & kernel | HARDWARE TEST REQUIRED | OPEN |
| OQ-044 | Which Unicam driver and mode bind on Pi 4/CM4 | 5 Pi 4 / CM4 | HARDWARE TEST REQUIRED | OPEN |
| OQ-045 | RGB888 memory byte order on Pi 4/CM4 | 5 Pi 4 / CM4 | HARDWARE TEST REQUIRED | OPEN |
| OQ-046 | Unicam Media Controller mode enumeration of UYVY | 5 Pi 4 / CM4 | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-047 | Broadcom documentation of the Unicam per-lane limit | 5 Pi 4 / CM4 | DATASHEET REQUIRED | OPEN |
| OQ-048 | Pi 4/CM4 firmware variant and GPU memory for the hardware codec | 5 Pi 4 / CM4 | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-049 | TC358743 capture through RP1 CFE on Pi 5/CM5 | 6 Pi 5 / CM5 | HARDWARE TEST REQUIRED | OPEN |
| OQ-050 | Effect of the CFE's 999 Mbps D-PHY setting | 6 Pi 5 / CM5 | HARDWARE TEST REQUIRED | OPEN |
| OQ-051 | Source-change event delivery on Pi 5/CM5 | 6 Pi 5 / CM5 | HARDWARE TEST REQUIRED | OPEN |
| OQ-052 | CM5 carrier-board connector and I2C mapping | 6 Pi 5 / CM5 | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED | OPEN |
| OQ-053 | Whether CFE capture buffers use CMA on Pi 5/CM5 | 6 Pi 5 / CM5 | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-054 | tc358743-audio overlay on Pi 5/CM5 | 6 Pi 5 / CM5 | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-055 | 16K versus 4K page-size kernel on Pi 5/CM5 | 6 Pi 5 / CM5 | HARDWARE TEST REQUIRED | OPEN |
| OQ-056 | Pi 4/CM4 hardware H.264 encode at 1080p60 | 7 Encoding & DMA | HARDWARE TEST REQUIRED | OPEN |
| OQ-057 | Pi 4/CM4 encoder input formats and conversion path | 7 Encoding & DMA | HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-058 | Zero-copy DMABUF from Unicam into the Pi 4/CM4 encoder | 7 Encoding & DMA | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-059 | Pi 5/CM5 software encode: CPU, thermal, latency, concurrency | 7 Encoding & DMA | HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-060 | Pi 5/CM5 UYVY-to-planar conversion offload | 7 Encoding & DMA | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-061 | CMA budget per platform | 7 Encoding & DMA | HARDWARE TEST REQUIRED | OPEN |
| OQ-062 | DMA-BUF heap names and uapi header | 7 Encoding & DMA | HARDWARE TEST REQUIRED; BUILD TEST REQUIRED | OPEN |
| OQ-063 | Audio encoder choice and cost | 7 Encoding & DMA | HARDWARE TEST REQUIRED; LEGAL CLARIFICATION REQUIRED | OPEN |
| OQ-064 | Buildroot baseline: release series, kernel and firmware pin | 8 Build system & OS | OWNER DECISION REQUIRED; BUILD TEST REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-065 | Buildroot boot integration: overlays and module autoloading | 8 Build system & OS | BUILD TEST REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-066 | Media-stack differences between Buildroot and Raspberry Pi OS | 8 Build system & OS | BUILD TEST REQUIRED | OPEN |
| OQ-067 | Reproducible Raspberry Pi OS based builds | 8 Build system & OS | VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED | OPEN |
| OQ-068 | Image size, RAM use and boot-to-first-frame time | 8 Build system & OS | OWNER DECISION REQUIRED; BUILD TEST REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-069 | Field update mechanism | 8 Build system & OS | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED | OPEN |
| OQ-070 | Support horizon and security updates for Raspberry Pi packages | 8 Build system & OS | VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-071 | Production hardening and secure boot | 8 Build system & OS | OWNER DECISION REQUIRED; BUILD TEST REQUIRED | OPEN |
| OQ-072 | Is camera_auto_detect=0 needed with the TC358743 overlay? | 8 Build system & OS | HARDWARE TEST REQUIRED | OPEN |
| OQ-073 | H.264 level signalling for 1080p WebRTC in browsers | 9 Streaming & WebRTC | HARDWARE TEST REQUIRED | OPEN |
| OQ-074 | WebRTC signalling and NAT traversal design | 9 Streaming & WebRTC | OWNER DECISION REQUIRED; BUILD TEST REQUIRED | OPEN |
| OQ-075 | RTMP server on PACSCORDER and network ports | 9 Streaming & WebRTC | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-076 | Is SRT required? | 9 Streaming & WebRTC | OWNER DECISION REQUIRED | OPEN |
| OQ-077 | Third-party ATEM protocol support for ATEM 10.x and concurrent clients | 10 ATEM | HARDWARE TEST REQUIRED | OPEN |
| OQ-078 | Routing the ATEM Mini Pro HDMI output to Program | 10 ATEM | HARDWARE TEST REQUIRED | OPEN |
| OQ-079 | Encoding parameters of the ATEM's RTMP output | 10 ATEM | HARDWARE TEST REQUIRED | OPEN |
| OQ-080 | ATEM publishing to a PACSCORDER RTMP URL on a LAN | 10 ATEM | HARDWARE TEST REQUIRED | OPEN |
| OQ-081 | ATEM Streaming Bridge with a non-Blackmagic encoder | 10 ATEM | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-082 | ATEM USB-C webcam output under Linux | 10 ATEM | HARDWARE TEST REQUIRED | OPEN |
| OQ-083 | ATEM HDMI output as seen by the TC358743 | 10 ATEM | HARDWARE TEST REQUIRED | OPEN |
| OQ-084 | ATEM control library and runtime | 10 ATEM | OWNER DECISION REQUIRED; LEGAL CLARIFICATION REQUIRED | OPEN |
| OQ-085 | TC358743 lifecycle and long-term supply | 11 Legal, licensing & supply | VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-086 | H.264 patent licensing | 11 Legal, licensing & supply | LEGAL CLARIFICATION REQUIRED | OPEN |
| OQ-087 | GPL and source-offer compliance | 11 Legal, licensing & supply | LEGAL CLARIFICATION REQUIRED; BUILD TEST REQUIRED | OPEN |
| OQ-088 | Proprietary GPU firmware licence | 11 Legal, licensing & supply | LEGAL CLARIFICATION REQUIRED | OPEN |
| OQ-089 | Blackmagic ATEM SDK licence terms | 11 Legal, licensing & supply | LEGAL CLARIFICATION REQUIRED; VENDOR CONFIRMATION REQUIRED | OPEN |
| OQ-090 | Software process model, threading and IPC | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-091 | Operator interface | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-092 | Configuration format and store; logging transport | 1 Owner decisions | OWNER DECISION REQUIRED | OPEN |
| OQ-093 | EDID provisioning trigger and ordering | 4 Linux driver & kernel | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-094 | Software updates versus active recordings | 8 Build system & OS | OWNER DECISION REQUIRED | OPEN |
| OQ-095 | Reading CSI-2 error counters on Unicam (Pi 4/CM4) | 5 Pi 4 / CM4 | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-096 | Is a GPU overclock acceptable in the product? | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-097 | Driver source in the shipped kernel versus the inspected branch | 4 Linux driver & kernel | KERNEL SOURCE INSPECTION REQUIRED | OPEN |
| OQ-098 | Raspberry Pi Ethernet, USB and storage facts for the candidate boards | 2 Hardware & bridge board | DATASHEET REQUIRED | OPEN |
| OQ-099 | CSI-2 link frequency (ADR-008) | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-100 | config.txt overlay-parameter syntax | 8 Build system & OS | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-101 | Command syntax used in procedures but not in the source register | 8 Build system & OS | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OPEN |
| OQ-102 | Which ATEM models and cameras must be supported as HDMI sources? | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | ANSWERED |
| OQ-103 | Which outputs use H.265, and is software-only H.265 acceptable? | 1 Owner decisions | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OPEN |

---

# 1. Owner decisions

Product requirements and pending decisions that only the owner can make. Each one blocks acceptance criteria in [REQUIREMENTS.md](REQUIREMENTS.md) or an ADR in [DECISIONS.md](DECISIONS.md).

## OQ-001 — Is 1920x1080@60 capture mandatory?

- **Question:** Must PACSCORDER capture 1920x1080 at 60 Hz, as REQ-CAP-001 states (taken from the rules' example), or is 1080p50 or 1080p30 acceptable for some or all product variants?
- **Why it matters:** Decides whether Pi 4 Model B, CM4 CAM0 and any 2-lane bridge board can be used at all (REQ-CAP-001, REQ-PLT-001, ADR-004, RISK-001). It also decides whether 1080p60 encode is a hard requirement (REQ-ENC-001, RISK-002, RISK-003).
- **Known so far:**
  - Official Raspberry Pi documentation: 2 CSI-2 lanes give at most 1080p30 RGB888 or 1080p50 YUV422; 4 lanes on a Compute Module give 1080p60 in either format [C-37].
  - Reasoning from the driver's lane formula at the overlay default of 972 Mbps per lane: 1080p60 needs 3 lanes in UYVY and 4 in RGB888 [C-47]; on 2 lanes 1080p60 UYVY would need 102.4 % of the link [C-48]; all 1080p30/50/60 combinations fit on 4 lanes [C-49].
  - Lanes per connector: Pi 4 Model B 2 [C-01]; CM4 CAM1 4 and CAM0 2 [C-02]; Pi 5 4 per port [C-04]; CM5 4 per MIPI interface [C-05].
  - Encode side: the Pi 4/CM4 hardware encoder is officially specified for 1080p30 [D-10]; reasoning: 1080p60 needs 2.0x that macroblock rate [D-52]. Pi 5/CM5 encode in software and the only official figure is for 1080p30 [G-22].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-CAP-002 (capture); TEST-ENC-001 (encode).
- **Answer:** The owner requires both 2-lane and 4-lane CSI-2 configurations, each capturing every frame rate its link can carry. Recorded interpretation (Claude; the owner may correct it): 1080p60 is required on 4-lane configurations; on 2-lane configurations the physical limit applies — at most 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48]. Captured as REQ-CAP-007; REQ-CAP-001 updated. The exact mode list remains OQ-002; fractional rates OQ-040.
- **Evidence:** Owner statement, 2026-10-07: "i need 2 lane and 4 lane with all frame rate".
- **Answered on:** 2026-10-07
- **Answered by:** Owner (interpretation recorded by Claude)
- **Status:** ANSWERED

## OQ-002 — Supported HDMI input modes and EDID content

- **Question:** Which HDMI input modes must PACSCORDER accept and therefore advertise in its EDID? Specifically: (a) is 60 Hz mandatory, or are 59.94 Hz and 50 Hz also required; (b) which other resolutions and rates (720p, 1080p24/25/30, PC modes); (c) must interlaced sources such as 1080i50/59.94 be supported; (d) what exact EDID file is loaded at every start?
- **Why it matters:** The EDID determines what the source sends (REQ-CAP-003, RISK-010). Advertising a mode the wired CSI-2 lanes cannot carry leads to a stream-start failure (REQ-CAP-005). Interlaced support cannot be met by the current driver (RISK-009).
- **Known so far:**
  - The driver never asserts hot-plug until userspace writes an EDID [A-33], [B-21] (B-21 CORRECTED). After every probe no EDID is stored (`edid_blocks_written == 0`) [B-21]; the EDID is held in the chip's 1 KB embedded EDID SRAM [A-31]. Research also found that the EDID is volatile and that the driver has no default EDID and no DT property for one (research gap, topic B — not a register fact). At most 8 blocks are accepted [B-22], [A-32]. The chip supports a 128-byte EDID 1.3 base block plus one CEA-861-D extension [A-31].
  - Limits: TMDS clock up to 165 MHz [A-07]; driver DV-timings capability 640–1920 x 350–1200, 13–165 MHz, progressive only [A-08], [B-26]. Interlaced input returns `-ERANGE` (reasoning from source) [B-27].
  - `v4l2-ctl --set-edid` has a built-in `hdmi` type that advertises up to 1080p60 [B-24]; reasoning: that is more than a 2-lane link carries [C-48].
  - The driver reports integer frame rates, so 59.94 Hz and 60 Hz sources are reported alike [B-28] (see OQ-040).
  - ATEM Mini Pro HDMI output standards are 1080p23.98 to 1080p60, with no 720p or 1080i [F-23].
- **Owner input (2026-10-07):** both 2-lane and 4-lane configurations with "all frame rate" (OQ-001 → REQ-CAP-007); sources are ATEM switchers and cameras connected directly (OQ-009 → REQ-CAP-008, models in OQ-102). The EDID may therefore need to differ per lane configuration (reasoning from [C-48], [C-49]). The exact mode list and EDID file remain open.
- **Resolution method:** OWNER DECISION REQUIRED (mode list); HARDWARE TEST REQUIRED (each target source accepts the EDID and outputs a listed mode).
- **Resolving test:** TEST-DRV-002, TEST-CAP-001, TEST-CAP-004
- **Status:** OPEN

## OQ-003 — Accepted capture pixel format (ADR-005)

- **Question:** Does the owner accept UYVY (YCbCr 4:2:2 8-bit, BT.601 limited range as the driver delivers it) as the default capture format (ADR-005, PROPOSED), or is RGB888 / 4:4:4 required?
- **Why it matters:** ADR-005. The format sets the lane budget (REQ-CAP-001, RISK-001), the corruption risk near lane limits (RISK-006), the RGB888 pixel-format label mismatch between receivers (RISK-016) and the encoder input path (REQ-ENC-001, RISK-003).
- **Known so far:**
  - The driver offers only `RGB888_1X24` (probe default) and `UYVY8_1X16` [A-09], [B-34].
  - Reasoning: 1080p60 payload is 1.99 Gbit/s in UYVY and 2.99 Gbit/s in RGB888 [C-46].
  - UYVY uses BT.601 limited range reported as SMPTE170M; RGB is full range [B-34].
  - Corrupted images at 1080p50 RGB24 on a 4-lane CM4 are reported in an open issue and attributed by a Raspberry Pi engineer to the FIFO level [C-43].
  - RGB888 maps to RGB24 on Unicam and BGR24 on CFE [C-34]; B,G,R memory order on CM4 is reported by an issue reporter and explained by a Raspberry Pi engineer [C-35].
  - Pi 4 encoder: a Raspberry Pi engineer reported that both formats are accepted directly [D-18]. Pi 5: `libx264` and `x264enc` do not accept packed UYVY [D-43], [D-40].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-CAP-002
- **Status:** OPEN

## OQ-004 — Is HDMI audio required?

- **Question:** Must PACSCORDER record and/or stream the HDMI source's audio (REQ-CAP-006)? If yes: how many channels, which sample rates, and what audio/video synchronisation tolerance?
- **Why it matters:** REQ-CAP-006, RISK-014. Audio needs extra wiring (OQ-025), a separate overlay (OQ-054 on Pi 5/CM5) and an audio encoder (OQ-063), and it changes RTMP and WebRTC (REQ-STR-001, REQ-STR-002). ATEM program audio exists only embedded in its HDMI and USB outputs (REQ-ATEM-001).
- **Known so far:**
  - The silicon can send audio over CSI-2 [A-05] or on I2S/TDM pins [A-11]; the Linux driver always configures 2-channel I2S [A-13].
  - The `tc358743-audio` overlay expects I2S on GPIO 18/19/20 [A-47]; official documentation says audio needs that overlay in addition to `tc358743` [C-37].
  - The driver exposes read-only "Audio sampling rate" and "Audio present" controls [B-16].
  - ATEM Mini Pro outputs program audio only embedded on HDMI and USB-C (CORRECTED) [F-24].
  - Legacy RTMP/FLV carries AAC [F-31]. RFC 7874 requires WebRTC endpoints to implement Opus and G.711; AAC is not a required WebRTC codec, so AAC audio has to be transcoded (typically to Opus) for browser playback [F-41].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-AUD-001
- **Answer:** Yes — HDMI audio is required in recordings and streams. REQ-CAP-006 moved from PROPOSED to DRAFT. Channel count, sample rates and the A/V synchronisation tolerance are not yet specified; the hardware path is OQ-025 (I2S wiring) and OQ-054 (Pi 5/CM5 overlay); encoders OQ-063.
- **Evidence:** Owner answer, 2026-10-07: "Yes, audio required".
- **Answered on:** 2026-10-07
- **Answered by:** Owner
- **Status:** ANSWERED

## OQ-005 — Encoding parameters: codec, bitrate, latency, simultaneous encodes

- **Question:** For REQ-ENC-001: which codec(s); which bitrate(s) and rate-control mode; what latency target (glass-to-glass for WebRTC, capture-to-file for recording); and how many simultaneous encodes (one shared stream, or separate recording, RTMP and WebRTC encodes at different bitrates)?
- **Why it matters:** REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002, ADR-004, ADR-007, RISK-002, RISK-003, RISK-019.
- **Known so far:**
  - No candidate platform has a hardware HEVC encoder [D-24], [D-31]. HEVC over RTMP also needs Enhanced RTMP [F-31].
  - Pi 4/CM4 hardware H.264 encoder: officially 1080p30 [D-10]; 25 kbit/s to 25 Mbit/s, VBR or CBR [D-13]; no B-frames [D-14]; Baseline, Constrained Baseline, Main and High profiles [D-11].
  - Pi 5/CM5 use software encoders with longer latency than the old hardware encoders [D-32]; BCM2712 1080p30 encode costs ~30–40 % CPU [G-22].
  - Browser WebRTC interoperability requires H.264 Constrained Baseline [F-36].
- **Owner input (2026-10-07):** codecs are H.264 **and** H.265 for recording and streaming (REQ-ENC-001). Which output uses which codec, and whether software-only H.265 is acceptable, is OQ-103. Bitrate, rate control, latency and the number of simultaneous encodes remain open here.
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-ENC-001
- **Status:** OPEN

## OQ-006 — Recording: container, storage, duration, power-loss behaviour

- **Question:** For REQ-REC-001: which container format (MP4, Matroska, MPEG-TS or other); which storage medium (SD, eMMC, USB, NVMe, network); what minimum continuous recording duration; and what behaviour is required when power is lost during recording (how much material may be lost, must the file remain playable)?
- **Why it matters:** REQ-REC-001, REQ-PERF-001; drives the storage hardware (OQ-018) and the partition/update layout (OQ-069).
- **Known so far:**
  - ATEM switchers record H.264 + AAC as MP4 [F-07], [F-26].
  - Raspberry Pi OS's `gstreamer1.0-plugins-good` ships the MP4 (isomp4), Matroska and FLV muxers [G-26]; Buildroot has an ISOMP4 option [E-34].
  - The raspi-config read-only option uses `overlayroot=tmpfs` [G-48], so root-filesystem writes do not persist across reboot (reasoning). rpi-image-gen's `image-rota` layout has a shared persistent data partition [G-40].
  - Recording behaviour on power loss was not researched; no source fact exists.
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-REC-001
- **Status:** OPEN

## OQ-007 — RTMP destinations and parameters

- **Question:** For REQ-STR-001: which RTMP servers or services must PACSCORDER publish to (for example a public platform, a local server, or an ATEM Streaming Bridge — see OQ-081); is RTMPS required; which video bitrate; is audio included?
- **Why it matters:** REQ-STR-001; feeds OQ-005 and OQ-075.
- **Known so far:**
  - Legacy RTMP/FLV carries H.264 (CodecID 7) and AAC; HEVC, AV1, VP9 and Opus need Enhanced RTMP [F-31].
  - GStreamer `rtmp2sink` publishes over RTMP and RTMPS [F-33]. FFmpeg's RTMP URL form uses default TCP port 1935 [F-32].
  - `flvmux` needs AVC-format H.264 and raw AAC (CORRECTED) [F-34]; reasoning: `v4l2h264enc` outputs byte-stream, so a parser is needed between them [F-35].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-STR-001
- **Status:** OPEN

## OQ-008 — WebRTC scope: reach, browsers, viewers, latency

- **Question:** For REQ-STR-002: are WebRTC viewers on the LAN only or also over the internet (NAT traversal, STUN/TURN)? Which browsers and versions must be supported? How many simultaneous viewers? What latency target? Is audio required in WebRTC?
- **Why it matters:** REQ-STR-002, RISK-019, ADR-007; decides OQ-073 and OQ-074.
- **Known so far:**
  - RFC 7742 requires WebRTC browsers to implement H.264 Constrained Baseline and VP8, with SPS/PPS sent in-band [F-36].
  - Chromium's libwebrtc advertises H.264 at Level 3.1 [F-39]; reasoning: 1080p needs Level 4 or higher [F-40].
  - RFC 7874 requires WebRTC endpoints to implement Opus and G.711 audio; AAC is not a required WebRTC codec, so AAC audio has to be transcoded (typically to Opus) for browser playback [F-41].
  - `webrtcbin` has no built-in signalling [F-42]; `webrtcsink` includes a simple signalling server [F-43]; the MediaMTX project reports that it serves WebRTC readers through a browser page and WHEP [F-45].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-STR-002
- **Status:** OPEN

## OQ-009 — ATEM integration scope

- **Question:** For REQ-ATEM-001: which integration(s) are required? (1) Capture the ATEM HDMI output through the TC358743. (2) Read tally / recording / streaming state, or control the switcher, over the network. (3) Receive the ATEM's RTMP stream. (4) Send PACSCORDER video into an ATEM setup. Which ATEM models and firmware versions must be supported?
- **Why it matters:** REQ-ATEM-001, RISK-018; decides which of OQ-075 and OQ-077 to OQ-084 need answers.
- **Known so far:**
  - The official ATEM SDK supports Windows and macOS only [F-01], [F-02] and does not document the wire protocol [F-10]. The OpenSwitcher project reports the UDP 9910 protocol as reverse-engineered [F-11].
  - ATEM Mini Pro HDMI output is 1080p only [F-23] and defaults to multiview [F-25].
  - ATEM Mini Pro streams over RTMP and SRT and records MP4 [F-26]; reasoning: to receive its RTMP push, PACSCORDER must run a listening RTMP server [F-46].
  - ATEM Streaming Bridge documents RTMP input only from Blackmagic sources (CORRECTED) [F-28].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-ATEM-001
- **Answer:** Integration (1) only: the ATEM is an HDMI source captured through the TC358743, and cameras may also be connected directly (REQ-CAP-008). Integrations (2) network tally/control, (3) receiving the ATEM RTMP stream and (4) sending video into an ATEM setup were offered and not selected; they are out of scope unless the owner adds them. The remaining sub-question — which ATEM models and firmware versions, and which cameras — moved to OQ-102.
- **Evidence:** Owner statement, 2026-10-07: "it can be atem and direct video from camera".
- **Answered on:** 2026-10-07
- **Answered by:** Owner (interpretation recorded by Claude)
- **Status:** ANSWERED

## OQ-010 — Sustained-operation envelope: temperature, soak duration, drop threshold

- **Question:** For REQ-PERF-001: what ambient operating temperature range must the product meet (in which enclosure, with or without active cooling); for how long must it run continuously; and what frame-drop threshold is acceptable?
- **Why it matters:** REQ-PERF-001, RISK-003; sets the conditions for OQ-035 and OQ-059.
- **Known so far:**
  - TC358743XBG is rated −30 to +70 °C ambient; TC9590XBG −40 to +85 °C, in a different package [A-41], [A-40].
  - TC358743 typical power is 543.2 mW at 1080p60 [A-38].
  - Pi 5/CM5 encode in software; 1080p30 encode costs ~30–40 % CPU [G-22].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-PERF-001
- **Status:** OPEN

## OQ-011 — Product target platform (ADR-004)

- **Question:** Which of Pi 4 Model B, CM4, Pi 5 and CM5 is (or are) the product platform? Owner answer of 2026-10-06: "Undecided — keep all four."
- **Why it matters:** ADR-004 (OPEN), REQ-PLT-001, REQ-CAP-001, REQ-ENC-001; RISK-001, RISK-002, RISK-003, RISK-012. Platform questions OQ-044 to OQ-060 matter only for the platforms that are kept.
- **Known so far:**
  - Lanes: see OQ-001 ([C-01], [C-02], [C-04], [C-05]).
  - Official Raspberry Pi TC358743 documentation covers only the Unicam (Pi 4 family) path [C-37], [C-38].
  - Hardware H.264 encoder: Pi 4/CM4 yes, specified for 1080p30 [D-10]; Pi 5/CM5 none [D-31], [G-22].
  - Reasoning: if OQ-001 makes 1080p60 mandatory, Pi 4 Model B is excluded ([C-01], [C-37], [C-48]). *(OQ-001 ANSWERED 2026-10-07: 1080p60 is required on the 4-lane configuration. Pi 4 Model B is therefore excluded from that configuration only; it remains a candidate for the 2-lane configuration of REQ-CAP-007.)*
- **Owner input (2026-10-07):** both 2-lane and 4-lane configurations are required (REQ-CAP-007), so the product spans more than one capture configuration. Which platform serves which configuration is still open (ADR-004).
- **Owner input (2026-10-07, second answer):** bring-up evaluates CM4 and CM5 side by side ("CM4 + CM5 side by side"); the product platform is then decided from measurements (ADR-004 OPEN).
- **Resolution method:** OWNER DECISION REQUIRED, informed by HARDWARE TEST REQUIRED on the candidate platforms.
- **Resolving test:** TEST-PLT-001, TEST-CAP-002, TEST-ENC-001 (evidence for the decision)
- **Status:** OPEN

## OQ-012 — OS and image build basis (ADR-003)

- **Question:** Does the owner accept ADR-003 (PROPOSED): Raspberry Pi OS Lite 64-bit for bring-up, `rpi-image-gen` for production images, Buildroot kept as the documented alternative? Should Yocto (`meta-raspberrypi`) also be evaluated?
- **Why it matters:** ADR-003, REQ-BLD-001, RISK-017. The answer decides whether the Buildroot questions (OQ-064 to OQ-066) or the Raspberry Pi OS questions (OQ-067, OQ-069, OQ-070) must be answered.
- **Known so far:**
  - The owner said "which ever is best that raspberry pi os" (2026-10-06), recorded in ADR-003.
  - Raspberry Pi OS Lite 2026-10-06 is based on trixie with kernel 6.18.50 [G-01], [G-04]. It ships `tc358743.ko.xz` for both kernels [G-16], and the Raspberry Pi defconfigs enable Unicam, RP1 CFE and `bcm2835-codec` [G-17], [G-18]. Reasoning from register inputs: both module trees include those drivers and one image can boot all four candidates (CORRECTED) [G-71].
  - `rpi-image-gen` is official and BSD-3-Clause, latest v2.8.0 [G-37]; it supports native arm64 Debian hosts only [G-39]; its minor releases contain breaking changes [G-44].
  - Buildroot 2026.08 pins kernel 6.12.61 [E-06], [G-59], forces 4K pages on Pi 5/CM5 [G-60] and does not install overlays in its Pi 5 defconfig [G-61].
  - Yocto was not evaluated (research gap, topic G).
- **Owner input (2026-10-07):** "which is best i need by own one". Recorded as REQ-BLD-002 (the product runs its own project-built OS image). The build-tool choice is delegated to Claude's recommendation, which is unchanged: `rpi-image-gen` (ADR-003, PROPOSED). This entry stays OPEN until the owner explicitly accepts ADR-003.
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-BLD-001
- **Answer:** ADR-003 is ACCEPTED: Raspberry Pi OS Lite 64-bit for bring-up; the product runs its own OS image built with `rpi-image-gen` from Raspberry Pi OS packages, with a package mirror for reproducibility; Buildroot stays the documented alternative. Yocto is not evaluated unless the owner asks.
- **Evidence:** Owner statement, 2026-10-07: "accept ADR-003".
- **Answered on:** 2026-10-07
- **Answered by:** Owner
- **Status:** ANSWERED

## OQ-013 — TC358743 driver strategy (ADR-002)

- **Question:** Does the owner accept ADR-002 (PROPOSED): use the in-tree `tc358743` driver and carry patches for defects proven on hardware, instead of writing the "PACSCORDER TC358743 driver" mentioned in the rules' examples? Should the known reference-clock defect be patched before bring-up?
- **Why it matters:** ADR-002, REQ-DRV-001, RISK-005, RISK-007.
- **Known so far:**
  - The in-tree driver is identical in mainline and `rpi-6.18.y` except one formatting difference [A-48], [B-02], and Raspberry Pi kernels build it as a module [E-39], [G-16].
  - It was written against non-public Toshiba documents; only a 20-page summary datasheet is public [A-42].
  - Known defects: an unsupported refclk rate leads to a kernel BUG instead of a clean probe failure (kernel source [A-22] and reasoning from it [B-11], both CORRECTED); FIFO level 374 is hard-coded [B-12]; a continuous clock is forced while the DT can request non-continuous [B-37].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-DRV-001 (evidence)
- **Status:** OPEN

## OQ-014 — Capture control model (ADR-006)

- **Question:** Does the owner accept ADR-006 (PROPOSED): Media Controller mode on every platform, including Pi 4/CM4 through the overlay's `media-controller` parameter, with EDID and DV timings handled on the TC358743 sub-device node?
- **Why it matters:** ADR-006, REQ-CAP-002, REQ-CAP-003, REQ-CAP-004; determines the capture application and every capture test procedure.
- **Known so far:**
  - Pi 4/CM4: the overlay default is legacy video-node mode; the `media-controller` parameter selects Media Controller mode [C-10], [B-43]. In legacy mode EDID and DV-timings ioctls go through the video node and sub-device nodes are read-only; in Media Controller mode they go through `/dev/v4l-subdevN` (both CORRECTED) [B-25], [C-36].
  - Pi 5/CM5 are Media Controller only [C-11]; sensor-to-csi2 links are immutable, csi2-to-video-node links must be enabled by userspace [C-32].
  - On Pi 5, a Raspberry Pi engineer reported that source-change events must be subscribed on the source sub-device [C-42].
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** TEST-PLT-001
- **Status:** OPEN

## OQ-015 — Userspace media framework (ADR-007)

- **Question:** Which framework carries capture, encode, recording, RTMP and WebRTC: GStreamer, FFmpeg, a direct V4L2 application, or a combination?
- **Why it matters:** ADR-007 (OPEN), REQ-DMA-001, REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002; licensing (OQ-087).
- **Known so far:**
  - GStreamer covers RTMP (`rtmp2sink`) [F-33] and WebRTC (`webrtcbin`, no signalling) [F-42]; its V4L2 M2M elements support `dmabuf-import` [D-53]; official documentation gives a `v4l2h264enc` pipeline [D-37].
  - Upstream FFmpeg's V4L2 M2M code is MMAP-only, so each frame is copied [D-44]; Raspberry Pi OS's patched FFmpeg adds DMABUF input [D-45].
  - The `rpicam-apps` direct V4L2 path imports DMABUFs into `/dev/video11` [D-34].
- **Resolution method:** OWNER DECISION REQUIRED, after HARDWARE TEST REQUIRED (capture and encode proven on the candidate platforms).
- **Resolving test:** TEST-DMA-001, TEST-ENC-001
- **Status:** OPEN

## OQ-016 — Is HDMI CEC required?

- **Question:** Does PACSCORDER need HDMI CEC (for example to control or query the source)? If yes, do the driver's CEC and hot-plug handling behave correctly in polling mode, without a wired interrupt?
- **Why it matters:** CEC needs a non-default kernel configuration (REQ-BLD-001, ADR-003) and raises I2C polling from 1000 ms to 10 ms when no interrupt is wired (RISK-013, OQ-020).
- **Known so far:**
  - The chip has a CEC pin and block; Linux support is behind `CONFIG_VIDEO_TC358743_CEC` [A-34].
  - The Raspberry Pi defconfigs do not enable it [B-20], [E-39].
  - Without an IRQ the driver polls every 1000 ms, or every 10 ms when a CEC adapter is registered [A-30], [B-19].
- **Resolution method:** OWNER DECISION REQUIRED; KERNEL SOURCE INSPECTION REQUIRED (CEC in polling mode).
- **Resolving test:** —
- **Status:** OPEN

## OQ-017 — Acceptance of DRAFT and PROPOSED requirements

- **Question:** Will the owner (a) confirm wording and acceptance criteria for the DRAFT requirements REQ-ARCH-001, REQ-PLT-001, REQ-DRV-001, REQ-CAP-001, REQ-CAP-002, REQ-DMA-001, REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002, REQ-ATEM-001 and REQ-BLD-001, and (b) accept or reject the PROPOSED requirements REQ-CAP-003, REQ-CAP-004, REQ-CAP-005, REQ-CAP-006 and REQ-PERF-001?
- **Why it matters:** No requirement has yet been accepted with acceptance criteria ([REQUIREMENTS.md](REQUIREMENTS.md) header), so no test can be given pass/fail thresholds (Rules 11 and 12).
- **Known so far:** The source and derivation of each requirement are recorded in [REQUIREMENTS.md](REQUIREMENTS.md). The parameters the owner must supply are split out as OQ-001 to OQ-010.
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-090 — Software process model, threading and IPC

- **Question:** How is the userspace software structured: one process or several services? What threading model, and what inter-process interface between the capture control, pipeline, recorder, streaming and ATEM components?
- **Why it matters:** REQ-ARCH-001, REQ-CAP-004, ADR-007; [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) marks these as "decision needed".
- **Known so far:** No source fact applies; this is a design decision. Candidate media frameworks are compared in ADR-007 (OPEN). `webrtcbin` has no built-in signalling transport [F-42], so a WebRTC design needs its own signalling component.
- **Resolution method:** OWNER DECISION REQUIRED (to be recorded as an ADR).
- **Resolving test:** —
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-091 — Operator interface

- **Question:** How does an operator control and monitor PACSCORDER: physical buttons and indicators, a web interface, a network API, ATEM-driven control, or a combination?
- **Why it matters:** Product scope; affects REQ-REC-001, REQ-STR-001, REQ-STR-002, REQ-ATEM-001 and the software architecture.
- **Known so far:** No requirement exists. Research found that ATEM switchers expose record/stream state and tally over UDP 9910 (reported by open-source libraries) [F-16], [F-17], which could drive control if OQ-009 includes it. *(OQ-009 ANSWERED 2026-10-07: network tally/control was not selected, so ATEM-driven control is not in current scope unless the owner adds it.)*
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-092 — Configuration format and store; logging transport

- **Question:** Where and in what format is device configuration stored (and how does it survive updates and a read-only root)? How are logs collected and exported?
- **Why it matters:** REQ-BLD-001, ADR-003 (image layout, A/B updates), [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md).
- **Known so far:** `rpi-image-gen` `image-rota` provides A/B system slots with a shared persistent data partition [G-40], [G-41]; `raspi-config` can make the root an overlay filesystem [G-48]. No configuration or logging design exists.
- **Resolution method:** OWNER DECISION REQUIRED (to be recorded as an ADR).
- **Resolving test:** —
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-096 — Is a GPU overclock acceptable in the product?

- **Question:** If Pi 4/CM4 hardware encode needs a raised GPU clock (for example `gpu_freq=550`) to reach the target frame rate, is running the product overclocked acceptable?
- **Why it matters:** RISK-002, REQ-ENC-001, REQ-PERF-001, OQ-056.
- **Known so far:** A reasoning-tier register entry records that the officially documented 720p120 high-frame-rate recipe suggests `gpu_freq=550` [D-52]; the official encode specification is 1080p30 [D-10]. Thermal and long-term reliability effects on PACSCORDER are unknown.
- **Resolution method:** OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (thermal soak).
- **Resolving test:** TEST-ENC-001, TEST-PERF-001
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-099 — CSI-2 link frequency (ADR-008)

- **Question:** Accept ADR-008 (keep the default 486 MHz link frequency, 972 Mbit/s per lane, on every platform), or use 297 MHz (594 Mbit/s) on a Compute Module 4 CAM1 4-lane link for 1080p60 UYVY?
- **Why it matters:** REQ-CAP-001, RISK-006, RISK-011, OQ-038, ADR-008.
- **Known so far:** Only 297000000 and 486000000 are supported values [A-46], [B-45]; the driver has timing tables only for 594 and 972 Mbit/s [A-24], [B-09]. At 972 Mbit/s 1080p60 UYVY uses 3 lanes; at 594 Mbit/s it uses 4 (reasoning) [C-47], [C-49]. A driver comment says 594 Mbit/s is meant for 4-lane 1080p60 [C-44]. RP1 CFE programs 999 Mbit/s for this bridge [C-31]; reasoning: that matches only the default [C-52].
- **Resolution method:** OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-002, TEST-CAP-004
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-102 — Which ATEM models and cameras must be supported as HDMI sources?

- **Question:** Which Blackmagic ATEM models (and firmware versions) and which camera models must PACSCORDER capture from? What HDMI output modes, colour formats and HDCP behaviour does each one have?
- **Why it matters:** REQ-CAP-008, REQ-ATEM-001, REQ-CAP-007, OQ-002 (EDID and mode list), RISK-008 (HDCP), RISK-009 (interlaced inputs).
- **Known so far:**
  - ATEM Mini Pro: HDMI output standards 1080p23.98 to 1080p60, no 720p, 1080i or Ultra HD; 4:2:2 YUV, 10-bit, Rec 709 [F-23]. Its HDMI output defaults to multiview, not program; the output source is changed with the front-panel buttons or in ATEM Software Control [F-25].
  - Reasoning from driver source: the driver accepts progressive timings only (interlaced returns `-ERANGE`) [B-27]; the TMDS clock is limited to 165 MHz [A-07]; HDCP is disabled by the driver [A-04].
  - Camera HDMI output behaviour is not covered by the source register: UNKNOWN — VERIFICATION REQUIRED per model.
- **Resolution method:** OWNER DECISION REQUIRED (model list); HARDWARE TEST REQUIRED (each listed source).
- **Resolving test:** TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001
- **Answer:** No model list: any HDMI camera (generic), plus ATEM switcher outputs (OQ-009). What is accepted is defined by the supported-mode matrix and the EDID (OQ-002); unsupported modes (for example interlaced) are rejected per REQ-CAP-005. Representative cameras and an ATEM are still needed for testing (TEST-CAP-001, TEST-CAP-004).
- **Evidence:** Owner answer, 2026-10-07: "Any HDMI camera (generic)".
- **Answered on:** 2026-10-07
- **Answered by:** Owner
- **Status:** ANSWERED
- **Added:** 2026-10-07 (split from OQ-009 after the owner's answer)

## OQ-103 — Which outputs use H.265, and is software-only H.265 acceptable?

- **Question:** For each output — recording, RTMP, WebRTC — must H.265 be available, at which resolutions and frame rates, and is it acceptable that H.265 is encoded in software on every candidate platform (with the CPU, thermal and licensing consequences)?
- **Why it matters:** REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002, RISK-022, RISK-015, ADR-004, ADR-007.
- **Known so far:**
  - No candidate platform has a hardware HEVC encoder [D-24], [D-31]. The only official software-encode figure is for H.264 at 1080p30 on Pi 5 (~30–40% CPU) [G-22]; no sourced H.265 figure exists yet (research topic H, 2026-10-07).
  - Legacy RTMP/FLV carries only H.264 video; HEVC needs Enhanced RTMP [F-31]. GStreamer `eflvmux` first appears in 1.28 [F-34], while Raspberry Pi OS ships GStreamer 1.26.2 [G-26] (reasoning: not available in stock packages).
  - WebRTC endpoints are required to support only VP8 and H.264 Constrained Baseline [F-36].
- **Resolution method:** OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED (software H.265 encode on CM4 and CM5).
- **Resolving test:** TEST-ENC-001, TEST-PERF-001
- **Status:** OPEN
- **Added:** 2026-10-07 (after the owner chose H.264 + H.265)

---

# 2. PACSCORDER hardware & bridge board

Every entry in this category is `UNKNOWN — VERIFICATION REQUIRED` for PACSCORDER: no board has been chosen and no hardware exists (owner, 2026-10-06). The facts below describe the TC358743 and the Raspberry Pi side, not PACSCORDER's own hardware.

## OQ-018 — PACSCORDER hardware composition (bridge board, carrier, inputs, Rule 8 items)

- **Question:** What makes up PACSCORDER HW REV A? Which TC358743 bridge board (third-party or a custom PCB)? If a Compute Module is chosen, which carrier (official IO board or custom)? How many HDMI inputs per unit? And the Rule 8 items not yet defined: Ethernet, USB, storage interface, enclosure.
- **Why it matters:** Every other entry in this category depends on it; [HARDWARE.md](HARDWARE.md) must describe the actual hardware (Rule 8); REQ-PLT-001, ADR-004, RISK-021.
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - A Raspberry Pi engineer reported that the Auvidea B101 uses a 15-pin FFC with contacts on the same side [C-45]; Raspberry Pi engineers reported a B102 probing on Pi 5 in December 2023 [C-41]. Other boards named in the research questions (Geekworm C77x/C779/C790, Waveshare, X1301) have no entries in the source register.
  - Official IO boards: the CM4 IO Board has a 2-lane CAM0 and a 4-lane CAM1 22-pin connector [C-03]; the CM5 IO Board has two 22-pin CAM/DISP connectors [C-06].
  - All public configurations put the TC358743 at I2C address 0x0f [A-15]; reasoning: two bridges on one I2C bus would collide (see OQ-026).
- **Resolution method:** OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED (schematics of the chosen boards).
- **Resolving test:** TEST-HW-001
- **Status:** OPEN

## OQ-019 — REFCLK oscillator frequency on the bridge board

- **Question:** Which reference-clock oscillator does the chosen bridge board fit, at what frequency and accuracy?
- **Why it matters:** The Device Tree must declare the real frequency; a wrong value crashes the kernel (RISK-007, RISK-021, REQ-DRV-001, ADR-002).
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED for the board; HARDWARE TEST REQUIRED).
  - The datasheet lists 27/26 MHz or 42 MHz [A-21]; the driver accepts only 26, 27 or 42 MHz [B-07].
  - Any other rate leads to a kernel BUG rather than a clean probe failure (kernel source [A-22] and reasoning from it [B-11], both CORRECTED).
  - Reasoning: only 27 MHz gives the exact 594/972 Mbps lane rates of the driver's timing tables [A-23], [B-10].
  - The Raspberry Pi overlay declares 27 MHz on a `fixed-clock` node [A-45]; reasoning from the DT: the Pi does not generate this clock, so the board must carry its own oscillator [B-47].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (board schematic and BOM); HARDWARE TEST REQUIRED (measure the oscillator).
- **Resolving test:** TEST-DRV-001
- **Status:** OPEN

## OQ-020 — INT and RESETN wiring to Pi GPIOs

- **Question:** On the chosen board, is the TC358743 INT output wired to a Pi GPIO, and is RESETN driven by a Pi GPIO or only by a power-on reset circuit? If neither is wired, does the owner accept up to about 1 s signal-change detection latency (REQ-CAP-004) and no software reset path?
- **Why it matters:** RISK-013, REQ-CAP-004, REQ-DRV-001; the DT `interrupts` and `reset-gpios` properties ([DEVICE_TREE.md](DEVICE_TREE.md)).
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED (GPIO numbers unknown).
  - INT is active-high, level-triggered, in the VDDIO2 domain [A-29]. Without an IRQ the driver polls every 1000 ms [A-30], [B-19].
  - RESETN is active-low [A-27]. `reset-gpios` is optional; when present, the driver pulses it at probe with timings chosen by the driver, not by Toshiba [A-28], [B-13].
  - The stock overlay has neither property [B-41], [C-23]. Driver removal does not assert reset [B-40].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (schematic); HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED (only if the latency must be accepted).
- **Resolving test:** TEST-DRV-001 (reset), TEST-CAP-003 (detection latency)
- **Status:** OPEN

## OQ-021 — CSI-2 lanes routed, connector type and cable

- **Question:** How many CSI-2 data lanes does the chosen board route to the Pi, through which connector (15-pin 1.0 mm or 22-pin 0.5 mm), with which cable or adapter, to which Pi connector?
- **Why it matters:** The lane count limits the capturable modes (REQ-CAP-001, RISK-001); a wrongly sided adapter can damage hardware (RISK-021); DT `data-lanes` and the overlay's `4lane` parameter must match the wiring.
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - The TC358743 supports 1 to 4 data lanes at up to 1 Gbps per lane [A-05], [A-06].
  - Pi side: Pi 4B one 15-pin 2-lane connector [C-01]; CM4 IO Board 22-pin connectors, CAM0 2 lanes and CAM1 4 lanes [C-03]; Pi 5 two 22-pin 4-lane ports [C-04]; CM5 two 4-lane interfaces [C-05].
  - Setting `4lane` on a 2-lane port does not fail the probe: Unicam logs a message and adopts the endpoint count [C-17].
  - A Raspberry Pi engineer reported that a wrongly sided 22-to-15-pin adapter swaps GND and 3V3 and can damage either board [C-45].
- **Owner input (2026-10-07):** both 2-lane and 4-lane configurations are required (REQ-CAP-007). Whether one bridge-board design can serve both — for example a 4-lane board run with 2 lanes on a 2-lane port — is UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED).
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (board schematic and cable pinout); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PLT-001, TEST-CAP-002
- **Status:** OPEN

## OQ-022 — Use of the camera-connector CAM_GPIO pin

- **Question:** Does the chosen board use the camera connector's CAM_GPIO / power-enable pin, for example to enable its regulators or to drive RESETN?
- **Why it matters:** With the stock overlay that pin is expected to stay low, so a board that needs it would remain unpowered or in reset (REQ-DRV-001, RISK-021); a custom overlay would then be needed ([DEVICE_TREE.md](DEVICE_TREE.md)).
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - Camera power-enable lines: Pi 4B and CM4 use expander GPIO 5 [C-21]; Pi 5 uses RP1 GPIO 34 and 46, CM5 RP1 GPIO 34 shared by both ports [C-22].
  - Neither the overlay nor the driver requests a regulator [C-23]; reasoning: the line is therefore expected to stay low [C-51].
  - On the CM5 IO Board only CAM/DISP 0 has a camera power-down signal [C-06].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (schematic); HARDWARE TEST REQUIRED (measure the pin and the board rails).
- **Resolving test:** TEST-HW-001
- **Status:** OPEN

## OQ-023 — Product power input and power budget

- **Question:** What is PACSCORDER's power input (voltage, connector, source such as DC jack, USB-C or PoE) and its total power budget? Rule 9's test example lists "12V power"; that is an example in the rules, not a PACSCORDER specification.
- **Why it matters:** [HARDWARE.md](HARDWARE.md) POWER (Rule 8); thermal design (REQ-PERF-001, OQ-010); the bridge-board supply (OQ-024).
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - TC358743 typical power is 480.5 mW at 720p60 and 543.2 mW at 1080p60 [A-38]; its rails are listed in [A-36].
  - No Raspberry Pi board power figure was collected in the research.
- **Resolution method:** OWNER DECISION REQUIRED; DATASHEET REQUIRED (Raspberry Pi and bridge-board power specifications); HARDWARE TEST REQUIRED (measure).
- **Resolving test:** —
- **Status:** OPEN

## OQ-024 — Bridge-board I/O voltage and HDMI HPD/+5V interface

- **Question:** At what voltage does the chosen board run the TC358743's VDDIO2 domain (1.8 V or 3.3 V)? How does it implement the HDMI hot-plug output and +5V sensing (level shifting, divider)?
- **Why it matters:** VDDIO2 sets the logic level of the I2C, RESETN, INT, REFCLK and I2S pins that face the Pi (OQ-020, OQ-025). HPD wiring decides whether sources see the sink (REQ-CAP-003, RISK-010).
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - VDDIO2 may be 1.8 V or 3.3 V [A-36]. REFCLK, RESETN, INT, the host I2C pins and the audio pins are in VDDIO2 [A-21], [A-27], [A-29], [A-39], [A-12].
  - DDC_SCL, DDC_SDA and HPDI are listed as 5 V tolerant in the 3.3 V VDDIO1 domain; HPDO is in VDDIO1 and is not listed as 5 V tolerant [A-39].
  - The driver drops HPD and clears the stored timings when source +5V disappears [B-23].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (schematic); DATASHEET REQUIRED (which pin senses source +5V).
- **Resolving test:** TEST-HW-001, TEST-DRV-002
- **Status:** OPEN

## OQ-025 — HDMI audio I2S wiring to the Pi

- **Question:** If audio is required (OQ-004), does the chosen board bring out the TC358743 I2S pins (A_SCK, A_WFS, A_SD), and how are they wired to Pi GPIO 18/19/20 or the platform's I2S pins?
- **Why it matters:** REQ-CAP-006, RISK-014.
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - The audio pins are outputs in VDDIO2 [A-12]; I2S runs in controller-clock mode only with 32-bit slots [A-11], so the Pi is the clock consumer [B-46].
  - `tc358743-audio` wiring: LRCK/WFS GPIO 19, BCK/SCK GPIO 18, DATA/SD GPIO 20 [A-47], [G-14]. Reasoning: this is wiring separate from the CSI-2 cable.
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (schematic); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-AUD-001
- **Status:** OPEN

## OQ-026 — TC358743 I2C address, address strap and bus sharing

- **Question:** At which I2C address does the TC358743 answer on the chosen board? Is the TC358743XBG address selectable, as on the sister part TC358749XBG (INT pin level at reset)? Which other devices share the camera I2C bus?
- **Why it matters:** The DT `reg` value ([DEVICE_TREE.md](DEVICE_TREE.md)), probe (REQ-DRV-001), multi-input designs (OQ-018) and pull resistors on INT (OQ-020).
- **Known so far:**
  - UNKNOWN — VERIFICATION REQUIRED.
  - The DT binding example and the Raspberry Pi overlay use 7-bit address 0x0f; the public datasheet states no address and no strap [A-15]. The driver prints the address in 8-bit form, 0x1e [A-16].
  - TC358749XBG documents 0x0F/0x1F selected by INT at reset [A-50]; no equivalent statement exists for TC358743XBG.
  - With CM5 on the CM4 IO Board, the `i2c_csi_dsi1` bus also serves DISP1, the RTC and the fan [C-27]. Research also noted that Buildroot's CM4IO/CM5IO sample configs place an RTC on the camera bus (research gap, topic E).
- **Resolution method:** DATASHEET REQUIRED (functional specification, OQ-027); HARDWARE TEST REQUIRED (I2C scan on the real board).
- **Resolving test:** TEST-HW-001
- **Status:** OPEN

## OQ-098 — Raspberry Pi Ethernet, USB and storage facts for the candidate boards

- **Question:** What Ethernet (PHY, speed, PoE option), USB (host ports, speeds) and boot-storage options (SD, eMMC, NVMe) do Pi 4 Model B, CM4 (+ carrier), Pi 5 and CM5 (+ carrier) provide?
- **Why it matters:** [HARDWARE.md](HARDWARE.md) ETHERNET / USB / STORAGE sections; REQ-REC-001 (storage), REQ-STR-001/002 (network).
- **Known so far:** Not covered by the 2026-10-06 research, except that Compute Module eMMC is flashed with `rpiboot` [G-45], [G-47].
- **Resolution method:** DATASHEET REQUIRED (official Raspberry Pi product briefs and datasheets).
- **Resolving test:** —
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

---

# 3. TC358743 silicon

## OQ-027 — Access to Toshiba NDA documentation (REF_01, REF_02)

- **Question:** Can the project obtain the TC358743XBG Functional Specification (REF_01) and the register-settings spreadsheet (REF_02) from Toshiba under NDA?
- **Why it matters:** RISK-005. Most silicon entries below are DATASHEET REQUIRED and can be answered only from these documents. The ADR-002 alternative of a new driver also depends on them.
- **Known so far:**
  - Only a 20-page summary datasheet is public, without register map, I2C address or AC timing; the driver cites REF_01 and REF_02 [A-42].
  - A Raspberry Pi engineer reported that Toshiba's FIFO formula is in an NDA document [A-43], and quoted a minimum FIFO level of 120 from Toshiba's spreadsheet [C-43].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED; OWNER DECISION REQUIRED (whether to sign an NDA).
- **Resolving test:** —
- **Status:** OPEN

## OQ-028 — HDCP version, keys, licensing and behaviour

- **Question:** Which HDCP version does the "HDCP (optional)" feature implement? Which orderable part suffixes carry eFuse keys? What licensing applies? What does PACSCORDER output when a source requires HDCP (laptops, cameras, the ATEM)?
- **Why it matters:** RISK-008, REQ-CAP-001, REQ-ATEM-001.
- **Known so far:**
  - The datasheet lists "Support HDCP (optional)" with eFuse keys and states no version [A-03].
  - The driver always disables automatic HDCP authentication on DT platforms [A-04], [B-12]; HDCP registers are write-protected through `s_register` [B-38].
  - Part suffixes (EL,NOK) and (EL,H4) were seen during research, but which carry keys is unknown (research gap, topic A).
- **Resolution method:** DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED; LEGAL CLARIFICATION REQUIRED (HDCP licensing); HARDWARE TEST REQUIRED (HDCP-requiring sources).
- **Resolving test:** TEST-CAP-001
- **Status:** OPEN

## OQ-029 — CHIPID revision byte of production silicon

- **Question:** What revision byte (CHIPID bits [7:0]) does production TC358743XBG silicon report, and does it differ between lots?
- **Why it matters:** Revision records in [HARDWARE.md](HARDWARE.md); the driver checks only the chip-ID byte (REQ-DRV-001).
- **Known so far:** CHIPID is register 0x0000, chip ID in bits [15:8] and revision in bits [7:0] [A-18]. The driver requires the chip-ID byte to be 0x00 and only prints the revision [A-19], [B-14].
- **Resolution method:** HARDWARE TEST REQUIRED; DATASHEET REQUIRED (expected values).
- **Resolving test:** TEST-DRV-001
- **Status:** OPEN

## OQ-030 — Input timing and CSI-2 rate limits of the silicon

- **Question:** What are the real input limits of the TC358743 beyond the driver's capability (for example 1920x1200@60 reduced blanking, minimum and maximum width and height), and the real CSI-2 lane-rate envelope?
- **Why it matters:** The supported-mode matrix (REQ-CAP-005) and the EDID content (OQ-002).
- **Known so far:**
  - Datasheet: TMDS up to 165 MHz, input up to 1080p60 [A-07]; CSI-2 up to 4 lanes and 1 Gbps per lane [A-05].
  - Driver: 640–1920 x 350–1200, 13–165 MHz; a driver comment says the width/height limits are unknown and only the pixel-clock limit is sourced [A-08], [B-26]. Lane rate 62.5 Mbps–1 Gbps, with timing tables only for 594 and 972 Mbps [A-24].
- **Resolution method:** DATASHEET REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-004
- **Status:** OPEN

## OQ-031 — Electrical and timing specifications missing from the public datasheet

- **Question:** What are the TC358743's (a) power-up sequencing between rails; (b) minimum RESETN pulse width and reset-to-I2C-ready time; (c) REFCLK tolerance, jitter and duty cycle; (d) I2C and I2S AC timing; (e) maximum I2C clock, 400 kHz or 2 MHz?
- **Why it matters:** Custom board design (OQ-018), reset-GPIO timing (OQ-020), I2C bus speed ([DEVICE_TREE.md](DEVICE_TREE.md)); REQ-DRV-001.
- **Known so far:**
  - Rails, ranges and noise limits are public [A-36], [A-37]. The public datasheet has no register map, I2C address or AC timing [A-42]; power-up sequencing, minimum reset width and REFCLK accuracy are also absent (research gap, topic A).
  - The reset timing in the driver (1–2 ms assert, 20 ms settle) is the driver's choice, not a Toshiba specification [A-28], [B-13].
  - I2C: the datasheet gives 100 kHz and 400 kHz; an older product brief lists 2 MHz; the documents conflict [A-14]. The research recommends treating 400 kHz as the limit until this is resolved.
- **Resolution method:** DATASHEET REQUIRED (through OQ-027); VENDOR CONFIRMATION REQUIRED (Toshiba field application engineer).
- **Resolving test:** —
- **Status:** OPEN

## OQ-032 — HPD state from power-on until an EDID is loaded

- **Question:** What is the reset value of HPD_CTL HPD_OUT0 at power-on, and is HPD guaranteed low between power-up and the first EDID write?
- **Why it matters:** A source that sees HPD before an EDID exists could read an empty EDID or pick an unsupported mode (REQ-CAP-003, RISK-010).
- **Known so far:** The driver raises HPD only after an EDID is written and +5V is present (CORRECTED) [B-21], [A-33]; writing an EDID drops HPD first [B-22]. The driver never drives HPD low explicitly during probe (research open question, topic B).
- **Resolution method:** DATASHEET REQUIRED (register reset values); HARDWARE TEST REQUIRED (observe HPD from power-on to EDID load).
- **Resolving test:** TEST-DRV-002
- **Status:** OPEN

## OQ-033 — Silicon capabilities the driver does not use

- **Question:** What does the silicon support that the driver does not use: YCbCr 4:2:2 12-bit output; audio and InfoFrame packets over CSI-2 (data types, virtual channels) and whether Unicam and RP1 CFE accept them; which HDMI audio sample rates and formats; 8-channel TDM audio?
- **Why it matters:** Could simplify audio capture (REQ-CAP-006, RISK-014) or change the format choice (ADR-005).
- **Known so far:**
  - Datasheet: video, audio and InfoFrame data can be sent over CSI-2 [A-05]; audio can also leave on I2S or 8-channel TDM [A-11].
  - The register header defines a 12-bit 4:2:2 output code the driver does not use [A-10]; the driver always selects 2-channel I2S [A-13]; stream-on sets the audio-buffer enable bit [B-36]; the driver exposes audio sampling-rate and audio-present controls [B-16].
- **Resolution method:** DATASHEET REQUIRED; KERNEL SOURCE INSPECTION REQUIRED (receiver data-type support); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-AUD-001 (audio part); — for the rest.
- **Status:** OPEN

## OQ-034 — Alternative parts: TC358743AXBG and TC9590XBG

- **Question:** How does TC358743AXBG differ from TC358743XBG, and is it a drop-in alternative? Is TC9590XBG, with its wider temperature range, usable on a custom board?
- **Why it matters:** Supply continuity (RISK-004, OQ-085) and temperature range (OQ-010).
- **Known so far:**
  - The current public datasheet is the combined TC358743XBG/TC9590XBG document, Rev. 2.20 of 2026-05-11 [A-01].
  - TC9590XBG is rated −40 to +85 °C but uses a different 7.0 x 7.0 mm, 0.80 mm pitch package [A-41], [A-40].
  - A separate TC358743AXBG datasheet (2019-01-23) was found, but its URL returned HTTP 404 (research open question, topic A).
- **Resolution method:** DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-035 — FIFO level and D-PHY timing validity per mode and temperature

- **Question:** Are the driver's hard-coded FIFO trigger level (374) and its 594/972 Mbps D-PHY timing constants correct for every mode PACSCORDER supports (for example 1080p50 UYVY on 2 lanes, 1080p60 on 3 or 4 lanes, 720p), across the operating temperature range and for long runs?
- **Why it matters:** RISK-006, REQ-CAP-001, REQ-CAP-005, REQ-PERF-001.
- **Known so far:**
  - `fifo_level = 374` is hard-coded; a driver comment gives it as suitable for 720p60/1080p60 at 594 Mbps and "most modes on 972Mbps" [B-12], [C-44].
  - A Raspberry Pi engineer reported the value is empirical [A-43]. An open issue reports corruption at 1080p50 RGB24 on 3 of 4 lanes, attributed by a Raspberry Pi engineer to the FIFO level [C-43].
  - Timing tables exist only for 594 and 972 Mbps [A-24], [B-09].
  - Reasoning (per-line check): 2-lane 1080p50 UYVY has about 2.0 µs of slack per line and about 7.6 % margin over the minimum rate from Toshiba's spreadsheet [C-50].
  - Reasoning (inputs [C-47], [C-48], [C-50]): 1080p50 UYVY on 2 lanes, which official documentation lists as supported [C-37], has the same per-active-lane load (85.3 % of 972 Mbps) and the same per-line transmit time (15.80 µs of 17.778 µs) as the reported-corrupt 1080p50 RGB888 on 3 lanes [C-43]. The Raspberry Pi engineer in [C-43] attributed that corruption to the FIFO level, so the match does not show that the UYVY mode is affected; it puts it in the same load class ([CSI_PIPELINE.md](CSI_PIPELINE.md) §10.6; RISK-006). The link frequency that sets these loads is ADR-008 (PROPOSED; OQ-099).
- **Resolution method:** HARDWARE TEST REQUIRED (mode matrix, soak, temperature); DATASHEET REQUIRED (REF_02 to compute the values, OQ-027).
- **Resolving test:** TEST-CAP-004, TEST-PERF-001
- **Status:** OPEN

---

# 4. Linux driver & kernel

## OQ-036 — Exact mainline commit used for the driver comparison

- **Question:** Which torvalds/linux commit and release was compared with `rpi-6.18.y` when the two driver copies were found identical?
- **Why it matters:** Reproducibility of the ADR-002 evidence; future rebasing of PACSCORDER patches.
- **Known so far:** Results are in [A-48] and [B-02]. The `rpi-6.18.y` tip at the time was af73e0836bf0 (6.18.55) [E-37]. The mainline side was read from `master` on 2026-10-06 without recording its SHA (research open question, topic B).
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-037 — Effective CSI-2 clock mode (continuous or non-continuous)

- **Question:** With the stock overlay, is the CSI-2 clock lane continuous or non-continuous on the wire? How do Unicam and RP1 CFE use the receiver-side flag, given that the driver reports a continuous clock?
- **Why it matters:** A transmitter/receiver clock-mode mismatch could make reception unreliable (REQ-CAP-001; clock configuration in [DEVICE_TREE.md](DEVICE_TREE.md)).
- **Known so far:** The overlay sets `clock-noncontinuous` [C-12], [B-41]. That flag affects only TXOPTIONCNTRL in `tc358743_set_csi()`; every stream-on writes continuous-clock mode, and `get_mbus_config` reports flags 0 with the comment "Support for non-continuous CSI-2 clock is missing in the driver" [B-37], [B-36].
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED (bus-flag use in Unicam and CFE); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-002
- **Status:** OPEN

## OQ-038 — Capture with 3 active lanes of 4 configured

- **Question:** Do Unicam and RP1 CFE capture frames correctly when the TC358743 activates 3 lanes on a 4-lane link, as it does for 1080p60 UYVY at 972 Mbps?
- **Why it matters:** 1080p60 in the proposed default format (ADR-005) uses 3 lanes at the default link frequency (REQ-CAP-001, RISK-006). A 4-lane port is therefore necessary but not shown sufficient for 1080p60 UYVY; ADR-008 (PROPOSED; OQ-099) proposes evaluating 297 MHz on a CM4 CAM1 4-lane link, where the mode uses all 4 lanes.
- **Known so far:**
  - The driver computes the lane count and reports it without clamping to the DT value (CORRECTED) [A-25], [B-31].
  - Both receivers reject more lanes than configured; fewer, such as 3 of 4, pass the stream-start check [C-16], [B-32].
  - Downstream Unicam accepts 1, 2 or 4 lanes on its DT endpoint [C-17].
  - Reasoning: 1080p60 UYVY needs 3 lanes at 972 Mbps [C-47].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-002, TEST-CAP-004
- **Status:** OPEN

## OQ-039 — Behaviour after driver unload; module reload as recovery

- **Question:** After the module is removed or the driver unbound, does HPD stay asserted and does the source keep transmitting? Is unloading and reloading the module a usable recovery action?
- **Why it matters:** The recovery design in [TC358743_DRIVER.md](TC358743_DRIVER.md); REQ-CAP-004.
- **Known so far:** Remove does not drop HPD, assert reset or disable the reference clock [B-40]. Writing zero EDID blocks clears the EDID and keeps HPD low [B-22]. The driver has no runtime power management (research gap, topic B).
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-003
- **Status:** OPEN

## OQ-040 — Distinguishing fractional frame rates (59.94 vs 60 Hz)

- **Question:** How will PACSCORDER determine the true source frame rate (59.94 vs 60, 29.97 vs 30, 23.976 vs 24) for encoder timestamps and recording metadata, when the driver reports integer rates?
- **Why it matters:** OQ-002 if 59.94 Hz is required; REQ-ENC-001, REQ-REC-001; audio/video drift.
- **Known so far:** The driver computes fps as round(10000 / FV_CNT) and derives the pixel clock from it, so fractional rates are reported as integer-rate pixel clocks [B-28]. The ATEM outputs both 1080p59.94 and 1080p60 [F-23].
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED (compare buffer timestamps against a 59.94 Hz source).
- **Resolving test:** TEST-CAP-001
- **Status:** OPEN

## OQ-041 — Colourimetry and quantisation range for the encoder

- **Question:** What colour matrix, range and transfer characteristics must the encoder signal for TC358743 UYVY output from HD sources, and do the receivers deliver the CSI-2 data types and pixel formats the documentation assumes?
- **Why it matters:** Visible colour shifts in recordings and streams (REQ-ENC-001, ADR-005).
- **Known so far:**
  - UYVY output uses BT.601 limited range and is reported as SMPTE170M; RGB output is full range [B-34]. Research found that UYVY is reported as SMPTE170M even for 720p/1080p sources, and that the driver has no quantisation-range control (research gap, topic B — not a register fact).
  - Both receivers map `UYVY8_1X16` to `V4L2_PIX_FMT_UYVY` (CSI-2 YUV422_8B) [C-34].
  - ATEM Mini Pro video sampling is 4:2:2 YUV 10-bit Rec 709 [F-23].
- **Resolution method:** HARDWARE TEST REQUIRED (colour reference through the whole chain); KERNEL SOURCE INSPECTION REQUIRED.
- **Resolving test:** TEST-CAP-002, TEST-ENC-001
- **Status:** OPEN

## OQ-042 — Driver features not covered by the source register

- **Question:** What do the module parameters (`debug`, `packet_type`), the debugfs InfoFrame interface and the zeroed `hdmi_phy_auto_reset_*` / `hdmi_detection_delay` platform-data fields on DT platforms actually do?
- **Why it matters:** Diagnostics ([TROUBLESHOOTING.md](TROUBLESHOOTING.md)) and lock behaviour with marginal sources (REQ-CAP-001, REQ-CAP-004).
- **Known so far:** Probe creates debugfs InfoFrame entries and remove frees them [B-15], [B-40]; initial setup configures InfoFrame capture [B-50]; on DT platforms the driver hard-codes some platform data [B-12]. The parameter names and the zeroed fields come from research gaps (topics A and B), not from register facts.
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED; DATASHEET REQUIRED (HDMI PHY auto-reset semantics).
- **Resolving test:** —
- **Status:** OPEN

## OQ-043 — Runtime device nodes, I2C bus numbers and media entity names

- **Question:** On each platform and connector, what are the actual `/dev/i2c-N`, `/dev/mediaN`, `/dev/v4l-subdevN` and `/dev/videoN` nodes and the media entity names (for example "tc358743 10-000f")?
- **Why it matters:** Procedures and the capture application must not hard-code names (REQ-CAP-002; [V4L2.md](V4L2.md), [TESTING.md](TESTING.md)).
- **Known so far:**
  - Pi 4B/CM4: CAM1 is `i2c-10` on GPIO 44/45, CAM0 is `i2c-0` [C-24], [C-25].
  - Pi 5: CAM/DISP0 is `i2c-10`, CAM/DISP1 is `i2c-11` [C-26]. A Raspberry Pi engineer reported different bus numbers (i2c-4/i2c-6) on earlier kernels, and the entity name changes with the bus (CORRECTED) [C-28].
  - CM5 depends on the IO board [C-27], [B-44].
  - CFE video nodes are named "rp1-cfe-<node>" [C-32]; Unicam in Media Controller mode registers only "unicam-image" for this single-pad bridge (CORRECTED) [C-36].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PLT-001
- **Status:** OPEN

## OQ-093 — EDID provisioning trigger and ordering

- **Question:** What triggers the EDID write (boot service, device-node appearance, after a driver reload), and in what order relative to overlay load, module load and media-graph setup? Does an EDID survive a driver unload and reload?
- **Why it matters:** REQ-CAP-003, REQ-CAP-004, RISK-010, OQ-039.
- **Known so far:** Hot-plug is not raised until an EDID has been written, and `edid_blocks_written` is 0 after probe [A-33], [B-21]. On +5V removal the driver drops HPD and zeroes stored timings [B-23]. Remove does not drop HPD [B-40].
- **Resolution method:** OWNER DECISION REQUIRED (design); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-DRV-002, TEST-CAP-003
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-097 — Driver source in the shipped kernel versus the inspected branch

- **Question:** Does `drivers/media/i2c/tc358743.c` in the Raspberry Pi OS 2026-10-06 kernel (6.18.50) differ from the `rpi-6.18.y` tip inspected during research (6.18.55)?
- **Why it matters:** [TC358743_DRIVER.md](TC358743_DRIVER.md) describes the inspected source; ADR-002, ADR-003.
- **Known so far:** Raspberry Pi OS 2026-10-06 ships kernel 6.18.50 [G-04]; the inspected tip was 6.18.55 [E-37]; the inspected file matches mainline `master` [A-48], [B-02].
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED (diff the file between the two tags).
- **Resolving test:** —
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

---

# 5. Pi 4 / CM4 platform

## OQ-044 — Which Unicam driver and mode bind on Pi 4/CM4

- **Question:** On the shipped image, which driver binds csi0/csi1 on Pi 4/CM4 (downstream `bcm2835-unicam-legacy` in legacy or Media Controller mode, or mainline `bcm2835-unicam`), with and without the overlay's `media-controller` parameter?
- **Why it matters:** ADR-006, REQ-CAP-002; [CSI_PIPELINE.md](CSI_PIPELINE.md).
- **Known so far:**
  - The base DT csi0/csi1 nodes use "brcm,bcm2835-unicam", which the downstream driver binds in Media Controller mode (CORRECTED) [E-40], [C-08], [C-09].
  - The `tc358743` overlay changes csi1 to "brcm,bcm2835-unicam-legacy"; the `media-controller` parameter removes that change [C-10], [B-43].
  - The mainline driver binds only "brcm,bcm2835-unicam-upstream" in this tree [G-21].
  - The source answer therefore exists; confirmation on a running system is missing.
- **Resolution method:** HARDWARE TEST REQUIRED (identify the bound driver on a running Pi 4/CM4).
- **Resolving test:** TEST-PLT-001
- **Status:** OPEN

## OQ-045 — RGB888 memory byte order on Pi 4/CM4

- **Question:** If RGB888 is ever used (OQ-003), what byte order does Pi 4/CM4 capture put in memory with the shipped kernel?
- **Why it matters:** RISK-016; colour-swapped video.
- **Known so far:** Unicam maps `RGB888_1X24` to RGB24 and CFE maps it to BGR24 [C-34]. On CM4, B,G,R memory order was reported in an issue and explained by a Raspberry Pi engineer [C-35]. A downstream Unicam change was still pending (research open question, topic C).
- **Resolution method:** HARDWARE TEST REQUIRED (capture a single-colour frame and inspect the bytes).
- **Resolving test:** TEST-CAP-002 (only if RGB888 is used)
- **Status:** OPEN

## OQ-046 — Unicam Media Controller mode enumeration of UYVY

- **Question:** In Media Controller mode, does Unicam's `VIDIOC_ENUM_FMT` filtered by `UYVY8_1X16` omit UYVY even though setting UYVY succeeds, and does link validation accept UYVY?
- **Why it matters:** ADR-006 consequences; the capture application must not depend on filtered enumeration (REQ-CAP-002).
- **Known so far:** Research found that downstream Unicam marks `UYVY8_1X16` as `mc_skip` (research gap, topic C — not a register fact). Both receivers map `UYVY8_1X16` to UYVY [C-34].
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PLT-001
- **Status:** OPEN

## OQ-047 — Broadcom documentation of the Unicam per-lane limit

- **Question:** Does Broadcom document the 1 Gbit/s per-lane limit of the BCM2711 Unicam receiver?
- **Why it matters:** Margin for 972 Mbps operation ([CSI_PIPELINE.md](CSI_PIPELINE.md), REQ-CAP-001).
- **Known so far:** Official Raspberry Pi documentation gives up to 1 Gbit/s per lane, maximum link frequency 500 MHz [C-07]. No BCM2711 Unicam datasheet was found (research open question, topic C). Until one exists, [C-07] is the documented limit.
- **Resolution method:** DATASHEET REQUIRED (not publicly available).
- **Resolving test:** —
- **Status:** OPEN

## OQ-048 — Pi 4/CM4 firmware variant and GPU memory for the hardware codec

- **Question:** On Pi 4/CM4, is the standard firmware (`start4.elf`) enough for `bcm2835-codec` H.264 encode and the ISP, or is `start4x.elf` needed? What `gpu_mem` and CMA settings does the codec need on 1, 2, 4 and 8 GB boards?
- **Why it matters:** REQ-ENC-001 on Pi 4/CM4; config.txt content in [BUILD_SYSTEM.md](BUILD_SYSTEM.md).
- **Known so far:**
  - The cut-down firmware (`start4cd.elf`, selected by `gpu_mem=16`) removes codec support [D-48]. Buildroot offers PI4, PI4_X ("more audio/video codecs") and PI4_CD variants [D-48], [E-13].
  - Buildroot's sample Pi 4 config.txt uses `start4.elf` and `gpu_mem_*=100` [E-17].
  - The codec driver is a VCHIQ child and pulls in `vc-sm-cma` [D-28], [D-03].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (Raspberry Pi config.txt documentation); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-ENC-001
- **Status:** OPEN

## OQ-095 — Reading CSI-2 error counters on Unicam (Pi 4/CM4)

- **Question:** How are CSI-2 receive errors (CRC, ECC, lane sync) observed on the Pi 4/CM4 Unicam receiver? OQ-050 covers RP1 CFE only.
- **Why it matters:** RISK-006, RISK-011, TEST-CAP-002, TEST-CAP-004 pass criteria.
- **Known so far:** No source fact covers Unicam error reporting.
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-002, TEST-CAP-004
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

---

# 6. Pi 5 / CM5 platform

## OQ-049 — TC358743 capture through RP1 CFE on Pi 5/CM5

- **Question:** Does TC358743 capture run end-to-end on Pi 5 and CM5 with `tc358743-pi5`, including the `4lane` parameter on the 22-pin connectors, at 1080p60 UYVY and the other required modes?
- **Why it matters:** RISK-012, REQ-PLT-001, REQ-CAP-001, ADR-004.
- **Known so far:**
  - On BCM2712, `dtoverlay=tc358743` is redirected to `tc358743-pi5`, which is always Media Controller [C-11], [E-43]; it accepts `4lane`, `link-frequency` and `cam0` [G-13].
  - Pi 5 ports and CM5 interfaces are 4-lane [C-04], [C-05]. The overlay README still says "Uses Unicam 1" and calls `4lane` Compute-Module-CAM1-only for the Pi 5 overlay — stale text [C-13], [B-45].
  - No official Pi 5 TC358743 documentation exists [C-38].
  - A Raspberry Pi engineer reported a capture sequence that he said functioned on kernel 6.18.39 [C-33]; Raspberry Pi engineers reported the bridge probing on Pi 5 [C-41].
  - Reasoning: all 1080p30/50/60 modes fit on 4 lanes at 972 Mbps [C-49].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PLT-001, TEST-CAP-001, TEST-CAP-002
- **Status:** OPEN

## OQ-050 — Effect of the CFE's 999 Mbps D-PHY setting

- **Question:** Is reception reliable when RP1 CFE programs its D-PHY for 999 Mbps while the TC358743 transmits at 972 Mbps (default) or 594 Mbps (`link-frequency=297000000`)?
- **Why it matters:** RISK-011; REQ-CAP-001 on Pi 5/CM5; ADR-008 (link frequency, PROPOSED; OQ-099).
- **Known so far:**
  - The CFE driver falls back to 999 Mbps when it finds no sensor entity or link-rate control [C-31]; reasoning from source: that fallback is always taken for this bridge [B-49]. Reasoning: 999 Mbps matches the default 972 Mbps link but not 594 Mbps [C-52].
  - RP1 supports up to 1.5 Gbps per lane and 8 Gbps in total across its two D-PHYs [C-30].
- **Resolution method:** HARDWARE TEST REQUIRED (stream stability and CSI-2 error counters across modes). How to read the CFE error counters is not in the source register (KERNEL SOURCE INSPECTION REQUIRED); the Unicam counterpart is OQ-095.
- **Resolving test:** TEST-CAP-002, TEST-CAP-004
- **Status:** OPEN

## OQ-051 — Source-change event delivery on Pi 5/CM5

- **Question:** On Pi 5/CM5, are `V4L2_EVENT_SOURCE_CHANGE` events delivered reliably on the TC358743 sub-device node, and with what latency?
- **Why it matters:** REQ-CAP-004, RISK-013, ADR-006.
- **Known so far:** An open issue reports that CFE video nodes do not deliver the event; a Raspberry Pi engineer said applications should subscribe on the source sub-device [C-42]. The driver sends the event when a sub-device node exists [B-39], and the sub-device has the events flag [B-18]. Without an IRQ the poll interval is 1000 ms [A-30].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-003
- **Status:** OPEN

## OQ-052 — CM5 carrier-board connector and I2C mapping

- **Question:** If CM5 is used: (a) on the CM4 IO Board, which overlay parameters (or which custom overlay) pair the CAM1 connector's lanes (CM5 MIPI0) with its I2C bus; (b) on the CM5 IO Board, are the J6 jumpers needed for CAM/DISP 1, as the official documentation says, despite a DT comment saying otherwise?
- **Why it matters:** REQ-PLT-001 (CM5); the CM5 configuration in [DEVICE_TREE.md](DEVICE_TREE.md).
- **Known so far:**
  - CM5 MIPI0 uses the CM4 CAM1 pins; the CM4 CAM0 pins carry USB 3.0 on CM5 [C-05].
  - CM5 on the CM4 IO Board: `i2c_csi_dsi1` is i2c6 and `i2c_csi_dsi0` is i2c0 [C-27], [B-44] (B-44 CORRECTED). The overlay pairs `i2c_csi_dsi` with csi1 by default and `i2c_csi_dsi0` with csi0 under `cam0` [B-41], [B-42]. Research found that neither pairing matches the CM4IO CAM1 connector (research open question, topic C).
  - CM5 IO Board: official documentation says CAM/DISP 1 needs two J6 jumpers [C-06]; a comment in `bcm2712-rpi-cm5io.dtsi` says no jumper is needed (research gap, topic C).
- **Resolution method:** HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED.
- **Resolving test:** TEST-HW-001, TEST-PLT-001
- **Status:** OPEN

## OQ-053 — Whether CFE capture buffers use CMA on Pi 5/CM5

- **Question:** With the RP1 CSI nodes behind an IOMMU on BCM2712, are CFE capture buffers allocated from CMA?
- **Why it matters:** CMA sizing (OQ-061, RISK-020) and DMABUF sharing with the software encoder path (REQ-DMA-001).
- **Known so far:** The RP1 CSI nodes have `iommus = <&iommu5>`; whether CFE buffers count against the 64 MB `vc4-kms-v3d-pi5` CMA default is not established (reasoning from kernel source, CORRECTED) [C-53]. CMA defaults per overlay are in [C-40].
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED (`CmaFree` in `/proc/meminfo` while streaming).
- **Resolving test:** TEST-PERF-001
- **Status:** OPEN

## OQ-054 — tc358743-audio overlay on Pi 5/CM5

- **Question:** Does the `tc358743-audio` overlay load and capture I2S audio on Pi 5/CM5 (RP1 I2S), and on which GPIOs?
- **Why it matters:** REQ-CAP-006 on Pi 5/CM5; RISK-014.
- **Known so far:** The overlay uses a dummy "linux,spdif-dir" codec as clock master and the `i2s_clk_consumer` CPU DAI with two 32-bit slots; its README refers to a "tc358743-fast" overlay that has no README entry [B-46]. The README wiring is GPIO 18/19/20 [A-47], [G-14]. Pi 5/CM5 behaviour is unconfirmed (research gaps, topics B, C and G).
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-AUD-001
- **Status:** OPEN

## OQ-055 — 16K versus 4K page-size kernel on Pi 5/CM5

- **Question:** Does any part of the PACSCORDER stack misbehave on Raspberry Pi OS's 16K-page Pi 5 kernel? Does Buildroot's forced 4K-page kernel change rp1-cfe, PiSP or DMABUF behaviour or performance?
- **Why it matters:** Results measured on one OS may not carry over to the other (ADR-003, OQ-012); REQ-PERF-001.
- **Known so far:** `bcm2712_defconfig` uses 16K pages [G-20], [E-51]; Buildroot's Pi 5/CM5 defconfigs force 4K pages [E-09], [G-60]; Raspberry Pi documents that the 4K `kernel8.img` also runs on BCM2712 [G-20]; `rpi-image-gen` tunes EROFS to the page size [G-41].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PERF-001
- **Status:** OPEN

---

# 7. Encoding & DMA

## OQ-056 — Pi 4/CM4 hardware H.264 encode at 1080p60

- **Question:** Can the BCM2711 encoder (`/dev/video11`) sustain 1080p60 from TC358743 UYVY at level 4.2 for long runs, at stock clocks and with `gpu_freq=550`? What is the worst-case IDR frame size at the chosen bitrate, and is the default 768 KiB encoded-buffer size enough? If hardware encode cannot reach 1080p60, what does software x264 on the Pi 4/CM4 CPU achieve?
- **Why it matters:** RISK-002, REQ-ENC-001, OQ-001, ADR-004. This entry covers the measurement only; whether running the product with a raised GPU clock such as `gpu_freq=550` is acceptable at all is the owner decision OQ-096.
- **Known so far:**
  - The official specification is 1080p30 encode [D-10]; the driver calls level 4.0 the hardware specification and says higher levels may not keep up with real time [D-12].
  - Reasoning: 1080p60 is 489,600 macroblocks/s, 2.0x the 1080p30 specification and about 1.13x the documented 720p120 recipe that suggests `gpu_freq=550` [D-52]; `rpicam-apps` forces level 4.2 for it [D-36].
  - A Raspberry Pi engineer reported that 1080p60 was an "edge case" on the hardware encoder [D-50].
  - The default encoded-buffer size is 768 KiB above 720p, and some 1080p frames exceed 512 KiB [D-23]; maximum bitrate is 25 Mbit/s [D-13].
  - No official figures exist for x264 on Pi 4 (research gap, topic D).
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-ENC-001
- **Status:** OPEN

## OQ-057 — Pi 4/CM4 encoder input formats and conversion path

- **Question:** Which input formats does `/dev/video11` accept on the firmware PACSCORDER ships? Does UYVY input cost throughput compared with YUV420 (capture → ISP → encoder versus capture → encoder)? Which device does GStreamer register as `v4l2convert` (`/dev/video12` ISP or `/dev/video18` deinterlace)?
- **Why it matters:** REQ-ENC-001, REQ-DMA-001, ADR-005, ADR-007.
- **Known so far:**
  - Input formats are read from the firmware at probe and filtered against a static table that includes UYVY [D-16]. A 2021 listing posted by a Raspberry Pi engineer (`v4l2-ctl --list-formats-out -d 11`) showed 13 formats including UYVY [D-17]; a Raspberry Pi engineer reported that both TC358743 formats are accepted directly [D-18].
  - `/dev/video12` is a simple ISP converter with DMABUF on both queues [D-25], [D-07]; `/dev/video18` is a deinterlacer [D-27]; node numbers [D-06].
  - GStreamer's V4L2 M2M elements are registered by probing devices at plugin load [D-38], [E-33].
- **Resolution method:** HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED (cost of UYVY input inside the firmware).
- **Resolving test:** TEST-ENC-001, TEST-DMA-001
- **Status:** OPEN

## OQ-058 — Zero-copy DMABUF from Unicam into the Pi 4/CM4 encoder

- **Question:** Can Unicam capture buffers be imported into `bcm2835-codec` without a copy on the current kernel (contiguity, single plane, stride alignment) for every supported input width, including through GStreamer `v4l2h264enc` with `dmabuf-import` on trixie / 6.18?
- **Why it matters:** REQ-DMA-001 and the DMABUF stage of REQ-ARCH-001; ADR-007.
- **Known so far:**
  - The encoder queues support DMABUF through `videobuf2-dma-contig` [D-19]; an import must be one contiguous chunk [D-20] holding a single plane [D-21]; `bytesperline` is rounded to 64 bytes for UYVY and YUV420 [D-22].
  - Unicam uses `videobuf2-dma-contig` with no IOMMU on Pi 4/CM4 (reasoning from kernel source, CORRECTED) [C-53].
  - `rpicam-apps` imports camera DMABUFs into `/dev/video11` [D-34]; GStreamer M2M elements offer `dmabuf-import` [D-53].
  - Reasoning (inputs [C-53]: 1920x1080 UYVY is 4,147,200 bytes, i.e. 2 bytes per pixel; [D-22]: 64-byte alignment): a 1920-pixel line is 3840 bytes, a multiple of 64; a 720-pixel line is 1440 bytes, which is not.
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED (Unicam stride); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-DMA-001
- **Status:** OPEN

## OQ-059 — Pi 5/CM5 software encode: CPU, thermal, latency, concurrency

- **Question:** On Pi 5/CM5, what CPU load, temperature and latency does `libx264`/`x264enc` produce at 1080p60 (and 1080p50/30) from TC358743 UYVY, including the UYVY → I420/NV12 conversion? How many simultaneous encodes (recording + RTMP + WebRTC) fit? Does the official "~30–40 % CPU" for 1080p30 refer to the whole CPU (all cores) or one core? (The source register records no BCM2712 core count.) Is any H.264 hardware block usable on BCM2712?
- **Why it matters:** RISK-003, REQ-ENC-001, REQ-PERF-001, ADR-004, OQ-005, OQ-010.
- **Known so far:**
  - Pi 5 uses software video encoders; BCM2712 "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22], [D-30]. The product briefs list no video encoder [D-31].
  - On the Raspberry Pi forum (community source), one Raspberry Pi engineer (jamesh) stated that BCM2712 has no H.264 hardware block; another (6by9) reported that software 1080p60 encode from camera capture is "easily achievable (which was edge case on the hardware encode)" [D-50].
  - Reasoning: `bcm2835-codec` cannot probe on Pi 5 [D-29]. The BCM2712 DT mentions "(unused) H264 accelerators" behind iommu2 but instantiates no node or driver [D-51].
  - `libx264` and `x264enc` do not accept UYVY [D-43], [D-40]. `rpicam-apps` low-latency `libx264` settings are recorded in [D-35].
- **Resolution method:** HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED (meaning of the CPU figure; the "H264 accelerators" comment).
- **Resolving test:** TEST-ENC-001, TEST-PERF-001
- **Status:** OPEN

## OQ-060 — Pi 5/CM5 UYVY-to-planar conversion offload

- **Question:** Can any BCM2712 hardware block (the PiSP back end as a standalone memory-to-memory converter, or the GPU) convert TC358743 UYVY to I420/NV12 outside libcamera, or must the conversion run on the CPU?
- **Why it matters:** RISK-003, REQ-DMA-001 on Pi 5/CM5, OQ-059.
- **Known so far:**
  - The PiSP back end is in the BCM2712 DT [D-51] and is built as a module [G-17]. Reasoning: there is no `bcm2835-codec` ISP on Pi 5 [D-29].
  - Raspberry Pi engineers reported that libcamera does not support the TC358743 [C-41].
  - A GStreamer `pispconvert` route with DMABuf caps was reported in a community issue (research gap, topic C — not a register fact).
- **Resolution method:** KERNEL SOURCE INSPECTION REQUIRED (PiSP back-end input formats); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-DMA-001, TEST-ENC-001
- **Status:** OPEN

## OQ-061 — CMA budget per platform

- **Question:** How much CMA does the chosen pipeline need on each platform (capture buffers, encoder buffers, `vc-sm-cma`, display if used), and which `cma` / `vc4-kms-v3d` parameter sets it?
- **Why it matters:** RISK-020, REQ-CAP-001, REQ-DMA-001; allocation failures at stream start.
- **Known so far:**
  - Reasoning: four 1080p UYVY buffers take about 16.6 MB, RGB888 about 24.9 MB (CORRECTED) [C-53].
  - The DT CMA pool is 64 MB, limited to the lower 768 MB on Pi 4/CM4 and the lower 1 GB on Pi 5 (CORRECTED) [E-47]. `vc4-kms-v3d-pi4` sets (512 − 4) MB, `vc4-kms-v3d-pi5` 64 MB, and `cma-*` parameters exist [C-40].
  - The codec drivers pull in `vc-sm-cma` [D-03].
- **Resolution method:** HARDWARE TEST REQUIRED (`CmaFree` while streaming at maximum load).
- **Resolving test:** TEST-PERF-001
- **Status:** OPEN

## OQ-062 — DMA-BUF heap names and uapi header

- **Question:** Which `/dev/dma_heap/*` names exist on the chosen kernel, and does the build toolchain provide `linux/dma-heap.h`?
- **Why it matters:** Applications that allocate DMABUFs from a heap (REQ-DMA-001); Buildroot toolchain choice (OQ-064).
- **Known so far:**
  - `rpi-6.18.y` registers "default_cma_region" plus a legacy heap named after the CMA area; 6.12.61 registers only the latter [E-48]. Heaps are enabled in the Raspberry Pi defconfigs [G-19].
  - The Bootlin toolchain in Buildroot 2026.08 declares kernel headers of at least 5.4 [E-12]; research found `dma-heap.h` first appears in v5.6 (research gap, topic E).
- **Resolution method:** HARDWARE TEST REQUIRED; BUILD TEST REQUIRED.
- **Resolving test:** TEST-DMA-001
- **Status:** OPEN

## OQ-063 — Audio encoder choice and cost

- **Question:** If audio is required (OQ-004): which AAC encoder (recording and RTMP) and which Opus encoder (WebRTC), at what CPU cost on each platform, under which licence?
- **Why it matters:** REQ-CAP-006, REQ-STR-001, REQ-STR-002, RISK-019; licensing (OQ-087).
- **Known so far:** RTMP/FLV carries AAC [F-31]; RFC 7874 requires WebRTC endpoints to implement Opus and G.711, and AAC is not a required WebRTC codec, so AAC has to be transcoded (typically to Opus) for browser playback [F-41]; `flvmux` needs raw AAC (CORRECTED) [F-34]; ATEM RTMP audio is AAC [F-06]. Candidate encoders (`fdkaacenc`, `avenc_aac`, `voaacenc`) and FDK-AAC licence terms were not researched (research gaps, topics D and F).
- **Resolution method:** HARDWARE TEST REQUIRED; LEGAL CLARIFICATION REQUIRED (encoder licences).
- **Resolving test:** TEST-AUD-001, TEST-STR-001, TEST-STR-002
- **Status:** OPEN

---

# 8. Build system & OS

## OQ-064 — Buildroot baseline: release series, kernel and firmware pin

- **Question:** If Buildroot is used (the ADR-003 alternative): which Buildroot release series, which Raspberry Pi kernel (6.12.61 as pinned, or `rpi-6.18.y`), and which raspberrypi/firmware commit matches that kernel?
- **Why it matters:** REQ-BLD-001, ADR-003; driver, overlay and Kconfig behaviour differ between kernel series.
- **Known so far:**
  - Buildroot 2026.08 (end of life December 2026) pins kernel 6.12.61 and firmware 063bcab6 [E-01], [E-06], [E-10]. The next LTS is 2027.02 [E-03]. LTS 2025.02.x has no CM5 defconfig, kernel 6.6.28, and firmware without `tc358743-pi5.dtbo` [E-05], [E-11], [E-53].
  - The Raspberry Pi default branch is `rpi-6.18.y` (6.18.55) [E-37]. Kconfig meanings differ between series for RP1 CFE and Unicam (CORRECTED) [E-45], [E-40], [E-41].
  - The 6.12.61 defconfigs build `tc358743`, Unicam, RP1 CFE and `bcm2835-codec` as modules [G-63], [E-39].
  - Research reports that 32-bit BCM2711 kernels are not supported from 6.18 (research gap, topic E — NEEDS VERIFICATION).
- **Resolution method:** OWNER DECISION REQUIRED (only if Buildroot is chosen); BUILD TEST REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-BLD-001
- **Status:** OPEN

## OQ-065 — Buildroot boot integration: overlays and module autoloading

- **Question:** In a Buildroot image, how are `tc358743` / `tc358743-pi5` overlays and `overlay_map.dtb` installed (firmware tarball or kernel tree; why does the Pi 5 defconfig disable overlays), and how are the driver modules loaded (mdev, eudev, systemd-udev or an explicit load step)?
- **Why it matters:** Without both, the TC358743 never probes on a Buildroot image (REQ-BLD-001, REQ-DRV-001).
- **Known so far:**
  - Overlays come from the firmware tarball, not the kernel build [E-14], [E-20], [G-61]. `raspberrypi5_defconfig` disables overlay installation; the CM5 IO defconfig enables it [E-15].
  - Whether Buildroot's DTSO options can build Raspberry Pi `-overlay.dts` files is untested (research open question, topic E).
  - The media drivers are modules [E-49]; Buildroot's default devtmpfs-only `/dev` does not load modules automatically, mdev or udev does [E-50].
  - `bcm2835-codec` depends on the VCHIQ platform device [D-28] and on non-cut-down firmware [D-48].
- **Resolution method:** BUILD TEST REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-BLD-001, TEST-PLT-001
- **Status:** OPEN

## OQ-066 — Media-stack differences between Buildroot and Raspberry Pi OS

- **Question:** What do Raspberry Pi's `+rpt` patches to FFmpeg and GStreamer change? Does Buildroot's FFmpeg 6.1.5 build enable `h264_v4l2m2m`, and is its V4L2 M2M code MMAP-only? Do any needed features (`webrtcbin`, `rtmp2sink`, `eflvmux`, gst-plugins-rs) need a newer GStreamer than Buildroot's 1.24.13?
- **Why it matters:** REQ-DMA-001, REQ-STR-001, REQ-STR-002, ADR-003, ADR-007.
- **Known so far:**
  - Raspberry Pi OS: FFmpeg 7.1.5 `+rpt2` [G-29] with DMABUF input to the V4L2 M2M encoder [D-45]; GStreamer 1.26.2 with Raspberry Pi builds of plugins-base and plugins-bad [G-26], [G-27], [G-31]. Source packages are published [G-70].
  - Buildroot: upstream FFmpeg 6.1.5 without Raspberry Pi patches (CORRECTED) [D-46]; upstream V4L2 M2M code is MMAP-only [D-44]; GStreamer 1.24.13 [E-31], [G-64]; `v4l2h264enc` needs the V4L2_PROBE option [E-32], [D-39].
  - `eflvmux` first appears in GStreamer 1.28, absent from 1.24 and 1.26 (CORRECTED) [F-34]; Buildroot has no gst1-plugins-rs package [F-43].
- **Resolution method:** BUILD TEST REQUIRED (build, inspect and diff the source packages).
- **Resolving test:** TEST-BLD-001, TEST-DMA-001
- **Status:** OPEN

## OQ-067 — Reproducible Raspberry Pi OS based builds

- **Question:** How can a PACSCORDER image built with `rpi-image-gen` be rebuilt identically later? Can `archive.raspberrypi.com` packages be pinned (snapshot service, retention policy, or a private mirror)? Which build host will the project use?
- **Why it matters:** RISK-017, REQ-BLD-001, ADR-003.
- **Known so far:**
  - `rpi-image-gen` describes itself as designed for reproducible artefacts and supports only native arm64 Debian hosts [G-39]; its minor releases contain breaking changes [G-44].
  - `apt full-upgrade` updates kernel and firmware [G-08]; the default kernel series changed in the middle of the trixie release [G-05]; the archive still carries older versioned kernel packages [G-07].
  - A snapshot.debian.org layer exists for the Debian half only; no equivalent was found for `archive.raspberrypi.com` (research gap, topic G).
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (Raspberry Pi archive retention); BUILD TEST REQUIRED (two builds weeks apart, SBOM comparison).
- **Resolving test:** TEST-BLD-001
- **Status:** OPEN

## OQ-068 — Image size, RAM use and boot-to-first-frame time

- **Question:** What image size, RAM use and boot-to-first-frame time does a PACSCORDER image reach for each OS option and platform, and what boot-time target does the owner require?
- **Why it matters:** The ADR-003 trade-off has no measurement behind it; REQ-BLD-001; storage choice (OQ-006).
- **Known so far:**
  - Raspberry Pi OS Lite 2026-10-06 is 550 MB compressed and 3.08 GB raw [G-33], with 633 installed packages including cloud-init and NetworkManager [G-32].
  - Buildroot sample images use a 32 MB boot partition and a 120 MB ext4 rootfs [E-19], which research judged too small for the media stack (research gap, topic E).
  - Documented boot-time settings are listed in [G-54] (CORRECTED).
- **Resolution method:** OWNER DECISION REQUIRED (target); BUILD TEST REQUIRED; HARDWARE TEST REQUIRED (measure).
- **Resolving test:** TEST-BLD-001
- **Status:** OPEN

## OQ-069 — Field update mechanism

- **Question:** How will units in the field be updated: full-image A/B (`rpi-image-gen` `image-rota`; whether it uses tryboot is asked below), Raspberry Pi Connect Remote Update, a third-party updater (RAUC, SWUpdate, Mender), or not at all? Does `image-rota` use tryboot/`autoboot.txt`, and does it support CM4/CM5 eMMC? Does Connect Remote Update support Compute Modules, and can it run without Raspberry Pi's cloud service?
- **Why it matters:** REQ-BLD-001; partition layout for recordings (OQ-006); ADR-003.
- **Known so far:**
  - `image-rota`: immutable A/B slots with a persistent partition; EROFS, dm-verity and LUKS2 options [G-40], [G-41].
  - `autoboot.txt` with tryboot is the firmware's A/B mechanism [G-50], [G-51] (G-51 CORRECTED); EEPROM A/B updates are Pi 5/CM5 only [G-52].
  - Connect Remote Update: the current documentation requires a Pi 4 or later on the internet, signed in to Connect, with at least 16 GB of storage, and no longer calls the feature beta (CORRECTED) [G-43]; `rpi-image-gen` can build Connect images [G-42]. Compute Module support is unconfirmed (research gap, topic G).
  - Debian trixie packages RAUC, SWUpdate and Mender [G-49]; Buildroot recommends whole-image upgrades [G-67].
- **Resolution method:** OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-070 — Support horizon and security updates for Raspberry Pi packages

- **Question:** For how long will Raspberry Pi maintain trixie's kernel, firmware and `+rpt` packages, and how quickly do the `+rpt` overrides receive Debian security fixes?
- **Why it matters:** Product maintenance lifetime (ADR-003, RISK-017).
- **Known so far:** Debian 13 full support runs to 2028-08-09 and LTS to 2030-06-30 [G-03]. For the previous OS, a Raspberry Pi staff comment promised only critical kernel fixes (CORRECTED) [G-55]. A new Raspberry Pi OS major release follows each Debian major release [G-56]. Raspberry Pi overrides several media packages [G-27], [G-29], [G-31].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-071 — Production hardening and secure boot

- **Question:** Which preinstalled components are removed (cloud-init, `rpi-connect-lite`, `rpi-update`)? How is the EEPROM bootloader version pinned and recorded? Is a boot watchdog used? Are secure boot and/or filesystem encryption required?
- **Why it matters:** REQ-BLD-001, ADR-003 consequences. Reasoning from [G-45] and [G-46]: secure-boot provisioning writes OTP, which cannot be undone, so the decision must precede production provisioning.
- **Known so far:**
  - The Lite image includes cloud-init [G-32], `rpi-connect-lite` [G-53] and `rpi-update` [G-10]; `rpi-update` installs pre-release firmware that can leave a system unbootable [G-09]; the `rpi-eeprom` updater is installed [G-06].
  - `BOOT_WATCHDOG_TIMEOUT` can reset a unit whose OS never starts (CORRECTED) [G-54].
  - `rpi-sb-provisioner` supports secure boot on Pi 4, Pi 5, CM4 and CM5 [G-46]; rpiboot extensions provision OTP [G-45]; `rpi-image-gen` integrates with the provisioner [G-39].
  - Buildroot does not manage the EEPROM bootloader (research gap, topic E).
- **Resolution method:** OWNER DECISION REQUIRED; BUILD TEST REQUIRED.
- **Resolving test:** TEST-BLD-001
- **Status:** OPEN

## OQ-072 — Is camera_auto_detect=0 needed with the TC358743 overlay?

- **Question:** Must `camera_auto_detect=0` be set in `config.txt` when only the TC358743 overlay is used?
- **Why it matters:** Boot configuration on every platform ([BUILD_SYSTEM.md](BUILD_SYSTEM.md), [DEVICE_TREE.md](DEVICE_TREE.md)).
- **Known so far:** Official documentation requires it for the listed camera-sensor overlays and does not state it for TC358743; disabling it is prudent but not a documented requirement (CORRECTED) [C-39], [G-15].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PLT-001
- **Status:** OPEN

## OQ-094 — Software updates versus active recordings

- **Question:** May an update be installed, or a reboot into a new slot be triggered, while a recording or stream is active? What happens to an in-progress recording?
- **Why it matters:** REQ-REC-001, REQ-BLD-001, ADR-003, [RELEASE.md](RELEASE.md).
- **Known so far:** `apt full-upgrade` updates the kernel and firmware [G-08]; A/B update mechanisms exist (`image-rota` [G-40], [G-41]; firmware `tryboot` [G-50], [G-51]).
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-100 — config.txt overlay-parameter syntax

- **Question:** Are bare boolean overlay parameters (for example `,4lane` or `,media-controller` without `=value`) and several parameters on one `dtoverlay=` line accepted? Does a `[pi4]` filter section match a CM4? What does the loader do with an unknown parameter?
- **Why it matters:** [DEVICE_TREE.md](DEVICE_TREE.md), ADR-006, TEST-PLT-001.
- **Known so far:** The documented form is `dtoverlay=tc358743,<param>=<val>` [G-12]; appending `,cam0` selects connector 0 [C-39]. The overlay defines `4lane`, `media-controller`, `link-frequency` and `cam0` parameters [B-42], [B-43], [G-12].
- **Scope note (2026-10-06, cross-document consistency; the question above is unchanged):** [DEVICE_TREE.md](DEVICE_TREE.md) §6.0 also links here whether `config.txt` accepts a comment after a value on the same line. That is a `config.txt` syntax point of the same kind and is resolved by the same documentation check.
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (official config.txt / overlay documentation); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-PLT-001
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

## OQ-101 — Command syntax used in procedures but not in the source register

- **Question:** What is the exact syntax for the procedure steps marked NEEDS VERIFICATION: `v4l2-ctl -d` with a sub-device path, the `--clear-edid` argument form, printing the media topology, streaming and counting frames, waiting for events, `i2cdetect -y`, the `rpi-image-gen` build command, and recording kernel/firmware/EEPROM versions?
- **Why it matters:** [TESTING.md](TESTING.md), [V4L2.md](V4L2.md), [BUILD_SYSTEM.md](BUILD_SYSTEM.md); every procedure must be runnable as written.
- **Known so far:** Attested forms: `--set-edid pad=<pad>[,…]` and `--clear-edid <pad>` [B-24]; `-d 11` in a listing posted by a Raspberry Pi engineer [D-17]; the `media-ctl -l` link example and running on `/dev/v4l-subdevN` as reported by a Raspberry Pi engineer [C-33] (both community sources). `i2cdetect -y X` appears only in the owner's Rule 9 example (Rule 23 priority 7).
- **Scope note (2026-10-06, cross-document consistency; the question above is unchanged):** Other documents link further unattested procedure commands of the same kind here: exporting the installed-package list and reading the shipped kernel configuration ([BUILD_SYSTEM.md](BUILD_SYSTEM.md)); enumerating video devices to find the encoder node, sampling CPU load, and reading SoC temperature and throttling state ([VIDEO_ENCODER.md](VIDEO_ENCODER.md), [PERFORMANCE.md](PERFORMANCE.md)). They are resolved the same way, from tool documentation or a hardware run.
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (tool documentation: v4l-utils, i2c-tools, rpi-image-gen); HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-HW-001, TEST-DRV-002, TEST-PLT-001, TEST-BLD-001
- **Status:** OPEN
- **Added:** 2026-10-06 (after the cross-document review; not from the original research lists)

---

# 9. Streaming & WebRTC

## OQ-073 — H.264 level signalling for 1080p WebRTC in browsers

- **Question:** Do Chrome, Firefox and Safari decode a 1080p Constrained Baseline stream when the SDP negotiated `42e01f` (Level 3.1), with or without `level-asymmetry-allowed=1`? Or must PACSCORDER signal Level 4.0 or 4.2?
- **Why it matters:** RISK-019, REQ-STR-002.
- **Known so far:**
  - Reasoning: `42e01f` is Constrained Baseline Level 3.1 [F-38]; it is libwebrtc's default [F-39].
  - Reasoning from H.264 Table A-1 as encoded in FFmpeg: 1080p needs Level 4 (level_idc 0x28) or above, and 1080p60 needs Level 4.2 (0x2A) [F-40]. The ITU-T specification was not cited directly (research gap, topic D).
  - RFC 6184 level-asymmetry rules (CORRECTED) [F-37]; RFC 7742 requirements [F-36].
  - As reported in a Raspberry Pi engineer's 2020 TC358743 instructions (community source), the GStreamer example uses `h264_level=10`, which is Level 3.2 [D-54].
- **Resolution method:** HARDWARE TEST REQUIRED (browser interoperability test).
- **Resolving test:** TEST-STR-002
- **Status:** OPEN

## OQ-074 — WebRTC signalling and NAT traversal design

- **Question:** Which signalling (own code with `webrtcbin`, `webrtcsink`'s built-in server, MediaMTX WHEP, or WHIP to an external server) and which ICE/STUN/TURN arrangement will PACSCORDER use, given the scope from OQ-008?
- **Why it matters:** REQ-STR-002, ADR-007; packaging (OQ-066).
- **Known so far:** `webrtcbin` has no signalling and needs libnice [F-42], [G-28]. `webrtcsink` (MPL-2.0) includes a signalling server but is not packaged in Buildroot [F-43]. The MediaMTX project reports a WHEP endpoint and browser page [F-45] and has no Buildroot package [F-44]. WHIP/WHEP standards and ICE requirements were not sourced (research gap, topic F).
- **Resolution method:** OWNER DECISION REQUIRED (with ADR-007); BUILD TEST REQUIRED.
- **Resolving test:** TEST-STR-002
- **Status:** OPEN

## OQ-075 — RTMP server on PACSCORDER and network ports

- **Question:** Will PACSCORDER host an RTMP server (for example to receive an ATEM's stream), with which software and packaging? Which ports must the product open (RTMP, WebRTC, ATEM control), and how are conflicts avoided when it both receives and publishes RTMP?
- **Why it matters:** REQ-STR-001, REQ-ATEM-001, OQ-009; network documentation in [STREAMING.md](STREAMING.md).
- **Known so far:**
  - Reasoning: to receive an ATEM push, PACSCORDER must run a listening server [F-46]; `rtmp2src` cannot listen [F-33]; FFmpeg has a `listen` option [F-32].
  - The MediaMTX project reports default addresses `rtmpAddress :1935`, `webrtcAddress :8889` and `webrtcLocalUDPAddress :8189` [F-44]. Reasoning (not stated by the source): :8189 is the UDP port for WebRTC ICE/media traffic.
  - The OpenSwitcher project reports ATEM control on UDP 9910 [F-11]; Blackmagic documents forwarding TCP 1935 to a Streaming Bridge for internet links (CORRECTED) [F-28].
- **Resolution method:** OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-STR-001, TEST-ATEM-001
- **Scope note (2026-10-07):** Receiving an ATEM's RTMP stream is not in current scope: OQ-009 was ANSWERED on 2026-10-07 with HDMI capture of the ATEM output only. The ports for PACSCORDER's own RTMP publishing and WebRTC (REQ-STR-001, REQ-STR-002) remain in scope. Status unchanged.
- **Status:** OPEN

## OQ-076 — Is SRT required?

- **Question:** Must PACSCORDER send or receive SRT in addition to RTMP?
- **Why it matters:** Scope of REQ-STR-001; packaging.
- **Known so far:** The ATEM SDK streams over RTMP or SRT [F-06]; ATEM Mini Pro streams RTMP and SRT [F-26]; the MediaMTX project reports SRT conversion [F-44]. GStreamer/Buildroot SRT support and the ATEM's SRT caller/listener mode were not researched (research gap, topic F).
- **Resolution method:** OWNER DECISION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

---

# 10. ATEM

## OQ-077 — Third-party ATEM protocol support for ATEM 10.x and concurrent clients

- **Question:** Do the third-party UDP 9910 libraries decode tally, recording status and streaming status on current ATEM 10.x firmware? How many simultaneous control clients (PACSCORDER, ATEM Software Control and others) does an ATEM Mini accept?
- **Why it matters:** RISK-018, REQ-ATEM-001.
- **Known so far:**
  - The atem-connection source is reported to define protocol versions only up to V9_6 (2.32), although ATEM 10.x software has shipped [F-18]; Blackmagic lists the ATEM Switchers 10.4.1 SDK, dated 3 September 2026 [F-02]. The project warns that new firmware will likely need library updates [F-15]; its recording/streaming status commands need protocol 2.30 or later [F-16].
  - The connection handshake and session behaviour are reported by the community [F-13]. No source gave a client limit (research open question, topic F).
- **Resolution method:** HARDWARE TEST REQUIRED (against current ATEM firmware).
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Not in current scope: network tally/control over UDP 9910 was not selected (OQ-009 ANSWERED 2026-10-07). Kept as reference; it applies only if the owner adds network control. Status unchanged.
- **Status:** OPEN

## OQ-078 — Routing the ATEM Mini Pro HDMI output to Program

- **Question:** Can PACSCORDER switch the ATEM Mini Pro HDMI output from its default multiview to Program over UDP 9910 (for example with the aux-source command for aux index 0), or must the operator set it on the front panel or in ATEM Software Control?
- **Why it matters:** Otherwise PACSCORDER records the multiview (REQ-ATEM-001; operator procedure in [ATEM.md](ATEM.md)).
- **Known so far:** The HDMI output defaults to multiview and is changed with front-panel buttons or ATEM Software Control [F-25]. atem-connection is reported to route aux outputs with "CAuS" [F-17]. No source maps the Mini Pro HDMI output to aux index 0 (research open question, topic F).
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Network control is not in current scope (OQ-009 ANSWERED 2026-10-07), so in current scope the operator sets the HDMI output source. Routing the output to Program still matters for HDMI capture of the ATEM output (REQ-CAP-008); the UDP 9910 part applies only if the owner adds network control. Status unchanged.
- **Status:** OPEN

## OQ-079 — Encoding parameters of the ATEM's RTMP output

- **Question:** Which H.264 profile and level, B-frame use, GOP length, SPS/PPS cadence and AAC parameters does the ATEM Mini Pro's RTMP output use, and does it ever send H.265?
- **Why it matters:** Decides whether an ATEM stream can be relayed to browsers over WebRTC without re-encoding (REQ-STR-002, REQ-ATEM-001, RISK-019).
- **Known so far:** The SDK streams H.264 or H.265 with AAC [F-06]; the profile XML sets bitrate, audio bitrate and keyframe interval (example value 2) [F-30]. The MediaMTX project reports that browsers do not accept H.264 B-frames in WebRTC [F-45]. RFC 7874 requires WebRTC endpoints to implement Opus and G.711; AAC is not a required WebRTC codec, so AAC has to be transcoded (typically to Opus) for browser playback [F-41].
- **Resolution method:** HARDWARE TEST REQUIRED (receive and analyse a real ATEM stream).
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Not in current scope: receiving the ATEM's RTMP stream was not selected (OQ-009 ANSWERED 2026-10-07). Kept as reference. Status unchanged.
- **Status:** OPEN

## OQ-080 — ATEM publishing to a PACSCORDER RTMP URL on a LAN

- **Question:** Can an ATEM Mini Pro/Extreme publish to an arbitrary RTMP URL on a LAN without internet access? How is the URL set: XML streaming profile, a field in ATEM Software Control, or the streaming-service command over the network?
- **Why it matters:** Option (3) of OQ-009; OQ-075.
- **Known so far:** The SDK exposes SetUrl, SetKey and SetProfileXml [F-06], [F-30]. atem-connection is reported to set the streaming service with "CRSS" [F-16]. Research found an SDK statement that streaming needs an internet connection on the Ethernet port (research gap, topic F — not a register fact).
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Not in current scope: option (3) of OQ-009 was not selected (OQ-009 ANSWERED 2026-10-07). Kept as reference. Status unchanged.
- **Status:** OPEN

## OQ-081 — ATEM Streaming Bridge with a non-Blackmagic encoder

- **Question:** Does an ATEM Streaming Bridge accept RTMP from PACSCORDER (stream key format, codec and bitrate constraints)?
- **Why it matters:** Option (4) of OQ-009: sending PACSCORDER video into an ATEM setup.
- **Known so far:** Documented inputs are RTMP from Web Presenter, ATEM Mini and ATEM SDI, and SRT from Web Presenter (CORRECTED) [F-28]. ATEM Mini Extreme ISO G2 remote sources are compatible Blackmagic cameras [F-29].
- **Resolution method:** VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Not in current scope as an ATEM integration: option (4) of OQ-009 was not selected (OQ-009 ANSWERED 2026-10-07). It becomes relevant again if the owner adds that integration or names a Streaming Bridge as an RTMP destination (OQ-007). Status unchanged.
- **Status:** OPEN

## OQ-082 — ATEM USB-C webcam output under Linux

- **Question:** Does the ATEM Mini USB-C webcam output enumerate as a standard UVC (and UAC) device under Linux on Pi 4, CM4, Pi 5 and CM5, and in which formats and resolutions? Could it replace HDMI capture for ATEM sources?
- **Why it matters:** An alternative capture path for REQ-ATEM-001 that bypasses the TC358743.
- **Known so far:** Blackmagic documents webcam use on Mac and Windows only and makes no Linux or UVC statement [F-27]. USB capture from switchers goes through the DeckLink SDK, which lists Linux [F-09].
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Not in current scope: the recorded scope is HDMI capture through the TC358743 (REQ-CAP-008; OQ-009 ANSWERED 2026-10-07), and this path bypasses the TC358743. Kept as reference. Status unchanged.
- **Status:** OPEN

## OQ-083 — ATEM HDMI output as seen by the TC358743

- **Question:** For the ATEM HDMI output: is HDCP ever asserted? Which pixel encoding (YCbCr 4:2:2, 4:4:4 or RGB) and quantisation range arrive? Does the ATEM honour the sink EDID, or always output its own video standard? How many embedded audio channels, at which sample rate?
- **Why it matters:** REQ-ATEM-001, REQ-CAP-001, REQ-CAP-006, RISK-008, OQ-002.
- **Known so far:** Output standards are 1080p23.98 to 1080p60 with 4:2:2 YUV 10-bit Rec 709 sampling [F-23]. Program audio is embedded on the HDMI output; the ATEM's HDMI *inputs* carry 2-channel audio, while the output's channel count and sample rate are not stated (CORRECTED) [F-24]. The driver disables HDCP [A-04] and always configures 2-channel I2S audio output [A-13]; the silicon can also send audio over CSI-2 [A-05] (OQ-033).
- **Resolution method:** HARDWARE TEST REQUIRED.
- **Resolving test:** TEST-CAP-001, TEST-ATEM-001
- **Scope note (2026-10-07):** In current scope: this is the HDMI capture of the ATEM output (REQ-CAP-008; OQ-009 ANSWERED 2026-10-07). Which ATEM models must be covered is OQ-102.
- **Status:** OPEN

## OQ-084 — ATEM control library and runtime

- **Question:** Which library, or an own implementation, will PACSCORDER use for UDP 9910, given licence and runtime constraints?
- **Why it matters:** REQ-ATEM-001, RISK-018; licensing (OQ-087); image content (OQ-066).
- **Known so far (community sources, as reported by each project):** atem-connection is MIT and needs Node.js [F-14]; PyATEMMax is GPL-3.0 and lacks recording/streaming status [F-19], [F-20]; pyatem is LGPL-3.0-only and supports network and USB [F-21]; LibAtem is LGPL-3.0, needs .NET and is incomplete [F-22]. There is no official Linux SDK [F-01].
- **Resolution method:** OWNER DECISION REQUIRED; LEGAL CLARIFICATION REQUIRED.
- **Resolving test:** TEST-ATEM-001
- **Scope note (2026-10-07):** Not in current scope: network control over UDP 9910 was not selected (OQ-009 ANSWERED 2026-10-07). Kept as reference; it applies only if the owner adds network control. Status unchanged.
- **Status:** OPEN

---

# 11. Legal, licensing & supply

## OQ-085 — TC358743 lifecycle and long-term supply

- **Question:** What is Toshiba's lifecycle status and longevity commitment for TC358743XBG?
- **Why it matters:** RISK-004; product BOM continuity; alternatives in OQ-034.
- **Known so far:**
  - The current public datasheet is Rev. 2.20 dated 2026-05-11 [A-01].
  - Research recorded that a Raspberry Pi engineer said, in the community issue thread behind [C-43], that he believed the part is end-of-life; research also found Toshiba's product page showing mass production with no EOL flag on 2026-10-06 (both research open questions/gaps, topics A and C — not register facts).
- **Resolution method:** VENDOR CONFIRMATION REQUIRED (written statement from Toshiba sales).
- **Resolving test:** —
- **Status:** OPEN

## OQ-086 — H.264 patent licensing

- **Question:** What H.264 patent licensing applies to PACSCORDER, for hardware encode on Pi 4/CM4 and for software encode (x264, openh264) on Pi 5/CM5?
- **Why it matters:** RISK-015, REQ-ENC-001, product release.
- **Known so far:** Not researched (research gaps, topics D and G). Software encoding is unavoidable on Pi 5/CM5 [G-22], [D-31].
- **Resolution method:** LEGAL CLARIFICATION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-087 — GPL and source-offer compliance

- **Question:** What are PACSCORDER's copyleft obligations (x264 GPL, FFmpeg built with `--enable-gpl`, LGPL GStreamer plugins, the chosen ATEM library)? How will corresponding source be supplied for packages from both Debian and `archive.raspberrypi.com`? Is the `rpi-image-gen` SBOM (or Buildroot `legal-info`) enough to produce it? Is openh264 a viable non-GPL alternative?
- **Why it matters:** RISK-015; [RELEASE.md](RELEASE.md); ADR-003, ADR-007.
- **Known so far:**
  - `libx264` requires FFmpeg `--enable-gpl` [D-42]; Buildroot marks FFmpeg GPL when x264 is enabled (CORRECTED) [D-46]; x264 is GPL [D-47].
  - `webrtcbin` is declared LGPL [F-42]; `webrtcsink` is MPL-2.0 [F-43].
  - Raspberry Pi publishes source packages [G-70]; its images ship with an SPDX SBOM [G-34]; `rpi-image-gen` produces SBOM and CVE reports [G-39]; Buildroot's `legal-info` output must be checked by the user [G-68].
  - `openh264enc` accepts only I420 [D-41]; Buildroot has no openh264 package (research gap, topic D).
- **Resolution method:** LEGAL CLARIFICATION REQUIRED; BUILD TEST REQUIRED (SBOM-to-source mapping).
- **Resolving test:** —
- **Status:** OPEN

## OQ-088 — Proprietary GPU firmware licence

- **Question:** Do the terms of the Raspberry Pi GPU firmware licence permit PACSCORDER's commercial distribution model?
- **Why it matters:** [RELEASE.md](RELEASE.md); ADR-003.
- **Known so far:** The firmware licence is binary-only and no-modification, restricted to use "for the purposes of developing for, running or using a Raspberry Pi device"; Buildroot's BSD-3-Clause label for it is misleading (CORRECTED) [G-69].
- **Resolution method:** LEGAL CLARIFICATION REQUIRED.
- **Resolving test:** —
- **Status:** OPEN

## OQ-089 — Blackmagic ATEM SDK licence terms

- **Question:** What do the "bmd-standard-sdk" terms allow (redistribution, use on unsupported operating systems, reverse-engineering clauses), and do they affect the use of third-party UDP 9910 libraries?
- **Why it matters:** REQ-ATEM-001, RISK-018, OQ-084.
- **Known so far:** Downloading the SDK requires accepting the "bmd-standard-sdk" terms [F-02]; the SDK supports Windows and macOS only [F-01]; its runtime ships inside the Mac and Windows product installers [F-04].
- **Resolution method:** LEGAL CLARIFICATION REQUIRED; VENDOR CONFIRMATION REQUIRED.
- **Resolving test:** —
- **Scope note (2026-10-07):** Relevant only if the owner adds network control: UDP 9910 integration was not selected (OQ-009 ANSWERED 2026-10-07). Status unchanged.
- **Status:** OPEN

---

# Appendix A — Research items to OQ mapping

Every item in `topics.<key>.open_questions` and `topics.<key>.gaps` of [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json), by 1-based position in its list, mapped to the OQ(s) that carry it. Some items are already answered by register facts from another topic; they map to the OQ whose **Known so far** records that answer.

| Topic | `open_questions` item → OQ | `gaps` item → OQ |
|---|---|---|
| A — TC358743 hardware | 1→026; 2→031; 3→028; 4→029; 5→030, 002; 6→031; 7→033; 8→018, 019, 020, 021; 9→034; 10→085; 11→035 | 1→049; 2→021, 049; 3→019, 013; 4→038; 5→037; 6→002; 7→002; 8→002; 9→031; 10→026; 11→024; 12→028; 13→033, 025, 024, 054; 14→041; 15→085; 16→042 |
| B — tc358743 Linux driver | 1→032; 2→030; 3→020; 4→019; 5→049; 6→050; 7→035; 8→036; 9→033; 10→028, 083; 11→037; 12→064, 065; 13→044 | 1→042; 2→014; 3→051; 4→no OQ (behaviour established by [B-23], [B-39]; carried by REQ-CAP-004 and TEST-CAP-003); 5→002; 6→038; 7→021, 049; 8→020; 9→026, 031; 10→027, 030, 035; 11→041; 12→028; 13→033, 054, 025; 14→043; 15→065; 16→039; 17→042 |
| C — Raspberry Pi CSI-2 receive path | 1→050; 2→045; 3→052; 4→022; 5→021, 019; 6→035; 7→085; 8→047; 9→053; 10→052; 11→064, 065 | 1→019; 2→021; 3→037; 4→021; 5→046; 6→014; 7→045; 8→051, 020; 9→052, 043; 10→054, 049; 11→002, 028; 12→035, 085; 13→050; 14→060, 059; 15→064, 065; 16→061, 053 |
| D — Encoders | 1→056; 2→057; 3→057; 4→059; 5→059; 6→060; 7→059; 8→058; 9→066; 10→057; 11→086, 087; 12→056 | 1→056; 2→059; 3→060; 4→001, 021; 5→061, 048; 6→058; 7→087, 086; 8→005, 073; 9→066, 064, 015; 10→059; 11→063; 12→073; 13→056; 14→065, 048 |
| E — Buildroot and kernel configuration | 1→065; 2→065; 3→064; 4→048; 5→066; 6→062; 7→055; 8→061; 9→064; 10→066 | 1→065; 2→065; 3→062; 4→062; 5→061; 6→001, 049; 7→059, 066; 8→048; 9→064; 10→064; 11→068, 069; 12→071; 13→026; 14→016; 15→014; 16→064; 17→066, 074, 084 |
| F — ATEM and streaming | 1→089; 2→077; 3→078; 4→079; 5→080; 6→081; 7→082; 8→077; 9→083; 10→073; 11→066 (answered by [F-34]); 12→059 (answered by [D-10], [D-31], [G-22]) | 1→004, 025, 083; 2→002, 083; 3→083; 4→078; 5→079; 6→080; 7→077; 8→082; 9→084; 10→066, 075; 11→063; 12→059; 13→073; 14→076; 15→074; 16→075 |
| G — Raspberry Pi OS and image tooling | 1→049; 2→044; 3→056, 058; 4→059; 5→067; 6→069; 7→069; 8→069; 9→072; 10→068; 11→064; 12→070; 13→055; 14→061 | 1→068; 2→067; 3→066; 4→070; 5→069; 6→049, 054; 7→059; 8→086; 9→064; 10→071; 11→087; 12→012 |

# Appendix B — Requirements, decisions and risks to OQ mapping

| Source | Item → OQ |
|---|---|
| [REQUIREMENTS.md](REQUIREMENTS.md) UNDEFINED / owner-to-confirm items | REQ-CAP-001 (60 Hz mandatory, 59.94 Hz, 50 Hz, pixel format, audio) → 001, 002, 003, 004; REQ-CAP-007 (2-lane and 4-lane, all frame rates; owner 2026-10-07) → 001, 002, 021, 038, 040; REQ-CAP-008 (ATEM and camera sources; owner 2026-10-07) → 009, 102; REQ-BLD-002 (own OS image; owner 2026-10-07) → 012; REQ-CAP-003 (EDID contents) → 002 (provisioning trigger and ordering → 093); REQ-CAP-006 (audio required) → 004; REQ-ENC-001 (codec, bitrate, latency, simultaneous encodes) → 005; REQ-REC-001 (container, storage, duration, power loss) → 006; REQ-STR-001 (server targets, bitrate) → 007; REQ-STR-002 (browsers, latency, LAN/internet) → 008; REQ-ATEM-001 (kind of integration) → 009; REQ-PERF-001 (duration, drop threshold, temperature) → 010; REQ-PLT-001 (platform) → 011; REQ-BLD-001 (OS/build) → 012; acceptance of all DRAFT/PROPOSED requirements → 017 |
| [DECISIONS.md](DECISIONS.md) OPEN / PROPOSED, and ADR-003 (ACCEPTED 2026-10-07) | ADR-002 → 013; ADR-003 → 012 (ANSWERED 2026-10-07; Buildroot alternative → 064, 065, 066; reproducibility → 067; image size and boot time → 068; hardening → 071; EDID provisioning at boot → 093); ADR-004 → 011; ADR-005 → 003; ADR-006 → 014 (`config.txt` parameter syntax → 100; `v4l2-ctl -d` sub-device form → 101); ADR-007 → 015; ADR-008 → 099 |
| [RISKS.md](RISKS.md) (each risk's **Open questions** line) | RISK-001 → 001, 011, 021, 038, 099; RISK-002 → 056, 096; RISK-003 → 059, 060; RISK-004 → 085, 034; RISK-005 → 027; RISK-006 → 035, 038, 095, 099; RISK-007 → 019, 013; RISK-008 → 028, 083; RISK-009 → 002; RISK-010 → 002, 032, 093; RISK-011 → 050, 095, 099; RISK-012 → 049, 050, 051, 052; RISK-013 → 020, 051; RISK-014 → 004, 025, 033, 054; RISK-015 → 086, 087, 088; RISK-016 → 045, 003; RISK-017 → 067, 070; RISK-018 → 009, 077, 084, 089; RISK-019 → 008, 073, 074; RISK-020 → 053, 061; RISK-021 → 018, 019, 021, 022, 024 |

---

# Verification status

## Verified from sources (fact IDs)

Every **Known so far** statement cites entries of [REFERENCES.md](REFERENCES.md) whose verdict is `CONFIRMED` or `CORRECTED`; no `UNVERIFIABLE` or `REFUTED` entry is cited. This register cites 301 distinct facts:

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-01, A-03, A-04, A-05, A-06, A-07, A-08, A-09, A-10, A-11, A-12, A-13, A-14, A-15, A-16, A-18, A-19, A-21, A-22, A-23, A-24, A-25, A-27, A-28, A-29, A-30, A-31, A-32, A-33, A-34, A-36, A-37, A-38, A-39, A-40, A-41, A-42, A-43, A-45, A-46, A-47, A-48, A-50 |
| B — tc358743 Linux driver | B-02, B-07, B-09, B-10, B-11, B-12, B-13, B-14, B-15, B-16, B-18, B-19, B-20, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-28, B-31, B-32, B-34, B-36, B-37, B-38, B-39, B-40, B-41, B-42, B-43, B-44, B-45, B-46, B-47, B-49, B-50 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-07, C-08, C-09, C-10, C-11, C-12, C-13, C-16, C-17, C-21, C-22, C-23, C-24, C-25, C-26, C-27, C-28, C-30, C-31, C-32, C-33, C-34, C-35, C-36, C-37, C-38, C-39, C-40, C-41, C-42, C-43, C-44, C-45, C-46, C-47, C-48, C-49, C-50, C-51, C-52, C-53 |
| D — Encoders | D-03, D-06, D-07, D-10, D-11, D-12, D-13, D-14, D-16, D-17, D-18, D-19, D-20, D-21, D-22, D-23, D-24, D-25, D-27, D-28, D-29, D-30, D-31, D-32, D-34, D-35, D-36, D-37, D-38, D-39, D-40, D-41, D-42, D-43, D-44, D-45, D-46, D-47, D-48, D-50, D-51, D-52, D-53, D-54 |
| E — Buildroot and kernel configuration | E-01, E-03, E-05, E-06, E-09, E-10, E-11, E-12, E-13, E-14, E-15, E-17, E-19, E-20, E-31, E-32, E-33, E-34, E-37, E-39, E-40, E-41, E-43, E-45, E-47, E-48, E-49, E-50, E-51, E-53 |
| F — ATEM and streaming | F-01, F-02, F-04, F-06, F-07, F-09, F-10, F-11, F-13, F-14, F-15, F-16, F-17, F-18, F-19, F-20, F-21, F-22, F-23, F-24, F-25, F-26, F-27, F-28, F-29, F-30, F-31, F-32, F-33, F-34, F-35, F-36, F-37, F-38, F-39, F-40, F-41, F-42, F-43, F-44, F-45, F-46 |
| G — Raspberry Pi OS and image tooling | G-01, G-03, G-04, G-05, G-06, G-07, G-08, G-09, G-10, G-12, G-13, G-14, G-15, G-16, G-17, G-18, G-19, G-20, G-21, G-22, G-26, G-27, G-28, G-29, G-31, G-32, G-33, G-34, G-37, G-39, G-40, G-41, G-42, G-43, G-44, G-45, G-46, G-47, G-48, G-49, G-50, G-51, G-52, G-53, G-54, G-55, G-56, G-59, G-60, G-61, G-63, G-64, G-67, G-68, G-69, G-70, G-71 |

- `CORRECTED` entries cited, used in their corrected wording only: A-22, A-25, B-11, B-21, B-25, B-44, C-28, C-36, C-39, C-53, D-46, E-40, E-47, F-24, F-28, F-34, F-37, G-43, G-51, G-54, G-55, G-69, G-71.
- `community` tier entries cited, worded as reports: A-43, C-28, C-33, C-35, C-41, C-42, C-43, C-45, D-17, D-18, D-50, D-54, F-11, F-13, F-14, F-15, F-16, F-17, F-18, F-19, F-20, F-21, F-22, F-44, F-45.
- `reasoning` tier entries cited, labelled as reasoning: A-23, B-10, B-11, B-27, B-47, B-49, C-46, C-47, C-48, C-49, C-50, C-51, C-52, C-53, D-29, D-36, D-52, F-35, F-38, F-40, F-46, G-71.
- Statements marked *research gap* or *research open question* come from the research JSON and are not register facts; they are recorded only to state what is unknown.
- "Verified from sources" means only that the cited source says so (see the note in [REFERENCES.md](REFERENCES.md)). Under Rule 23, a hardware measurement overrides any of these facts.

## Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No open question has been answered by a test; 98 of 103 entries are `OPEN`; OQ-001, OQ-004, OQ-009, OQ-012 and OQ-102 were ANSWERED by owner statements on 2026-10-07, not by tests.

# Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Added OQ-090 to OQ-101 for gaps found by the per-document reviews (software design decisions, operator interface, configuration/logging, EDID provisioning trigger, update policy, Unicam error counters, GPU overclock, driver source diff, Ethernet/USB/storage facts, link frequency / ADR-008, config.txt syntax, unattested command syntax). Summary table, header count and Appendix B updated. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: OQ-002 "EDID RAM is volatile, no default [B-22]" replaced by what [B-21] and [A-31] support plus a topic B research-gap label ([B-22] kept only for the 8-block limit); OQ-003 RISK-016 described as a pixel-format label mismatch, not a byte-order problem; OQ-004, OQ-008, OQ-063, OQ-079 AAC wording aligned with [F-41] ("not a *required* WebRTC codec", transcode typically to Opus); OQ-035 1080p50 UYVY 2-lane load-class reasoning added (CSI_PIPELINE.md §10.6) with link to ADR-008; OQ-038 and OQ-050 linked to ADR-008 / OQ-099 (a 4-lane port is necessary but not shown sufficient); OQ-050 notes the CFE counter method is unknown and links OQ-095; OQ-041 HD-source SMPTE170M statement attributed to the topic B research gap, not [B-34]; OQ-056 "Cortex-A72" removed (not a register fact) and the overclock owner decision linked to OQ-096; OQ-059 "quad-core" removed and [D-50] split by engineer (jamesh: no H.264 block; 6by9: 1080p60 "easily achievable", community); OQ-069 option no longer labelled "with tryboot"; OQ-075 MediaMTX :8189 given as `webrtcLocalUDPAddress` with ICE labelled as reasoning; OQ-083 audio wording ("configures 2-channel I2S output", silicon can also use CSI-2 [A-05]); OQ-096 and OQ-099 reasoning-tier labels for [D-52] and [C-52]; OQ-101 community labels for [D-17] and [C-33]; Summary category labels of OQ-095 and OQ-098 aligned; Appendix B: RISKS row aligned with each risk's Open questions line (adds OQ-095 to RISK-006/RISK-011, OQ-099 to RISK-001/RISK-006/RISK-011, OQ-096 to RISK-002, OQ-093 to RISK-010, plus previously missing 009, 024, 033, 038, 050, 051, 052, 074, 088), REQ-CAP-003 → 093, ADR-003 → 093, ADR-006 → 100, 101; Verification status fact list completed (A-46, G-12, G-47; 301 distinct facts). No question added, removed, renumbered or re-statused. Final verification pass (same date): the numbering note in the introduction no longer names an undefined ID ("next free number after OQ-101"); dated scope notes added under OQ-100 (trailing comments in `config.txt`, linked from DEVICE_TREE.md §6.0) and OQ-101 (package-list export, kernel-config read-out, encoder-node enumeration, CPU sampling, SoC temperature/throttling read-out, linked from BUILD_SYSTEM.md, VIDEO_ENCODER.md and PERFORMANCE.md); question texts and statuses unchanged. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner answers recorded: OQ-001 and OQ-009 ANSWERED (owner statements of 2026-10-07, interpretations recorded); owner input added to OQ-002, OQ-011, OQ-012, OQ-021; OQ-102 added (ATEM and camera models). | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated (verification pass): OQ-011 conditional "if OQ-001 makes 1080p60 mandatory, Pi 4 Model B is excluded" annotated (excluded from the 4-lane configuration only; 2-lane candidate under REQ-CAP-007); OQ-091 annotated (ATEM-driven control not in current scope); OQ-102 "must be switched to program" replaced by what [F-25] states (defaults to multiview; source selectable), and [B-27] labelled as reasoning; dated scope notes added under OQ-075, OQ-077, OQ-078, OQ-079, OQ-080, OQ-081, OQ-082, OQ-084, OQ-089 (not in current scope, or only partly; kept as reference) and OQ-083 (in scope); Verification status wording "No open question has been answered" corrected to "…answered by a test". Original question texts kept; no OQ added, removed, renumbered or re-statused. | Claude (session 2026-10-07) |
| 2026-10-07 | OQ-012 ANSWERED: owner accepted ADR-003 ("accept ADR-003"). Summary row and counts updated. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); Appendix B decisions row label now reads "OPEN / PROPOSED, and ADR-003 (ACCEPTED 2026-10-07)" and its ADR-003 → 012 mapping marks OQ-012 ANSWERED; the ADR-003 follow-on questions (OQ-064 to OQ-068, OQ-071, OQ-093) stay OPEN. The OQ-012 entry, its summary row and the counts were updated earlier and are unchanged here. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner answers recorded: OQ-004 ANSWERED (audio required), OQ-102 ANSWERED (any HDMI camera, no model list); owner input added to OQ-005 (H.264 + H.265) and OQ-011 (evaluate CM4 and CM5 side by side); OQ-103 added (H.265 scope per output). Summary and counts updated. | Claude (session 2026-10-07) |
