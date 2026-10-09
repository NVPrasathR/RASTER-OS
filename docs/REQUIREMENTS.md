# PACSCORDER Requirements

| | |
|---|---|
| Document status | DRAFT — no requirement has been accepted with acceptance criteria yet (OQ-017) |
| Last updated | 2026-10-09 |
| Applies to | The PACSCORDER product on all four candidate platforms: Pi 4 Model B, CM4, Pi 5, CM5 |
| Verification | Requirements only. Supporting facts are from source research of 2026-10-06 and 2026-10-08 (topics H, I, J and K) ([REFERENCES.md](REFERENCES.md)). Nothing has been tested on PACSCORDER hardware; no hardware exists as of 2026-10-06. |
| Owner | Project owner |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rules 11, 12, 22 |

This document lists every product requirement with a stable ID (Rule 11). Requirements are never deleted. A requirement that is dropped is marked `WITHDRAWN`, and the reason is written here (Rule 11, Rule 14).

Traceability from each requirement to design, implementation, test and result is kept in [TRACEABILITY.md](TRACEABILITY.md) (Rule 12).

## How to read this document

Each requirement has two independent status fields.

**Acceptance** — where the requirement came from and whether the owner has agreed to it:

| Value | Meaning |
|---|---|
| `ACCEPTED` | The owner has confirmed the wording and acceptance criteria. |
| `DRAFT` | The requirement comes from the owner's engineering rules ([ENGINEERING_RULES.md](ENGINEERING_RULES.md)) or from a dated owner statement quoted in the requirement. The intent is the owner's, but the wording (written by Claude), parameters or acceptance criteria are not yet confirmed. |
| `PROPOSED` | Claude derived it from verified research (cited by fact ID from [REFERENCES.md](REFERENCES.md)). The owner has not agreed to it. |
| `DEFERRED` | The owner wants it later, but it is not in the current scope. It has no planned tests; the reason and date are recorded in the requirement. |
| `WITHDRAWN` | No longer required. The reason is recorded in the requirement. |

**Implementation status** — Rule 10 vocabulary only:

`NOT STARTED` · `IMPLEMENTED — NOT TESTED` · `TESTED — PASS` · `TESTED — FAIL` · `PARTIALLY VERIFIED` · `BLOCKED — HARDWARE REQUIRED`

As of 2026-10-06 **no hardware exists** (owner, 2026-10-06), so every hardware-dependent requirement is `NOT STARTED` and its test is `BLOCKED — HARDWARE REQUIRED`.

Fact references such as `[C-37]` point to [REFERENCES.md](REFERENCES.md). Open questions such as `OQ-012` point to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Decisions such as `ADR-004` point to [DECISIONS.md](DECISIONS.md).

## Requirement summary

| ID | Title | Acceptance | Implementation | Test |
|---|---|---|---|---|
| REQ-ARCH-001 | Video pipeline architecture | DRAFT | NOT STARTED | TEST-PLT-001, TEST-DMA-001 |
| REQ-PLT-001 | Raspberry Pi platform coverage | DRAFT | NOT STARTED | TEST-PLT-001 |
| REQ-DRV-001 | TC358743 controlled by a Linux kernel driver | DRAFT | NOT STARTED | TEST-HW-001, TEST-DRV-001 |
| REQ-CAP-001 | 1920x1080@60 HDMI capture | DRAFT | NOT STARTED | TEST-CAP-002 |
| REQ-CAP-002 | Capture through V4L2 and Media Controller | DRAFT | NOT STARTED | TEST-CAP-001 |
| REQ-CAP-003 | EDID provisioning at every start | PROPOSED | NOT STARTED | TEST-DRV-002 |
| REQ-CAP-004 | Source change handling | PROPOSED | NOT STARTED | TEST-CAP-003 |
| REQ-CAP-005 | Unsupported input mode handling | PROPOSED | NOT STARTED | TEST-CAP-004 |
| REQ-CAP-006 | HDMI audio capture | DRAFT | NOT STARTED | TEST-AUD-001 |
| REQ-CAP-007 | 2-lane and 4-lane CSI-2 configurations, all frame rates each link carries | DRAFT | NOT STARTED | TEST-CAP-004, TEST-CAP-002 |
| REQ-CAP-008 | HDMI sources: ATEM switchers and cameras | DRAFT | NOT STARTED | TEST-CAP-001, TEST-CAP-004, TEST-ATEM-001 |
| REQ-DMA-001 | DMABUF buffer sharing | DRAFT | NOT STARTED | TEST-DMA-001 |
| REQ-ENC-001 | Video encoding (H.264) | DRAFT | NOT STARTED | TEST-ENC-001 |
| REQ-ENC-002 | H.265 (HEVC) encoding (deferred) | DEFERRED | NOT STARTED | — (deferred) |
| REQ-REC-001 | Recording | DRAFT | NOT STARTED | TEST-REC-001 |
| REQ-STR-001 | RTMP streaming | DRAFT | NOT STARTED | TEST-STR-001 |
| REQ-STR-002 | WebRTC streaming | DRAFT | NOT STARTED | TEST-STR-002 |
| REQ-ATEM-001 | Blackmagic ATEM integration | DRAFT | NOT STARTED | TEST-ATEM-001 |
| REQ-BLD-001 | Reproducible product image build | DRAFT | NOT STARTED | TEST-BLD-001 |
| REQ-BLD-002 | Project-owned product OS image | DRAFT | NOT STARTED | TEST-BLD-001 |
| REQ-PERF-001 | Sustained operation | PROPOSED | NOT STARTED | TEST-PERF-001 |

---

## REQ-ARCH-001 — Video pipeline architecture

**Requirement.** The video path shall follow this architecture:

```text
HDMI source → TC358743 → CSI-2 → Raspberry Pi CSI-2 receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder / RTMP / WebRTC
```

- **Source:** ENGINEERING_RULES.md Rule 5.
- **Acceptance:** DRAFT — the architecture is mandated by the rules; acceptance criteria are not defined.
- **Implementation:** NOT STARTED.
- **Notes:** The receiver and Media Controller layer differ per platform. Pi 4/CM4 use Unicam [C-09]; Pi 5/CM5 use the RP1 CFE, which supports Media Controller only [C-11], [C-29]. See [ARCHITECTURE.md](ARCHITECTURE.md).

## REQ-PLT-001 — Raspberry Pi platform coverage

**Requirement.** The project shall document and support configuration for Raspberry Pi 4 Model B, Compute Module 4, Raspberry Pi 5 and Compute Module 5.

- **Source:** ENGINEERING_RULES.md Rule 6 (Pi 4 / CM4 / Pi 5 / CM5 configurations). Owner, 2026-10-06: the product target platform is **undecided — keep all four** (OQ-011).
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:** The platforms are not equivalent. Pi 4 Model B has a 2-lane camera connector [C-01], which cannot carry 1080p60 from the TC358743 (official documentation [C-37]; reasoning-tier lane calculation [C-48]). It therefore cannot serve the 4-lane configuration that 1080p60 (REQ-CAP-001) needs; under the owner decision of 2026-10-07 it remains a candidate for the 2-lane configuration (REQ-CAP-007). The product platform choice is an open decision, made per lane configuration: ADR-004.

## REQ-DRV-001 — TC358743 controlled by a Linux kernel driver

**Requirement.** The TC358743 shall be configured and monitored by a Linux kernel driver that exposes it as a V4L2 sub-device in the Media Controller graph. Userspace shall not access the TC358743 or the CSI-2 receiver registers directly.

- **Source:** ENGINEERING_RULES.md Rule 5 (V4L2 / Media Controller in the pipeline), Rule 7 (driver documentation), Rule 13 example decision (use V4L2 instead of direct userspace access). Recorded as ADR-001.
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:** An in-tree driver exists (`drivers/media/i2c/tc358743.c`). It is identical in mainline Linux and in the Raspberry Pi kernel branch `rpi-6.18.y` [A-48], [B-02]. Whether PACSCORDER uses it as-is, patches it, or replaces it is ADR-002 (PROPOSED: use it as-is).

## REQ-CAP-001 — 1920x1080@60 HDMI capture

**Requirement.** The system shall capture HDMI input through the TC358743 at 1920x1080@60Hz.

