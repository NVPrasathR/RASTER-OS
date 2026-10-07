# PACSCORDER Technical Risk Register

| | |
|---|---|
| Document status | Active — 22 risks, all OPEN |
| Last updated | 2026-10-07 |
| Applies to | PACSCORDER on all four candidate platforms: Pi 4 Model B, CM4, Pi 5, CM5; both the 2-lane and the 4-lane capture configuration (REQ-CAP-007) |
| Verification | Source research only. No risk has been confirmed or retired by a test; nothing has been tested on PACSCORDER hardware. |
| Basis | Source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)) and the owner decisions of 2026-10-07 recorded in [REQUIREMENTS.md](REQUIREMENTS.md) (REQ-CAP-007, REQ-CAP-008, REQ-BLD-002; REQ-ATEM-001 scope). **No risk below has been confirmed or retired on PACSCORDER hardware** — none exists yet. |

This register lists facts, found in sources, that could stop PACSCORDER meeting a requirement. Each entry names:

- its evidence (fact IDs in [REFERENCES.md](REFERENCES.md));
- the requirements it affects ([REQUIREMENTS.md](REQUIREMENTS.md));
- how it will be retired.

A risk is retired only by test evidence (Rule 10). It is never deleted; a retired risk is marked `RETIRED` with the test ID and date.

**Severity** is Claude's assessment from the evidence (Rule 23 priority 8: reasoning). It is a starting point for owner review, not a measurement.

| ID | Risk | Affects | Severity | Status |
|---|---|---|---|---|
| RISK-001 | 1080p60 needs a 4-lane CSI-2 link; 2-lane configurations (Pi 4 Model B, CM4 CAM0, 2-lane boards) cannot carry it | REQ-CAP-001, REQ-CAP-007, ADR-004 | High | OPEN |
| RISK-002 | 1080p60 hardware H.264 encode on Pi 4/CM4 is unproven | REQ-ENC-001, ADR-004 | High | OPEN |
| RISK-003 | Pi 5/CM5 have no hardware video encoder | REQ-ENC-001, REQ-STR-001/002 | High | OPEN |
| RISK-004 | TC358743 supply / lifecycle uncertain | Product | High | OPEN |
| RISK-005 | No public TC358743 register documentation | REQ-DRV-001 | Medium | OPEN |
| RISK-006 | FIFO-level image corruption near lane-count limits | REQ-CAP-001, REQ-CAP-005, REQ-CAP-007 | Medium | OPEN |
| RISK-007 | Wrong reference-clock value in DT causes a kernel BUG | REQ-DRV-001 | Medium | OPEN |
| RISK-008 | HDCP-protected sources produce no video | REQ-CAP-001, REQ-CAP-008, REQ-ATEM-001 | Medium | OPEN |
| RISK-009 | Interlaced inputs (e.g. 1080i) are not supported | REQ-CAP-005, REQ-CAP-008 | Medium | OPEN |
| RISK-010 | No video until userspace loads an EDID | REQ-CAP-003 | High (if unmitigated) | OPEN |
| RISK-011 | Pi 5 receiver D-PHY rate fixed at 999 Mbps for this bridge | REQ-CAP-001 on Pi 5/CM5 | Medium | OPEN |
| RISK-012 | Pi 5/CM5 TC358743 path has no official documentation | REQ-PLT-001, ADR-004 | Medium | OPEN |
| RISK-013 | Signal changes detected only by 1 s polling | REQ-CAP-004 | Low | OPEN |
| RISK-014 | HDMI audio needs a separate I2S path with the stock driver | REQ-CAP-006 | Medium | OPEN |
| RISK-015 | GPL and patent licensing of software H.264 encoding | REQ-ENC-001, product release | Medium | OPEN |
| RISK-016 | RGB888 pixel-format label differs between receivers | ADR-005 | Low | OPEN |
| RISK-017 | Raspberry Pi OS image reproducibility needs an archive mirror | REQ-BLD-001, REQ-BLD-002, ADR-003 | Medium | OPEN |
| RISK-018 | ATEM control protocol is reverse-engineered | REQ-ATEM-001 — only if network tally/control is added; not in current scope (owner, 2026-10-07) | Medium | OPEN |
| RISK-019 | WebRTC browser profile/level and audio codec constraints | REQ-STR-002 | Medium | OPEN |
| RISK-020 | Contiguous memory (CMA) sizing for capture buffers | REQ-CAP-001, REQ-DMA-001 | Low | OPEN |
| RISK-021 | Third-party bridge-board wiring hazards | Hardware | Medium | OPEN |
| RISK-022 | H.265 is required but software-only on every candidate | REQ-ENC-001, REQ-STR-001, REQ-STR-002, REQ-REC-001 | High | OPEN |

