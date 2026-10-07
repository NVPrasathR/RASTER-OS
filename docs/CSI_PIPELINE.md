# CSI-2 Pipeline

| | |
|---|---|
| Document status | DRAFT — derived from source research only |
| Last updated | 2026-10-07 |
| Applies to | TC358743 CSI-2 transmitter; Raspberry Pi 4 Model B, CM4 (CAM0 and CAM1), Pi 5, CM5; both the 2-lane and the 4-lane configuration the product needs (REQ-CAP-007); kernel sources as read on `rpi-6.18.y` [D-01], [E-37] |
| Implementation status | NOT STARTED |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)). **Nothing in this document has been run or measured on PACSCORDER hardware. No hardware exists as of 2026-10-06.** |

This document describes the MIPI CSI-2 link from the TC358743 to the Raspberry Pi CSI-2 receiver, end to end:

- the transmitter limits;
- how the link rate and the lane count are chosen;
- the two receiver families (Unicam on Pi 4/CM4, RP1 CFE on Pi 5/CM5);
- the data formats on the link;
- the bandwidth arithmetic for every 1080p mode;
- what each candidate platform can carry;
- the 2-lane and 4-lane configurations the product needs (REQ-CAP-007) and what each can carry (§11.3);
- the known defects and open questions.

Related documents: [TC358743_DRIVER.md](TC358743_DRIVER.md) (driver internals), [DEVICE_TREE.md](DEVICE_TREE.md) (overlay and endpoint properties), [V4L2.md](V4L2.md) (Media Controller and ioctl sequence), [DMA.md](DMA.md) (what happens to the frames after the receiver), [HARDWARE.md](HARDWARE.md) (the actual board), [TESTING.md](TESTING.md) (procedures and results).

Conventions follow [README.md](README.md):

- `[C-37]` cites [REFERENCES.md](REFERENCES.md).
- Facts from `community` sources are worded as reports.
- Every calculation is marked **Reasoning** and names its inputs.
- `OQ-NNN` entries are in [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).

---

## 1. Summary for a new engineer

- **The driver, not the Device Tree, picks how many lanes carry video.** It computes the lane count at runtime from the active resolution, frame rate and bus format. It reports that count to the receiver without comparing it with the Device Tree `data-lanes` [C-15], [B-31].
- **The receiver checks the count at STREAMON.** If the bridge asks for more lanes than the receiver endpoint has, STREAMON fails with `-EINVAL`. Fewer lanes are accepted [C-16], [B-32].
- **The default link is 972 Mbps per lane** (`link-frequency` 486 MHz). The overlay declares a 27 MHz reference clock [C-12], [A-45].
- **1080p60 needs a 4-lane port at the supported link rates.** At the default rate 1080p60 needs 3 lanes in UYVY and 4 in RGB888 [C-47]. Of the four candidate platforms, only CM4 CAM1, Pi 5 and CM5 have 4-lane ports; Pi 4 Model B has a single 2-lane connector [C-01], [C-02], [C-04], [C-05]. A 4-lane port is necessary but not shown sufficient for 1080p60 UYVY: at 972 Mbps the driver uses 3 of the 4 lanes, which is unproven (OQ-038; §13; link frequency: ADR-008, OQ-099).
- **2-lane ports are limited.** Pi 4 Model B and CM4 CAM0 are limited to 1080p50 UYVY or 1080p30 RGB888 [C-37], [C-48].
- **The product needs both lane counts.** The owner requires a 2-lane and a 4-lane configuration, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; owner 2026-10-07, OQ-001 ANSWERED). Recorded interpretation: 1080p60 is required on the 4-lane configuration; the 2-lane configuration is capped at the 2-lane limit above. The platform for each configuration is not chosen (ADR-004, OPEN). See §11.3.
- **Pi 5/CM5 use a fallback D-PHY rate.** The downstream RP1 CFE driver cannot read a link rate from this bridge, so it always programs its D-PHY for 999 Mbps [C-31], [B-49].
- **Unresolved items that matter for REQ-CAP-001 and the 4-lane configuration of REQ-CAP-007:**
  - capture with 3 of 4 lanes active (OQ-038);
  - the hard-coded FIFO trigger level (OQ-035, RISK-006);
  - the clock-lane mode mismatch (OQ-037);
  - the effect of the 999 Mbps D-PHY setting (OQ-050, RISK-011).

---

## 2. End-to-end link

```text
HDMI source: ATEM switcher output or camera (REQ-CAP-008; models OQ-102)
  │  TMDS clock ≤ 165 MHz; input up to 1080p60                          [A-07]
  ▼
TC358743  (HDMI receiver → video FIFO → CSI-2 transmitter)
  │  REFCLK: 26, 27 or 42 MHz from an oscillator on the bridge board    [B-07], [B-47]
  │          PACSCORDER board value: UNKNOWN — VERIFICATION REQUIRED (OQ-019)
  │  PLL: lane rate = refclk / pll_prd × pll_fbd                         [B-08]
  │  FIFO trigger level: hard-coded 374                                  [B-12]
  ▼
D-PHY: clock lane (clock-lanes <0>) [C-12] + 1–4 data lanes; the driver activates 1–4 at runtime  [A-06], [A-25]
  │  required: a 2-lane and a 4-lane configuration (REQ-CAP-007; §11.3)
  │  connector, cable, lanes routed on PACSCORDER:
  │  UNKNOWN — VERIFICATION REQUIRED (OQ-021)
  ▼
CSI-2 receiver
  Pi 4 Model B / CM4 : Unicam  csi0 (2 lanes; not exposed on Pi 4B),
                               csi1 (4 lanes; 2 on Pi 4B)                     [C-07], [C-08]
  Pi 5 / CM5         : RP1 CFE csi0 / csi1, 4-lane D-PHY each                 [C-29], [C-30]
  ▼
Media Controller graph
  Pi 4/CM4 (MC mode) : "tc358743 …":0 ──(immutable, enabled)──► "unicam-image"        [C-36]
  Pi 5/CM5           : "tc358743 …" ──(immutable, enabled)──► "csi2" sink pad
                       "csi2":4 ──(disabled until userspace enables it)──► "rp1-cfe-csi2_ch0"  [C-32]
  ▼
V4L2 capture video node → frame buffers in memory  (continued in DMA.md)
```

Community reports show the bridge's media entity name following its I2C bus [C-28] (community, CORRECTED):

- On 6.18 kernels it was reported as "tc358743 10-000f" (CAM/DISP0) or "tc358743 11-000f" (CAM/DISP1) on Pi 5. Those bus numbers match the Pi 5 DT aliases: CAM/DISP0 is `/dev/i2c-10` and CAM/DISP1 is `/dev/i2c-11` [C-26].
- Earlier kernels numbered the buses differently. In 2023 a Raspberry Pi engineer reported the CAM/DISP buses as i2c-4 and i2c-6, and later forum posts showed the bridge as "tc358743 4-000f".

Do not hard-code the name (OQ-043).

---

## 3. Transmitter: TC358743 CSI-2 TX

| Property | Value | Source |
|---|---|---|
| Data lanes | Up to 4. The product brief says 1, 2, 3 or 4 are configurable. | [A-05], [A-06] |
| Lane rate | Up to 1 Gbps per data lane | [A-05], [A-06] |
| Interface standards cited | MIPI D-PHY v01-00-00 (2009); MIPI CSI-2 v1.01 | [A-06] |
| HDMI input | TMDS clock up to 165 MHz. Video up to 1080p60: RGB and YCbCr 4:4:4 at 24 bpp, YCbCr 4:2:2 at 24 bpp. | [A-07] |
| Content the silicon can send over CSI-2 | Video, audio and InfoFrame data | [A-05] |
| Audio path the Linux driver configures | 2-channel I2S, not CSI-2 | [A-13] |
| Register map, I2C address, AC timing | Not in the public 20-page summary datasheet. The driver was written against non-public Toshiba documents REF_01 (functional specification) and REF_02 (register-settings spreadsheet). | [A-42] |
| Lane-rate range the driver accepts | 62.5 Mbps to 1 Gbps. Timing tables exist only for 594 and 972 Mbps. | [A-24], [B-08], [B-09] |