- **Source:** ENGINEERING_RULES.md Rule 11 (example requirement text, used here as the owner's stated intent).
- **Acceptance:** DRAFT. The owner must confirm the following:
  - whether 1080p60 is mandatory at all (OQ-001) — **answered by the owner on 2026-10-07:** "i need 2 lane and 4 lane with all frame rate". Recorded interpretation: 1080p60 is required on 4-lane configurations; 2-lane configurations capture every rate their link can carry (REQ-CAP-007);
  - whether 60 Hz is mandatory or 59.94 Hz is also required (OQ-002, OQ-040);
  - whether 50 Hz is required (OQ-002);
  - which pixel format is acceptable (OQ-003, ADR-005);
  - whether audio is part of this requirement (see REQ-CAP-006, OQ-004).
- **Implementation:** NOT STARTED. Test status: BLOCKED — HARDWARE REQUIRED.
- **Verified constraints (from sources, not hardware):**
  - Reasoning from the driver's lane formula (reasoning-tier entries; A-26 CORRECTED): 1080p60 needs more than 2 CSI-2 lanes in both formats the driver supports. UYVY needs 3 lanes and RGB888 needs 4 at the overlay default of 972 Mbps per lane [A-26], [C-47], [C-48].
  - Official Raspberry Pi documentation states that 2 lanes give at most 1080p30 RGB888 or 1080p50 YUV422, and that 4 lanes on a Compute Module give 1080p60 [C-37].
  - 4-lane connectors exist on CM4 CAM1 [C-02], on both Pi 5 ports [C-04] and on CM5 MIPI0/1 [C-05]. **Pi 4 Model B cannot meet this requirement** [C-01]; under the owner decision of 2026-10-07 it remains a candidate for the 2-lane configuration (REQ-CAP-007).
  - A 4-lane port is necessary but not shown to be sufficient. Reasoning from the driver's lane formula at 972 Mbps: 1080p60 UYVY activates 3 of the 4 configured lanes [C-47]. The receivers accept 3 of 4 lanes at stream start [C-16], but capture with 3 of 4 lanes is unproven (OQ-038), and corruption near lane-count limits has been reported in a community issue (RISK-006) [C-43]. The bridge board's own lane count is UNKNOWN (OQ-021). The link frequency is ADR-008 (PROPOSED; OQ-099), which proposes keeping the 486 MHz default and evaluating 297 MHz on a CM4 CAM1 4-lane link, where 1080p60 UYVY uses all 4 lanes, in TEST-CAP-002.
  - The Pi 4/CM4 hardware H.264 encoder is officially specified for 1080p30 encode [D-10], so 1080p60 *encode* is a separate unresolved risk (REQ-ENC-001, RISK-002).

## REQ-CAP-002 — Capture through V4L2 and Media Controller

**Requirement.** Captured frames shall be obtained from a V4L2 video capture device fed by the platform CSI-2 receiver driver. Formats and links shall be configured through the V4L2 and Media Controller APIs.

- **Source:** ENGINEERING_RULES.md Rule 5.
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:**
  - libcamera cannot be used for the TC358743: the bridge is not a raw camera sensor and exposes no link-frequency or pixel-rate control [B-16]. Raspberry Pi engineers reported (community source) that libcamera does not support it [C-41].
  - Pi 4/CM4 can run Unicam in legacy video-node mode (the overlay default) or in Media Controller mode [B-43], [C-10]. Pi 5/CM5 support Media Controller mode only [C-11]. The mode choice is ADR-006.

## REQ-CAP-003 — EDID provisioning at every start

**Requirement (PROPOSED).** The system shall write a project-defined EDID to the TC358743 after every boot and every driver load, before capture is expected. The EDID shall advertise only modes the configured CSI-2 link can carry.

- **Source:** Derived by Claude from verified facts.
  - The driver never asserts HDMI hot-plug until an EDID has been written [A-33], [B-21] (B-21 CORRECTED).
  - After every probe the driver has no EDID stored (`edid_blocks_written == 0`) [B-21], and the EDID is held in the chip's embedded SRAM [A-31]. Reasoning: an EDID must therefore be written after every boot and every driver load. Research also found no default EDID and no Device Tree property for one in the driver (research gap, topic B — not a register fact).
  - The TMDS clock is limited to 165 MHz [A-07], [B-26].
- **Acceptance:** PROPOSED. Without this, no HDMI source will output video to the device.
- **Implementation:** NOT STARTED.
- **Open:** EDID contents are an open decision: OQ-002 (OWNER DECISION REQUIRED). The HPD state between power-on and the first EDID write is OQ-032. What triggers the EDID write and in what order relative to overlay, module and media-graph setup is OQ-093.

## REQ-CAP-004 — Source change handling

**Requirement (PROPOSED).** The system shall detect the following and reconfigure the capture pipeline without a reboot:

- an HDMI source being connected or disconnected;
- a loss of signal;
- a change in input timing.

- **Source:** Derived by Claude from verified facts.
  - The driver emits `V4L2_EVENT_SOURCE_CHANGE` but does not apply new timings itself [B-39].
  - Stored timings are cleared when +5V is lost [B-23].
  - On Pi 5 an open issue reports that the CFE video nodes do not deliver the event, and a Raspberry Pi engineer advised subscribing on the TC358743 sub-device node instead (community source) [C-42]; OQ-051.
  - Without a wired interrupt, the driver polls every 1000 ms [A-30], [B-19].
- **Acceptance:** PROPOSED.
- **Implementation:** NOT STARTED.

## REQ-CAP-005 — Unsupported input mode handling

**Requirement (PROPOSED).** When the HDMI source sends a mode the pipeline cannot carry, the system shall report it as unsupported and shall not stream corrupted video. Such modes include:

- interlaced video;
- a pixel clock above 165 MHz;
- a mode that needs more CSI-2 lanes than are wired.

- **Source:** Derived by Claude from verified facts.
  - Interlaced input is rejected with `-ERANGE` (reasoning from driver source) [B-27].
  - The DV timings capability is 640–1920 × 350–1200 and 13–165 MHz [A-08].
  - Receivers fail STREAMON when the bridge requests more lanes than are configured in the Device Tree [B-32], [C-16]. Note: the receivers compare against the Device Tree `data-lanes` value, not the physical wiring. The DT value must therefore match the lanes the board actually routes (OQ-021); a DT value larger than the wiring is not caught by this check ([CSI_PIPELINE.md](CSI_PIPELINE.md) §5).
  - Image corruption has been reported near lane-count boundaries in a community issue [C-43].
- **Acceptance:** PROPOSED.
- **Implementation:** NOT STARTED.

## REQ-CAP-006 — HDMI audio capture

**Requirement.** The system shall capture the embedded HDMI audio of the source from the TC358743 and include it in recordings and streams.

- **Owner decision, 2026-10-07:** audio is required ("Yes, audio required", answer to OQ-004). Acceptance changed from PROPOSED to DRAFT. The original PROPOSED wording (2026-10-06) was: "If the product records or streams audio, embedded HDMI audio shall be captured from the TC358743."

- **Source:** Derived by Claude.
  - RTMP/FLV carries AAC audio [F-31].
  - ATEM program audio is available only embedded in HDMI/USB [F-24] (CORRECTED).
  - The TC358743 silicon can send audio over CSI-2 [A-05] or on I2S/TDM output pins [A-11]. The Linux driver always configures 2-channel I2S [A-13], and the `tc358743-audio` overlay expects that I2S on GPIO 18/19/20 [A-47]. Reasoning: with the stock driver, audio therefore needs I2S wiring that is separate from the CSI-2 camera cable (OQ-025). Whether audio over CSI-2 works with the Raspberry Pi receivers is OQ-033.
- **Acceptance:** DRAFT. Channels, sample rates and the audio/video synchronisation tolerance are not yet specified (OQ-004 answered for "required"; parameters remain open there and in OQ-025, OQ-054, OQ-063). *(2026-10-08: also tracked in OQ-110 — channels and formats; OQ-111 — sample-rate policy; OQ-112 — A/V synchronisation and tolerance; OQ-114 — GPIO 18–21 allocation.)*
- **Implementation:** NOT STARTED.
- **Evidence added 2026-10-08 (research topic I; from sources, not hardware; wording, acceptance and status unchanged):**
  - Overlay: `tc358743-audio` enables `i2s_clk_consumer`, adds a `linux,spdif-dir` stub codec as bit-clock and frame master, and creates the ALSA card `tc358743` with 2 × 32-bit slots [I-01], [I-02], [I-03], [I-15]. The README's "tc358743-fast" is a stale overlay name [I-04]. The datasheet makes the TC358743 the I2S clock master only [I-25].
  - CM4: the CPU side is `bcm2835-i2s` on GPIO 18–21, which captures exactly 2 channels at 8–384 kHz [I-10], [I-13].
  - CM5: `overlay_map` does not block the overlay, and its labels resolve to RP1 I2S1 on GPIO 18–21, the clock-consumer instance that a codec-master link needs [I-05], [I-06], [I-07], [I-08], [I-09]; RP1 I2S1's channel count and formats come from hardware registers [I-14]. No source shows audio being captured through this path on CM5 (OQ-054).
  - Channels: the driver hard-codes 2-channel I2S at probe [I-24]. Reasoning: with the stock driver the requirement can be met for stereo only; compressed or multichannel input is OQ-110.
  - Sample rate: reasoning from source — the kernel does not carry the HDMI sample rate into ALSA, and a mismatch makes audio run fast or slow without an error [I-18]. The driver exposes the rate as a read-only control with a change event [I-19], [I-20], [I-21]; without a wired interrupt a change takes up to about 1 s to appear [I-22] (OQ-111, RISK-023).
  - Synchronisation: the TC358743's audio PLL follows the source's audio clock [I-26], video buffers are stamped with `CLOCK_MONOTONIC` [I-33], and GStreamer and FFmpeg align the two differently by default [I-35], [I-36], [I-37], [I-38] (OQ-112, RISK-024).
  - Pins: the audio pins are VDDIO2 outputs, rated 1.8–3.3 V [I-27]; the CM4 and CM5 IO Boards have a selectable 1.8 V or 3.3 V GPIO voltage, which VDDIO2 should match, or the lines need level shifting [I-29]. The overlay claims GPIO 18–21 [I-30], which conflicts with overlays that default to GPIO 18 [I-31] (OQ-114).
  - Encoders: FFmpeg native `aac` and `libopus`, GStreamer `voaacenc`, `avenc_aac` and `opusenc` are available in Raspberry Pi OS; `fdk-aac` is non-free and GPL-incompatible [I-39], [I-41], [I-43], [I-44], [I-45], [I-46] (OQ-063).

## REQ-CAP-007 — 2-lane and 4-lane CSI-2 configurations, all frame rates each link carries

**Requirement.** The system shall support TC358743 capture over both a 2-lane and a 4-lane CSI-2 link. On each configuration it shall capture every HDMI input frame rate that the configured link can carry.

- **Source:** Owner statement, 2026-10-07: "i need 2 lane and 4 lane with all frame rate" (answer to OQ-001). Wording by Claude.
- **Acceptance:** DRAFT. Acceptance criteria are the supported-mode matrix for each lane count, which is not yet defined (OQ-002). Fractional rates (23.98 / 29.97 / 59.94 Hz) are part of "all frame rates" unless the owner says otherwise; the driver reports integer frame rates [B-28] (OQ-040).
- **Implementation:** NOT STARTED. Test status: BLOCKED — HARDWARE REQUIRED.
- **Verified constraints (from sources, not hardware):**
  - Physical limit: a 2-lane link cannot carry 1080p60 in either supported format. Official Raspberry Pi documentation gives at most 1080p30 RGB888 or 1080p50 YUV422 on 2 lanes [C-37]. Reasoning at the default 972 Mbit/s per lane: 1080p30 UYVY and RGB888 and 1080p50 UYVY fit; 1080p50 RGB888 and 1080p60 in either format do not [C-48]. "All frame rates" on a 2-lane configuration therefore means all rates up to that limit.
  - On 4 lanes, all 1080p30/50/60 × UYVY/RGB888 combinations fit by bandwidth (reasoning) [C-49]. 1080p60 UYVY then uses 3 of the 4 lanes (reasoning) [C-47], which is unproven (OQ-038; ADR-008).
  - Lower resolutions need fewer lanes. Reasoning at 972 Mbit/s: 720p60 needs 1 lane in UYVY and 2 in RGB888 [B-33].
  - 2-lane configurations: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port (reasoning [C-48]; board lane count OQ-021). 4-lane configurations: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05].
  - The `4lane` overlay parameter switches `data-lanes` to `<1 2 3 4>` [B-42]. On the 2-lane ports (Pi 4 Model B, CM4 CAM0; both Unicam) that does not fail the probe: Unicam logs a message and adopts the endpoint's lane count [C-17]. The configuration must therefore match the wiring ([DEVICE_TREE.md](DEVICE_TREE.md)).

