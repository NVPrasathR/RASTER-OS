# PACSCORDER System Architecture

| | |
|---|---|
| Document status | DRAFT. It describes the mandated layer stack (Rule 5) and how each layer maps onto each candidate platform, from source research. No implementation exists. |
| Last updated | 2026-10-07 |
| Applies to | The PACSCORDER video pipeline, end to end, on all four candidate platforms: Raspberry Pi 4 Model B, Compute Module 4 (CM4), Raspberry Pi 5, Compute Module 5 (CM5), in both the 2-lane and the 4-lane CSI-2 configuration (REQ-CAP-007). The product platform is not chosen (ADR-004, OPEN; since 2026-10-07 a choice per lane configuration). |
| Verification | Verified from sources only: the source research of 2026-10-06 in [REFERENCES.md](REFERENCES.md). Nothing has been verified on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 5 (this document), Rule 8 (no invented hardware facts), Rule 10 (status words), Rule 12 (traceability), Rule 22 (unknowns), Rule 25 (new-engineer questions) |

This document answers Rule 25's questions "What is this?", "How does the video pipeline work?" and "Why was it designed this way?" at system level. It names the kernel driver, device node or component that implements each layer of the mandated pipeline on each platform. It also separates the **control plane** (configuration, status, events) from the **data plane** (pixels and bitstream), and links every layer to its requirements, decisions, risks and tests.

Userspace components are described in [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md). Layer detail lives in [HARDWARE.md](HARDWARE.md), [DEVICE_TREE.md](DEVICE_TREE.md), [TC358743_DRIVER.md](TC358743_DRIVER.md), [CSI_PIPELINE.md](CSI_PIPELINE.md), [V4L2.md](V4L2.md), [DMA.md](DMA.md), [VIDEO_ENCODER.md](VIDEO_ENCODER.md), [RECORDING.md](RECORDING.md), [STREAMING.md](STREAMING.md) and [ATEM.md](ATEM.md).

> **Current state (owner, 2026-10-06):**
> - No hardware exists.
> - No code exists.
> - Nothing has been tested.
> - The target platform is undecided, so all four candidates are kept (ADR-004).
>
> Every layer below is `NOT STARTED`. Every hardware-dependent test is `BLOCKED — HARDWARE REQUIRED`; TEST-BLD-001 (image build) is `NOT STARTED` ([TESTING.md](TESTING.md)). A statement with a fact ID such as `[C-37]` means only that the cited source says so (see [REFERENCES.md](REFERENCES.md)). Under Rule 23, a hardware measurement overrides it.

> **Owner decisions of 2026-10-07** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md)):
> - **Sources.** The HDMI sources are Blackmagic ATEM switcher outputs and cameras connected directly (REQ-CAP-008, DRAFT; OQ-009 ANSWERED). The ATEM integration is HDMI capture of the ATEM output only; ATEM network tally/control (UDP 9910) and RTMP exchange are not in current scope. Which ATEM and camera models must be supported is OQ-102.
> - **Lane configurations.** Both a 2-lane and a 4-lane CSI-2 configuration are required, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; OQ-001 ANSWERED). Recorded interpretation: 1080p60 is required on 4-lane configurations; on a 2-lane configuration the physical limit is 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48]. ADR-004 is now a platform choice per configuration (still OPEN).
> - **OS image.** The product runs its own project-built OS image (REQ-BLD-002, DRAFT). The build tool is ADR-003 (ACCEPTED 2026-10-07: Raspberry Pi OS Lite for bring-up, `rpi-image-gen` for the product image, Buildroot as the alternative); OQ-012 ANSWERED (owner, 2026-10-07).

**Conventions used here** (from [README.md](README.md)):

- `[X-NN]` cites the source register.
- Facts from `community` sources are worded "reported by …".
- Calculations are labelled **reasoning** and name their inputs.
- Facts with a `CORRECTED` verdict are used in their corrected wording only.
- Anything PACSCORDER-specific that is not known is written `UNKNOWN — VERIFICATION REQUIRED`, with a resolution marker and its `OQ-NNN` from [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md).

---

## 1. The mandated layer stack (Rule 5)

Rule 5 of [ENGINEERING_RULES.md](ENGINEERING_RULES.md) mandates this architecture, which REQ-ARCH-001 restates as a requirement:

```text
Hardware
    ↓
TC358743
    ↓
CSI-2
    ↓
Raspberry Pi CSI receiver
    ↓
Media Controller
    ↓
V4L2
    ↓
DMABUF
    ↓
Encoder
    ↓
Recorder / RTMP / WebRTC
```

In one line: `Hardware → TC358743 → CSI-2 → Raspberry Pi CSI receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder / RTMP / WebRTC`.

The stack is a logical order, not nine separate programs. Two points matter when mapping it to real components:

- **Media Controller and V4L2 are not sequential stages for pixels.** The Media Controller is the kernel graph that connects the TC358743 sub-device, the receiver and the capture video node. V4L2 provides the device nodes and ioctls through which that graph is configured and frames are streamed. On Pi 4/CM4 in Media Controller mode the graph has no separate CSI-2 receiver sub-device [C-36]. On Pi 5/CM5 the receiver appears as a `csi2` sub-device [C-32].
- **The Encoder layer is hardware on Pi 4/CM4 and software on Pi 5/CM5.** Pi 4/CM4 have the `bcm2835-codec` hardware H.264 encoder [D-06], [D-10]. Pi 5/CM5 have no hardware video encoder and encode in software [D-31], [G-22]. The DMABUF layer therefore means a different thing on each family (section 3.7).

### 1.1 Layer summary

| # | Layer | Pi 4 Model B / CM4 | Pi 5 / CM5 | Governing decisions | Status |
|---|---|---|---|---|---|
| 1 | Hardware | HDMI source: ATEM output or camera (REQ-CAP-008). 2-lane connector on Pi 4B [C-01]; CM4 CAM1 4 lanes, CAM0 2 lanes [C-02]. 2-lane configuration candidates: Pi 4B, CM4 CAM0; 4-lane: CM4 CAM1 (REQ-CAP-007). PACSCORDER board: UNKNOWN (OQ-018) | HDMI source: ATEM output or camera (REQ-CAP-008). 4-lane ports on Pi 5 [C-04]; CM5 MIPI0/1 4 lanes [C-05]; 4-lane configuration candidates (REQ-CAP-007). PACSCORDER board: UNKNOWN (OQ-018) | ADR-004 (OPEN; per lane configuration) | NOT STARTED |
| 2 | TC358743 | In-tree `tc358743` driver, module `tc358743` [A-48], [G-16]; overlay `tc358743` [G-12] | Same driver [B-02], [G-16]; overlay `tc358743-pi5`, auto-selected [C-11], [G-13] | ADR-001 (ACCEPTED), ADR-002 (PROPOSED), ADR-005 (PROPOSED) | NOT STARTED |
| 3 | CSI-2 | D-PHY, 972 Mbps/lane by default [A-45]; Unicam up to 1 Gbit/s per lane [C-07] | D-PHY; RP1 up to 1.5 Gbps per lane [C-30]; CFE programs 999 Mbps for this bridge [C-31] | ADR-004 (OPEN), ADR-005 (PROPOSED), ADR-008 (PROPOSED) | NOT STARTED |
| 4 | Raspberry Pi CSI receiver | Unicam, downstream driver module `bcm2835-unicam-legacy` [C-09] | RP1 CFE, downstream driver module `rp1-cfe-downstream` [C-29] | ADR-004 (OPEN), ADR-006 (PROPOSED) | NOT STARTED |
| 5 | Media Controller | Legacy video-node mode by overlay default, or Media Controller mode via the `media-controller` parameter [C-10] | Media Controller only [C-11]; `csi2` sub-device, links set by userspace [C-32] | ADR-006 (PROPOSED) | NOT STARTED |
| 6 | V4L2 | Capture node `unicam-image` [C-36]; TC358743 sub-device node [B-18] | Capture node `rp1-cfe-csi2_ch0` [C-32]; TC358743 sub-device node [B-18] | ADR-001 (ACCEPTED), ADR-006 (PROPOSED) | NOT STARTED |
| 7 | DMABUF | Target: capture buffer imported into the hardware encoder; encoder queues support DMABUF [D-19]; zero-copy from Unicam is unproven (OQ-058) | No hardware encoder to import into; frames go to a CPU encoder after format conversion [D-43] | ADR-007 (OPEN) | NOT STARTED |
| 8 | Encoder | Hardware `bcm2835-codec`, `/dev/video11`, H.264 spec 1080p30 [D-06], [D-10] | Software H.264; "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22] | ADR-004 (OPEN), ADR-007 (OPEN) | NOT STARTED |
| 9 | Recorder / RTMP / WebRTC | Userspace; framework not chosen | Userspace; framework not chosen | ADR-007 (OPEN) | NOT STARTED |

---

## 2. Control plane and data plane

PACSCORDER has two distinct paths through the stack.

- The **control plane** decides *what* the hardware does:
  - load the EDID;
  - detect and apply HDMI timings;
  - set media-bus and pixel formats;
  - enable media links;
  - report signal changes.

  It is ioctl-driven. Operations on the bridge (EDID, timings, status) reach the TC358743 over I2C through its driver [A-17]; link and capture-format operations act on the receiver and media graph.
- The **data plane** carries pixels from the HDMI cable to memory, and then a compressed bitstream to the outputs. Userspace does not touch it at register level: ADR-001 (ACCEPTED) keeps the CSI receiver and DMA in the kernel.

```text
CONTROL PLANE  (configuration, status, events)

  Userspace capture control  (SOFTWARE_ARCHITECTURE.md; NOT STARTED)
     |  EDID write   DV-timings query/set   pad formats + links   event wait
     |                      |                      |
     v                      v                      v
  /dev/v4l-subdevN      /dev/mediaN           /dev/videoN
  (tc358743 sub-device) (media graph)         (capture node: pixel format)
     |
     v
  tc358743 kernel driver --I2C, 16-bit register addresses [A-17]--> TC358743 registers
     ^
     |  signal status: INT-pin IRQ if wired [B-19],
     |  otherwise an I2C poll every 1000 ms (stock overlay) [A-30], [C-20]

DATA PLANE  (pixels, then bitstream)

  HDMI TMDS --> TC358743 --> CSI-2 D-PHY lanes --> Unicam / RP1 CFE --> DMA into
  capture buffers --> /dev/videoN (V4L2 streaming) --> DMABUF --> encoder -->
  H.264 elementary stream --> recorder / RTMP / WebRTC
```