The silicon's real D-PHY timing and lane-rate envelope: **UNKNOWN — VERIFICATION REQUIRED (DATASHEET REQUIRED; OQ-030, OQ-031, OQ-027).**

---

## 4. Link frequency, PLL and lane rate

### 4.1 From Device Tree value to lane rate

The driver uses only `link-frequencies[0]` from the endpoint [B-08]. The calculation runs as follows:

1. `bps_pr_lane = 2 × link_frequencies[0]`. It must be between 62,500,000 and 1,000,000,000 bps, or probe returns `-EINVAL` [B-08], [C-14].
2. `pll_prd = refclk_hz / 6,000,000` (integer). This gives 4, 4 and 7 for 26, 27 and 42 MHz [B-07].
3. `pll_fbd = bps_pr_lane / refclk_hz × pll_prd`. The division is integer, so it truncates [B-08].
4. Actual lane rate `= refclk_hz / pll_prd × pll_fbd` [B-08]. The runtime lane-count formula (§5) divides by this PLL-derived rate [A-25], [B-31].
5. D-PHY timing constants are chosen by `switch (bps_pr_lane)`:
   - only 594,000,000 and 972,000,000 have tables;
   - any other value logs `untested bps per lane` and uses the 594 Mbps constants [B-09], [A-24].

**Reasoning** (inputs [B-08], [B-09], [B-10]): the timing table is chosen from the nominal `bps_pr_lane`, while the PLL produces the truncated rate. With a 26 MHz or 42 MHz reference, the link therefore runs slightly below the rate its timing table was written for.

### 4.2 Reference clock versus lane rate

| REFCLK | `pll_prd` | `link-frequency` 297 MHz (nominal 594 Mbps) | `link-frequency` 486 MHz (nominal 972 Mbps) | Source |
|---|---|---|---|---|
| 26 MHz | 4 | `pll_fbd` 88 → 572 Mbps | `pll_fbd` 148 → 962 Mbps | [A-23], [B-10] |
| **27 MHz** | 4 | `pll_fbd` 88 → **594 Mbps** | `pll_fbd` 144 → **972 Mbps** | [A-23], [B-10] |
| 42 MHz | 7 | `pll_fbd` 98 → 588 Mbps | `pll_fbd` 161 → 966 Mbps | [A-23], [B-10]; the `pll_fbd` value 98 is computed here (**Reasoning**: 594 / 42 → 14 (truncated) × 7 = 98) |
| Any other | — | Logs `unsupported refclk rate`, but probe continues. If the chip answers the CHIPID read, `tc358743_set_ref_clk()` hits `BUG_ON()` (kernel BUG) instead of a clean probe failure | same | [A-22], [B-11] (both CORRECTED); RISK-007 |

Only 27 MHz gives the exact rates the timing tables were written for [A-23], [B-10]. The Raspberry Pi overlay declares 27 MHz on a `fixed-clock` node. The Pi does not generate this clock, so the bridge board must carry its own oscillator [A-45], [B-47]. **PACSCORDER bridge-board oscillator: UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED; OQ-019).**

### 4.3 Values the overlay supports

- Default `link-frequency` is 486000000 (972 Mbps per lane) [C-12], [A-45].
- The overlays README says only 297000000 and 486000000 are supported. It labels 297 MHz as "574Mbit/s", which contradicts the driver's 2 × 297 MHz = 594 Mbit/s [C-13], [A-46].
- Edge case: `link-frequency` 500 MHz gives `bps_pr_lane` = 1,000,000,000, which is inside the driver's range check [B-08], and 999 Mbps actual at 27 MHz. The lane formula would then return 2 lanes for 1080p60 UYVY. That rate is "untested", uses the 594 Mbps D-PHY timings and leaves almost no margin, so it is **not a supported configuration** [A-26] (CORRECTED).

### 4.4 Margin to the per-lane limits

**Reasoning** (inputs [A-05], [C-07], [C-30]):

- 972 Mbps is 97.2 % of the TC358743's 1 Gbps per-lane maximum and of Unicam's documented 1 Gbit/s per-lane limit.
- 972 Mbps is 64.8 % of RP1's 1.5 Gbps per-lane limit.
- On Pi 5/CM5 the receiver is therefore not the per-lane bottleneck. The transmitter and the driver's timing tables are.

### 4.5 Recommendation

**PROPOSED — recorded as ADR-008 (PROPOSED). OWNER DECISION REQUIRED (OQ-099).**

ADR-008 proposes keeping `link-frequency` at the default 486000000 on every platform. The reasons:

- At 594 Mbps, 1080p60 and 1080p50 RGB888 are rejected even on 4 lanes [C-47], [C-49].
- On Pi 5/CM5, 594 Mbps does not match the CFE's constant 999 Mbps D-PHY setting [C-52].
- Only 594 and 972 Mbps have driver timing tables [B-09].
- **Reasoning:** the per-line check in §10.4 leaves 1080p50 UYVY at 594 Mbps with 0.54 µs of slack per line. That is less than the ~675 ns LP↔HS transition time reported in [C-50], although that figure was reported for a different configuration (2-lane 1080p50 UYVY at 972 Mbps; see §10.6).

ADR-008 also proposes evaluating 297 MHz on a CM4 CAM1 4-lane link only, for 1080p60 UYVY in TEST-CAP-002. At that rate the mode uses all 4 lanes, which avoids the unproven 3-of-4-lane case (§13, OQ-038). Adopting it would need a new ADR.

---

## 5. Runtime lane selection by the driver

The driver computes the lane count as [A-25], [B-31], [C-15]:

```text
lanes = DIV_ROUND_UP(width × height × fps × bpp, bps_per_lane)
        bpp = 16 for MEDIA_BUS_FMT_UYVY8_1X16, 24 otherwise
        width × height = active pixels only (blanking ignored)
        bps_per_lane = refclk / pll_prd × pll_fbd
```

- The driver writes the count into the NOL field of `CSI_CONTROL` indirectly, through the `CSI_CONFW` write port. It disables the unused data lanes [A-25] (CORRECTED).
- The count is stored in `csi_lanes_in_use`. It is reported by `get_mbus_config` as type `V4L2_MBUS_CSI2_DPHY`, `flags = 0`, `num_data_lanes = csi_lanes_in_use`. It is not clamped to the DT `data-lanes` or to 4 [B-31].
- `s_dv_timings` stores new timings, mutes the stream and reprograms PLL and CSI, which recomputes the lane count. It does not compare the result with the DT `data-lanes` [B-30].
- **Reasoning, from [B-33]:** a result above 4 falls into the `MASK_NOL_1` branch, i.e. the lane count is mis-programmed. The receiver check in §6 still rejects such a mode at STREAMON [C-47].
- **The bus format changes the lane count.** The probe default is `RGB888_1X24` [A-09]. **Reasoning** (inputs [A-09], [C-47]): if userspace does not select `UYVY8_1X16` on the bridge pad, 1080p50 at 972 Mbps requests 3 lanes instead of 2, and fails on a 2-lane port.

**Lanes the driver requests** (Reasoning facts [C-47], [B-33]):

| Mode | UYVY at 972 Mbps | RGB888 at 972 Mbps | UYVY at 594 Mbps | RGB888 at 594 Mbps |
|---|---|---|---|---|
| 1080p60 | 3 | 4 | 4 | 6 |
| 1080p50 | 2 | 3 | 3 | 5 |
| 1080p30 | 2 | 2 | 2 | 3 |
| 720p60 | 1 [B-33] | 2 [B-33] | not in register | not in register |

**Consequence.** A mode that needs more lanes than the receiver's DT endpoint configures is accepted by `S_DV_TIMINGS` and by set-format, which only recompute the lane count. It fails only at STREAMON [B-30], [B-35], [B-31], [B-32].

**Reasoning** (inputs [B-32], [C-16]): the STREAMON check compares against the DT endpoint, not against the physical wiring. If the DT declares more lanes than the bridge board and cable actually carry, the check passes. What the receiver then delivers is **UNKNOWN — VERIFICATION REQUIRED (HARDWARE TEST REQUIRED)**. The DT lane count therefore has to match the wiring recorded in [HARDWARE.md](HARDWARE.md) (OQ-021).

**PROPOSED** (supports REQ-CAP-005, itself PROPOSED; no ADR):