## REQ-CAP-008 — HDMI sources: ATEM switchers and cameras

**Requirement.** The system shall capture HDMI video both from the HDMI output of Blackmagic ATEM switchers and directly from cameras.

- **Source:** Owner statement, 2026-10-07: "it can be atem and direct video from camera" (answer to OQ-009). Wording by Claude.
- **Acceptance:** DRAFT. Sources (owner decision, 2026-10-07; OQ-102 ANSWERED): **any HDMI camera, no model list** ("Any HDMI camera (generic)"), plus ATEM switcher outputs (OQ-009). What is accepted is defined by the supported-mode matrix and the EDID (OQ-002), not by a model list.
- **Implementation:** NOT STARTED. Test status: BLOCKED — HARDWARE REQUIRED.
- **Notes (from sources, not hardware):**
  - ATEM Mini Pro HDMI output standards are 1080p23.98 to 1080p60, with no 720p or 1080i; video is 4:2:2 YUV, 10-bit, Rec 709 [F-23]. Its HDMI output defaults to multiview, not program [F-25].
  - Camera HDMI output modes are UNKNOWN — VERIFICATION REQUIRED per camera model (OQ-102). Reasoning from driver source: the driver accepts progressive timings only; interlaced input returns `-ERANGE` [B-27] (RISK-009). The TMDS clock limit is 165 MHz [A-07]. The driver disables HDCP [A-04] (RISK-008).

## REQ-DMA-001 — DMABUF buffer sharing

**Requirement.** Video frames shall pass from capture to encoder as DMABUF buffers without a CPU copy, where the platform encoder supports DMABUF import.

- **Source:** ENGINEERING_RULES.md Rule 5 ("DMABUF" stage).
- **Acceptance:** DRAFT.
- **Implementation:** NOT STARTED.
- **Notes:** (The cited facts are `CONFIRMED` in [REFERENCES.md](REFERENCES.md).)
  - The Pi 4/CM4 encoder queues support DMABUF import [D-19]; an imported buffer must be a single DMA-contiguous region at least as large as the plane [D-20]. Whether Unicam capture buffers meet that without a copy is OQ-058.
  - On Pi 5/CM5 encoding runs in software, so "zero-copy into the encoder" has a different meaning there [D-30], [G-22] (OQ-060).
  - See [DMA.md](DMA.md).

## REQ-ENC-001 — Video encoding

**Requirement.** Captured video shall be compressed by an encoder before recording and streaming.

- **Source:** ENGINEERING_RULES.md Rule 5 ("Encoder" stage).
- **Codec (owner decision, 2026-10-08): H.264 only, for all outputs** ("H.264 only for now", answer to OQ-103). H.265 is deferred and recorded as REQ-ENC-002 (DEFERRED).
- **Encodes (owner decision, 2026-10-08):** two simultaneous H.264 encodes — one for **recording** and one **live** encode shared by RTMP and WebRTC ("Separate record + live", answer to OQ-005). Bitrate, rate control and latency are still open (OQ-005). *(Superseded in part 2026-10-09: the latency target was set by the owner later on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers only, RTMP best-effort (OQ-116 ANSWERED; REQ-STR-002). Bitrate and rate control remain open.)* CM4 concurrency: OQ-115; CM5: OQ-059.
  - Reasoning from sources: because the live encode also feeds WebRTC, it must be receivable by browsers — RFC 7742 requires H.264 Constrained Baseline support [F-36], libwebrtc assumes Constrained Baseline Level 3.1 when unsignalled [F-39], 1080p needs Level 4.0 or above [F-40], and browsers do not accept B-frames in WebRTC (reported by MediaMTX) [F-45]. The Pi 4/CM4 encoder produces no B-frames [D-14] and offers Constrained Baseline [D-11]; in-band SPS/PPS must be enabled for late joiners [D-15].
- *(History.)* On 2026-10-07 the owner chose "H.264 + H.265 (HEVC)" (answer to OQ-005). The 2026-10-08 answer to OQ-103 narrowed this to H.264 after the topic H research.
- **Acceptance:** DRAFT. The following are still **UNDEFINED — owner to specify** (OQ-005):
  - bitrate; *(Superseded 2026-10-09: owner decisions of 2026-10-09 — live encode CBR 17 Mbit/s, recording encode 25 Mbit/s VBR (OQ-005). Whether these fit the signalled H.264 profile and level: OQ-073.)*
  - latency target; *(Superseded in part 2026-10-09: set by the owner on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers only, RTMP best-effort (OQ-116 ANSWERED; see REQ-STR-002). Bitrate and rate control remain open (OQ-005).)*
  - how many simultaneous encodes are needed. *(Answered 2026-10-08 by the owner: two — see **Encodes** above (OQ-005). Acceptance stays DRAFT; bitrate and latency target are still open.)*
