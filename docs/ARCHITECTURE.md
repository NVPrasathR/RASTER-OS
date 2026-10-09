# PACSCORDER System Architecture

| | |
|---|---|
| Document status | DRAFT. It describes the mandated layer stack (Rule 5) and how each layer maps onto each candidate platform, from source research. No implementation exists. |
| Last updated | 2026-10-09 |
| Applies to | The PACSCORDER video pipeline, end to end, on all four candidate platforms: Raspberry Pi 4 Model B, Compute Module 4 (CM4), Raspberry Pi 5, Compute Module 5 (CM5), in both the 2-lane and the 4-lane CSI-2 configuration (REQ-CAP-007). The product platform is not chosen (ADR-004, OPEN; since 2026-10-07 a choice per lane configuration). Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07, second answer); Pi 4 Model B and Pi 5 stay documented as candidates while ADR-004 is OPEN. Since 2026-10-08 it also covers the HDMI audio side path (section 3.11) and the H.265 encode and transport paths (sections 3.8.1, 3.9.1). The H.265 sections are kept as evidence for REQ-ENC-002, which the owner deferred on 2026-10-08 ("H.264 only for now", OQ-103); they are not in current scope. Since 2026-10-08 (owner answer to OQ-005, "Separate record + live") the Encoder layer runs two simultaneous H.264 encodes from one capture: a recording encode, and one live encode shared by RTMP and WebRTC (sections 3.7–3.9; OQ-115, OQ-059). *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* Since the owner decisions of 2026-10-08 the Recorder writes every recording as fragmented MP4 to two drives, a PCIe NVMe SSD and a self-powered USB-to-SATA HDD (ADR-009, ACCEPTED; section 3.9.3), and the live path has a target of under 1 s camera-to-viewer for WebRTC viewers only, with RTMP best-effort (OQ-116 ANSWERED; section 3.9.4). |
| Verification | Verified from sources only: the source research of 2026-10-06 and of 2026-10-08 (topic H, H.265/HEVC; topic I, HDMI audio; topic J, recording storage and power loss; topic K, live latency) in [REFERENCES.md](REFERENCES.md). Nothing has been verified on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
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
> - **Sources.** The HDMI sources are Blackmagic ATEM switcher outputs and cameras connected directly (REQ-CAP-008, DRAFT; OQ-009 ANSWERED). The ATEM integration is HDMI capture of the ATEM output only; ATEM network tally/control (UDP 9910) and RTMP exchange are not in current scope. Which ATEM and camera models must be supported is OQ-102. *(Superseded by the second set below: OQ-102 ANSWERED — any HDMI camera, no model list.)*
> - **Lane configurations.** Both a 2-lane and a 4-lane CSI-2 configuration are required, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; OQ-001 ANSWERED). Recorded interpretation: 1080p60 is required on 4-lane configurations; on a 2-lane configuration the physical limit is 1080p50 UYVY / 1080p30 RGB888 for 1920x1080 [C-37], [C-48]. ADR-004 is now a platform choice per configuration (still OPEN).
> - **OS image.** The product runs its own project-built OS image (REQ-BLD-002, DRAFT). The build tool is ADR-003 (ACCEPTED 2026-10-07: Raspberry Pi OS Lite for bring-up, `rpi-image-gen` for the product image, Buildroot as the alternative); OQ-012 ANSWERED (owner, 2026-10-07).

> **Owner decisions of 2026-10-07, second set** (recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [DECISIONS.md](DECISIONS.md) ADR-004, [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) and [RISKS.md](RISKS.md); propagated here on 2026-10-08):
> - **Bring-up boards.** Bring-up evaluates **CM4 and CM5 side by side** ("CM4 + CM5 side by side"). The product platform is decided from the TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 results; ADR-004 stays OPEN until then (OQ-011).
> - **HDMI audio is required** in recordings and streams (REQ-CAP-006, DRAFT; OQ-004 ANSWERED). Channels, sample rates and the A/V tolerance are not yet specified (OQ-110, OQ-111, OQ-112). Section 3.11.
> - **Codecs: H.264 and H.265 (HEVC)** for recording and streaming (REQ-ENC-001; OQ-005 answered for the codec). Which output uses which codec is OQ-103. No candidate has a hardware HEVC encoder [D-24], [D-31], so H.265 is software-encoded on every candidate (reasoning; RISK-022). Sections 3.8.1 and 3.9.1. *(Superseded by the owner decision of 2026-10-08 below: H.264 only; H.265 deferred, REQ-ENC-002.)*
> - **Sources: any HDMI camera, no model list**, plus ATEM switcher outputs (REQ-CAP-008; OQ-102 ANSWERED). What is accepted is defined by the supported-mode matrix and the EDID (OQ-002).

> **Owner decision of 2026-10-08** (answer to OQ-103; recorded in [REQUIREMENTS.md](REQUIREMENTS.md), [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md), [RISKS.md](RISKS.md) and [DECISIONS.md](DECISIONS.md) ADR-007): **"H.264 only for now."**
> - Every output — recording, RTMP and WebRTC — uses H.264 (REQ-ENC-001). There is one video codec in the current scope.
> - H.265 is deferred as REQ-ENC-002 (acceptance DEFERRED; no planned tests). Sections 3.8.1 and 3.9.1, the video part of 3.9.2 and the H.265 rows elsewhere are kept as its evidence and labelled "deferred — REQ-ENC-002; not in current scope". TEST-ENC-001 has no H.265 runs in the current scope.
> - RISK-022, RISK-025 and OQ-104 to OQ-109 stay OPEN but are not in current scope. No ADR status changed; ADR-007 carries a scope note that the HEVC-over-RTMP constraint does not drive the framework choice for the current scope.
> - Unchanged: CM4 + CM5 side-by-side bring-up, HDMI audio required, any HDMI camera, ADR-003 ACCEPTED.

> **Owner decision of 2026-10-08, second** (answer to OQ-005; recorded in [REQUIREMENTS.md](REQUIREMENTS.md) REQ-ENC-001, [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005 and OQ-115, and [RISKS.md](RISKS.md) RISK-002 and RISK-003): **"Separate record + live."**
> - **Two simultaneous H.264 encodes from one capture.** One is the recording encode. The other is one live encode, shared by RTMP and WebRTC. Bitrate, rate control and latency are still open (OQ-005). *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — the latency target is under 1 s camera-to-viewer for WebRTC viewers only, and RTMP is best-effort (OQ-116 ANSWERED). Bitrate and rate control are still open; OQ-005 stays OPEN.)*
> - **Pi 4/CM4.** Both encodes would run on `bcm2835-codec`. Whether it runs two at once is UNKNOWN — VERIFICATION REQUIRED (OQ-115; RISK-002). Reasoning: two 1080p30 encodes need the macroblock rate of one 1080p60 encode, about 2.0× the 1080p30 specification [D-10], [D-52].
> - **Pi 5/CM5.** Both encodes run in software (OQ-059; RISK-003). Reasoning from [G-22]: that roughly doubles the encode CPU load.
> - **The live encode must be WebRTC-receivable** (section 3.9). RTMP receives the same stream.
>   - Constrained Baseline [F-36], which the Pi 4/CM4 encoder offers [D-11].
>   - libwebrtc assumes Constrained Baseline Level 3.1 when unsignalled [F-39], while 1080p needs Level 4.0 or above (reasoning [F-40]; OQ-073, RISK-019).
>   - No B-frames: reported by the MediaMTX project [F-45]. The Pi 4/CM4 encoder produces none [D-14].
>   - SPS/PPS in-band [F-36]. Repetition is off by default on Pi 4/CM4 [D-15].
> - **Audio is still two encodes:** AAC for recording and RTMP, Opus for WebRTC (section 3.9.2).
> - **Unchanged:** H.265 stays deferred (REQ-ENC-002). No ADR status changed.

> **Source research of 2026-10-08** (topics H and I in [REFERENCES.md](REFERENCES.md); gaps in [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json)). It adds the H.265 encode paths (section 3.8.1), the HEVC-over-RTMP framework constraint (section 3.9.1) — both deferred with REQ-ENC-002 since the owner decision of 2026-10-08 above — and the HDMI audio data path, rate tracking and A/V synchronisation inputs (section 3.11). New registers: OQ-104 to OQ-114, RISK-023 to RISK-025. None of it has been run on PACSCORDER hardware.

> **Owner decisions of 2026-10-08, third** (recording storage and live latency; recorded in [DECISIONS.md](DECISIONS.md) ADR-009, [REQUIREMENTS.md](REQUIREMENTS.md) REQ-REC-001, REQ-STR-001 and REQ-STR-002, and [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) OQ-005, OQ-006 and OQ-116; propagated here 2026-10-09):
> - **Live latency.** Under 1 s camera-to-viewer, for **WebRTC viewers only** ("Under 1 second"; "WebRTC viewers only"; REQ-STR-002; OQ-116 ANSWERED). RTMP outputs are best-effort; their latency is set by the receiving platform (REQ-STR-001). Bitrate and rate control are still open (OQ-005 stays OPEN). Section 3.9.4.
> - **Recording (ADR-009, ACCEPTED 2026-10-08).** Container MP4, written **fragmented** for power-loss safety; every recording **mirrored** to a PCIe NVMe SSD and a USB-to-SATA HDD; the HDD in a **self-powered** enclosure. ADR-009's fourth point, ext4 on the recording volumes, is Claude's proposal inside the ADR, **not an owner decision** (OQ-120). The maximum recording duration is still open (OQ-006). Section 3.9.3.
> - **Consequence.** Each recording encode feeds two file writers. The HDD writer must be decoupled, so that an HDD stall cannot reach the NVMe copy, the shared encoder or the live path (ADR-009 Consequences; OQ-117, RISK-028).
> - **Unchanged:** two simultaneous H.264 encodes (recording, and one live encode shared by RTMP and WebRTC; OQ-115 open for CM4); H.265 deferred (REQ-ENC-002); HDMI audio required; CM4 and CM5 evaluated side by side (ADR-004 OPEN); ADR-003 ACCEPTED; ADR-007 (media framework) OPEN, so every framework-specific route in this document is a candidate, not a decision.

> **Source research of 2026-10-08, later** (topics J, recording storage and power loss, and K, live latency, in [REFERENCES.md](REFERENCES.md); research gaps, open questions and design risks in [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json); propagated here 2026-10-09). It adds the storage attachment per board and the fragmented-MP4 facts (section 3.9.3) and the latency facts of the live path (section 3.9.4). New registers: OQ-117 to OQ-128, RISK-026 to RISK-034. None of it has been run on PACSCORDER hardware.

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
- **The Encoder layer is hardware on Pi 4/CM4 and software on Pi 5/CM5.** Pi 4/CM4 have the `bcm2835-codec` hardware H.264 encoder [D-06], [D-10]. Pi 5/CM5 have no hardware video encoder and encode in software [D-31], [G-22]. The DMABUF layer therefore means a different thing on each family (section 3.7). *(Added 2026-10-08; updated the same day after the owner deferred H.265.)* This split holds for H.264, the only codec in the current scope (owner, 2026-10-08: "H.264 only for now", OQ-103). H.265 — required by REQ-ENC-001 from 2026-10-07, now deferred as REQ-ENC-002 and not in current scope — would be software-encoded on all four candidates, because `bcm2835-codec` has no HEVC encoder [D-24] and Pi 5/CM5 have no hardware video encoder [D-31] (reasoning; RISK-022; section 3.8.1). *(Added 2026-10-08; owner answer to OQ-005, "Separate record + live".)* The layer is to run two H.264 encodes at once: a recording encode, and a live encode shared by RTMP and WebRTC. On Pi 4/CM4 both would use `bcm2835-codec` (whether it runs two at once: OQ-115). On Pi 5/CM5 both use the CPU (OQ-059).
- **HDMI audio is a side path, not a Rule 5 layer** *(added 2026-10-08)*. It leaves the TC358743 on I2S wiring, not on the CSI-2 lanes, because the driver always selects I2S output [A-13], [I-24]. It enters userspace through ALSA and joins the video only at the muxers (section 3.11).
- **Recording storage and the live-latency target belong to layer 9, not to new layers** *(added 2026-10-09; ADR-009 / OQ-116 / research topics J and K)*. The recorder writes fragmented MP4 to two drives that attach through PCIe and USB, not through the capture path (section 3.9.3). The < 1 s target for WebRTC viewers runs from the camera to the viewer, so every layer from the receiver to the WebRTC output adds to it (section 3.9.4).

### 1.1 Layer summary

| # | Layer | Pi 4 Model B / CM4 | Pi 5 / CM5 | Governing decisions | Status |
|---|---|---|---|---|---|
| 1 | Hardware | HDMI source: ATEM output or camera (REQ-CAP-008). 2-lane connector on Pi 4B [C-01]; CM4 CAM1 4 lanes, CAM0 2 lanes [C-02]. 2-lane configuration candidates: Pi 4B, CM4 CAM0; 4-lane: CM4 CAM1 (REQ-CAP-007). PACSCORDER board: UNKNOWN (OQ-018) | HDMI source: ATEM output or camera (REQ-CAP-008). 4-lane ports on Pi 5 [C-04]; CM5 MIPI0/1 4 lanes [C-05]; 4-lane configuration candidates (REQ-CAP-007). PACSCORDER board: UNKNOWN (OQ-018) | ADR-004 (OPEN; per lane configuration) | NOT STARTED |
| 2 | TC358743 | In-tree `tc358743` driver, module `tc358743` [A-48], [G-16]; overlay `tc358743` [G-12] | Same driver [B-02], [G-16]; overlay `tc358743-pi5`, auto-selected [C-11], [G-13] | ADR-001 (ACCEPTED), ADR-002 (PROPOSED), ADR-005 (PROPOSED) | NOT STARTED |
| 3 | CSI-2 | D-PHY, 972 Mbps/lane by default [A-45]; Unicam up to 1 Gbit/s per lane [C-07] | D-PHY; RP1 up to 1.5 Gbps per lane [C-30]; CFE programs 999 Mbps for this bridge [C-31] | ADR-004 (OPEN), ADR-005 (PROPOSED), ADR-008 (PROPOSED) | NOT STARTED |
| 4 | Raspberry Pi CSI receiver | Unicam, downstream driver module `bcm2835-unicam-legacy` [C-09] | RP1 CFE, downstream driver module `rp1-cfe-downstream` [C-29] | ADR-004 (OPEN), ADR-006 (PROPOSED) | NOT STARTED |
| 5 | Media Controller | Legacy video-node mode by overlay default, or Media Controller mode via the `media-controller` parameter [C-10] | Media Controller only [C-11]; `csi2` sub-device, links set by userspace [C-32] | ADR-006 (PROPOSED) | NOT STARTED |
| 6 | V4L2 | Capture node `unicam-image` [C-36]; TC358743 sub-device node [B-18] | Capture node `rp1-cfe-csi2_ch0` [C-32]; TC358743 sub-device node [B-18] | ADR-001 (ACCEPTED), ADR-006 (PROPOSED) | NOT STARTED |
| 7 | DMABUF | Target: capture buffer imported into the hardware encoder; encoder queues support DMABUF [D-19]; zero-copy from Unicam is unproven (OQ-058). *(2026-10-08: the same buffer goes to two encodes, recording and live; OQ-115.)* | No hardware encoder to import into; frames go to a CPU encoder after format conversion [D-43]. *(2026-10-08: to two CPU encoders, recording and live; OQ-059.)* | ADR-007 (OPEN) | NOT STARTED |
| 8 | Encoder | Hardware `bcm2835-codec`, `/dev/video11`, H.264 spec 1080p30 [D-06], [D-10]. H.265 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope): software only, no HEVC encoder in `bcm2835-codec` [D-24] (section 3.8.1). Two concurrent encodes, recording and live (added 2026-10-08): unproven (OQ-115, RISK-002) | Software H.264; "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22]. H.265 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope): software only [D-31] (section 3.8.1). Two concurrent software encodes (added 2026-10-08): roughly double the encode CPU load (reasoning from [G-22]; OQ-059, RISK-003) | ADR-004 (OPEN), ADR-007 (OPEN); codec: H.264 on every output (OQ-103 ANSWERED 2026-10-08); two encodes, recording and shared live (owner answer to OQ-005, 2026-10-08) | NOT STARTED |
| 9 | Recorder / RTMP / WebRTC | Userspace; framework not chosen. *(2026-10-08: the recorder takes the recording encode; RTMP and WebRTC share the live encode, OQ-005.)* HEVC over RTMP (added 2026-10-08; deferred — REQ-ENC-002; not in current scope): FFmpeg 7.1.5 can mux it [H-26]; GStreamer 1.26.2 `flvmux` cannot [H-27] (section 3.9.1). *(Added 2026-10-09; ADR-009 / OQ-116.)* Recorder: fragmented MP4 to an NVMe SSD writer and a decoupled HDD writer (OQ-117). CM4: NVMe in the CM4 IO Board's single PCIe Gen 2 x1 socket, HDD on its shared USB 2.0 hub [J-03], [J-06] (RISK-026). Pi 4 Model B: storage not researched in topic J (NEEDS VERIFICATION; OQ-098). Live: < 1 s for WebRTC viewers only; RTMP best-effort (sections 3.9.3, 3.9.4) | Userspace; framework not chosen. *(2026-10-08: as Pi 4/CM4, recording encode to the recorder and live encode to RTMP and WebRTC.)* HEVC over RTMP (deferred — REQ-ENC-002; not in current scope): as Pi 4/CM4 [H-26], [H-27]. *(Added 2026-10-09; ADR-009 / OQ-116.)* Recorder as Pi 4/CM4. CM5: NVMe on the CM5 IO Board's M.2 slot at PCIe Gen 2 x1, HDD on USB 3.0 [J-13], [J-18], [J-19] (M.2 enablement OQ-124). Pi 5: storage not researched in topic J (NEEDS VERIFICATION; OQ-098). Live as Pi 4/CM4 | ADR-007 (OPEN); ADR-009 (ACCEPTED 2026-10-08; recorder; added here 2026-10-09) | NOT STARTED |
| — | HDMI audio side path (not a Rule 5 layer; added 2026-10-08) | CM4: I2S into `bcm2835-i2s` on GPIO 18–21, 2 channels, ALSA card `tc358743` [I-10], [I-13], [I-15]. Pi 4 Model B: not separately researched in topic I (NEEDS VERIFICATION) | CM5: overlay labels resolve to RP1 I2S1 on GPIO 18–21 [I-07], [I-09]; operation unconfirmed (OQ-054). Pi 5: not separately researched in topic I (NEEDS VERIFICATION) | ADR-004 (OPEN), ADR-007 (OPEN) | NOT STARTED |

---

## 2. Control plane and data plane

PACSCORDER has two distinct paths through the stack.