- The capture application evaluates the formula above for the detected timings and chosen format before STREAMON. It reports the mode as unsupported instead of attempting to stream it.
- The EDID advertises only modes that the configured CSI-2 link can carry (REQ-CAP-003, PROPOSED; EDID content is OQ-002). **Reasoning** (inputs [C-48], [C-49]): the 2-lane and 4-lane configurations of REQ-CAP-007 carry different mode sets, so the EDID may need to differ per lane configuration (§11.3).

---

## 6. Receiver lane checks

| Check | Behaviour | Source |
|---|---|---|
| At stream start | Both receivers call `get_mbus_config` and store the bridge's lane count: Unicam in `dev->active_data_lanes`, CFE in `cfe->csi2.dphy.active_lanes`. | [B-32], [C-16] |
| More lanes than the DT endpoint | STREAMON fails with `-EINVAL` and logs `Device has requested %u data lanes, which is >%u configured in DT`. | [B-32], [C-16] |
| Fewer lanes than the DT endpoint (for example 3 of 4) | Passes the check. Whether frames are then received correctly is open (§13, OQ-038). | [C-16] |
| Unicam endpoint lane count | Only 1, 2 or 4 lanes, in order. | [C-17] |
| Unicam endpoint lists more lanes than `brcm,num-data-lanes` | Probe does **not** fail. Unicam logs `subdevice requires %u data lanes when %u are supported` and adopts the endpoint count. | [C-17] |

A community report gives an example of the STREAMON check firing. On a 4-lane CM4 with `link-frequency=297000000`, Unicam rejected 1080p50 RGB24 with `Device has requested 5 data lanes, which is >4 configured in DT` [C-43].

**PROPOSED** (Claude's reasoning from [C-17], [C-01], [C-02]): never set the overlay's `4lane` parameter on Pi 4 Model B or on CM4 CAM0. The kernel does not reject it at probe, and those ports route only 2 lanes. The research flagged this as a documentation gap (topic C). **Reasoning** (from §5, [B-32], [C-16]): the same applies to a 2-lane configuration of REQ-CAP-007 built on a 4-lane port with a bridge board that routes only 2 lanes, because the STREAMON check compares against the DT, not the wiring.

---

## 7. Receivers

### 7.1 Unicam — Pi 4 Model B and CM4

| Property | Value | Source |
|---|---|---|
| Instances | Two. The first supports 2 data lanes and the second 4. | [C-07] |
| Per-lane limit | Up to 1 Gbit/s per lane (DDR, so the maximum link frequency is 500 MHz) | [C-07] |
| Broadcom documentation of that limit | Not found. Treat [C-07] as the documented limit (DATASHEET REQUIRED; OQ-047). | — |
| DT nodes | `csi0` (`csi@7e800000`) with `brcm,num-data-lanes = <2>`; `csi1` (`csi@7e801000`) with `<4>`. Both are compatible `brcm,bcm2835-unicam`. | [C-08] |
| Pi 4 Model B | `csi1` is limited to 2 lanes (`bcm283x-rpi-csi1-2lane.dtsi`). The connector is 15-pin with 2 lanes. | [C-08], [B-48], [C-01] |
| CM4 | `csi0` has 2 lanes (CAM0) and `csi1` has 4 lanes (CAM1) | [C-08], [B-48], [C-02] |
| Drivers in `rpi-6.18.y` | The downstream `bcm2835-unicam-legacy` module binds `brcm,bcm2835-unicam` (forces Media Controller mode) and `brcm,bcm2835-unicam-legacy` (mode follows the `media_controller` module parameter, default 0). The mainline driver binds only `brcm,bcm2835-unicam-upstream`. | [C-09], [G-21] |
| Mode chosen by the stock overlay | The `tc358743` overlay changes the `csi1` compatible to `brcm,bcm2835-unicam-legacy` (video-node mode). The `media-controller` parameter removes that fragment, giving Media Controller mode. | [C-10], [B-43] |
| Media graph in MC mode | A single image node, `unicam-image`, with an immutable, enabled link straight from the bridge pad. There is no CSI-2 receiver sub-device. `unicam-embedded` is not created for the single-pad TC358743. | [C-36] (CORRECTED) |
| EDID and DV timings | Legacy mode: through the video node. MC mode: only through a read-write `/dev/v4l-subdevN`. | [C-36], [B-25] |

Which driver and mode actually bind on a running Pi 4/CM4: **HARDWARE TEST REQUIRED (OQ-044).** One control model on every platform (Media Controller) is **PROPOSED** in ADR-006.

### 7.2 RP1 CFE — Pi 5 and CM5

| Property | Value | Source |
|---|---|---|
| DT nodes | `rp1_csi0` (`csi@110000`) and `rp1_csi1` (`csi@128000`), compatible `raspberrypi,rp1-cfe` | [C-29] |
| Drivers in `rpi-6.18.y` | The downstream module `rp1-cfe-downstream` binds `raspberrypi,rp1-cfe`. The mainline-derived driver binds only `raspberrypi,rp1-cfe-upstream`. Both are modules in `bcm2712_defconfig`. | [C-29], [G-17] |
| Per-lane limit | 1.5 Gbps per lane (byte clock ≤ 187.5 MHz) | [C-30] |
| D-PHYs | Two shared 4-lane D-PHYs (CSI-2 or DSI), 8 Gbps in total | [C-30] |
| Pi 5 ports | Two 22-pin 0.5 mm combined CSI/DSI ports, each 4 lanes at 1.5 Gbps | [C-04] |
| CM5 interfaces | MIPI0 (on the CM4 CAM1 pins) and MIPI1 (on the CM4 DSI1 pins), 4 lanes each. The CM4 CAM0 pins carry USB 3.0 on CM5. | [C-05] |
| D-PHY rate programming | `sensor_link_rate()` walks the graph for a `MEDIA_ENT_F_CAM_SENSOR` entity, or one exposing `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE`. If none is found, it logs `Unable to determine sensor link rate, using 999 Mbps` and uses 999 Mbps. The `hsfreqrange` table covers 80–1500 Mbps. | [C-31] |
| For TC358743 | The bridge is `MEDIA_ENT_F_VID_IF_BRIDGE` and has no `LINK_FREQ` or `PIXEL_RATE` control, so the 999 Mbps fallback is always taken. | [C-19], [B-16], [B-49], [C-31] |
| Media graph | The `csi2` sub-device has sink pads 0–3 and source pads 4–7. The bridge→`csi2` link is immutable and enabled. The `csi2` source-pad links to `rp1-cfe-<node>` video nodes and to `pisp-fe` are created **disabled**. To write frames to memory, userspace must enable `csi2`:4 → `rp1-cfe-csi2_ch0`. | [C-32] |
| Control model | Media Controller only. `dtoverlay=tc358743` is redirected to `tc358743-pi5`, which has no legacy fragment and no `media-controller` parameter. | [C-11], [E-43], [G-13] |
| EDID and DV timings | The CFE driver has no EDID or DV-timings handling. They are always issued on the bridge sub-device node. | [B-25] |
| Official TC358743 documentation for Pi 5 | None. The only TC358743 text is in the Unicam section. | [C-38]; RISK-012 |

**Reasoning, 999 Mbps versus the actual link** (input [C-52]):

- The fallback matches the default 972 Mbps link.
- With `link-frequency=297000000` (594 Mbps), the `hsfreqrange` row programmed for 999 Mbps (`{ 999, … }`) is not the row whose range covers 594 Mbps (`{ 599, … }`).
- Effect on reception: **UNKNOWN — HARDWARE TEST REQUIRED (OQ-050, RISK-011).**

A Raspberry Pi engineer gave a Pi 5 capture sequence and reported capturing with it on kernel 6.18.39 [C-33] (community). 6.18.39 is only the kernel named in that report: Raspberry Pi OS 2026-10-06 ships 6.18.50 [G-04], and the source was inspected at 6.18.55 [E-37]. §14 reproduces its link-related steps.

**Candidate driver patch (not proposed).** The CFE's graph walk stops at an entity that exposes `V4L2_CID_LINK_FREQ` [C-31]. **Reasoning:** adding that control to the TC358743 driver is therefore a candidate fix for RISK-011. Under ADR-002 (PROPOSED), a driver patch is made only after a defect is shown on PACSCORDER hardware. Whether CFE then programs the correct rate: **NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED).**

### 7.3 Side by side

| | Unicam (Pi 4B, CM4) | RP1 CFE (Pi 5, CM5) |
|---|---|---|
| Per-lane limit | 1 Gbit/s [C-07] | 1.5 Gbps [C-30] |
| Lanes per port | Pi 4B: 2; CM4 CAM0: 2; CM4 CAM1: 4 [C-01], [C-02] | 4 per port [C-04], [C-05] |
| D-PHY rate source | Not in the source register (KERNEL SOURCE INSPECTION REQUIRED) | Always the 999 Mbps fallback for this bridge [C-31] |
| CSI-2 receiver sub-device in the graph | None [C-36] | `csi2` [C-32] |
| Link the user must enable | None (immutable) [C-36] | `csi2`:4 → `rp1-cfe-csi2_ch0` [C-32] |
| Lane check at STREAMON | Yes [B-32] | Yes [B-32] |
| Allowed endpoint lane counts | 1, 2, 4 [C-17] | Not in the source register |

---

## 8. Clock-lane mode discrepancy

| Layer | What it says | Source |
|---|---|---|
| Overlay (`tc358743.dtsi`) | Sets `clock-lanes <0>` and `clock-noncontinuous` | [C-12], [B-41] |
| Driver, `tc358743_set_csi()` | The DT flag selects `TXOPTIONCNTRL`: 0 if non-continuous, `MASK_CONTCLKMODE` otherwise | [B-37] |
| Driver, stream on | Always writes `TXOPTIONCNTRL = 0`, then `TXOPTIONCNTRL = MASK_CONTCLKMODE` (continuous clock), to force the clock-lane LP-11→HS transition | [B-36], [B-37] |
| Driver, stream off | Re-runs `tc358743_set_csi()` to put all lanes in LP-11 | [B-36] |
| Driver → receiver (`get_mbus_config`) | `flags = 0`, with the comment "Support for non-continuous CSI-2 clock is missing in the driver" | [B-37], [B-31] |

**Reasoning** (inputs above): while streaming, the transmitter clock lane is continuous whatever the DT says, and the bridge reports a continuous clock to the receiver. How Unicam and CFE use their own endpoint flag, and whether a mismatch affects reception: **UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED; OQ-037).** Until resolved, receiver configuration must tolerate a continuous clock.