---

## RISK-001 — 1080p60 needs a 4-lane CSI-2 link

- **Evidence:**
  - Reasoning from the driver's lane formula (reasoning-tier entries; A-26 CORRECTED): at the overlay default of 972 Mbps per lane, the driver needs 3 lanes for 1080p60 UYVY and 4 for RGB888 [A-26], [C-47].
  - Official Raspberry Pi documentation: 2 lanes give at most 1080p30 RGB888 / 1080p50 YUV422; 4 lanes on a Compute Module give 1080p60 [C-37].
  - The Pi 4 Model B connector has 2 lanes [C-01]; CM4 CAM0 has 2 lanes [C-02].
- **Impact:** REQ-CAP-001 cannot be met on Pi 4 Model B, on CM4 CAM0 or on any 2-lane bridge board. Since the owner decision of 2026-10-07 (OQ-001 ANSWERED; REQ-CAP-007, DRAFT), the product needs both a 2-lane and a 4-lane configuration, and 1080p60 is required on 4-lane configurations only. 2-lane configurations remain in scope and are bounded by the physical limit: for 1920x1080, at most 1080p50 UYVY or 1080p30 RGB888 [C-37], [C-48] (C-48 is reasoning). Pi 4 Model B is therefore not excluded from the product, only from 1080p60; it remains a 2-lane candidate (ADR-004, OPEN). On the 4-lane configuration, a 4-lane port is necessary but not shown sufficient: at the default 972 Mbps, 1080p60 UYVY uses 3 of the 4 lanes, which is unproven (OQ-038; RISK-006). ADR-008 (PROPOSED; OQ-099) proposes evaluating 297 MHz on a CM4 CAM1 4-lane link, where the mode uses all 4 lanes.
- **Open questions:** OQ-001 (ANSWERED 2026-10-07 by the owner), OQ-002 (mode list per lane configuration), OQ-011, OQ-021, OQ-038, OQ-099.
- **Retire by:** ADR-004 platform choice for the 4-lane configuration (a per-configuration choice since 2026-10-07; still OPEN), then TEST-CAP-002 on that 4-lane platform. For the 2-lane configuration, TEST-CAP-004 records the supported-mode matrix up to the physical limit (REQ-CAP-007).

## RISK-002 — 1080p60 hardware encode on Pi 4/CM4 is unproven

- **Evidence:**
  - The official BCM2711 specification is H.264 1080p30 encode [D-10].
  - The driver calls level 4.0 the hardware spec [D-12].
  - Reasoning: 1080p60 needs about twice the specified macroblock rate [D-52].
  - A Raspberry Pi engineer called hardware 1080p60 an "edge case" (community source) [D-50].
