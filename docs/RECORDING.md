# PACSCORDER Recording

| | |
|---|---|
| Document status | Active — source research only. Recording design and implementation: NOT STARTED |
| Last updated | 2026-10-07 |
| Applies to | REQ-REC-001; all four candidate platforms (Pi 4 Model B, CM4, Pi 5, CM5); the project's own product OS image (REQ-BLD-002, DRAFT), built per ADR-003 (ACCEPTED: Raspberry Pi OS with `rpi-image-gen`; Buildroot as the documented alternative) |
| Verification | Source research of 2026-10-06 only ([REFERENCES.md](REFERENCES.md)). Nothing has been tested. No PACSCORDER hardware or code exists as of 2026-10-06. |

This document answers the Rule 25 question "How is recording performed?". As of 2026-10-06 the honest answer is: **recording is not designed yet.** REQ-REC-001 is `DRAFT`, and the parameters that would drive a design (container, storage medium, duration, power-loss behaviour) are undefined.

What this document does record:

- what is undefined and who must decide it;
- what the sources say about the building blocks (muxers, encoder behaviour, storage);
- which design questions are open.

Fact IDs such as `[G-26]` point to [REFERENCES.md](REFERENCES.md). `OQ-NNN` points to [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md). Facts of tier `community` are worded as reports. Calculations are marked as reasoning and list their inputs.

## 1. Status at a glance

| Item | Status |
|---|---|
| REQ-REC-001 Recording | Acceptance `DRAFT`. Implementation `NOT STARTED`. |
| TEST-REC-001 Recording integrity and duration | `BLOCKED — HARDWARE REQUIRED` |
| Container format | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). |
| Storage medium | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| Minimum continuous recording duration | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). |
| Behaviour on power loss | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). Not researched; no source fact exists. |
| Audio in recordings | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-004). |
| Codec, bitrate, shared or separate encode | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-005). |
| Userspace framework (GStreamer, FFmpeg, direct V4L2) | ADR-007 `OPEN` (OQ-015). |
| Partition layout and update scheme | OQ-069; depends on ADR-003 (`ACCEPTED`). |
| Software updates or reboots while a recording is active | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-094). |

## 2. Where recording sits in the pipeline

```text
HDMI → TC358743 → CSI-2 → CSI-2 receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder (this document)
                                                                                     ├→ RTMP    (STREAMING.md)
                                                                                     └→ WebRTC  (STREAMING.md)
```

The recorder receives the encoder's output. The encoder differs per platform, which affects what can be recorded:

| Platform | Encoder (facts) | Consequence for recording (reasoning) |
|---|---|---|
| Pi 4 Model B, CM4 | Hardware H.264 encoder, officially specified for 1080p30 encode [D-10] | 1080p60 recording depends on unproven 1080p60 hardware encode (RISK-002, OQ-056). |
| Pi 5, CM5 | No hardware video encoder [D-31]; software encoders; official figure "H264 1080p30 encode (from ISP) ~30–40% CPU" [G-22] | If recording has its own encode, it competes for CPU with the RTMP and WebRTC encodes (RISK-003, OQ-005, OQ-059). |

No candidate platform has a hardware HEVC encoder [D-24], [D-31].

Whether the recorder shares one encoded stream with RTMP and WebRTC, or has its own encode, is part of OQ-005. Reasoning: browser WebRTC interoperability requires H.264 Constrained Baseline [F-36], and the MediaMTX project reports that browsers do not accept H.264 B-frames [F-45]. A single shared encode would therefore carry those restrictions into the recording as well. See [STREAMING.md](STREAMING.md) and [VIDEO_ENCODER.md](VIDEO_ENCODER.md).

## 3. Container muxers available in Raspberry Pi OS and the Buildroot alternative