---

## 9. Data types and pixel formats

The driver offers exactly two media bus codes [A-09], [B-34]:

| Media bus code | Driver index | bpp on the link | Colorimetry | Unicam fourcc (Pi 4/CM4) | CFE fourcc (Pi 5/CM5) | Notes |
|---|---|---|---|---|---|---|
| `MEDIA_BUS_FMT_RGB888_1X24` | 0, **probe default** | 24 [C-15] | `SRGB`, full range [B-34] | `V4L2_PIX_FMT_RGB24` [C-34] | `V4L2_PIX_FMT_BGR24` [C-34]; the sequence a Raspberry Pi engineer reported uses `BGR3` [C-33] (community) | On CM4, bytes in memory were reported as B,G,R and explained by a Raspberry Pi engineer [C-35] (community). RISK-016, OQ-045. |
| `MEDIA_BUS_FMT_UYVY8_1X16` | 1 | 16 [C-15] | `SMPTE170M`, BT.601 limited range [B-34]. The research reports that this also applies to 720p/1080p sources (research gap, topic B; OQ-041). | `V4L2_PIX_FMT_UYVY` [C-34] | `V4L2_PIX_FMT_UYVY` [C-34] | CSI-2 data type YUV422_8B [C-34]. `CONFCTL` `YCBCRFMT = 422_8_BIT` [A-09]. **PROPOSED default (ADR-005).** |

- The register header also defines 12-bit 4:2:2 and colour-bar output codes, which the driver does not use [A-10].
- Official Raspberry Pi documentation: only RGB888 and YUV422 have been tested [C-37].
- **Reasoning** (input [C-15]): UYVY uses 16/24 = two-thirds of RGB888's link bandwidth for the same mode.
- In Unicam Media Controller mode, enumeration filtered by `UYVY8_1X16` may omit UYVY even though setting it succeeds. That comes from a research gap (topic C), not a register fact (OQ-046).
- Not in the source register: the CSI-2 data type used on the wire for RGB888, and the virtual channel the bridge uses. **NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED; OQ-041).**
- Encoder colour signalling for BT.601-limited UYVY from HD sources is OQ-041.

---

## 10. Bandwidth

### 10.1 Inputs

**Reasoning fact [C-46].** Payload is computed with the driver's active-pixel formula. Frame rates come from CEA timings (pixel clock / (htotal × vtotal)):

| Mode | Pixel clock | htotal × vtotal | UYVY payload (16 bpp) | RGB888 payload (24 bpp) |
|---|---|---|---|---|
| 1080p60 | 148.5 MHz | 2200 × 1125 | 1.990656 Gbit/s | 2.985984 Gbit/s |
| 1080p50 | 148.5 MHz | 2640 × 1125 | 1.658880 Gbit/s | 2.488320 Gbit/s |
| 1080p30 | 74.25 MHz | 2200 × 1125 | 0.995328 Gbit/s | 1.492992 Gbit/s |

Link capacity is lanes × 2 × link frequency [C-46]:

| Lane rate | 2 lanes | 4 lanes |
|---|---|---|
| 594 Mbps (297 MHz) | 1.188 Gbit/s | 2.376 Gbit/s |
| 972 Mbps (486 MHz, default) | 1.944 Gbit/s | 3.888 Gbit/s |

### 10.2 972 Mbps per lane (overlay default)

**Reasoning.** Inputs: [C-46], [C-47]. The 2-lane percentages are [C-48]. In the 4-lane columns, the 1080p60 RGB888 value is [C-49]; the other 4-lane percentages are computed here from [C-46] and [C-47].

- "% of configured link" = payload ÷ (configured lanes × 972 Mbps).
- "% of active lanes" = payload ÷ (lanes the driver activates × 972 Mbps).

| Mode | Format | Lanes requested | 2-lane link: % of link | 2-lane outcome | 4-lane link: % of configured link | 4-lane: active lanes, % of active lanes | 4-lane outcome |
|---|---|---|---|---|---|---|---|
| 1080p60 | UYVY | 3 | 102.4 % | Exceeds; STREAMON `-EINVAL` | 51.2 % | 3, 68.3 % | Fits (3 of 4 lanes: OQ-038) |
| 1080p60 | RGB888 | 4 | 153.6 % | Exceeds | 76.8 % | 4, 76.8 % | Fits |
| 1080p50 | UYVY | 2 | 85.3 % | Fits | 42.7 % | 2, 85.3 % | Fits |
| 1080p50 | RGB888 | 3 | 128.0 % | Exceeds | 64.0 % | 3, 85.3 % | Fits by bandwidth; corruption reported [C-43] (community) |
| 1080p30 | UYVY | 2 | 51.2 % | Fits | 25.6 % | 2, 51.2 % | Fits |
| 1080p30 | RGB888 | 2 | 76.8 % | Fits | 38.4 % | 2, 76.8 % | Fits |

### 10.3 594 Mbps per lane (`link-frequency=297000000`)

**Reasoning.** Inputs: [C-46], [C-47]. On 2 lanes only 1080p30 UYVY fits (83.8 %) [C-48]. The 4-lane outcomes are [C-49]. The other percentages are computed here.

| Mode | Format | Lanes requested | 2-lane link: % of link | 2-lane outcome | 4-lane link: % of configured link | 4-lane: active lanes, % of active lanes | 4-lane outcome |
|---|---|---|---|---|---|---|---|
| 1080p60 | UYVY | 4 | 167.6 % | Exceeds | 83.8 % | 4, 83.8 % | Fits |
| 1080p60 | RGB888 | 6 | 251.3 % | Exceeds | 125.7 % | — | Exceeds (6 > 4) |
| 1080p50 | UYVY | 3 | 139.6 % | Exceeds | 69.8 % | 3, 93.1 % | Fits by formula; see §10.4 |
| 1080p50 | RGB888 | 5 | 209.5 % | Exceeds | 104.7 % | — | Exceeds (5 > 4) |
| 1080p30 | UYVY | 2 | 83.8 % | Fits | 41.9 % | 2, 83.8 % | Fits |
| 1080p30 | RGB888 | 3 | 125.7 % | Exceeds | 62.8 % | 3, 83.8 % | Fits |