### 2.1 Control-plane operations

Each operation below is what the in-tree driver does, according to its source. None of it has been observed on PACSCORDER hardware.

| Operation | Interface | Driver / receiver behaviour | Facts |
|---|---|---|---|
| Load EDID | `VIDIOC_SUBDEV_S_EDID`, pad 0, at most 8 blocks of 128 bytes | Drops HPD, writes the EDID RAM, then re-enables HPD only if source +5V is present. Writing 0 blocks clears the EDID. | [B-22], [A-32] |
| Hot-plug (HPD) | Driver-internal | HPD is never raised until an EDID has been written; it is raised after HZ/7 jiffies once an EDID and +5V are both present. Per the driver comment, DDC access to the EDID is gated by HPD. | [A-33], [B-21] |
| Detect timings | `QUERY_DV_TIMINGS` | Returns `-ENOLINK` (HPD low or no TMDS), `-ENOLCK` (no stable sync) or `-ERANGE` (outside the capability). | [B-29] |
| Apply timings | `S_DV_TIMINGS` | Stores the timings, mutes the stream and reprograms PLL and CSI, recomputing the lane count. It does **not** compare that lane count with the Device Tree `data-lanes`. | [B-30] |
| Timing capability | `DV_TIMINGS_CAP` | 640–1920 × 350–1200, 13–165 MHz pixel clock, progressive only | [B-26] |
| Media-bus format | sub-device `set_fmt`, pad 0 | Changes only the media-bus code; the frame size always comes from the current DV timings; the field is always `V4L2_FIELD_NONE`. | [B-35] |
| Pixel format | `VIDIOC_S_FMT` on the video node | Changes only the pixel format; timings are set through DV timings | [C-37] |
| Source change | `V4L2_EVENT_SOURCE_CHANGE`, subscribed on the sub-device | Sent on a sync change or a DE size/position change, when a sub-device node exists. The driver mutes the stream but does **not** apply new timings itself. | [B-39], [B-38] |
| Source +5V lost | Driver-internal | Drops HPD and zeroes the stored timings | [B-23] |
| Status polling | Driver-internal timer | Without an IRQ, the interrupt status is polled over I2C every 1000 ms, or every 10 ms when a CEC adapter is registered. The stock overlay has no `interrupts` property. | [A-30], [B-19], [C-20] |
| Lane check | Receiver at stream start | Unicam and RP1 CFE call `get_mbus_config` at stream start. They fail with `-EINVAL` if the bridge requests more lanes than the receiver's DT endpoint provides; fewer lanes, such as 3 of 4, are accepted. | [C-16], [B-32] |
| Status controls | V4L2 controls on the sub-device | `V4L2_CID_DV_RX_POWER_PRESENT`, plus read-only "Audio sampling rate" and "Audio present". There is no `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE`. | [B-16] |

**Which node carries EDID and DV-timings ioctls** depends on the receiver mode [B-25] (CORRECTED), [C-36] (CORRECTED):

| Platform and mode | EDID / DV-timings ioctls go to | Sub-device node |
|---|---|---|
| Pi 4/CM4, legacy video-node mode (stock overlay default) | The video node, which forwards them to the TC358743 sub-device | Registered read-only |
| Pi 4/CM4, Media Controller mode | The TC358743 sub-device node `/dev/v4l-subdevN`; the video node has no EDID or DV-timings ioctls | Read-write |
| Pi 5/CM5 (always Media Controller) | The TC358743 sub-device node; the CFE driver has no EDID or DV-timings handling | Read-write (reasoning: these ioctls can only be issued there [B-25]; the Pi 5 sequence reported by a Raspberry Pi engineer issues them on `/dev/v4l-subdevN` [C-33], community) |

ADR-006 (PROPOSED) proposes Media Controller mode on every platform, so that one control path serves all four.

Event delivery on Pi 5: an open issue reports that `rp1-cfe` video nodes do not deliver `V4L2_EVENT_SOURCE_CHANGE`. A Raspberry Pi engineer replied that applications should subscribe on the source sub-device instead [C-42] (reported by a community source; OQ-051).

### 2.2 Data-plane path

| Stage | What carries the data | Facts |
|---|---|---|
| HDMI → TC358743 | TMDS up to 165 MHz; input up to 1080p60 | [A-07] |
| TC358743 → receiver | CSI-2 over D-PHY, 1–4 data lanes, up to 1 Gbps per lane at the transmitter. The driver picks the active lane count at runtime from active pixels × fps × bits per pixel ÷ lane rate. | [A-05], [A-25] (CORRECTED), [B-31] |
| Receiver → memory | Pi 4/CM4: Unicam, `videobuf2-dma-contig`, no IOMMU, CMA. Pi 5/CM5: RP1 CSI nodes sit behind `iommu5`, and it is not established whether CFE buffers count against CMA. | [C-53] (CORRECTED; reasoning from kernel source) |
| Memory → userspace | V4L2 streaming on the capture video node; UYVY is mapped to `V4L2_PIX_FMT_UYVY` on both receivers | [C-34] |
| Capture → encoder | Pi 4/CM4: DMABUF import into `/dev/video11` (target; zero-copy from Unicam unproven, OQ-058). Pi 5/CM5: CPU encoder after UYVY-to-planar conversion. | [D-19], [D-43] |
| Encoder → outputs | H.264 elementary stream to a muxer or packetiser | [F-31], [F-36] |

---

## 3. Layers

Each subsection gives: what the layer is; what implements it on Pi 4/CM4 and on Pi 5/CM5; its control-plane and data-plane roles; its key constraints; the decisions that govern it; and its implementation status.

### 3.1 Layer 1 — Hardware

**What it is.** The layer has five parts:

- the HDMI source: a Blackmagic ATEM switcher's HDMI output or a camera connected directly (REQ-CAP-008, DRAFT; owner, 2026-10-07; models OQ-102);
- the HDMI connector;
- the bridge board carrying the TC358743 and its reference-clock oscillator;
- the CSI-2 cable or connector;
- the Raspberry Pi board, or a Compute Module plus its carrier board.

Audio wiring (I2S, the audio output the Linux driver configures [A-13]) is a separate side path (section 3.10).

**PACSCORDER hardware.** None exists and none has been chosen (owner, 2026-10-06). Every PACSCORDER-specific hardware fact is unknown:

| Item | PACSCORDER value | Resolution | OQ |
|---|---|---|---|
| Product platform for each lane configuration (2-lane and 4-lane, REQ-CAP-007) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED | OQ-011 (ADR-004) |
| HDMI source models: ATEM switchers and cameras (REQ-CAP-008) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-102 |
| Bridge board / carrier / number of HDMI inputs | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED | OQ-018 |
| CSI-2 lanes routed, connector, cable, for each configuration; whether one bridge-board design serves both the 2-lane and the 4-lane configuration | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-021 |
| REFCLK oscillator frequency | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-019 |
| INT and RESETN wiring, GPIO numbers | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-020 |
| TC358743 I2C address and bus sharing | UNKNOWN — VERIFICATION REQUIRED | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-026 |
| VDDIO2 voltage, HPD and +5V interface | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; DATASHEET REQUIRED | OQ-024 |
| Use of the camera-connector CAM_GPIO pin | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-022 |
| Power input and budget | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-023 |
| Audio I2S wiring | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-025 |
| Recording storage, Ethernet, USB | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; DATASHEET REQUIRED (Raspberry Pi Ethernet, USB and storage facts) | OQ-018, OQ-006, OQ-098 |

**Raspberry Pi side (from sources, not PACSCORDER measurements):**

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Camera connector(s) | One 15-pin, 1.0 mm pitch, 2 data lanes [C-01] | CAM0 2 lanes, CAM1 4 lanes [C-02]; 22-pin, 0.5 mm connectors on the CM4 IO Board [C-03] | Two 22-pin, 0.5 mm CSI/DSI ports, each 4-lane, 1.5 Gbps per lane [C-04] | Two 4-lane MIPI interfaces, MIPI0 on the CM4 CAM1 pins [C-05]; two 22-pin CAM/DISP connectors on the CM5 IO Board [C-06] |
| Camera I2C | `/dev/i2c-10` on GPIO 44/45 [C-24] | CAM1 `/dev/i2c-10` (GPIO 44/45), CAM0 `/dev/i2c-0` (GPIO 0/1) on the CM4 IO Board [C-25] | CAM/DISP0 `/dev/i2c-10`, CAM/DISP1 `/dev/i2c-11` [C-26] | Depends on the IO board [C-27] |

**Hardware constraints from sources:**

- **Reference clock.**
  - The Pi does not generate the TC358743 reference clock: the overlay declares a fixed 27 MHz clock, so the bridge board must carry its own oscillator [A-45], [B-47] (reasoning).
  - The driver accepts only 26, 27 or 42 MHz [B-07].
  - Any other rate leads to a kernel `BUG_ON` rather than a clean probe failure [A-22] (CORRECTED) — RISK-007.
- **INT and RESETN.** The stock overlay has neither `interrupts` nor `reset-gpios` [B-41]. Both are optional in the binding and the driver [A-44], [A-28], [B-19].
- **Adapter wiring.** A Raspberry Pi engineer reported that a wrongly sided 22-to-15-pin adapter on Pi 5 swaps GND and 3V3 and can damage either board [C-45] (reported; community source) — RISK-021.
- **Operating temperature.** The TC358743XBG is rated −30 to +70 °C ambient [A-41].

**Control plane.** I2C to the TC358743, plus the optional INT and RESETN GPIOs. On the HDMI side, the HPD and DDC/EDID signals face the source.

**Data plane.** HDMI TMDS in, CSI-2 lanes out.

**Key constraints.** RISK-001 (lane count), RISK-004 (TC358743 supply), RISK-007 (refclk), RISK-021 (third-party board wiring). Source behaviour per ATEM or camera model (HDCP, interlace, colour format) is UNKNOWN — VERIFICATION REQUIRED (OQ-102; RISK-008, RISK-009).

**Decisions.** ADR-004 (OPEN; since 2026-10-07 a platform choice per lane configuration, REQ-CAP-007).

**Implementation status.** NOT STARTED. TEST-HW-001: BLOCKED — HARDWARE REQUIRED.

### 3.2 Layer 2 — TC358743 HDMI-to-CSI-2 bridge

**What it is.** The Toshiba TC358743XBG is an HDMI-RX to MIPI CSI-2-TX bridge [A-01]:

- **HDMI receiver:** HDMI 1.4, with no Audio Return Channel or HDMI Ethernet Channel [A-02]. TMDS clock up to 165 MHz; video input up to 1080p60 [A-07].
- **CSI-2 transmitter:** up to 4 data lanes at up to 1 Gbps per lane [A-05].
- **EDID:** 1 KB EDID SRAM [A-31].
- **HDCP:** listed only as "optional" [A-03].
- **Audio:** output on I2S/TDM [A-11], or over CSI-2 [A-05]. The Linux driver always configures 2-channel I2S output [A-13].

**What implements it.** The kernel driver is the same on all four platforms:

- **Driver:** the in-tree driver `drivers/media/i2c/tc358743.c`. It is identical in mainline Linux and in Raspberry Pi's `rpi-6.18.y` apart from one formatting difference [A-48], [B-02].
- **Packaging:** both Raspberry Pi kernels build it as module `tc358743`, and the 2026-10-06 Raspberry Pi OS image ships it [G-16].
- **Matching:** it matches Device Tree compatible `toshiba,tc358743` [A-20].
- **Media entity:** a V4L2 sub-device with one source pad, entity function `MEDIA_ENT_F_VID_IF_BRIDGE`, and the device-node and events flags set [B-18].
- **Formats:** it offers exactly two media-bus codes, `MEDIA_BUS_FMT_RGB888_1X24` (the probe default) and `MEDIA_BUS_FMT_UYVY8_1X16` [B-34].
- **Initial timings:** it starts at 640x480p59.94 [C-19].

| | Pi 4 / CM4 | Pi 5 / CM5 |
|---|---|---|
| Overlay | `tc358743` (parameters `4lane`, `link-frequency`, `media-controller`, `cam0`) [G-12] | `dtoverlay=tc358743` is redirected to `tc358743-pi5` by the overlay map (parameters `4lane`, `link-frequency`, `cam0`; no `media-controller`) [C-11], [E-43], [G-13] |
| Where its controls are reached | Video node (legacy mode) or sub-device node (Media Controller mode) [B-25] (CORRECTED) | Sub-device node only [B-25] (CORRECTED) |

**Control plane.** EDID, DV timings, formats, events and polling, as listed in section 2.1. HDCP is always disabled on Device Tree platforms [A-04].

**Data plane.**

- The driver computes the active lane count at runtime and reports it to the receiver through `get_mbus_config`, without clamping to the DT `data-lanes` [A-25] (CORRECTED), [B-31].
- Every stream-on forces continuous-clock mode, even though the overlay sets `clock-noncontinuous` [B-36], [B-37] (OQ-037).

**Key constraints.**

| Constraint | Evidence | Risk / OQ |
|---|---|---|
| No video until userspace writes an EDID: after every probe no EDID is stored (`edid_blocks_written == 0`) and HPD is not raised until one is written. The EDID is held in the chip's 1 KB embedded EDID SRAM. That the driver loads no default EDID is a *research gap*, topic B, not a register fact. | [A-33], [B-21] (CORRECTED), [A-31] | RISK-010, REQ-CAP-003, OQ-093 |
| Interlaced input is rejected with `-ERANGE` (reasoning from source) | [B-27] | RISK-009, REQ-CAP-005 |
| Signal changes are detected by a 1000 ms poll unless INT is wired | [A-30], [B-19] | RISK-013, OQ-020 |
| FIFO trigger level hard-coded at 374 | [B-12] | RISK-006, OQ-035 |
| Fractional frame rates (59.94) are reported as integer-rate pixel clocks | [B-28] | OQ-040 |
| UYVY output is BT.601 limited range, reported as `SMPTE170M` [B-34]. That this also applies to HD sources is a *research gap*, topic B, not part of [B-34]. | [B-34] | OQ-041 |
| HDCP-protected sources: the driver always disables HDCP authentication | [A-04] | RISK-008, OQ-028 |
| No public register map; the driver was written against NDA documents | [A-42] | RISK-005, OQ-027 |

**Decisions.**

- ADR-001 (ACCEPTED): kernel V4L2 / Media Controller.
- ADR-002 (PROPOSED): use the in-tree driver as the baseline (OQ-013).
- ADR-005 (PROPOSED): UYVY by default (OQ-003).

**Implementation status.** NOT STARTED. TEST-DRV-001, TEST-DRV-002 and TEST-CAP-001: BLOCKED — HARDWARE REQUIRED.

### 3.3 Layer 3 — CSI-2 link

**What it is.** MIPI CSI-2 over D-PHY [A-06], with 1 to 4 data lanes [A-05] and one clock lane (`clock-lanes = <0>` in the binding) [A-44].

**Link configuration.** The link is described by the TC358743 Device Tree endpoint: `data-lanes`, `clock-lanes`, `clock-noncontinuous` and `link-frequencies` [A-44].

- **Raspberry Pi overlay:**
  - `link-frequencies` 486000000, which is 972 Mbps per lane;
  - `data-lanes <1 2>` by default;
  - a `4lane` override that switches both endpoints (the TC358743 endpoint and `csi1_ep`) to `<1 2 3 4>`

  [A-45], [C-12], [B-42].
- **Driver:** the lane rate is 2 × link-frequency. The driver has register (PHY) timing tables only for 594 and 972 Mbps per lane; any other rate logs "untested bps per lane" and falls back to the 594 Mbps values [A-24], [C-14].

**Per platform:**

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Lanes available to the bridge | 2; the DT limits `csi1` to 2 lanes [C-01], [B-48] | CAM1 4, CAM0 2 [C-02], [C-08] | 4 per port [C-04] | 4 per MIPI interface [C-05] |
| Configuration candidate under REQ-CAP-007 (ADR-004, OPEN) | 2-lane | CAM1: 4-lane; CAM0: 2-lane | 4-lane | 4-lane |
| Receiver lane-rate limit | Up to 1 Gbit/s per lane (maximum link frequency 500 MHz) [C-07] | As Pi 4 [C-07] | 1.5 Gbps per lane; 8 Gbps total across RP1's two D-PHYs [C-30] | As Pi 5 [C-30] |
| Receiver D-PHY setting for this bridge | — | — | Always 999 Mbps, because CFE finds no link-rate control [C-31] | As Pi 5 [C-31] |

**Bandwidth.** This table is **reasoning** from the research register:

- inputs: the driver's lane formula [B-31] and the CEA-861 frame timings;
- payload figures [C-46];
- lanes requested [C-47];
- feasibility on 2 lanes [C-48] and on 4 lanes [C-49].

All values assume the default 972 Mbps per lane.

| Mode | CSI-2 payload (Gbit/s) | Lanes the driver requests | On a 2-lane link | On a 4-lane link |
|---|---|---|---|---|
| 1080p60 UYVY | 1.991 | 3 | Not feasible (102.4 %) | Fits by bandwidth, but the driver activates only 3 of the 4 lanes; capture on 3 of 4 lanes is unproven (OQ-038; ADR-008) |
| 1080p60 RGB888 | 2.986 | 4 | Not feasible (153.6 %) | Fits (76.8 %) |
| 1080p50 UYVY | 1.659 | 2 | Feasible (85.3 %) | Fits |
| 1080p50 RGB888 | 2.488 | 3 | Not feasible (128 %) | Fits; image corruption reported (see below) |
| 1080p30 UYVY | 0.995 | 2 | Feasible (51.2 %) | Fits |
| 1080p30 RGB888 | 1.493 | 2 | Feasible (76.8 %) | Fits |

Official Raspberry Pi documentation agrees with these limits [C-37]:

- 2 lanes give at most 1080p30 RGB888 or 1080p50 YUV422.
- 4 lanes on a Compute Module give 1080p60 in either format.

A 4-lane port is therefore necessary for 1080p60, but it is not shown to be sufficient for 1080p60 UYVY: at 972 Mbit/s per lane the driver activates 3 of the 4 lanes [C-47], [C-49], which is unproven (OQ-038). ADR-008 (PROPOSED) proposes evaluating 297 MHz (594 Mbit/s, 4 active lanes) for this mode on a CM4 CAM1 4-lane link only, in TEST-CAP-002 (OQ-099).

**Both configurations are required (REQ-CAP-007, DRAFT; owner, 2026-10-07; OQ-001 ANSWERED).** PACSCORDER must support a 2-lane and a 4-lane link, each capturing every frame rate it can carry. Reasoning from the table above:

- **2-lane configuration** (Pi 4 Model B, CM4 CAM0, or a 2-lane bridge board on any port [C-48]; board lane count OQ-021): for 1920x1080, up to 1080p50 UYVY or 1080p30 RGB888 [C-37], [C-48]. 1080p60 is not carried in either format.
- **4-lane configuration** (CM4 CAM1, Pi 5, CM5): all six 1080p30/50/60 × UYVY/RGB888 combinations fit by bandwidth [C-49]; 1080p60 is required here. 1080p60 UYVY on 3 of 4 lanes is unproven (OQ-038; ADR-008).

The supported-mode matrix for each configuration is OQ-002 and is measured in TEST-CAP-004; 1080p60 capture is TEST-CAP-002.

A stricter per-line check (reasoning) leaves 2-lane 1080p50 UYVY about 2.0 µs of slack per line, before LP/HS overhead [C-50]. Separately, Toshiba's spreadsheet minimum of 898.12 Mbps per lane for this mode, as read by a Raspberry Pi engineer, leaves about 7.6 % margin at 972 Mbps [C-50] (reasoning).

**Control plane.** Userspace does not set the lane count. The driver derives it from the timings and format [B-31], and the receiver checks it against its DT endpoint at stream start [C-16]. The configured maximum (DT `data-lanes`, switched by the `4lane` overlay parameter [C-12]) must match each configuration's wiring: on Pi 4/CM4, an endpoint that lists more lanes than the Unicam instance supports does not fail the probe; the driver only logs a message and adopts the endpoint count [C-17] ([DEVICE_TREE.md](DEVICE_TREE.md)).

**Key constraints.**

- **RISK-001:** 1080p60 needs more than 2 lanes. Reasoning: the Pi 4 Model B and CM4 CAM0 cannot carry it [C-48]. Under REQ-CAP-007 they remain candidates for the 2-lane configuration, whose limit is 1080p50 UYVY / 1080p30 RGB888 [C-37]. A 4-lane port is necessary but not shown sufficient for 1080p60 UYVY, which uses 3 of 4 lanes at 972 Mbit/s (OQ-038; ADR-008).
- **RISK-006:**
  - An open issue reports corrupted images at 1080p50 RGB24 on a 4-lane CM4, where the driver chose 3 lanes.
  - A Raspberry Pi engineer attributed this to the hard-coded FIFO trigger level [C-43] (reported; community source).
  - OQ-038 covers capture on 3 of 4 lanes in general.