- **Impact:** A CM4 product may capture 1080p60 but be unable to encode it in real time. 1080p60 capture is required on the 4-lane configuration (REQ-CAP-007, owner 2026-10-07), and CM4 CAM1 is a 4-lane candidate (ADR-004, OPEN). Whether the captured 1080p60 must also be encoded at 60 fps is not specified in REQ-ENC-001 (encoding parameters: OQ-005).
- **Open questions:** OQ-056 (measurement, including at `gpu_freq=550`); OQ-096 (owner decision: whether a GPU overclock is acceptable in the product).
- **Retire by:** TEST-ENC-001 (sustained 1080p60 encode on CM4). The "at least 10 minutes" duration used in [TESTING.md](TESTING.md) and [PERFORMANCE.md](PERFORMANCE.md) comes from a research open question (topic D); it is not an owner-accepted criterion (OQ-010, OQ-017).

## RISK-003 — No hardware video encoder on Pi 5/CM5

- **Evidence:**
  - Official: "Raspberry Pi 5 uses software video encoders" [G-22].
  - BCM2712: "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22].
  - The product briefs list no encoder [D-31].
- **Impact:** CPU and thermal load at 1080p60, especially with recording, RTMP and WebRTC running at the same time. UYVY must also be converted to a planar format per frame [D-43]. Pi 5 and CM5 are 4-lane candidates (ADR-004, OPEN), where 1080p60 capture is required (REQ-CAP-007, owner 2026-10-07).
- **Open questions:** OQ-059, OQ-060.
- **Retire by:** TEST-ENC-001 and TEST-PERF-001 on Pi 5/CM5.

## RISK-004 — TC358743 supply / lifecycle uncertain

- **Evidence:**
  - Research recorded that a Raspberry Pi engineer said, in the community issue thread behind [C-43], that he believed the part is end-of-life (research gap, topic C — not a register fact).
  - Research also found Toshiba's product page showing the part in mass production on 2026-10-06, with no EOL flag (research gap, topic A — not a register fact). The current public datasheet is Rev. 2.20 of 2026-05-11 [A-01].
- **Impact:** Product BOM continuity.
- **Open questions:** OQ-085, OQ-034.
- **Retire by:** Written lifecycle confirmation from Toshiba sales (VENDOR CONFIRMATION REQUIRED).

## RISK-005 — No public register documentation

- **Evidence:**
  - Only a 20-page summary datasheet is public, with no register map, I2C address or AC timing [A-42].
  - The driver relies on NDA documents [A-42].
  - The FIFO level of 374 is hard-coded in the driver [B-12]; a Raspberry Pi engineer reported that it is empirical (community source) [A-43].
- **Impact:** Debugging below the driver, and any new driver work, depend on NDA material.
- **Open questions:** OQ-027.
- **Retire by:** Obtaining the Toshiba Functional Specification under NDA, or accepting the risk in an ADR.

## RISK-006 — FIFO-level image corruption near lane limits

- **Evidence:**
  - An open community issue reports corrupted images at 1080p50 RGB888 on a 4-lane CM4 when the driver chose 3 lanes [C-43].
  - The FIFO level is hard-coded [B-12].
  - Reasoning (inputs [C-47], [C-48]): the per-active-lane load of that reported case (1080p50 RGB888 on 3 lanes, 128 % of 2 lanes = 85.3 % of 3 lanes at 972 Mbps) equals that of 1080p50 UYVY on 2 lanes (85.3 %), which official documentation lists as supported [C-37]. 1080p60 UYVY on 4-lane ports also runs on 3 of 4 lanes [C-47]. See [CSI_PIPELINE.md](CSI_PIPELINE.md) §10.6.
- **Impact:** Some input modes may stream visibly corrupted video. The scope includes 1080p50 UYVY on 2 lanes and 1080p60 UYVY on 3 of 4 lanes, not only RGB888. Under REQ-CAP-007 (owner, 2026-10-07) both of these are required modes: 1080p50 UYVY is within the 2-lane physical limit [C-37], [C-48] (C-48 is reasoning), and 1080p60 is required on the 4-lane configuration.
- **Open questions:** OQ-035, OQ-038, OQ-095, OQ-099.
- **Retire by:** TEST-CAP-004 across the supported-mode matrix, including 1080p50 UYVY on 2 lanes and 1080p60 UYVY on 4-lane ports. Prefer UYVY (ADR-005), which lowers the lane count per mode but does not by itself remove this risk. ADR-008 (PROPOSED; OQ-099) proposes keeping the default 486 MHz link frequency and evaluating 297 MHz on a CM4 CAM1 4-lane link, which avoids the 3-of-4-lane case for 1080p60 UYVY. How to read CSI-2 error counters is not in the source register for either receiver (OQ-050 for CFE, OQ-095 for Unicam).