### 10.4 Per-line check

The driver's formula averages over the whole frame and ignores blanking [C-15]. A stricter check asks whether one active line can be sent within one HDMI line period [C-50].

**Reasoning fact [C-50]** (972 Mbps):

- Line periods: 1080p60 = 14.815 µs; 1080p50 = 17.778 µs; 1080p30 = 29.630 µs.
- One active line takes:
  - UYVY: 15.80 µs on 2 lanes, 7.90 µs on 4 lanes;
  - RGB888: 23.70 µs on 2 lanes, 11.85 µs on 4 lanes.
- 2-lane 1080p60 UYVY and 2-lane 1080p50 RGB888 cannot keep up.
- 2-lane 1080p50 UYVY fits with about 2.0 µs of slack before LP↔HS overhead. As reported by Raspberry Pi engineer 6by9 from his reading of Toshiba's spreadsheet (community input to [C-50]), that overhead is about 675 ns for this mode.
- That spreadsheet's minimum of 898.12 Mbps per lane for this mode (same report) leaves about 7.6 % margin at 972 Mbps.

**Reasoning** (Claude, extending the method of [C-50] to the lane counts the driver actually activates [C-47]):

- Bits per active line = 1920 × bpp, i.e. 30,720 for UYVY and 46,080 for RGB888 [C-50].
- Line transmit time = bits ÷ (active lanes × lane rate).

| Lane rate | Mode | Format | Active lanes | Line period | Line transmit time | Slack per line |
|---|---|---|---|---|---|---|
| 972 Mbps | 1080p60 | UYVY | 3 | 14.815 µs | 10.53 µs | 4.28 µs |
| 972 Mbps | 1080p60 | RGB888 | 4 | 14.815 µs | 11.85 µs | 2.96 µs |
| 972 Mbps | 1080p50 | UYVY | 2 | 17.778 µs | 15.80 µs | 1.98 µs |
| 972 Mbps | 1080p50 | RGB888 | 3 | 17.778 µs | 15.80 µs | 1.98 µs |
| 972 Mbps | 1080p30 | UYVY | 2 | 29.630 µs | 15.80 µs | 13.83 µs |
| 972 Mbps | 1080p30 | RGB888 | 2 | 29.630 µs | 23.70 µs | 5.93 µs |
| 594 Mbps | 1080p60 | UYVY | 4 | 14.815 µs | 12.93 µs | 1.89 µs |
| 594 Mbps | 1080p50 | UYVY | 3 | 17.778 µs | 17.24 µs | **0.54 µs** |
| 594 Mbps | 1080p30 | UYVY | 2 | 29.630 µs | 25.86 µs | 3.77 µs |
| 594 Mbps | 1080p30 | RGB888 | 3 | 29.630 µs | 25.86 µs | 3.77 µs |

### 10.5 Official limits

Official Raspberry Pi documentation [C-37]:

- With 2 CSI-2 lanes, the maximum is 1080p30 as RGB888 or 1080p50 as YUV422.
- With 4 lanes on a Compute Module, 1080p60 can be received in either format.

The 2-lane calculation in §10.2 agrees with these limits [C-48].

### 10.6 Observations

All of the following are **Reasoning** from the tables above. None has been measured.

1. **A 4-lane link adds no per-lane margin to modes the driver packs into fewer lanes.** The driver activates only the lanes it computes [C-15], [C-49]. 1080p50 UYVY on a 4-lane port therefore uses 2 lanes at 85.3 %, exactly as on a 2-lane port.
2. **The reported-corrupt case and the official 2-lane case have the same per-lane load.**
   - 1080p50 RGB888 on 3 lanes is the mode in the open corruption report [C-43].
   - 1080p50 UYVY on 2 lanes is officially supported [C-37].
   - Both have the same per-active-lane load (85.3 %) and the same per-line transmit time (15.80 µs of 17.778 µs).

   In [C-43] a Raspberry Pi engineer attributed the corruption to the FIFO trigger level, so this match does not show that the UYVY mode is affected. It does put the proposed default format (ADR-005) at 1080p50 in the same load class, so that mode belongs in TEST-CAP-004 (RISK-006, OQ-035).
3. **1080p50 UYVY at 594 Mbps is doubtful.**
   - The frame-average formula accepts it on 3 lanes [C-49].
   - The per-line slack of 0.54 µs is below the ~675 ns LP↔HS transition time reported in [C-50].
   - Caveat: the 675 ns figure was reported for 2-lane 1080p50 UYVY at 972 Mbps. Its value at 594 Mbps on 3 lanes is not in the source register (DATASHEET REQUIRED; OQ-027, OQ-035).

   **NEEDS VERIFICATION (HARDWARE TEST REQUIRED).** This is one reason for the §4.5 recommendation.
4. **At the default rate, 1080p60 UYVY on 4 lanes has the lowest per-lane load of the 1080p50/60 modes** (68.3 % on 3 active lanes). It is also the case that depends on 3-of-4-lane operation (§13).

---

## 11. Per-platform feasibility

### 11.1 Platform facts

| Platform / port | Data lanes at the connector | Receiver (DT node) | Overlay and parameters | Platform-specific concerns |
|---|---|---|---|---|
| **Pi 4 Model B** | 2 (15-pin, 1.0 mm) [C-01] | Unicam `csi1`, limited to 2 lanes [C-08], [B-48] | `tc358743` (default 2 lanes) [C-12] | `4lane` is not rejected at probe [C-17]; do not use it (§6) |
| **CM4 CAM0** | 2 [C-02] | Unicam `csi0` (2 lanes) [C-08] | `tc358743` with `cam0` [C-10], [B-42] | CM4 IO Board: J6 jumpers needed for I2C to reach CAM0 [C-03]. I2C bus `i2c-0` [C-24], [C-25]. |
| **CM4 CAM1** | 4 [C-02]; CM4 IO Board 22-pin, 0.5 mm [C-03] | Unicam `csi1` (4 lanes) [C-08] | `tc358743` with `4lane` [C-12], [B-42] | I2C bus `i2c-10` [C-24], [C-25]. Only port with official 4-lane TC358743 documentation: the official text describes 4 lanes "on a Compute Module" and sits in the Unicam section [C-37], [C-38]. |
| **Pi 5** (either port) | 4 per port (22-pin, 0.5 mm) [C-04] | RP1 CFE `rp1_csi0` / `rp1_csi1` [C-29] | `tc358743` → `tc358743-pi5`; `4lane`, `cam0` [C-11], [G-13] | The overlays README still says "Uses Unicam 1" and calls `4lane` CM-CAM1-only — stale text [C-13], [B-45]. Whether `4lane` combined with `cam0` sets 4 lanes on `csi0`: NEEDS VERIFICATION (OQ-049). 999 Mbps D-PHY [C-31]. A wrongly sided 22-to-15-pin adapter can swap GND and 3V3, as reported by a Raspberry Pi engineer [C-45] (community); RISK-021. |
| **CM5** | 4 per MIPI interface (MIPI0 on the CM4 CAM1 pins, MIPI1 on the CM4 DSI1 pins); no CM4-style CAM0 [C-05] | RP1 CFE [C-29] | As Pi 5 [C-11] | I2C mapping depends on the carrier board [C-27], [B-44]. CM5 IO Board CAM/DISP 1 needs J6 jumpers [C-06]. CM5 on a CM4 IO Board: the CAM1 connector pairing is open (OQ-052). |

Lanes actually routed by the PACSCORDER bridge board and carrier: **UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED; OQ-018, OQ-021).**

The "Overlay and parameters" column names overlay parameters, not `config.txt` syntax. Only `dtoverlay=tc358743,<param>=<val>` [G-12] and appending `,cam0` [C-39] are attested. Bare boolean parameters such as `4lane`, and several parameters on one line, are NEEDS VERIFICATION (OQ-100; see [DEVICE_TREE.md](DEVICE_TREE.md)).

### 11.2 Mode feasibility at the default 972 Mbps per lane