- The **control plane** decides *what* the hardware does:
  - load the EDID;
  - detect and apply HDMI timings;
  - set media-bus and pixel formats;
  - enable media links;
  - report signal changes;
  - *(added 2026-10-08)* report the HDMI audio sampling rate and audio presence. The kernel does not apply the rate to ALSA, so userspace must (reasoning from source) [I-18], [I-21] (section 3.11).

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
  capture buffers --> /dev/videoN (V4L2 streaming) --> DMABUF --+--> recording encoder --> H.264 --> recorder
                                                                +--> live encoder ------> H.264 --> RTMP and WebRTC
  (two H.264 encodes, owner 2026-10-08, OQ-005; REQ-ENC-001; H.265 deferred, REQ-ENC-002)

  recorder (ADR-009): fragmented MP4 muxer --+--> NVMe SSD writer
                                             +--> HDD branch, decoupled (OQ-117) --> USB-to-SATA HDD writer
  live: < 1 s camera-to-viewer for WebRTC viewers only; RTMP best-effort (OQ-116)

AUDIO DATA PLANE  (added 2026-10-08; side path, not a Rule 5 layer; section 3.11)

  HDMI audio --> TC358743 (internal audio PLL tracks the source's N/CTS [I-26];
  2-channel I2S, TC358743 is clock master [I-24], [I-25]) --> I2S wires to Pi GPIO
  18/19/20 [A-47] --> Pi I2S as clock consumer [I-09], [I-13] --> ALSA card
  "tc358743" [I-15] --> audio encoder (AAC and/or Opus) --> the same muxers