- **Implementation:** NOT STARTED.
- **Notes:**
  - Pi 4/CM4 have a hardware H.264 encoder, officially specified for 1080p30 [D-10].
  - Pi 5/CM5 have no hardware video encoder. Official figure: "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22].
  - No candidate platform has a hardware HEVC encoder [D-24], [D-31], so H.265 is software-encoded on every candidate (reasoning; RISK-022). Legacy RTMP/FLV carries only H.264 video; HEVC needs Enhanced RTMP [F-31]. WebRTC mandates only VP8 and H.264 [F-36].
  - *(Added 2026-10-08, research topic H; wording, acceptance and status unchanged.)* Software H.265 encoders in Raspberry Pi OS: Debian's x265 4.1-2, not overridden by Raspberry Pi [H-01], [H-02]; `libx265` in the Raspberry Pi FFmpeg 7.1.5 [H-08], [H-09]; GStreamer `x265enc` [H-11], [H-12] (CORRECTED). Both accept planar input only, not the TC358743's packed UYVY [H-10] (CORRECTED), [H-13]; reasoning: the conversion at 1080p60 reads about 249 MB/s and writes about 187 MB/s on the CPU [H-43].
  - *(Added 2026-10-08.)* H.265 cost: research found no official figure in a site search of raspberrypi.com [H-19]. A Raspberry Pi engineer stated on the official forum that software H.265 encode "is too intensive an operation to perform at any significant resolution" (community source) [H-19]. Community benchmarks report `libx265` at 10.00 FPS on Pi 5 and 4.33 FPS on a Pi 400 in a test that is not a 1080p60 live measurement (community sources) [H-20], [H-21], [H-22]; reasoning: about 6.6 times slower than `libx264` in that harness [H-23]. x265's Neon DotProd kernels can apply only on CM5 [H-04], [H-05]. Measurement: OQ-104, OQ-105.
  - *(Added 2026-10-08.)* Low latency: `tune=zerolatency` disables B-frames and lookahead and uses one frame thread [H-16]; GStreamer 1.26.2 `x265enc` reports a hard-coded 5-frame latency unless `tune=zerolatency` [H-15]. *(Superseded 2026-10-08: OQ-103 is answered — H.264 only; these H.265 notes are kept as evidence for REQ-ENC-002.)*
  - *(Added later on 2026-10-08, research topic K — the live encode, keyframes and low latency; wording, acceptance and status unchanged.)*
    - CM4 hardware encoder (`bcm2835-codec`, `rpi-6.18.y`): B-frames are limited to 0, so it never emits them; its defaults are profile High, level 4.0 and a GOP of 60 [K-30]. Reasoning from [K-30] and [K-17]: a 60-frame GOP is 2 s at 30 fps and 1 s at 60 fps, within YouTube's 2 s keyframe recommendation. It implements force-keyframe, and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe, so an IDR can be requested on demand, for example when a WebRTC viewer joins [K-31]. GStreamer's `v4l2h264enc` on BCM2711 reports 0 latency to the pipeline [K-32]. A Raspberry Pi engineer stated that the encoder holds no extra buffers and its latency depends on the macroblock count, about 10 ms for 720p on a Pi 4 (community source) [K-33].
    - CM5 software encoder (x264): the `zerolatency` tune sets lookahead, sync-lookahead and B-frames to 0 and uses sliced threads [K-27], so x264 holds no frames back and GStreamer `x264enc` reports 0 frames of latency [K-28]. Without it, GStreamer 1.26 `x264enc` runs x264's medium preset (3 B-frames, rc-lookahead 40) by default; its `bframes=0` property default does not guarantee a B-frame-free stream unless `tune=zerolatency`, an explicit `bframes=0` or `profile=baseline` in caps is set (CORRECTED) [K-29]. Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39].
    - Shared live encode: MediaMTX documents that browsers deliberately do not support H.264 B-frames over WebRTC and recommends Baseline [K-04]; YouTube recommends 2 s keyframes (not over 4 s), CBR, and 2 B-frames [K-17]. Reasoning: the live encode therefore has to be B-frame-free, and its keyframe interval is bounded by YouTube's recommendation and by WebRTC viewer join time, unless keyframes are requested when a viewer joins (OQ-127, RISK-019). Live-latency budget: OQ-125, RISK-031.

## REQ-ENC-002 — H.265 (HEVC) encoding (deferred)

**Requirement.** The system may in future offer H.265 (HEVC) encoding for recording and/or streaming.

- **Source:** Owner decisions. 2026-10-07: "H.264 + H.265 (HEVC)". 2026-10-08: "H.264 only for now" — H.265 is deferred and recorded as a future requirement (OQ-103 ANSWERED).
- **Acceptance:** DEFERRED (2026-10-08). Not in the current scope; no tests are planned.
- **Implementation:** NOT STARTED.
- **Why deferred (evidence; see REQ-ENC-001 notes and RISK-022):**
  - No candidate board has a hardware HEVC encoder [D-24], [D-31].
  - A Raspberry Pi engineer reported that software H.265 encode is too intensive for these boards (community) [H-19].
  - HEVC over RTMP needs FFmpeg rather than the distribution's GStreamer [H-26], [H-27].
  - Browser support for H.265 in WebRTC is partial [H-33], [H-35].
- **To re-activate:** the owner changes the acceptance to DRAFT. OQ-104 to OQ-109 then become relevant again.

## REQ-REC-001 — Recording

**Requirement.** The system shall record encoded video to local storage.

- **Source:** ENGINEERING_RULES.md Rule 5 ("Recorder").
- **Acceptance:** DRAFT. The following are **UNDEFINED — owner to specify** (OQ-006):
  - container format; *(Superseded 2026-10-09: MP4, written fragmented — owner decisions of 2026-10-08, ADR-009 ACCEPTED.)*
  - storage medium; *(Superseded 2026-10-09: PCIe NVMe SSD and USB-to-SATA HDD, every recording mirrored to both, HDD in a self-powered enclosure — owner decisions of 2026-10-08, ADR-009 ACCEPTED.)*
  - minimum recording duration; *(Superseded 2026-10-09: no fixed limit — recordings run until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED). Behaviour when one mirrored drive fills or fails first: OQ-129.)*
  - behaviour on power loss. *(Superseded in part 2026-10-09: the strategy is fragmented MP4 (owner decision of 2026-10-08, ADR-009 ACCEPTED); how much is still lost on a power cut is open (OQ-119). Acceptance stays DRAFT.)*
- **Owner decisions (2026-10-08):** container **MP4**; storage **PCIe NVMe SSD and USB-to-SATA HDD** ("pcie nvme and usb to sata hdd"; OQ-006). Duration and power-loss behaviour are still UNDEFINED (OQ-006). Interface facts for CM4/CM5: OQ-098. *(2026-10-08, later: the power-loss strategy is now decided — fragmented MP4, see the next line and ADR-009; the amount that may be lost is measured under OQ-119. Duration is still UNDEFINED, OQ-006.)* *(Superseded 2026-10-09: duration decided — until stopped or the disk is full (owner, 2026-10-09; OQ-006 ANSWERED); follow-up OQ-129.)*
- **Owner decisions (2026-10-08, second):** each recording is mirrored to both drives; the HDD is in a self-powered enclosure; MP4 is written fragmented for power-loss safety (ADR-009, ACCEPTED).
- **Owner decisions (2026-10-09):** recording encode 25 Mbit/s, VBR (OQ-005). If one mirrored drive fills, is absent or fails, recording continues on the other drive and the operator is alerted (OQ-129; file splitting, alert method and drive return still open).
- **Implementation:** NOT STARTED.
- **Notes (added 2026-10-08; from sources, not hardware; wording, acceptance and status unchanged):**
  - HEVC recording (H.265 is required, REQ-ENC-001): GStreamer 1.26.2 `qtmux`/`mp4mux` and `matroskamux` accept H.265, with `h265parse` needed after `x265enc`; `matroskamux` warns that the `hev1` form is not officially supported [H-37]. FFmpeg 7.1.5's MP4 muxer tags HEVC as `hev1`, `hvc1` or `dvh1`, and its Matroska muxer handles HEVC [H-38]. *(Superseded 2026-10-08, later: OQ-103 ANSWERED — recordings are H.264 only; H.265 is deferred (REQ-ENC-002, DEFERRED). This HEVC note is kept as evidence for REQ-ENC-002 and is not in current scope.)*
  - Audio in recordings (REQ-CAP-006): AAC encoders available in Raspberry Pi OS are FFmpeg's native `aac` (128 kb/s stereo by default; CORRECTED) [I-40], GStreamer `voaacenc` [I-44] and `avenc_aac` [I-46]. The recorded audio rate must follow the source (RISK-023, OQ-111), and A/V synchronisation is unmeasured (RISK-024, OQ-112).