## RISK-007 — Wrong reference clock causes a kernel BUG

- **Evidence:** An unsupported `refclk` rate (anything other than 26, 27 or 42 MHz) is not rejected cleanly. Probe continues and hits `BUG_ON()` in `tc358743_set_ref_clk()` (kernel source [A-22] and reasoning from it [B-11], both CORRECTED).
- **Impact:** A Device Tree mistake crashes the kernel instead of failing the probe.
- **Open questions:** OQ-019, OQ-013.
- **Retire by:** Matching the DT `clock-frequency` to the board oscillator. 27 MHz is preferred because, by reasoning from the driver's integer PLL arithmetic, only 27 MHz gives the exact 594/972 Mbps lane rates [A-23], [B-10]. Optionally patch the driver (ADR-002).

## RISK-008 — HDCP-protected sources produce no video

- **Evidence:**
  - The driver disables HDCP on DT platforms [A-04].
  - The HDCP version and key provisioning of the chip are undocumented publicly [A-03].
- **Impact:** Sources that enforce HDCP will not be capturable. Since the owner decision of 2026-10-07 (REQ-CAP-008, DRAFT), the sources are ATEM switcher HDMI outputs and cameras connected directly. Whether any ATEM model or camera to be supported asserts HDCP on its HDMI output is not in the source register: UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED; OQ-083 for the ATEM, OQ-102 for the model list and each camera's behaviour).
- **Open questions:** OQ-028, OQ-083, OQ-102.
- **Retire by:** TEST-CAP-001 with each source on the REQ-CAP-008 list (models: OQ-102), including the ATEM models (ATEM HDCP behaviour: OQ-083) and the cameras.

## RISK-009 — Interlaced inputs not supported

- **Evidence:**
  - Reasoning from driver source: the driver's DV-timings capability is progressive-only, so interlaced modes return `-ERANGE` [B-27].
  - ATEM Mini Pro HDMI output is 1080p only [F-23], so this does not affect capture from an ATEM Mini Pro. The output standards of other ATEM models are not in the source register (OQ-102).
  - Cameras connected directly are in scope since the owner decision of 2026-10-07 (REQ-CAP-008, DRAFT). Their HDMI output modes, including whether any of them outputs interlaced video, are not in the source register: UNKNOWN — VERIFICATION REQUIRED per camera model (OWNER DECISION REQUIRED for the model list, HARDWARE TEST REQUIRED; OQ-102). This risk register does not assert that cameras output interlaced video.
- **Impact:** 1080i sources cannot be captured. Reasoning from [B-27]: a source on the REQ-CAP-008 list that sends only interlaced video could not be captured with the stock driver; whether any listed source does so is unknown (OQ-102).
- **Open questions:** OQ-002, OQ-102.
- **Retire by:** Documenting the supported-mode list (REQ-CAP-005) and an EDID that does not advertise interlaced modes, then TEST-CAP-001 and TEST-CAP-004 with each ATEM model and camera on the REQ-CAP-008 list (OQ-102).

## RISK-010 — No video until an EDID is loaded

- **Evidence:**
  - Hot-plug is never asserted until an EDID has been written [A-33], [B-21] (CORRECTED).
  - After every probe the driver has no EDID stored (`edid_blocks_written == 0`) [B-21]; the EDID is held in the chip's embedded SRAM [A-31]. Research also found no default EDID and no DT property for one (research gap, topic B — not a register fact).