```

*(Data-plane lines updated 2026-10-08 after the owner answered OQ-005 with "Separate record + live". They read: "DMABUF --> encoder --> H.264 elementary stream (REQ-ENC-001; H.265 deferred, REQ-ENC-002) --> recorder / RTMP / WebRTC".)* Reasoning: one capture now feeds two encoders, so each capture buffer has two consumers (section 3.7). The live bitstream is split after its encoder between RTMP and WebRTC (section 3.9). *(Diagram extended 2026-10-09; ADR-009 / OQ-116. The three lines starting "recorder (ADR-009)" and "live:" were added; the lines above them are unchanged. They show the recorder's two file writers, with the HDD writer decoupled (ADR-009, ACCEPTED 2026-10-08; OQ-117, RISK-028; section 3.9.3), and the scope of the latency target (OQ-116 ANSWERED; section 3.9.4). Whether one muxer feeds both writers or each writer has its own muxer is open under ADR-007.)*

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
| Status controls | V4L2 controls on the sub-device | `V4L2_CID_DV_RX_POWER_PRESENT`, plus read-only "Audio sampling rate" and "Audio present". There is no `V4L2_CID_LINK_FREQ` or `V4L2_CID_PIXEL_RATE`. *(Added 2026-10-08.)* The audio control IDs are 0x00981980 (rate, integer 0–768000) and 0x00981981 (present, boolean) [I-20]. The rate reads 0 when no TMDS signal is present [I-19]. Both are updated from the CBIT interrupt status, and `V4L2_EVENT_CTRL` can be subscribed for changes [I-21]. Without INT a rate change takes up to about 1 s, plus I2C time, to appear [I-22]. On CM4 with legacy Unicam the controls are also copied to `/dev/videoN`; on CM5 they exist only on the TC358743 sub-device node [I-23]. | [B-16], [I-19], [I-20], [I-21], [I-22], [I-23] |
| Audio output configuration *(added 2026-10-08)* | Driver-internal, once at probe | `tc358743_set_hdmi_audio()` runs only from the probe-time setup. It hard-codes I2S output with 2 channels, a 500 ms `BUFINIT_START` and a 100 ms `DIV_MODE` delay; the TDM, CSI and 4/6/8-channel settings in the register header are never selected. | [I-24] |

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
| Capture → encoder | Pi 4/CM4: DMABUF import into `/dev/video11` (target; zero-copy from Unicam unproven, OQ-058). Pi 5/CM5: CPU encoder after UYVY-to-planar conversion. *(2026-10-08: two consumers per frame, the recording and live encodes (OQ-005). Two encode sessions on `/dev/video11`: OQ-115. Two CPU encoders: OQ-059.)* | [D-19], [D-43] |
| Encoder → outputs | H.264 elementary stream to a muxer or packetiser. *(2026-10-08: two streams. The recording encode's stream goes to the recorder. The live encode's stream is split between the FLV muxer (RTMP) and the RTP packetiser (WebRTC); OQ-005.)* *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* H.265: `x265enc` outputs byte-stream, so `h265parse` is needed before the MP4 and Matroska muxers [H-13], [H-37]. | [F-31], [F-36], [H-13], [H-37] |
| HDMI audio → Pi *(added 2026-10-08)* | I2S, one data lane, 2 channels in 32-bit slots; the TC358743 is the I2S clock master and its I2S clocks follow the source's audio clock | [I-24], [I-25], [I-26], [I-28] (reasoning) |
| I2S → userspace *(added 2026-10-08)* | `linux,spdif-dir` stub codec plus the Pi I2S as clock consumer, exposed as ALSA card `tc358743`. CM4: `bcm2835-i2s`, exactly 2 channels, 8–384 kHz, S16_LE/S24_LE/S32_LE. CM5: RP1 I2S1; channel count and formats come from hardware registers. | [I-02], [I-11], [I-13], [I-14], [I-15] |
| Audio → outputs *(added 2026-10-08)* | AAC encoders: FFmpeg native `aac` (CORRECTED), GStreamer `voaacenc` and `avenc_aac`. Opus encoders: FFmpeg `libopus`, GStreamer `opusenc`, which accept only 48, 24, 16, 12 or 8 kHz | [I-40], [I-41], [I-44], [I-45], [I-46], [I-47] |
| Recorder → storage *(added 2026-10-09; ADR-009)* | Fragmented MP4, written twice. NVMe SSD: CM4 IO Board PCIe Gen 2 x1 socket through a passive adaptor; CM5 IO Board M.2 M-key slot at PCIe Gen 2 x1. USB-to-SATA HDD: CM4 at USB 2.0 through the IO Board hub; CM5 on USB 3.0. Reasoning: about 1.02 / 1.52 / 3.15 MB/s per drive at 8 / 12 / 25 Mbit/s video plus 192 kbit/s AAC (research examples; the bitrate is still OQ-005). Section 3.9.3 | [J-03], [J-06], [J-13], [J-18], [J-39], [J-45]; [J-36] (reasoning) |
| Live encode → WebRTC viewer *(added 2026-10-09; OQ-116)* | Candidate route only (ADR-007 OPEN): RTSP publish (`rtspclientsink`) to MediaMTX, which serves WebRTC readers, including over WHEP (the WHEP part reported by the MediaMTX project; community source). Target under 1 s camera-to-viewer. Section 3.9.4 | [K-05]; [F-45] (community) |

---

## 3. Layers

Each subsection gives: what the layer is; what implements it on Pi 4/CM4 and on Pi 5/CM5; its control-plane and data-plane roles; its key constraints; the decisions that govern it; and its implementation status.

### 3.1 Layer 1 — Hardware

**What it is.** The layer has five parts:

- the HDMI source: a Blackmagic ATEM switcher's HDMI output or a camera connected directly (REQ-CAP-008, DRAFT; owner, 2026-10-07; models OQ-102 — ANSWERED 2026-10-07: any HDMI camera, no model list);
- the HDMI connector;
- the bridge board carrying the TC358743 and its reference-clock oscillator;
- the CSI-2 cable or connector;
- the Raspberry Pi board, or a Compute Module plus its carrier board.

Audio wiring (I2S, the audio output the Linux driver configures [A-13]) is a separate side path (section 3.10; detailed in section 3.11, added 2026-10-08). HDMI audio is required (REQ-CAP-006, DRAFT; owner, 2026-10-07).

*(Added 2026-10-09; ADR-009.)* Since the owner decisions of 2026-10-08 the hardware also includes two recording drives: a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure (ADR-009, ACCEPTED). They serve layer 9; their attachment per board is in section 3.9.3.

**PACSCORDER hardware.** None exists and none has been chosen (owner, 2026-10-06). Every PACSCORDER-specific hardware fact is unknown:

| Item | PACSCORDER value | Resolution | OQ |
|---|---|---|---|
| Product platform for each lane configuration (2-lane and 4-lane, REQ-CAP-007) | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED | OQ-011 (ADR-004) |
| HDMI source models: ATEM switchers and cameras (REQ-CAP-008) | UNKNOWN — VERIFICATION REQUIRED. *(2026-10-07, second answer: the model list is decided — any HDMI camera, no model list, plus ATEM outputs. What each source outputs remains unknown.)* | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED. *(Owner decision made; HARDWARE TEST REQUIRED remains, with representative sources.)* | OQ-102 (ANSWERED) |
| Bridge board / carrier / number of HDMI inputs | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED | OQ-018 |
| CSI-2 lanes routed, connector, cable, for each configuration; whether one bridge-board design serves both the 2-lane and the 4-lane configuration | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-021 |
| REFCLK oscillator frequency | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-019 |
| INT and RESETN wiring, GPIO numbers | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-020 |
| TC358743 I2C address and bus sharing | UNKNOWN — VERIFICATION REQUIRED | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-026 |
| VDDIO2 voltage, HPD and +5V interface | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; DATASHEET REQUIRED | OQ-024 |
| Use of the camera-connector CAM_GPIO pin | UNKNOWN — VERIFICATION REQUIRED | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-022 |
| Power input and budget | UNKNOWN — VERIFICATION REQUIRED | OWNER DECISION REQUIRED; DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-023 |
| Audio I2S wiring | UNKNOWN — VERIFICATION REQUIRED. *(Added 2026-10-08.)* The four TC358743 audio pins are outputs powered from VDDIO2, rated 1.8–3.3 V [I-27]; the CM4 and CM5 IO Boards have a selectable 1.8 V or 3.3 V GPIO voltage, which VDDIO2 should match, or the lines need level shifting [I-29]. | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-025, OQ-024 |
| GPIO 18–21 allocation with HDMI audio *(added 2026-10-08)* | UNKNOWN — VERIFICATION REQUIRED. The `tc358743-audio` pin group claims GPIO 18–21, including GPIO 21, which the audio path does not use [I-30]; `pwm`, `pwm-2chan` and `gpio-ir` default to GPIO 18 [I-31]. | OWNER DECISION REQUIRED; BUILD TEST REQUIRED | OQ-114 |
| Recording storage, Ethernet, USB | UNKNOWN — VERIFICATION REQUIRED. *(Superseded in part 2026-10-09: owner decisions of 2026-10-08, ADR-009 ACCEPTED — the recording drives are a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure. Still UNKNOWN — VERIFICATION REQUIRED: which SSD, which CM4 PCIe adaptor, which enclosure and USB-to-SATA bridge (OQ-121, OQ-122); Ethernet on all four boards, and USB and storage on Pi 4 Model B and Pi 5 (OQ-098). Topic J covered USB and storage for CM4, CM5 and their IO Boards; section 3.9.3.)* | OWNER DECISION REQUIRED; DATASHEET REQUIRED (Raspberry Pi Ethernet, USB and storage facts). *(2026-10-09: drive types decided; part selection is OWNER DECISION REQUIRED and VENDOR CONFIRMATION REQUIRED, OQ-121, OQ-122.)* | OQ-018, OQ-006, OQ-098; added 2026-10-09: OQ-121, OQ-122 |

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

**Key constraints.** RISK-001 (lane count), RISK-004 (TC358743 supply), RISK-007 (refclk), RISK-021 (third-party board wiring). Source behaviour per ATEM or camera model (HDCP, interlace, colour format) is UNKNOWN — VERIFICATION REQUIRED (OQ-102; RISK-008, RISK-009). *(2026-10-08: OQ-102 is ANSWERED — no model list — so this is now a HARDWARE TEST REQUIRED item on representative sources, bounded by the supported-mode matrix and EDID, OQ-002.)* RISK-014 (audio needs separate I2S wiring; added here 2026-10-08). *(Added 2026-10-09; ADR-009.)* RISK-026 (CM4 storage path: the NVMe SSD takes the only PCIe lane, the HDD shares USB 2.0, and the CM4 IO Board's PCIe slot needs its 12 V input [J-05]) and RISK-027 (USB-to-SATA bridge UAS faults); section 3.9.3.

**Decisions.** ADR-004 (OPEN; since 2026-10-07 a platform choice per lane configuration, REQ-CAP-007). *(Added 2026-10-09.)* ADR-009 (ACCEPTED 2026-10-08) for the recording drives.

**Implementation status.** NOT STARTED. TEST-HW-001: BLOCKED — HARDWARE REQUIRED.

### 3.2 Layer 2 — TC358743 HDMI-to-CSI-2 bridge

**What it is.** The Toshiba TC358743XBG is an HDMI-RX to MIPI CSI-2-TX bridge [A-01]:

- **HDMI receiver:** HDMI 1.4, with no Audio Return Channel or HDMI Ethernet Channel [A-02]. TMDS clock up to 165 MHz; video input up to 1080p60 [A-07].
- **CSI-2 transmitter:** up to 4 data lanes at up to 1 Gbps per lane [A-05].
- **EDID:** 1 KB EDID SRAM [A-31].
- **HDCP:** listed only as "optional" [A-03].
- **Audio:** output on I2S/TDM [A-11], or over CSI-2 [A-05]. The Linux driver always configures 2-channel I2S output [A-13]. *(Added 2026-10-08.)* It does so once, at probe [I-24]. The datasheet makes the I2S output master-clock only, with 32-bit time slots and 16/18/20/24-bit data that "depend on HDMI input stream" [I-25]; an internal audio PLL tracks the N/CTS values of the source's ACR packets, so the I2S clocks follow the source's audio clock [I-26]. Section 3.11.

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

**H.265 path, all four platforms (added 2026-10-08; deferred — REQ-ENC-002; not in current scope).**

- FFmpeg's `libx265` wrapper accepts only planar (or gray) input, never packed `uyvy422`, `yuyv422` or `nv12` [H-10] (CORRECTED). GStreamer 1.26.2 `x265enc` accepts only planar Y444, Y42B and I420 (plus 10- and 12-bit variants), not UYVY, YUY2 or NV12 [H-13].
- Reasoning: because H.265 is software-encoded everywhere [D-24], [D-31], the H.265 path on Pi 4/CM4 has the same shape as the Pi 5/CM5 path: a per-frame UYVY-to-planar conversion before the encoder. Done on the CPU at 1080p60, that conversion reads about 249 MB/s and writes about 187 MB/s [H-43] (reasoning-tier entry).
- Reasoning from [D-40], [H-10] (CORRECTED), [H-13]: `x264enc` accepts NV12 but the H.265 encoders do not, so I420 is a planar format that both encoder families accept, if one converted frame is to feed both.
- Whether the Pi 4/CM4 ISP node (`/dev/video12`) or a BCM2712 block can do this conversion at 1080p60 for the H.265 path is unknown (research gap, topic H; OQ-057, OQ-060, OQ-104).

**Two consumers per frame (added 2026-10-08; owner answer to OQ-005, "Separate record + live").** Reasoning, not a design:

- **One capture feeds two encoders**, the recording encode and the live encode. Each capture buffer must therefore reach both before it can be re-queued.
- **Pi 4/CM4.** The target is to import the same capture DMABUF into two encode sessions on `/dev/video11`, whose queues support DMABUF [D-19].
  - Whether `bcm2835-codec` runs two encode sessions at once is not in the source register: UNKNOWN — VERIFICATION REQUIRED. KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED (OQ-115).
  - Zero-copy import itself is OQ-058.
- **Pi 5/CM5.** One converted frame could feed both software encoders, so the UYVY-to-planar conversion would run once per frame. `x264enc` and `libx264` both accept I420 and NV12 [D-40], [D-43]. Whether the conversion can be offloaded is OQ-060.
- **Buffer hold time.** A buffer is held until the slower of the two encoders releases it. The capture buffer count and the CMA budget are therefore open: OQ-061, RISK-020.
- **Choice.** How buffers are shared between the two encoders is part of ADR-007 (OPEN).
- **Back-pressure and latency** *(added 2026-10-09; ADR-009 / OQ-116 / research topic K)*. V4L2 buffer queues are FIFOs, and `VIDIOC_DQBUF` returns the oldest filled buffer; reasoning: each filled capture buffer still waiting adds one frame period, 33.3 ms at 30p [K-36]. On CM4, when no buffer is queued, Unicam writes the frame to a dummy buffer and drops it [K-34]. Reasoning (RISK-028, RISK-032): a stalled consumer — the recording encode held up by the HDD writer (section 3.9.3), or a CM5 software encode slower than one frame period — therefore shows up at capture either as added live latency or as dropped frames. Queue depth and policy: OQ-126; HDD-branch decoupling: OQ-117.

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

*(Superseded in part — owner decision of 2026-10-07, propagated 2026-10-08.)* The codec is decided: H.264 **and** H.265 (HEVC) for recording and streaming (REQ-ENC-001). Which output uses which codec, and whether software-only H.265 is acceptable, is OQ-103. Bitrate, latency and the number of simultaneous encodes remain UNDEFINED (OQ-005). The table below covers H.264; H.265 is in section 3.8.1.

*(Superseded 2026-10-08 — owner answer to OQ-103: "H.264 only for now".)* The codec is H.264 only, for recording, RTMP and WebRTC (REQ-ENC-001). H.265 is deferred as REQ-ENC-002 (DEFERRED); section 3.8.1 is kept as its evidence and is not in current scope. Bitrate, latency and the number of simultaneous encodes remain UNDEFINED (OQ-005).

*(Superseded in part 2026-10-08, later — owner answer to OQ-005: "Separate record + live".)* There are **two simultaneous H.264 encodes**: a recording encode, and one live encode shared by RTMP and WebRTC (REQ-ENC-001). Bitrate, rate control and latency remain UNDEFINED (OQ-005). The two rows added to the table below cover the concurrency and the live-encode settings.

*(Superseded in part 2026-10-09 — owner decision of 2026-10-08, OQ-116 ANSWERED.)* The latency target is under 1 s camera-to-viewer for WebRTC viewers only, so it binds the live encode; RTMP outputs are best-effort (REQ-STR-002, REQ-STR-001). Bitrate and rate control remain UNDEFINED (OQ-005, OPEN). The row "Live-encode latency facts" below is added from research topic K; the latency budget is in section 3.9.4.

| | Pi 4 / CM4 | Pi 5 / CM5 |
|---|---|---|
| Implementation | Hardware: `bcm2835-codec`, a downstream-only V4L2 M2M driver [D-05]. Encode node `/dev/video11` [D-06], [D-07]. Backed by the VideoCore firmware component `ril.video_encode` over VCHIQ [D-08]. | **No hardware video encoder** [D-31], [G-22]. Reasoning: `bcm2835-codec` cannot probe on Pi 5/CM5, because the BCM2712 Device Tree has no VCHIQ node [D-29], [D-28]. |
| Official capability | H.264 1080p30 encode [D-10] | "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22] |
| Limits | Hardware specification is level 4.0 [D-12]. Bitrate 25 kbit/s – 25 Mbit/s, VBR or CBR [D-13]. No B-frames; GOP 60 by default [D-14]. Frame size at most 1920x1920 [D-09]. Profiles: Baseline, Constrained Baseline, Main, High [D-11]. | Software encoders usually output frames with more latency than the old hardware encoders [D-32] |
| Input format | Read from the firmware at probe; the static table includes UYVY [D-16]. A Raspberry Pi engineer listed UYVY among 13 accepted input formats [D-17] (reported; community source) and reported that both TC358743 formats are accepted directly [D-18] (reported; community source). | `x264enc` / `libx264` need planar input, not UYVY [D-40], [D-43] |
| 1080p60 | **Unproven.** Reasoning: 1080p60 needs 2.0x the specified macroblock rate [D-52]. A Raspberry Pi engineer (6by9) reported that 1080p60 was an "edge case" on the hardware encoder [D-50] (reported; community source). | A Raspberry Pi engineer (6by9) reported that software 1080p60 encode from camera capture is "easily achievable" [D-50] (reported; community source). Not measured with TC358743 input. No official 1080p60 figure. |
| Two concurrent encodes: recording + live *(added 2026-10-08; owner answer to OQ-005)* | Both on `/dev/video11` [D-06]. Whether the encoder runs two encode sessions at once: UNKNOWN — VERIFICATION REQUIRED. HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED (OQ-115). Reasoning: two 1080p30 encodes need 2 × 244,800 = 489,600 MB/s, the rate of one 1080p60 encode and about 2.0x the specification [D-10], [D-52]. Two 1080p60 encodes would need 2 × 489,600 = 979,200 MB/s, about 4.0x (reasoning, same inputs). Whether captured 1080p60 must be encoded at full rate is not specified (OQ-005, RISK-002). | Both in software on the same CPU [D-31], [G-22]. Reasoning from [G-22]: roughly double the encode CPU load of one encode. Whether the "~30–40% CPU" figure means all cores or one core is part of OQ-059 (RISK-003). If one converted frame feeds both encoders, the UYVY conversion runs once per frame (reasoning; section 3.7). |
| Live-encode settings for WebRTC *(added 2026-10-08)* | Constrained Baseline available, default High [D-11]; no B-frames [D-14]; SPS/PPS repetition off by default [D-15], set by `repeat_sequence_header=1` in the official pipeline [D-37]. Level: section 3.9. | B-frames must be off (reasoning from the MediaMTX project's report [F-45], community source); `rpicam-apps` normal mode allows one [D-35]. How to select Constrained Baseline in `x264enc` / `libx264`: NEEDS VERIFICATION. *(2026-10-09: forcing `profile=baseline` in downstream caps is one of the three ways [K-29] names to keep GStreamer `x264enc` free of B-frames (CORRECTED); whether that yields Constrained Baseline signalling is still NEEDS VERIFICATION, OQ-073.)* |
| Live-encode latency facts *(added 2026-10-09; research topic K; OQ-116)* | `bcm2835-codec` limits B-frames to 0, so it never emits them; defaults are profile High, level 4.0 and a GOP of 60 [K-30]. It implements force-keyframe, and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe [K-31]. `v4l2h264enc` on BCM2711 reports 0 latency to the pipeline [K-32]. A Raspberry Pi engineer stated that the encoder holds no extra buffers and that its latency depends on the macroblock count, about 10 ms for 720p on a Pi 4 (community source) [K-33]; the register gives no 1080p figure for BCM2711, and the recording encode would share the encoder (OQ-115). Reasoning from [K-30] and [F-36]: the default profile High is not Constrained Baseline, so the live encode must set its profile explicitly. Pi 4 Model B: same encoder driver (reasoning) | x264's `zerolatency` tune sets lookahead, sync-lookahead and B-frames to 0 and uses sliced threads [K-27], so x264 holds no frames back and GStreamer `x264enc` reports 0 frames of latency [K-28]. GStreamer 1.26 `x264enc` otherwise runs x264's medium preset (3 B-frames, rc-lookahead 40); its `bframes=0` property default does not guarantee a B-frame-free stream unless `tune=zerolatency`, an explicit `bframes=0` or `profile=baseline` in caps is set (CORRECTED) [K-29]. Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39]. Per-frame x264 time on BCM2712 is unknown (OQ-059). Pi 5: same SoC (reasoning) |
| Prerequisites | Non-cut-down GPU firmware: the cut-down firmware removes codec support [D-48] (OQ-048) | CPU and thermal headroom (OQ-059) |
| Risk | RISK-002, OQ-056; added 2026-10-08: OQ-115 (two encodes); added 2026-10-09: RISK-024 (0 latency reported [K-32]), RISK-031, OQ-127 | RISK-003, OQ-059, OQ-060; added 2026-10-09: RISK-031, RISK-032, OQ-127 |

**Common to all four.**

- No candidate has a hardware HEVC encoder [D-24], [D-31].
- Using `libx264` through FFmpeg requires a GPL build (`--enable-gpl`) [D-42]; x264 itself is GPL [D-47] — RISK-015, OQ-086, OQ-087.

#### 3.8.1 H.265 encode paths (added 2026-10-08; deferred — REQ-ENC-002; not in current scope)

*(Scope, 2026-10-08.)* H.265 was required by REQ-ENC-001 from the owner decision of 2026-10-07 until the owner deferred it on 2026-10-08 ("H.264 only for now", OQ-103). It is now REQ-ENC-002 (DEFERRED), and this section is kept as its evidence. H.265 would be software-encoded on every candidate (reasoning from [D-24], [D-31]; RISK-022, not in current scope). The register gives these facts; none has been measured on PACSCORDER hardware. The columns follow the bring-up boards (ADR-004); Pi 4 Model B shares CM4's BCM2711 and Pi 5 shares CM5's BCM2712.

| | CM4 (BCM2711, Cortex-A72) | CM5 (BCM2712, Cortex-A76) | Facts |
|---|---|---|---|
| Hardware HEVC encoder | None: `bcm2835-codec` has no HEVC; the HEVC driver is a decoder only | None | [D-24], [D-31] |
| Encoder library | Debian x265 4.1-2 (`libx265-215`), not overridden by Raspberry Pi; Debian's build enables arm64 assembly and links Main, Main10 and Main12 into one library | Same | [H-01], [H-02], [H-03] |
| Framework entry points | FFmpeg 7.1.5 (Raspberry Pi build): `libx265` is configured and linked. GStreamer 1.26.2: `x265enc` in `gstreamer1.0-plugins-bad` (Raspberry Pi build) | Same | [H-08], [H-09], [H-11], [H-12] (CORRECTED) |
| Other HEVC encoders | kvazaar and the HM reference encoder are packaged in trixie but wired into neither distribution media stack; HM is not suitable for real-time use; SVT-HEVC is not packaged | Same | [H-18] (CORRECTED) |
| Input format | Planar only; packed UYVY from the TC358743 must be converted first (section 3.7) | Same | [H-10] (CORRECTED), [H-13], [H-43] (reasoning) |
| x265 SIMD | Neon only: Cortex-A72 is Armv8-A + CRC, no DotProd | Neon DotProd kernels can apply: Cortex-A76 is Armv8.2-A with DotProd. I8MM, SVE and SVE2 apply on neither board. x265 enables DotProd only when `AT_HWCAP` reports ASIMDDP | [H-04], [H-05] |
| x265 version | x265 4.0 added Arm SIMD that its release notes describe as "up to 57% faster encoding compared to release 3.6"; 4.1 lists no new Arm SIMD work. Trixie's 4.1-2 lacks the 4.2 and 4.3 AArch64 speed-ups; only their Neon parts could help these cores | Same | [H-06], [H-07] (OQ-105) |
| Cost evidence (not PACSCORDER measurements) | A community benchmark reports 4.33 FPS for `libx265` "Live" on a Pi 400 (Cortex-A72 @ 1.8 GHz) | A community benchmark reports 10.00 FPS for `libx265` "Live" on a Pi 5 (Cortex-A76 @ 2.4 GHz) | [H-20], [H-21] (community) |

What the cost evidence does and does not show:

- Research found no raspberrypi.com figure for HEVC software encode cost (site search; absence cannot be proven exhaustively). A Raspberry Pi engineer (6by9) stated on the official forum that software H.265 encode "is too intensive an operation to perform at any significant resolution" [H-19] (reported; community source).
- The benchmark behind [H-20] and [H-21] is not a 1080p60 live measurement. It encodes vbench clips with `-threads 1` and an x265 snapshot from 2022 that predates x265 4.0's Arm optimisations [H-22] (reported; community source).
- Reasoning: in that harness `libx265` was about 6.6 times slower than `libx264` on Pi 5. This is a relative cost only, not a prediction of PACSCORDER throughput [H-23] (reasoning-tier entry).
- Measurement: OQ-104 (throughput, latency, CPU headroom on CM4 and CM5), OQ-105 (SIMD paths active), in TEST-ENC-001 and TEST-PERF-001. *(2026-10-08: these H.265 runs are deferred, not run in current scope; OQ-104 and OQ-105 stay OPEN until REQ-ENC-002 is re-activated.)*

Latency-relevant encoder behaviour:

- x265 4.1 `tune=zerolatency` sets B-frames 0, lookahead 0 and one frame thread; wavefront (WPP) row parallelism stays on [H-16]. The `ultrafast` preset on its own keeps 3 B-frames [H-17].
- GStreamer 1.26.2 `x265enc` reports a hard-coded latency of 5 frames unless `tune=zerolatency` (then 0); latency computed from the real parameters arrived only in 1.26.8 and is not backported [H-15].
- FFmpeg's `libx265` wrapper copies its thread count into x265's frame threads after applying preset and tune [H-10] (CORRECTED). Reasoning from [H-10] and [H-16]: the `-threads` setting therefore replaces `tune=zerolatency`'s single frame thread unless it is set explicitly (research gap, topic H; OQ-104).

Concurrency (reasoning; not measured):

- On CM5 both codecs are software [D-31], [G-22]. If WebRTC keeps an H.264 track for browser reach (section 3.9.2) while another output uses H.265, CM5 runs two concurrent software video encodes plus the UYVY conversion (OQ-104, OQ-108).
- On CM4 the H.264 encodes can stay on the hardware encoder [D-10], while H.265 runs on the Cortex-A72 cores [H-05].

Licensing: x265 is licensed under GPL version 2 or later, and also under a commercial MulticoreWare licence; neither licence covers HEVC patents [H-39]. HEVC patent pools charge per unit [H-40], [H-41], [H-42] (RISK-015, OQ-087, OQ-109; [RELEASE.md](RELEASE.md)).

**Decisions.** ADR-004 (OPEN); ADR-007 (OPEN). *(Added 2026-10-08.)* H.265 evidence is recorded in the ADR-004 analysis in [DECISIONS.md](DECISIONS.md); which outputs use H.265 was OQ-103, ANSWERED 2026-10-08: none for now (H.264 only; REQ-ENC-002 DEFERRED).

**Implementation status.** NOT STARTED. TEST-ENC-001: BLOCKED — HARDWARE REQUIRED (H.264 runs on CM4 and CM5; H.265 runs deferred, not run in current scope — REQ-ENC-002; RISK-022 not in current scope). *(Added 2026-10-08.)* After the owner's answer to OQ-005, TEST-ENC-001 must include a two-encode run, recording and live at the same time, on CM4 (OQ-115, RISK-002) and on CM5 (OQ-059, RISK-003). TEST-PERF-001 must cover the combined load: two video encodes, the AAC and Opus audio encodes, recording, RTMP and WebRTC at once. These are scope notes, not accepted criteria ([TESTING.md](TESTING.md)). *(Added 2026-10-09; OQ-116, research topic K.)* For the < 1 s target, the encode term has to be measured per frame while both encodes run: on CM4 the hardware encode time at 1080p (OQ-115, OQ-125), on CM5 the x264 time on BCM2712 (OQ-059); reasoning (CORRECTED) [K-45]. Scope note, not an accepted criterion.

### 3.9 Layer 9 — Recorder / RTMP / WebRTC

**What it is.** The userspace consumers of the encoded stream. Their design responsibilities are in [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md).

| Output | Requirement | Constraints from sources | Open |
|---|---|---|---|
| Recorder | REQ-REC-001 | Container, storage medium, duration and power-loss behaviour are UNDEFINED. Debian trixie's `gstreamer1.0-plugins-good`, which the Raspberry Pi archive does not override, ships the `isomp4` (MP4), `matroska` and `flv` plugins [G-26]. It is not installed in the Lite image [G-32]. *(Added 2026-10-08.)* HEVC (deferred — REQ-ENC-002; not in current scope): GStreamer 1.26.2 `qtmux`/`mp4mux` and `matroskamux` accept H.265 after `h265parse`; `matroskamux` warns that the `hev1` form is not officially supported [H-37]. FFmpeg 7.1.5's MP4 muxer tags HEVC as `hev1`, `hvc1` or `dvh1`, and its Matroska muxer handles HEVC [H-38]. Audio (required): AAC from FFmpeg `aac` (CORRECTED), `voaacenc` or `avenc_aac` [I-40], [I-44], [I-46]. *(2026-10-08.)* Video comes from the recording encode, separate from the live encode (owner answer to OQ-005). *(Superseded in part 2026-10-09: owner decisions of 2026-10-08, ADR-009 ACCEPTED — container MP4, written fragmented for power-loss safety; every recording mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure; the HDD writer decoupled (OQ-117). The maximum recording duration is still UNDEFINED (OQ-006). Section 3.9.3.)* | OQ-006; added 2026-10-08: OQ-111, OQ-112; OQ-005 (recording-encode parameters); added 2026-10-09: OQ-117 to OQ-124 |
| RTMP | REQ-STR-001 | Legacy RTMP/FLV carries H.264 and AAC; HEVC and Opus need Enhanced RTMP [F-31]. GStreamer `flvmux` needs AVC-format H.264 and raw AAC [F-34] (CORRECTED). `rtmp2sink` publishes over RTMP and RTMPS [F-33]. *(Added 2026-10-08.)* HEVC over RTMP (deferred — REQ-ENC-002; not in current scope): FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV [H-26]; GStreamer 1.26.2 `flvmux` cannot carry H.265 [H-27]. Section 3.9.1. *(2026-10-08.)* Video comes from the live encode, shared with WebRTC (owner answer to OQ-005). Reasoning: RTMP therefore carries the WebRTC-receivable stream described below. Whether each destination accepts it is not in the register (OQ-007). *(Added 2026-10-09; OQ-116 ANSWERED.)* Latency is best-effort and set by the receiving platform (owner, 2026-10-08). YouTube Live's lowest mode, ultra-low, is "less than 5 seconds" for most viewers [K-15]; reasoning (CORRECTED): YouTube documents no sub-second mode and names the player's read-ahead buffer as the main source of latency, so PACSCORDER's encoder settings cannot make that output meet < 1 s [K-18]. YouTube recommends 2 s keyframes (not over 4 s), CBR and 2 B-frames; the B-frame advice conflicts with the B-frame-free live encode [K-17] (OQ-127, RISK-019). | OQ-007, OQ-075, OQ-076; added 2026-10-08: OQ-106, OQ-107 (both not in current scope); added 2026-10-09: OQ-116 (ANSWERED), OQ-127 |
| WebRTC | REQ-STR-002 | Browsers must implement H.264 Constrained Baseline, with SPS/PPS in-band [F-36]. Reasoning: 1080p needs Level 4 or higher, which a `42e01f` (Level 3.1) negotiation does not cover [F-38], [F-40]. WebRTC endpoints must implement Opus and G.711; AAC is not a required WebRTC codec, so AAC audio has to be transcoded for browser playback [F-41]. `webrtcbin` has no built-in signalling [F-42]. *(Added 2026-10-08.)* H.265 (deferred — REQ-ENC-002; not in current scope): RFC 7742 does not require it [H-32]; Chrome 136+ enables it only where the platform decodes HEVC in hardware, with no software fallback [H-33]; Safari 18.0 supports the standard HEVC RTP payload [H-34]; no evidence of Firefox support was found [H-35]; a Microsoft Q&A answer reports that Edge 147 does not enable it by default [H-36] (reported; community source). GStreamer 1.26.2 `rtph265pay` lacks profile, tier and level in its caps (added in 1.26.4) [H-31]. Opus: FFmpeg `libopus` and GStreamer `opusenc` accept only 48, 24, 16, 12 or 8 kHz [I-41], [I-47]. *(2026-10-08.)* Video comes from the live encode, shared with RTMP (owner answer to OQ-005). *(Added 2026-10-09; OQ-116 ANSWERED.)* Target: under 1 s camera-to-viewer for WebRTC viewers (owner, 2026-10-08; REQ-STR-002). MediaMTX documents that browsers deliberately do not support H.264 B-frames over WebRTC, recommends Baseline with Opus, and lists Opus, G722 and G711 as its WebRTC audio codecs [K-04]. Section 3.9.4. | OQ-008, OQ-073, OQ-074; added 2026-10-08: OQ-108 (not in current scope), OQ-111; added 2026-10-09: OQ-116 (ANSWERED), OQ-125 to OQ-128 |

Whether one encode feeds all three outputs, or each gets its own encode, is UNDEFINED (OQ-005). **Reasoning:** a shared H.264 stream would have to meet the strictest consumer:

- WebRTC's Constrained Baseline profile [F-36];
- no B-frames — reported by the MediaMTX project as a browser limitation [F-45] (reported; community source).

The Pi 4/CM4 hardware encoder offers Constrained Baseline [D-11] and produces no B-frames [D-14].

*(Superseded in part 2026-10-08 — owner answer to OQ-005: "Separate record + live".)* There are two encodes: the recorder has its own, and RTMP and WebRTC share one live encode. Reasoning from that decision:

- **The strictest-consumer rule now applies to the live encode only.** It must be WebRTC-receivable:
  - Constrained Baseline [F-36].
  - Level: libwebrtc assumes Constrained Baseline Level 3.1 when `profile-level-id` is absent [F-39], while 1080p needs Level 4 or higher and 1080p60 Level 4.2 [F-40] (OQ-073, RISK-019).
  - No B-frames, as reported by the MediaMTX project [F-45].
  - SPS/PPS in-band [F-36]. On the Pi 4/CM4 encoder that means switching on `REPEAT_SEQ_HEADER`, which is off by default [D-15].
- **RTMP receives the same live stream.** Whether each RTMP destination accepts it is OQ-007.
- **The recording encode is not bound by these constraints.** Its profile, level, bitrate and rate control are UNDEFINED (OQ-005).
- **Topology.** The live bitstream is split after its encoder, one branch to the FLV muxer and one to the RTP packetiser. How it is split is part of ADR-007 (OPEN).

#### 3.9.1 HEVC over RTMP: the framework constraint (added 2026-10-08; deferred — REQ-ENC-002; not in current scope)

*(Scope, 2026-10-08.)* OQ-103 is ANSWERED: RTMP carries H.264 only, so this constraint does not apply in the current scope and, per the ADR-007 scope note in [DECISIONS.md](DECISIONS.md), does not drive the framework choice. RISK-025 stays OPEN, not in current scope. The section is kept as evidence for REQ-ENC-002.

| Component (Raspberry Pi OS trixie) | Can it put HEVC into FLV/RTMP? | Facts |
|---|---|---|
| Enhanced RTMP specification (v2-2026-01-31-r2) | Defines the HEVC FourCC `hvc1` and lists it among the `fourCcList` connect-command values | [H-24] |
| FFmpeg 7.1.5 (Raspberry Pi build) | **Yes.** It maps HEVC to `hvc1` and writes enhanced video headers; `rtmp_enhanced_codecs` writes a `fourCcList` but accepts only `hvc1`, `av01` and `vp09`. FFmpeg 6.1 was the first release with HEVC in FLV. Its FLV audio table has AAC but **no Opus**. | [H-25], [H-26] |
| GStreamer 1.26.2 `flvmux` | **No.** Its video sink pad has no H.265. | [H-27] |
| GStreamer `eflvmux` | First appears in the 1.28 branch; absent from 1.24 and 1.26 | [F-34] (CORRECTED) |
| SRT / MPEG-TS instead of RTMP | The components exist: 1.26.2 `mpegtsmux` accepts H.265, plugins-bad ships the SRT and MPEG-TS plugins, and the Raspberry Pi FFmpeg is built with `--enable-libsrt` | [H-30] |

Destinations: YouTube Live lists H.264, H.265 and AV1 over RTMP/RTMPS with AAC or MP3 audio; the page does not use the term "Enhanced RTMP" [H-29]. MediaMTX's documentation lists H265 among RTMP publish and read codecs [H-28]. Whether each required destination accepts FFmpeg's signalling is OQ-106.

**Consequence for the architecture (reasoning from [H-26], [H-27], [F-34]).** If RTMP must carry H.265 (OQ-103) and GStreamer is chosen for capture, recording or WebRTC (ADR-007), HEVC RTMP needs one of: FFmpeg 7.1.5 for that output, a backported or newer GStreamer carried outside the distribution (ADR-003), or SRT/MPEG-TS instead of RTMP (OQ-076). The first is a split GStreamer/FFmpeg design, and the two frameworks also align audio and video timestamps differently [I-35], [I-38] (section 3.11). Tracked as RISK-025 and OQ-107; the choice belongs to ADR-007.

#### 3.9.2 Fan-out with two video codecs and audio (added 2026-10-08; video part deferred — REQ-ENC-002; not in current scope)

Reasoning, not a design; the number of encodes is OQ-005 and the codec per output is OQ-103. *(2026-10-08: OQ-103 is ANSWERED — H.264 on every output — so the video bullet applies only if REQ-ENC-002 is re-activated; the audio bullet is in current scope.)* *(2026-10-08, later: the number of video encodes is answered — two H.264 encodes, recording and shared live (owner answer to OQ-005, "Separate record + live").)*

- **Video (deferred — REQ-ENC-002; not in current scope).** An H.265-only WebRTC stream would not play in Firefox [H-35], would play in Chrome only where the platform decodes HEVC in hardware [H-33], and is reported not to be enabled in Edge by default [H-36] (reported; community source). An H.264 WebRTC track therefore has to remain for browser reach (RISK-019, OQ-108). If recording or RTMP uses H.265 at the same time, PACSCORDER runs at least one H.264 and one H.265 encode (section 3.8.1).
- **Audio.** FFmpeg's FLV muxer has no Opus [H-26], and WebRTC endpoints must implement Opus or G.711 while AAC is not required [F-41]. Simultaneous RTMP and WebRTC with audio therefore need two audio encodes, AAC and Opus. A 44.1 kHz source also needs resampling before Opus [I-41], [I-47] (OQ-063, OQ-111). *(2026-10-08: unchanged by the two video encodes. Audio is still two encodes: one AAC encode for recording and RTMP, one Opus encode for WebRTC.)*

#### 3.9.3 Recorder: fragmented MP4 mirrored to two drives (added 2026-10-09)

*(Added 2026-10-09; ADR-009 / research topic J.)* Nothing here has been built or run on PACSCORDER hardware. Detail for the recorder is in [RECORDING.md](RECORDING.md); the userspace component is [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) section 5.4.1.

**What is decided (ADR-009, ACCEPTED 2026-10-08).** The first three points are owner decisions; the fourth is not.

1. Container MP4, written **fragmented** for power-loss safety. The fragment duration is set in implementation (NEEDS VERIFICATION by TEST-REC-001; OQ-118).
2. Every recording is written to **both** the PCIe NVMe SSD and the USB-to-SATA HDD (mirror).
3. The HDD is powered from a **self-powered enclosure or hub**, never from the board's USB VBUS. *(The owner's words were "Self-powered enclosure"; "or hub" in ADR-009 decision 3 is Claude's addition, not an owner decision.)*
4. ext4 on the recording volumes. This is **Claude's proposal inside ADR-009**, based on [J-29] and [J-35] (CORRECTED); **the owner did not decide it** (OQ-120).

The maximum recording duration is still UNDEFINED (OQ-006).

**Data path (reasoning from ADR-009; not a design).**

```text
recording encode (H.264, layer 8) + AAC audio (section 3.11)
     |
     v