- **Notes (added later on 2026-10-08, research topic J — storage and power-loss safety for ADR-009; from sources, not hardware; wording, acceptance and status unchanged):**
  - CM4: one PCIe Gen 2 x1 lane and one USB 2.0 port [J-01]; no USB 3.0 controller [J-08]. On the CM4 IO Board the NVMe SSD goes in the single PCIe Gen 2 x1 socket through a passive adaptor [J-03], [J-11]; that slot is powered only from the 12 V barrel input [J-05]; the HDD then runs at USB 2.0 through the board's USB2514B hub, whose ports share one VBUS switch of about 1.2 A [J-06]; reasoning (CORRECTED): that USB 2.0 path is shared with every other USB device unless a PCIe switch is added, and booting through a switch is not supported [J-09], [J-04]. (RISK-026; OQ-121, OQ-122, OQ-123.)
  - CM5: the CM5 IO Board's M.2 M-key slot runs at PCIe Gen 2 x1 and takes 2230 to 2280 drives [J-13], [J-14]; in the `rpi-6.18.y` device tree that link is disabled by default and `dtparam=pciex1` controls it [J-15], [J-17] (OQ-124). Two USB 3.0 ports share about 1.2 A of VBUS [J-18], [J-19]. *(Corrected 2026-10-09: [J-15] says to set `pciex1` (alias `nvme`) to "on"; no register source quotes the bare `dtparam=pciex1` line, so the exact `config.txt` line is NEEDS VERIFICATION (OQ-100, OQ-124). The two ports sharing about 1.2 A are the CM5 IO Board's USB 3.0 Type-A ports [J-19]; the CM5 module has two USB 3.0 interfaces [J-18].)*
  - HDD power: the register's example 2.5-inch HDD (a Seagate BarraCuda family) draws up to 1.0 A at 5 V to spin up and takes 2.5 s typical, 3.0 s maximum from standby to ready (CORRECTED) [J-30]; no drive or enclosure has been chosen for PACSCORDER (OQ-122; spin-down behaviour OQ-117); a 3.5-inch HDD also needs 12 V [J-32] *(corrected 2026-10-09: [J-32] is one Seagate BarraCuda 3.5-inch family's datasheet; that any 3.5-inch HDD needs an external 12 V supply is the entry's reasoning, because USB VBUS is 5 V only)*; reasoning: bus power is marginal or insufficient [J-31]; Raspberry Pi advises a powered hub or enclosure for HDDs [J-28]. ADR-009 chose a self-powered enclosure.
  - USB-to-SATA bridges: `uas` cannot bind behind CM4's `dwc2` controller but can under Raspberry Pi OS's default `otg_mode=1` [J-24], [J-07]; the kernel applies bridge quirks [J-25]; a Raspberry Pi engineer's forum post reports that some UAS bridges stop responding or, rarely, lose written data, with `usb-storage.quirks=…:u` as the workaround (community source, CORRECTED) [J-27] (RISK-027, OQ-122).
  - Filesystem: the CM4 and CM5 defconfigs build ext4, VFAT, NVMe, USB storage and UAS in, and exFAT as a module [J-29]; ext4's 5 s commit limits metadata loss to 5 s, but delayed allocation can lose older data (CORRECTED) [J-35]; Raspberry Pi's storage guide says to install `exfat-fuse` for exFAT and documents `nofail` with a device timeout (CORRECTED) [J-33] (OQ-120).
  - MP4 power-loss behaviour: a plain GStreamer `mp4mux` file is unplayable after power loss [J-38]; `fragment-duration` > 0 gives a fragmented file and disables robust muxing [J-39]; FFmpeg documents that fragmented files stay decodable when interrupted but are less compatible, and offers `hybrid_fragmented` [J-45]; `splitmuxsink` can split long recordings at keyframes [J-44]. Robust muxing, not chosen by ADR-009, is described in [J-40] and [J-41]. (RISK-029, RISK-030; OQ-118, OQ-119.)
  - Data rates: reasoning, per destination about 1.02 / 1.52 / 3.15 MB/s at 8 / 12 / 25 Mbit/s plus AAC, and a 1 TB disk holds about 271 h at 8 Mbit/s or 88 h at 25 Mbit/s [J-36]; reasoning: interface bandwidth is not the bottleneck on either module [J-37]. The mirror's HDD branch must not stall the NVMe copy or the live path (ADR-009 Consequences; RISK-028, OQ-117).

## REQ-STR-001 — RTMP streaming

**Requirement.** The system shall publish encoded video, and audio if required, to an RTMP server.

- **Source:** ENGINEERING_RULES.md Rule 5 ("RTMP").
- **Acceptance:** DRAFT. Server targets and bitrate are **UNDEFINED** (OQ-007).
- **Latency (owner decision, 2026-10-08):** RTMP outputs are best-effort; their latency is set by the receiving platform (OQ-116 ANSWERED: the < 1 s target applies to WebRTC viewers only). For example, YouTube documents under 5 s at best, in ultra-low-latency mode [K-15]. *(Corrected 2026-10-09: [K-15] says Ultra-low gives "less than 5 seconds" for most viewers; the CORRECTED [K-18] reads that as an upper bound for most viewers, not a best case.)*
- **Implementation:** NOT STARTED.
- **Notes:** Legacy RTMP/FLV carries H.264 video and AAC audio [F-31]. GStreamer `rtmp2sink` is a client (publisher) [F-33].
  - *(Added 2026-10-08, research topic H; wording, acceptance and status unchanged.)* HEVC over RTMP: the Enhanced RTMP specification defines the HEVC FourCC `hvc1` [H-24]; FFmpeg 6.1 was the first release to mux HEVC into FLV [H-25], and FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP publishing; its FLV muxer has no Opus [H-26]. GStreamer 1.26.2's `flvmux` cannot carry H.265 [H-27] (OQ-107, RISK-025). YouTube Live lists H.264, H.265 and AV1 over RTMP/RTMPS with AAC or MP3 audio [H-29]; whether it and other destinations accept FFmpeg's signalling is OQ-106. Components for HEVC over SRT in MPEG-TS exist in the distribution stacks [H-30] (OQ-076). *(2026-10-08, later: deferred — REQ-ENC-002; not in current scope. RTMP carries H.264 only (OQ-103 ANSWERED); OQ-106, OQ-107 and RISK-025 stay OPEN, not in current scope.)*
  - *(Added 2026-10-08, research topic I.)* AAC encoders: FFmpeg native `aac` (CORRECTED) [I-40], GStreamer `voaacenc` [I-44] and `avenc_aac` [I-46]. `fdk-aac` would need `--enable-nonfree` in the GPL FFmpeg build, making it unredistributable [I-42], [I-43] (OQ-063, OQ-113).
  - *(Added later on 2026-10-08, research topic K; wording, acceptance and status unchanged.)* Latency at the destination: YouTube Live offers Normal (no figure), Low ("less than 10 seconds" for most viewers) and Ultra-low ("less than 5 seconds" for most viewers, no 4K) [K-15]; the API's `latencyPreference` takes normal, low or ultraLow, and ultraLow supports neither closed captions nor resolutions above 1080p [K-16]. Reasoning (CORRECTED): YouTube documents no sub-second mode and names the player's read-ahead buffer as the main source of latency, so PACSCORDER's encoder settings cannot make YouTube meet < 1 s [K-18] — consistent with the owner's best-effort decision (OQ-116). *(Corrected 2026-10-09: the CORRECTED [K-18] concludes that the YouTube output cannot be planned or claimed to meet < 1 s and that PACSCORDER's encoder settings cannot remove YouTube's player-side buffer; it does not show that YouTube can never deliver under 1 s.)* Encoder settings: YouTube recommends a 2 s keyframe frequency (not over 4 s), CBR, 2 B-frames, 1 reference frame and CABAC; the B-frame advice conflicts with the B-frame-free live encode that WebRTC needs [K-17] (OQ-127, RISK-019). Other RTMP platforms were not checked for latency modes (research gap, topic K; OQ-007).