"Fits" and "Exceeds" are bandwidth results from §10 (**Reasoning**). They are not test results.

| Mode / format | Lanes requested [C-47] | Pi 4 Model B | CM4 CAM0 | CM4 CAM1 | Pi 5 | CM5 |
|---|---|---|---|---|---|---|
| 1080p60 UYVY | 3 | Exceeds [C-48] | Exceeds [C-48] | Fits; 3 of 4 lanes (OQ-038); officially documented for 4 lanes, link rate unstated [C-37] | Fits; 3 of 4 lanes (OQ-038); 999 Mbps D-PHY (OQ-050) | as Pi 5 |
| 1080p60 RGB888 | 4 | Exceeds [C-48] | Exceeds [C-48] | Fits [C-49]; officially documented [C-37] | Fits [C-49] | as Pi 5 |
| 1080p50 UYVY | 2 | Fits, 85.3 % [C-48]; officially documented [C-37] | Fits, 85.3 % [C-48] | Fits; still 2 lanes at 85.3 % (§10.6) | Fits (2 lanes) | as Pi 5 |
| 1080p50 RGB888 | 3 | Exceeds [C-48] | Exceeds [C-48] | Fits; corruption reported on 3 of 4 lanes [C-43] (community) | Fits; same 3-lane load (OQ-038) | as Pi 5 |
| 1080p30 UYVY / RGB888 | 2 / 2 | Fits [C-48]; RGB888 officially documented [C-37] | Fits [C-48] | Fits | Fits | as Pi 5 |
| **REQ-CAP-001 (1080p60)** | — | **Cannot be met** at the supported link rates [C-37], [C-48] (RISK-001); 2-lane configuration candidate only (REQ-CAP-007) | **Cannot be met** at the supported link rates [C-37], [C-48] (RISK-001); 2-lane configuration candidate only (REQ-CAP-007) | Feasible by bandwidth and official documentation, which does not state the link rate [C-37]; in UYVY at 972 Mbps it depends on 3 of 4 lanes (OQ-038; ADR-008, OQ-099); NOT STARTED | Feasible by bandwidth; in UYVY at 972 Mbps it depends on 3 of 4 lanes (OQ-038); no official documentation (RISK-012); RISK-011 | as Pi 5 |

- 1080p60: required on the 4-lane configuration; the 2-lane configuration captures every rate its link carries (REQ-CAP-007; OQ-001 ANSWERED by the owner on 2026-10-07). The exact mode list is OQ-002.
- Product platform: ADR-004 (OPEN), now a choice per lane configuration (§11.3); OQ-011.
- Capture format: ADR-005 (PROPOSED), OQ-003.
- 1080p60 *encode* is a separate constraint. See [VIDEO_ENCODER.md](VIDEO_ENCODER.md) and RISK-002.

### 11.3 Lane configurations required by REQ-CAP-007

The owner requires both a 2-lane and a 4-lane CSI-2 configuration, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; owner statement of 2026-10-07, "i need 2 lane and 4 lane with all frame rate"; OQ-001 ANSWERED). Recorded interpretation in [REQUIREMENTS.md](REQUIREMENTS.md): 1080p60 is required on the 4-lane configuration; on the 2-lane configuration the physical limit applies. **No platform is chosen here** (ADR-004, OPEN).

**Candidate connectors per configuration** (from §11.1; a mapping, not a selection):

| Configuration | Candidate connectors | Sources | Notes |
|---|---|---|---|
| 2-lane | Pi 4 Model B; CM4 CAM0; or a 2-lane bridge board on any 4-lane port (reasoning; board lane count OQ-021) | [C-01], [C-02] | Never set `4lane` (§6) |
| 4-lane | CM4 CAM1; Pi 5 CAM/DISP1 (CAM/DISP0 with `4lane`: NEEDS VERIFICATION, OQ-049); CM5 MIPI0 / MIPI1 (carrier-dependent, OQ-052) | [C-02], [C-04], [C-05] | 1080p60 UYVY runs on 3 of 4 lanes at 972 Mbps (OQ-038; ADR-008) |

Whether one bridge-board design can serve both configurations, for example a 4-lane board run with 2 lanes, is UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED; OQ-021).

**Supported-mode limits per configuration** (**Reasoning** from §10; bandwidth results, not test results):

| Configuration | Link rate | Fits | Exceeds | Sources |
|---|---|---|---|---|
| 2-lane | 972 Mbps (default) | 1080p30 UYVY (51.2 %), 1080p30 RGB888 (76.8 %), 1080p50 UYVY (85.3 %); 720p60 UYVY (1 lane) and RGB888 (2 lanes) | 1080p50 RGB888 (128 %), 1080p60 UYVY (102.4 %), 1080p60 RGB888 (153.6 %) | [C-37], [C-48], [B-33] |
| 2-lane | 594 Mbps | 1080p30 UYVY (83.8 %) | Every other 1080p mode in §10.3 | [C-48] |
| 4-lane | 972 Mbps (default) | All six 1080p30/50/60 × UYVY/RGB888 combinations; the highest is 1080p60 RGB888 at 76.8 % | — | [C-37], [C-49] |
| 4-lane | 594 Mbps | 1080p60 UYVY (4 lanes), 1080p50 UYVY (3), 1080p30 UYVY and RGB888 | 1080p50 RGB888 (5 lanes), 1080p60 RGB888 (6 lanes) | [C-49] |

- Official Raspberry Pi documentation gives the same 2-lane and 4-lane limits [C-37]. "All frame rates" on the 2-lane configuration therefore means all rates up to 1080p50 UYVY / 1080p30 RGB888 for 1920x1080.
- 4-lane caveats: 3-of-4-lane capture is unproven (§13, OQ-038), and 1080p50 RGB888 on 3 of 4 lanes has an open corruption report [C-43] (community; RISK-006). 2-lane caveat: 1080p50 UYVY is in the same per-lane load class as that report (§10.6; TEST-CAP-004).
- **Other and fractional frame rates (Reasoning** from the lane formula [C-15]): the requested lane count never decreases as the frame rate rises, so at the same resolution and format a lower rate (for example 1080p24 or 1080p25) requests no more lanes than 1080p30. A fractional rate (23.98, 29.97, 59.94 Hz) is below its integer rate and requests no more lanes than it; the driver reports fractional rates as integer rates [B-28] (OQ-040). REQ-CAP-007 counts fractional rates as part of "all frame rates" unless the owner says otherwise.

**HDMI sources (REQ-CAP-008).** The sources are ATEM switcher HDMI outputs and cameras connected directly (owner, 2026-10-07; OQ-009 ANSWERED; models OQ-102).

- The ATEM Mini Pro HDMI output standards are 1080p23.98, 24, 25, 29.97, 30, 50, 59.94 and 60, with no 720p or 1080i [F-23].
- **Reasoning** (inputs [F-23], [C-48]): on the 2-lane configuration, that ATEM's 1080p59.94 and 1080p60 standards exceed the link, and 1080p50 fits only in UYVY. The source must therefore be steered to a mode the 2-lane link carries. Whether each source follows the EDID is HARDWARE TEST REQUIRED (OQ-002).
- Camera HDMI output modes are UNKNOWN — VERIFICATION REQUIRED per model (OQ-102). Interlaced input is rejected by the driver with `-ERANGE` [B-27] (RISK-009).

**EDID per configuration (Reasoning; OQ-002).** The EDID is supplied from userspace [C-37]. The built-in `hdmi` EDID type of `v4l2-ctl` advertises up to 1080p60 [B-24], more than a 2-lane link carries [C-48]. Because the two configurations carry different mode sets, the EDID loaded at start may need to differ per lane configuration (REQ-CAP-003, PROPOSED; §5). The EDID content is OQ-002.

---

## 12. FIFO trigger level issue