fragmented MP4 muxer -- framework OPEN (ADR-007); fragment duration OQ-118
     |
     +--> file writer --> NVMe SSD volume (ext4 proposed, OQ-120)
     |
     +--> HDD branch, decoupled: buffer size and overflow policy OQ-117
            --> file writer --> USB-to-SATA HDD volume, self-powered enclosure (ext4 proposed, OQ-120)
```

*(Diagram added 2026-10-09.)* One muxer is drawn for brevity. Whether one muxer feeds both writers or each writer has its own muxer is open under ADR-007; concurrent writing to two file sinks has not been tested (research gap, topic J; OQ-117).

**Why fragmented (from sources).**

- In GStreamer 1.26.2's default (moov-at-end) mode the moov is written only at EOS, so an unclean stop leaves an unplayable MP4 unless fragmented or robust mode is used [J-38].
- GStreamer `mp4mux` writes a fragmented file when `fragment-duration` > 0, and that setting silently disables robust muxing [J-39].
- FFmpeg 7.1 documents that a fragmented file stays decodable if writing is interrupted, at the cost of lower compatibility with other applications [J-45] (RISK-029, OQ-118).
- Robust muxing [J-40], which fsyncs each moov rewrite [J-41], was not chosen by ADR-009.

**The HDD writer must be decoupled (ADR-009 Consequences; OQ-117, RISK-028).**

- The register's only HDD timing is for one Seagate BarraCuda 2.5-inch family: 2.5 s typical and 3.0 s maximum from standby to ready (CORRECTED) [J-30]. It is an example, not a general HDD figure. Other stall sources named in OQ-117: a UAS reset (RISK-027), a periodic `fsync()`, and on CM4 the shared USB 2.0 path (RISK-026).
- Reasoning (RISK-028): if the HDD writer blocks and its branch has no buffer that can absorb or drop, the stall propagates back to the recording encode, then to the NVMe copy, and, if it reaches the capture queue, into the live path, where each waiting frame adds one frame period [K-36]. On CM4 the recording and live encodes would share one hardware encoder; whether it runs both at once is OQ-115. A 3.0 s stall is three times the whole < 1 s live budget.
- Reasoning from [J-30] (CORRECTED) and [J-36] (OQ-117; not a design figure): riding out a 3.0 s stall at about 3.15 MB/s per drive (25 Mbit/s video plus AAC) needs about 9.5 MB of HDD-branch buffering, before any margin.
- Research design risks (topic J — not register facts): the HDD branch needs a large or leaky queue; spin-down should be disabled or extended where the bridge allows. The buffer size and the overflow policy (drop and mark the HDD copy incomplete, or stop the HDD copy) are UNDEFINED: BUILD TEST REQUIRED; HARDWARE TEST REQUIRED (OQ-117).

**Storage attachment per board (from sources; no drive, adaptor or enclosure has been chosen, OQ-121, OQ-122).**

| | CM4 on the CM4 IO Board | CM5 on the CM5 IO Board | Facts |
|---|---|---|---|
| PCIe for the NVMe SSD | CM4 has one PCIe Gen 2 x1 lane (5 Gbps) [J-01]. The IO Board's single PCIe Gen 2 x1 socket takes standard PC cards and has been used with an NVMe drive through a passive adaptor [J-03]; Raspberry Pi's NVMe documentation names that slot [J-11] | CM5 has one PCIe Gen 2 lane; Gen 3 "is unsupported and might not function reliably" [J-12]. The IO Board's M.2 M-key slot runs at PCIe Gen 2 x1 by default (Gen 3 experimental, unsupported) [J-13] and takes 2230 to 2280 drives [J-14] | [J-01], [J-03], [J-11], [J-12], [J-13], [J-14] |
| Link enablement and power | The slot is powered only from the 12 V barrel input; with a 5 V-only PoE HAT, PCIe cards do not work [J-05] | In the `rpi-6.18.y` device tree the M.2 link (`pcie1`) is disabled by default; `dtparam=pciex1` (alias `nvme`) controls it [J-15], [J-17]. Whether the product image must set it: OQ-124 | [J-05], [J-15], [J-17] |
| Host-controller notes | No 64-bit accesses from the ARM; the two official datasheets disagree on MSI-X [J-02] (OQ-123) | The NVMe link and RP1 (which carries USB) are on separate PCIe controllers [J-17] | [J-02], [J-17] |
| USB for the HDD | One USB 2.0 port (480 Mbps) [J-01]; no USB 3.0 controller [J-08]. The IO Board's USB2514B hub shares one VBUS switch of about 1.2 A across its ports, and plugging in the micro-USB cable disables the hub [J-06]. Reasoning (CORRECTED): with NVMe in the socket, the HDD runs at USB 2.0 through that hub, shared with every other USB device; USB 3 would need a PCIe switch, and booting through a switch is not supported [J-09], [J-04] | Two USB 3.0 interfaces, each up to 5 Gb/s at the same time [J-18]; RP1's two xHCI controllers give each downstream port independent bandwidth [J-21]. The IO Board's two USB 3.0 Type-A ports share about 1.2 A of VBUS [J-19] | [J-01], [J-04], [J-06], [J-08], [J-18], [J-19], [J-21]; [J-09] (CORRECTED; reasoning) |
| HDD power (self-powered, ADR-009) | Reasoning: a bus-powered 2.5-inch HDD needing 1.0 A to spin up would leave only about 0.2 A of the shared switch for the bridge and every other USB device [J-31] | Reasoning: it would exceed the 600 mA peripheral limit on a 3 A supply and leave about 0.2 A on a 5 A supply [J-31], [J-20] | [J-20], [J-30] (CORRECTED), [J-31] (reasoning) |
| USB-to-SATA bridge driver | `uas` cannot bind behind `dwc2`, but can under Raspberry Pi OS's default `otg_mode=1` controller [J-24], [J-07] | RP1 xHCI [J-21]; the bound driver is to be recorded on hardware (OQ-122) | [J-07], [J-21], [J-24]; RISK-027 |
| Risks / open | RISK-026, RISK-027; OQ-121, OQ-122, OQ-123 | RISK-027; OQ-121, OQ-122, OQ-124 | — |

Pi 4 Model B and Pi 5 were not researched for USB and storage in topic J, apart from Pi 5 NVMe boot and Gen 3 [J-10], [J-16]: NEEDS VERIFICATION (OQ-098). CM5 on the CM4 IO Board was not researched either: NEEDS VERIFICATION.

**Bandwidth (reasoning).** Per drive, H.264 at 8 / 12 / 25 Mbit/s plus 192 kbit/s AAC writes about 1.02 / 1.52 / 3.15 MB/s, and the mirror doubles the system total to about 2.05–6.30 MB/s [J-36]; these bitrates are research examples, not an owner choice (OQ-005). Even the highest rate is about 5.2 % of USB 2.0's 480 Mbit/s signalling rate and about 0.63 % of the ~4 Gbit/s a PCIe Gen 2 x1 or USB 3.0 link carries after 8b/10b coding, so interface bandwidth is not the recording bottleneck on either module; on CM4 the USB 2.0 bus limits only offload and copy times [J-37]. Claude's reading (OQ-117): stalls, not the average rate, are the concern.

**Power loss and filesystem (from sources).**

- ext4 defaults to `data=ordered`, `barrier=1` and `commit=5`: a power loss loses at most the last 5 s of metadata changes without damaging the filesystem, but because of delayed allocation even older data can be lost (CORRECTED) [J-35]. Whether fragmented mode forces each fragment to storage is not in the register: UNKNOWN — VERIFICATION REQUIRED; HARDWARE TEST REQUIRED (OQ-119, RISK-030). Research design risk (topic J — not a register fact): even fragmented MP4 loses the last seconds still in the page cache or drive cache.
- The CM4 and CM5 defconfigs build ext4, VFAT, NVMe, USB storage and UAS into the kernel, and exFAT as a module [J-29].
- Raspberry Pi's storage guide documents `nofail`, and `x-systemd.device-timeout=30` shortens the 90 s boot wait for an absent disk to 30 s rather than removing it (CORRECTED) [J-33]. How a missing HDD is kept from delaying boot or blocking the NVMe copy is OQ-120.

**Long recordings.** GStreamer `splitmuxsink` can start a new file at a keyframe before a size or time limit is crossed [J-44] (candidate; ADR-007). Reasoning from [J-45] (OQ-118): with FFmpeg's `+frag_keyframe`, the recording encode's keyframe interval sets the fragment length. File splitting depends on the maximum duration (OQ-006).

**Key constraints.** RISK-026, RISK-027, RISK-028, RISK-029, RISK-030.

#### 3.9.4 Live path and the < 1 s WebRTC target (added 2026-10-09)

*(Added 2026-10-09; OQ-116 / research topic K.)* Nothing here has been measured on PACSCORDER hardware. Detail is in [STREAMING.md](STREAMING.md); the full latency budget is in [PERFORMANCE.md](PERFORMANCE.md).

**Target (owner decisions of 2026-10-08).**

- WebRTC viewers: under 1 s camera-to-viewer (REQ-STR-002; OQ-116 ANSWERED).
- RTMP outputs: best-effort; latency set by the receiving platform (REQ-STR-001). YouTube's lowest documented mode is "less than 5 seconds" for most viewers [K-15]; reasoning (CORRECTED): encoder settings cannot make YouTube meet < 1 s [K-18].
- Reasoning (CORRECTED): LL-HLS through MediaMTX defaults is unlikely to reliably reach under 1 s, and regular HLS cannot [K-26]. HLS is therefore not a route to the target.
- Still open: bitrate and rate control (OQ-005); viewer reach, LAN or internet (OQ-008).

**Candidate publishing route (reasoning recorded under ADR-007, OPEN — not a decision).**

```text
live encode (H.264, B-frame-free) + Opus audio
     |
     v
RTSP publish from GStreamer (rtspclientsink)   RTSP-client publishing: MediaMTX's recommended GStreamer path [K-05]
     |                                         rtspclientsink latency default 2000 ms [K-09]; effect on the sender: OQ-126
     v
MediaMTX                                       relay latency undocumented; no numeric WebRTC figure [K-03] (OQ-125)
     |  WebRTC readers, including WHEP (reported by the MediaMTX project [F-45]); WHEP is an Internet-Draft [K-02]
     v