## REQ-STR-002 — WebRTC streaming

**Requirement.** The system shall make live video available to WebRTC clients.

- **Source:** ENGINEERING_RULES.md Rule 5 ("WebRTC").
- **Acceptance:** DRAFT. The following are **UNDEFINED** (OQ-008):
  - target browsers;
  - latency target; *(Superseded 2026-10-09: set by the owner on 2026-10-08 — under 1 s camera-to-viewer for WebRTC viewers (OQ-116 ANSWERED); see **Latency** below. Acceptance stays DRAFT.)* *(Verifier note, same date: settled only in part — how the < 1 s target is judged (statistic, samples, modes, network, load) is still open under OQ-008.)*
  - whether viewers are on the LAN only or over the internet (NAT traversal). *(Superseded 2026-10-09: LAN only (owner, 2026-10-09; OQ-008); internet viewers are not in current scope (OQ-128, RISK-033).)*
- **Latency (owner decision, 2026-10-08):** under 1 second camera-to-viewer for **WebRTC viewers** ("Under 1 second"; "WebRTC viewers only"; OQ-005, OQ-116 ANSWERED).
- **Latency criterion and reach (owner decisions, 2026-10-09):** the target is judged at the **95th percentile** — 95 % of camera-to-viewer samples under 1 s, over a sustained run with the recording running ("95th percentile < 1 s"); viewers are on the **LAN only** ("LAN only"). Still open (OQ-008): target browsers, number of viewers, and the sample count and run length of the measurement.
- **Implementation:** NOT STARTED.
- **Notes:**
  - Browser interoperability requires H.264 Constrained Baseline per RFC 7742 [F-36]. RFC 7874 requires WebRTC endpoints to implement Opus and G.711 audio; AAC is not a required WebRTC codec, so AAC audio has to be transcoded (typically to Opus) for browser playback [F-41].
  - Reasoning from H.264 Table A-1 as encoded in FFmpeg: a 1080p H.264 stream needs Level 4.0 or above (1080p60 needs Level 4.2), which a strict `42e01f` (Level 3.1) negotiation does not cover [F-40] (OQ-073).
  - *(Added 2026-10-08, research topic H; wording, acceptance and status unchanged.)* H.265 in WebRTC: RFC 7742 does not require it [H-32]. Chrome 136+ enables it only where the platform decodes it in hardware, with no software fallback [H-33]; Safari 18.0 supports the standard HEVC RTP payload [H-34]; no evidence of Firefox support was found [H-35]; Edge 147 is reported not to enable it by default (community source) [H-36]. GStreamer 1.26.2's `rtph265pay` lacks profile, tier and level in its caps (added in 1.26.4) [H-31]. Reasoning: an H.264 WebRTC track has to remain for browser reach (OQ-108, RISK-019). *(2026-10-08, later: deferred — REQ-ENC-002; not in current scope. WebRTC carries H.264 only (OQ-103 ANSWERED); OQ-108 stays OPEN, not in current scope.)*
  - *(Added 2026-10-08, research topic I.)* Opus encoders: FFmpeg `libopus` [I-41] and GStreamer `opusenc` [I-45]; both accept only 48, 24, 16, 12 or 8 kHz, so a 44.1 kHz HDMI source needs resampling first [I-41], [I-47] (OQ-111).
  - *(Added later on 2026-10-08, research topic K — the < 1 s target; wording, acceptance and status unchanged.)*
    - Protocols: WHIP is RFC 9725 and covers ingest only [K-01]; WHEP, the playback protocol MediaMTX serves, is still an Internet-Draft as of 2026-10-08 [K-02] (RISK-034). *(Corrected 2026-10-09: [K-02] covers only WHEP's draft status; that MediaMTX serves WHEP readers is reported by the MediaMTX project (community source) [F-45].)*
    - MediaMTX: no numeric WebRTC latency figure is documented [K-03]; browsers deliberately do not support H.264 B-frames over WebRTC, Baseline with Opus is recommended, and WebRTC audio is Opus, G722 or G711 only [K-04]. RTSP-client publishing is MediaMTX's recommended way for GStreamer to publish to it [K-05]. `webrtcsink` and `whipclientsink` (gst-plugins-rs) are not packaged in Debian trixie or the Raspberry Pi archive [K-06] (RISK-034). MediaMTX documents that it favours real-time delivery over reliability: most protocols run over UDP so late packets can be dropped, and a full outgoing circular buffer (`writeQueueSize`, default 512) drops packets [K-44].
    - Default buffering to set explicitly: `webrtcbin` 200 ms [K-07], `rtpbin`/`rtpjitterbuffer` 200 ms [K-08], `rtspclientsink`/`rtspsrc` 2000 ms [K-09]; reasoning: the jitter-buffer values apply only to RTP received inside the live path, since a browser viewer uses its own buffer [K-10] (RISK-032, OQ-126).
    - Capture: Unicam (CM4) and RP1 CFE (CM5) complete each buffer at frame end, so capture costs about one frame's readout [K-34], [K-35] *(Corrected 2026-10-09: [K-34] says at least one frame's readout time on CM4; [K-35] says about one on CM5.)*; `v4l2src` reports at least one frame of latency [K-37]; V4L2 queues are FIFOs, and reasoning: each waiting frame adds one frame period [K-36].
    - Budget: reasoning (a labelled budget, not a measurement; CORRECTED): for a CM4 1080p30 viewer on a LAN the documented terms are about 56 ms typical and 75 ms worst case, leaving about 925–945 ms for undocumented terms (source device, TC358743, conversion, MediaMTX relay, network, browser buffer, decode and render); the encode term assumes the live encode has the CM4 hardware encoder to itself, but the recording encode shares it (OQ-115), and the CM5 encode term is unknown until x264's per-frame time on BCM2712 is measured (OQ-059) [K-45]. *(Corrected 2026-10-09: [K-45] calls these terms "documented or extrapolated" — the encode term is scaled from a 720p community figure [K-33] — and "the recording encode shares it" should read "would share it": whether the CM4 hardware encoder sustains two sessions is OQ-115.)* The TC358743's internal buffering is undocumented [K-38]. A third-party CDN documents WHEP playback under 500 ms (reference only, not this stack) [K-14]. Viewers can measure their own buffering through the W3C statistics API (CORRECTED) [K-12] and hint a buffer target with `jitterBufferTarget` [K-11]. (RISK-031, OQ-125.)
    - Alternatives: reasoning (CORRECTED), LL-HLS through MediaMTX defaults is unlikely to reliably reach under 1 s and regular HLS cannot [K-26].
    - Internet viewers: MediaMTX listens on UDP :8189 with TCP off by default, because TCP adds progressive delay under congestion [K-41]; internet clients need the public address in `webrtcAdditionalHosts`, or STUN/TURN when the listeners cannot be reached [K-42]; four connection methods are documented [K-43] (RISK-033, OQ-128; reach is still UNDEFINED, OQ-008).

## REQ-ATEM-001 — Blackmagic ATEM integration

**Requirement.** The system shall integrate with Blackmagic Design ATEM switchers.

- **Source:** ENGINEERING_RULES.md Rule 2 (`ATEM.md`), Rule 11 (`REQ-ATEM-001` listed).
- **Scope (owner, 2026-10-07; OQ-009 ANSWERED):** "it can be atem and direct video from camera". Recorded interpretation: the integration is **option 1 below** — PACSCORDER captures the ATEM's HDMI output (REQ-CAP-008). Options 2 and 3 were offered and not selected, so they are **not in scope** unless the owner adds them. Supported ATEM models: OQ-102.
- **Acceptance:** DRAFT. The researched options were:
  1. capture the ATEM HDMI output through the TC358743 [F-23], [F-25];
  2. read tally and record/stream state, or control the switcher, over the UDP 9910 protocol, which the official SDK manual does not document [F-10] and which the OpenSwitcher project reports as reverse-engineered (community source) [F-11];
  3. exchange RTMP streams [F-26]; reasoning: to receive an ATEM's RTMP push, PACSCORDER must run a listening RTMP server [F-46].
- **Implementation:** NOT STARTED.
- **Notes:** The official ATEM SDK supports Windows and macOS only, not Linux [F-01], [F-02].

## REQ-BLD-001 — Reproducible product image build

**Requirement.** The product image shall be built from this repository by a documented, repeatable procedure that pins kernel, firmware and package versions.