- **Impact:** After every boot or module reload, sources see no display until the EDID is provisioned.
- **Open questions:** OQ-002, OQ-032, OQ-093.
- **Retire by:** Implementing REQ-CAP-003 (provisioning trigger and ordering: OQ-093), tested by TEST-DRV-002.

## RISK-011 — Pi 5 receiver D-PHY rate fixed at 999 Mbps

- **Evidence:** The CFE driver falls back to 999 Mbps when it cannot determine a sensor link rate [C-31]. Reasoning from driver source: it cannot read one from this bridge, so it always programs 999 Mbps [B-49]. Reasoning: that matches the default 972 Mbps link but not `link-frequency=297000000` (594 Mbps) [C-52].
- **Impact:** Possible unreliable reception at the non-default link frequency on Pi 5/CM5.
- **Open questions:** OQ-050, OQ-095, OQ-099.
- **Retire by:** Using the default link frequency on Pi 5/CM5 (ADR-008, PROPOSED; OQ-099), plus a hardware test of stream stability and CSI-2 error counters. How to read those counters on CFE is not in the source register: UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED; OQ-050). The Unicam counterpart, needed to compare against Pi 4/CM4 on the same modes, is OQ-095.

## RISK-012 — Pi 5/CM5 path has no official documentation

- **Evidence:**
  - Official TC358743 documentation covers only the Unicam (Pi 4 family) path [C-38].
  - A Raspberry Pi engineer reported a Pi 5 capture sequence that he said functioned on kernel 6.18.39 (community source) [C-33].
  - An open issue reports that source-change events are not delivered to the CFE video node (community source) [C-42].
- **Impact:** Pi 5/CM5 bring-up relies on community procedures.
- **Open questions:** OQ-049, OQ-050, OQ-051, OQ-052.
- **Retire by:** TEST-PLT-001 and TEST-CAP-001/002/003 on Pi 5 or CM5.

## RISK-013 — Signal changes detected only by polling

- **Evidence:**
  - The stock overlays wire no interrupt, so the driver polls every 1000 ms [A-30], [B-19], [C-20].
  - The INT pin is active-high, level-triggered [A-29].
- **Impact:** Up to about 1 s latency to detect connect, disconnect or resolution change.
- **Open questions:** OQ-020, OQ-051.
- **Retire by:** A hardware design that wires INT to a GPIO, plus an overlay `interrupts` property, or acceptance of the latency in REQ-CAP-004.

## RISK-014 — HDMI audio needs a separate I2S path with the stock driver

- **Evidence:**
  - The silicon can send audio over CSI-2 [A-05] or on I2S/TDM output pins [A-11].
  - The Linux driver always configures 2-channel I2S [A-13]; whether audio over CSI-2 works with the Raspberry Pi receivers is unknown (OQ-033).
  - The `tc358743-audio` overlay expects I2S on GPIO 18/19/20 [A-47], [G-14]. [A-47] is marked as applying to Pi 4/CM4; whether the overlay works on Pi 5/CM5 is unverified (research gaps, topics B and C — not register facts; OQ-054).
- **Impact:** With the stock driver, audio needs extra wiring that the CSI-2 camera cable does not carry (reasoning). The audio pins are outputs in the VDDIO2 domain, which is 1.8 V or 3.3 V depending on the board [A-12]; research noted that VDDIO2 must be 3.3 V to interface with Pi GPIO 18/19/20 (research gap, topic A — not a register fact; OQ-024, OQ-025).
- **Open questions:** OQ-004, OQ-025, OQ-033, OQ-054.
- **Owner decision (2026-10-07):** audio is required (REQ-CAP-006 DRAFT, OQ-004 ANSWERED), so this risk now affects a firm requirement.
- **Retire by:** An owner decision on REQ-CAP-006 (OQ-004), then TEST-AUD-001.

## RISK-015 — GPL and patent licensing of software encoding