browser viewer: own jitter buffer, decode, render (undocumented; OQ-125)
```

*(Diagram added 2026-10-09.)* Reasoning from [F-45], [K-05] and [K-06], as recorded under ADR-007: with the packages in Debian trixie and the Raspberry Pi archive, this is the candidate GStreamer route to browser viewers, because `webrtcsink` and `whipclientsink` (gst-plugins-rs) are not packaged there [K-06] (RISK-034). It is not the only option: ADR-007 also names a self-built gst-plugins-rs for WHIP, and GStreamer's `webrtcbin` (gst-plugins-bad) implements most of the RTCPeerConnection API but has no built-in signalling transport [F-42], so using it means providing signalling (reasoning). Publishing over WHIP with `whipclientsink` would need GStreamer 1.22 or later and, for H.264, the Baseline profile [K-05]. For FFmpeg and a direct application the WebRTC route has no register fact (NEEDS VERIFICATION, as in ADR-007).

**Latency-relevant design rules (candidates; reasoning from sources; none tested).**

| Rule | Evidence | CM4 | CM5 | Open |
|---|---|---|---|---|
| No B-frames on the live encode | MediaMTX documents that browsers deliberately do not support H.264 B-frames over WebRTC [K-04]; B-frames make x264 hold frames back [K-28] | The encoder never emits B-frames [K-30] | `tune=zerolatency` [K-27], [K-28]; otherwise GStreamer `x264enc` runs x264's medium preset with 3 B-frames unless `tune=zerolatency`, an explicit `bframes=0` or `profile=baseline` in caps is set (CORRECTED) [K-29] | OQ-127, RISK-019 |
| Keyframes on demand | Reasoning from [K-30] (OQ-127): the CM4 encoder's default GOP of 60 is 2 s at 30 fps, so without an on-demand keyframe a new viewer may wait up to about 2 s for a decodable frame | Force-keyframe is implemented and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe [K-31] | No register fact on requesting keyframes from `x264enc` (NEEDS VERIFICATION) | OQ-127: whether MediaMTX passes a viewer's keyframe request back to an RTSP publisher is a research open question |
| Set every element latency explicitly | Defaults: `webrtcbin` 200 ms [K-07]; `rtpbin` and `rtpjitterbuffer` 200 ms [K-08]; `rtspclientsink` and `rtspsrc` 2000 ms, while MediaMTX's own reader examples set `rtspsrc latency=0` [K-09]. Reasoning: the jitter-buffer values apply only to RTP received inside the live path [K-10] | Same | Same | OQ-126, RISK-032 |
| Keep capture and queues short | Unicam and RP1 CFE stamp each buffer at frame start and complete it at frame end: on CM4 a frame reaches userspace at least one frame's readout time after its first line [K-34]; on CM5 capture costs about one frame's readout time [K-35]. `v4l2src` reports a minimum latency of one frame and a maximum of pool depth × frame duration, with a pool minimum of 2 [K-37]. Reasoning: each waiting filled buffer adds one frame period [K-36] | Unicam drops frames into a dummy buffer when none is queued [K-34] | CFE uses `min_queued_buffers=1` [K-35] | OQ-126 (leaky queues are a research open question) |
| Do not rely on the pipeline's own latency report | `v4l2h264enc` on BCM2711 reports 0 latency [K-32]; reasoning (research design risk, topic K): latency and A/V-sync calculations then leave out the real encode time | Applies | — | RISK-024, OQ-112, OQ-125 |
| Keep the HDD branch away from the live path | Section 3.9.3 | Shared hardware encoder (OQ-115) | Shared CPU (OQ-059) | OQ-117, RISK-028 |

Research design risk (topic K — not a register fact): on CM5, if x264 cannot finish a frame within 33.3 ms, queues grow by one frame period per queued frame unless the live branch has leaky queues or a lower resolution or frame rate (RISK-032; OQ-059).

**Budget summary (reasoning, CORRECTED; a labelled budget, not a measurement) [K-45].** For a CM4 1080p30 WebRTC/WHEP viewer on a LAN, the documented or extrapolated terms are about 56 ms typical and about 75 ms worst case: capture readout about 33.3 ms [K-34]; hardware encode about 23 ms, the 720p Pi 4 figure that a Raspberry Pi engineer stated (community source) [K-33] scaled by macroblocks; and no frames waiting in queues [K-36]. That leaves about 925–945 ms for undocumented terms: the HDMI source or ATEM, the TC358743 (its internal buffering is undocumented [K-38]), any pixel-format conversion, the MediaMTX relay, the LAN, and the browser's jitter buffer, decode and render. The encode term assumes the live encode has the CM4 hardware encoder to itself, but the recording encode would share it (OQ-115). On CM5 the encode term is unknown until x264's per-frame time on BCM2712 is measured (OQ-059); Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39]. Neither module is shown to meet the target (RISK-031, OQ-125). Full budget: [PERFORMANCE.md](PERFORMANCE.md).

- Reference only, not this stack: a third-party CDN documents WHEP playback "with less than 500 milliseconds of latency" [K-14].
- Viewer-side measurement inputs: the W3C statistics API defines `jitterBufferDelay`, `jitterBufferEmittedCount`, `jitterBufferTargetDelay`, `jitterBufferMinimumDelay` and `totalProcessingDelay` on the viewer's inbound stream (CORRECTED) [K-12]; `RTCRtpReceiver.jitterBufferTarget` is a hint of at most 4000 ms that influences but does not set the browser's target [K-11]. Measurement method: OQ-125, TEST-STR-002.
- Audio: Opus frames are 2.5–60 ms and GStreamer `opusenc` defaults to 20 ms [K-40]; audio-path latency and browser A/V sync are unmeasured (research gap, topic K; OQ-112, OQ-125).

**MediaMTX behaviour and internet viewers (from sources).** MediaMTX documents that it favours real-time delivery over reliability: most protocols run over UDP so late packets can be dropped, and a full outgoing circular buffer (`writeQueueSize`, default 512) drops packets [K-44]. Its v1.21.1 configuration listens for WebRTC on UDP `:8189` and leaves TCP disabled, because, it says, TCP adds a progressive delay under congestion [K-41]. Its documentation says internet clients need the public IP or DNS name in `webrtcAdditionalHosts`, or STUN/TURN when the listeners cannot be reached [K-42], and lists four connection methods [K-43]. Whether internet viewers are required is OQ-008 (RISK-033, OQ-128).

**Key constraints.** RISK-019, RISK-024, RISK-031, RISK-032, RISK-033, RISK-034.

**Decisions.** ADR-007 (OPEN). *(Added 2026-10-08.)* ADR-007 is to record the HEVC-over-RTMP path (OQ-107) and the A/V clock model (OQ-112). *(2026-10-08: the HEVC-over-RTMP path is not in current scope — H.265 deferred, REQ-ENC-002; ADR-007 scope note. The A/V clock model is.)* *(Added 2026-10-09; ADR-009 / OQ-116.)* ADR-009 (ACCEPTED 2026-10-08) governs the recorder (section 3.9.3). Per the ADR-007 Decision note in [DECISIONS.md](DECISIONS.md), ADR-007 is also to record the live publishing route (RISK-034), the explicit latency of every buffering element in the live path (OQ-126, RISK-032), the fragmented-MP4 settings (OQ-118) and the HDD-branch decoupling (OQ-117); its status is unchanged (OPEN).

**Implementation status.** NOT STARTED. TEST-REC-001, TEST-STR-001 and TEST-STR-002: BLOCKED — HARDWARE REQUIRED. *(Added 2026-10-09; scope notes, not accepted criteria — [TESTING.md](TESTING.md).)* TEST-REC-001 is to cover both drives, power cuts (OQ-119) and injected HDD stalls with no gap in the NVMe copy (RISK-028); TEST-STR-002 is to measure camera-to-viewer latency on CM4 and CM5 with the recording encode running (OQ-125, RISK-031).

### 3.10 Paths outside the mandated stack

These connect to the stack but are not layers of Rule 5.

| Path | Where it attaches | What is known | Status |
|---|---|---|---|
| HDMI audio (REQ-CAP-006, PROPOSED; owner decision OQ-004). *(2026-10-07: REQUIRED — REQ-CAP-006 DRAFT, OQ-004 ANSWERED.)* | Layer 1 wiring → ALSA → layer 9 muxers | The chip can output audio on I2S or TDM, or send it over CSI-2 [A-11], [A-05]; the Linux driver always configures 2-channel I2S output [A-13]. The `tc358743-audio` overlay expects that I2S on GPIO 18/19/20 and creates an ALSA card named `tc358743` [A-47], [G-14]. Reasoning: the I2S path is wiring separate from the CSI-2 cable. Behaviour on Pi 5/CM5 is unconfirmed (OQ-054). *(Added 2026-10-08: on CM5 the firmware does not block the overlay and its labels resolve to RP1 I2S1 [I-05], [I-06], [I-07], [I-09], but no source shows capture through it. Data path, rate tracking and A/V synchronisation: section 3.11.)* | NOT STARTED — RISK-014; added 2026-10-08: RISK-023, RISK-024 |
| Blackmagic ATEM integration (REQ-ATEM-001; scope recorded 2026-10-07, OQ-009 ANSWERED: HDMI capture only) | Layer 1 only: the ATEM's HDMI output is an HDMI source, like a camera connected directly (REQ-CAP-008). In the current scope this is not a separate path. | ATEM Mini Pro HDMI output is 1080p only, with no 720p or 1080i [F-23], and defaults to multiview [F-25]. **Not in current scope** (kept as reference in [ATEM.md](ATEM.md)): network tally/control over UDP 9910, a protocol reported as reverse-engineered by the OpenSwitcher project [F-11] (reported; community source), for which the official SDK supports Windows and macOS only [F-01]; and RTMP exchange, where receiving an ATEM RTMP push would need a listening RTMP server on PACSCORDER (reasoning) [F-46]. Models: OQ-102 (ANSWERED 2026-10-07: no model list; any HDMI camera plus ATEM outputs). ATEM program audio is embedded on HDMI [F-24] (CORRECTED), so it arrives through the audio side path of section 3.11 (added 2026-10-08). | NOT STARTED — RISK-008; RISK-018 applies only if network control is added |
| HDMI CEC (OQ-016) | Layer 2 | Optional in the driver behind `CONFIG_VIDEO_TC358743_CEC`, which the Raspberry Pi defconfigs do not enable [A-34], [B-20] | NOT STARTED; needed only if the owner requires CEC (OQ-016) |

### 3.11 HDMI audio side path (added 2026-10-08)

**What it is.** The path that carries the HDMI source's embedded audio to the recorder and streams. HDMI audio is required (REQ-CAP-006, DRAFT; owner, 2026-10-07; OQ-004 ANSWERED). It is not a Rule 5 layer: it does not travel over CSI-2 with the stock driver, which always selects I2S output [A-13], [I-24], although the silicon can also send audio over CSI-2 [I-26] (OQ-033). The facts below are from sources (topic I); nothing has been run on PACSCORDER hardware.

**Data path.**

```text
HDMI source (camera or ATEM output; REQ-CAP-008)
   |  embedded audio; source audio clock carried by ACR packets (N/CTS)
   v
TC358743   internal audio PLL tracks N/CTS, so I2S clocks follow the source [I-26]
           I2S output configured once at probe: 2 channels, I2S format [I-24]
           I2S master-clock only; 32-bit time slots; 16-24-bit data [I-25]
   |  A_SCK (BCK) -> GPIO 18, A_WFS (LRCK) -> GPIO 19, A_SD (DATA) -> GPIO 20 [A-47], [I-04]
   |  pins are VDDIO2 outputs, 1.8-3.3 V [I-27]; GPIO voltage should match, else level-shift [I-29]
   v
Pi CPU I2S, clock consumer (BC_FC)             [I-03], [I-09], [I-13], [I-28] (reasoning)
   CM4: bcm2835-i2s on GPIO 18-21 (ALT0)       [I-10], [I-13]
   CM5: RP1 I2S1 (rp1_i2s1) on GPIO 18-21      [I-07], [I-08], [I-09] -- unconfirmed (OQ-054)
   |  plus the "linux,spdif-dir" stub codec as bit-clock and frame master [I-02], [I-11]
   v
ALSA card id "tc358743" (simple-audio-card)    [I-03], [I-15]
   open as hw:CARD=tc358743,DEV=0; select by card id, not by PCM name or index [I-15], [I-17] (reasoning)
   |
   v
userspace audio capture (SOFTWARE_ARCHITECTURE.md section 5.8)
   |  resample if the encoder needs it [I-47]
   v
AAC encoder (recording, RTMP) and/or Opus encoder (WebRTC) --> muxers of layer 9
```

**Per platform.**

| | CM4 | CM5 | Facts |
|---|---|---|---|
| Overlay | `tc358743-audio`, loaded together with `tc358743`; official documentation says audio needs `tc358743-audio` in addition [C-37]. A Raspberry Pi engineer stated that it "*requires* dtoverlay=tc358743 to be loaded too" [I-16] (reported; community source). | Same overlay. `overlay_map` has no entry for it, and an overlay not in the map is assumed compatible with all platforms, so the firmware does not block it [I-05], [I-06] | [C-37], [I-05], [I-06], [I-16] |
| CPU DAI behind `i2s_clk_consumer` | The single `bcm2835-i2s` (`i2s@7e203000`), pins GPIO 18–21 in ALT0 [I-10] | RP1 I2S1, pin group `rp1_i2s1_18_21`, function `i2s1` [I-07], [I-08]. `dwc-i2s` accepts the codec-master format only on a clock-consumer instance, which I2S1 is [I-09] | [I-07], [I-08], [I-09], [I-10] |
| Capture capability | Exactly 2 channels, 8–384 kHz, S16_LE/S24_LE/S32_LE; the requested rate never reaches hardware in this mode, the rate on the wire is set by the TC358743 [I-13] | Channel count and formats are read from RP1 hardware registers not visible in source; hw_params accepts 2, 4, 6 or 8 channels and S16/S24/S32_LE [I-14] | [I-13], [I-14] |
| Expected PCM name | `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15] | Reasoning from source: `1f000a4000.i2s-dir-hifi dir-hifi-0` [I-17] | [I-15], [I-17] |
| Where the audio V4L2 controls are | `/dev/videoN` in legacy Unicam mode; reasoning from [I-23]: with `media-controller` (ADR-006, PROPOSED) they are not copied there, so the sub-device node is used | TC358743 sub-device node only | [I-23] |
| Kernel options | `bcm2711_defconfig` sets `CONFIG_SND_SIMPLE_CARD=m`, `CONFIG_SND_BCM2835_SOC_I2S=m`, `CONFIG_SND_DESIGNWARE_I2S=m`, `CONFIG_SND_DESIGNWARE_PCM=y` and `CONFIG_VIDEO_TC358743=m`; the packaged `rpi-v8` kernel has `CONFIG_SND_SOC_SPDIF=m` | `bcm2712_defconfig` sets the same options; the packaged `rpi-2712` kernel has `CONFIG_SND_SOC_SPDIF=m` | [I-12] |
| Status | Documented path; NOT STARTED | Bring-up gate: no official statement or test shows capture through this path (research gap, topic I; OQ-054). Reasoning recorded in ADR-004: TEST-AUD-001 must pass on CM5 before CM5 can be chosen. | — |

Pi 4 Model B and Pi 5 were not researched separately in topic I, which covered the bring-up boards; their audio path is NEEDS VERIFICATION.

**Rate-tracking responsibility (userspace).**

- Reasoning from source: the kernel has no path that carries HDMI sample-rate changes into ALSA. The stub codec has no controls and no `hw_params`, the CPU I2S runs as clock consumer and ignores the requested rate, and nothing outside `tc358743.c` uses the rate control. If the application opens the card at 48000 Hz while the source sends 44100 Hz, the frames are labelled 48 kHz, play 8.84 % fast, and A/V drift accumulates [I-18] (reasoning-tier entry; RISK-023).
- What the driver offers: the rate as the read-only "Audio sampling rate" control (ID 0x00981980; 0 when no TMDS signal is present) and "Audio present" (0x00981981) [I-19], [I-20], with `V4L2_EVENT_CTRL` change events [I-21].
- So the architecture assigns this to userspace: read the rate before opening ALSA, subscribe to the control-change event, and reopen ALSA at the new rate [I-21]. Because the control lives on the TC358743 node, the component that owns that node (the capture control service) must pass rate changes to audio capture ([SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) sections 5.2 and 5.8; process split OQ-090).
- Detection latency: without a wired INT pin the driver polls every 1000 ms, so a change can take up to about 1 s, plus I2C time, to reach the control [I-22] (OQ-020). Audio captured in that window carries the old rate label (reasoning from [I-18], [I-22]).
- Alternative or complement: force one rate through the EDID audio descriptors (research design risk, topic I — not a register fact; OQ-002, OQ-111). The policy is OQ-111.

**A/V synchronisation design inputs.** Audio and video come from separate clock domains and meet only in userspace:

| Input | Fact | Facts |
|---|---|---|
| Audio sample clock | Follows the HDMI source's audio clock through the TC358743's audio PLL | [I-26] |
| Video timestamps | Every Raspberry Pi CSI receiver driver in `rpi-6.18.y` (Unicam and RP1 CFE variants) marks buffers `TIMESTAMP_MONOTONIC` and stamps them with `CLOCK_MONOTONIC` in its frame-start interrupt | [I-33] |
| ALSA timestamps | The application chooses the status-timestamp clock; alsa-lib 1.2.14 (the trixie version) switches each newly opened `hw` PCM to monotonic timestamps when the kernel PCM protocol is 2.0.9 or later | [I-34] |
| GStreamer 1.26.2 | `alsasrc` uses ALSA driver timestamps only when the element clock is a monotonic `GstSystemClock`; defaults are provide-clock=TRUE, slave-method=skew, buffer-time 200 ms, latency-time 10 ms. In a `v4l2src` + `alsasrc` pipeline, `alsasrc`'s audio clock normally becomes the pipeline clock, so driver timestamps are then not used. `v4l2src` sets PTS from the pipeline clock minus the measured buffer age, and after a bad timestamp assumes a one-frame delay for the rest of the session. | [I-35], [I-36], [I-37] |
| FFmpeg 7.1 | The ALSA input stamps packets with wall-clock time; the V4L2 input passes monotonic timestamps through by default. Without `-ts abs` or `mono2abs` the clock bases are mixed, and the CLI's default per-input start shift discards the real offset between the inputs. | [I-38] |

Reasoning from these inputs: each framework aligns audio and video differently by default, so the A/V clock model is an architecture decision, to be recorded with ADR-007 (OQ-112, RISK-024). A direct application could put both streams on `CLOCK_MONOTONIC` timestamps [I-33], [I-34], although the audio samples are still clocked by the source [I-26]. The A/V tolerance is not yet set: OWNER DECISION REQUIRED (OQ-112). A sample-rate mismatch adds drift (RISK-023). Fractional frame rates also affect timestamps (OQ-040).

**Channels and formats.** Stereo only with the stock driver [I-24], [I-13]. Behaviour with compressed (IEC 61937) or multichannel HDMI audio, and the placement of 24-bit samples in the 32-bit slots, are undocumented: DATASHEET REQUIRED; HARDWARE TEST REQUIRED (OQ-110).

**Pins.** The overlay's pin group claims GPIO 18–21, including GPIO 21, which the audio path does not use [I-30]; the `pwm`, `pwm-2chan` and `gpio-ir` overlays default to GPIO 18, and `audremap` offers `pins_18_19` on BCM2711 [I-31] (OQ-114). VDDIO2 and board GPIO voltage: [I-27], [I-29] (OQ-024, OQ-025).

**Encoders.** Section 3.9 and [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) section 5.8: FFmpeg native `aac` (CORRECTED) and `libopus`; GStreamer `voaacenc`, `avenc_aac` and `opusenc`; `fdk-aac` is non-free and GPL-incompatible [I-39], [I-40], [I-41], [I-43], [I-44], [I-45], [I-46] (OQ-063, OQ-113). CPU cost alongside software video encoding is unmeasured (research gap, topic I; OQ-063).

**Key constraints.** RISK-014 (separate I2S wiring, stereo only, CM5 unconfirmed), RISK-023 (silent sample-rate mismatch), RISK-024 (A/V clock domains).

**Decisions.** ADR-004 (OPEN; CM5 audio is a bring-up gate in its analysis), ADR-007 (OPEN; A/V clock model and rate policy).

**Implementation status.** NOT STARTED. TEST-AUD-001: BLOCKED — HARDWARE REQUIRED (on CM4 and on CM5).

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
   two H.264 encodes from the same capture buffer (owner 2026-10-08, OQ-005):
   recording encode + live encode; two at once unproven (OQ-115)
   official spec 1080p30 encode [D-10]; 1080p60 unproven (RISK-002)
          |  two H.264 elementary streams
          v
 Recorder (recording encode) / RTMP + WebRTC (live encode): userspace, framework OPEN (ADR-007)
   recorder (ADR-009): fragmented MP4 --> NVMe SSD in the CM4 IO Board PCIe Gen 2 x1 socket [J-03]
                                      --> HDD at USB 2.0 through the IO Board hub [J-06], self-powered;
                                          HDD writer decoupled (OQ-117)
   live: < 1 s camera-to-viewer for WebRTC viewers only (OQ-116); RTMP best-effort