- **RISK-011:**
  - The Pi 5 CFE always programs 999 Mbps. Reasoning: that matches the default 972 Mbps link but not `link-frequency=297000000` (594 Mbps) [C-52].
  - Tracked as OQ-050. The link-frequency choice is ADR-008 (PROPOSED; OQ-099).
- **Clock-lane mode:** effective continuous or non-continuous behaviour is unresolved [B-37] (OQ-037).

**Decisions.**

- ADR-004 (OPEN): the platform fixes the lanes available; since 2026-10-07 it is a platform choice per lane configuration (REQ-CAP-007).
- ADR-005 (PROPOSED): UYVY needs two-thirds of RGB888's bandwidth (reasoning) [C-46].
- ADR-008 (PROPOSED): proposes keeping the overlay default `link-frequency` of 486 MHz (972 Mbit/s per lane) on all platforms, and evaluating 297 MHz only on a CM4 CAM1 4-lane link for 1080p60 UYVY in TEST-CAP-002. Owner decision: OQ-099.

**Implementation status.** NOT STARTED. TEST-CAP-002 and TEST-CAP-004: BLOCKED — HARDWARE REQUIRED.

### 3.4 Layer 4 — Raspberry Pi CSI-2 receiver

**What it is.** The receiver block that terminates the CSI-2 link and writes frames to memory by DMA: Unicam in the BCM2711 on Pi 4/CM4 [C-07], and the CFE in RP1 on Pi 5/CM5 [C-29], [C-30].

| | Pi 4 / CM4 (BCM2711) | Pi 5 / CM5 (BCM2712 + RP1) |
|---|---|---|
| Receiver | Unicam: two instances, the first 2-lane and the second 4-lane [C-07] | RP1 CFE: `rp1_csi0` and `rp1_csi1`, compatible `raspberrypi,rp1-cfe` [C-29] |
| Kernel driver that binds | Downstream `bcm2835-unicam.c`, module `bcm2835-unicam-legacy`. It binds `brcm,bcm2835-unicam` (Media Controller mode) and `brcm,bcm2835-unicam-legacy` (video-node mode). The mainline driver binds only `brcm,bcm2835-unicam-upstream` in this tree [C-09], [G-21]. | Downstream `rp1_cfe`, module `rp1-cfe-downstream`. The mainline-derived `rp1-cfe` binds only `raspberrypi,rp1-cfe-upstream`. Both are built as modules [C-29], [E-44]. |
| Mode selection | Base DT nodes use `brcm,bcm2835-unicam`, so the downstream driver binds in Media Controller mode [E-40] (CORRECTED). The `tc358743` overlay changes `csi1` to `-legacy` unless its `media-controller` parameter is set [C-10], [B-43]. | Media Controller only [C-11] |
| DT lane handling | Accepts 1, 2 or 4 lanes on the endpoint. If the endpoint lists more lanes than the instance supports, it only logs a message and adopts the endpoint count [C-17]. | — |
| Buffer memory | `videobuf2-dma-contig`, no IOMMU, allocated from CMA [C-53] (CORRECTED; reasoning from kernel source) | RP1 CSI nodes are behind `iommu5`; CMA use not established [C-53] (CORRECTED; reasoning from kernel source) — OQ-053 |
| UYVY8_1X16 | `V4L2_PIX_FMT_UYVY` [C-34] | `V4L2_PIX_FMT_UYVY` [C-34] |
| RGB888_1X24 | `V4L2_PIX_FMT_RGB24` [C-34] | `V4L2_PIX_FMT_BGR24` [C-34] — RISK-016 |

**Common behaviour.** Both receivers call `get_mbus_config` at stream start. Both reject a request for more lanes than configured, with "Device has requested %u data lanes, which is >%u configured in DT" [C-16], [B-32].

**Control plane.**

- Pi 4/CM4: the receiver mode decides where EDID and DV-timings ioctls go (section 2.1).
- Pi 5/CM5: links must be enabled by userspace (section 3.5).

**Data plane.** DMA from the CSI-2 receiver into V4L2 capture buffers.

**Key constraints.**

- RISK-011 and RISK-012:
  - The official Raspberry Pi TC358743 documentation covers only the Unicam path [C-38].
  - The Pi 5 path rests on community reports [C-33], [C-41] (both community).
- OQ-044 and OQ-049.

**Decisions.** ADR-004 (OPEN); ADR-006 (PROPOSED).

**Implementation status.** NOT STARTED. TEST-PLT-001: BLOCKED — HARDWARE REQUIRED.

### 3.5 Layer 5 — Media Controller

**What it is.** The kernel's graph of media entities, pads and links, reached through `/dev/mediaN`. Userspace uses it to:

- enable links;
- set the format on each pad;
- find the device node behind each entity.

**Pi 4 / CM4.**