| Container | Raspberry Pi OS (trixie) | Buildroot 2026.08 | Notes |
|---|---|---|---|
| MP4 (ISO BMFF) | `libgstisomp4` ships in `gstreamer1.0-plugins-good` 1.26.2 [G-26] | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_ISOMP4` (MP4 muxing) [E-34] | ATEM switchers record H.264 + AAC in MP4 [F-07], [F-26]. |
| Matroska | `libgstmatroska` ships in `gstreamer1.0-plugins-good` [G-26] | NEEDS VERIFICATION — no register fact | — |
| FLV | `libgstflv` ships in `gstreamer1.0-plugins-good` [G-26] | `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_FLV` [E-34], [F-34] | FLV is the RTMP container. In legacy FLV, H.264 (AVC) is the only modern video codec, and AAC is audio SoundFormat 10 [F-31]. `flvmux` caps are in [F-34]. |
| MPEG-TS | NEEDS VERIFICATION — no register fact | NEEDS VERIFICATION — no register fact | Named as a candidate in OQ-006 only. |
| FFmpeg muxers | FFmpeg 7.1.5 (Raspberry Pi build `+rpt2`) is in the archive [G-29]. Its muxer list is not in the register: NEEDS VERIFICATION. | FFmpeg 6.1.5 [E-36], [G-64]. Muxers: NEEDS VERIFICATION. | — |

Further constraints:

- **GStreamer must be installed.** The Raspberry Pi OS Lite image of 2026-10-06 has no GStreamer packages installed [G-32].
- **Versions differ between the OS options.** Raspberry Pi OS trixie ships GStreamer 1.26.2 [G-26], [G-31]. Buildroot 2026.08 ships 1.24.13 [E-31], [G-64]. Differences are tracked in OQ-066.
- **Element names and properties are not sourced.** The register names the isomp4 and matroska *plugins* only. Their element names, and any properties for fragmenting, segmenting or finalising files, are NEEDS VERIFICATION before a design is written.

## 4. Encoder facts that affect recording

| Fact | Effect on recording (reasoning unless cited) |
|---|---|
| The Pi 4/CM4 hardware encoder produces no B-frames. GOP size defaults to 60. Every I-frame is an IDR frame [D-14]. | Random-access points in the recording are the IDR frames. Reasoning with inputs GOP = 60 [D-14] and frame rates 30 or 60: one IDR every 2 s at 30 fps, every 1 s at 60 fps. The GOP length bounds how finely a recording can be cut or split into segments without re-encoding. GOP size is a control (`V4L2_CID_MPEG_VIDEO_GOP_SIZE`) [D-14]. |
| On the Pi 4/CM4 encoder, `V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER` (SPS/PPS inline with every IDR) defaults to off [D-15]. Raspberry Pi's official GStreamer streaming pipeline sets `repeat_sequence_header=1` [D-37]. | If a recording is split into segments that must each decode on their own, each segment needs SPS/PPS. Whether the chosen muxer stores the parameter sets itself is NEEDS VERIFICATION. |
| The Pi 4/CM4 encoder queues copy input timestamps to encoded buffers (`V4L2_BUF_FLAG_TIMESTAMP_COPY`) [D-19]. | Capture timestamps can flow through the hardware encoder into the container. |
| The TC358743 driver derives the reported timings from an integer frame rate, so fractional rates such as 59.94 Hz are reported as integer-fps pixel clocks [B-28]. The ATEM Mini Pro outputs both 1080p59.94 and 1080p60 [F-23]. | Reasoning: container frame-rate metadata and timestamps cannot rely on the reported DV timings alone, because 59.94 and 60 Hz sources are not distinguished there. How the true rate is found is OQ-040. |
| UYVY output is BT.601 limited range, reported as `SMPTE170M` [B-34]. ATEM output is Rec 709 [F-23]. | The colour metadata written into the file is undecided (OQ-041). |
| Pi 4/CM4 encoder bitrate range is 25 kbit/s to 25 Mbit/s, default 10 Mbit/s, VBR (default) or CBR [D-13]. | Input to storage sizing (Section 5.3). |

**Pi 5 / CM5.** The encoder rows above ([D-13], [D-14], [D-15], [D-19]) describe the Pi 4/CM4 hardware encoder only. For the Pi 5/CM5 software encoders, the register states only that `rpicam-apps` uses `max_b_frames=1` for `libx264` in normal mode [D-35]. Their GOP length, IDR behaviour, SPS/PPS repetition and timestamp handling are NEEDS VERIFICATION. See [VIDEO_ENCODER.md](VIDEO_ENCODER.md).

## 5. Storage

### 5.1 Storage per platform (what the source register says)

| Platform | Storage facts in the register | PACSCORDER recording storage |
|---|---|---|
| Pi 4 Model B | For A/B boot updates, Raspberry Pi Connect Remote Update needs a Raspberry Pi 4 or later with a storage device (typically microSD) of at least 16 GB [G-43] (CORRECTED). This is an update-mechanism requirement, not a recording requirement. Storage and provisioning differ between boards (SD vs eMMC with `rpiboot`) [G-71] (reasoning, CORRECTED). | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| CM4 | Compute Module eMMC is flashed by fitting nRPI_BOOT (J2) and running `rpiboot` [G-47]. | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| Pi 5 | Same Connect statement as Pi 4 [G-43]. | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |
| CM5 | eMMC flashing as for CM4 [G-47]. The Raspberry Pi OS image includes CM5 Lite device trees [G-71]. | UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006, OQ-018). |

**Not in the source register** (each is UNKNOWN — VERIFICATION REQUIRED):

- USB mass storage and NVMe/PCIe as recording media. DATASHEET REQUIRED (OQ-098).
- eMMC capacities of the Compute Module variants, and the storage of Lite (no-eMMC) variants. DATASHEET REQUIRED (OQ-098).
- Sustained write throughput of any medium. HARDWARE TEST REQUIRED, as part of TEST-REC-001 and TEST-PERF-001.

### 5.2 Filesystem and partition constraints

- **Read-only root.** On Raspberry Pi OS, raspi-config's overlay file system option prepends `overlayroot=tmpfs` to `cmdline.txt` [G-48]. Reasoning: if that overlay is enabled, recordings written under it would be lost at reboot and would consume RAM, so recordings would need a separate persistent partition.
- **rpi-image-gen A/B layout.** The `image-rota` layout provides immutable A/B system slots and a single shared persistent data partition [G-40], [G-41]. This is a candidate location for recordings, subject to OQ-069. Whether an update or a reboot into a new slot may happen while a recording is active, and what happens to that recording, is OQ-094 (OWNER DECISION REQUIRED).
- **Buildroot Raspberry Pi sample images.** Buildroot 2026.08 `board/raspberrypi/genimage.cfg.in` defines a 32 MB boot partition, and the defconfigs set a 120 MB ext4 root filesystem [E-19]. Reasoning: there is no recording space in that layout, so a project partition layout would be needed.
- **OS footprint.** The Raspberry Pi OS Lite image of 2026-10-06 expands to 3,078,619,136 bytes [G-33]. Reasoning: on a stock-image installation, that much of the medium is used before any recording space is allocated. The stock image is for bring-up only; the product runs its own project-built OS image (REQ-BLD-002, owner 2026-10-07). An `rpi-image-gen` image may differ (OQ-068).
- **Recording filesystem.** UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-006). For reference only: ATEM Mini Pro records to ExFAT or HFS+ media [F-26].

### 5.3 Storage sizing (reasoning)

Formula: bytes per hour = video bitrate (bit/s) × 3600 / 8. GB here means 10^9 bytes. Audio and container overhead are excluded. In VBR mode the actual bitrate varies around the target, so measured sizes will differ (reasoning).

| Video bitrate (input) | Bytes per hour, video only |
|---|---|
| 10 Mbit/s — Pi 4/CM4 encoder default [D-13] | 4.5 GB |
| 25 Mbit/s — Pi 4/CM4 encoder maximum [D-13] | 11.25 GB |

The product bitrate is undecided (OQ-005), so these are examples, not requirements.

## 6. Power-loss behaviour — OPEN

REQ-REC-001 leaves power-loss behaviour undefined. **It was not researched:** no fact in [REFERENCES.md](REFERENCES.md) describes how any container, muxer or filesystem behaves when power is lost during recording.

**Owner decisions required (OQ-006):**

- How much recorded material may be lost when power is cut?
- Must the file being written remain playable?
- Is a hardware hold-up (time to close files after input power fails) acceptable? This depends on the product power design, which is UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-023).

**Candidate approaches to evaluate.** These are Claude's reasoning, not sourced facts and not decisions. Each one is NEEDS VERIFICATION through TEST-REC-001 with deliberate power cuts.

1. **Segmented recording.** Write the recording as a sequence of shorter files that are closed regularly, so that a power cut affects at most the file that is open. Segment boundaries would fall on IDR frames (Section 4, [D-14]).
2. **Progressive index.** Use a container or muxer mode that writes index data during recording rather than only at the end. Which of the available muxers ([G-26], [E-34]) offer such a mode is NEEDS VERIFICATION.
3. **Power-fail detection.** Detect input power loss in hardware and close files during a hold-up interval. This requires hardware support that is UNKNOWN — VERIFICATION REQUIRED. OWNER DECISION REQUIRED (OQ-018, OQ-023).
4. **Filesystem choice and mount options** for the recording partition. Behaviour under power loss is NEEDS VERIFICATION.

After OQ-006 is answered, the chosen design is recorded in a new ADR in [DECISIONS.md](DECISIONS.md). No such ADR exists yet.

## 7. Audio in recordings

Whether recordings include audio is undecided (OQ-004). If audio is required:

- The TC358743 datasheet allows audio to be sent over CSI-2 [A-05] or on the multiplexed I2S/TDM pins [A-11]. The Linux driver always configures 2-channel I2S output [A-13], so with that driver the audio leaves on I2S, not on CSI-2.
- The `tc358743-audio` overlay routes that I2S to Pi GPIO 18/19/20 [A-47]. Wiring on PACSCORDER hardware is UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED (OQ-025). Pi 5/CM5 support is UNKNOWN — VERIFICATION REQUIRED. KERNEL SOURCE INSPECTION REQUIRED (OQ-054). See RISK-014.
- The audio encoder choice and its CPU cost are open (OQ-063). For reference, ATEM recordings use AAC in MP4 [F-07].

## 8. Reference: how an ATEM records

ATEM switchers record on their own. This is listed as a reference for PACSCORDER's recorder design. Controlling or reading ATEM recording over the network is not in current scope: OQ-009 was answered on 2026-10-07 with HDMI capture of the ATEM output only (REQ-ATEM-001, REQ-CAP-008).

- The ATEM SDK Recording API records H.264 video and AAC audio as MP4 to an externally connected disk [F-07].
  - Its states are Idle, Recording and Stopping.
  - Its errors include `NoMedia`, `MediaFull` and `DroppingFrames` [F-07].
- ATEM Mini Pro records to USB-C media as `.mp4` H.264 with AAC audio, on ExFAT or HFS+ [F-26].
- The atem-connection project reports recording commands `RcTM` (set) and `RTMS` (status) over the network protocol [F-16] (reference only; network control is not in current scope). See [ATEM.md](ATEM.md).

Reasoning: the ATEM's error states (no media, media full, dropping frames) are a useful checklist for the status that PACSCORDER's own recorder must report. That is a design input, not a requirement.

## 9. Open design decisions

| Decision | Options | Status | Resolved by |
|---|---|---|---|
| Container | MP4, Matroska, MPEG-TS, other | `OPEN` | OQ-006 (OWNER DECISION REQUIRED) |
| Storage medium and interface | SD, eMMC, USB, NVMe, network | `OPEN` | OQ-006, OQ-018; board storage facts OQ-098 |
| Shared or separate encode for recording | One encode for all outputs, or a separate recording encode | `OPEN` | OQ-005, OQ-059 (Pi 5/CM5 CPU budget) |
| Userspace framework | GStreamer, FFmpeg, direct V4L2 application | `OPEN` | ADR-007, OQ-015 |
| Power-loss strategy | Section 6 candidates | `OPEN` | OQ-006, then TEST-REC-001 |
| Recordings on a persistent partition, separate from the root filesystem | — | `PROPOSED` as a recommendation only (Claude's reasoning from [G-48], [G-40]; not accepted; no recording ADR exists yet, so it is not a decision record). Depends on ADR-003 (`ACCEPTED`). | OQ-006 (storage and recording requirements), OQ-069 (partition and update layout), OQ-094 (updates versus active recordings), then owner acceptance |

## 10. Test

TEST-REC-001 "Recording integrity and duration" (canonical ID, [README.md](README.md)) is `BLOCKED — HARDWARE REQUIRED`. Its acceptance criteria cannot be written until OQ-006 is answered. Once they exist, the test must at least cover:

- the minimum duration;
- file integrity and playability;
- power-cut behaviour;
- sustained write throughput.

The procedure will be written in [TESTING.md](TESTING.md). No recording command is given here, because no recording command has been run on PACSCORDER hardware.

## Verification status

### Verified from sources (fact IDs)

This document cites 40 register entries, all with verdict `CONFIRMED` or `CORRECTED`:

A-05, A-11, A-13, A-47, B-28, B-34, D-10, D-13, D-14, D-15, D-19, D-24, D-31, D-35, D-37, E-19, E-31, E-34, E-36, F-07, F-16, F-23, F-26, F-31, F-34, F-36, F-45, G-22, G-26, G-29, G-31, G-32, G-33, G-40, G-41, G-43, G-47, G-48, G-64, G-71.

- `CORRECTED` entries, used in their corrected wording only: F-34, G-43, G-71.
- `community` entries, worded as reports: F-16, F-45.
- `reasoning` entries, labelled as reasoning: G-71.
- "Verified from sources" means only that the cited source says so. Under Rule 23 a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Adversarial review against the source register. Corrected the TC358743 audio statement: the datasheet also allows audio over CSI-2 [A-05], and the driver selects I2S [A-13]. Reworded the fractional-frame-rate row to match [B-28]. Labelled the platform consequences and the read-only-root conclusion as reasoning. Added a Pi 5/CM5 note to the encoder facts [D-35]. Stated the Buildroot partition source [E-19] precisely. Marked the Connect storage figure as an update requirement [G-43]. Added the unit and VBR caveat to the sizing calculation. Normalised the UNKNOWN markers to the README convention. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: the persistent-partition recommendation (Section 9) now references OQ-006, OQ-069 and OQ-094 and stays labelled as a recommendation (no recording ADR exists); updates versus active recordings (OQ-094) added to the status table and Section 5.2; storage-media gaps linked to OQ-098 | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: REQ-BLD-002 (own product OS image) in the header "Applies to" row and the §5 OS-footprint note (stock image is bring-up only); §8 no longer lists the ATEM recording reference as input to an open "ATEM integration scope (OQ-009)" — OQ-009 is answered (HDMI capture only; REQ-ATEM-001, REQ-CAP-008), and the network recording commands are marked reference only. No citation added or removed. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); header "Applies to" row, §1 "Partition layout and update scheme" row and §9 persistent-partition row: ADR-003 (`PROPOSED`) → (`ACCEPTED`); §3 heading "…in the candidate OS builds" → "…in Raspberry Pi OS and the Buildroot alternative". The persistent-partition recommendation itself stays `PROPOSED`; OQ-069 stays open. No evidence, other ADR status or implementation status changed. | Claude (session 2026-10-07) |