```

| | Pi 4 Model B | CM4 on CAM1 | CM4 on CAM0 |
|---|---|---|---|
| Connector and lanes | 15-pin, 2 lanes [C-01] | 22-pin on the CM4 IO Board, 4 lanes [C-03] | 22-pin, 2 lanes; on the CM4 IO Board both J6 jumpers must be fitted for I2C [C-03] |
| Receiver instance in DT | `csi1`, limited to 2 lanes [B-48], [C-08] | `csi1`, 4 lanes [C-08] | `csi0`, 2 lanes [C-08] |
| Overlay parameters (`config.txt` syntax for bare boolean and combined parameters: NEEDS VERIFICATION, OQ-100) | `media-controller` (ADR-006, PROPOSED) [G-12] | `media-controller` and `4lane` [G-12], [B-42] | `media-controller` and `cam0` [C-10] |
| Configuration under REQ-CAP-007 | 2-lane candidate | 4-lane candidate | 2-lane candidate |
| 1080p60 capture (bandwidth reasoning) | Not feasible [C-48]; the 2-lane configuration carries up to 1080p50 UYVY / 1080p30 RGB888 [C-37] | Feasible by bandwidth [C-49]; officially documented for a Compute Module with 4 lanes [C-37]. For UYVY at 972 Mbit/s the driver uses 3 of the 4 lanes, which is unproven (OQ-038; ADR-008). | Not feasible [C-48]; as Pi 4 Model B |
| Hardware encode | H.264, spec 1080p30 [D-10]. *(2026-10-08: two concurrent encodes, recording and live: OQ-115.)* | Same [D-10] | Same [D-10] |
| H.265 encode *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | Software only (x265 on Cortex-A72, no DotProd), after UYVY-to-planar conversion; not on `/dev/video11` [D-24], [H-05], [H-10] (CORRECTED), [H-13] | Same | Same |
| HDMI audio *(added 2026-10-08)* | Not separately researched in topic I: NEEDS VERIFICATION | I2S into `bcm2835-i2s`, GPIO 18–21, 2 channels, ALSA card `tc358743` [I-10], [I-13], [I-15] | Same as CM4 on CAM1: the I2S path does not depend on the camera connector (reasoning from [I-10]) |
| Recording drives *(added 2026-10-09; ADR-009)* | Not researched in topic J: NEEDS VERIFICATION (OQ-098) | On the CM4 IO Board: NVMe SSD in the single PCIe Gen 2 x1 socket, slot powered from the 12 V input; HDD at USB 2.0 through the shared hub [J-03], [J-05], [J-06], [J-09] (CORRECTED; reasoning). Section 3.9.3; RISK-026 | Same as CM4 on CAM1: the drives do not depend on the camera connector (reasoning) |

On Pi 4/CM4 the diagram above shows the H.264 path, the only video path in the current scope (owner, 2026-10-08; OQ-103). *(Diagram updated 2026-10-08 after the owner answered OQ-005 with "Separate record + live". The encoder box read "H.264; official spec 1080p30 encode [D-10]; 1080p60 unproven (RISK-002)", with one "H.264 elementary stream" to "Recorder / RTMP / WebRTC". It now shows the recording and live encodes on `/dev/video11`; whether the encoder runs both at once is OQ-115.)* *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* The H.265 path would leave the diagram at `/dev/videoN`: CPU (or, if OQ-057 allows, ISP) conversion to planar, then software x265 through FFmpeg `libx265` or GStreamer `x265enc` (section 3.8.1). The audio side path joins at the recorder and stream muxers (section 3.11). *(Diagram extended 2026-10-09; ADR-009 / OQ-116. The four lines starting "recorder (ADR-009)" and "live:" were added under the last box, which is unchanged; they show the CM4 IO Board storage attachment (section 3.9.3) and the latency scope (section 3.9.4). Pi 4 Model B storage was not researched in topic J.)*

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
 Two software H.264 encoders (x264 family; ADR-007 OPEN):
   recording encode + live encode (owner 2026-10-08, OQ-005)
   official: 1080p30 encode ~30-40% CPU [G-22]; no hardware encoder [D-31]
   two encodes roughly double the encode CPU load (reasoning; OQ-059)
          |  two H.264 elementary streams
          v
 Recorder (recording encode) / RTMP + WebRTC (live encode): userspace, framework OPEN (ADR-007)
   recorder (ADR-009): fragmented MP4 --> NVMe SSD in the CM5 IO Board M.2 slot, PCIe Gen 2 x1 [J-13]
                                          (link disabled by default; dtparam=pciex1, OQ-124) [J-15], [J-17]
                                      --> HDD on USB 3.0 [J-18], [J-19], self-powered;
                                          HDD writer decoupled (OQ-117)
   live: < 1 s camera-to-viewer for WebRTC viewers only (OQ-116); RTMP best-effort
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
| H.265 encode *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | Software only (x265 on Cortex-A76) [D-31]. DotProd kernels can apply on the BCM2712 core [H-04], [H-05]; for Pi 5 this is reasoning (same SoC as CM5) | Same | Software only; DotProd kernels can apply [D-31], [H-04], [H-05] | Same as CM5 on the CM5 IO Board |
| HDMI audio *(added 2026-10-08)* | Not separately researched in topic I: NEEDS VERIFICATION | Not separately researched in topic I: NEEDS VERIFICATION | Overlay labels resolve to RP1 I2S1 on GPIO 18–21 [I-07], [I-08], [I-09]; capture unconfirmed (OQ-054) | Reasoning: the labels come from the CM5 Device Tree [I-07], so the same RP1 I2S1 path is expected; NEEDS VERIFICATION on this carrier (OQ-052). The CM4 IO Board's GPIO voltage is selectable and VDDIO2 should match it [I-29] |
| Recording drives *(added 2026-10-09; ADR-009)* | Not researched in topic J beyond NVMe boot and Gen 3 [J-10], [J-16]: NEEDS VERIFICATION (OQ-098) | Same as CAM/DISP0 | NVMe SSD in the M.2 M-key slot at PCIe Gen 2 x1, 2230–2280; the link is disabled by default in the `rpi-6.18.y` device tree (OQ-124); HDD on USB 3.0, with about 1.2 A of VBUS shared by the two ports [J-13], [J-14], [J-15], [J-17], [J-18], [J-19]. Section 3.9.3 | Not researched in topic J: NEEDS VERIFICATION |

The diagram above shows the software H.264 path, the only video path in the current scope (owner, 2026-10-08; OQ-103). *(Diagram updated 2026-10-08 after the owner answered OQ-005 with "Separate record + live". The encoder box read "Software H.264 encoder (x264 family; ADR-007 OPEN)", with one "H.264 elementary stream" to "Recorder / RTMP / WebRTC". It now shows two software encoders, recording and live. Reasoning: one UYVY conversion per frame can feed both (section 3.7); the CPU budget is OQ-059, RISK-003.)* *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* H.265 would follow the same shape — UYVY-to-planar conversion, then x265 through FFmpeg `libx265` or GStreamer `x265enc` (section 3.8.1) — and would compete with H.264 for the same CPU (reasoning from [D-31], [G-22]; the BCM2712 core count is not in the source register). The audio side path joins at the muxers (section 3.11). *(Diagram extended 2026-10-09; ADR-009 / OQ-116. The five lines starting "recorder (ADR-009)" and "live:" were added under the last box, which is unchanged; they show the CM5 IO Board storage attachment (section 3.9.3) and the latency scope (section 3.9.4). Pi 5 storage was not researched in topic J.)*

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
| Two H.264 encodes, recording + live (owner, 2026-10-08; OQ-005; row added 2026-10-08) | Both on the hardware encoder; two at once unproven (OQ-115; RISK-002) | Both on the hardware encoder; two at once unproven (OQ-115; RISK-002) | Both in software; roughly double the encode CPU load (reasoning from [G-22]; OQ-059; RISK-003) | Both in software; roughly double the encode CPU load (reasoning from [G-22]; OQ-059; RISK-003) |
| Official TC358743 documentation | Yes, 2-lane [C-37] | Yes, 4-lane Compute Module case [C-37] | None [C-38] | None [C-38] |
| Kernel in Raspberry Pi OS 2026-10-06 | 6.18.50, `rpi-v8` (4K pages) [G-04], [G-71] (CORRECTED; reasoning) | 6.18.50, `rpi-v8` [G-04], [G-71] (CORRECTED; reasoning) | 6.18.50, `rpi-2712` (16K pages) [G-04], [G-20] | 6.18.50, `rpi-2712` [G-04], [G-20] |
| Bring-up evaluation (owner, 2026-10-07, second answer; added 2026-10-08) | Documented candidate; not a bring-up board | **Bring-up board** | Documented candidate; not a bring-up board | **Bring-up board** |
| H.265 encode (added 2026-10-08; deferred — REQ-ENC-002; not in current scope) | Software (Cortex-A72; no DotProd) [D-24], [H-05] | Software (Cortex-A72; no DotProd) [D-24], [H-05] | Software (Cortex-A76; DotProd can apply — reasoning, same BCM2712 core as CM5) [D-31], [H-05] | Software (Cortex-A76; DotProd can apply) [D-31], [H-05] |
| HDMI audio CPU I2S (added 2026-10-08) | NEEDS VERIFICATION (not researched in topic I) | `bcm2835-i2s`, 2 channels [I-10], [I-13] | NEEDS VERIFICATION (not researched in topic I) | RP1 I2S1; unconfirmed (OQ-054) [I-07], [I-09] |
| Recording drives, NVMe SSD + USB-to-SATA HDD (ADR-009; added 2026-10-09) | NEEDS VERIFICATION (not researched in topic J; OQ-098) | CM4 IO Board: NVMe in the only PCIe Gen 2 x1 socket (12 V slot power); HDD at USB 2.0 on the shared hub [J-03], [J-05], [J-06], [J-09] (CORRECTED; reasoning). RISK-026; OQ-121, OQ-123 | NEEDS VERIFICATION (topic J covered only NVMe boot and Gen 3 [J-10], [J-16]; OQ-098) | CM5 IO Board: M.2 at PCIe Gen 2 x1 (link disabled by default, OQ-124); HDD on USB 3.0 [J-13], [J-15], [J-17], [J-18], [J-19]. OQ-121 |
| Live-encode latency evidence (OQ-116; added 2026-10-09) | As CM4 (reasoning: same BCM2711 encoder driver) | No B-frames [K-30]; force-keyframe [K-31]; reports 0 latency to GStreamer [K-32]; about 10 ms at 720p on a Pi 4, stated by a Raspberry Pi engineer (community source) [K-33]; to be shared with the recording encode (OQ-115) | As CM5 (reasoning: same BCM2712) | x264 `zerolatency` holds no frames back [K-27], [K-28]; per-frame time unknown (OQ-059); Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39] |

One Raspberry Pi OS image can boot all four candidates. Overlay, overlay parameters, control model, encode path and lane count still differ per board [G-71] (CORRECTED, reasoning).

**Product OS image (REQ-BLD-002, DRAFT; owner, 2026-10-07).** The product runs its own project-built OS image, not an unmodified stock image. ADR-003 (ACCEPTED 2026-10-07; OQ-012 ANSWERED) decides Raspberry Pi OS Lite for bring-up and `rpi-image-gen` for the product image, with Buildroot as the alternative. [DECISIONS.md](DECISIONS.md) chose `rpi-image-gen` because REQ-CAP-007 may put the two lane configurations on different board families (ADR-004, OPEN), and one Raspberry Pi OS Lite image ships both kernels with module trees that include `tc358743`, Unicam and RP1 CFE and boots all four candidate boards [G-71] (CORRECTED, reasoning). Whether that image contains the compiled `tc358743` overlays is NEEDS VERIFICATION. See [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

---

## 5. Cross-cutting constraints

| Constraint | Layers | Evidence | Risk | Retired by |
|---|---|---|---|---|
| 1080p60 needs more than 2 CSI-2 lanes; a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s, OQ-038). REQ-CAP-007 requires both a 2-lane configuration (up to 1080p50 UYVY / 1080p30 RGB888) and a 4-lane configuration (1080p60 required) | 1, 3, 4 | [C-37]; reasoning [C-47], [C-48], [C-49] | RISK-001, RISK-006 | ADR-004 (per configuration) and ADR-008 (OQ-099), then TEST-CAP-002 and TEST-CAP-004 |
| Source behaviour per ATEM or camera model (output modes, interlace, HDCP, colour format) is not in the source register; the ATEM Mini Pro outputs progressive 1080p only [F-23] | 1, 2 | [F-23]; [B-27] (reasoning); [A-04] | RISK-008, RISK-009 | OQ-102, then TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001. *(2026-10-08: OQ-102 ANSWERED 2026-10-07 — any HDMI camera, no model list — so only the tests remain, with representative sources.)* |
| 1080p60 hardware encode on Pi 4/CM4 is unproven | 8 | [D-10]; reasoning [D-52]; [D-50] (reported; community source) | RISK-002 | TEST-ENC-001 |
| No hardware encoder on Pi 5/CM5; UYVY must be converted per frame. *(2026-10-08: two software encodes at once, recording and live (owner answer to OQ-005), roughly doubling the encode CPU load; reasoning from [G-22]; OQ-059.)* | 7, 8 | [G-22], [D-31], [D-43] | RISK-003 | TEST-ENC-001, TEST-PERF-001. *(2026-10-08: including the two-encode run on CM5 and the combined load.)* |
| *(Added 2026-10-08.)* Two simultaneous H.264 encodes, recording and live, from one capture (owner answer to OQ-005). Whether the Pi 4/CM4 hardware encoder runs both is unknown; two 1080p30 encodes need about 2.0x its 1080p30 specification (reasoning). Each capture buffer has two consumers. | 7, 8 | [D-10]; reasoning [D-52] | RISK-002 | OQ-115, then TEST-ENC-001 (two-encode run on CM4) and TEST-PERF-001 |
| *(Added 2026-10-08.)* The live encode is shared by RTMP and WebRTC, so it must be WebRTC-receivable: Constrained Baseline, a level that covers 1080p, no B-frames, SPS/PPS in-band. *(2026-10-09: MediaMTX documents the B-frame point [K-04]; YouTube recommends 2 B-frames for RTMP, which conflicts [K-17]; keyframe interval OQ-127.)* | 8, 9 | [F-36], [F-39], [F-40] (reasoning), [F-45] (reported; community source), [D-11], [D-14], [D-15]; added 2026-10-09: [K-04], [K-17] | RISK-019 | OQ-073, then TEST-STR-002 and TEST-STR-001; added 2026-10-09: OQ-127 |
| No video until an EDID is written after every boot or driver load (no EDID stored after probe; the "no default EDID" part is a *research gap*, topic B) | 2 | [A-33], [B-21] (CORRECTED), [A-31] | RISK-010 | REQ-CAP-003 (trigger and ordering: OQ-093), TEST-DRV-002 |
| Signal changes seen only by 1 s polling unless INT is wired | 2, 6 | [A-30], [B-19], [C-20] | RISK-013 | OQ-020, TEST-CAP-003 |
| CMA sizing | 4, 7 | [C-53] (CORRECTED; reasoning), [E-47] (CORRECTED), [C-40] | RISK-020 | TEST-PERF-001 |
| Pi 5/CM5 path relies on community procedures | 4, 5 | [C-38], [C-33] (reported; community source), [C-42] (reported; community source) | RISK-012 | TEST-PLT-001, TEST-CAP-001/002/003 |
| Thermal envelope: TC358743 rated −30 to +70 °C ambient, typical 543.2 mW at 1080p60; Pi 5 encode is CPU-bound | 1, 2, 8 | [A-41], [A-38], [G-22] | — (REQ-PERF-001, OQ-010) | TEST-PERF-001 |
| *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* H.265 is software-only on every candidate (it was required from 2026-10-07 until the owner deferred it on 2026-10-08); real-time 1080p cost is unmeasured, and the only cost evidence is a community statement and community benchmarks | 7, 8 | [D-24], [D-31], [H-05]; [H-19], [H-20], [H-21], [H-22] (community); [H-23], [H-43] (reasoning) | RISK-022 (OPEN; not in current scope) | OQ-103 scoping (ANSWERED 2026-10-08: H.264 only); if REQ-ENC-002 is re-activated, TEST-ENC-001 and TEST-PERF-001 with H.265 on CM4 and CM5 (OQ-104, OQ-105) |
| *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* HEVC over RTMP is possible with FFmpeg 7.1.5 but not with the distribution's GStreamer 1.26.2 `flvmux`, which may force a split GStreamer/FFmpeg design | 9 | [H-26], [H-27], [F-34] (CORRECTED), [H-30] | RISK-025 (OPEN; not in current scope) | If REQ-ENC-002 is re-activated: ADR-007 records the path (OQ-107), then TEST-STR-001 (OQ-106) |
| *(Added 2026-10-08; deferred — REQ-ENC-002; not in current scope.)* H.265 over WebRTC reaches only some browsers, so an H.264 track remains | 9 | [H-32], [H-33], [H-34], [H-35]; [H-36] (community) | RISK-019 | If REQ-ENC-002 is re-activated: TEST-STR-002 on the target viewer devices (OQ-108) |
| *(Added 2026-10-08.)* HDMI audio sample-rate changes never reach ALSA; a mismatch is silent | Audio side path | [I-18] (reasoning); [I-19], [I-20], [I-21], [I-22] | RISK-023 | Rate policy (OQ-111), then TEST-AUD-001 at 44.1 and 48 kHz on CM4 and CM5 |
| *(Added 2026-10-08.)* Audio and video are clocked independently and each framework aligns them differently by default | Audio side path, 9 | [I-26], [I-33], [I-34], [I-35], [I-36], [I-37], [I-38] | RISK-024 | A/V clock model with ADR-007 and tolerance (OQ-112), then TEST-AUD-001 and TEST-PERF-001 |
| *(Added 2026-10-08.)* HDMI audio on CM5 is unconfirmed; CM5 is a bring-up board (ADR-004) | Audio side path | [I-05], [I-06], [I-07], [I-09], [I-14] | RISK-014 | TEST-AUD-001 on CM5 (OQ-054) |
| *(Added 2026-10-09; ADR-009.)* CM4 storage path: the NVMe SSD takes the only PCIe lane, the HDD shares USB 2.0, and the CM4 IO Board's PCIe slot needs its 12 V input | 1, 9 | [J-01], [J-05], [J-06], [J-08]; [J-09] (CORRECTED; reasoning) | RISK-026 | OQ-121, OQ-123, then TEST-REC-001 and TEST-PERF-001 on CM4, powered as the product will be |
| *(Added 2026-10-09; ADR-009.)* USB-to-SATA bridge UAS faults (hangs, resets, rare data loss) | 1, 9 | [J-24], [J-25], [J-28]; [J-27] (CORRECTED; community source) | RISK-027 | OQ-122, then TEST-REC-001 and TEST-PERF-001 on CM4 and CM5 |
| *(Added 2026-10-09; ADR-009.)* An HDD stall can back-pressure the mirrored recording into the shared encoder, the NVMe copy and the live path | 7, 8, 9 | [J-30] (CORRECTED); [K-36] (reasoning part) | RISK-028 | OQ-117, then TEST-REC-001 and TEST-PERF-001 with injected HDD stalls, and TEST-STR-002 |
| *(Added 2026-10-09; ADR-009.)* Fragmented MP4 is less compatible with players and editors | 9 | [J-39], [J-45] | RISK-029 | OQ-118, then TEST-REC-001 |
| *(Added 2026-10-09; ADR-009.)* The last seconds of a recording are still lost on a power cut | 9 | [J-35] (CORRECTED), [J-45] | RISK-030 | OQ-119, OQ-120, then TEST-REC-001 power-cut runs |
| *(Added 2026-10-09; OQ-116.)* The < 1 s WebRTC target is unproven; most of the latency budget is undocumented | 4–9 | [K-34], [K-35], [K-03], [K-38], [K-39]; [K-33] (community source); [K-45] (CORRECTED; reasoning) | RISK-031 | OQ-125, then TEST-STR-002 on CM4 and CM5 with the recording encode running |
| *(Added 2026-10-09; OQ-116.)* Hidden default latencies and queue backlog in the live path | 6–9 | [K-07], [K-08], [K-09], [K-29] (CORRECTED), [K-32], [K-37]; [K-10], [K-36] (reasoning) | RISK-032 | OQ-126, then TEST-STR-002 |
| *(Added 2026-10-09; OQ-116.)* NAT traversal and TURN for internet WebRTC viewers, if required | 9 | [K-41], [K-42], [K-43] | RISK-033 | OQ-008, OQ-128, then TEST-STR-002 |
| *(Added 2026-10-09; OQ-116.)* WebRTC publishing stack: gst-plugins-rs not packaged; WHEP still a draft | 9 | [K-02], [K-05], [K-06] | RISK-034 | ADR-007 (OQ-074), then TEST-BLD-001 and TEST-STR-002 |

---

## 6. Traceability by layer (Rule 12)

Implementation status is `NOT STARTED` for every row. Every test is `BLOCKED — HARDWARE REQUIRED` except TEST-BLD-001, which is `NOT STARTED`. Design-to-implementation links will be added as implementation starts ([TRACEABILITY.md](TRACEABILITY.md)).

| Layer | Requirements | Decisions | Risks | Tests |
|---|---|---|---|---|
| 1 Hardware | REQ-PLT-001, REQ-DRV-001, REQ-CAP-007, REQ-CAP-008; REQ-REC-001 (recording drives; added 2026-10-09) | ADR-004; ADR-009 (added 2026-10-09) | RISK-001, RISK-004, RISK-007, RISK-021; RISK-026, RISK-027 (added 2026-10-09) | TEST-HW-001; TEST-REC-001 (recording drives; added 2026-10-09) |
| 2 TC358743 | REQ-DRV-001, REQ-CAP-003, REQ-CAP-004, REQ-CAP-005, REQ-CAP-008 | ADR-001, ADR-002, ADR-005 | RISK-005, RISK-007, RISK-008, RISK-009, RISK-010, RISK-013 | TEST-DRV-001, TEST-DRV-002, TEST-CAP-001 |
| 3 CSI-2 | REQ-CAP-001, REQ-CAP-005, REQ-CAP-007 | ADR-004, ADR-005, ADR-008 | RISK-001, RISK-006, RISK-011 | TEST-CAP-002, TEST-CAP-004 |
| 4 CSI receiver | REQ-ARCH-001, REQ-PLT-001, REQ-CAP-002 | ADR-004, ADR-006 | RISK-011, RISK-012, RISK-016 | TEST-PLT-001 |
| 5 Media Controller | REQ-ARCH-001, REQ-CAP-002 | ADR-001, ADR-006 | RISK-012 | TEST-PLT-001 |
| 6 V4L2 | REQ-CAP-002, REQ-CAP-004 | ADR-001, ADR-006 | RISK-013, RISK-016 | TEST-CAP-001, TEST-CAP-003 |
| 7 DMABUF | REQ-DMA-001, REQ-ARCH-001 | ADR-005, ADR-007 | RISK-020 | TEST-DMA-001 |
| 8 Encoder | REQ-ENC-001 (H.264 only since 2026-10-08; two encodes, recording and shared live, since the owner's answer to OQ-005 the same day); REQ-ENC-002 (H.265, DEFERRED; no planned test) | ADR-004, ADR-007 | RISK-002, RISK-003, RISK-015; RISK-022 (added 2026-10-08; not in current scope); RISK-028, RISK-031, RISK-032 (added 2026-10-09) | TEST-ENC-001 |
| 9 Recorder / RTMP / WebRTC | REQ-REC-001, REQ-STR-001, REQ-STR-002 | ADR-007; ADR-009 (ACCEPTED 2026-10-08; added 2026-10-09) | RISK-019; RISK-022, RISK-025 (added 2026-10-08; not in current scope); RISK-026 to RISK-034 (added 2026-10-09) | TEST-REC-001, TEST-STR-001, TEST-STR-002; TEST-PERF-001 (HDD-stall soak, RISK-028; added 2026-10-09) |
| Audio side path (section 3.11) | REQ-CAP-006 | — ; ADR-004 and ADR-007 (added 2026-10-08) | RISK-014; RISK-023, RISK-024 (added 2026-10-08) | TEST-AUD-001 |
| ATEM integration (HDMI capture only, OQ-009 ANSWERED) | REQ-ATEM-001, REQ-CAP-008 | — | RISK-008; RISK-018 only if network control is added | TEST-ATEM-001 |
| Whole system | REQ-PERF-001, REQ-BLD-001, REQ-BLD-002 | ADR-003 | RISK-017, RISK-020 | TEST-PERF-001, TEST-BLD-001 |

---

## 7. Open architectural decisions

No decision below may be treated as made until [DECISIONS.md](DECISIONS.md) records it as `ACCEPTED`.

| ADR | Title | Status (2026-10-07; ADR-003 ACCEPTED 2026-10-07, the others unchanged since 2026-10-06; added 2026-10-09: ADR-009 ACCEPTED 2026-10-08) | Layers affected | What it blocks | OQ |
|---|---|---|---|---|---|
| ADR-003 | OS and image basis: Raspberry Pi OS Lite (64-bit) for bring-up, `rpi-image-gen` for production, Buildroot as the documented alternative | ACCEPTED (2026-10-07) | All; kernel version and media-stack versions differ: Raspberry Pi OS ships kernel 6.18.50 [G-04], Buildroot 2026.08 pins 6.12.61 [G-59] | Nothing since acceptance; it governs REQ-BLD-001; REQ-BLD-002 (own OS image; owner, 2026-10-07; accepted with the proposal unchanged); build, update and release design | OQ-012 (ANSWERED) |
| ADR-004 | Product target platform (since 2026-10-07 a choice per lane configuration, REQ-CAP-007). *(Owner, 2026-10-07, second answer: bring-up evaluates CM4 and CM5 side by side; decided from TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001.)* | OPEN | 1, 3, 4, 5, 7, 8; audio side path (added 2026-10-08) | Platform for the 2-lane and for the 4-lane configuration; REQ-CAP-001 feasibility on the 4-lane configuration; encode path; every platform-specific procedure. *(Added 2026-10-08: H.265 throughput on CM4 versus CM5, OQ-104 — not in current scope since H.265 was deferred the same day, REQ-ENC-002; HDMI audio on CM5, OQ-054.)* *(Added 2026-10-09: storage attachment for ADR-009 — Claude's reading in DECISIONS.md is that storage excludes neither module, with CM4 constrained by RISK-026 and CM5 needing the M.2 enablement checked, OQ-124; the < 1 s WebRTC target, which neither module is shown to meet, RISK-031, OQ-125.)* | OQ-011 |
| ADR-005 | Default capture pixel format: UYVY | PROPOSED | 2, 3, 6, 7, 8 | Lane budget, encoder input path, colour metadata | OQ-003 |
| ADR-006 | Media Controller mode on every platform | PROPOSED | 4, 5, 6 | Capture control design, test procedures | OQ-014 |
| ADR-007 | Userspace media framework | OPEN | 7, 8, 9; audio side path (added 2026-10-08) | All userspace pipeline design ([SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md)). *(Added 2026-10-08: the HEVC-over-RTMP path, OQ-107, RISK-025 — not in current scope since H.265 was deferred the same day (ADR-007 scope note); the A/V clock model, OQ-112, RISK-024; the audio sample-rate policy, OQ-111, RISK-023.)* *(Added 2026-10-09: the live publishing route, RISK-034; element latencies, OQ-126, RISK-032; fragmented-MP4 settings, OQ-118; HDD-branch decoupling, OQ-117, RISK-028.)* | OQ-015 |
| ADR-008 | CSI-2 link frequency: proposes keeping the overlay default 486 MHz (972 Mbit/s per lane) on all platforms; 297 MHz to be evaluated only on a CM4 CAM1 4-lane link for 1080p60 UYVY in TEST-CAP-002 | PROPOSED | 2, 3, 4 | Device Tree `link-frequency` setting; 1080p60 UYVY lane use (3 of 4 lanes at 972 Mbit/s, OQ-038); Pi 5/CM5 receiver match [C-52] | OQ-099 |
| ADR-009 *(row added 2026-10-09)* | Recording storage and power-loss safety: fragmented MP4 mirrored to a PCIe NVMe SSD and a self-powered USB-to-SATA HDD | ACCEPTED (2026-10-08) | 1, 9 | Nothing since acceptance; it governs REQ-REC-001. Its fourth point (ext4 on the recording volumes) is Claude's proposal inside the ADR, not an owner decision (OQ-120). Implementation and verification: OQ-117 to OQ-124 | OQ-006 (still OPEN for the maximum recording duration) |

*(2026-10-08: no ADR status changed. The owner input of 2026-10-07 (second answer) was added to ADR-004, and evidence from research topics H and I was added to ADR-004 and ADR-007 in [DECISIONS.md](DECISIONS.md); both stay OPEN. The ADR-007 scope note added after the owner deferred H.265 on 2026-10-08 changes no status either.)* *(2026-10-08, later: the owner's answer to OQ-005 ("Separate record + live", two H.264 encodes) changes no ADR status. It bears on ADR-004 through the encode path, with two concurrent encodes (OQ-115 on CM4, OQ-059 on CM5). It bears on ADR-007 through how one capture feeds two encoders and how the live bitstream is split to RTMP and WebRTC.)* *(2026-10-09: ADR-009 (ACCEPTED 2026-10-08, owner decisions) is added to the table. No other ADR status changed: [DECISIONS.md](DECISIONS.md) records 3 ACCEPTED, 4 PROPOSED and 2 OPEN. Research topics J and K added evidence to ADR-004, ADR-007 and ADR-009 there; ADR-004 and ADR-007 stay OPEN.)*

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
| Raspberry Pi Ethernet, USB and storage facts per candidate board *(2026-10-09: research topic J covered USB and storage for CM4, CM5 and their IO Boards (section 3.9.3); Ethernet on all four boards and USB and storage on Pi 4 Model B and Pi 5 are still open)* | DATASHEET REQUIRED | OQ-098 |
| `config.txt` overlay-parameter syntax (bare boolean, combined parameters, `[pi4]` matching CM4, unknown parameters) | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-100 |
| Userspace design: process model, operator interface, configuration and logging, EDID trigger, updates during recording | OWNER DECISION REQUIRED | OQ-090 to OQ-094 |
| Whether Pi 4/CM4 hardware encode sustains 1080p60 | HARDWARE TEST REQUIRED | OQ-056 |
| Zero-copy DMABUF from Unicam into the encoder | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OQ-058 |
| *(Added 2026-10-08.)* Whether the Pi 4/CM4 hardware encoder runs two encodes at once (recording + live; owner answer to OQ-005), and at what combined resolution and frame rate | HARDWARE TEST REQUIRED; KERNEL SOURCE INSPECTION REQUIRED | OQ-115 |
| Pi 5/CM5 software encode cost and UYVY conversion offload *(2026-10-08: two concurrent software encodes, recording and live)* | HARDWARE TEST REQUIRED | OQ-059, OQ-060 |
| CMA budget per platform | HARDWARE TEST REQUIRED | OQ-061 |
| Runtime device nodes, I2C bus numbers and media entity names | HARDWARE TEST REQUIRED | OQ-043 |
| Encoding parameters and number of simultaneous encodes *(codec answered 2026-10-07: H.264 and H.265; bitrate, latency and number of encodes still open)* *(2026-10-08: codec narrowed to H.264 only, OQ-103 ANSWERED; H.265 deferred, REQ-ENC-002)* *(2026-10-08, later: number of encodes answered — two, recording and shared live ("Separate record + live"); bitrate, rate control and latency still open)* *(2026-10-09: latency answered by the owner on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers only, RTMP best-effort, OQ-116 ANSWERED; bitrate and rate control still open)* | OWNER DECISION REQUIRED | OQ-005 |
| Whether audio is required *(ANSWERED 2026-10-07: required, REQ-CAP-006 DRAFT; no longer an unknown)* | OWNER DECISION REQUIRED *(made)* | OQ-004 (ANSWERED) |
| Which ATEM models and cameras must be supported as HDMI sources (REQ-CAP-008; integration scope answered 2026-10-07, OQ-009) *(ANSWERED 2026-10-07: any HDMI camera, no model list, plus ATEM outputs; per-source behaviour still needs tests)* | OWNER DECISION REQUIRED *(made)*; HARDWARE TEST REQUIRED | OQ-102 (ANSWERED) |
| *(Added 2026-10-08.)* Which outputs use H.265, and whether software-only H.265 is acceptable *(ANSWERED 2026-10-08: no output uses H.265 for now — "H.264 only for now"; H.265 deferred as REQ-ENC-002; no longer an unknown)* | OWNER DECISION REQUIRED *(made)* | OQ-103 (ANSWERED) |
| *(Added 2026-10-08; not in current scope — H.265 deferred, REQ-ENC-002.)* Software H.265 throughput, latency and CPU headroom on CM4 and CM5; x265 SIMD paths and version | HARDWARE TEST REQUIRED; BUILD TEST REQUIRED; VENDOR CONFIRMATION REQUIRED | OQ-104, OQ-105 |
| *(Added 2026-10-08; not in current scope — H.265 deferred, REQ-ENC-002.)* HEVC over Enhanced RTMP at the destinations, and the muxing path with GStreamer 1.26.2 | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED; BUILD TEST REQUIRED; OWNER DECISION REQUIRED | OQ-106, OQ-107 |
| *(Added 2026-10-08; not in current scope — H.265 deferred, REQ-ENC-002.)* Which viewer browsers and devices receive H.265 over WebRTC | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-108 |
| *(Added 2026-10-08.)* HEVC and AAC patent licensing *(HEVC part not in current scope — H.265 deferred, REQ-ENC-002)* | LEGAL CLARIFICATION REQUIRED | OQ-109, OQ-113 |
| *(Added 2026-10-08.)* `tc358743-audio` capture on CM5 (RP1 I2S1) | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OQ-054 |
| *(Added 2026-10-08.)* TC358743 I2S output for compressed, multichannel and 24-bit HDMI audio | DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-110 |
| *(Added 2026-10-08.)* Audio sample-rate detection, rate changes and output sample rate | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED; BUILD TEST REQUIRED | OQ-111 |
| *(Added 2026-10-08.)* A/V synchronisation across the I2S and CSI-2 clock domains, and the A/V tolerance | HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED | OQ-112 |
| *(Added 2026-10-08.)* GPIO 18–21 allocation when HDMI audio is enabled | OWNER DECISION REQUIRED; BUILD TEST REQUIRED | OQ-114 |
| *(Added 2026-10-09.)* Whether the < 1 s latency target applies to RTMP outputs *(ANSWERED 2026-10-08: WebRTC viewers only; RTMP best-effort; no longer an unknown)* | OWNER DECISION REQUIRED *(made)* | OQ-116 (ANSWERED) |
| *(Added 2026-10-09.)* HDD-branch buffering and overflow policy, so that HDD stalls cannot reach the NVMe copy, the shared encoder or the live path | BUILD TEST REQUIRED; HARDWARE TEST REQUIRED | OQ-117 |
| *(Added 2026-10-09.)* Fragment duration, muxer support on the shipped builds, player and editor compatibility, file splitting | BUILD TEST REQUIRED; OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-118 |
| *(Added 2026-10-09.)* Recording lost on a power cut with fragmented MP4 on ext4, per drive and board | HARDWARE TEST REQUIRED; OWNER DECISION REQUIRED | OQ-119 |
| *(Added 2026-10-09.)* Recording-volume filesystem (ext4 is Claude's proposal in ADR-009, not an owner decision) and mount options | OWNER DECISION REQUIRED; BUILD TEST REQUIRED; HARDWARE TEST REQUIRED | OQ-120 |
| *(Added 2026-10-09.)* NVMe SSD and PCIe/M.2 adaptor qualification on the CM4 and CM5 IO Boards | OWNER DECISION REQUIRED; VENDOR CONFIRMATION REQUIRED; DATASHEET REQUIRED; HARDWARE TEST REQUIRED | OQ-121 |
| *(Added 2026-10-09.)* USB-to-SATA bridge qualification: chipset, UAS or Bulk-Only, quirks, sustained throughput | VENDOR CONFIRMATION REQUIRED; HARDWARE TEST REQUIRED | OQ-122 |
| *(Added 2026-10-09.)* CM4 PCIe interrupt mode (MSI or MSI-X) with NVMe on kernel 6.18 | KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED | OQ-123 |
| *(Added 2026-10-09.)* CM5 IO Board M.2: PCIe enablement (`dtparam=pciex1`), NVMe boot, link speed | HARDWARE TEST REQUIRED; VENDOR CONFIRMATION REQUIRED | OQ-124 |
| *(Added 2026-10-09.)* Measured camera-to-viewer latency of the WebRTC path on CM4 and CM5, and its undocumented terms | HARDWARE TEST REQUIRED; DATASHEET REQUIRED; VENDOR CONFIRMATION REQUIRED | OQ-125 |
| *(Added 2026-10-09.)* Live-path element latencies and queue policy | BUILD TEST REQUIRED; HARDWARE TEST REQUIRED | OQ-126 |
| *(Added 2026-10-09.)* Live-encode keyframe interval, on-demand keyframes and B-frame settings shared by RTMP and WebRTC | VENDOR CONFIRMATION REQUIRED; BUILD TEST REQUIRED | OQ-127 |
| *(Added 2026-10-09.)* WebRTC for internet viewers: WHEP through MediaMTX, ICE, STUN and TURN | OWNER DECISION REQUIRED; HARDWARE TEST REQUIRED | OQ-128 |
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
| D — Encoders | D-05, D-06, D-07, D-08, D-09, D-10, D-11, D-12, D-13, D-14, D-16, D-17, D-18, D-19, D-20, D-21, D-22, D-24, D-25, D-28, D-29, D-31, D-32, D-34, D-40, D-42, D-43, D-44, D-45, D-47, D-48, D-50, D-52, D-53. Added 2026-10-08 (two encodes): D-15, D-35, D-37 |
| E — Buildroot and kernel configuration | E-40, E-43, E-44, E-47, E-48 |
| F — ATEM and streaming | F-01, F-11, F-23, F-24 (added 2026-10-08), F-25, F-31, F-33, F-34, F-36, F-38, F-39 (added 2026-10-08, two encodes), F-40, F-41, F-42, F-45, F-46 |
| G — Raspberry Pi OS and image tooling | G-04, G-12, G-13, G-14, G-16, G-19, G-20, G-21, G-22, G-26, G-32, G-59, G-71 |
| H — H.265/HEVC software encoding and transport (added 2026-10-08; since 2026-10-08 evidence for REQ-ENC-002, deferred) | H-01, H-02, H-03, H-04, H-05, H-06, H-07, H-08, H-09, H-10, H-11, H-12, H-13, H-15, H-16, H-17, H-18, H-19, H-20, H-21, H-22, H-23, H-24, H-25, H-26, H-27, H-28, H-29, H-30, H-31, H-32, H-33, H-34, H-35, H-36, H-37, H-38, H-39, H-40, H-41, H-42, H-43 |
| I — HDMI audio path (added 2026-10-08) | I-02, I-03, I-04, I-05, I-06, I-07, I-08, I-09, I-10, I-11, I-12, I-13, I-14, I-15, I-16, I-17, I-18, I-19, I-20, I-21, I-22, I-23, I-24, I-25, I-26, I-27, I-28, I-29, I-30, I-31, I-33, I-34, I-35, I-36, I-37, I-38, I-39, I-40, I-41, I-43, I-44, I-45, I-46, I-47 |
| J — Recording storage and power loss (research of 2026-10-08; cited here from 2026-10-09) | J-01, J-02, J-03, J-04, J-05, J-06, J-07, J-08, J-09, J-10, J-11, J-12, J-13, J-14, J-15, J-16, J-17, J-18, J-19, J-20, J-21, J-24, J-25, J-27, J-28, J-29, J-30, J-31, J-33, J-35, J-36, J-37, J-38, J-39, J-40, J-41, J-44, J-45 |
| K — Live latency (research of 2026-10-08; cited here from 2026-10-09) | K-02, K-03, K-04, K-05, K-06, K-07, K-08, K-09, K-10, K-11, K-12, K-14, K-15, K-17, K-18, K-26, K-27, K-28, K-29, K-30, K-31, K-32, K-33, K-34, K-35, K-36, K-37, K-38, K-39, K-40, K-41, K-42, K-43, K-44, K-45 |

How each tier is used:

- **`CORRECTED` entries**, used in their corrected wording only: A-22, A-25, B-21, B-25, B-44, C-28, C-36, C-39, C-53, E-40, E-47, F-34, G-71. Added 2026-10-08: F-24, H-10, H-12, H-18, I-40. Added 2026-10-09: J-09, J-27, J-30, J-33, J-35, K-12, K-18, K-26, K-29, K-45.
- **`community` entries**, worded as reports: C-28, C-33, C-41, C-42, C-43, C-45, D-17, D-18, D-50, F-11, F-45. Added 2026-10-08: H-19, H-20, H-21, H-22, H-36, I-16. Added 2026-10-09: J-27, K-33.
- **`reasoning` entries**, labelled as reasoning: B-27, B-47, C-46, C-47, C-48, C-49, C-50, C-52, C-53, D-29, D-52, F-38, F-40, F-46, G-71. Added 2026-10-08: H-23, H-43, I-17, I-18, I-28. Added 2026-10-09: J-09, J-31, J-36, J-37, K-10, K-18, K-26, K-45, and the reasoning sentence of the `kernel-source` entry K-36.
- Statements marked *research gap* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). They are not register facts and are recorded only to state what is unknown. Since 2026-10-08, *research gap*, *research open question* and *research design risk* items for topics H and I come from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json), on the same terms. Since 2026-10-09, such items for topics J and K come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json), on the same terms.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No layer has been implemented, built or tested; every layer is `NOT STARTED`. Every test listed above is `BLOCKED — HARDWARE REQUIRED`, except TEST-BLD-001, which is `NOT STARTED`. *(2026-10-09: unchanged. No drive has been chosen or attached, no recording has been written, and no latency has been measured.)*

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review against REFERENCES.md. Changes: aligned wording with [C-50], [D-32], [D-50], [F-41], [G-26], [B-21]; added PACSCORDER unknowns (REFCLK OQ-019, connector OQ-021, I2C address OQ-026) to the diagrams; marked Pi 5 connector-to-receiver mapping and the `4lane` + `cam0` combination as reasoning / NEEDS VERIFICATION; labelled the Pi 4/CM4 DMABUF import as a target (OQ-058); used Rule 10 status for the CEC row; distinguished TEST-BLD-001 (`NOT STARTED`) from hardware-blocked tests. Added citations A-44, C-13, C-14, D-47. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: ADR-006 "chooses" → "proposes"; ADR-008 (PROPOSED, OQ-099) added to the §1.1 layer table, §3.3 decisions, §6 traceability and the §7 ADR table; 4-lane port marked necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s, OQ-038) in §3.3, §4.1, §4.2, §4.3 and §5; Pi 5 CAM/DISP0 4-lane feasibility conditioned on OQ-049; EDID-volatility rows no longer cite [B-22] (now [A-31], [B-21] and the topic B research gap; OQ-093); HD-source colourimetry attributed to the research gap, not [B-34]; TC358743 audio notes now say the driver configures I2S [A-13] while the silicon can also use CSI-2 [A-05]; RISK-016 described as a pixel-format label difference; 6.18.39 identified as the kernel of the [C-33] report (shipped kernel 6.18.50 [G-04]); [D-50] "edge case" attributed to one engineer (6by9); OQ-098 linked for storage/Ethernet/USB; OQ-100 noted for overlay-parameter syntax; §9 unknowns extended with OQ-038, OQ-090 to OQ-095, OQ-098 to OQ-100. No requirement, decision, risk or test status changed. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: summary block added under "Current state"; sources are ATEM HDMI output or cameras connected directly (REQ-CAP-008; models OQ-102) in §1.1, §3.1, the §4.1/§4.2 diagrams and a new §5 row; ATEM row of §3.10 reduced to HDMI capture, with UDP 9910 and RTMP exchange marked not in current scope (OQ-009 ANSWERED); both 2-lane and 4-lane configurations (REQ-CAP-007) added to §1.1, §3.1, §3.3 (new requirement paragraph, configuration row, `4lane`/wiring note [C-12], [C-17], RISK-001 nuance that Pi 4 Model B and CM4 CAM0 remain 2-lane candidates), §4.1–§4.3 and §5; ADR-004 described as a choice per lane configuration (still OPEN); own OS image (REQ-BLD-002) added after §4.3 and to the ADR-003 row of §7; §6 traceability extended with REQ-CAP-007, REQ-CAP-008, REQ-BLD-002; §9 "Whether 1080p60 is mandatory (OQ-001)" and "ATEM integration scope (OQ-009)" replaced by the per-configuration platform, bridge-board (OQ-021), source-model (OQ-102) and build-tool acceptance (OQ-012) unknowns. No requirement, decision, risk or test status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); "Owner decisions of 2026-10-07" summary block (ADR-003 ACCEPTED, OQ-012 ANSWERED); §4.3 "Product OS image" paragraph (ADR-003 ACCEPTED, OQ-012 ANSWERED; "proposes" → "decides", "prefers" → "chose"); §7 status-column heading and ADR-003 row (ACCEPTED, no longer blocks, OQ-012 ANSWERED); §9 OQ-012 row marked ANSWERED. Not changed: evidence and citations, ADR-002, ADR-005, ADR-006, ADR-008 (PROPOSED) and ADR-004, ADR-007 (OPEN), implementation status (NOT STARTED), Change-history rows. TEST-ATEM-001 appears here by ID only, so no title changed. | Claude (session 2026-10-07) |
| 2026-10-07 | §4.3 ADR-003 rationale aligned with the corrected wording in DECISIONS.md (overlay inclusion NEEDS VERIFICATION; board families "may" differ). | Claude (session 2026-10-07) |
| 2026-10-08 | Second set of owner decisions of 2026-10-07 and research topics H (H.265/HEVC) and I (HDMI audio) propagated. Header (Last updated, Applies to: CM4 + CM5 bring-up, Verification: topics H and I); new summary blocks for the second owner set (CM4 and CM5 side by side, ADR-004 still OPEN; HDMI audio required, REQ-CAP-006 DRAFT, OQ-004 ANSWERED; H.264 + H.265, OQ-103; any HDMI camera, OQ-102 ANSWERED) and for the 2026-10-08 research; OQ-102 statements in the first summary block, §3.1, §3.10, §5 and §9 marked superseded/ANSWERED; §1 note that H.265 is software on all four and that audio is a side path; §1.1 rows 8 and 9 extended and an audio side-path row added; §2 audio control-plane item, H.265 in the data-plane diagram, new audio data-plane diagram; §2.1 audio control IDs, events, polling delay and location per platform [I-19]–[I-23], new audio-configuration row [I-24]; §2.2 H.265 parse step and three audio rows; §3.1 audio pins and voltage [I-27], [I-29], new GPIO 18–21 row (OQ-114); §3.2 audio facts [I-24]–[I-26]; §3.7 H.265 planar-input path [H-10], [H-13], [H-43]; §3.8 codec item marked superseded and new §3.8.1 H.265 encode paths; §3.9 table extended (HEVC recording, HEVC RTMP, H.265 WebRTC, Opus rates), new §3.9.1 HEVC-over-RTMP framework constraint [H-24]–[H-30] (RISK-025, OQ-107) and §3.9.2 fan-out reasoning; §3.10 audio row marked REQUIRED; new §3.11 HDMI audio side path (data path, CM4/CM5 table, rate-tracking responsibility [I-18]–[I-23], A/V synchronisation inputs [I-26], [I-33]–[I-38], channels, pins, encoders); §4.1–§4.3 H.265, audio and bring-up rows; §5 six new cross-cutting rows (RISK-022 to RISK-025, RISK-019, RISK-014); §6 risks and decisions extended; §7 ADR-004 and ADR-007 rows annotated (statuses unchanged); §9 OQ-004, OQ-005, OQ-102 rows annotated and OQ-054, OQ-103 to OQ-114 added; Verification status gains topics H and I and F-24. No requirement, decision, risk or test status changed; implementation status unchanged (NOT STARTED). | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic H and I additions: §3.11 audio diagram line "board voltage must match [I-29]" corrected to the register's wording ("GPIO voltage should match, else level-shift" — [I-29] gives level shifting as the alternative). All other [H-xx] and [I-xx] citations checked against the register; no change needed. No status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header Applies to (H.265 sections kept as evidence, not in current scope); second-set codec bullet marked superseded and new "Owner decision of 2026-10-08" summary block (H.264 on every output; REQ-ENC-002 DEFERRED; RISK-022, RISK-025, OQ-104 to OQ-109 OPEN but not in current scope; no ADR status changed; CM4 + CM5, audio, any HDMI camera, ADR-003 unchanged); research block notes the deferral; §1 encoder-split note reworded (H.264 the only codec in scope; H.265 conditional); §1.1 rows 8 and 9 labelled and row 8 governing decision now "H.264 on every output (OQ-103 ANSWERED)"; §2 data-plane diagram line now "H.264 elementary stream (REQ-ENC-001; H.265 deferred, REQ-ENC-002)"; §2.2 H.265 parse step, §3.7 H.265 path, §3.8.1, §3.9 HEVC recording / HEVC RTMP / H.265 WebRTC items, §3.9.1, the video part of §3.9.2, the §4.1–§4.3 H.265 rows and notes, the three H.265 rows of §5 and the §9 OQ-104 to OQ-109 rows labelled "deferred — REQ-ENC-002; not in current scope" (with scope notes in §3.8.1, §3.9.1, §3.9.2); §3.8 new superseded note (H.264 only); §3.8 decisions line (OQ-103 ANSWERED) and implementation status (TEST-ENC-001 H.264 runs only; H.265 runs deferred, not run in current scope); §3.9.2 decisions line (HEVC-over-RTMP path not in current scope per the ADR-007 scope note); §5 "retired by" for the H.265 rows made conditional on re-activation; §6 row 8 adds REQ-ENC-002 (DEFERRED) and RISK-022/RISK-025 marked not in current scope; §7 ADR-004 and ADR-007 annotations and status note (no ADR status changed); §9 OQ-005 row annotated, OQ-103 row marked ANSWERED; Verification status topic H row labelled. No other decision, status or citation changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): header "Applies to" sentence (two encodes from one capture; OQ-115, OQ-059); new "Owner decision of 2026-10-08, second" summary block (recording encode plus one live encode shared by RTMP and WebRTC; bitrate, rate control and latency still OQ-005; Pi 4/CM4 concurrency OQ-115, RISK-002, 2.0× reasoning [D-10], [D-52]; Pi 5/CM5 two software encodes OQ-059, RISK-003 [G-22]; live encode WebRTC-receivable [F-36], [F-39], [F-40], [F-45], [D-11], [D-14], [D-15]; audio still AAC + Opus; H.265 still deferred; no ADR status changed); §1 Encoder-layer note; §1.1 rows 7–9 annotated (two consumers per buffer; two concurrent encodes; recorder vs shared live; governing note); §2 data-plane diagram redrawn (DMABUF to recording and live encoders; old lines recorded) with topology reasoning; §2.2 "Capture → encoder" and "Encoder → outputs" rows annotated; §3.7 new "Two consumers per frame" paragraph (two encode sessions on `/dev/video11` UNKNOWN, OQ-115; zero-copy OQ-058; one conversion feeding both encoders on Pi 5/CM5 [D-40], [D-43], OQ-060; buffer hold time OQ-061, RISK-020; ADR-007); §3.8 superseded-in-part note and two table rows (two concurrent encodes, with 2.0× and 4.0× reasoning; live-encode settings [D-11], [D-14], [D-15], [D-37], [D-35]), risk row (OQ-115) and implementation-status note (TEST-ENC-001 two-encode run on CM4 and CM5; TEST-PERF-001 combined load); §3.9 output-table notes (recorder from recording encode; RTMP and WebRTC from the shared live encode; RTMP destination acceptance OQ-007), strictest-consumer paragraph superseded in part (applies to the live encode: Constrained Baseline, level [F-39], [F-40], OQ-073, RISK-019, no B-frames, SPS/PPS [D-15]; recording encode not bound; split ADR-007); §3.9.2 encode-count and audio notes; §4.1 and §4.2 diagrams redrawn (two encodes; old encoder boxes recorded in the notes below them) and §4.1 hardware-encode row; §4.3 new two-encode row; §5 Pi 5/CM5 row annotated and two rows added (two encodes, RISK-002, OQ-115; WebRTC-receivable live encode, RISK-019, OQ-073); §6 row 8 note; §7 ADR note (no status changed; bearing on ADR-004 and ADR-007); §9 OQ-005 row annotated, OQ-115 row added, OQ-059 row annotated; Verification status gains D-15, D-35, D-37, F-39. No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — §1 Encoder-layer note "runs two H.264 encodes" → "is to run", CM4 "both use" → "would use" (OQ-115); §3.8 live-encode row (Pi 5/CM5): B-frames reasoning attributed to the MediaMTX report [F-45] (community source); §4.2 Pi 5/CM5 diagram note (H.265, deferred): "the same four cores" replaced — the BCM2712 core count is not in the source register. No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header (Last updated; Applies to sentence; Verification adds topics J and K); "Owner decision of 2026-10-08, second" latency line marked superseded in part; new "Owner decisions of 2026-10-08, third" block (< 1 s for WebRTC viewers only, RTMP best-effort, OQ-005 open for bitrate and rate control; ADR-009 fragmented MP4 mirrored to NVMe SSD + self-powered USB-SATA HDD, ext4 marked Claude's proposal, not an owner decision, OQ-120; decoupled HDD writer, OQ-117, RISK-028; unchanged decisions) and topic J/K research block; §1 layer-9 note; §1.1 row 9 (storage per board [J-03], [J-06], [J-13], [J-18], [J-19]; ADR-009 as governing decision); §2 data-plane diagram extended (recorder → NVMe + decoupled HDD writers; latency scope) with dated note; §2.2 two rows (recorder → storage, live → WebRTC viewer); §3.1 recording drives, unknown-row superseded in part (OQ-121, OQ-122), RISK-026/027, ADR-009; §3.7 back-pressure bullet [K-34], [K-36]; §3.8 superseded-in-part latency note, `x264enc` baseline note [K-29], new "Live-encode latency facts" row [K-27]–[K-33], [K-39], risk row, implementation-status scope note; §3.9 Recorder/RTMP/WebRTC rows annotated [K-04], [K-15], [K-17], [K-18]; new §3.9.3 (ADR-009 decisions, data-path diagram, fragmented-MP4 facts [J-38]–[J-41], HDD decoupling [J-30], storage-attachment table CM4/CM5 [J-01]–[J-21], [J-24], bandwidth reasoning [J-36], [J-37], power loss and filesystem [J-29], [J-33], [J-35], [J-44], [J-45]) and new §3.9.4 (target, candidate RTSP → MediaMTX → WebRTC/WHEP route [K-05], [K-06], [F-45], design-rule table [K-04], [K-07]–[K-10], [K-27]–[K-32], [K-34]–[K-37], budget summary [K-45] pointing to PERFORMANCE.md, MediaMTX and internet-viewer facts [K-41]–[K-44]); §3.9 Decisions and implementation-status notes; §4.1 and §4.2 diagrams extended with dated notes and "Recording drives" rows; §4.3 two rows; §5 one row annotated and nine rows added (RISK-026 to RISK-034); §6 rows 1, 8, 9; §7 ADR-009 row, ADR-004/ADR-007 annotations, status note; §9 OQ-005 and OQ-098 rows annotated, OQ-116 (ANSWERED) and OQ-117 to OQ-128 rows added; Verification status gains topics J and K with CORRECTED, community and reasoning entries. The §3.9.4 capture wording follows the register reading of 2026-10-09 ([K-34]: at least one frame's readout on CM4; [K-35]: about one on CM5). No requirement, ADR, risk, OQ or test status changed; implementation status unchanged (NOT STARTED). Verifier pass (same date): §3.8 live-encode latency row "no 1080p figure exists for BCM2711" → "the register gives no 1080p figure for BCM2711", and the recording encode "would share" the encoder (OQ-115); §3.8 x264enc note "one of three documented ways" → "one of the three ways [K-29] names"; §3.9.3 [J-30] worded as one Seagate BarraCuda 2.5-inch family (an example, not a general HDD figure), CM4 encoders "would share" one hardware encoder (OQ-115), and the 0.63 % figure tied to the ~4 Gbit/s after 8b/10b coding [J-37]; §3.9.4 diagram note "this is the GStreamer route" → "the candidate GStreamer route", with the self-built gst-plugins-rs and `webrtcbin` [F-42] alternatives named (ADR-007 OPEN); §3.9.4 keyframe row: the 60-frame GOP is the CM4 encoder's default [K-30]; budget summary and §4.3 row: the recording encode "would share" the CM4 encoder. Main session, same date: "self-powered enclosure or hub" stated as the owner decision reworded to the owner's words, "self-powered enclosure"; where ADR-009 decision 3 is quoted, "or hub" is marked as Claude's addition (consistent with the DECISIONS.md correction of 2026-10-09). | Claude (session 2026-10-09) |