- **Legacy video-node mode** (the stock overlay's default) [C-10]:
  - EDID and DV-timings ioctls are forwarded by the video node;
  - sub-device nodes are registered read-only [C-36] (CORRECTED).
- **Media Controller mode:**
  - the capture node `unicam-image` gets an `IMMUTABLE|ENABLED` link straight from the TC358743 source pad;
  - there is no CSI-2 receiver sub-device in between;
  - because the TC358743 has a single pad, only `unicam-image` is registered (no `unicam-embedded`) [C-36] (CORRECTED).

```text
Pi 4 / CM4, Media Controller mode (ADR-006, PROPOSED)

  "tc358743 <bus>-000f" pad 0 (source) --IMMUTABLE|ENABLED--> "unicam-image" --> /dev/videoN
          |
          +-- /dev/v4l-subdevN (read-write in this mode)
```

**Pi 5 / CM5.**

- The CFE registers a `csi2` sub-device with sink pads 0–3 and source pads 4–7.
- Sensor-to-`csi2` links are created `IMMUTABLE|ENABLED`.
- `csi2` source pads link to video nodes named `rp1-cfe-<node>` and to `pisp-fe`, but those links start **disabled**. Userspace must enable `csi2:4 → rp1-cfe-csi2_ch0` to write frames to memory [C-32].
- A Raspberry Pi engineer reported a capture sequence on kernel 6.18.39 [C-33] (reported; community source; 6.18.39 is the kernel of that report, while Raspberry Pi OS 2026-10-06 ships 6.18.50 [G-04]) that, after setting the EDID and DV timings on the TC358743 sub-device node:
  1. enables that link;
  2. sets matching formats on the TC358743 pad and on `csi2` pads 0 and 4, with `field:none` and a colorspace;
  3. sets the video-node pixel format.

  The issue reporter found that leaving out `field:none` on the `csi2` pads makes STREAMON fail with `-EPIPE`.

```text
Pi 5 / CM5 (Media Controller only)

  "tc358743 1x-000f":0 --IMMUTABLE|ENABLED--> "csi2":0
                                              "csi2":4 --(disabled; userspace enables)--> "rp1-cfe-csi2_ch0":0 --> /dev/videoN
                                              "csi2":4..7 --(disabled)--> other rp1-cfe-* nodes, "pisp-fe"
          |
          +-- /dev/v4l-subdevN (EDID, DV timings, source-change events)
```

**Entity names are not constant.** The TC358743 entity name contains the I2C bus number. On 6.18 kernels it appears as `tc358743 11-000f` on Pi 5 CAM/DISP1 or `tc358743 10-000f` on CAM/DISP0. Earlier Pi 5 kernels used other bus numbers (reported by a Raspberry Pi engineer and corrected against the 6.18 Device Tree) [C-28] (CORRECTED, community). The `-000f` suffix in the diagrams assumes the stock overlay's I2C address 0x0f [B-41]; the PACSCORDER board's address is UNKNOWN — VERIFICATION REQUIRED, DATASHEET REQUIRED; HARDWARE TEST REQUIRED (OQ-026). Software must therefore discover entity names and device nodes at runtime (OQ-043).

**Decisions.**

- ADR-001 (ACCEPTED).
- ADR-006 (PROPOSED): use Media Controller mode everywhere, so one graph-configuration procedure serves all four platforms (OQ-014).

**Implementation status.** NOT STARTED. TEST-PLT-001: BLOCKED — HARDWARE REQUIRED.

### 3.6 Layer 6 — V4L2

**What it is.** The Video4Linux2 device nodes and ioctls through which userspace configures the bridge and streams frames.

- **TC358743 sub-device node `/dev/v4l-subdevN`** [B-18]. Its operations [B-38] are:
  - EDID get/set;
  - DV timings query, set, get, enumerate and capability;
  - `get_mbus_config`;
  - `enum_mbus_code`, `set_fmt`, `get_fmt`;
  - `subscribe_event` for `V4L2_EVENT_SOURCE_CHANGE` and `V4L2_EVENT_CTRL`;
  - `log_status`.

  There is no `enum_frame_size` or `enum_frame_interval` [B-38].
- **Capture video node `/dev/videoN`:** `unicam-image` on Pi 4/CM4 [C-36], `rp1-cfe-csi2_ch0` on Pi 5/CM5 [C-32]. Pixel format is UYVY for `UYVY8_1X16`; on Pi 5, `BGR3` is used for `RGB888_1X24` [C-33] (reported; community source), [C-34].

**Why plain V4L2 and not libcamera.** The TC358743 is not a raw sensor and registers no `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE` control [B-16]. Raspberry Pi engineers reported that libcamera does not support it [C-41] (reported; community source). ADR-001 (ACCEPTED) records V4L2.

**Reference commands.** Only the command forms attested in the sources are named here. Procedures live in [V4L2.md](V4L2.md) and [TESTING.md](TESTING.md). **NOT YET RUN ON PACSCORDER HARDWARE.**

| Purpose | Attested form | Facts |
|---|---|---|
| Load an EDID | `v4l2-ctl --set-edid pad=<pad>[,type=<type>\|file=<file>][,format=<fmt>]` | [B-24], [C-37] |
| Apply detected timings | `v4l2-ctl --set-dv-bt-timings query` | [C-37], [C-33] (Pi 5 sequence reported; community source) |
| Enable the Pi 5 capture link | `media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'` | [C-33] (reported; community source) |
| Set a Pi 5 pad format | `media-ctl -d <N> -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'` | [C-33] (reported; community source) |

In the source evidence of [C-33], the reported sequence selects the media device with `-d 2`. The media device number on PACSCORDER is platform-specific: NEEDS VERIFICATION on hardware (OQ-043).

**Key constraints.**

- RISK-013 (polling).
- RISK-016 (RGB888 pixel-format label differs between receivers: `RGB24` on Unicam, `BGR24` on CFE [C-34]).
- OQ-046: whether Unicam in Media Controller mode enumerates UYVY.

**Decisions.** ADR-001 (ACCEPTED); ADR-006 (PROPOSED).

**Implementation status.** NOT STARTED. TEST-CAP-001 and TEST-CAP-003: BLOCKED — HARDWARE REQUIRED.

### 3.7 Layer 7 — DMABUF

**What it is.** The Linux mechanism for sharing a buffer between devices by passing a file descriptor. REQ-DMA-001 requires frames to pass from capture to encoder without a CPU copy, where the encoder can import DMABUFs.

**Pi 4 / CM4: hardware import into the encoder.**

Encoder-side requirements:

- Both queues of the `bcm2835-codec` M2M devices support `VB2_MMAP | VB2_DMABUF` through `videobuf2-dma-contig` [D-19].
- An imported DMABUF must be one DMA-contiguous region at least as large as the plane [D-20].
- The codec always uses one memory plane [D-21].
- For UYVY the encoder rounds `bytesperline` up to a multiple of 64 bytes [D-22].

Capture side: Unicam allocates capture buffers with `videobuf2-dma-contig` from CMA, without an IOMMU [C-53] (CORRECTED; reasoning from kernel source).

Prior art:

- `rpicam-apps` imports camera DMABUFs into `/dev/video11` [D-34].
- GStreamer's V4L2 M2M elements offer a `dmabuf-import` I/O mode [D-53].
- Raspberry Pi OS's patched FFmpeg adds DMABUF input to the V4L2 M2M encoder [D-45]; upstream FFmpeg uses MMAP only and copies every frame [D-44].
- Whether Unicam buffers import into the codec without a copy for every supported width is **unproven** (OQ-058).

**Pi 5 / CM5: no hardware encoder to import into.**

- `libx264` and `x264enc` do not accept packed UYVY, so each frame must be converted to a planar format such as I420 or NV12 before encoding [D-43], [D-40].
- Reasoning: with no hardware encoder [D-31], the DMABUF layer on these platforms means sharing capture buffers with the converter or software encoder; there is no hardware encoder to import into (REQ-DMA-001 notes).
- Whether a BCM2712 hardware block can do the conversion is open (OQ-060).

**Memory sizing.**

- **CMA pool:** the Device Tree CMA pool is 64 MB, limited to the lower 768 MB on Pi 4/CM4 and the lower 1 GB on Pi 5 [E-47] (CORRECTED).
- **Overlay defaults:** `vc4-kms-v3d-pi4` sets (512 − 4) MB and `vc4-kms-v3d-pi5` sets 64 MB [C-40].
- **Reasoning:** four 1920x1080 UYVY capture buffers take about 16.6 MB [C-53] (CORRECTED).
- **DMA-BUF heaps** are enabled in the Raspberry Pi kernels [G-19]. Heap names differ between kernel series [E-48] (OQ-062).

**Key constraints.** RISK-020 (CMA), OQ-061.

**Decisions.** ADR-007 (OPEN): the framework decides how buffers are passed. ADR-005 (PROPOSED): UYVY.

**Implementation status.** NOT STARTED. TEST-DMA-001: BLOCKED — HARDWARE REQUIRED.

### 3.8 Layer 8 — Encoder

**What it is.** The stage that compresses captured video to H.264. REQ-ENC-001 leaves these parameters UNDEFINED (OQ-005):

- codec;
- bitrate;
- latency;
- number of simultaneous encodes.

| | Pi 4 / CM4 | Pi 5 / CM5 |
|---|---|---|
| Implementation | Hardware: `bcm2835-codec`, a downstream-only V4L2 M2M driver [D-05]. Encode node `/dev/video11` [D-06], [D-07]. Backed by the VideoCore firmware component `ril.video_encode` over VCHIQ [D-08]. | **No hardware video encoder** [D-31], [G-22]. Reasoning: `bcm2835-codec` cannot probe on Pi 5/CM5, because the BCM2712 Device Tree has no VCHIQ node [D-29], [D-28]. |
| Official capability | H.264 1080p30 encode [D-10] | "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22] |
| Limits | Hardware specification is level 4.0 [D-12]. Bitrate 25 kbit/s – 25 Mbit/s, VBR or CBR [D-13]. No B-frames; GOP 60 by default [D-14]. Frame size at most 1920x1920 [D-09]. Profiles: Baseline, Constrained Baseline, Main, High [D-11]. | Software encoders usually output frames with more latency than the old hardware encoders [D-32] |
| Input format | Read from the firmware at probe; the static table includes UYVY [D-16]. A Raspberry Pi engineer listed UYVY among 13 accepted input formats [D-17] (reported; community source) and reported that both TC358743 formats are accepted directly [D-18] (reported; community source). | `x264enc` / `libx264` need planar input, not UYVY [D-40], [D-43] |
| 1080p60 | **Unproven.** Reasoning: 1080p60 needs 2.0x the specified macroblock rate [D-52]. A Raspberry Pi engineer (6by9) reported that 1080p60 was an "edge case" on the hardware encoder [D-50] (reported; community source). | A Raspberry Pi engineer (6by9) reported that software 1080p60 encode from camera capture is "easily achievable" [D-50] (reported; community source). Not measured with TC358743 input. No official 1080p60 figure. |
| Prerequisites | Non-cut-down GPU firmware: the cut-down firmware removes codec support [D-48] (OQ-048) | CPU and thermal headroom (OQ-059) |
| Risk | RISK-002, OQ-056 | RISK-003, OQ-059, OQ-060 |

**Common to all four.**

- No candidate has a hardware HEVC encoder [D-24], [D-31].
- Using `libx264` through FFmpeg requires a GPL build (`--enable-gpl`) [D-42]; x264 itself is GPL [D-47] — RISK-015, OQ-086, OQ-087.

**Decisions.** ADR-004 (OPEN); ADR-007 (OPEN).

**Implementation status.** NOT STARTED. TEST-ENC-001: BLOCKED — HARDWARE REQUIRED.

### 3.9 Layer 9 — Recorder / RTMP / WebRTC

**What it is.** The userspace consumers of the encoded stream. Their design responsibilities are in [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md).

| Output | Requirement | Constraints from sources | Open |
|---|---|---|---|
| Recorder | REQ-REC-001 | Container, storage medium, duration and power-loss behaviour are UNDEFINED. Debian trixie's `gstreamer1.0-plugins-good`, which the Raspberry Pi archive does not override, ships the `isomp4` (MP4), `matroska` and `flv` plugins [G-26]. It is not installed in the Lite image [G-32]. | OQ-006 |
| RTMP | REQ-STR-001 | Legacy RTMP/FLV carries H.264 and AAC; HEVC and Opus need Enhanced RTMP [F-31]. GStreamer `flvmux` needs AVC-format H.264 and raw AAC [F-34] (CORRECTED). `rtmp2sink` publishes over RTMP and RTMPS [F-33]. | OQ-007, OQ-075, OQ-076 |
| WebRTC | REQ-STR-002 | Browsers must implement H.264 Constrained Baseline, with SPS/PPS in-band [F-36]. Reasoning: 1080p needs Level 4 or higher, which a `42e01f` (Level 3.1) negotiation does not cover [F-38], [F-40]. WebRTC endpoints must implement Opus and G.711; AAC is not a required WebRTC codec, so AAC audio has to be transcoded for browser playback [F-41]. `webrtcbin` has no built-in signalling [F-42]. | OQ-008, OQ-073, OQ-074 |

Whether one encode feeds all three outputs, or each gets its own encode, is UNDEFINED (OQ-005). **Reasoning:** a shared H.264 stream would have to meet the strictest consumer:

- WebRTC's Constrained Baseline profile [F-36];
- no B-frames — reported by the MediaMTX project as a browser limitation [F-45] (reported; community source).

The Pi 4/CM4 hardware encoder offers Constrained Baseline [D-11] and produces no B-frames [D-14].

**Decisions.** ADR-007 (OPEN).

**Implementation status.** NOT STARTED. TEST-REC-001, TEST-STR-001 and TEST-STR-002: BLOCKED — HARDWARE REQUIRED.

### 3.10 Paths outside the mandated stack

These connect to the stack but are not layers of Rule 5.

| Path | Where it attaches | What is known | Status |
|---|---|---|---|
| HDMI audio (REQ-CAP-006, PROPOSED; owner decision OQ-004) | Layer 1 wiring → ALSA → layer 9 muxers | The chip can output audio on I2S or TDM, or send it over CSI-2 [A-11], [A-05]; the Linux driver always configures 2-channel I2S output [A-13]. The `tc358743-audio` overlay expects that I2S on GPIO 18/19/20 and creates an ALSA card named `tc358743` [A-47], [G-14]. Reasoning: the I2S path is wiring separate from the CSI-2 cable. Behaviour on Pi 5/CM5 is unconfirmed (OQ-054). | NOT STARTED — RISK-014 |
| Blackmagic ATEM integration (REQ-ATEM-001; scope recorded 2026-10-07, OQ-009 ANSWERED: HDMI capture only) | Layer 1 only: the ATEM's HDMI output is an HDMI source, like a camera connected directly (REQ-CAP-008). In the current scope this is not a separate path. | ATEM Mini Pro HDMI output is 1080p only, with no 720p or 1080i [F-23], and defaults to multiview [F-25]. **Not in current scope** (kept as reference in [ATEM.md](ATEM.md)): network tally/control over UDP 9910, a protocol reported as reverse-engineered by the OpenSwitcher project [F-11] (reported; community source), for which the official SDK supports Windows and macOS only [F-01]; and RTMP exchange, where receiving an ATEM RTMP push would need a listening RTMP server on PACSCORDER (reasoning) [F-46]. Models: OQ-102. | NOT STARTED — RISK-008; RISK-018 applies only if network control is added |
| HDMI CEC (OQ-016) | Layer 2 | Optional in the driver behind `CONFIG_VIDEO_TC358743_CEC`, which the Raspberry Pi defconfigs do not enable [A-34], [B-20] | NOT STARTED; needed only if the owner requires CEC (OQ-016) |

---

## 4. Per-platform instantiation

The two diagrams below show the whole stack on each SoC family, from sources. Every PACSCORDER-specific box is unknown. Device-node numbers (`N`) and I2C bus numbers in entity names are discovered at runtime (OQ-043).

### 4.1 Pi 4 Model B and CM4 (BCM2711, Unicam, hardware encoder)

```text
 HDMI source: ATEM HDMI output or camera connected directly (REQ-CAP-008)
   |  TMDS <= 165 MHz [A-07]; per the driver comment, DDC access to the
   |  EDID is gated by HPD [B-21]
   v
 +--------------------------------------------------------------------------+
 | Bridge board: UNKNOWN - VERIFICATION REQUIRED (OQ-018)                   |
 |   TC358743XBG <-- REFCLK from an on-board oscillator; driver accepts     |
 |                   26/27/42 MHz, overlay declares 27 MHz [B-07], [A-45]   |
 |                   PACSCORDER oscillator frequency: UNKNOWN (OQ-019)      |
 |   INT / RESETN to Pi GPIOs: UNKNOWN (OQ-020)                             |
 +--------+-------------------------------------------+---------------------+
          | CSI-2 D-PHY, 972 Mbps/lane default [A-45]  | I2C, address 0x0f in public configs [A-15]
          | Pi 4B: 2 lanes [C-01]                      | Pi 4B / CM4 CAM1: /dev/i2c-10 (GPIO 44/45)
          | CM4: CAM1 4 lanes, CAM0 2 lanes [C-02]     | CM4 CAM0: /dev/i2c-0 (GPIO 0/1) [C-24], [C-25]
          v                                            v
 Unicam csi1 (or csi0 with "cam0")            tc358743 driver, module tc358743 [G-16]
 driver: bcm2835-unicam-legacy [C-09]         V4L2 sub-device, 1 source pad [B-18]
 DMA: videobuf2-dma-contig, CMA              -> /dev/v4l-subdevN: EDID, DV timings
 (reasoning from kernel source) [C-53]           in MC mode [B-25]; SOURCE_CHANGE events [B-38]
          |
          v
 Media graph, MC mode (ADR-006, PROPOSED):
   "tc358743 <bus>-000f":0 --IMMUTABLE|ENABLED--> "unicam-image" [C-36]
          |
          v
 /dev/videoN "unicam-image": V4L2 capture, V4L2_PIX_FMT_UYVY (ADR-005, PROPOSED) [C-34]
          |  DMABUF: one contiguous plane, 64-byte-aligned UYVY lines [D-19], [D-20], [D-21], [D-22]
          |  (optional conversion through the ISP M2M node /dev/video12 [D-25])
          v
 bcm2835-codec encoder /dev/video11 (VideoCore firmware via VCHIQ) [D-06], [D-08]
   H.264; official spec 1080p30 encode [D-10]; 1080p60 unproven (RISK-002)
          |  H.264 elementary stream
          v
 Recorder / RTMP / WebRTC: userspace, framework OPEN (ADR-007)
```

| | Pi 4 Model B | CM4 on CAM1 | CM4 on CAM0 |
|---|---|---|---|
| Connector and lanes | 15-pin, 2 lanes [C-01] | 22-pin on the CM4 IO Board, 4 lanes [C-03] | 22-pin, 2 lanes; on the CM4 IO Board both J6 jumpers must be fitted for I2C [C-03] |
| Receiver instance in DT | `csi1`, limited to 2 lanes [B-48], [C-08] | `csi1`, 4 lanes [C-08] | `csi0`, 2 lanes [C-08] |
| Overlay parameters (`config.txt` syntax for bare boolean and combined parameters: NEEDS VERIFICATION, OQ-100) | `media-controller` (ADR-006, PROPOSED) [G-12] | `media-controller` and `4lane` [G-12], [B-42] | `media-controller` and `cam0` [C-10] |
| Configuration under REQ-CAP-007 | 2-lane candidate | 4-lane candidate | 2-lane candidate |
| 1080p60 capture (bandwidth reasoning) | Not feasible [C-48]; the 2-lane configuration carries up to 1080p50 UYVY / 1080p30 RGB888 [C-37] | Feasible by bandwidth [C-49]; officially documented for a Compute Module with 4 lanes [C-37]. For UYVY at 972 Mbit/s the driver uses 3 of the 4 lanes, which is unproven (OQ-038; ADR-008). | Not feasible [C-48]; as Pi 4 Model B |
| Hardware encode | H.264, spec 1080p30 [D-10] | Same [D-10] | Same [D-10] |

### 4.2 Pi 5 and CM5 (BCM2712 + RP1, CFE, software encoder)

```text
 HDMI source: ATEM HDMI output or camera connected directly (REQ-CAP-008)
   |  TMDS <= 165 MHz [A-07]
   v
 +--------------------------------------------------------------------------+
 | Bridge board: UNKNOWN - VERIFICATION REQUIRED (OQ-018)                   |
 |   TC358743XBG <-- REFCLK from board oscillator (reasoning) [B-47]        |
 |                   PACSCORDER oscillator frequency: UNKNOWN (OQ-019)      |
 |   Pi 5 side is 22-pin [C-04]; bridge-board connector UNKNOWN (OQ-021).   |
 |   A wrongly sided 22-to-15-pin adapter is a reported damage hazard       |
 |   [C-45]                                                                 |
 +--------+-------------------------------------------+---------------------+
          | CSI-2 D-PHY, 4 lanes per port [C-04],      | I2C: Pi 5 CAM/DISP0 /dev/i2c-10 (RP1 i2c6, GPIO 38/39),
          | CM5 4 per MIPI interface [C-05];           | CAM/DISP1 /dev/i2c-11 (RP1 i2c4, GPIO 40/41) [C-26];
          | CFE D-PHY programmed at 999 Mbps [C-31]    | CM5: depends on the IO board [C-27]
          v                                            v
 RP1 CFE rp1_csi0 / rp1_csi1                  tc358743 driver, module tc358743 [G-16]
 driver: rp1-cfe-downstream [C-29]            V4L2 sub-device, 1 source pad [B-18]
 overlay: tc358743 -> tc358743-pi5 [C-11]     -> /dev/v4l-subdevN: EDID, DV timings [B-25];
 DMA behind iommu5; CMA use unknown              SOURCE_CHANGE events [B-38] (video node
 (reasoning from kernel source) [C-53]           reported not to deliver them [C-42])
          |
          v
 Media graph (always MC):
   "tc358743 1x-000f":0 --IMMUTABLE|ENABLED--> "csi2":0
   "csi2":4 --(userspace must enable)--> "rp1-cfe-csi2_ch0":0 [C-32]
   pad formats must match, with field:none (reported) [C-33]
          |
          v
 /dev/videoN "rp1-cfe-csi2_ch0": V4L2 capture, V4L2_PIX_FMT_UYVY (ADR-005, PROPOSED) [C-34]
          |  CPU access; UYVY -> I420/NV12 conversion per frame [D-43], [D-40]
          |  (hardware offload: OPEN, OQ-060)
          v
 Software H.264 encoder (x264 family; ADR-007 OPEN)
   official: 1080p30 encode ~30-40% CPU [G-22]; no hardware encoder [D-31]
          |  H.264 elementary stream
          v
 Recorder / RTMP / WebRTC: userspace, framework OPEN (ADR-007)
```

| | Pi 5 CAM/DISP0 | Pi 5 CAM/DISP1 | CM5 on CM5 IO Board | CM5 on CM4 IO Board |
|---|---|---|---|---|
| Connector and lanes | 22-pin, 4 lanes [C-04] | 22-pin, 4 lanes [C-04] | Two 22-pin CAM/DISP connectors. CAM/DISP1 needs two J6 jumpers for I2C and cannot power down a camera [C-06]. | CM5 MIPI0 uses the CM4 CAM1 pins; the CM4 CAM0 pins carry USB 3.0 [C-05] |
| Receiver | `rp1_csi0`. Reasoning: `cam0` retargets the CSI fragment to `csi0` [B-42], which is the label of `rp1_csi0` [C-29]. | `rp1_csi1`: the `csi1` label points to `rp1_csi1` [E-43] | `csi0`/`csi1` = `rp1_csi0`/`rp1_csi1` [B-44] (CORRECTED) | As CM5 [B-44] (CORRECTED) |
| Camera I2C | `/dev/i2c-10` [C-26] | `/dev/i2c-11` [C-26] | CAM/DISP1: RP1 i2c0 on GPIO 0/1 (`i2c-11`); CAM/DISP0: RP1 i2c6 on GPIO 38/39 [C-27] | `i2c_csi_dsi1` is i2c6 (shared with DISP1, RTC and fan), `i2c_csi_dsi0` is i2c0 [C-27] |
| Overlay selection | `cam0` parameter [G-13], [C-39]. Whether `4lane` together with `cam0` gives 4 lanes on `csi0`: NEEDS VERIFICATION, because `4lane` is defined on the TC358743 endpoint and `csi1_ep` [B-42], and the README text for `tc358743-pi5` is stale [C-13] (OQ-049) | Default connector [C-39] | As Pi 5 [C-39] | Overlay-to-connector pairing unresolved (OQ-052) |
| Configuration under REQ-CAP-007 | 4-lane candidate | 4-lane candidate | 4-lane candidate | 4-lane candidate |
| 1080p60 capture (bandwidth reasoning) | Feasible by bandwidth [C-49], only if `4lane` with `cam0` really gives 4 lanes on `csi0` (NEEDS VERIFICATION, OQ-049) | Feasible by bandwidth [C-49] | Feasible by bandwidth [C-49] | Feasible by bandwidth [C-49]; overlay pairing unresolved (OQ-052) |
| Official TC358743 documentation | None [C-38] | None [C-38] | None [C-38] | None [C-38] |

In every column, "feasible by bandwidth" means only that the 4-lane port is necessary and large enough. It is not shown to be sufficient for 1080p60 UYVY: at 972 Mbit/s per lane the driver activates 3 of the 4 lanes [C-47], [C-49], and capture on 3 of 4 lanes through RP1 CFE is unproven (OQ-038, OQ-049). See ADR-008 (PROPOSED) and OQ-099 for the link frequency.

A 2-lane bridge board on a Pi 5 or CM5 port would form a 2-lane configuration with the 2-lane limits of [C-48] (reasoning; [C-48] covers any 2-lane TC358743 board). Whether the PACSCORDER board is 2-lane or 4-lane is OQ-021.

### 4.3 Platform summary

| Aspect | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Lanes to the bridge | 2 [C-01] | CAM1 4, CAM0 2 [C-02] | 4 per port [C-04] | 4 per interface [C-05] |
| Configuration candidate under REQ-CAP-007 (platform per configuration: ADR-004, OPEN) | 2-lane | CAM1 4-lane; CAM0 2-lane | 4-lane | 4-lane |
| 1080p60 capture by bandwidth (reasoning; a 4-lane port is necessary, not shown sufficient for UYVY: 3 of 4 lanes at 972 Mbit/s, OQ-038, ADR-008) | No [C-48]; 2-lane limit 1080p50 UYVY / 1080p30 RGB888 [C-37] | CAM1 by bandwidth [C-49] | By bandwidth [C-49]; CAM/DISP0 4-lane configuration NEEDS VERIFICATION (OQ-049) | By bandwidth [C-49]; carrier-dependent (OQ-052) |
| Receiver / driver | Unicam / `bcm2835-unicam-legacy` [C-09] | Unicam / `bcm2835-unicam-legacy` [C-09] | RP1 CFE / `rp1-cfe-downstream` [C-29] | RP1 CFE / `rp1-cfe-downstream` [C-29] |
| Control model | Legacy (overlay default) or MC [C-10] | Legacy (overlay default) or MC [C-10] | MC only [C-11] | MC only [C-11] |
| Encoder | Hardware H.264, spec 1080p30 [D-10] | Hardware H.264, spec 1080p30 [D-10] | Software only [G-22] | Software only [D-31] |
| Official TC358743 documentation | Yes, 2-lane [C-37] | Yes, 4-lane Compute Module case [C-37] | None [C-38] | None [C-38] |
| Kernel in Raspberry Pi OS 2026-10-06 | 6.18.50, `rpi-v8` (4K pages) [G-04], [G-71] (CORRECTED; reasoning) | 6.18.50, `rpi-v8` [G-04], [G-71] (CORRECTED; reasoning) | 6.18.50, `rpi-2712` (16K pages) [G-04], [G-20] | 6.18.50, `rpi-2712` [G-04], [G-20] |

One Raspberry Pi OS image can boot all four candidates. Overlay, overlay parameters, control model, encode path and lane count still differ per board [G-71] (CORRECTED, reasoning).

**Product OS image (REQ-BLD-002, DRAFT; owner, 2026-10-07).** The product runs its own project-built OS image, not an unmodified stock image. ADR-003 (ACCEPTED 2026-10-07; OQ-012 ANSWERED) decides Raspberry Pi OS Lite for bring-up and `rpi-image-gen` for the product image, with Buildroot as the alternative. [DECISIONS.md](DECISIONS.md) chose `rpi-image-gen` because REQ-CAP-007 may put the two lane configurations on different board families (ADR-004, OPEN), and one Raspberry Pi OS Lite image ships both kernels with module trees that include `tc358743`, Unicam and RP1 CFE and boots all four candidate boards [G-71] (CORRECTED, reasoning). Whether that image contains the compiled `tc358743` overlays is NEEDS VERIFICATION. See [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

---

## 5. Cross-cutting constraints

| Constraint | Layers | Evidence | Risk | Retired by |
|---|---|---|---|---|
| 1080p60 needs more than 2 CSI-2 lanes; a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s, OQ-038). REQ-CAP-007 requires both a 2-lane configuration (up to 1080p50 UYVY / 1080p30 RGB888) and a 4-lane configuration (1080p60 required) | 1, 3, 4 | [C-37]; reasoning [C-47], [C-48], [C-49] | RISK-001, RISK-006 | ADR-004 (per configuration) and ADR-008 (OQ-099), then TEST-CAP-002 and TEST-CAP-004 |
| Source behaviour per ATEM or camera model (output modes, interlace, HDCP, colour format) is not in the source register; the ATEM Mini Pro outputs progressive 1080p only [F-23] | 1, 2 | [F-23]; [B-27] (reasoning); [A-04] | RISK-008, RISK-009 | OQ-102, then TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 |
| 1080p60 hardware encode on Pi 4/CM4 is unproven | 8 | [D-10]; reasoning [D-52]; [D-50] (reported; community source) | RISK-002 | TEST-ENC-001 |
| No hardware encoder on Pi 5/CM5; UYVY must be converted per frame | 7, 8 | [G-22], [D-31], [D-43] | RISK-003 | TEST-ENC-001, TEST-PERF-001 |
| No video until an EDID is written after every boot or driver load (no EDID stored after probe; the "no default EDID" part is a *research gap*, topic B) | 2 | [A-33], [B-21] (CORRECTED), [A-31] | RISK-010 | REQ-CAP-003 (trigger and ordering: OQ-093), TEST-DRV-002 |
| Signal changes seen only by 1 s polling unless INT is wired | 2, 6 | [A-30], [B-19], [C-20] | RISK-013 | OQ-020, TEST-CAP-003 |
| CMA sizing | 4, 7 | [C-53] (CORRECTED; reasoning), [E-47] (CORRECTED), [C-40] | RISK-020 | TEST-PERF-001 |
| Pi 5/CM5 path relies on community procedures | 4, 5 | [C-38], [C-33] (reported; community source), [C-42] (reported; community source) | RISK-012 | TEST-PLT-001, TEST-CAP-001/002/003 |
| Thermal envelope: TC358743 rated −30 to +70 °C ambient, typical 543.2 mW at 1080p60; Pi 5 encode is CPU-bound | 1, 2, 8 | [A-41], [A-38], [G-22] | — (REQ-PERF-001, OQ-010) | TEST-PERF-001 |

---

## 6. Traceability by layer (Rule 12)

Implementation status is `NOT STARTED` for every row. Every test is `BLOCKED — HARDWARE REQUIRED` except TEST-BLD-001, which is `NOT STARTED`. Design-to-implementation links will be added as implementation starts ([TRACEABILITY.md](TRACEABILITY.md)).

| Layer | Requirements | Decisions | Risks | Tests |
|---|---|---|---|---|
| 1 Hardware | REQ-PLT-001, REQ-DRV-001, REQ-CAP-007, REQ-CAP-008 | ADR-004 | RISK-001, RISK-004, RISK-007, RISK-021 | TEST-HW-001 |
| 2 TC358743 | REQ-DRV-001, REQ-CAP-003, REQ-CAP-004, REQ-CAP-005, REQ-CAP-008 | ADR-001, ADR-002, ADR-005 | RISK-005, RISK-007, RISK-008, RISK-009, RISK-010, RISK-013 | TEST-DRV-001, TEST-DRV-002, TEST-CAP-001 |
| 3 CSI-2 | REQ-CAP-001, REQ-CAP-005, REQ-CAP-007 | ADR-004, ADR-005, ADR-008 | RISK-001, RISK-006, RISK-011 | TEST-CAP-002, TEST-CAP-004 |
| 4 CSI receiver | REQ-ARCH-001, REQ-PLT-001, REQ-CAP-002 | ADR-004, ADR-006 | RISK-011, RISK-012, RISK-016 | TEST-PLT-001 |
| 5 Media Controller | REQ-ARCH-001, REQ-CAP-002 | ADR-001, ADR-006 | RISK-012 | TEST-PLT-001 |
| 6 V4L2 | REQ-CAP-002, REQ-CAP-004 | ADR-001, ADR-006 | RISK-013, RISK-016 | TEST-CAP-001, TEST-CAP-003 |
| 7 DMABUF | REQ-DMA-001, REQ-ARCH-001 | ADR-005, ADR-007 | RISK-020 | TEST-DMA-001 |
| 8 Encoder | REQ-ENC-001 | ADR-004, ADR-007 | RISK-002, RISK-003, RISK-015 | TEST-ENC-001 |
| 9 Recorder / RTMP / WebRTC | REQ-REC-001, REQ-STR-001, REQ-STR-002 | ADR-007 | RISK-019 | TEST-REC-001, TEST-STR-001, TEST-STR-002 |
| Audio side path | REQ-CAP-006 | — | RISK-014 | TEST-AUD-001 |
| ATEM integration (HDMI capture only, OQ-009 ANSWERED) | REQ-ATEM-001, REQ-CAP-008 | — | RISK-008; RISK-018 only if network control is added | TEST-ATEM-001 |
| Whole system | REQ-PERF-001, REQ-BLD-001, REQ-BLD-002 | ADR-003 | RISK-017, RISK-020 | TEST-PERF-001, TEST-BLD-001 |

---

## 7. Open architectural decisions

No decision below may be treated as made until [DECISIONS.md](DECISIONS.md) records it as `ACCEPTED`.

| ADR | Title | Status (2026-10-07; ADR-003 ACCEPTED 2026-10-07, the others unchanged since 2026-10-06) | Layers affected | What it blocks | OQ |
|---|---|---|---|---|---|
| ADR-003 | OS and image basis: Raspberry Pi OS Lite (64-bit) for bring-up, `rpi-image-gen` for production, Buildroot as the documented alternative | ACCEPTED (2026-10-07) | All; kernel version and media-stack versions differ: Raspberry Pi OS ships kernel 6.18.50 [G-04], Buildroot 2026.08 pins 6.12.61 [G-59] | Nothing since acceptance; it governs REQ-BLD-001; REQ-BLD-002 (own OS image; owner, 2026-10-07; accepted with the proposal unchanged); build, update and release design | OQ-012 (ANSWERED) |
| ADR-004 | Product target platform (since 2026-10-07 a choice per lane configuration, REQ-CAP-007) | OPEN | 1, 3, 4, 5, 7, 8 | Platform for the 2-lane and for the 4-lane configuration; REQ-CAP-001 feasibility on the 4-lane configuration; encode path; every platform-specific procedure | OQ-011 |
| ADR-005 | Default capture pixel format: UYVY | PROPOSED | 2, 3, 6, 7, 8 | Lane budget, encoder input path, colour metadata | OQ-003 |
| ADR-006 | Media Controller mode on every platform | PROPOSED | 4, 5, 6 | Capture control design, test procedures | OQ-014 |
| ADR-007 | Userspace media framework | OPEN | 7, 8, 9 | All userspace pipeline design ([SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md)) | OQ-015 |
| ADR-008 | CSI-2 link frequency: proposes keeping the overlay default 486 MHz (972 Mbit/s per lane) on all platforms; 297 MHz to be evaluated only on a CM4 CAM1 4-lane link for 1080p60 UYVY in TEST-CAP-002 | PROPOSED | 2, 3, 4 | Device Tree `link-frequency` setting; 1080p60 UYVY lane use (3 of 4 lanes at 972 Mbit/s, OQ-038); Pi 5/CM5 receiver match [C-52] | OQ-099 |

For context:

- ADR-001 (use kernel V4L2 / Media Controller) is **ACCEPTED**.
- ADR-002 (use the in-tree `tc358743` driver as the baseline) is **PROPOSED** (OQ-013).

---

## 8. Architecture vs implementation (Rule 5)

- **No implementation exists** as of 2026-10-06: no code, no Device Tree overlay of our own, no image and no hardware (owner, 2026-10-06).
- **Therefore there is no divergence yet** between this document and an implementation.
- What this document records is:
  1. the architecture mandated by Rule 5; and
  2. how that architecture maps onto stock Raspberry Pi kernels, overlays and drivers, according to sources.

  Neither has been checked on PACSCORDER hardware.
- **This document must be updated whenever the implementation changes** (Rule 5, Rule 24). That covers a new or changed driver, overlay, Device Tree, kernel version, encoder path, userspace framework or platform choice. When an implementation differs from what is written here, the difference is recorded in the register below *and* the affected layer section is updated in the same change. Entries are never deleted (Rule 21).
- **When an ADR changes status, update in the same change:**
  - section 7;
  - the "Governing decisions" column of section 1.1;
  - the affected layer subsections.

**Divergence register** (documented architecture vs implementation):

| Date | Layer | Documented | Implemented | Reason | Action / ADR |
|---|---|---|---|---|---|
| — | — | — | — | No implementation exists (2026-10-06) | — |

---

## 9. Unknowns that affect the architecture

| Unknown | Marker | OQ |
|---|---|---|
| Which platform serves each lane configuration (2-lane and 4-lane, REQ-CAP-007; OQ-001 ANSWERED 2026-10-07) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-011 (ADR-004) |
| Whether one bridge-board design can serve both the 2-lane and the 4-lane configuration | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-021 |
| Supported input modes and EDID content, for each lane configuration (REQ-CAP-007) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-002 |
| PACSCORDER hardware (bridge board, lanes, REFCLK, INT/RESET, I2C, power) | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-018 to OQ-026 |
| Whether Pi 5/CM5 TC358743 capture succeeds end to end | HARDWARE TEST REQUIRED | OQ-049, OQ-050, OQ-051 |
| Whether capture on 3 active lanes of 4 configured is reliable (1080p60 UYVY at 972 Mbit/s) | HARDWARE TEST REQUIRED | OQ-038 |
| CSI-2 link frequency (ADR-008, PROPOSED) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-099 |
| How CSI-2 receive errors are read on Unicam (Pi 4/CM4) | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OQ-095 |
| Raspberry Pi Ethernet, USB and storage facts per candidate board | DATASHEET REQUIRED | OQ-098 |
| `config.txt` overlay-parameter syntax (bare boolean, combined parameters, `[pi4]` matching CM4, unknown parameters) | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-100 |
| Userspace design: process model, operator interface, configuration and logging, EDID trigger, updates during recording | OWNER DECISION REQUIRED | OQ-090 to OQ-094 |
| Whether Pi 4/CM4 hardware encode sustains 1080p60 | HARDWARE TEST REQUIRED | OQ-056 |
| Zero-copy DMABUF from Unicam into the encoder | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OQ-058 |
| Pi 5/CM5 software encode cost and UYVY conversion offload | HARDWARE TEST REQUIRED | OQ-059, OQ-060 |
| CMA budget per platform | HARDWARE TEST REQUIRED | OQ-061 |
| Runtime device nodes, I2C bus numbers and media entity names | HARDWARE TEST REQUIRED | OQ-043 |
| Encoding parameters and number of simultaneous encodes | OWNER DECISION REQUIRED | OQ-005 |
| Whether audio is required | OWNER DECISION REQUIRED | OQ-004 |
| Which ATEM models and cameras must be supported as HDMI sources (REQ-CAP-008; integration scope answered 2026-10-07, OQ-009) | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-102 |
| Acceptance of the build tool for the project-owned OS image (REQ-BLD-002; ADR-003 ACCEPTED 2026-10-07: `rpi-image-gen`) | ANSWERED (owner, 2026-10-07); no longer an unknown | OQ-012 (ANSWERED) |

---

## Verification status

### Verified from sources (fact IDs)

The statements in this document rest on these entries of [REFERENCES.md](REFERENCES.md). Each has the verdict `CONFIRMED` or `CORRECTED`. "Verified from sources" means only that the cited source says so.

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-01, A-02, A-03, A-04, A-05, A-06, A-07, A-11, A-13, A-15, A-17, A-20, A-22, A-24, A-25, A-28, A-30, A-31, A-32, A-33, A-34, A-38, A-41, A-42, A-44, A-45, A-47, A-48 |
| B — tc358743 Linux driver | B-02, B-07, B-12, B-16, B-18, B-19, B-20, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-28, B-29, B-30, B-31, B-32, B-34, B-35, B-36, B-37, B-38, B-39, B-41, B-42, B-43, B-44, B-47, B-48 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-07, C-08, C-09, C-10, C-11, C-12, C-13, C-14, C-16, C-17, C-19, C-20, C-24, C-25, C-26, C-27, C-28, C-29, C-30, C-31, C-32, C-33, C-34, C-36, C-37, C-38, C-39, C-40, C-41, C-42, C-43, C-45, C-46, C-47, C-48, C-49, C-50, C-52, C-53 |
| D — Encoders | D-05, D-06, D-07, D-08, D-09, D-10, D-11, D-12, D-13, D-14, D-16, D-17, D-18, D-19, D-20, D-21, D-22, D-24, D-25, D-28, D-29, D-31, D-32, D-34, D-40, D-42, D-43, D-44, D-45, D-47, D-48, D-50, D-52, D-53 |
| E — Buildroot and kernel configuration | E-40, E-43, E-44, E-47, E-48 |
| F — ATEM and streaming | F-01, F-11, F-23, F-25, F-31, F-33, F-34, F-36, F-38, F-40, F-41, F-42, F-45, F-46 |
| G — Raspberry Pi OS and image tooling | G-04, G-12, G-13, G-14, G-16, G-19, G-20, G-21, G-22, G-26, G-32, G-59, G-71 |

How each tier is used:

- **`CORRECTED` entries**, used in their corrected wording only: A-22, A-25, B-21, B-25, B-44, C-28, C-36, C-39, C-53, E-40, E-47, F-34, G-71.
- **`community` entries**, worded as reports: C-28, C-33, C-41, C-42, C-43, C-45, D-17, D-18, D-50, F-11, F-45.
- **`reasoning` entries**, labelled as reasoning: B-27, B-47, C-46, C-47, C-48, C-49, C-50, C-52, C-53, D-29, D-52, F-38, F-40, F-46, G-71.
- Statements marked *research gap* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). They are not register facts and are recorded only to state what is unknown.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No layer has been implemented, built or tested; every layer is `NOT STARTED`. Every test listed above is `BLOCKED — HARDWARE REQUIRED`, except TEST-BLD-001, which is `NOT STARTED`.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review against REFERENCES.md. Changes: aligned wording with [C-50], [D-32], [D-50], [F-41], [G-26], [B-21]; added PACSCORDER unknowns (REFCLK OQ-019, connector OQ-021, I2C address OQ-026) to the diagrams; marked Pi 5 connector-to-receiver mapping and the `4lane` + `cam0` combination as reasoning / NEEDS VERIFICATION; labelled the Pi 4/CM4 DMABUF import as a target (OQ-058); used Rule 10 status for the CEC row; distinguished TEST-BLD-001 (`NOT STARTED`) from hardware-blocked tests. Added citations A-44, C-13, C-14, D-47. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: ADR-006 "chooses" → "proposes"; ADR-008 (PROPOSED, OQ-099) added to the §1.1 layer table, §3.3 decisions, §6 traceability and the §7 ADR table; 4-lane port marked necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s, OQ-038) in §3.3, §4.1, §4.2, §4.3 and §5; Pi 5 CAM/DISP0 4-lane feasibility conditioned on OQ-049; EDID-volatility rows no longer cite [B-22] (now [A-31], [B-21] and the topic B research gap; OQ-093); HD-source colourimetry attributed to the research gap, not [B-34]; TC358743 audio notes now say the driver configures I2S [A-13] while the silicon can also use CSI-2 [A-05]; RISK-016 described as a pixel-format label difference; 6.18.39 identified as the kernel of the [C-33] report (shipped kernel 6.18.50 [G-04]); [D-50] "edge case" attributed to one engineer (6by9); OQ-098 linked for storage/Ethernet/USB; OQ-100 noted for overlay-parameter syntax; §9 unknowns extended with OQ-038, OQ-090 to OQ-095, OQ-098 to OQ-100. No requirement, decision, risk or test status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: summary block added under "Current state"; sources are ATEM HDMI output or cameras connected directly (REQ-CAP-008; models OQ-102) in §1.1, §3.1, the §4.1/§4.2 diagrams and a new §5 row; ATEM row of §3.10 reduced to HDMI capture, with UDP 9910 and RTMP exchange marked not in current scope (OQ-009 ANSWERED); both 2-lane and 4-lane configurations (REQ-CAP-007) added to §1.1, §3.1, §3.3 (new requirement paragraph, configuration row, `4lane`/wiring note [C-12], [C-17], RISK-001 nuance that Pi 4 Model B and CM4 CAM0 remain 2-lane candidates), §4.1–§4.3 and §5; ADR-004 described as a choice per lane configuration (still OPEN); own OS image (REQ-BLD-002) added after §4.3 and to the ADR-003 row of §7; §6 traceability extended with REQ-CAP-007, REQ-CAP-008, REQ-BLD-002; §9 "Whether 1080p60 is mandatory (OQ-001)" and "ATEM integration scope (OQ-009)" replaced by the per-configuration platform, bridge-board (OQ-021), source-model (OQ-102) and build-tool acceptance (OQ-012) unknowns. No requirement, decision, risk or test status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); "Owner decisions of 2026-10-07" summary block (ADR-003 ACCEPTED, OQ-012 ANSWERED); §4.3 "Product OS image" paragraph (ADR-003 ACCEPTED, OQ-012 ANSWERED; "proposes" → "decides", "prefers" → "chose"); §7 status-column heading and ADR-003 row (ACCEPTED, no longer blocks, OQ-012 ANSWERED); §9 OQ-012 row marked ANSWERED. Not changed: evidence and citations, ADR-002, ADR-005, ADR-006, ADR-008 (PROPOSED) and ADR-004, ADR-007 (OPEN), implementation status (NOT STARTED), Change-history rows. TEST-ATEM-001 appears here by ID only, so no title changed. | Claude (session 2026-10-07) |
| 2026-10-07 | §4.3 ADR-003 rationale aligned with the corrected wording in DECISIONS.md (overlay inclusion NEEDS VERIFICATION; board families "may" differ). | Claude (session 2026-10-07) |