- **Evidence:**
  - x264 is GPL; FFmpeg must be built `--enable-gpl` to use it [D-42], [D-46], [D-47].
  - The GPU firmware licence is proprietary [G-69].
  - H.264 patent licensing was not researched (OQ-086).
- **Impact:** Product licensing and source-offer obligations, especially on Pi 5/CM5, where software encoding is mandatory.
- **Open questions:** OQ-086, OQ-087, OQ-088.
- **Retire by:** Legal review (LEGAL CLARIFICATION REQUIRED).

## RISK-016 — RGB888 pixel-format label differs between receivers

- **Evidence:**
  - Pi 5 CFE maps `RGB888_1X24` to `BGR24`; Pi 4/CM4 Unicam maps it to `RGB24` [C-34].
  - On CM4 the bytes in memory were reported as B, G, R, so Unicam's `RGB24` label does not match the memory order (community source) [C-35]. Research found the memory order is B, G, R on both receivers (research gap, topic C — not a register fact). What differs is the label, not the bytes.
- **Impact:** Colour-swapped video if RGB888 is used and software trusts the label.
- **Open questions:** OQ-045, OQ-003.
- **Retire by:** ADR-005 (use UYVY). If RGB888 is ever used, a byte-dump test.

## RISK-017 — Reproducibility of Raspberry Pi OS based images

- **Evidence:**
  - `apt full-upgrade` updates kernel and firmware [G-08].
  - The default kernel series changed mid-release, from 6.12 to 6.18 [G-05].
  - No snapshot service for `archive.raspberrypi.com` was found (research gap, topic G — not a register fact; OQ-067).
- **Impact:** Two builds of the same configuration could differ. Since the owner decision of 2026-10-07 the product runs a project-built OS image (REQ-BLD-002, DRAFT). Under ADR-003 (ACCEPTED 2026-10-07) that image is built with `rpi-image-gen` from Raspberry Pi OS and Debian packages, so this risk applies to the product image itself.
- **Open questions:** OQ-067, OQ-070; OQ-012 (acceptance of ADR-003, ANSWERED 2026-10-07: the owner accepted ADR-003).
- **Retire by:** A package mirror plus pinned versions in the image configuration (ADR-003), and TEST-BLD-001.

## RISK-018 — ATEM control protocol is reverse-engineered

- **Evidence:**
  - The official SDK supports Windows and macOS only [F-01].
  - The official SDK manual does not document the UDP 9910 protocol [F-10]; the OpenSwitcher project reports it as reverse-engineered (community source) [F-11].
  - The `atem-connection` source is reported to define protocol versions only up to 9.6, while ATEM software 10.x exists (community source) [F-18].
- **Impact:** Tally and record-state integration may break on ATEM firmware updates.
- **Scope note (owner decision of 2026-10-07; OQ-009 ANSWERED):** REQ-ATEM-001 now covers HDMI capture of the ATEM output only (REQ-CAP-008). Network tally/control over UDP 9910 was offered and not selected, so it is not in current scope unless the owner adds it. This risk therefore applies only if that integration is added, and its relevance is reduced. It stays OPEN and is not retired: a risk is retired only by test evidence. The evidence above is kept as reference, not in current scope.
- **Open questions:** OQ-009 (ANSWERED 2026-10-07: network integration not selected), OQ-077, OQ-084, OQ-089 — relevant only if network integration is added.
- **Retire by:** If the owner adds network tally/control to REQ-ATEM-001: TEST-ATEM-001 against current ATEM firmware. While it stays out of scope, the risk remains OPEN with reduced relevance.

## RISK-019 — WebRTC profile/level and audio constraints

- **Evidence:**
  - Reasoning: the common `42e01f` decodes as Constrained Baseline Level 3.1 [F-38], which cannot describe 1080p; 1080p needs Level 4.0 or above, 1080p60 Level 4.2 [F-40].
  - The MediaMTX project reports that browsers do not accept H.264 B-frames in WebRTC (community source) [F-45].
  - RFC 7874 requires WebRTC endpoints to implement Opus and G.711; AAC is not a required WebRTC codec, so AAC audio has to be transcoded (typically to Opus) for browser playback [F-41].