- **Source:** ENGINEERING_RULES.md Rule 17 ("Build"), Rule 25 ("How is the system built?").
- **Acceptance:** DRAFT. The OS / build basis is decided: ADR-003 ACCEPTED on 2026-10-07 (OQ-012 ANSWERED) — own OS image built with `rpi-image-gen`, with a package mirror for reproducibility. Acceptance criteria for "reproducible" are not yet defined.
- **Implementation:** NOT STARTED.

## REQ-BLD-002 — Project-owned product OS image

**Requirement.** The product shall run a project-built OS image of its own, not an unmodified stock distribution image.

- **Source:** Owner statement, 2026-10-07: "which is best i need by own one" (answer to the OS/build question, OQ-012). Wording by Claude.
- **Acceptance:** DRAFT. The build tool is decided: ADR-003 ACCEPTED on 2026-10-07 (owner: "accept ADR-003") — the image is built with `rpi-image-gen` from Raspberry Pi OS packages; Buildroot remains the documented alternative.
- **Implementation:** NOT STARTED.

## REQ-PERF-001 — Sustained operation

**Requirement (PROPOSED).** The system shall capture, encode and record or stream continuously for a duration to be specified by the owner, without frame loss beyond a threshold also to be specified, across the product's operating temperature range.

- **Source:** Derived by Claude.
  - The TC358743 is rated −30 to +70 °C ambient [A-41].
  - Its FIFO trigger level (374) is hard-coded in the driver [B-12]; a Raspberry Pi engineer reported that the value is empirical (community source) [A-43]. The driver has D-PHY timing tables only for 594 and 972 Mbps [A-24], [B-09]. Their validity across modes and temperature is OQ-035.
  - Pi 5 has no hardware video encoder; the official figure for software H.264 1080p30 encode is ~30–40 % CPU [G-22] (OQ-059).
- **Acceptance:** PROPOSED. Duration, drop threshold and temperature range are **UNDEFINED** (OQ-010).
- **Implementation:** NOT STARTED.

---

## Verification status

### Verified from sources (fact IDs)

Every fact cited above is in [REFERENCES.md](REFERENCES.md) with verdict `CONFIRMED` or `CORRECTED`; `CORRECTED` entries (A-26, B-21, F-24) are used in their corrected wording. Community-tier entries (A-43, C-41, C-42, C-43, F-11) are worded as reports. Reasoning-tier entries (A-26, B-27, B-33, C-47, C-48, C-49, F-40, F-46) are labelled as reasoning. "Verified from sources" means only that the cited source says so; it says nothing about PACSCORDER hardware.

Added 2026-10-08 (research topics H and I, cited in REQ-CAP-006, REQ-ENC-001, REQ-REC-001, REQ-STR-001 and REQ-STR-002):

- Topic H facts cited: H-01, H-02, H-04, H-05, H-08, H-09, H-10, H-11, H-12, H-13, H-15, H-16, H-19, H-20, H-21, H-22, H-23, H-24, H-25, H-26, H-27, H-29, H-30, H-31, H-32, H-33, H-34, H-35, H-36, H-37, H-38, H-43.
- Topic I facts cited: I-01, I-02, I-03, I-04, I-05, I-06, I-07, I-08, I-09, I-10, I-13, I-14, I-15, I-18, I-19, I-20, I-21, I-22, I-24, I-25, I-26, I-27, I-29, I-30, I-31, I-33, I-35, I-36, I-37, I-38, I-39, I-40, I-41, I-42, I-43, I-44, I-45, I-46, I-47.
- `CORRECTED` entries H-10, H-12 and I-40 are used in their corrected wording. Community-tier entries H-19, H-20, H-21, H-22 and H-36 are worded as reports. Reasoning-tier entries H-23, H-43 and I-18 are labelled as reasoning. All cited H and I entries have verdict `CONFIRMED` or `CORRECTED`.

Added later on 2026-10-08 (research topics J and K, cited in REQ-ENC-001, REQ-REC-001, REQ-STR-001 and REQ-STR-002; [K-15] was already cited in REQ-STR-001):