| Item | Content | Source |
|---|---|---|
| Driver value | On DT platforms the driver hard-codes `fifo_level = 374`. The source comment says 16 fails at higher rates, and that "A value of 374 works with both those modes at 594Mbps" (720p60 on 2 lanes, 1080p60 on 4 lanes) "and with most modes on 972Mbps". This is the driver author's statement, not a PACSCORDER result. | [B-12], [C-44] |
| Intended use of the two rates | Driver comment: 594 Mbps is meant for 4-lane 1080p60 or 2-lane 720p60; 972 Mbps allows 1080p50 UYVY over 2 lanes. Mainline has the same code. | [C-44] |
| Origin of the value | A Raspberry Pi engineer reported that Toshiba's FIFO formula is in an NDA document and that 374 is an empirical value. | [A-43] (community) |
| Open defect report | An open issue reports corrupted images at 1080p50 RGB24 on a 4-lane CM4, where the driver chose 3 lanes. A Raspberry Pi engineer attributed it to the hard-coded FIFO trigger level (Toshiba's spreadsheet gives a minimum of 120) and to the lane formula using active height instead of total line time. | [C-43] (community) |
| Margin of the officially supported 2-lane mode | About 7.6 % for 1080p50 UYVY at 972 Mbps | [C-50] |
| What would resolve it | Toshiba's register spreadsheet (REF_02), which is not public [A-42]. A Raspberry Pi engineer reported that the FIFO formula is covered by NDA [A-43] (community). | [A-42], [A-43]; OQ-027 |

- Status: **BLOCKED — HARDWARE REQUIRED.**
- Retired only by TEST-CAP-004 across the supported-mode matrix, including the 85.3 % cases from §10.6, and by TEST-PERF-001 for long runs and temperature (RISK-006, OQ-035).
- Under ADR-002 (PROPOSED, not yet decided), a driver change would be made only if corruption is reproduced on PACSCORDER hardware.

---

## 13. Open question: 3 of 4 lanes

**Why it matters.** At the default 972 Mbps, two modes use 3 lanes [C-47]:

- 1080p60 UYVY, i.e. REQ-CAP-001 and the 4-lane configuration of REQ-CAP-007 in the proposed default format (ADR-005);
- 1080p50 RGB888.

At 594 Mbps, 1080p50 UYVY and 1080p30 RGB888 use 3 lanes [C-47].

**What the sources establish:**

- The driver programs and reports 3 lanes, without clamping [A-25], [B-31].
- Both receivers accept 3 against a 4-lane endpoint at STREAMON and record the active count [C-16], [B-32].
- The Unicam DT endpoint itself must be 1, 2 or 4 lanes. 3 lanes is reached only at runtime through `get_mbus_config` [C-17].

**What is not established:** whether Unicam and RP1 CFE then receive frames correctly on 3 active lanes. The research flagged this as a gap (topic B).

**Indirect evidence (community, official and kernel sources; none is a test of this question):**

- The open corruption report [C-43] is a 3-of-4-lane case (1080p50 RGB24, "Lanes in use: 3" on CM4). It was attributed to the FIFO level, not to the lane count.
- The cross-check in [C-49] cites a Pi 5 4-lane community report in which 1080p60 BGR888 streamed at 60.14 fps, and 1080p60 UYVY was reported to stream once the pads were set up correctly.
  - If that report used the default 486 MHz link, the UYVY case ran on 3 lanes [C-47].
  - The register does not record the link frequency used, so this is not evidence for 3-of-4 operation.
- Official Raspberry Pi documentation says 4 lanes on a Compute Module receive 1080p60 "in either format" [C-37]. It does not state the link frequency. A driver comment says 594 Mbps is meant for 4-lane 1080p60 [C-44], and at that rate 1080p60 UYVY uses all 4 lanes [C-47]. So this is not evidence for 3-of-4 operation either.

**Fallback if 3 lanes fails** (**Reasoning**):

- At 594 Mbps, 1080p60 UYVY uses all 4 lanes at 83.8 % [C-47], [C-49].
- That rate costs RGB888 1080p50/60 [C-47].
- On Pi 5/CM5 it mismatches the 999 Mbps D-PHY setting [C-52].
- Its per-line slack is 1.89 µs (§10.4).
- ADR-008 (PROPOSED) proposes evaluating this rate in TEST-CAP-002 on a CM4 CAM1 4-lane link only, not on Pi 5/CM5 (§4.5, OQ-099). Adopting it would need a new ADR.

**Resolution:** HARDWARE TEST REQUIRED on each 4-lane candidate (OQ-038). Resolving tests: TEST-CAP-002, TEST-CAP-004.

---

## 14. Link bring-up checks (procedure outline)

> **NOT YET RUN ON PACSCORDER HARDWARE.** This is an outline of link-level checks. The authoritative procedures and results belong in [TESTING.md](TESTING.md) under TEST-PLT-001, TEST-CAP-002 and TEST-CAP-004. Device numbers, media device numbers and entity names are unknown until TEST-PLT-001 (OQ-043).

**Before power-on**

1. Record the bridge board's lane count, connector, cable orientation and REFCLK frequency in [HARDWARE.md](HARDWARE.md) (OQ-021, OQ-019, RISK-021).

**Kernel messages that identify link problems**

| Message | Meaning | Source |
|---|---|---|
| `unsupported bps per lane` | 2 × `link-frequencies[0]` is outside 62.5 Mbps–1 Gbps. Probe returns `-EINVAL`. | [B-08] |
| `untested bps per lane` | `link-frequency` is not 297000000 or 486000000. The 594 Mbps timings are being used. | [A-24], [B-09] |
| `unsupported refclk rate` | The DT clock rate is not 26, 27 or 42 MHz. A kernel `BUG_ON()` follows if the chip answers. | [A-22] |
| `subdevice requires %u data lanes when %u are supported` (Unicam) | The DT endpoint lists more lanes than the port. Probe continues anyway. | [C-17] |
| `Unable to determine sensor link rate, using 999 Mbps` (CFE) | Always expected with TC358743 on Pi 5/CM5. | [C-31] |
| `Device has requested %u data lanes, which is >%u configured in DT` | The current mode/format needs more lanes than are configured. STREAMON fails with `-EINVAL`. | [B-32], [C-16] |

**Pi 5 / CM5 link and pad setup.** Source: a sequence a Raspberry Pi engineer gave and reported capturing with on kernel 6.18.39, the kernel named in that report [C-33] (community). `<M>`, `<N>` and `<TC>` are placeholders (OQ-043).

```sh
# NOT YET RUN ON PACSCORDER HARDWARE.
# Option forms: 'v4l2-ctl -d' appears in the register only as '-d 11' [D-17]
# (community); passing a sub-device path to it is NEEDS VERIFICATION (OQ-101).
# [C-33] says only that the timing command is run "on /dev/v4l-subdevN".
# 'media-ctl -d' appears only as '-d 2' [C-33]; other argument forms are
# NEEDS VERIFICATION (OQ-101). See the notes table in V4L2.md.
# 1. EDID first: v4l2-ctl --set-edid on the bridge sub-device [C-37], [C-33].
#    Option syntax: --set-edid pad=<pad>[,type=<type>|file=<file>]... [B-24];
#    full command in V4L2.md. EDID content: OQ-002.
# 2. Lock the detected timings on the bridge sub-device [C-33], [C-37]:
v4l2-ctl -d /dev/v4l-subdev<N> --set-dv-bt-timings query
# 3. Enable the CSI-2 → video-node link [C-33], [C-32]:
media-ctl -d <M> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'
# 4. Set matching pad formats, including field:none [C-33]:
media-ctl -d <M> -V '"<TC>":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
media-ctl -d <M> -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
media-ctl -d <M> -V '"csi2":4 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'
# 5. Set the capture node to pixelformat UYVY (BGR3 for RGB888_1X24) [C-33].
#    Exact v4l2-ctl option: NEEDS VERIFICATION (OQ-101).
```

- The reporter found that leaving out `field:none` on the `csi2` pads makes STREAMON fail with `-EPIPE` [C-33].
- On Pi 4/CM4 in Media Controller mode, the bridge → `unicam-image` link is immutable and enabled [C-36], so no link needs enabling.
- If UYVY is used (ADR-005, PROPOSED), the bridge pad format must still be set to `UYVY8_1X16`, because the probe default is RGB888 [A-09]. Doing this with `media-ctl -V` on Unicam: **NEEDS VERIFICATION.**
- Full per-platform sequences are in [V4L2.md](V4L2.md).

**Link-quality measurement.** How to read CSI-2 error counters on Unicam and CFE is not in the source register. **UNKNOWN — VERIFICATION REQUIRED (KERNEL SOURCE INSPECTION REQUIRED, HARDWARE TEST REQUIRED).** OQ-050 covers this for CFE; OQ-095 covers Unicam (Pi 4/CM4).

---

## 15. Traceability

| ID | Relation to this document | Status |
|---|---|---|
| REQ-ARCH-001 | §2: the CSI-2 → receiver → Media Controller part of the mandated path | NOT STARTED |
| REQ-CAP-001 | §10–§11: 1080p60 needs a 4-lane port (necessary, not shown sufficient for UYVY: OQ-038); open items §12–§13 | NOT STARTED; TEST-CAP-002 BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-002 | §7: receiver driver and graph per platform | NOT STARTED |
| REQ-CAP-003 (PROPOSED) | §5, §11.3: EDID must advertise only modes the configured CSI-2 link can carry; may differ per lane configuration (OQ-002) | NOT STARTED |
| REQ-CAP-005 (PROPOSED) | §5–§6: lane pre-check before STREAMON (PROPOSED) | NOT STARTED; TEST-CAP-004 BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-007 (DRAFT) | §1, §11.2, §11.3: 2-lane and 4-lane configurations and the modes each link carries | NOT STARTED; TEST-CAP-002, TEST-CAP-004 BLOCKED — HARDWARE REQUIRED |
| REQ-CAP-008 (DRAFT) | §2, §11.3: ATEM and camera HDMI sources (models OQ-102) | NOT STARTED; TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 BLOCKED — HARDWARE REQUIRED |
| REQ-PLT-001 | §11: all four platforms | NOT STARTED; TEST-PLT-001 BLOCKED — HARDWARE REQUIRED |
| ADR-002 (PROPOSED) | §7.2, §12: patch the in-tree driver only for defects shown on hardware | — |
| ADR-004 (OPEN) | §11, §11.3: platform choice, per lane configuration (REQ-CAP-007) | — |
| ADR-005 (PROPOSED) | §9, §10.6, §13: UYVY default and its 3-lane consequence at 1080p60 | — |
| ADR-006 (PROPOSED) | §7.1: Media Controller mode on every platform | — |
| ADR-008 (PROPOSED) | §4.5, §13: keep link-frequency 486 MHz on every platform; evaluate 297 MHz on CM4 CAM1 only (OQ-099) | — |
| RISK-001 | §10–§11 | OPEN |
| RISK-006 | §10.6, §12 | OPEN |
| RISK-007 | §4.2 | OPEN |
| RISK-011 | §7.2 | OPEN |
| RISK-012 | §7.2, §11 | OPEN |
| RISK-016 | §9 | OPEN |
| RISK-021 | §11.1, §14 | OPEN |

**Open questions referenced:** OQ-001 (ANSWERED 2026-10-07), OQ-002, OQ-003, OQ-009 (ANSWERED 2026-10-07), OQ-011, OQ-018, OQ-019, OQ-021, OQ-027, OQ-030, OQ-031, OQ-035, OQ-037, OQ-038, OQ-040, OQ-041, OQ-043, OQ-044, OQ-045, OQ-046, OQ-047, OQ-049, OQ-050, OQ-052, OQ-095, OQ-099, OQ-100, OQ-101, OQ-102.

---

## Verification status

**Verified from sources (fact IDs)**

These are statements found in the cited sources, or arithmetic built on them. They are not observations on PACSCORDER hardware.

- **Transmitter and link rate:** [A-05], [A-06], [A-07], [A-10], [A-13], [A-22] (CORRECTED), [A-23], [A-24], [A-25] (CORRECTED), [A-26] (CORRECTED), [A-42], [A-45], [A-46], [B-07], [B-08], [B-09], [B-10], [B-11] (CORRECTED), [B-12], [B-30], [B-31], [B-33], [B-35], [B-36], [B-37], [B-41], [B-42], [B-47], [C-12], [C-13], [C-14], [C-15], [C-44].
- **Commands and overlay syntax:** [B-24] (`--set-edid` syntax), [D-17] (community; `v4l2-ctl -d` option form), [C-33] (community), [C-37], [G-12] (`dtoverlay=tc358743,<param>=<val>`), [C-39] (CORRECTED; `,cam0`).
- **Receivers:**
  - Unicam: [B-25] (CORRECTED), [B-32], [B-43], [B-48], [C-01], [C-02], [C-03], [C-07], [C-08], [C-09], [C-10], [C-16], [C-17], [C-24], [C-25], [C-36] (CORRECTED), [G-21].
  - RP1 CFE: [B-16], [B-44] (CORRECTED), [B-45], [B-49], [C-26], [C-04], [C-05], [C-06], [C-11], [C-19], [C-27], [C-28] (community, CORRECTED), [C-29], [C-30], [C-31], [C-32], [C-38], [C-52], [E-43], [G-13], [G-17].
- **Formats:** [A-09], [B-34], [C-34], [C-35] (community), [C-33] (community).
- **Bandwidth (reasoning facts):** [C-46], [C-47], [C-48], [C-49], [C-50]; official limits [C-37].
- **HDMI sources and detected timings (§11.3):** [F-23] (ATEM Mini Pro output standards), [B-27] (reasoning; interlaced rejected), [B-28] (fractional rates reported as integer rates).
- **Defect reports:** [A-43] (community), [C-43] (community), [C-45] (community).
- **Kernel baseline:** [D-01], [E-37], [G-04].
- **Claude's own reasoning in this document (not register facts):**
  - the active-lane percentages in §10.2–§10.3;
  - the per-line extension in §10.4;
  - the observations in §10.6;
  - the 42 MHz `pll_fbd` value in §4.2;
  - the DT-versus-wiring note in §5 and the candidate `V4L2_CID_LINK_FREQ` patch in §7.2;
  - in §11.3: the connector-to-configuration mapping for 2-lane bridge boards on 4-lane ports, the lower-rate and fractional-rate lane argument, the ATEM 2-lane consequence and the per-configuration EDID note.

**Verified on PACSCORDER hardware:** nothing (no hardware exists as of 2026-10-07).

Every procedure in §14 is **NOT YET RUN ON PACSCORDER HARDWARE**. Every hardware-dependent item is **BLOCKED — HARDWARE REQUIRED**.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review: community and CORRECTED wording, NDA claims, 675 ns caveat, DT-versus-wiring note, command option forms marked NEEDS VERIFICATION, `--set-edid` syntax cited, conditional wording for PROPOSED ADRs. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: §4.5 link-frequency recommendation linked to ADR-008 (PROPOSED) and OQ-099, with ADR-008's CM4 CAM1 297 MHz evaluation noted (§4.5, §13 fallback no longer "not proposed"); "4-lane port is necessary but not shown sufficient for 1080p60 UYVY" stated in §1, §11.2 and §15 (OQ-038); Unicam CSI-2 error counters linked to OQ-095 (§14); `v4l2-ctl -d` sub-device form, `media-ctl -d` forms and the video-node format option linked to OQ-101 (§14); `config.txt` parameter-syntax caveat added under §11.1 ([G-12], [C-39], OQ-100); 6.18.39 identified as the kernel of the community report only, with 6.18.50 [G-04] and 6.18.55 [E-37], and "reported as successful" reworded (§7.2, §14); ADR-008 traceability row and OQ list updated. No status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: both 2-lane and 4-lane configurations required (REQ-CAP-007; OQ-001 ANSWERED) in the header, intro, §1, §2, §6, §11.2 ("Whether 1080p60 is mandatory: OQ-001" replaced; Pi 4 Model B and CM4 CAM0 recorded as 2-lane candidates) and §13; new §11.3 maps candidate connectors to configurations and gives per-configuration supported-mode limits [C-37], [C-48], [C-49], [B-33], lower/fractional-rate reasoning [B-28] (OQ-040), HDMI sources = ATEM outputs and cameras (REQ-CAP-008, OQ-102, [F-23], [B-27]) and the per-configuration EDID note (OQ-002, reasoning [B-24]); §5 EDID note extended; ADR-004 now a per-configuration choice (still OPEN); traceability rows added for REQ-CAP-007 and REQ-CAP-008; OQ list adds OQ-009, OQ-040, OQ-102 and marks OQ-001/OQ-009 ANSWERED. Added citations B-27, B-28, F-23. No platform chosen; no ADR status changed. | Claude (session 2026-10-07) |