- **Impact:** 1080p WebRTC needs correct level signalling and an Opus audio path.
- **Open questions:** OQ-008, OQ-073, OQ-074.
- **Retire by:** TEST-STR-002 in Chrome, Firefox and Safari.

## RISK-020 — CMA sizing for capture buffers

- **Evidence:**
  - Reasoning: four 1080p UYVY buffers take about 16.6 MB [C-53] (CORRECTED verdict).
  - The default CMA pool from the Device Tree is 64 MB [E-47] (CORRECTED verdict).
  - Whether Pi 5 CFE buffers use CMA (an IOMMU is present) is unresolved (reasoning from kernel source, CORRECTED) [C-53] (OQ-053).
- **Impact:** Allocation failures at STREAMON if CMA is exhausted.
- **Open questions:** OQ-053, OQ-061.
- **Retire by:** Measuring `CmaFree` while streaming (TEST-PERF-001).

## RISK-021 — Third-party bridge-board wiring hazards

- **Evidence:**
  - A Raspberry Pi engineer reported that a wrongly sided 22-to-15-pin adapter on Pi 5 swaps GND and 3V3 and can damage hardware (community source) [C-45].
  - The board must supply its own reference clock: the Pi-side clock node is a `fixed-clock` that only declares the frequency [A-45]; reasoning from the DT: the Pi does not generate it [B-47]. The driver accepts only 26, 27 or 42 MHz [B-07], the datasheet values [A-21]; any other value leads to a kernel BUG (RISK-007) [A-22] (CORRECTED).
- **Impact:** Hardware damage, or a kernel BUG (RISK-007).
- **Open questions:** OQ-018, OQ-019, OQ-021 (including whether one bridge-board design can serve both the 2-lane and the 4-lane configuration required by REQ-CAP-007), OQ-022, OQ-024.
- **Retire by:** Recording the chosen board's schematic facts in [HARDWARE.md](HARDWARE.md) before first power-up.

---

## RISK-022 — H.265 is required but software-only on every candidate

- **Added:** 2026-10-07, after the owner chose H.264 + H.265 (REQ-ENC-001, OQ-005).
- **Evidence:**
  - No candidate platform has a hardware HEVC encoder [D-24], [D-31].
  - The only official software-encode figure is H.264 1080p30 on Pi 5 at ~30–40% CPU [G-22]; no sourced H.265 figure exists (research topic H in progress, 2026-10-07).
  - Legacy RTMP/FLV carries only H.264 video; HEVC needs Enhanced RTMP [F-31]. GStreamer `eflvmux` appears in 1.28 [F-34]; Raspberry Pi OS ships 1.26.2 [G-26] (reasoning).
  - WebRTC endpoints must support only VP8 and H.264 [F-36].
- **Impact:** H.265 at the required rates may exceed CPU or thermal limits, especially alongside H.264 outputs; HEVC over RTMP and WebRTC may not reach common receivers.
- **Open questions:** OQ-103, OQ-005, OQ-059.
- **Retire by:** OQ-103 owner scoping, then TEST-ENC-001 and TEST-PERF-001 with H.265 runs on CM4 and CM5.

## Verification status

### Verified from sources (fact IDs)