- Topic J facts cited: J-01, J-03, J-04, J-05, J-06, J-07, J-08, J-09, J-11, J-13, J-14, J-15, J-17, J-18, J-19, J-24, J-25, J-27, J-28, J-29, J-30, J-31, J-32, J-33, J-35, J-36, J-37, J-38, J-39, J-40, J-41, J-44, J-45.
- Topic K facts cited: K-01, K-02, K-03, K-04, K-05, K-06, K-07, K-08, K-09, K-10, K-11, K-12, K-14, K-15, K-16, K-17, K-18, K-26, K-27, K-28, K-29, K-30, K-31, K-32, K-33, K-34, K-35, K-36, K-37, K-38, K-39, K-41, K-42, K-43, K-44, K-45.
- `CORRECTED` entries J-09, J-27, J-30, J-33, J-35, K-12, K-18, K-26, K-29 and K-45 are used in their corrected wording. Community-tier entries J-27 and K-33 are worded as reports. *(Corrected 2026-10-09: [F-45], cited in REQ-ENC-001 and in a 2026-10-09 correction under REQ-STR-002, is also a `community`-tier entry and is worded as a report; it was missing from the community lists above.)* Reasoning-tier entries J-09, J-31, J-36, J-37, K-10, K-18, K-26 and K-45 are labelled as reasoning, as is the reasoning sentence of the `kernel-source` entry K-36. [K-14] is a third-party CDN figure, cited as a reference only. All cited J and K entries have verdict `CONFIRMED` or `CORRECTED`. Statements marked *research gap* or *research design risk* come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json) and are not register facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No requirement has a test result; every test listed above is `BLOCKED — HARDWARE REQUIRED` except TEST-BLD-001, which is `NOT STARTED`.

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Document created. Rule-derived requirements entered as DRAFT; research-derived requirements entered as PROPOSED. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document review. No requirement wording, acceptance or status changed. Changes: header rows "Applies to" and "Verification" added; owner-to-specify items linked to their OQ IDs (OQ-001 to OQ-012, OQ-017, OQ-040); REQ-CAP-003 "EDID RAM is volatile [B-22]" replaced by what [B-21] and [A-31] support, and the vague "OQ list" replaced by OQ-002 / OQ-032; REQ-CAP-006 audio note corrected (the silicon can also send audio over CSI-2 [A-05]; I2S is the driver's choice [A-13]); REQ-STR-002 AAC wording aligned with [F-41] ("not a *required* WebRTC codec"); REQ-DMA-001 stale "still being verified" note removed; community-tier facts worded as reports; DT-versus-wiring lane note added to REQ-CAP-005; 3-of-4-lane caveat (OQ-038) added to REQ-CAP-001; REQ-PERF-001 FIFO/D-PHY sources corrected; "Verification status" section added. Cross-document consistency fixes (second pass, same date; no requirement wording, acceptance or status changed): REQ-CAP-001 4-lane note linked to ADR-008 / OQ-099; REQ-CAP-003 linked to OQ-093 (EDID provisioning trigger and ordering); REQ-PLT-001 [C-48] labelled as reasoning. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions recorded: OQ-001 answer added to REQ-CAP-001; REQ-ATEM-001 scope recorded (OQ-009); new DRAFT requirements REQ-CAP-007 (2-lane and 4-lane, all frame rates each link carries), REQ-CAP-008 (ATEM and camera HDMI sources) and REQ-BLD-002 (own OS image). DRAFT definition extended to dated owner statements. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated (verification pass): REQ-PLT-001 note no longer says Pi 4 Model B simply "conflicts with REQ-CAP-001" — it cannot serve the 4-lane configuration and remains a 2-lane candidate (REQ-CAP-007); REQ-CAP-007 citations corrected — the 3-of-4-lane statement now cites [C-47] (not only [C-49]), the "2-lane bridge board on any port" reasoning cites [C-48], and the `4lane` probe note is scoped to the Unicam 2-lane ports with [B-42] for what `4lane` sets; REQ-CAP-008 [B-27] labelled as reasoning; Verification status reasoning-tier list adds B-33 and C-49. No requirement status, acceptance or ID changed. | Claude (session 2026-10-07) |
| 2026-10-07 | REQ-BLD-001 and REQ-BLD-002 notes updated: ADR-003 ACCEPTED by the owner (OQ-012 ANSWERED). No requirement wording or acceptance status changed. | Claude (session 2026-10-07) |
| 2026-10-07 | Owner decisions recorded: REQ-CAP-006 audio required (PROPOSED → DRAFT; original wording kept in the entry); REQ-ENC-001 codecs H.264 and H.265 (OQ-103, RISK-022); REQ-CAP-008 sources generic (OQ-102 answered). Counts now 16 DRAFT / 4 PROPOSED. | Claude (session 2026-10-07) |
| 2026-10-08 | Evidence from research topics H (H.265/HEVC) and I (HDMI audio) added as dated notes: REQ-CAP-006 (overlay mechanics, CM4 and CM5 I2S paths, stereo only, sample-rate handling, A/V clock domains, VDDIO2 and GPIO 18–21, audio encoders; acceptance line annotated with OQ-110, OQ-111, OQ-112, OQ-114), REQ-ENC-001 (x265/libx265/x265enc availability, planar-only input, community cost evidence, DotProd only on CM5, low-latency settings), REQ-REC-001 (HEVC in MP4/Matroska, AAC encoders), REQ-STR-001 (HEVC over Enhanced RTMP with FFmpeg, not with GStreamer 1.26.2 `flvmux`; YouTube H.265; SRT components; AAC encoders) and REQ-STR-002 (H.265 browser support, Opus encoders and rates). Header Verification row and Verification status fact lists updated. No requirement wording, acceptance, status or ID changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Owner answer to OQ-103 ("H.264 only for now"): REQ-ENC-001 codec is H.264 only; new REQ-ENC-002 (H.265, DEFERRED); new acceptance value DEFERRED defined. 21 requirements: 16 DRAFT, 4 PROPOSED, 1 DEFERRED. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): verifier pass — dated H.265 notes outside REQ-ENC-001 annotated: REQ-REC-001 note "H.265 is required, REQ-ENC-001" marked superseded (recordings H.264 only); REQ-STR-001 HEVC-over-RTMP note and REQ-STR-002 H.265-in-WebRTC note labelled deferred — REQ-ENC-002; not in current scope. No requirement wording, acceptance, status, citation or ID changed; counts unchanged (21: 16 DRAFT, 4 PROPOSED, 1 DEFERRED). | Claude (session 2026-10-08) |
| 2026-10-08 | REQ-ENC-001: owner decision on OQ-005 recorded (two H.264 encodes: recording + live shared by RTMP and WebRTC) with the WebRTC-compatibility constraints on the live encode (reasoning from [F-36], [F-39], [F-40], [F-45], [D-11], [D-14], [D-15]). No acceptance or status change. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — REQ-ENC-001 acceptance list: "how many simultaneous encodes are needed" annotated as answered by the owner (two; see **Encodes**); acceptance stays DRAFT, bitrate and latency still open (OQ-005). No requirement wording, status or ID changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Owner decisions recorded as notes: REQ-REC-001 (MP4; PCIe NVMe + USB-to-SATA HDD) and REQ-STR-002 (< 1 s live latency). No acceptance or status change. | Claude (session 2026-10-08) |
| 2026-10-08 | Notes: REQ-STR-002 latency scope (WebRTC viewers only), REQ-STR-001 RTMP best-effort, REQ-REC-001 mirror + fragmented MP4 + self-powered HDD (ADR-009). No acceptance/status change. | Claude (session 2026-10-08) |
| 2026-10-08 | Storage + latency (topics J/K; ADR-009; OQ-116): dated evidence notes added, no requirement wording, acceptance, status or ID changed (counts unchanged: 21 — 16 DRAFT, 4 PROPOSED, 1 DEFERRED). REQ-REC-001 (topic J): CM4 one PCIe Gen 2 x1 lane and USB 2.0 only, IO Board PCIe socket on 12 V, HDD on the shared USB2514B hub [J-01], [J-03] to [J-06], [J-08], [J-09], [J-11]; CM5 IO Board M.2 (Gen 2 x1, 2230–2280, disabled by default in the device tree) and USB 3.0 with about 1.2 A shared VBUS [J-13] to [J-15], [J-17] to [J-19]; HDD power [J-28], [J-30] to [J-32]; UAS bridge behaviour and quirks [J-07], [J-24], [J-25], [J-27] (community); filesystems [J-29], [J-33], [J-35]; MP4 power-loss behaviour, fragmentation and robust muxing [J-38] to [J-41], [J-44], [J-45]; data-rate reasoning [J-36], [J-37]; power-loss line annotated as decided by ADR-009 (amount lost: OQ-119). REQ-ENC-001 (topic K): CM4 `bcm2835-codec` B-frames limited to 0, default GOP 60, force-keyframe, 0 reported latency, community encode-latency report [K-30] to [K-33]; GOP reasoning against YouTube's 2 s recommendation [K-17], [K-30]; x264 `zerolatency` and `x264enc` defaults [K-27] to [K-29]; Pi 5 software-encoder latency [K-39]; shared live-encode B-frame and keyframe constraints [K-04], [K-17] (OQ-127, RISK-019). REQ-STR-001 (topic K): YouTube latency modes, API values and encoder recommendations [K-15] to [K-17]; reasoning that YouTube cannot meet < 1 s [K-18]; other RTMP platforms a research gap. REQ-STR-002 (topic K): WHIP RFC 9725 and WHEP draft [K-01], [K-02]; MediaMTX facts [K-03] to [K-05], [K-41] to [K-44]; gst-plugins-rs not packaged [K-06]; default buffering [K-07] to [K-10]; capture latency [K-34] to [K-38]; labelled latency budget [K-45] with the shared-encoder caveat (OQ-115, OQ-059); viewer-side statistics [K-11], [K-12]; CDN reference [K-14]; LL-HLS reasoning [K-26] (RISK-031 to RISK-034; OQ-125, OQ-126, OQ-128). Header Verification row and Verification status (J and K fact lists; CORRECTED, community and reasoning entries) updated. Wording: "B-frames are fixed at 0" reworded to "B-frames are limited to 0" (Rule 10); the [J-30] HDD note names the register's example drive; the [J-33] and [K-44] notes follow the register wording. | Claude (session 2026-10-08) |
| 2026-10-09 | Documentation catch-up for commit `54269bf`: acceptance lists annotated where the owner decisions of 2026-10-08 already settle an item — REQ-ENC-001 and REQ-STR-002 "latency target" (OQ-116 ANSWERED), REQ-REC-001 "container format", "storage medium" and "behaviour on power loss" (ADR-009 ACCEPTED; OQ-119 remains). REQ-STR-002 topic K note: capture-term wording corrected ([K-34] says *at least* one frame's readout on CM4). No requirement wording, acceptance, status or ID changed (21 — 16 DRAFT, 4 PROPOSED, 1 DEFERRED). REQ-ENC-001 **Encodes** note: "latency still open" marked superseded in part (OQ-116). REQ-STR-001 latency note: "under 5 s at best" corrected to what [K-15] and [K-18] support. Verifier pass (same date): dated correction notes added, original text kept — REQ-REC-001: bare `dtparam=pciex1` line NEEDS VERIFICATION ([J-15] says set `pciex1` to "on"; OQ-100, OQ-124); the about 1.2 A shared VBUS is the CM5 IO Board's ([J-19]), not the module's; [J-32] is one Seagate 3.5-inch family and "any 3.5-inch HDD needs external 12 V" is its reasoning. REQ-STR-001: [K-18] says the YouTube output cannot be planned or claimed to meet < 1 s, not that it never can. REQ-STR-002: acceptance "latency target" settled only in part (how it is judged is open, OQ-008); WHEP being served by MediaMTX is the MediaMTX project's report [F-45], not [K-02]; [K-45] terms are "documented or extrapolated" and "shares it" → would share it (OQ-115). Verification status: [F-45] is community tier. No requirement wording, acceptance, status or ID changed (21 — 16 DRAFT, 4 PROPOSED, 1 DEFERRED). | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09: REQ-REC-001 acceptance "minimum recording duration" superseded (until stopped or disk full; OQ-006 ANSWERED; OQ-129); REQ-STR-002 acceptance "LAN only or internet" superseded (LAN only) and a latency-criterion line added (95th percentile). No requirement wording, acceptance status or ID changed. REQ-REC-001 owner-decision note of 2026-10-08 ("Duration is still UNDEFINED") marked superseded. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 (later): REQ-ENC-001 acceptance "bitrate" superseded (live CBR 17 Mbit/s, recording 25 Mbit/s VBR; OQ-005, OQ-073); REQ-REC-001 note on the recording bitrate and the drive-failure policy (OQ-129). No requirement wording, acceptance status or ID changed. | Claude (session 2026-10-09) |