Every **Evidence** item cites [REFERENCES.md](REFERENCES.md) entries with verdict `CONFIRMED` or `CORRECTED`. `CORRECTED` entries (A-22, A-26, B-11, B-21, C-53, D-46, E-47, G-69) are used in their corrected wording. Community-tier entries (A-43, C-35, C-42, C-43, C-45, D-50, F-11, F-18, F-45) and the community report [C-33] are worded as reports. Reasoning-tier entries (A-23, A-26, B-10, B-11, B-27, B-47, B-49, C-47, C-48, C-52, C-53, D-52, F-38, F-40) are labelled as reasoning. Statements marked *research gap* come from the research JSON and are not register facts. Severity is Claude's assessment, not a measurement.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No risk has been confirmed or retired by a test.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06: RISK-001 to RISK-021, all OPEN. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document review. No risk added, removed, re-scored or retired. Changes: vague references ("topic A gaps", "open question", "recorded as an open question") replaced by research-gap labels and OQ IDs, and an **Open questions** line added to each risk; RISK-004 evidence no longer attributes the product-page and EOL statements to [A-01]/[C-43] (both are research gaps, OQ-085); RISK-010 "EDID RAM is volatile [B-22]" replaced by what [B-21] and [A-31] support; RISK-012 "reported working" reworded (Rule 10); RISK-014 title and evidence corrected (the silicon can also send audio over CSI-2 [A-05]; I2S is the driver's choice [A-13]; 3.3 V is a research note, not [A-12]); RISK-016 title changed from "byte order" to "pixel-format label", per [C-34], [C-35]; RISK-006 scope extended by reasoning to 1080p50 UYVY on 2 lanes and 1080p60 UYVY on 3 of 4 lanes; RISK-019 AAC wording aligned with [F-41]; RISK-021 reference-clock rates re-cited to [B-07], [A-21]; RISK-002 "10 minutes" labelled as not owner-accepted; RISK-011 notes that the CSI-2 error-counter method is unknown; stale "check REFERENCES.md" hedge removed; community and reasoning facts labelled; header rows, Verification status and Change history added. Cross-document consistency fixes (second pass, same date; no risk added, removed, re-scored or retired): RISK-001 and RISK-006 linked to ADR-008 / OQ-099 (4-lane port necessary but not shown sufficient; 297 MHz evaluation on CM4 CAM1); RISK-006 and RISK-011 link the CSI-2 error-counter OQs (OQ-050 CFE, OQ-095 Unicam), and RISK-011 links ADR-008 / OQ-099 for the default link frequency; RISK-002 links OQ-096 (GPU overclock owner decision); RISK-010 links OQ-093 (EDID provisioning trigger); RISK-007 labels [B-11] and RISK-020 labels [C-53] as reasoning; RISK-019 adds "typically to Opus" per [F-41]. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: RISK-001 impact — 1080p60 required on 4-lane configurations only, 2-lane configurations bounded by the physical limit (1080p50 UYVY / 1080p30 RGB888), Pi 4 Model B still a 2-lane candidate (REQ-CAP-007; OQ-001 ANSWERED), retire-by per configuration, [C-02] added for CM4 CAM0; RISK-002 and RISK-003 note that 1080p60 capture is required on the 4-lane candidates (REQ-CAP-007); RISK-006 linked to REQ-CAP-007 (both corruption-prone cases are now required modes); RISK-008 and RISK-009 extended to direct camera sources (REQ-CAP-008, OQ-102; camera HDCP and interlace behaviour UNKNOWN, not asserted), RISK-009 ATEM statement narrowed to the ATEM Mini Pro; RISK-017 linked to REQ-BLD-002; RISK-018 scope note — network integration not in current scope, relevance reduced, not retired; RISK-021 links OQ-021 to REQ-CAP-007; header Applies-to and Basis rows updated. No risk added, removed, re-scored or retired. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); RISK-017 impact now says the product image is built with `rpi-image-gen` under ADR-003 (ACCEPTED 2026-10-07), and its OQ-012 reference is recorded as ANSWERED. RISK-017 stays OPEN (Medium): a risk is retired only by test evidence (TEST-BLD-001, NOT STARTED). No risk added, removed, re-scored or retired. | Claude (session 2026-10-07) |
| 2026-10-07 | RISK-022 added (H.265 software-only on every candidate; owner chose H.264 + H.265). RISK-014 notes that audio is now required (OQ-004 answered). | Claude (session 2026-10-07) |
