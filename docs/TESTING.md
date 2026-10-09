# PACSCORDER Test Procedures and Results

| | |
|---|---|
| Document status | Active — 17 procedures defined; **no test has been run** |
| Last updated | 2026-10-09 |
| Applies to | PACSCORDER on every candidate platform (Pi 4 Model B, CM4, Pi 5, CM5), in both the 2-lane and the 4-lane CSI-2 configuration (REQ-CAP-007); TC358743 bridge; HDMI sources: ATEM switcher outputs and cameras connected directly (REQ-CAP-008); Raspberry Pi OS Lite 64-bit for bring-up and the project's own product image (REQ-BLD-002), as decided in ADR-003 (ACCEPTED 2026-10-07). *(Added 2026-10-08.)* Bring-up evaluates **CM4 and CM5 side by side** (owner, 2026-10-07; ADR-004 stays OPEN until measured); HDMI audio is required (REQ-CAP-006, DRAFT); video codecs are H.264 **and** H.265 (REQ-ENC-001; which outputs use H.265 is OQ-103) *(superseded later on 2026-10-08: owner answer to OQ-103, "H.264 only for now" — the video codec is **H.264 only** for recording, RTMP and WebRTC (REQ-ENC-001); H.265 is deferred as REQ-ENC-002 (DEFERRED), and the H.265 runs below are kept but not run in current scope)*; sources are any HDMI camera, with no model list, plus ATEM outputs (OQ-102 ANSWERED). *(Added later on 2026-10-08: owner answer to OQ-005, "Separate record + live" — **two simultaneous H.264 encodes**, a recording encode and one live encode shared by RTMP and WebRTC (REQ-ENC-001). Bitrate, rate control and latency are still open (OQ-005). Whether the CM4 hardware encoder runs both is OQ-115; on CM5 both run in software (OQ-059).)* *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* Owner decisions of 2026-10-08: every recording is written as **fragmented MP4** and **mirrored** to a PCIe NVMe SSD and a USB-to-SATA HDD in a **self-powered** enclosure (ADR-009, ACCEPTED; its decision 3 also allows a self-powered hub; ext4 is Claude's proposal inside ADR-009, not an owner decision, OQ-120); live latency is **under 1 s camera-to-viewer for WebRTC viewers only**, and RTMP outputs are best-effort, their latency set by the receiving platform (OQ-116 ANSWERED; REQ-STR-002, REQ-STR-001). Bitrate and rate control are still open (OQ-005); the maximum recording duration is still open (OQ-006). |
| Verification | Procedures derived from the source research of 2026-10-06 ([REFERENCES.md](REFERENCES.md)), and extended on 2026-10-08 with the source research of topic H (H.265 software encoding and transport) and topic I (HDMI audio path), and on 2026-10-09 with the source research of 2026-10-08 on topic J (recording storage and power loss) and topic K (live latency). Nothing has been run on PACSCORDER hardware: no hardware exists as of 2026-10-06. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 9 (test documentation), Rule 10 (status words), Rule 12 (traceability), Rule 21 (no retroactive edits), Rule 22 (unknowns) |

This document holds one procedure for each canonical test ID in [README.md](README.md), in the same order, written in the Rule 9 format. The requirement-to-test mapping is in [TRACEABILITY.md](TRACEABILITY.md). Failure signatures that a test may hit are in [TROUBLESHOOTING.md](TROUBLESHOOTING.md).

> **State on 2026-10-06:**
> - No PACSCORDER hardware exists.
> - No PACSCORDER code exists.
> - No test below has been run.
>
> Every hardware test is `BLOCKED — HARDWARE REQUIRED`. TEST-BLD-001 is `NOT STARTED` because no build configuration exists.

---

## Test status summary

| Test ID | Title | Verifies | Status |
|---|---|---|---|
| TEST-HW-001 | TC358743 I2C detection | REQ-DRV-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-DRV-001 | TC358743 driver probe and chip ID | REQ-DRV-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-DRV-002 | EDID load and HDMI hot-plug assertion | REQ-CAP-003 | BLOCKED — HARDWARE REQUIRED |
| TEST-PLT-001 | Overlay load and media graph per platform | REQ-PLT-001, REQ-ARCH-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-001 | DV timings detection for each source mode | REQ-CAP-002, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-002 | 1080p60 capture: frame rate and frame integrity | REQ-CAP-001, REQ-CAP-007 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-003 | Source connect / disconnect / mode change | REQ-CAP-004 | BLOCKED — HARDWARE REQUIRED |
| TEST-CAP-004 | Unsupported-mode rejection and supported-mode matrix | REQ-CAP-005, REQ-CAP-007, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-AUD-001 | HDMI audio capture over I2S | REQ-CAP-006 | BLOCKED — HARDWARE REQUIRED |
| TEST-DMA-001 | DMABUF capture → encoder buffer sharing | REQ-DMA-001, REQ-ARCH-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-ENC-001 | Sustained real-time H.264 encode (H.265 deferred) | REQ-ENC-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-REC-001 | Recording integrity and duration | REQ-REC-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-STR-001 | RTMP publish and playback | REQ-STR-001 | BLOCKED — HARDWARE REQUIRED |
| TEST-STR-002 | WebRTC browser playback | REQ-STR-002 | BLOCKED — HARDWARE REQUIRED |
| TEST-ATEM-001 | ATEM HDMI output capture (scope per OQ-009) | REQ-ATEM-001, REQ-CAP-008 | BLOCKED — HARDWARE REQUIRED |
| TEST-BLD-001 | Image build from clean checkout | REQ-BLD-001, REQ-BLD-002 | NOT STARTED |
| TEST-PERF-001 | Soak: thermal, CPU, CMA, frame drops | REQ-PERF-001 | BLOCKED — HARDWARE REQUIRED |

---

## How to read and run these procedures

### Conventions

- **Commands.** Every command comes from a cited source and is marked **NOT YET RUN ON PACSCORDER HARDWARE**.
  - Where a source attests the action but not the exact command-line option, the step says `NEEDS VERIFICATION` instead of guessing syntax. These steps are tracked together in OQ-101.
  - **`v4l2-ctl -d /dev/v4l-subdevN`**, that is, a sub-device path given to `-d`, is NEEDS VERIFICATION (OQ-101). The register attests `-d 11` [D-17] and running the EDID and timing steps on `/dev/v4l-subdevN` [C-33], but not that option form. The `--set-edid pad=<pad>[,…]` and `--clear-edid <pad>` forms are attested as `v4l2-ctl` help text [B-24]; the argument form `--clear-edid 0` is NEEDS VERIFICATION (OQ-101).
  - Generic read-only actions, such as reading the kernel log or listing device nodes, are written as actions ("inspect the kernel log for …"). No register fact attests a specific command for them. The command actually used must be written into the result log entry.
  - *(Added 2026-10-08.)* **Audio and H.265 commands.** The register attests the ALSA device string `hw:CARD=tc358743,DEV=0` (and `plughw:` / `sysdefault:CARD=tc358743`) [I-15], and one `arecord` capture command from a 2019 forum thread (community source) [I-16]. It also attests element and property names (`x265enc` [H-11], [H-14]; `opusenc`, `avenc_aac`, `voaacenc` [I-47]) and the FFmpeg option names `rtmp_enhanced_codecs` [H-26] and `-ts abs` / `mono2abs` [I-38]. It attests no complete GStreamer or FFmpeg pipeline for H.265 or audio, no `arecord` listing option and no `v4l2-ctl` option for reading a control or waiting for a control event. Those steps are NEEDS VERIFICATION and are tracked in OQ-101.
  - *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* **Storage and latency commands.** The register attests: the kernel parameter `usb-storage.quirks=VID:PID:Flags`, where flag `u` is IGNORE_UAS [J-26], and its use in `cmdline.txt` as a UAS workaround as reported by a Raspberry Pi engineer (community source, CORRECTED) [J-27]; the BCM2712 dtparams `pciex1` (alias `nvme`) and `pciex1_gen`, and the line `dtparam=pciex1_gen=3` [J-15]; the fstab options `defaults,auto,users,rw,nofail` and `x-systemd.device-timeout=30` (CORRECTED) [J-33]; the GStreamer properties `fragment-duration` and `fragment-mode` [J-39] and the FFmpeg movflags `+frag_keyframe` and `hybrid_fragmented` [J-45]; the `latency` properties of `webrtcbin`, `rtpbin`, `rtpjitterbuffer`, `rtspclientsink` and `rtspsrc` [K-07], [K-08], [K-09]; and the `x264enc` settings `tune=zerolatency`, `bframes=0` and `profile=baseline` in caps (CORRECTED) [K-29]. It attests no command for listing PCIe or USB devices, showing which USB storage driver is bound, showing the NVMe interrupt mode, inspecting the structure of an MP4 file, checking a filesystem after a power cut, controlling HDD standby, or measuring camera-to-viewer latency. The procedures below set the `pciex1` dtparam to on, because it defaults to off [J-15]; no source in the register quotes the `config.txt` line that does so (bare `dtparam=pciex1` or with a value), so the line form is NEEDS VERIFICATION. Those steps are NEEDS VERIFICATION and are tracked in OQ-101.
- **Device names are placeholders.** `/dev/v4l-subdevN`, `/dev/mediaN` (`media-ctl -d <N>`), `/dev/videoN`, `<bus>` and entity names such as `"tc358743 1x-000f"` vary by board and connector, and a Raspberry Pi engineer reported that they changed between kernel versions on Pi 5 [C-28]. On PACSCORDER they are `UNKNOWN — VERIFICATION REQUIRED` (HARDWARE TEST REQUIRED, OQ-043). Record the actual names in every result entry.
- **Control model.** The procedures follow ADR-006, which is **PROPOSED, not accepted** (OQ-014): Media Controller mode on every platform, with EDID and DV timings issued on the TC358743 sub-device node.
  - On Pi 4/CM4 the stock overlay default is the legacy video-node mode instead [B-43], [C-10]. In that mode EDID and DV-timings ioctls go through `/dev/videoN` [B-25], and sub-device nodes are registered read-only [C-36].
  - If ADR-006 is rejected, the Pi 4/CM4 steps must be rewritten for legacy mode.
- **Expected results are source predictions, not acceptance criteria.** No requirement has owner-accepted acceptance criteria yet (OQ-017).
  - Quantitative thresholds (duration, allowed frame drops, latency, temperature) are `UNDEFINED`.
  - A result may be recorded as `TESTED — PASS` only against criteria the owner has accepted. Until then, record what was observed and compare it with the source-predicted result.
- **Community and reasoning sources.** Facts of tier `community` are worded "reported by …". Facts of tier `reasoning` are calculations or inferences from other cited facts and are labelled as reasoning.

### Common setup (applies to every hardware test)

| Item | Value |
|---|---|
| Platform | One of Pi 4 Model B, CM4, Pi 5, CM5. The product platform is **undecided** (ADR-004 OPEN, OQ-011). Run each test on every platform still under consideration. *(Added 2026-10-08.)* Owner, 2026-10-07: bring-up evaluates **CM4 and CM5 side by side**; ADR-004 stays OPEN until the TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 results exist. Run every hardware test on both CM4 and CM5 and record each board separately. Pi 4 Model B and Pi 5 are not removed from ADR-004 or OQ-011, so their columns stay in the platform reference below. |
| Lane configuration | Both a 2-lane and a 4-lane CSI-2 configuration are required, each capturing every frame rate its link carries (REQ-CAP-007, DRAFT; owner, 2026-10-07). ADR-004 is now a choice per configuration (still OPEN). 2-lane: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port. 4-lane: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05]. Whether one bridge board can serve both is UNKNOWN (OQ-021). Record the lane configuration in every result entry. |
| Bridge board | UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, OQ-018). The lane count, REFCLK oscillator, INT/RESETN wiring, CAM_GPIO use and I/O voltage are all unknown (OQ-019, OQ-020, OQ-021, OQ-022, OQ-024). |
| HDMI source(s) | Source types: the HDMI output of Blackmagic ATEM switchers and cameras connected directly (REQ-CAP-008, DRAFT; owner, 2026-10-07). Which ATEM and camera models is OPEN — OWNER DECISION REQUIRED (OQ-102). *(Superseded 2026-10-08: OQ-102 was ANSWERED by the owner on 2026-10-07 — "Any HDMI camera (generic)", with no model list, plus ATEM switcher outputs. What is accepted is defined by the supported-mode matrix and the EDID (OQ-002). Representative cameras and an ATEM are still needed to run the tests; record the model, firmware and audio settings of every source used.)* The mode list and EDID are OWNER DECISION REQUIRED (OQ-002). |
| Power | UNKNOWN — VERIFICATION REQUIRED (OQ-023). The rules' "12V power" is an example, not a PACSCORDER specification. *(Added 2026-10-09; ADR-009 / research topic J.)* For the bring-up IO boards: the CM4 IO Board's PCIe slot, which holds the NVMe SSD of ADR-009, is powered only from the +12 V barrel input, and with a 5 V-only PoE HAT PCIe cards do not work [J-05] (RISK-026). The CM5 IO Board is powered through USB-C and negotiates 5 V at 5 A by default; on 5 V/3 A a 600 mA peripheral limit applies [J-20]. Record the supply in every result entry. |
| OS image (bring-up) | Raspberry Pi OS Lite (64-bit) 2026-10-06 image, based on Debian 13 "trixie" [G-01], with Linux kernel 6.18.50 [G-04]. This follows ADR-003, which is ACCEPTED (owner, 2026-10-07; OQ-012 ANSWERED). The stock image is for bring-up only: the product runs the project's own built image (REQ-BLD-002, DRAFT), which TEST-BLD-001 builds and boots. Driver behaviour in the expected results comes from the source inspected in research, the `rpi-6.18.y` tip at 6.18.55 [E-37]; whether `tc358743.c` differs in the shipped 6.18.50 is OQ-097 (KERNEL SOURCE INSPECTION REQUIRED). |
| Tools in that image | `v4l2-ctl` and `media-ctl` (v4l-utils) are installed [G-24]. `i2c-tools` is in the archive but **not installed** [G-25]. **No GStreamer packages are installed** [G-32]. |
| Encode, audio and streaming packages *(added 2026-10-08)* | Taken from the archive, not from the Lite image. GStreamer: `x265enc` is in `gstreamer1.0-plugins-bad`, which Raspberry Pi rebuilds and still ships with it [H-11], [H-12] (CORRECTED); `voaacenc` is also in that package [I-44]; `opusenc` is in `gstreamer1.0-plugins-base` and `alsasrc` in `gstreamer1.0-alsa` [I-45]; `avenc_aac` is in Debian's `gstreamer1.0-libav` [I-46]. FFmpeg: the Raspberry Pi FFmpeg 7.1.5 links `libx265` [H-08], [H-09] and `libopus`, and has the native `aac` encoder but no `libfdk_aac` [I-39]. x265 is Debian's 4.1-2, not overridden by Raspberry Pi [H-01], [H-02]. Whether `ffmpeg` and the ALSA user tools (`arecord`) are installed in the Lite image is not in the source register: NEEDS VERIFICATION (BUILD TEST REQUIRED). Research noted that the FFmpeg encoder list was inferred from package dependencies and should be checked on the image itself (research gap, topic I — not a register fact; OQ-063). *(Later on 2026-10-08, H.265 deferred — REQ-ENC-002; not in current scope: `x265enc`, FFmpeg `libx265` and x265 are needed only if REQ-ENC-002 is re-activated. `libx265-215` is still pulled in as a dependency of the Raspberry Pi FFmpeg's `libavcodec61` [H-09], and `x265enc` ships inside `gstreamer1.0-plugins-bad` [H-11], [H-12] (CORRECTED), the package that also carries `rtmp2sink` and `webrtcbin` [G-27]. Reasoning: both can be present on the image even though H.265 is not used.)* |
| Boot configuration file | `/boot/firmware/config.txt` [G-11] |
| Hardware revision | `—` (no PACSCORDER hardware revision exists; Rule 8 revision history is in [HARDWARE.md](HARDWARE.md)) |

### Platform reference (from sources; nothing measured on PACSCORDER)

| Item | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| CSI-2 data lanes at the connector | 2 [C-01] | CAM1: 4, CAM0: 2 [C-02] | 4 per port [C-04] | 4 per MIPI interface [C-05] |
| Overlay | `tc358743` [G-12] | `tc358743`. `4lane` applies to CAM1 only [A-46]; `cam0` retargets to CAM0 [B-42] | `dtoverlay=tc358743` is redirected to `tc358743-pi5` [C-11], [E-43]; parameters `4lane`, `link-frequency`, `cam0` [G-13] | as Pi 5 [C-11], [E-43] |
| Media Controller mode | `media-controller` overlay parameter [B-43], [C-10] | as Pi 4 | always [C-11] | always [C-11] |
| CSI-2 receiver driver | downstream Unicam, module `bcm2835-unicam-legacy` [C-09], [G-21] | as Pi 4 | downstream RP1 CFE, module `rp1-cfe-downstream` [C-29], [E-44] | as Pi 5 |
| Camera I2C bus (bridge expected at 7-bit 0x0f [A-15]) | `i2c-10` on GPIO 44/45 [C-24] | CAM1 `i2c-10` (GPIO 44/45); CAM0 `i2c-0` (GPIO 0/1) [C-24], [C-25] | CAM/DISP0 `/dev/i2c-10`, CAM/DISP1 `/dev/i2c-11` [C-26]; a Raspberry Pi engineer reported other numbers on earlier kernels [C-28] | depends on the carrier board [C-27]. CM5 IO Board: CAM/DISP 1 is RP1 i2c0 on GPIO 0/1, CAM/DISP 0 is RP1 i2c6 on GPIO 38/39 [C-27], [B-44]; `/dev` number UNKNOWN (OQ-052) |
| I2C jumpers on the official IO board | — | CAM0 needs both J6 jumpers [C-03] | — | CAM/DISP 1 needs two J6 jumpers [C-06]; a DT comment disagrees (research gap, topic C; OQ-052) |
| Encoder | hardware `bcm2835-codec`, `/dev/video11` [D-06], [D-07] | as Pi 4 | software only [G-22], [D-31] | software only [D-31] |
| Two concurrent H.264 encodes, recording + live *(added 2026-10-08; owner, OQ-005)* | reasoning: as CM4, because Pi 4 Model B is also BCM2711 [D-10] | one encoder, a V4L2 M2M device [D-06], [D-08]; the register has no fact on concurrent encode sessions: UNKNOWN (OQ-115). Reasoning: two 1080p30 encodes need 2 × 244,800 = 489,600 macroblocks/s, the rate of one 1080p60 encode and about 2.0× the 1080p30 specification [D-10], [D-52] (RISK-002) | both in software [G-22]; as CM5 | both in software [D-31], [G-22]. Reasoning from [G-22]: roughly double the encode CPU of one encode; the base of the official "~30–40 % CPU" figure is UNKNOWN (OQ-059, RISK-003) |
| H.265 encoder *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | software only: no HEVC encoder in `bcm2835-codec` [D-24] | as Pi 4 [D-24] | software only [D-31] | software only [D-31] |
| x265 SIMD paths *(added 2026-10-08; deferred — REQ-ENC-002; not in current scope)* | reasoning: as CM4, because Pi 4 Model B is also BCM2711 [D-10], [H-05] | Cortex-A72: Neon only, no DotProd; I8MM, SVE, SVE2 not applicable [H-05] | reasoning: as CM5, because Pi 5 is also BCM2712 [D-30], [H-05] | Cortex-A76: Neon DotProd kernels can apply, enabled only if the kernel reports ASIMDDP [H-04], [H-05]; whether they are active is OQ-105 |
| HDMI audio CPU DAI behind `i2s_clk_consumer` *(added 2026-10-08)* | not covered by the topic I research (CM4 and CM5 only) | single `bcm2835-i2s` on GPIO 18–21 (ALT0) [I-10]; capture exactly 2 channels, 8–384 kHz, S16_LE / S24_LE / S32_LE [I-13] | not covered by the topic I research (OQ-054) | RP1 I2S1 (`rp1_i2s1`, `snps,designware-i2s`) on GPIO 18–21 [I-07], [I-08]; the clock-consumer instance a codec-master link needs [I-09]; channel count and formats come from hardware registers [I-14]. **Operation unconfirmed** (OQ-054) |
| TC358743 audio controls appear on *(added 2026-10-08)* | not covered by the topic I research | `/dev/videoN` with legacy Unicam (stock overlay, `media-controller` off) [I-23]; in Media Controller mode (ADR-006, PROPOSED) reasoning from [I-23]: the TC358743 sub-device node | not covered by the topic I research | only the TC358743 `/dev/v4l-subdevN` [I-23] |
| Expected ALSA PCM name *(added 2026-10-08)* | not covered by the topic I research | `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15]; card index varies, reported as card 0 and card 1 in the same 2019 thread (community source) [I-16] | not covered by the topic I research | reasoning from source: `1f000a4000.i2s-dir-hifi dir-hifi-0` [I-17]. Select the device by card id `tc358743` on both boards [I-15], [I-17] |
| NVMe SSD attachment, ADR-009 *(added 2026-10-09)* | not covered by the topic J research (CM4 and CM5 only) | one PCIe Gen 2 x1 lane [J-01]. CM4 IO Board: the single PCIe Gen 2 x1 socket, with a passive PCIe-to-M.2 adaptor [J-03], [J-11]; slot powered only from the +12 V barrel input [J-05]; SSD appears as `/dev/nvme0`, namespace `/dev/nvme0n1` [J-11]; the two official datasheets disagree on MSI-X (OQ-123) [J-02] | not covered by the topic J research | CM5 IO Board M.2 M-key slot: PCIe Gen 2 x1 by default, Gen 3 "experimental and therefore unsupported" [J-13]; 2230, 2242, 2260 and 2280 [J-14]. In `rpi-6.18.y` the `pcie1` link used by the slot is "disabled" in the device tree and the `pciex1` dtparam (alias `nvme`) defaults to off [J-15], [J-17]; whether the firmware enables it at run time is unknown (research gap, topic J; OQ-124) |
| USB path for the HDD, ADR-009 *(added 2026-10-09)* | not covered by the topic J research | USB 2.0 only; no USB 3.0 controller [J-01], [J-08]. CM4 IO Board: on-board USB2514B hub, one VBUS switch of about 1.2 A for the connectors; plugging in the micro-USB cable disables the hub [J-06]. USB is enabled by `otg_mode=1` (XHCI USB 2.0) in Raspberry Pi OS, or by `dtoverlay=dwc2,dr_mode=host` per the IO Board datasheet [J-07]; `uas` cannot bind behind `dwc2` but can under `otg_mode=1` [J-24] | not covered by the topic J research | two USB 3.0 interfaces [J-18] on RP1's two xHCI controllers, each port with independent bandwidth [J-21]; the NVMe link and RP1 are on separate PCIe root ports [J-17]. CM5 IO Board: two USB 3.0 Type-A ports sharing about 1.2 A of VBUS [J-19]; 600 mA peripheral limit on a 5 V/3 A supply [J-20] |

### Test order

Bring-up uses the stock image (ADR-003, ACCEPTED), so TEST-BLD-001 does not gate the hardware tests. The product's own image (REQ-BLD-002) is verified by TEST-BLD-001, whose step 4 re-runs TEST-PLT-001 on it.

```text
TEST-HW-001 → TEST-DRV-001 → TEST-DRV-002 → TEST-PLT-001 → TEST-CAP-001 → TEST-CAP-002
TEST-DRV-002 → TEST-AUD-001                       (only if audio is required, OQ-004)
                                                  (superseded 2026-10-08: audio is required, OQ-004 ANSWERED 2026-10-07;
                                                   run on CM4 and CM5 — on CM5 it gates the platform, ADR-004)
TEST-CAP-002 (or a lower-rate capture) + TEST-AUD-001 steps 1–8 → TEST-AUD-001 step 9 (A/V offset)
TEST-AUD-001 → TEST-REC-001, TEST-STR-001, TEST-STR-002   (audio in every output, REQ-CAP-006)
TEST-CAP-002 → TEST-CAP-003
TEST-CAP-002 → TEST-CAP-004                       (4-lane configuration)
TEST-CAP-001 → TEST-CAP-004                       (2-lane configuration: TEST-CAP-002's 1080p60 run does not apply)
TEST-CAP-002 → TEST-DMA-001 → TEST-ENC-001
TEST-ENC-001 → TEST-REC-001, TEST-STR-001, TEST-STR-002
TEST-CAP-001, TEST-CAP-002 → TEST-ATEM-001 (HDMI capture of the ATEM output; the only part in current scope, OQ-009)
TEST-ENC-001 + the selected outputs → TEST-PERF-001
TEST-BLD-001                                      (independent of bring-up)
```

*(Added 2026-10-08.)* TEST-ENC-001 now contains H.264 and H.265 runs on CM4 and CM5 (REQ-ENC-001); its canonical title in [README.md](README.md) still says "H.264" and is unchanged here. TEST-REC-001, TEST-STR-001, TEST-STR-002 and TEST-PERF-001 take the H.265 runs as well as the H.264 runs, as far as the owner scopes H.265 to each output (OQ-103). *(Superseded later on 2026-10-08 by the owner's answer to OQ-103, "H.264 only for now": TEST-ENC-001 is now titled "Sustained real-time H.264 encode (H.265 deferred)" in [README.md](README.md). Its H.265 runs, and the H.265 runs in TEST-REC-001, TEST-STR-001, TEST-STR-002 and TEST-PERF-001, are deferred — REQ-ENC-002; not run in current scope. They are kept as the procedure for when REQ-ENC-002 is re-activated. The test order above is unchanged.)*

*(Added later on 2026-10-08.)* Owner answer to OQ-005, "Separate record + live": REQ-ENC-001 requires two simultaneous H.264 encodes, a recording encode and one live encode shared by RTMP and WebRTC. In the procedures below:
- TEST-DMA-001 also records how one capture feeds both encoders (one capture buffer, two consumers);
- TEST-ENC-001 includes a **two-encode run on CM4 and CM5** (CM4: OQ-115, RISK-002; CM5: OQ-059, RISK-003);
- TEST-REC-001 uses the recording encode, and TEST-STR-001 and TEST-STR-002 share the live encode;
- TEST-PERF-001 measures the **combined load**: two video encodes, two audio encodes (AAC and Opus) and all three outputs.

The test order above is unchanged. The canonical title of TEST-ENC-001 in [README.md](README.md) is unchanged.

*(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* Owner decisions of 2026-10-08 on recording storage (ADR-009, ACCEPTED) and live latency (OQ-116 ANSWERED). In the procedures below:
- TEST-REC-001 has a full procedure for ADR-009 on CM4 and CM5: mirrored recording to the NVMe SSD and the HDD, fragmented-MP4 check, power cut, HDD stall injection, USB-to-SATA bridge soak, player and editor compatibility, and boot with the HDD absent;
- TEST-STR-002 measures **camera-to-viewer latency** against the < 1 s target for WebRTC viewers, LAN and internet viewers separately — internet viewers only if OQ-008 puts them in scope; how the target is judged is also OQ-008 (OQ-125, OQ-128);
- TEST-STR-001 records RTMP latency at each destination, best-effort, with no pass or fail on latency (OQ-116);
- TEST-ENC-001 records the live encode's keyframe interval, B-frame and latency settings (OQ-127);
- TEST-PERF-001 runs its soak with the mirrored recording.

Dependencies added, no order change: on CM5, TEST-REC-001 step 1 records whether the M.2 NVMe link enumerates with and without the `pciex1` dtparam (OQ-124, whose resolving tests also include TEST-PLT-001 and TEST-BLD-001); TEST-REC-001 step 5 (HDD stall injection) runs together with TEST-STR-001 and TEST-STR-002, because it checks that a stall does not reach the live path (OQ-117, RISK-028). No test ID, title or "Verifies" mapping changed.

---

## TEST-HW-001 — TC358743 I2C detection

| | |
|---|---|
| Verifies | REQ-DRV-001 |
| Related risks | RISK-021 (bridge-board wiring hazards) |
| Related open questions | OQ-018, OQ-021, OQ-022, OQ-024, OQ-026, OQ-043, OQ-052, OQ-101 |
| Depends on | Hardware assembled; cable orientation checked (see Setup) |

### Objective

Confirm that the TC358743 on the PACSCORDER bridge board answers on the platform's camera I2C bus at the expected address, independently of the Linux driver.

### Setup

- Platform and connector: see the platform reference table. PACSCORDER's choice is UNKNOWN — OWNER DECISION REQUIRED (OQ-011).
- Bridge board: UNKNOWN — VERIFICATION REQUIRED (OQ-018).
  - The stock overlay and driver request no regulator [C-23]. Reasoning: the connector's CAM_GPIO / power-enable line is therefore expected to stay low [C-51].
  - A board that needs CAM_GPIO for power or reset will not answer (OQ-022, VENDOR CONFIRMATION REQUIRED).
- Cable: UNKNOWN (OQ-021). A Raspberry Pi engineer reported that a wrongly sided 22-to-15-pin adapter on Pi 5 swaps GND and 3V3 and can damage either board [C-45]. Reasoning: the CM4 IO Board camera connectors are also 22-pin [C-03] and the CM5 IO Board uses 22-pin CAM/DISP connectors [C-06], so the same adapter hazard is expected there. **Check cable orientation against the board schematic before power-up.**
- Official IO boards: fit the J6 jumpers where the connector needs them (CM4 CAM0 [C-03]; CM5 CAM/DISP 1 [C-06]).
- HDMI source: not required.
- Software: install `i2c-tools`, which is not in the Lite image [G-25].
- Driver state (reasoning): run first **without** the `tc358743` overlay, so that no driver holds the address. Whether the camera I2C bus node exists without the overlay is NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED). If it does not, run with the overlay loaded and continue with TEST-DRV-001.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

```bash
sudo apt install i2c-tools      # package name [G-25]; "apt install" form as in [G-47]
i2cdetect -y <bus>              # i2cdetect ships in i2c-tools [G-25]; "-y" form is from the owner's Rule 9 example (OQ-101)
```

- `<bus>` is taken from the platform reference table [C-24], [C-25], [C-26], [C-27]. It is UNKNOWN for PACSCORDER until observed (OQ-043).
- No register fact documents the semantics of `i2cdetect` options: NEEDS VERIFICATION (OQ-101).
- Optional: read the CHIPID register (0x0000) [A-18] directly. Register addresses are 16-bit, sent MSB first, and register values are little-endian [A-17]. `i2ctransfer` ships in `i2c-tools` [G-25], but its syntax for this read is NEEDS VERIFICATION.

### Expected Result

- A device answers at 7-bit address **0x0f**. That is the address in the kernel DT binding example and in the Raspberry Pi overlay [A-15]. The bus is the platform's camera I2C bus [C-24], [C-25], [C-26], [C-27]. On Pi 5 the bus numbering changed between kernels, as reported by a Raspberry Pi engineer and corrected in research [C-28].
- **Address uncertainty.** The public TC358743XBG datasheet states no I2C address and no address strap [A-15]. The sister part TC358749XBG documents 0x0F or 0x1F, selected by the INT pin level at reset [A-50]. If the bridge answers at **0x1f**, record it: the DT `reg` value must then change ([DEVICE_TREE.md](DEVICE_TREE.md); OQ-026, DATASHEET REQUIRED).
- Other devices may share the bus. With CM5 on the CM4 IO Board, `i2c_csi_dsi1` also serves DISP1, the RTC and the fan [C-27].
- Optional CHIPID read: the chip-ID byte (bits [15:8]) is expected to be 0x00 [A-18], [A-19]. The revision byte (bits [7:0]) is UNKNOWN — record it (OQ-029).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-DRV-001 — TC358743 driver probe and chip ID

| | |
|---|---|
| Verifies | REQ-DRV-001 |
| Related risks | RISK-005, RISK-007, RISK-021 |
| Related open questions | OQ-013, OQ-019, OQ-020, OQ-029, OQ-043, OQ-052, OQ-100 |
| Depends on | TEST-HW-001 |

### Objective

Confirm that the in-tree `tc358743` driver (ADR-002, PROPOSED) probes the bridge, passes the chip-ID check and registers a V4L2 sub-device, with no clock, endpoint or lane-rate errors.

### Setup

- Image: the driver is built as module `tc358743` [A-35]. It is enabled as a module in both Raspberry Pi arm64 defconfigs [E-39], and the 2026-10-06 image ships `tc358743.ko.xz` for both kernels [G-16].
- **Reference clock — check before first boot.**
  - The DT clock frequency must equal the bridge board's oscillator. That frequency is UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, OQ-019).
  - The stock overlay declares 27 MHz [A-45].
  - The driver accepts only 26, 27 or 42 MHz [B-07]. Any other value leads to a kernel BUG instead of a clean probe failure (kernel source [A-22] and reasoning from it [B-11], both CORRECTED; RISK-007).
- Lanes and link frequency:
  - The overlay default is `data-lanes <1 2>` and `link-frequencies` 486000000 (972 Mbps per lane) [A-45], [C-12].
  - Use `4lane` only where the board and connector carry 4 lanes (OQ-021) [B-42], [A-46].
- `config.txt` lines (PROPOSED; NOT YET RUN ON PACSCORDER HARDWARE). The parameter form is `dtoverlay=tc358743,<param>=<val>` [G-12]; `,cam0` selects connector 0 [C-39]. Every line below that gives a parameter by name alone or combines several parameters is NEEDS VERIFICATION (OQ-100); see the note under the table.

  | Platform | Line |
  |---|---|
  | Pi 4 Model B | `dtoverlay=tc358743,media-controller` (ADR-006 PROPOSED) [B-43] — bare parameter: syntax NEEDS VERIFICATION (OQ-100) |
  | CM4, CAM1 with a 4-lane board | `dtoverlay=tc358743,4lane,media-controller` [B-42], [B-43] — bare and combined parameters: syntax NEEDS VERIFICATION (OQ-100) |
  | CM4, CAM0 | `dtoverlay=tc358743,cam0,media-controller` [B-42], [B-43] — 2 lanes only [C-02]; combined parameters: syntax NEEDS VERIFICATION (OQ-100) |
  | Pi 5 | `dtoverlay=tc358743-pi5` [G-13], or `dtoverlay=tc358743`, which is redirected to it [C-11]. [DEVICE_TREE.md](DEVICE_TREE.md) §6.3 proposes naming `tc358743-pi5` explicitly. Add `,4lane` for a 4-lane board and `,cam0` for CAM/DISP0 [G-13], [C-39]; a bare `4lane` and the combination `cam0,4lane` are NEEDS VERIFICATION (OQ-100) |
  | CM5 on the CM5 IO Board | As Pi 5 [C-11], [E-43]. Which connector the default line targets, and whether the J6 jumpers are needed, NEEDS VERIFICATION (OQ-052) |
  | CM5 on the CM4 IO Board | **No line is proposed** ([DEVICE_TREE.md](DEVICE_TREE.md) §6.4). Research found that neither stock I2C/CSI pairing matches the CAM1 connector on CM5 (research open question, topic C; not a register fact). A custom overlay or test evidence is required (OQ-052). The CAM0 connector cannot be used: on CM5 those pins carry USB 3.0 [C-05] |
  | All | `camera_auto_detect=0` — PROPOSED. Prudent, but documented only for camera-sensor overlays, not for TC358743 (CORRECTED) [C-39], [G-15]; OQ-072 |

  - **Syntax NEEDS VERIFICATION (OQ-100).** The register attests the `dtoverlay=tc358743,<param>=<val>` form [G-12], [G-13] and appending `,cam0` [C-39]. It does not attest giving `4lane` or `media-controller` by name alone, or combining several parameters on one line. Check both against the official `config.txt` documentation before use; [DEVICE_TREE.md](DEVICE_TREE.md) §6.0 records the same caveat.
  - CM5: the configuration depends on the carrier board (OQ-018, OQ-052). A CM5 on a custom carrier is UNKNOWN — VERIFICATION REQUIRED.
- Reset and interrupt: the stock overlay has neither `reset-gpios` nor `interrupts` [B-41]. Whether the board wires RESETN or INT to a Pi GPIO is UNKNOWN (OQ-020).
- HDMI source: not required.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot with the `config.txt` line for the platform.
2. Inspect the kernel log for messages from `tc358743` and from the CSI-2 receiver driver. Record the exact lines.
3. Confirm that a `/dev/v4l-subdevN` node exists for the bridge. The sub-device sets `V4L2_SUBDEV_FL_HAS_DEVNODE` [B-18]; in Media Controller mode the node is read-write [C-36]. The method to map N to the bridge is NEEDS VERIFICATION (OQ-043).
4. Record the CHIPID revision byte. The driver's `log_status` prints it [A-19]; the user-space command that triggers it is NEEDS VERIFICATION.
5. If `reset-gpios` is wired (OQ-020): observe RESETN at probe with an instrument (HARDWARE TEST REQUIRED).

### Expected Result

- **No** `not a TC358743 on address 0x1e`.
  - The driver reads CHIPID and requires the chip-ID byte to be 0x00; otherwise it logs that message and returns `-ENODEV` [A-19], [B-14].
  - The address is printed in 8-bit form, so 7-bit 0x0f appears as 0x1e [A-16].
- **No** `unsupported refclk rate` and no kernel BUG (kernel source [A-22]; reasoning from it [B-11]). **No** `failed to get refclk` [A-22].
- **No** `missing endpoint node`, `missing CSI-2 properties in endpoint` or `invalid number of lanes` [B-06].
- **No** `untested bps per lane` with `link-frequency` 486000000 or 297000000 [A-24], [B-09].
  - With a 26 or 42 MHz reference clock, the PLL-derived lane rate differs from the nominal one (reasoning) [A-23], [B-10].
  - Whether that also triggers the message is KERNEL SOURCE INSPECTION REQUIRED.
- **No** `-EIO` from a missing SMBus byte-data capability [A-20].
- The probe and failure messages show the address as 0x1e [A-16].
- The entity is named `tc358743 <bus>-000f`. On Pi 5 with 6.18 kernels the corrected register entry gives `tc358743 11-000f` on CAM/DISP1 or `tc358743 10-000f` on CAM/DISP0, from the DT aliases and later issue reports; earlier forum posts in a Raspberry Pi engineer's thread showed `tc358743 4-000f` (CORRECTED, community) [C-28]. Names on other platforms are UNKNOWN until observed (OQ-043).
- State after probe:
  - one source pad, entity function `MEDIA_ENT_F_VID_IF_BRIDGE` [B-18];
  - three controls: `V4L2_CID_DV_RX_POWER_PRESENT`, "Audio sampling rate", "Audio present" [B-16];
  - default timings CEA 640x480p59.94 and bus code `RGB888_1X24` [B-15], [C-19].
- Without an `interrupts` property, the driver polls the interrupt status every 1000 ms [A-30], [C-20].
- CHIPID revision byte: UNKNOWN — record the value (OQ-029).
- On Pi 5, Raspberry Pi engineers reported a B102 board probing in December 2023 [C-41]. That is not evidence for the PACSCORDER board.

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-DRV-002 — EDID load and HDMI hot-plug assertion

| | |
|---|---|
| Verifies | REQ-CAP-003 |
| Related risks | RISK-010 |
| Related open questions | OQ-002, OQ-024, OQ-032, OQ-093, OQ-101 |
| Depends on | TEST-DRV-001 |

### Objective

Confirm that a source sees no sink until an EDID is written, that writing the EDID asserts HDMI hot-plug (HPD), and that the EDID must be rewritten after every boot or driver load.

### Setup

- As TEST-DRV-001, with an HDMI source connected and supplying +5V.
- EDID file: the PACSCORDER EDID content is UNKNOWN — OWNER DECISION REQUIRED (OQ-002).
  - REQ-CAP-003 (PROPOSED) requires an EDID that advertises only modes the wired CSI-2 lanes can carry.
  - For bring-up only, `v4l2-ctl` has a built-in `hdmi` EDID that advertises up to 1080p60 [B-24]. Reasoning: that exceeds what a 2-lane link carries [C-48], so on a 2-lane platform a source may pick a mode that later fails at stream start (TEST-CAP-004).
- HPD observation: measure the HPD line with an instrument at a board test point, which is UNKNOWN (OQ-024; HARDWARE TEST REQUIRED). Also record whether the source reports a connected display.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot. Before any EDID is written, observe HPD and the source's display detection.
2. Write the EDID on the sub-device node (pad must be 0 [B-22]):
   ```bash
   v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,file=<pacscorder-edid>   # --set-edid syntax [B-24]; EDID via VIDIOC_S_EDID [C-37]
   # bring-up alternative:
   v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,type=hdmi                # built-in type [B-24]
   ```
   The `-d /dev/v4l-subdevN` form is NEEDS VERIFICATION (OQ-101; see Conventions). In legacy mode on Pi 4/CM4 the same ioctl goes to `/dev/videoN` instead [B-25]. That mode is not used if ADR-006 is accepted.
3. Observe HPD and the source's display detection again.
4. Clear the EDID and observe HPD:
   ```bash
   v4l2-ctl -d /dev/v4l-subdevN --clear-edid 0                            # help-text form "--clear-edid <pad>" [B-24]
   ```
   The argument form `0` and the `-d /dev/v4l-subdevN` form are NEEDS VERIFICATION (OQ-101).
5. Reboot, or reload the driver, and repeat step 1. When the product writes the EDID, and whether an EDID survives a driver unload and reload, is OQ-093.
6. Optional negative test: write an EDID of more than 8 blocks [B-22].
7. Read back the EDID and the `V4L2_CID_DV_RX_POWER_PRESENT` control [B-16] at each step. Commands NEEDS VERIFICATION (OQ-101).

### Expected Result

- Step 1:
  - No HPD and no sink seen by the source. The driver does not enable hot-plug until an EDID has been written; its code path is labelled `no EDID -> no hotplug` [A-33], [B-21]. Whether that string reaches the kernel log at the default log level is NEEDS VERIFICATION (KERNEL SOURCE INSPECTION REQUIRED).
  - Reading the EDID returns `-ENODATA` [B-22].
  - The HPD state between power-on and the first EDID write is UNKNOWN (OQ-032).
- Step 2: the write succeeds with pad 0; any other pad or a non-zero start block returns `-EINVAL` [B-22].
- Step 3: with +5V present, delayed work raises HPD after HZ/7 jiffies. The driver comment says 143 ms; with integer division it is 140 ms at the Raspberry Pi default HZ=250 (CORRECTED) [B-21]. The source detects a sink.
- Step 4: zero blocks clears the EDID and HPD stays low [B-22].
- Step 5: the EDID is held in on-chip SRAM [A-31], and after probe the driver has no EDID stored (`edid_blocks_written == 0`) [B-21]. Reasoning: the EDID is therefore expected to be absent after every boot or driver load, so the step 1 result repeats. Research also found no DT property and no default EDID in the driver (research gap, topic B — not a register fact).
- Step 6: more than 8 blocks returns `-E2BIG` [B-22].
- `V4L2_CID_DV_RX_POWER_PRESENT` is one of the driver's three controls [B-16], and the driver updates its controls on a +5V interrupt [B-23]. Reasoning: it is expected to reflect source +5V. When +5V is removed, the driver drops HPD [B-23].

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-PLT-001 — Overlay load and media graph per platform

| | |
|---|---|
| Verifies | REQ-PLT-001, REQ-ARCH-001 |
| Related risks | RISK-001, RISK-012 |
| Related open questions | OQ-011, OQ-014, OQ-021, OQ-043, OQ-044, OQ-046, OQ-049, OQ-052, OQ-065, OQ-072, OQ-100, OQ-101 |
| Depends on | TEST-DRV-001 on the same platform |

### Objective

For each candidate platform, confirm three things:

- the correct overlay loads;
- the expected CSI-2 receiver driver binds;
- the Media Controller graph contains the expected TC358743 → receiver → video-node entities and links.

This covers the "TC358743 → CSI-2 → receiver → Media Controller → V4L2" part of REQ-ARCH-001.

### Setup

- One unit of each platform still under consideration (ADR-004 OPEN), with the `config.txt` lines from TEST-DRV-001. Their bare and combined parameter syntax is NEEDS VERIFICATION (OQ-100); this test is the resolving test for it.
- **CM5 carrier caveat.** The CM5 configuration depends on the carrier board (OQ-052; [DEVICE_TREE.md](DEVICE_TREE.md) §6.4):
  - on the CM4 IO Board no `config.txt` line is proposed;
  - on the CM5 IO Board the connector the default line targets, and the J6 jumper requirement, NEEDS VERIFICATION.

  Record the carrier board in every CM5 result entry. A CM5 result applies only to the carrier board used.
- Drivers for every platform:
  - the 2026-10-06 image ships `tc358743.ko.xz` for both kernels [G-16];
  - both rpi-6.18.y arm64 defconfigs build both Unicam drivers and both RP1 CFE drivers as modules [G-17];
  - reasoning on verified inputs (CORRECTED): the image's module trees include unicam and rp1-cfe, and one image can boot all four candidates, but the overlay, its parameters and the capture model differ per board [G-71].
- HDMI source: optional.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Boot with the platform's `config.txt` line.
2. Identify which CSI-2 receiver driver is bound (method NEEDS VERIFICATION; OQ-044).
3. Print the media graph of the receiver's `/dev/mediaN`. No register fact attests a `media-ctl` option for printing the topology: NEEDS VERIFICATION (OQ-101).
4. Pi 5 / CM5 only: enable the csi2 → video-node link, as reported by Raspberry Pi engineer 6by9 [C-33]. The report used `-d 2`; the device number is system-specific (OQ-043).
   ```bash
   media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'      # [C-33]
   ```
5. Record every entity name, pad number, link flag and `/dev` node.

### Expected Result

**Pi 4 Model B and CM4 (Media Controller mode)**

- Overlay `tc358743` is loaded [G-12].
- The `media-controller` parameter removes the fragment that would otherwise select `brcm,bcm2835-unicam-legacy` [B-43], [C-10].
- The downstream Unicam driver (`bcm2835-unicam-legacy.ko`) binds the base `brcm,bcm2835-unicam` compatible in Media Controller mode (CORRECTED) [E-40], [C-09], [G-21].
- In the graph (CORRECTED) [C-36]:
  - an image node named `unicam-image`, with an IMMUTABLE|ENABLED link straight from the TC358743 pad;
  - no CSI-2 receiver sub-device in between;
  - no `unicam-embedded` node, because the TC358743 has a single source pad.
- The TC358743 sub-device node is read-write [C-36].
- Research found that filtered `ENUM_FMT` may not list UYVY in this mode (research gap, topic C; OQ-046). Record the result; do not treat a missing UYVY entry as a failure.
- Pi 4 Model B only: the csi1 port is limited to 2 data lanes [B-48], [C-08].

**Pi 5 and CM5**

- `tc358743-pi5` is applied, whether named directly [G-13] or reached through the `dtoverlay=tc358743` redirect. It is always Media Controller [C-11], [E-43].
- The downstream RP1 CFE driver (`rp1-cfe-downstream`) binds `raspberrypi,rp1-cfe` [C-29], [E-44].
- In the graph [C-32]:
  - a `csi2` sub-device with sink pads 0–3 and source pads 4–7;
  - an IMMUTABLE|ENABLED link from the TC358743 to `csi2`;
  - links from `csi2` source pads to `rp1-cfe-<node>` video nodes (for example `rp1-cfe-csi2_ch0`) and to `pisp-fe`, all disabled until enabled. After step 4, `csi2:4 → rp1-cfe-csi2_ch0` is enabled.
- The TC358743 entity is named `tc358743 1x-000f`, where the bus number depends on the connector, as reported by a Raspberry Pi engineer [C-28], [C-33].

**All platforms**

- Reasoning from [C-39]: `camera_auto_detect` makes the firmware auto-load overlays for recognised CSI cameras, so with `camera_auto_detect=0` no sensor overlay is expected to be auto-loaded. Whether that line is required at all is OQ-072.

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-001 — DV timings detection for each source mode

| | |
|---|---|
| Verifies | REQ-CAP-002, REQ-CAP-008 |
| Related risks | RISK-008, RISK-009, RISK-012 |
| Related open questions | OQ-002, OQ-028, OQ-040, OQ-049, OQ-083, OQ-102 |
| Depends on | TEST-DRV-002, TEST-PLT-001 |

### Objective

Confirm that, for every source mode PACSCORDER must accept, the DV timings are detected and applied through the V4L2 DV-timings API on the TC358743 sub-device, and that the error codes for no signal, unstable signal and out-of-range timing match the driver's documented behaviour.

### Setup

- As TEST-DRV-002, with the EDID loaded.
- Sources (REQ-CAP-008): run with both source types the owner named on 2026-10-07 — an ATEM switcher's HDMI output and each camera connected directly. The ATEM and camera models are OPEN (OQ-102); record the model and firmware of every source in the result entry. *(Superseded 2026-10-08: OQ-102 ANSWERED 2026-10-07 — any HDMI camera, no model list. Use representative cameras and an ATEM; the accepted set is defined by the supported-mode matrix and the EDID (OQ-002), and unsupported modes are rejected per REQ-CAP-005.)*
  - ATEM Mini Pro: set its HDMI output to Program first; it defaults to multiview [F-25].
  - Cameras: their HDMI output modes are UNKNOWN — VERIFICATION REQUIRED per model (OQ-102).
- Source modes: REQ-CAP-007 requires every frame rate the configured link carries; the exact list is UNKNOWN — OWNER DECISION REQUIRED (OQ-002). Candidates from sources:
  - the ATEM Mini Pro HDMI output standards 1080p23.98, 24, 25, 29.97, 30, 50, 59.94 and 60 [F-23];
  - at least one 59.94 Hz source (OQ-040); fractional rates are part of "all frame rates" unless the owner says otherwise (REQ-CAP-007);
  - one HDCP-requiring source (OQ-028).
- Lane configurations (reasoning): the timings are derived from the TC358743's own timing registers (DE_WIDTH, FV_CNT) [B-28], and the lane count is checked only at stream start [B-30], [B-32]. Detection is therefore expected to behave the same on the 2-lane and the 4-lane configuration (REQ-CAP-007); whether a detected mode can then be streamed on the configured lanes is TEST-CAP-004.
- Interlaced and out-of-range sources are covered by TEST-CAP-004.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.** For each source mode:

```bash
v4l2-ctl -d /dev/v4l-subdevN --set-dv-bt-timings query     # [C-33], [C-37]; "-d <sub-device path>" form: OQ-101
```

- The `-d /dev/v4l-subdevN` form is NEEDS VERIFICATION (OQ-101; see Conventions).
- Record the command's return status and the timings it reports. How `v4l2-ctl` prints the detected timings is NEEDS VERIFICATION (OQ-101).
- Repeat with the HDMI cable unplugged and while the source is switching modes.

### Expected Result

- A progressive mode within the driver capability succeeds. The capability is 640–1920 × 350–1200, pixel clock 13–165 MHz, progressive only [A-08], [B-26].
- The detected width and height equal the source's active size [B-28].
- The frame rate is reported as an integer: fps = round(10000 / FV_CNT), with the pixel clock derived from it. So 59.94 Hz and 60 Hz sources are reported alike [B-28] (OQ-040).
- Error codes from `query_dv_timings` [B-29]:
  - `-ENOLINK` when HPD is low or there is no TMDS signal (cable unplugged, or no EDID loaded);
  - `-ENOLCK` when sync is not stable (for example while the source switches mode);
  - `-ERANGE` when the timings fall outside the capability.
- HDCP: the driver always configures HDCP as disabled on DT platforms [A-04]. A source that enforces HDCP is expected to give no usable video (RISK-008). The exact symptom is UNKNOWN (OQ-028).
- Official Raspberry Pi documentation: timings are set through DV_TIMINGS, and `VIDIOC_S_FMT` changes only the pixel format [C-37].

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-002 — 1080p60 capture: frame rate and frame integrity

| | |
|---|---|
| Verifies | REQ-CAP-001, REQ-CAP-007 |
| Related risks | RISK-001, RISK-006, RISK-011, RISK-012, RISK-016, RISK-020 |
| Related open questions | OQ-001 (ANSWERED 2026-10-07), OQ-003, OQ-011, OQ-021, OQ-037, OQ-038, OQ-041, OQ-045, OQ-049, OQ-050, OQ-095, OQ-099 |
| Depends on | TEST-CAP-001 |

### Objective

Capture 1920x1080 at 60 Hz from the TC358743 into memory through V4L2. Measure the delivered frame rate and check frames for corruption.

Scope against the owner decision of 2026-10-07 (OQ-001 answered: "i need 2 lane and 4 lane with all frame rate"):

- REQ-CAP-001: 1080p60 is required on the 4-lane configuration. This test is its main evidence.
- REQ-CAP-007: this test covers the highest rate of the 4-lane configuration (1080p60). The 2-lane configuration cannot carry 1080p60 [C-37], [C-48]; its highest 1920x1080 rates (1080p50 UYVY, 1080p30 RGB888) and every lower rate of both configurations are covered by the TEST-CAP-004 supported-mode matrix, which reuses this test's configuration and measurement steps.

### Setup

- **4-lane configuration only:** CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05].
  - **2-lane configurations cannot run this test's 1080p60 run.** The Pi 4 Model B connector has 2 lanes [C-01], and so does CM4 CAM0 [C-02]. Official documentation limits 2 lanes to 1080p30 RGB888 or 1080p50 YUV422 [C-37], and the reasoning in [C-48] agrees. Under REQ-CAP-007 these remain candidates for the 2-lane configuration (ADR-004, OPEN); they are tested at the rates their link carries in TEST-CAP-004.
  - The bridge board must route 4 lanes (UNKNOWN, OQ-021), with the `4lane` overlay parameter [B-42].
  - **A 4-lane port is necessary but not shown to be sufficient.** At 972 Mbps the driver uses 3 of the 4 lanes for 1080p60 UYVY (reasoning) [C-47]. Capture with 3 active lanes is unproven (OQ-038; ADR-008).
- Link frequency: overlay default 486000000 (972 Mbps per lane) [A-45]. ADR-008 (PROPOSED; OQ-099) proposes keeping this default on every platform.
  - CM4 CAM1 with a 4-lane board only: ADR-008 also proposes an evaluation run at `link-frequency=297000000` (594 Mbps per lane) [G-12]. Reasoning: at that rate the driver requests all 4 lanes for 1080p60 UYVY [C-47], [C-49], which avoids the 3-of-4-lane case. A driver comment says 594 Mbps is meant for 4-lane 1080p60 [C-44].
  - Not on Pi 5 / CM5. Reasoning: the CFE programs 999 Mbps for this bridge, which matches only the default rate [C-52].
- Capture format: UYVY (`UYVY8_1X16`), following ADR-005 (PROPOSED; OQ-003).
- 1080p60 HDMI source and an EDID that offers 1080p60 (TEST-DRV-002). Sources are ATEM outputs and cameras (REQ-CAP-008): run with each required model that outputs 1080p60 (models OQ-102). *(2026-10-08: OQ-102 ANSWERED — no model list; run with each representative camera and ATEM that outputs 1080p60.)* The ATEM Mini Pro lists 1080p60 among its output standards [F-23]; set its HDMI output to Program first [F-25].

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

**Pi 5 / CM5** — sequence reported by Raspberry Pi engineer 6by9 on kernel 6.18.39 [C-33].

- 6.18.39 is only the kernel of that community report. The bring-up image ships 6.18.50 [G-04], and the driver source inspected in research is the `rpi-6.18.y` tip at 6.18.55 [E-37] (OQ-097).
- `<N>` and the entity name are placeholders (OQ-043).
- The source shows the `-V` line literally for `"csi2":0`; the other two `-V` lines use the same form on the pads the report names.
- The `-d /dev/v4l-subdevN` form of the two `v4l2-ctl` lines is NEEDS VERIFICATION (OQ-101; see Conventions).

```bash
v4l2-ctl -d /dev/v4l-subdevN --set-edid pad=0,file=<pacscorder-edid>          # [B-24]; -d form: OQ-101
v4l2-ctl -d /dev/v4l-subdevN --set-dv-bt-timings query                       # [C-33], [C-37]; -d form: OQ-101
media-ctl -d <N> -l '"csi2":4 -> "rp1-cfe-csi2_ch0":0 [1]'                   # [C-33]
media-ctl -d <N> -V '"tc358743 1x-000f":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'   # [C-33]
media-ctl -d <N> -V '"csi2":0 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'               # [C-33]
media-ctl -d <N> -V '"csi2":4 [fmt:UYVY8_1X16/1920x1080 field:none colorspace:smpte170m]'               # [C-33]
```

- Then set the video-node format to `pixelformat=UYVY` [C-33]. The full `v4l2-ctl` option string is NEEDS VERIFICATION (OQ-101).
- Then stream and count frames. The streaming command is NEEDS VERIFICATION (OQ-101).

**CM4 (CAM1, Media Controller mode)**

- EDID and DV timings: same as above, on the sub-device node [B-25], [C-36].
- Pad format on the TC358743 and the `unicam-image` format: no register fact gives the Unicam Media Controller command sequence, so these are NEEDS VERIFICATION.

**All platforms — measurements**

- Delivered frame rate, from buffer timestamps.
- Dropped and repeated frames.
- Frame integrity: visual inspection of a known test pattern, and a byte check of at least one frame.
- Kernel log during streaming.
- CSI-2 receive errors. How to read them is UNKNOWN on both receivers: RP1 CFE is OQ-050, Unicam (Pi 4/CM4) is OQ-095.
- Duration: UNDEFINED (OQ-010, OQ-017).

### Expected Result

- **Lane count** (reasoning): the driver requests 3 lanes for 1080p60 UYVY at 972 Mbps [C-47], [B-33].
  - Both receivers accept fewer lanes than configured, such as 3 of 4 [C-16].
  - Whether frames are captured correctly with 3 active lanes is UNKNOWN (OQ-038).
  - In the ADR-008 evaluation run on CM4 CAM1 at `link-frequency=297000000`, the driver is expected to request 4 lanes (reasoning) [C-47], [C-49]. Which rate to keep is OQ-099.
- **No** `Device has requested N data lanes, which is >M configured in DT` [B-32], [C-16].
- **Pi 5 / CM5:** `Unable to determine sensor link rate, using 999 Mbps` is **expected**. The CFE always takes this fallback for TC358743 (kernel source [C-31]; reasoning [B-49]).
  - Reasoning: 999 Mbps matches the default 972 Mbps link [C-52].
  - Reception reliability at this setting is UNKNOWN (OQ-050).
- Video-node pixel format UYVY (both receivers map `UYVY8_1X16` to `V4L2_PIX_FMT_UYVY`) [C-34]. Each frame is 4,147,200 bytes (reasoning, CORRECTED) [C-53].
- Colourimetry: BT.601 limited range, reported as SMPTE170M [B-34] (OQ-041).
- Frame rate: 60 frames/s from a 60 Hz source. The driver cannot distinguish 59.94 Hz from 60 Hz [B-28].
- No visible or byte-level corruption.
  - An open issue reports corruption at 1080p50 RGB888 on 3 of 4 lanes on CM4. A Raspberry Pi engineer attributed it to the hard-coded FIFO level of 374 [C-43], [B-12] (RISK-006).
  - The driver's continuous-clock behaviour versus the overlay's `clock-noncontinuous` setting may matter [B-37] (OQ-037).
- If RGB888 is used instead of UYVY:
  - Pi 5 / CM5: CFE maps `RGB888_1X24` to `BGR24` [C-34]; set `BGR3` on the video node, as in the reported Pi 5 sequence [C-33].
  - Pi 4 / CM4: Unicam labels it `RGB24` [C-34], but a B,G,R memory order was reported on CM4 [C-35] (RISK-016, OQ-045).
- Thresholds for duration and allowed drops: UNDEFINED (OQ-010, OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-003 — Source connect / disconnect / mode change

| | |
|---|---|
| Verifies | REQ-CAP-004 |
| Related risks | RISK-012, RISK-013 |
| Related open questions | OQ-020, OQ-039, OQ-051, OQ-090, OQ-093 |
| Depends on | TEST-CAP-002 (or a lower-rate capture on a 2-lane platform) |

### Objective

Confirm that HDMI connect, disconnect, signal loss and timing changes are detected, and that capture resumes with the new timings without a reboot. Measure detection latency.

### Setup

- As TEST-CAP-002 (or TEST-CAP-001 on a 2-lane platform at a mode the link carries).
- A source that can change output mode on command.
- Access to unplug the HDMI cable.
- The application that reacts to events and reconfigures the pipeline does not exist yet: NOT STARTED. Its process model is OQ-090; when it rewrites the EDID is OQ-093.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Subscribe to `V4L2_EVENT_SOURCE_CHANGE` on the TC358743 sub-device node `/dev/v4l-subdevN`, not on `/dev/videoN`, as a Raspberry Pi engineer advised [B-38], [C-42]. The `v4l2-ctl` option to wait for an event is NEEDS VERIFICATION (OQ-101).
2. While streaming:
   - (a) unplug HDMI;
   - (b) re-plug;
   - (c) change the source's output mode;
   - (d) power-cycle the source.
3. After each event, re-run the detection and configuration steps of TEST-CAP-002 (`--set-dv-bt-timings query`, pad formats, video-node format) and restart streaming.
4. Measure the time from the physical change to event delivery.
5. Optional recovery check (OQ-039, OQ-093): unload and reload the driver module. The command is NEEDS VERIFICATION.

### Expected Result

- On a sync change, or a DE size/position change with a stable signal, the driver mutes the stream if the signal is lost or the timings differ. It sends `V4L2_EVENT_SOURCE_CHANGE` (`V4L2_EVENT_SRC_CH_RESOLUTION`) when a sub-device node exists [B-39].
- **The driver does not apply the new timings itself** [B-39]. Capture resumes only after userspace re-queries and reconfigures (REQ-CAP-004).
- On +5V loss:
  - the driver drops HPD, zeroes the stored timings and updates its controls [B-23];
  - when +5V returns and an EDID is stored, HPD is raised again [B-23], [B-21].
- Latency: up to about 1 s. Without a wired interrupt, the driver polls every 1000 ms [A-30], [B-19] (RISK-013). INT wiring is UNKNOWN (OQ-020).
- Pi 5 / CM5: an open issue reports that the CFE video nodes do not deliver this event; a Raspberry Pi engineer said applications should subscribe on the source sub-device [C-42] (OQ-051).
- Driver removal does not drop HPD, assert reset or disable the reference clock [B-40]. Whether a module reload recovers capture is UNKNOWN (OQ-039).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-CAP-004 — Unsupported-mode rejection and supported-mode matrix

| | |
|---|---|
| Verifies | REQ-CAP-005, REQ-CAP-007, REQ-CAP-008 |
| Related risks | RISK-001, RISK-006, RISK-008, RISK-009, RISK-011 |
| Related open questions | OQ-002, OQ-021, OQ-030, OQ-035, OQ-038, OQ-040, OQ-050, OQ-083, OQ-095, OQ-099, OQ-102 |
| Depends on | TEST-CAP-001; TEST-CAP-002 on the 4-lane configuration (its 1080p60 run does not apply to the 2-lane configuration) |

### Objective

Establish, for each platform and lane configuration, which input modes capture without corruption, and confirm that unsupported modes are reported rather than streamed corrupted.

The resulting supported-mode matrix is also the evidence for two DRAFT requirements from the owner decisions of 2026-10-07:

- REQ-CAP-007: both the 2-lane and the 4-lane configuration capture every frame rate their link carries. The matrix must be completed for **both** configurations. Its acceptance criteria are the supported-mode matrix per lane count, which the owner has not yet defined (OQ-002).
- REQ-CAP-008: HDMI sources are ATEM switcher outputs and cameras connected directly. The matrix must be completed for **both** source types, with every model the owner lists (OQ-102). *(Superseded 2026-10-08: OQ-102 ANSWERED 2026-10-07 — "Any HDMI camera (generic)", no model list. The matrix is completed with representative cameras and an ATEM, and it, with the EDID (OQ-002), defines what PACSCORDER accepts.)*

Unsupported modes include:

- interlaced input;
- pixel clock above 165 MHz;
- modes that need more lanes than are wired. The receivers compare the bridge's lane request with the Device Tree `data-lanes` value, not with the physical wiring [B-32], [C-16], so the DT value must match the routed lanes (OQ-021; REQ-CAP-005 note).

### Setup

- Both lane configurations (REQ-CAP-007), on every platform still under consideration for each:
  - 2-lane: Pi 4 Model B [C-01], CM4 CAM0 [C-02], or a 2-lane bridge board on any port (board lane count OQ-021);
  - 4-lane: CM4 CAM1 [C-02], Pi 5 [C-04], CM5 [C-05].
  - The DT `data-lanes` value must match the lanes the board routes: setting `4lane` on a 2-lane Unicam port does not fail the probe [C-17], and the STREAMON check compares against the DT value only (see Objective).
- Both source types (REQ-CAP-008):
  - **ATEM switcher HDMI output**, set to Program [F-25]. The ATEM Mini Pro outputs 1080p23.98, 24, 25, 29.97, 30, 50, 59.94 and 60 only, with no 720p or 1080i [F-23], so for that model only the 1080p rows apply. Whether the ATEM honours the sink EDID or always outputs its own video standard is UNKNOWN (OQ-083). Other ATEM models: OQ-102.
  - **Cameras connected directly.** Output modes, interlaced output and HDCP behaviour are UNKNOWN — VERIFICATION REQUIRED per model (OQ-102). Record what each camera actually outputs.
- Sources able to output:
  - each mode in the matrix below;
  - an interlaced mode (for example 1080i);
  - a mode outside 640–1920 × 350–1200 or 13–165 MHz.
- For the negative cases, the EDID may have to advertise the mode, or the source must force it. EDID content is OQ-002. Reasoning: because the two configurations carry different modes [C-48], [C-49], the EDID may have to differ per lane configuration (OQ-002).

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

- For each lane configuration (2-lane and 4-lane), each source (ATEM output and each listed camera) and each mode and format: run the TEST-CAP-001 detection step, then the TEST-CAP-002 configuration and streaming steps. Record:
  - the lane configuration, platform, connector and source model;
  - the return code of each step;
  - the kernel log;
  - the lane count the driver chose;
  - frame integrity;
  - CSI-2 receive errors, where a method exists (RP1 CFE: OQ-050; Unicam: OQ-095).
- Run the matrix with the default link frequency (486000000), which ADR-008 (PROPOSED; OQ-099) proposes to keep. Repeat it with `link-frequency=297000000` [G-12] only if the owner wants that rate evaluated (OQ-099); ADR-008 proposes evaluating it only on CM4 CAM1 with 4 lanes, in TEST-CAP-002.

### Expected Result

**Lane matrix at the default 486 MHz link frequency (972 Mbps per lane).** Lane counts are reasoning from the driver formula [C-47]; feasibility percentages are reasoning [C-48], [C-49].

| Mode | Format | Lanes requested | 2-lane link | 4-lane link |
|---|---|---|---|---|
| 1080p60 | UYVY | 3 | rejected at STREAMON [B-32] | fits by bandwidth [C-49]; capture with 3 of 4 lanes active is unproven (OQ-038; ADR-008) |
| 1080p60 | RGB888 | 4 | rejected | fits, 76.8 % of link [C-49] |
| 1080p50 | UYVY | 2 | fits, 85.3 % [C-48]; per-line check leaves about 2.0 µs slack before LP/HS overhead, and 972 Mbps is about 7.6 % above the 898.12 Mbps/lane minimum from Toshiba's spreadsheet as read by a Raspberry Pi engineer (reasoning) [C-50]. Reasoning: the per-active-lane load equals that of the reported-corrupt 3-lane 1080p50 RGB888 case (RISK-006) | fits, but still on 2 lanes at 85.3 % (reasoning) [C-49], [C-48] |
| 1080p50 | RGB888 | 3 | rejected [C-48] | fits by bandwidth; corruption reported on CM4 [C-43] (RISK-006) |
| 1080p30 | UYVY | 2 | fits, 51.2 % [C-48] | fits |
| 1080p30 | RGB888 | 2 | fits, 76.8 % [C-48] | fits |

**Other rates required by REQ-CAP-007 ("all frame rates" each link carries).** None of these has a lane count in the source register; record the lane count the driver chooses (HARDWARE TEST REQUIRED).

- 1080p23.98, 1080p24, 1080p25 (reasoning): the driver's payload is active pixels × frame rate × bits per pixel [C-46], so at the same size and format these carry less payload than 1080p30. They are expected to need no more lanes than 1080p30 (2 at 972 Mbps) [C-47], and therefore to fit on both configurations.
- 1080p29.97 and 1080p59.94: the driver reports fractional rates as integer rates, so these are reported as 30 and 60 fps [B-28] (OQ-040). Reasoning: they are expected to request the same lanes as 1080p30 and 1080p60. On the 2-lane configuration 1080p59.94 is therefore expected to be rejected in both formats, as 1080p60 is [C-48].
- 720p60 (reasoning): 1 lane in UYVY and 2 in RGB888 at 972 Mbps [B-33], so it is within both configurations' lane count. The ATEM Mini Pro has no 720p output [F-23]; this row applies to cameras only (OQ-102).
- Thus "all frame rates" on the 2-lane configuration means, for 1920x1080, at most 1080p50 in UYVY and 1080p30 in RGB888 [C-37], [C-48]; on the 4-lane configuration every 1080p rate up to 60 fits by bandwidth [C-49], with the 3-of-4-lane caveat for 1080p60 UYVY (OQ-038).

**At `link-frequency=297000000` (594 Mbps per lane)**

- Lanes requested (reasoning) [C-47]:
  - 1080p60: UYVY 4, RGB888 6;
  - 1080p50: UYVY 3, RGB888 5;
  - 1080p30: UYVY 2, RGB888 3.
- Reasoning: on 4 lanes, 1080p50 RGB888 and 1080p60 RGB888 are rejected [C-49]. An issue reporter saw `Device has requested 5 data lanes, which is >4 configured in DT` [C-43].
- Reasoning: on 2 lanes only 1080p30 UYVY fits [C-48].
- On Pi 5 / CM5 this rate does not match the CFE's 999 Mbps D-PHY setting (reasoning) [C-52] (RISK-011, OQ-050). This is one reason ADR-008 (PROPOSED; OQ-099) proposes keeping the default rate.

**Rejections**

- The driver does not compare the lanes it needs with the DT `data-lanes` [B-30], [C-15]. A mode needing more lanes than configured fails only at STREAMON, with `-EINVAL` and `Device has requested N data lanes, which is >M configured in DT` [B-32], [C-16].
- Interlaced input returns `-ERANGE` from `query_dv_timings` and `s_dv_timings`, because the capability lacks the interlaced flag (reasoning from source) [B-27] (RISK-009).
- Timings outside the capability, including a pixel clock above 165 MHz, return `-ERANGE` [A-08], [B-29], [C-18].

**Per source type (REQ-CAP-008)**

- ATEM output: only progressive 1080p standards are expected [F-23]. Reasoning from that list and the matrix above: on the 2-lane configuration its 1080p59.94 and 1080p60 standards cannot be captured, and 1080p50 only in UYVY [C-37], [C-48]. What reaches the TC358743 (pixel encoding, quantisation range, HDCP) is UNKNOWN (OQ-083).
- Cameras: an interlaced output mode returns `-ERANGE` [B-27] (RISK-009). A source that enforces HDCP is expected to give no usable video, because the driver disables HDCP [A-04] (RISK-008). Every other camera result is UNKNOWN until observed (OQ-102).

**Product behaviour (REQ-CAP-005)**

The application must report the mode as unsupported and must not stream corrupted video. The application is NOT STARTED. Thresholds for "corrupted" are UNDEFINED (OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-AUD-001 — HDMI audio capture over I2S

| | |
|---|---|
| Verifies | REQ-CAP-006 |
| Related risks | RISK-014; added 2026-10-08: RISK-023 (sample-rate mismatch), RISK-024 (A/V synchronisation) |
| Related open questions | OQ-004 (ANSWERED 2026-10-07), OQ-024, OQ-025, OQ-033, OQ-054, OQ-063; added 2026-10-08: OQ-052, OQ-101, OQ-110, OQ-111, OQ-112, OQ-114 |
| Depends on | TEST-DRV-002; owner decision that audio is required (OQ-004). *(2026-10-08: that decision was made on 2026-10-07 — audio is required — so this dependency is met.)* Step 9 (A/V offset) also needs TEST-CAP-002, or a lower-rate capture on the 2-lane configuration, on the same board. |

### Objective

Capture the HDMI source's embedded audio from the TC358743 I2S output into an ALSA device on the Pi.

The silicon can also send audio over CSI-2 [A-05]. The Linux driver always configures 2-channel I2S output instead [A-13], so this test covers the I2S path only. Reasoning: the `tc358743-audio` overlay expects the I2S signals on Pi GPIO 18–20 [A-47], [G-14], not on the camera connector, so the bridge board needs separate audio wiring to the Pi (OQ-025).

*(Added 2026-10-08.)* HDMI audio is required in recordings and streams (REQ-CAP-006, DRAFT; owner, 2026-10-07). The test is run on **both bring-up boards, CM4 and CM5** (ADR-004, OPEN). Besides capture itself, it establishes:

- on CM5, whether the overlay binds and captures at all. No source shows audio captured through this path on CM5 (research gap, topic I; OQ-054); reasoning from ADR-004's analysis: this result gates CM5 as a product platform while audio is required;
- that the source sample rate can be read from the driver and that a capture opened at the wrong rate is detected, because ALSA does not report it (RISK-023, OQ-111);
- what the I2S output carries for compressed, multichannel and 24-bit sources (OQ-110);
- the audio/video offset and drift (RISK-024, OQ-112).

### Setup

- **Precondition:** OQ-004 answered "audio required". Otherwise this test does not apply; REQ-CAP-006 is PROPOSED. *(Superseded 2026-10-08: OQ-004 was ANSWERED on 2026-10-07 — "Yes, audio required" — and REQ-CAP-006 is now DRAFT. The test applies on every board.)*
- Wiring:
  - The bridge board must bring out the I2S pins. That is UNKNOWN (VENDOR CONFIRMATION REQUIRED, OQ-025). The audio pins are outputs in the VDDIO2 domain (1.8 V or 3.3 V) [A-12]; the board's VDDIO2 level is UNKNOWN (OQ-024).
  - Wiring expected by the `tc358743-audio` overlay: LRCK/WFS → GPIO 19, BCK/SCK → GPIO 18, DATA/SD → GPIO 20 [A-47], [G-14].
  - *(Added 2026-10-08.)* The TC358743 has four audio pins, all outputs: A_SCK, A_WFS, A_SD and the oversampling clock A_OSCK, powered from VDDIO2, which is rated 1.8–3.3 V [I-27]. The CM4 and CM5 IO Boards have a selectable 1.8 V or 3.3 V GPIO voltage; VDDIO2 should match the selected voltage, or the audio lines need level shifting [I-29] (OQ-024). **Record the IO board's GPIO voltage setting and the bridge board's VDDIO2 level before power-up.** Whether A_OSCK must be connected is unknown (research open question, topic I; OQ-025).
  - *(Added 2026-10-08.)* Clock roles: the public datasheet makes the TC358743 the I2S clock master only, with 32-bit time slots [I-25]. The overlay makes the codec link bit-clock and frame master and binds the Pi through `i2s_clk_consumer` [I-03]. Reasoning: the two agree, with 64 bit clocks per frame [I-28]. Reasoning from [I-03] and the overlay wiring [A-47]: the Pi therefore receives BCK and LRCK from the bridge, on GPIO 18 and 19.
- `config.txt`: `dtoverlay=tc358743-audio` in addition to the `tc358743` line [C-37], [G-14].
  - The overlay README refers to a `tc358743-fast` overlay that has no README entry [B-46]. *(2026-10-08: that is a stale name — only `tc358743`, `tc358743-audio` and `tc358743-pi5` are built [I-04]. Use the platform's `tc358743` line from TEST-DRV-001.)* A Raspberry Pi engineer reported that `tc358743-audio` requires `tc358743` to be loaded as well, because the bridge must be configured (community source) [I-16].
  - Behaviour on Pi 5 / CM5 is UNKNOWN (OQ-054). *(2026-10-08: on CM5, `overlay_map` has no `tc358743-audio` entry, so `dtoverlay=tc358743` loads `tc358743-pi5` and `tc358743-audio` loads under its own name [I-05]; the firmware does not block it [I-06]; its labels resolve to RP1 I2S1 on GPIO 18–21 [I-07], [I-08]. Operation on CM5 is still unconfirmed, OQ-054.)*
  - *(Added 2026-10-08.)* The kernel options this path needs are set in both arm64 defconfigs and in both packaged 6.18.50 kernels [I-12].
- *(Added 2026-10-08.)* **GPIO 18–21 allocation (OQ-114).** With `tc358743-audio` enabled, the I2S pin group claims GPIO 18, 19, 20 and 21, although the audio path uses only 18–20 [I-30]. Remove or move any other overlay that uses these pins: `pwm` and `pwm-2chan` default to pin 18, `gpio-ir` defaults to GPIO 18, and `audremap` offers `pins_18_19` on BCM2711 [I-31]. Record every `dtoverlay` line in the result entry. See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) §8.3.
- *(Added 2026-10-08.)* **Platforms.** CM4 and CM5, side by side (owner, 2026-10-07; ADR-004 OPEN). For CM5, record the carrier board (OQ-052).
- *(Added 2026-10-08.)* **Control node.** The TC358743 audio controls appear on `/dev/videoN` on CM4 with legacy Unicam (the stock overlay default), and only on the TC358743 sub-device node on CM5 [I-23]. In Media Controller mode on CM4 (ADR-006, PROPOSED), reasoning from [I-23]: they are expected on the sub-device node only; record where they appear.
- An HDMI source sending 2-channel PCM audio at a known sample rate.
- *(Added 2026-10-08.)* **Sources.** Representative cameras and an ATEM (OQ-102 ANSWERED: any HDMI camera, no model list, plus ATEM outputs). ATEM Mini Pro outputs its program audio embedded on HDMI (CORRECTED) [F-24]. What audio format and rate each source sends is UNKNOWN per model: record it. Needed:
  - a 2-channel LPCM source at **48 kHz** and one at **44.1 kHz** (the driver's rate table decodes both [I-19]);
  - a source that can change its audio sample rate, or switch programmes, while running;
  - if available: a compressed (AC-3 or DTS bitstream) source, a multichannel LPCM source and a 24-bit LPCM source (OQ-110);
  - for step 9: a source with a simultaneous visual and audible marker (clapper or flash-and-beep, as named in OQ-112).
- *(Added 2026-10-08.)* **EDID audio.** The EDID's audio descriptors control what sources send; the EDID content is OQ-002. Research proposes advertising 2-channel LPCM only (research design risk, topic I — not a register fact; OQ-110). Record the EDID used.
- *(Added 2026-10-08.)* **Software.** The ALSA capture tool and, for step 9, the media framework (ADR-007, OPEN). Packages: see the common setup. Whether `arecord` is in the Lite image is NEEDS VERIFICATION.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

*Original steps of 2026-10-06, kept for the record (Rule 21). Superseded on 2026-10-08 by the full procedure below, which covers them.*

1. Boot. Confirm that an ALSA card is registered (command NEEDS VERIFICATION).
2. Read the "Audio present" and "Audio sampling rate" controls on the TC358743 sub-device [B-16] (command NEEDS VERIFICATION).
3. Record audio from the ALSA card (command NEEDS VERIFICATION). Compare with the source audio.
4. Change the source sample rate and repeat step 2.

**Full procedure (2026-10-08).** Run every step on CM4 and on CM5. Record the board, carrier board, kernel version, `config.txt` lines, GPIO voltage, VDDIO2 level, source model and source audio format in each result entry.

1. **Boot and kernel log.** Boot with the platform's `tc358743` line plus `dtoverlay=tc358743-audio`. Inspect the kernel log for the sound card and for errors from the CPU DAI driver (`bcm2835-i2s` on CM4, `dwc-i2s` on CM5) and the sound card driver. Record the exact lines.
2. **Card.** List the ALSA capture devices. Confirm a card with id `tc358743`, and record its card index and PCM name. Command NEEDS VERIFICATION (OQ-101); research suggested `arecord -l` (research open question, topic I — not a register fact).
3. **Hardware parameters.** Dump the capture hardware parameters (channels, sample formats, rate range) of `hw:CARD=tc358743,DEV=0` [I-15]. Command NEEDS VERIFICATION (OQ-101); research suggested `arecord --dump-hw-params -D hw:CARD=tc358743` (research open question, topic I — not a register fact). On CM5 this records the RP1 I2S1 channel count and formats that source inspection could not determine (OQ-054).
4. **Sample-rate and audio-present controls.** With the EDID written (TEST-DRV-002) and the source sending audio, read "Audio present" (ID 0x00981981) and "Audio sampling rate" (ID 0x00981980) [I-20] on the node that carries them (see Setup, control node). The `v4l2-ctl` option is NEEDS VERIFICATION (OQ-101). A 2019 forum thread showed them listed as `audio_sampling_rate 0x00981980` with value 48000 and `audio_present 0x00981981` with value 1 (community source) [I-16]. Repeat with the HDMI cable unplugged.
5. **Capture at the source rate.** Capture 2 channels at the rate read in step 4, while the source plays a known reference (a steady tone of known frequency and a timed marker). A capture command was reported in the 2019 thread, on a Pi with `bcm2835-i2s` and a 2019 kernel (community source) [I-16]:
   ```bash
   arecord -vv -d 20 -r 48000 -c 2 -f dat -t wav -D sysdefault:CARD=tc358743 out.wav   # [I-16]
   ```
   - Replace `48000` with the rate read in step 4. The meaning of the other options is not in the source register: NEEDS VERIFICATION (OQ-101).
   - Compare the recording's pitch, duration and content with the source.
   - Repeat in a 32-bit sample format (CM4's `bcm2835-i2s` accepts S32_LE [I-13]; on CM5 `hw_params` accepts S32_LE, but the formats RP1 I2S1 offers come from its hardware registers, so use what step 3 reports [I-14]), to see where 16- to 24-bit HDMI samples sit in the 32-bit slots (OQ-110). Command NEEDS VERIFICATION (OQ-101).
6. **Rate-mismatch check (RISK-023).** With the 44.1 kHz source, capture once with the device opened at 44100 Hz and once at 48000 Hz. With the 48 kHz source, capture once at 48000 Hz and once at 44100 Hz. For each recording, measure the tone frequency and the recording's duration against a reference clock, and record whether ALSA reported any error.
7. **Rate change during capture (OQ-111).** Subscribe to `V4L2_EVENT_CTRL` for the sampling-rate control on the node from step 4 [I-21]; the `v4l2-ctl` event option is NEEDS VERIFICATION (OQ-101). While capturing, switch the source between 48 kHz and 44.1 kHz, and between programmes. Record:
   - the time from the switch to the event and to the new control value;
   - what the capture contains during the switch (silence, glitch, clean re-lock);
   - whether "Audio present" changes.

   The PACSCORDER application that reopens ALSA at the new rate is NOT STARTED. Until it exists, reopen the capture by hand and record the gap.
8. **Formats and channels (OQ-110).** Where such sources are available, send (a) compressed audio (AC-3 or DTS bitstream), (b) multichannel LPCM and (c) 24-bit LPCM. Record the control values and what is captured in each case.
9. **A/V offset (OQ-112, RISK-024).** With video captured as in TEST-CAP-002 (or a lower-rate capture on the 2-lane configuration) and audio captured in the same pipeline, use the marker source. Measure the audio/video offset at the start and after a long run, once with GStreamer and once with FFmpeg (ADR-007, OPEN). Record the clock settings used:
   - GStreamer `alsasrc`: `provide-clock`, `use-driver-timestamps` and `slave-method` [I-35];
   - FFmpeg V4L2 input: the timestamp option (default, `-ts abs` or `mono2abs`) [I-38].

   Full pipelines are NEEDS VERIFICATION (OQ-101). The run length is UNDEFINED (OQ-010, OQ-112). The multi-hour drift check is part of TEST-PERF-001.

### Expected Result

- An ALSA card named `tc358743` (the default `card-name`) [A-47], [G-14].
- The driver always configures 2-channel I2S output [A-13], although the chip could also send audio over CSI-2 [A-05]. I2S on the TC358743 is controller (master) clock mode only, with 32-bit slots [A-11]. The overlay makes the Pi the clock consumer with two 32-bit slots [B-46].
- "Audio present" reads 1 while the source sends audio. "Audio sampling rate" (read-only, 0–768000) reports the source rate [B-16].
- Audio/video synchronisation tolerance: UNDEFINED (OQ-004). *(2026-10-08: OQ-004 is ANSWERED for "audio required" but the tolerance was not set; it is now tracked in OQ-112.)*

*Added 2026-10-08 for the full procedure. These are source predictions, not acceptance criteria (OQ-017).*

**Card and device (both boards; steps 1–3)**

- The card id is `tc358743`, taken from the sound card name, so the device can be opened as `hw:CARD=tc358743,DEV=0` whatever card index is assigned [I-15]. The overlay names the card `tc358743`, makes the codec link bit-clock and frame master, and gives the CPU DAI two 32-bit slots [I-03].
- The overlay's codec is a `linux,spdif-dir` stub, because the TC358743 has no ASoC codec driver of its own [I-02]. The stub is capture only, accepts 8–768 kHz and has no ALSA controls [I-11].

**CM4**

- PCM name `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15]. The card index may differ between systems: the same 2019 forum thread reported it as card 0 and as card 1 (community source) [I-16].
- Hardware parameters: exactly 2 channels; S16_LE, S24_LE or S32_LE; any rate from 8 to 384 kHz [I-13].
- In this clock-consumer mode the rate the application requests never reaches the hardware; the rate on the wire is set by the TC358743 [I-13].

**CM5 — bring-up gate**

- The firmware does not block the overlay [I-05], [I-06]. Its labels resolve to RP1 I2S1 on GPIO 18–21 [I-07], [I-08].
- `dwc-i2s` accepts the codec-master (BC_FC) format only when the hardware reports the instance as a clock consumer, and returns `-EINVAL` for mixed formats. The RP1 datasheet calls I2S1 the clock-consumer instance, and the overlay uses I2S1 [I-09].
- If the card registers: reasoning from source, the PCM name is `1f000a4000.i2s-dir-hifi dir-hifi-0` [I-17]. `hw_params` accepts only 2, 4, 6 or 8 channels and S16/S24/S32_LE; the maximum channel count and the formats come from RP1 hardware registers, so record what step 3 reports [I-14].
- Whether the card registers and captures at all is **UNKNOWN — HARDWARE TEST REQUIRED** (OQ-054). No official statement or test result shows audio captured through this path (research gap, topic I). Reasoning from ADR-004's analysis: a failure here blocks CM5 as the product platform while audio is required.

**Controls (step 4)**

- "Audio sampling rate" is ID 0x00981980, a read-only integer 0–768000; "Audio present" is ID 0x00981981, a read-only boolean [I-20].
- The driver decodes the rate from register FS_SET through a fixed table that includes 32000, 44100, 48000, 88200, 96000, 176400 and 192000 Hz. It returns 0 when there is no TMDS signal, because FS_SET is not cleared on disconnect [I-19]. Reasoning: the rate therefore reads 0 with the cable unplugged.
- The driver updates the rate control from its CBIT interrupt status, and those interrupts are unmasked only while +5V / cable is detected [I-21].

**Capture and rate mismatch (steps 5 and 6; RISK-023)**

- At the source rate: correct pitch and duration.
- Reasoning from source [I-18]: no kernel path carries the HDMI rate into ALSA. Opening the card at 48000 Hz while the source sends 44100 Hz gives **no ALSA error**; the recording plays 48000/44100 = 1.0884 times too fast (+8.84 %, about +1.47 semitones), and its timeline is 0.919 of real time, so A/V drift accumulates.
- Reasoning (Claude; the same arithmetic as [I-18], reversed): opening at 44100 Hz while the source sends 48000 Hz gives the opposite error — playback at 0.919 times speed, and a timeline 1.088 times real time.

**Rate change (step 7; OQ-111)**

- The control changes and a `V4L2_EVENT_CTRL` event is delivered; the driver accepts that subscription [I-21].
- Without an `interrupts` property the driver polls every 1000 ms, and the packaged kernels have no TC358743 CEC support, so a rate change can take up to about 1 s, plus I2C time, to reach the control [I-22] (OQ-020).
- The driver programs a 500 ms `BUFINIT_START` and a 100 ms `DIV_MODE` delay at probe [I-24]. What the I2S output does during a rate change is UNKNOWN (research open question, topic I; OQ-111).

**Formats and channels (steps 5 and 8; OQ-110)**

- Stereo only: the driver hard-codes 2-channel I2S at probe and never selects its TDM, CSI or 4/6/8-channel settings [I-24].
- Datasheet: the I2S output is one stereo data lane carrying 16/18/20/24-bit data that "depend on HDMI input stream", in 32-bit time slots only [I-25].
- What the I2S output carries for compressed or multichannel input, and where 24-bit samples sit in the 32-bit slots, is UNKNOWN (OQ-110).

**A/V offset (step 9; OQ-112, RISK-024)**

- Audio and video run on separate clocks. The TC358743's internal audio PLL tracks the source's N/CTS values, so the I2S clocks follow the source's audio clock [I-26]. Every Raspberry Pi CSI receiver driver in `rpi-6.18.y` stamps video buffers with `CLOCK_MONOTONIC` at frame start [I-33].
- GStreamer 1.26.2: in a `v4l2src` + `alsasrc` pipeline, `alsasrc`'s audio clock normally becomes the pipeline clock, and ALSA driver timestamps are then not used [I-36], [I-35]. After a bad timestamp, `v4l2src` assumes a one-frame delay for the rest of the session [I-37].
- FFmpeg 7.1: the ALSA input uses wall-clock time and the V4L2 input passes monotonic timestamps through by default. Without `-ts abs` or `mono2abs` the clock bases are mixed, and the CLI's default per-input start shift discards the real offset between the inputs [I-38]. Reasoning from [I-38]: a run without one of those options is expected to hide the start offset rather than measure it; record the option used.
- Tolerance: UNDEFINED (OQ-112).

**GPIO (Setup; OQ-114)**

- With `tc358743-audio` enabled, GPIO 18–21 are claimed, including GPIO 21, which the audio path does not use [I-30]. Another overlay on those pins conflicts [I-31]; on CM5 the base Device Tree's power button and fan do not use header GPIO 18–21 [I-32].

**For the output tests**

- Encoder input rates differ: `opusenc` accepts only 48, 24, 16, 12 or 8 kHz, so a 44.1 kHz source needs `audioresample` first; `avenc_aac` and `voaacenc` accept 44.1 and 48 kHz [I-47]. See TEST-STR-002.

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-DMA-001 — DMABUF capture → encoder buffer sharing

| | |
|---|---|
| Verifies | REQ-DMA-001, REQ-ARCH-001 |
| Related risks | RISK-003, RISK-020 |
| Related open questions | OQ-015, OQ-057, OQ-058, OQ-060, OQ-062, OQ-066; added 2026-10-08: OQ-005, OQ-115 (two encoders fed from one capture) |
| Depends on | TEST-CAP-002 |

### Objective

Confirm that captured frames reach the encoder as DMABUF buffers without a CPU copy, where the platform encoder supports DMABUF import. On Pi 5 / CM5, which have no hardware encoder, record the buffer path that is used instead. This covers the "V4L2 → DMABUF → Encoder" part of REQ-ARCH-001.

*(Added 2026-10-08.)* Owner answer to OQ-005, "Separate record + live": one capture now feeds **two** H.264 encoders, the recording encode and the live encode shared by RTMP and WebRTC (REQ-ENC-001). This test therefore also records how one captured frame reaches both encoders (one capture buffer, two consumers), on CM4 and CM5.

### Setup

- As TEST-CAP-002.
- Userspace framework: undecided (ADR-007 OPEN, OQ-015). Both candidates below need packages that are not in the Lite image [G-32]:
  - GStreamer: `gstreamer1.0-plugins-good` contains the V4L2 elements [G-26].
  - FFmpeg: Raspberry Pi OS ships its own FFmpeg build [G-29].
- **Pi 4 / CM4:** encoder `/dev/video11` (`bcm2835-codec-encode`) [D-06], [D-07], [D-08].
- **Pi 5 / CM5:**
  - There is no hardware encoder [D-31], [G-22].
  - Reasoning: `/dev/video11` does not exist there [D-29].
  - The software H.264 encoders researched, GStreamer `x264enc` and FFmpeg `libx264`, do not accept packed UYVY [D-40], [D-43], so a conversion stage is needed (OQ-060).
- *(Added 2026-10-08.)* **H.265 on CM4 and CM5.** H.265 is software-encoded on both boards [D-24], [D-31], and `x265enc` and `libx265` accept planar input only, not UYVY [H-13], [H-10] (CORRECTED). Reasoning: for H.265, zero-copy into a hardware encoder does not apply on CM4 either; record the conversion and copy path as on Pi 5/CM5 (OQ-060). *(Deferred — REQ-ENC-002; not in current scope: H.264 only for now (OQ-103 ANSWERED 2026-10-08), so this H.265 check is not run in current scope.)*
- *(Added later on 2026-10-08.)* **One capture buffer, two consumers (two H.264 encodes, OQ-005).** No register fact describes how a framework hands one capture buffer to two encoders. No register fact says whether `bcm2835-codec` runs two encode sessions at once (OQ-115; KERNEL SOURCE INSPECTION REQUIRED). The framework is ADR-007 (OPEN, OQ-015). Candidate buffer flows are in [DMA.md](DMA.md).

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

**Pi 4 / CM4 with GStreamer**

- Reference pipeline, reported by a Raspberry Pi engineer [D-54]:
  ```bash
  gst-launch-1.0 v4l2src ! "video/x-raw,framerate=30/1,format=UYVY" ! v4l2h264enc extra-controls="controls,h264_profile=4,h264_level=10,video_bitrate=256000;" ...
  ```
- Add `output-io-mode=dmabuf-import` to `v4l2h264enc` [D-53].
- Points to resolve before running:
  - Per the same report, `h264_level=10` is Level 3.2 [D-54]. Reasoning: that is below the Level 4 needed for 1080p [F-40]. The level value to use is NEEDS VERIFICATION.
  - The `v4l2src` device selection and the combined pipeline are NEEDS VERIFICATION.

**Pi 4 / CM4 with FFmpeg (alternative)**

- Raspberry Pi OS's patched FFmpeg imports DMABUF into `h264_v4l2m2m` when the input pixel format is `AV_PIX_FMT_DRM_PRIME` [D-45]. Command NEEDS VERIFICATION.
- Upstream FFmpeg's V4L2 M2M path is MMAP-only and copies every frame [D-44].

**Pi 5 / CM5**

Run capture → conversion → software encoder and record where buffers are copied. The method is NEEDS VERIFICATION (OQ-060).

**All platforms**

- Record whether DMABUF import succeeded.
- Record any import error in the kernel log.
- Record the `bytesperline` of the capture buffers.
- Record the CPU load compared with a copy-based run. The comparison method is NEEDS VERIFICATION.

**Two encoders from one capture — CM4 and CM5 (added later on 2026-10-08; owner: "Separate record + live", OQ-005)**

Pipelines and commands are NEEDS VERIFICATION (OQ-101).

1. **CM4.** Run the recording encode and the live encode on `/dev/video11` at the same time, both importing the same capture buffers. Record:
   - whether the second encode session starts at all (OQ-115);
   - whether both imports succeed;
   - any import error in the kernel log.
2. **CM5.** Record whether the UYVY → planar conversion runs once and feeds both software encoders, or once per encoder. Record the CPU load of each stage (OQ-059, OQ-060).
3. **Both boards.** With both encoders running, record the number of capture buffers in flight and `CmaFree` (OQ-061).
   - Reasoning: a capture buffer read by two consumers can go back to the capture queue only after both have finished with it. The slower encode therefore sets how long each buffer is held.

### Expected Result

**Pi 4 / CM4**

- Both encoder queues support `VB2_DMABUF` through `videobuf2-dma-contig` [D-19].
- An imported buffer must be one contiguous region at least as large as the plane; otherwise import fails with `contiguous chunk is too small` [D-20].
- The encoder uses one memory plane even for planar formats [D-21]. For UYVY it rounds `bytesperline` up to a multiple of 64 bytes [D-22].
- Reasoning, from inputs [C-53] (2 bytes per UYVY pixel) and [D-22]: a 1920-pixel UYVY line is 3840 bytes, a multiple of 64 (OQ-058).
- Unicam allocates capture buffers with `videobuf2-dma-contig` and no IOMMU (reasoning, CORRECTED) [C-53].
- Expected: no `contiguous chunk is too small`, and the encoder consumes the capture buffers.
- `rpicam-apps` uses this zero-copy import into `/dev/video11` for camera buffers [D-34]. That is not evidence for TC358743 buffers.

**Pi 5 / CM5**

- Zero-copy *into the encoder* does not apply, because encoding is in software [G-22].
- Whether CFE buffers come from CMA is not established (reasoning, CORRECTED) [C-53] (OQ-053).
- The DMA-BUF heap names on rpi-6.18.y are `default_cma_region` plus a legacy heap named after the CMA area [E-48] (OQ-062).

**Two encoders from one capture** *(added later on 2026-10-08)*

- CM4: no source-derived expectation exists for two encode sessions importing the same capture buffer: UNKNOWN — HARDWARE TEST REQUIRED (OQ-115). Reasoning from [D-19] and [D-20]: each import has to meet the same contiguity and size condition as a single import.
- CM5: zero-copy into the encoder applies to neither encode, because both are software [G-22].
- CMA use with both encoders running is UNKNOWN until measured (OQ-061).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-ENC-001 — Sustained real-time H.264 encode (H.265 deferred)

| | |
|---|---|
| Verifies | REQ-ENC-001 |
| Related risks | RISK-002, RISK-003, RISK-015; added 2026-10-08: RISK-022 (H.265 software-only; not in current scope — H.265 deferred, REQ-ENC-002); added 2026-10-09: RISK-019 (live-encode B-frames and keyframes, OQ-127), RISK-031 (encode term of the < 1 s WebRTC budget) |
| Related open questions | OQ-001, OQ-005, OQ-011, OQ-015, OQ-041, OQ-048, OQ-056, OQ-057, OQ-059, OQ-060, OQ-096; added 2026-10-08: OQ-103, OQ-104, OQ-105, OQ-101 (OQ-103 ANSWERED 2026-10-08; OQ-104 and OQ-105 not in current scope — H.265 deferred, REQ-ENC-002); added later on 2026-10-08: OQ-115 (two concurrent encodes on CM4); added 2026-10-09: OQ-116 (ANSWERED 2026-10-08: < 1 s for WebRTC viewers only), OQ-125 (measured latency, encode term), OQ-127 (live keyframe interval, on-demand keyframes, B-frames) |
| Depends on | TEST-CAP-002 and TEST-DMA-001 (see Preconditions) |

### Objective

Measure whether each platform can encode the captured video to H.264 in real time for a sustained period, at the frame rates PACSCORDER captures: 1080p60 on the 4-lane configuration (REQ-CAP-001), and every rate each lane configuration carries (REQ-CAP-007; OQ-001 answered by the owner on 2026-10-07).

*(Scope extended 2026-10-08.)* REQ-ENC-001 requires H.264 **and** H.265 (owner, 2026-10-07; OQ-005). This procedure therefore also measures **software H.265 encoding on CM4 and CM5**, the two bring-up boards (ADR-004, OPEN). The canonical test title in [README.md](README.md) still says "H.264" and is not changed here. Which outputs must use H.265, and whether software-only H.265 is acceptable, is OQ-103; this test supplies the measurements for that decision and for OQ-104 and OQ-105.

*(Superseded later on 2026-10-08 by the owner's answer to OQ-103, "H.264 only for now".)* REQ-ENC-001 is **H.264 only**, for all outputs, and H.265 is deferred as REQ-ENC-002 (DEFERRED). In the current scope this test runs **H.264 only**, on CM4 and CM5. The H.265 setup, runs and expected results below are kept as the procedure and evidence for REQ-ENC-002, but they are deferred and not run in current scope. The canonical title is now "Sustained real-time H.264 encode (H.265 deferred)" ([README.md](README.md)).

*(Added later on 2026-10-08.)* Owner answer to OQ-005, "Separate record + live": REQ-ENC-001 requires **two simultaneous H.264 encodes**, a recording encode and one live encode shared by RTMP and WebRTC. This test therefore includes a **two-encode run on CM4 and CM5**:
- on CM4 both encodes share the hardware encoder, whose concurrency is unknown (OQ-115, RISK-002);
- on CM5 both run in software (OQ-059, RISK-003).

Bitrate, rate control and latency are still open (OQ-005). The combined load with audio and all outputs is measured in TEST-PERF-001. *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — the live latency target is under 1 s camera-to-viewer for WebRTC viewers only, and RTMP is best-effort (OQ-116 ANSWERED; REQ-STR-002). Bitrate and rate control are still open (OQ-005). This test records the live encode's per-frame encode time and latency settings, which are one term of that budget (OQ-125); the end-to-end latency is measured in TEST-STR-002.)*

### Setup

- As TEST-DMA-001.
- **Preconditions** (the same as [PERFORMANCE.md](PERFORMANCE.md) §10.3):
  - TEST-CAP-002 has a recorded result of TESTED — PASS for the mode under test. TEST-CAP-002 cannot run on a 2-lane connector; there, as for TEST-CAP-003, a passing lower-rate capture of the mode under test is needed instead.
  - TEST-DMA-001 has been run.
  - The encoder settings are recorded: profile, level (set from the mode; [VIDEO_ENCODER.md](VIDEO_ENCODER.md) §6), bitrate and mode, GOP, and the repeat-header setting.
- Encoding parameters (codec, bitrate, latency target, number of simultaneous encodes): UNDEFINED — OWNER DECISION REQUIRED (OQ-005). *(Partly superseded 2026-10-08: the codec is decided — H.264 and H.265, owner 2026-10-07. Bitrate, latency target and the number of simultaneous encodes are still OQ-005; the H.265 scope per output is OQ-103.)* *(Superseded later on 2026-10-08: the codec is H.264 only — owner, "H.264 only for now" (OQ-103 ANSWERED); H.265 is deferred, REQ-ENC-002. Bitrate, latency target and the number of simultaneous encodes are still OQ-005.)* *(Superseded again later on 2026-10-08: the number of simultaneous encodes is decided. There are two: a recording encode and a live encode shared by RTMP and WebRTC (owner, "Separate record + live"; OQ-005). Bitrate, rate control and latency target are still OQ-005. Record the encoder settings named in the preconditions separately for each encode.)*
- *(Added later on 2026-10-08.)* **Live-encode settings (WebRTC-receivable; REQ-ENC-001).** The live encode feeds WebRTC as well as RTMP, so:
  - **Profile: Constrained Baseline.** RFC 7742 requires WebRTC endpoints to support H.264 Constrained Baseline [F-36]. libwebrtc assumes Constrained Baseline Level 3.1 when `profile-level-id` is absent [F-39]. The CM4 encoder offers Constrained Baseline; its default is High [D-11]. The `h264_profile` value or caps that select Constrained Baseline are NEEDS VERIFICATION (OQ-101). In the pipeline reported by a Raspberry Pi engineer, `h264_profile=4` is High (community source) [D-54].
  - **Level.** Reasoning: 1080p30 needs Level 4 or above, and 1080p60 needs Level 4.2 [F-40]. The CM4 driver calls Level 4.0 the hardware specification [D-12]. Whether browsers decode 1080p when `42e01f` (Level 3.1) was negotiated, or PACSCORDER must signal Level 4.0 or 4.2, is OQ-073.
  - **No B-frames.** Browsers do not accept H.264 B-frames in WebRTC (reported by the MediaMTX project) [F-45]. The CM4 encoder produces none [D-14]. A software configuration can produce them: rpicam-apps' normal-mode `libx264` defaults set `max_b_frames=1` [D-35]. On CM5, set and record the B-frame setting.
  - **In-band SPS/PPS.** WebRTC requires SPS/PPS in-band [F-36]. Repetition is off by default on the CM4 encoder [D-15]; the official pipeline enables it with `repeat_sequence_header=1` [D-37].
  - **CM5.** How to select Constrained Baseline and turn off B-frames on `x264enc` or `libx264` is not in the source register: NEEDS VERIFICATION (OQ-101). *(Superseded in part 2026-10-09: research topic K documents how GStreamer `x264enc` is made B-frame-free (CORRECTED) [K-29]; see the next bullet. Selecting **Constrained** Baseline (the register names `profile=baseline` only) and the FFmpeg `libx264` options are still NEEDS VERIFICATION, OQ-101.)*
- *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* **Live-encode latency and keyframe settings (OQ-127; < 1 s for WebRTC viewers, OQ-116).** Record each of these for the live encode in every run:
  - **CM4 (`bcm2835-codec`).** B-frames are limited to 0, so the encoder never emits them; its defaults are profile High, level 4.0 and a GOP (I-period) of 60 [K-30]. Reasoning from [K-30]: a 60-frame GOP is 2 s at 30 fps. The encoder implements force-keyframe, and GStreamer 1.26 `v4l2videoenc` issues it for frames flagged force-keyframe, so an IDR can be requested on demand, for example when a WebRTC viewer joins [K-31]. Record the GOP used and whether on-demand keyframes are used.
  - **CM5 (x264).** The `zerolatency` tune sets lookahead, sync-lookahead and B-frames to 0 and uses sliced threads [K-27], so x264 holds no frames back and `x264enc` reports a latency of 0 frames [K-28]. By default GStreamer 1.26 `x264enc` runs x264's medium preset (3 B-frames, rc-lookahead 40), and its `bframes=0` property default does not guarantee a B-frame-free stream; set `tune=zerolatency`, set `bframes=0` explicitly, or force `profile=baseline` in caps (CORRECTED) [K-29]. Record the preset, tune, B-frame setting, keyframe interval and thread settings. Whether the target build's output is B-frame-free with the settings used is a BUILD TEST REQUIRED item (research gap, topic K — not a register fact).
  - **Keyframe interval.** YouTube recommends a 2 s keyframe frequency ("Do not exceed 4 seconds") and CBR, and also 2 B-frames, which conflicts with the B-frame-free stream WebRTC needs [K-17], [K-04]. Which interval the shared live encode uses is OQ-127 (RISK-019).
- *(Added later on 2026-10-08.)* **Recording-encode settings.** Profile, level and bitrate are not specified (OQ-005); record what is used. The CM4 encoder's default profile is High [D-11].
- *(Added 2026-10-08.)* **H.265 encoders (CM4 and CM5) (deferred — REQ-ENC-002; not in current scope).** No candidate platform has a hardware HEVC encoder [D-24], [D-31], so H.265 runs in software on both boards.
  - x265: Debian's 4.1-2, unchanged in Raspberry Pi OS [H-01], [H-02]. One `libx265-215` provides Main, Main10 and Main12 [H-03].
  - GStreamer: `x265enc` in `gstreamer1.0-plugins-bad` [H-11], [H-12] (CORRECTED). Properties: `speed-preset` (default `medium`), `tune` (default `ssim`), `bitrate` in kbit/s (default 2048), `key-int-max` and `option-string` [H-14].
  - FFmpeg: `libx265` is built into the Raspberry Pi FFmpeg 7.1.5 [H-08], [H-09].
  - **Input format.** Neither accepts the TC358743's packed UYVY. `x265enc` takes Y444, Y42B or I420 (plus 10/12-bit variants) [H-13]; `libx265` takes planar or gray formats only, including 10/12-bit with Debian's library (CORRECTED) [H-10]. A UYVY → planar conversion stage is needed, as for H.264 on Pi 5/CM5 (OQ-060). Reasoning: on the CPU at 1080p60 it reads about 249 MB/s and writes about 187 MB/s before x265 starts [H-43].
  - **Low latency.** In x265 4.1, `tune=zerolatency` sets B-frames 0, lookahead 0, scenecut off and one frame thread, leaving only wavefront row parallelism [H-16]. FFmpeg's wrapper copies its thread count into x265's frame threads *after* applying preset and tune [H-10] (CORRECTED). Reasoning from [H-10] and [H-16]: set the FFmpeg thread count explicitly and record it, because it overrides zerolatency's single frame thread (research gap, topic H; OQ-104).
  - The `ultrafast` preset's settings are listed in [H-17].
- Pi 4 / CM4:
  - **2-lane connectors.** Pi 4 Model B and CM4 CAM0 have 2 lanes, so 1080p60 cannot be captured from the TC358743 there [C-01], [C-02], [C-48]. On those connectors, which are candidates for the 2-lane configuration of REQ-CAP-007, the highest encode run is 1080p50 UYVY (reasoning) [C-48]. The 1080p60 run applies to CM4 CAM1 only.
  - Do not use the cut-down firmware: `gpu_mem=16` selects `start4cd.elf`, which removes codec support [D-48].
  - The firmware variant and GPU memory needed are OQ-048.
- Pi 5 / CM5: GStreamer `x264enc` is in the GStreamer Ugly plug-ins [D-40]. The Debian package name is NEEDS VERIFICATION.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

**Pi 4 / CM4**

1. List the encoder input formats. A 2021 listing posted by a Raspberry Pi engineer used this command [D-17]:
   ```bash
   v4l2-ctl --list-formats-out -d 11       # [D-17]
   ```
2. Encode with `v4l2h264enc`. Official documentation uses `extra-controls="controls,repeat_sequence_header=1"` with caps `video/x-h264,level=(string)4` [D-37].
   - Reasoning: 1080p60 needs Level 4.2 [F-40], [D-36]. The level string to use is NEEDS VERIFICATION.
   - Run 1080p60 on CM4 CAM1 only; on Pi 4 Model B and CM4 CAM0 start at 1080p50 (see Setup).
3. Optional, informational: repeat with `gpu_freq=550`, the value suggested in the official 720p120 recipe (reasoning entry) [D-52], only if the owner allows overclocking to be evaluated (OQ-056). Whether an overclock is acceptable in the product is OQ-096.

**Pi 5 / CM5**

- Official documentation says to replace the encoder with `x264enc speed-preset=1 threads=1` [D-37].
- Insert a UYVY → I420/NV12 conversion before it [D-40]. The element and its placement are NEEDS VERIFICATION.

**Two-encode run — CM4 and CM5 (added later on 2026-10-08; owner: "Separate record + live", OQ-005)**

Run the recording encode and the live encode at the same time from one capture (see TEST-DMA-001). Set the live encode as in Setup (live-encode settings). Full pipelines are NEEDS VERIFICATION (OQ-101). On Pi 4 Model B and Pi 5, if they are still evaluated (ADR-004, OQ-011), run as on CM4 and CM5 respectively.

1. **CM4: hardware encoder, `/dev/video11`** [D-06].
   - Run both encodes at 1080p30 first. Reasoning: they need 2 × 244,800 = 489,600 macroblocks/s, the rate of one 1080p60 encode and about 2.0× the 1080p30 specification [D-10], [D-52] (OQ-115, RISK-002).
   - Then run both at the capture rate of the configuration under test: 1080p60 on CM4 CAM1, at most 1080p50 on CM4 CAM0 (reasoning [C-48]). Reasoning: two 1080p60 encodes need 979,200 macroblocks/s, about 4.0× the specification (from [D-52]).
   - Record whether the second encode session starts, and the highest combination of resolution and frame rate that both encodes sustain.
   - If they do not sustain the required rates, record that. Which split is acceptable is an owner decision (OQ-115).
2. **CM5: software.** Run two `x264enc` or `libx264` encodes plus the UYVY → planar conversion (OQ-060). Record per-core CPU load and throttling.
   - Reasoning from [G-22]: two encodes need roughly twice the encode CPU of one. The official figure is "~30–40 % CPU" for one 1080p30 encode, and whether that means the whole CPU or one core is UNKNOWN (OQ-059, RISK-003).
3. **Optional, CM4 only.** Repeat with `gpu_freq=550`, as in the Pi 4 / CM4 step 3 above, only if the owner allows an overclock to be evaluated (OQ-056, OQ-096).
4. **Live-encode output check.** The inspection tool is NEEDS VERIFICATION (OQ-101). Check the live encode's output for:
   - profile and level;
   - no B-frames;
   - SPS/PPS with every IDR [D-15].
   - *(Added 2026-10-09; OQ-127.)* the IDR spacing actually produced, against the GOP set (Setup, live-encode latency and keyframe settings); on CM4, that a keyframe requested on demand appears in the output [K-31]; on CM5, that no B-frames appear with the `x264enc` settings used [K-29].

**H.265 runs — CM4 and CM5 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope)**

*Deferred, not run in current scope* (owner, 2026-10-08: "H.264 only for now"; OQ-103 ANSWERED). Kept for when REQ-ENC-002 is re-activated. Run on both boards. Full pipelines and command lines are NEEDS VERIFICATION (OQ-101); the conversion element is NEEDS VERIFICATION (OQ-060).

1. **x265 CPU features (OQ-105).** Record which SIMD paths x265 reports at start-up on each board. Reasoning from [H-04] and [H-05]: CM5 is expected to report Neon DotProd and CM4 baseline Neon only. Research suggested reading this from `x265 --version` or the encoder log, and checking that the CM5 kernel reports `asimddp` (research open question and gap, topic H — not register facts).
2. **GStreamer.** Capture → UYVY-to-planar conversion → `x265enc` with `speed-preset`, `tune=zerolatency`, `bitrate` and `key-int-max` set [H-14] → `h265parse`, which the MP4 and Matroska muxers need after `x265enc` [H-37] → sink.
3. **FFmpeg.** Capture → planar conversion → `-c:v libx265` with an explicit thread count (see Setup, low latency).
4. **Run matrix.** For each framework:
   - 1080p30 and 1080p60 on the 4-lane configuration; on the 2-lane configuration, the rates its link carries (at most 1080p50 UYVY; reasoning [C-48]);
   - presets `ultrafast` and `superfast`;
   - H.265 alone, then H.265 with a concurrent H.264 encode (CM4: hardware `v4l2h264enc`; CM5: `x264enc` or `libx264`), then both with audio encoding from TEST-AUD-001 (OQ-104).
5. **Conversion cost.** Measure the UYVY → planar conversion alone (CPU load, frame rate), so that its share can be separated from the encoder's (OQ-060).
6. **Sustained run.** On CM5, record whether the CPU throttles under sustained all-core load (OQ-104). Duration: UNDEFINED (OQ-010).

**All platforms — measurements**

- Output frame rate against input frame rate.
- Dropped frames.
- Encode latency.
- CPU load per core.
- SoC temperature.
- Duration: UNDEFINED.
  - The research suggested at least 10 minutes (research open question, topic D), and [RISKS.md](RISKS.md) RISK-002 repeats it. The owner has not accepted it (OQ-010, OQ-017).
  - Commands for these measurements are NEEDS VERIFICATION.
- *(Added later on 2026-10-08.)* In the two-encode run, record the frame-rate, dropped-frame and latency measurements for each encode separately (recording and live). Record CPU load and SoC temperature for the two together.
- *(Added 2026-10-09; OQ-125, OQ-116.)* **Live-encode time per frame**, on CM4 and CM5, with the recording encode running. This is the encode term of the < 1 s WebRTC budget (RISK-031):
  - Measure it; do not read it from the pipeline. GStreamer 1.26 `v4l2videoenc` reports min-buffers × frame duration, and on BCM2711 `v4l2h264enc` therefore reports 0 latency [K-32]. Research suggested timing each frame from queue to dequeue on the encoder device, as in the issue reported in [K-33] (community source; the suggestion itself is a research open question, topic K — not a register fact); the method is NEEDS VERIFICATION (OQ-101).
  - CM5: record whether the per-frame x264 time stays below one frame period. Reasoning from [K-36]: otherwise filled buffers wait in the V4L2 FIFO queues and each adds one frame period, 33.3 ms at 30p (OQ-059, RISK-032).

### Expected Result

**Pi 4 / CM4**

- UYVY appears among the encoder input formats. It did in the 2021 listing posted by a Raspberry Pi engineer [D-17], but the list is read from the firmware at probe [D-16] (OQ-057).
- A Raspberry Pi engineer reported that both TC358743 formats are accepted directly [D-18].
- 1080p30 is within the official specification [D-10].
- **1080p60 is unproven** (RISK-002, OQ-056), and applies to CM4 CAM1 only (2-lane connectors cannot capture it [C-01], [C-02], [C-48]):
  - the driver calls Level 4.0 the hardware specification and says higher levels may not keep up in real time [D-12];
  - reasoning: 1080p60 needs 2.0x the specified macroblock rate [D-52];
  - one Raspberry Pi engineer (6by9) reported 1080p60 hardware encode as an "edge case" (community report) [D-50];
  - the official 720p120 recipe suggests a GPU overclock, `gpu_freq=550` (reasoning entry) [D-52]; whether an overclock is acceptable in the product is OQ-096.
- Encoder limits:
  - bitrate 25 kbit/s to 25 Mbit/s, VBR or CBR [D-13];
  - no B-frames [D-14];
  - default encoded-buffer size 768 KiB above 720p [D-23].

**Pi 5 / CM5**

- Official figure: H.264 1080p30 encode (from ISP) costs about 30–40 % CPU [G-22]. Whether that is the whole CPU or one core is UNKNOWN (OQ-059).
- One Raspberry Pi engineer (6by9) reported software 1080p60 encode from camera capture as "easily achievable" (community report) [D-50]. Not measured with TC358743 input.
- Software encoders usually have more latency than the old hardware encoders [D-32].

**All platforms**

No candidate platform has a hardware HEVC encoder [D-24], [D-31]. Pass thresholds are UNDEFINED (OQ-005, OQ-010, OQ-017).

**Two encodes — CM4 and CM5 (added later on 2026-10-08)**

- **CM4.**
  - The encoder is one V4L2 M2M device [D-06], [D-08]. The register has no fact on concurrent encode sessions, so whether two run at once is UNKNOWN (OQ-115).
  - Reasoning: two 1080p30 encodes need about 2.0× the specified macroblock rate [D-10], [D-52]. That is the same demand as one 1080p60 encode, which is itself unproven (RISK-002, OQ-056).
  - For the live encode, Constrained Baseline is offered [D-11], no B-frames are produced [D-14], and SPS/PPS repetition has to be switched on [D-15], [D-37].
- **CM5.** Reasoning from [G-22]: two software encodes need roughly twice the encode CPU of one, plus the conversion (OQ-060). No source covers two concurrent encodes (OQ-059, RISK-003).
- Pass thresholds: UNDEFINED (OQ-005, OQ-010, OQ-017).

**Live-encode latency and keyframes — CM4 and CM5 (added 2026-10-09; OQ-116, OQ-125, OQ-127)**

- **CM4.** No B-frames; default GOP 60, profile High, level 4.0 [K-30]. The element reports 0 latency to the pipeline [K-32]. A Raspberry Pi engineer stated that the encoder holds no extra buffers and that its latency depends on the macroblock count, about 10 ms for 720p on a Pi 4; the issue reporter measured median 9.7 ms, p95 14.3 ms and max 18.5 ms at 1280x720@60 on a Pi 4B (community source) [K-33]. Reasoning (a labelled budget, not a measurement; CORRECTED): scaled by macroblocks, about 23 ms per 1080p frame, assuming the live encode has the hardware encoder to itself — but the recording encode shares it (OQ-115) [K-45]. Measured value: UNKNOWN — HARDWARE TEST REQUIRED.
- **CM5.** With `tune=zerolatency`, x264 holds no frames back [K-27], [K-28]. Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39]. The per-frame x264 time on BCM2712 is unknown until measured, as the reasoning entry [K-45] notes (OQ-059).
- Encode-time threshold: none set. The owner's target is end-to-end (< 1 s for WebRTC viewers, OQ-116), not per stage; the end-to-end result is TEST-STR-002's.

**H.265 — CM4 and CM5 (added 2026-10-08; deferred — REQ-ENC-002; not in current scope)**

- **No sourced throughput figure exists for either board.** Research found no raspberrypi.com figure in a site search [H-19].
  - Raspberry Pi engineer 6by9 stated on the official forum (October 2024) that software H.265 encode "is too intensive an operation to perform at any significant resolution" (community source) [H-19].
  - Community benchmarks report `libx265` "Live" results of 10.00 FPS on Pi 5 and 4.33 FPS on a Raspberry Pi 400 (Cortex-A72 @ 1.8 GHz) [H-20], [H-21]. That test encodes vbench clips with `-threads 1` and a 2022 x265 snapshot, so it is not a 1080p60 live measurement (community source) [H-22].
  - Reasoning: in that harness `libx265` was about 6.6 times slower than `libx264` on Pi 5. This is a relative cost only, not a prediction of PACSCORDER throughput [H-23].
- Reasoning (as in RISK-022): real-time 1080p60 H.265 is therefore **unproven on both boards**, and more doubtful on CM4. Only CM5's Cortex-A76 can use x265's Neon DotProd kernels [H-04], [H-05], and trixie's x265 4.1-2 lacks the AArch64 speed-ups of 4.2 and 4.3 [H-07].
- **Latency reporting.** GStreamer 1.26.2 `x265enc` reports a hard-coded latency of 5 frames, or 0 with `tune=zerolatency`, not one computed from its settings [H-15]. Measure encode latency; do not read it from the element.
- **Output.** `x265enc` produces `video/x-h265`, `stream-format=byte-stream`, `alignment=au` [H-13].
- Whether software-only H.265 at the measured rate is acceptable is an owner decision (OQ-103). Pass thresholds: UNDEFINED (OQ-005, OQ-010, OQ-017). *(Superseded 2026-10-08: OQ-103 is ANSWERED — H.264 only for now. These H.265 expectations apply only if REQ-ENC-002 is re-activated.)*

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-REC-001 — Recording integrity and duration

| | |
|---|---|
| Verifies | REQ-REC-001 |
| Related risks | None registered in [RISKS.md](RISKS.md) *(superseded 2026-10-08: RISK-022, RISK-023 and RISK-024 now list REQ-REC-001; RISK-022 is not in current scope — H.265 deferred, REQ-ENC-002)*; added 2026-10-09 (ADR-009): RISK-026 (CM4 storage path), RISK-027 (USB-to-SATA bridge UAS faults), RISK-028 (HDD stalls), RISK-029 (fragmented-MP4 compatibility), RISK-030 (seconds lost on a power cut) |
| Related open questions | OQ-006, OQ-069; added 2026-10-08: OQ-103, OQ-111, OQ-112, OQ-113 (OQ-103 ANSWERED 2026-10-08: H.264 only); added later on 2026-10-08: OQ-005 (recording encode); added 2026-10-09 (ADR-009): OQ-117 (HDD-branch buffering), OQ-118 (fragment duration, muxer support, compatibility), OQ-119 (loss on a power cut), OQ-120 (filesystem and mount options), OQ-121 (NVMe SSD and adaptor), OQ-122 (USB-to-SATA bridge), OQ-123 (CM4 MSI/MSI-X), OQ-124 (CM5 M.2 enablement), OQ-101 (command syntax) |
| Depends on | TEST-ENC-001; added 2026-10-08: TEST-AUD-001 (audio is required in recordings, REQ-CAP-006); added 2026-10-09: step 5 (HDD stall injection) runs together with TEST-STR-001 and TEST-STR-002 (OQ-117, RISK-028) |

### Objective

Confirm that encoded video is written to local storage as a playable file of the expected duration, and record the behaviour on power loss.

*(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* ADR-009 (ACCEPTED, owner decisions of 2026-10-08) fixes the container (MP4, written fragmented for power-loss safety), the storage (a PCIe NVMe SSD and a USB-to-SATA HDD, every recording mirrored to both) and the HDD power (a self-powered enclosure; ADR-009 decision 3 also allows a self-powered hub). The test is run on **CM4 and CM5** (ADR-004, OPEN) and establishes:

- that every recording is written to both drives, and the two copies are complete;
- that the files are fragmented MP4, and what the fragment settings produce (OQ-118);
- how many seconds are lost on each drive at a power cut, and whether each copy and its filesystem survive (OQ-119, RISK-030);
- that an HDD stall does not stall the NVMe copy, the recording encode or the live path (OQ-117, RISK-028);
- that the USB-to-SATA bridge runs without hangs, resets or I/O errors over a long recording, on each board (OQ-122, RISK-027);
- that the files open in the players and editing tools the owner names (OQ-118, RISK-029);
- on CM4, that the NVMe SSD works in the IO Board's PCIe slot with the board's power arrangement (OQ-121, OQ-123, RISK-026); on CM5, what the M.2 slot needs to enumerate (OQ-124).

The minimum recording duration is still UNDEFINED (OQ-006).

### Setup

- As TEST-ENC-001.
- Container, storage medium, minimum duration and power-loss behaviour: UNDEFINED — OWNER DECISION REQUIRED (OQ-006). The storage options of the candidate boards are not in the source register (OQ-098). *(Superseded in part 2026-10-09: owner decisions of 2026-10-08 — container MP4, written fragmented for power-loss safety; storage a PCIe NVMe SSD and a USB-to-SATA HDD, every recording mirrored to both; HDD in a self-powered enclosure (ADR-009, ACCEPTED; decision 3 also allows a self-powered hub). The storage interfaces of CM4 and CM5 are now in the register (research topic J; see "Storage per board" below). The minimum duration is still UNDEFINED (OQ-006), and the acceptable loss at a power cut is an owner decision under OQ-119.)*
- Available muxers: `gstreamer1.0-plugins-good` ships the MP4 (`isomp4`), Matroska and FLV muxers [G-26].
- Storage location (reasoning):
  - The raspi-config read-only option uses `overlayroot=tmpfs` [G-48]. If that option is used, recordings must go to a separate persistent partition.
  - `rpi-image-gen`'s `image-rota` layout provides a shared persistent data partition [G-40].
  - The partition layout is OQ-069.
  - *(Added 2026-10-09.)* Under ADR-009 every recording goes to the NVMe SSD and the HDD. Whether either drive also holds the operating system is not decided (boot medium: OQ-098, OQ-124).
- *(Added 2026-10-09; ADR-009 / research topic J.)* **Storage per board.** No SSD, adaptor, HDD, enclosure or bridge has been chosen: UNKNOWN — OWNER DECISION REQUIRED and VENDOR CONFIRMATION REQUIRED (OQ-121, OQ-122). Record the part numbers used.
  - **CM4 on the CM4 IO Board.**
    - NVMe SSD in the board's single PCIe Gen 2 x1 socket through a passive PCIe-to-M.2 adaptor. Raspberry Pi states that an NVMe drive has been used this way [J-03], and its NVMe boot documentation suggests a "PCI-E 3.0 ×1 lane to M.2 NGFF M-Key SSD NVMe PCI Express adapter card" [J-11]. That socket is CM4's only PCIe lane [J-01], [J-09] (reasoning, CORRECTED).
    - Power the IO Board from the **+12 V barrel input**: the PCIe slot is powered only from it, and with a 5 V-only PoE HAT PCIe cards do not work [J-05] (RISK-026). If the product will be powered differently (OQ-023), also run step 2 in that arrangement.
    - HDD in a self-powered USB-to-SATA enclosure on the board's USB 2.0 hub. CM4 has only USB 2.0 [J-01], [J-08]; the hub's connectors share one VBUS switch of about 1.2 A, and plugging in the micro-USB cable disables the hub [J-06]. Record every other USB device on the hub (RISK-026).
    - USB host controller: Raspberry Pi OS enables CM4 USB with `otg_mode=1`, which selects an XHCI USB 2.0 controller; the IO Board datasheet uses `dtoverlay=dwc2,dr_mode=host` instead [J-07]. `uas` cannot bind behind `dwc2` and can under `otg_mode=1` [J-24]. Record which `config.txt` setting is used.
  - **CM5 on the CM5 IO Board.**
    - NVMe SSD in the M.2 M-key slot: PCIe Gen 2 x1 by default, Gen 3 "experimental and therefore unsupported" [J-13]; 2230, 2242, 2260 or 2280 [J-14]. Do not raise the link speed with `pciex1_gen=3` [J-15] in this test; research recommends staying at Gen 2 (research design risk, topic J — not a register fact; OQ-124).
    - M.2 enablement: in `rpi-6.18.y` the `pcie1` link used by the slot has status "disabled", and neither the CM5 nor the CM5 IO Board device-tree files enable it; the `pciex1` dtparam (alias `nvme`) controls it and defaults to off [J-15], [J-17]. Whether the firmware enables it at run time is unknown (research gap, topic J; OQ-124). Step 1 boots once without and once with the `pciex1` dtparam set to on (no source quotes the `config.txt` line: NEEDS VERIFICATION, OQ-101).
    - HDD in a self-powered USB-to-SATA enclosure on a USB 3.0 port. CM5 has two USB 3.0 interfaces [J-18]; the IO Board's two USB 3.0 Type-A ports share about 1.2 A of VBUS [J-19]. The NVMe link and the RP1-attached USB 3 HDD are on separate PCIe root ports [J-17].
  - **Both boards.**
    - HDD power: self-powered enclosure, never the board's VBUS (ADR-009 decision 3). The register's only 2.5-inch HDD figure is one Seagate BarraCuda 2.5-inch family (ST2000LM015/ST1000LM048/ST500LM030), which draws up to 1.0 A at 5 V during spin-up (CORRECTED) [J-30]; it is an example, not a general HDD figure. Reasoning: bus-powered, that exceeds the 600 mA peripheral limit of a CM5 IO Board on a 3 A supply, and under the about 1.2 A limits of the CM4 IO Board and of a CM5 IO Board on a 5 A supply it leaves only about 0.2 A for the bridge and any other USB device [J-31]. A 3.5-inch HDD always needs an external 12 V supply [J-32].
    - Kernel: the `rpi-6.18.y` CM4 and CM5 defconfigs build ext4, VFAT, NVMe, USB storage and UAS in, and exFAT as a module [J-29]. Whether the shipped kernel package matches the branch inspected was not checked (research gap, topic J; BUILD TEST REQUIRED).
    - Filesystem: ext4 on both recording volumes, as proposed by Claude inside ADR-009 (decision 4; not an owner decision; OQ-120). ext4 defaults to `data=ordered`, `barrier=1` and `commit=5` (CORRECTED) [J-35]. For the HDD mount, Raspberry Pi's external-storage guide documents `nofail` and `x-systemd.device-timeout=30` (CORRECTED) [J-33]. Record the filesystem and mount options of each volume.
    - **Fragmented MP4 (ADR-009 decision 1).** GStreamer 1.26.2 `mp4mux`/`qtmux`: `fragment-duration` is in milliseconds and produces a fragmented file when > 0; `fragment-mode` is `dash-or-mss` (default) or `first-moov-then-finalise`; setting `fragment-duration` > 0 together with robust-muxing properties silently disables robust muxing, so do not combine them [J-39]. FFmpeg 7.1: `+frag_keyframe` starts a fragment at each video keyframe, and `hybrid_fragmented` writes a fragmented file and converts it to a normal one at the end [J-45]. Reasoning from [J-45]: with `frag_keyframe`, the recording encode's keyframe interval sets the fragment length. The fragment duration is not chosen (OQ-118): record the value used. Whether the Raspberry Pi OS builds (GStreamer 1.26.2, FFmpeg 7.1.5) expose these options as upstream documents them was not checked (research open question, topic J; OQ-118): BUILD TEST REQUIRED. If long recordings are split, `splitmuxsink` starts a new file at a keyframe before `max-size-time` or `max-size-bytes` is crossed [J-44]; record the split rule (OQ-118, OQ-006).
    - **Mirror and HDD branch.** The recording output feeds two file writers, one per drive. The HDD branch must not stall the NVMe copy or the live path (ADR-009 Consequences); its buffering and overflow policy are not designed yet (OQ-117). Research suggests a large or leaky queue in front of the HDD branch (research design risk, topic J — not a register fact). Record the buffer size and overflow policy used. Reasoning from [J-30] and [J-36] (as in OQ-117; not a design figure): riding out a 3.0 s stall (the standby-to-ready maximum of that one Seagate 2.5-inch family [J-30]) at about 3.15 MB/s per destination needs about 3.0 × 3.15 ≈ 9.5 MB of HDD-branch buffering, before any margin.
    - **Data rate.** Reasoning: per destination about 1.02 / 1.52 / 3.15 MB/s at 8 / 12 / 25 Mbit/s video plus 192 kbit/s AAC; the mirror doubles the system total to about 2.05–6.30 MB/s [J-36]. These are research examples; the recording bitrate is still open (OQ-005).
    - **Time reference for loss measurements** (Claude's proposal, reasoning; NEEDS VERIFICATION): an HDMI source that shows a running clock or timecode, so that the last decodable frame of a cut recording can be matched to the moment of the cut.
- *(Added 2026-10-08.)* **Codecs.** Record H.264 and, as far as the owner scopes it to recording (OQ-103), H.265, on CM4 and CM5. *(Superseded later on 2026-10-08: OQ-103 is ANSWERED — "H.264 only for now". Record H.264 only, on CM4 and CM5. The two H.265 container notes below are deferred — REQ-ENC-002; not in current scope.)*
  - GStreamer 1.26.2 `qtmux` / `mp4mux` and `matroskamux` accept H.265; `h265parse` is needed after `x265enc`; `matroskamux` warns that the `hev1` form is not officially supported [H-37].
  - FFmpeg 7.1.5's MP4 muxer tags HEVC as `hev1`, `hvc1` or `dvh1`, and its Matroska muxer handles HEVC [H-38].
- *(Added 2026-10-08.)* **Audio** (REQ-CAP-006). AAC encoders available: FFmpeg's native `aac` (CORRECTED) [I-40], GStreamer `voaacenc` [I-44] and `avenc_aac` [I-46]. Capture audio at the rate the source sends (TEST-AUD-001; RISK-023).
- *(Added later on 2026-10-08.)* **Recording encode.** Recordings come from the recording encode. It is separate from the live encode that RTMP and WebRTC use (owner, "Separate record + live", OQ-005; REQ-ENC-001).
  - Its bitrate and rate control are still open (OQ-005). Record the profile, level, bitrate and mode used.
  - Run at least once with the live encode running at the same time (CM4: OQ-115; CM5: OQ-059). TEST-PERF-001 covers the full combined load.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.** No register fact gives a recording pipeline for this chain, so commands are NEEDS VERIFICATION.

*Original steps of 2026-10-06 and 2026-10-08, kept for the record (Rule 21). Superseded on 2026-10-09 by the full procedure below, which covers them (steps 1 and 2 → full-procedure step 2; step 3 → step 4; step 4 → step 2, audio checks; step 5 → step 2, which runs with the live encode).*

1. Record for the owner-defined duration.
2. Stop recording normally and check the file plays, its duration and its frame count.
3. If OQ-006 requires it, remove power during recording, reboot, and check what is recoverable.
4. *(Added 2026-10-08.)* Repeat steps 1–2 with H.265 video (if scoped to recording, OQ-103) and with audio, using a 44.1 kHz and a 48 kHz source. Check the recorded audio's pitch and duration against the source, and the A/V offset at the start and end of the file (OQ-112). *(Superseded later on 2026-10-08: the H.265 repeat is deferred, not run in current scope — REQ-ENC-002 (OQ-103 ANSWERED). The audio repeat runs with H.264 video.)*
5. *(Added later on 2026-10-08.)* Repeat steps 1–2 with the live encode running at the same time (two encodes, OQ-005). Compare the recorded frame count with the run of step 2.

**Full procedure (2026-10-09; ADR-009).** Run every step on CM4 and on CM5. In each result entry record: board and carrier board, kernel version, `config.txt` and `cmdline.txt` lines, power supply, SSD and adaptor part numbers, enclosure, bridge VID:PID and chipset, HDD model, filesystem and mount options of each volume, muxer and fragment settings, recording-encode settings, and the HDD-branch buffer size and overflow policy. Commands not named below are NEEDS VERIFICATION (OQ-101).

1. **Storage enumeration.** Boot. Inspect the kernel log for the NVMe controller and for the USB-to-SATA bridge, and record the lines.
   - Confirm that the SSD appears as `/dev/nvme0` with namespace `/dev/nvme0n1` [J-11], and record the HDD's block device.
   - Record the bridge's VID:PID and whether `uas` or `usb-storage` is bound (OQ-122; [J-24], [J-25]). Research suggested `lsusb -t` for the bound driver (research open question, topic J — not a register fact).
   - CM4: record the active USB host controller (`otg_mode=1` XHCI or `dwc2`) [J-07], and the interrupt mode the `nvme` driver gets, MSI-X, MSI or legacy (OQ-123; the official sources disagree [J-02]).
   - CM5: boot once without and once with the `pciex1` dtparam set to on (it defaults to off [J-15]; `config.txt` line form NEEDS VERIFICATION, OQ-101), and record whether the SSD enumerates each time and the PCIe link speed (OQ-124). Research suggested `lspci` for enumeration (research open question, topic J — not a register fact).
2. **Mirrored recording, clean stop.** With the live encode running (two encodes, OQ-005; TEST-STR-001 and TEST-STR-002 publishing), record H.264 video and AAC audio to both drives at once, for the owner-defined duration (UNDEFINED, OQ-006; record the duration used). Stop normally. For each copy:
   - check that it plays, and record its duration and frame count; compare the recording encode's frame count with a run without the live encode (original step 5);
   - check the audio pitch and duration against the source with a 44.1 kHz and a 48 kHz source, and the A/V offset at the start and end of the file (OQ-112; original step 4);
   - compare the two copies: duration, frame count, first and last timestamps. Record any difference.
3. **Fragmented-MP4 check (OQ-118).** Inspect the structure of each cleanly stopped copy and confirm that it is fragmented; record the fragment duration actually produced. If `hybrid_fragmented` is used, confirm that a cleanly stopped file has been converted to a normal MP4 [J-45]. If `fragment-mode=first-moov-then-finalise` is used, record what a cleanly stopped file contains: the register names that mode [J-39] but does not describe its output (NEEDS VERIFICATION). Before the first run, list the muxer properties of the shipped GStreamer and the movflags of the shipped FFmpeg (research open questions, topic J; BUILD TEST REQUIRED). The inspection tools are NEEDS VERIFICATION (OQ-101).
4. **Power cut during recording (OQ-119, RISK-030).** While recording to both drives with the time-reference source, cut the power as a product power failure would. Whether the board and the HDD enclosure share one supply in the product is UNKNOWN (OQ-023): run once with both cut together and once with only the board cut, and record which. Reboot, then for each drive:
   - record the filesystem's recovery messages in the kernel log and the result of a filesystem check (command NEEDS VERIFICATION, OQ-101);
   - check that the file is decodable and plays [J-45];
   - record the **seconds lost**: the time of the cut minus the time shown in the last decodable frame;
   - repeat for each fragment setting evaluated (OQ-118). The number of repetitions is UNDEFINED (OQ-119); record it.
   - Optional control, once per board: record a plain moov-at-end MP4 and cut power, to confirm the documented failure on PACSCORDER [J-38].
5. **HDD stall injection (OQ-117, RISK-028).** Run TEST-STR-001 and TEST-STR-002 (including its latency measurement) at the same time, recording to both drives. Inject, one at a time:
   - a. **Spin-up from standby:** let the HDD go to standby, then start a recording. Whether the chosen enclosure or drive spins down on its own, and whether standby can be disabled or lengthened through the bridge, is UNKNOWN (OQ-117). Research suggested `hdparm -B/-S` (research open question, topic J — not a register fact; NEEDS VERIFICATION).
   - b. **HDD disconnect:** unplug the HDD's USB cable during a recording, then reconnect it.
   - c. **Enclosure power cycle** during a recording.
   - d. **CM4 only:** load the shared USB 2.0 hub with any other USB device the product will carry (RISK-026).
   - e. **Bridge reset:** the method to force a UAS reset is not in the register: NEEDS VERIFICATION (OQ-101).

   For each, record: the stall seen by the HDD writer; whether the HDD-branch buffer overflowed and what the overflow policy did; whether the **NVMe copy** has any gap (frame count and continuous timestamps); whether the recording encode or the capture dropped frames; and the **live latency and frame rate** before, during and after (TEST-STR-002; RTMP output, TEST-STR-001). Also record what the recorder does with one mirror target missing; that behaviour is UNDEFINED — OWNER DECISION REQUIRED (OQ-117).
6. **USB-to-SATA bridge soak (RISK-027, OQ-122).** Record to both drives for several hours (duration UNDEFINED, OQ-010, OQ-006) while both encodes and both streams run. Inspect the kernel log for USB resets, UAS errors and I/O errors. If the bridge hangs or resets under `uas`, repeat with the workaround `usb-storage.quirks=VID:PID:u` in `cmdline.txt`, which a Raspberry Pi engineer gave (community source, CORRECTED) [J-27]; flag `u` is IGNORE_UAS [J-26]. Run on CM4 (USB 2.0) and CM5 (USB 3.0) separately: reasoning from [J-24], a bridge qualified on one board is not qualified on the other (RISK-027). On CM4, also record the sustained write rate and CPU load of the HDD path (OQ-122); the method is NEEDS VERIFICATION (OQ-101).
7. **Player and editor compatibility (OQ-118, RISK-029).** Open the cleanly stopped files (step 2) and the power-cut files (step 4) in each player and editing tool the owner names (OWNER DECISION REQUIRED, OQ-118). Record for each tool: opens, seeks, shows the right duration, plays audio, imports without remuxing. If a tool fails, record whether a remuxed copy opens; FFmpeg documents that an aborted `hybrid_fragmented` file can be remuxed by hand [J-45].
8. **HDD absent at boot (OQ-120).** Boot with the HDD disconnected. Record the boot delay, with `nofail` alone and with `x-systemd.device-timeout=30` added [J-33], and whether recording to the NVMe SSD starts.

### Expected Result

- No source-derived expectation exists for PACSCORDER recording. Acceptance criteria are UNDEFINED (OQ-006, OQ-017). *(Superseded in part 2026-10-09: research topic J gives source predictions for the storage path and for power-loss behaviour; they are listed below. Acceptance criteria are still UNDEFINED (OQ-006, OQ-017), and the acceptable loss at a power cut is an owner decision (OQ-119).)*
- For comparison only: ATEM switchers record H.264 + AAC in MP4 [F-07], [F-26].
- *(Added 2026-10-08.)* HEVC in MP4 and Matroska is supported by both frameworks [H-37], [H-38] (deferred — REQ-ENC-002; not in current scope). FFmpeg's native `aac` encoder defaults to 128 kb/s for stereo when no bitrate is given, and an explicit bitrate selects CBR (CORRECTED) [I-40]. If the capture rate does not match the source rate, the recorded audio has the wrong speed and pitch with no error (reasoning) [I-18] (RISK-023).

*Added 2026-10-09 for the full procedure (ADR-009). These are source predictions, not acceptance criteria (OQ-017).*

**Enumeration (step 1)**

- **CM4.** The SSD appears as `/dev/nvme0` / `/dev/nvme0n1` [J-11], and only when the IO Board is powered from +12 V [J-05]. Reasoning (CORRECTED): the HDD runs at USB 2.0 High-Speed through the USB2514B hub, shared with every other USB device on the board [J-09]. `uas` can bind under `otg_mode=1` and cannot behind `dwc2` [J-24]; the kernel's built-in quirks can still force `usb-storage` for some bridges, for example some ASMedia bridges connected below SuperSpeed [J-25]. Interrupt mode: UNKNOWN (OQ-123) [J-02].
- **CM5.** The M.2 link is disabled in the `rpi-6.18.y` device tree [J-17]; whether the SSD enumerates without the `pciex1` dtparam set to on is UNKNOWN — HARDWARE TEST REQUIRED (OQ-124). The HDD is on a USB 3.0 port [J-18], on a different PCIe root port from the NVMe link [J-17].

**Mirrored recording and fragmentation (steps 2 and 3)**

- Reasoning: even the highest of the research example rates (25.192 Mbit/s; the recording bitrate is open, OQ-005) is about 5.2 % of USB 2.0's 480 Mbit/s signalling rate and about 0.63 % of the about 4 Gbit/s left after 8b/10b coding on a PCIe Gen 2 x1 or USB 3.0 link, so interface bandwidth is not the bottleneck on either board; on CM4 the USB 2.0 bus limits only offload and copy times [J-37].
- Reasoning: a 1 TB disk holds about 271 h at 8 Mbit/s and about 88 h at 25 Mbit/s [J-36].
- GStreamer: `fragment-duration` > 0 produces a fragmented file [J-39]. FFmpeg: `+frag_keyframe` starts a fragment at each video keyframe; `hybrid_fragmented` converts the file to a normal MP4 at the end [J-45].
- Whether the two copies are identical: UNKNOWN; it depends on the mirror design (OQ-117, ADR-007).

**Power cut (step 4; OQ-119, RISK-030)**

- FFmpeg 7.1's muxer documentation says that a fragmented file stays decodable if writing is interrupted, and that a normal MOV/MP4 that was not properly finished is undecodable [J-45]. No register entry states this for GStreamer's fragmented output; step 4 checks it. A plain GStreamer moov-at-end file has no moov after an unclean stop and is not playable [J-38] (optional control run).
- ext4 with its defaults (`data=ordered`, `barrier=1`, `commit=5`): a power loss loses at most the last 5 s of metadata changes and the journal keeps the filesystem undamaged, but because of delayed allocation even older data can be lost (CORRECTED) [J-35].
- `filesink` calls `fsync()` after writing a buffer flagged sync-after, which robust mode sets on its index updates, and its `o-sync` property defaults to FALSE [J-41]. Whether fragmented mode forces each fragment to storage is not in the register: UNKNOWN — HARDWARE TEST REQUIRED (OQ-119).
- Seconds lost per drive and board: UNKNOWN — HARDWARE TEST REQUIRED (OQ-119). The drives' own volatile write caches may add an unknown amount (research open question, topic J — not a register fact; OQ-121, OQ-122). Reasoning (as in RISK-030): both copies lose their final seconds at the same cut, so the mirror does not reduce this loss.

**HDD stall (step 5; OQ-117, RISK-028)**

- One Seagate BarraCuda 2.5-inch family, the register's only HDD timing figure, takes 2.5 s typical, 3.0 s maximum from standby to ready (CORRECTED) [J-30]; it is not a general HDD figure, and the chosen drive's figure is UNKNOWN (OQ-122).
- Reasoning (as in RISK-028, from [J-30] and [K-36]): a 3.0 s stall is three times the whole < 1 s live budget, and if back-pressure reached the capture queue each waiting frame would add one frame period, 33.3 ms at 30p. ADR-009 requires that the NVMe copy and the live path are not stalled; whether the chosen HDD-branch design achieves that is UNKNOWN until measured.

**Bridge soak (step 6; RISK-027, OQ-122)**

- A Raspberry Pi engineer's forum post reports that some UAS devices that do not fully implement the UAS specification stop responding, or in rare cases throw write data away, which can corrupt the filesystem; the workaround given is `usb-storage.quirks=VID:PID:u` (community source, CORRECTED) [J-27].
- Raspberry Pi's documentation warns that USB SATA adapters can fail if Linux selects UAS mode, and that HDDs without a powered hub can fail intermittently even when everything appears to work [J-28].

**Compatibility (step 7; OQ-118, RISK-029)**

- FFmpeg documents lower compatibility as the downside of fragmented files [J-45]. Per-tool results: UNKNOWN until tested; the tool list is an owner decision (OQ-118).

**HDD absent at boot (step 8; OQ-120)**

- Raspberry Pi's guide: an absent disk adds 90 s to boot; `x-systemd.device-timeout=30` after `nofail` shortens that wait to 30 s rather than removing it (CORRECTED) [J-33]. Whether recording to the NVMe SSD starts without the HDD depends on the recorder design: UNDEFINED (OQ-117).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-STR-001 — RTMP publish and playback

| | |
|---|---|
| Verifies | REQ-STR-001 |
| Related risks | RISK-003; added 2026-10-08: RISK-022, RISK-023, RISK-024, RISK-025 (RISK-022 and RISK-025 not in current scope — H.265 deferred, REQ-ENC-002); added 2026-10-09: RISK-019 (shared live encode: B-frames and keyframes, OQ-127), RISK-028 (HDD stalls must not reach the live path) |
| Related open questions | OQ-007, OQ-063, OQ-075; added 2026-10-08: OQ-076, OQ-101, OQ-103, OQ-106, OQ-107, OQ-111, OQ-113 (OQ-103 ANSWERED 2026-10-08; OQ-106 and OQ-107 not in current scope — H.265 deferred, REQ-ENC-002); added later on 2026-10-08: OQ-005 (live encode shared with WebRTC); added 2026-10-09: OQ-116 (ANSWERED 2026-10-08: RTMP latency is best-effort), OQ-127 (live keyframe interval and B-frames) |
| Depends on | TEST-ENC-001; added 2026-10-08: TEST-AUD-001 (audio) |

### Objective

Publish the encoded stream (and audio, if required) to an RTMP server and confirm that it plays back. *(2026-10-08: audio is required — OQ-004 ANSWERED 2026-10-07 — so every run carries the HDMI audio. H.264 and, as far as the owner scopes it to RTMP (OQ-103), H.265 are published, on CM4 and CM5.)* *(Superseded later on 2026-10-08: OQ-103 is ANSWERED — "H.264 only for now". Only H.264 is published, on CM4 and CM5; the H.265 run is deferred — REQ-ENC-002; not run in current scope.)*

### Setup

- As TEST-ENC-001.
- RTMP destination: UNKNOWN — OWNER DECISION REQUIRED (OQ-007).
- Test server: UNKNOWN (OQ-075). Candidates from sources:
  - MediaMTX, which its project reports listens for RTMP on `:1935` [F-44];
  - FFmpeg with its `listen` option [F-32].
- Packages:
  - `rtmp2sink` is in `gstreamer1.0-plugins-bad` (`libgstrtmp2`) [G-27];
  - `flvmux` is in `gstreamer1.0-plugins-good` (`libgstflv`) [G-26].
- *(Added 2026-10-08.)* **H.265 over RTMP (deferred — REQ-ENC-002; not in current scope).**
  - GStreamer 1.26.2's `flvmux` has no H.265 on its video sink pad [H-27], and `eflvmux` first appears in the 1.28 branch (CORRECTED) [F-34]. **The distribution's GStreamer cannot publish H.265 over RTMP.** Which path PACSCORDER uses is OQ-107 (RISK-025).
  - FFmpeg 7.1.5 can mux HEVC + AAC into enhanced FLV for RTMP publishing, with FourCC `hvc1`. Its `rtmp_enhanced_codecs` option writes a `fourCcList` into the connect command and accepts only `hvc1`, `av01` and `vp09` [H-26]. The Enhanced RTMP specification defines `hvc1` for HEVC [H-24].
  - Test servers: MediaMTX's documentation lists H265 among RTMP publish and read codecs [H-28]. YouTube Live lists H.264, H.265 and AV1 over RTMP/RTMPS, with AAC or MP3 audio [H-29]. Whether each required destination accepts FFmpeg's signalling is OQ-106.
  - Alternative transport: the distribution stacks contain the components for HEVC over SRT in MPEG-TS [H-30]; whether SRT is required is OQ-076.
- *(Added 2026-10-08.)* **Audio.** AAC encoders: FFmpeg's native `aac` (CORRECTED) [I-40], GStreamer `voaacenc` [I-44] and `avenc_aac` [I-46]. FFmpeg 7.1.5's FLV muxer has no Opus [H-26]. `fdk-aac` is not shipped and would need `--enable-nonfree` in the GPL FFmpeg build, which makes it unredistributable [I-39], [I-42] (OQ-113). Use sources at 44.1 kHz and 48 kHz (TEST-AUD-001).
- *(Added later on 2026-10-08.)* **Live encode.** RTMP publishes the live encode, which WebRTC shares (owner, "Separate record + live", OQ-005; REQ-ENC-001).
  - It is set up to be WebRTC-receivable: Constrained Baseline, no B-frames, in-band SPS/PPS (TEST-ENC-001 Setup, live-encode settings).
  - Its bitrate and rate control are still open (OQ-005).
  - Audio is still a separate encode: AAC for RTMP, beside the Opus encode for WebRTC (TEST-STR-002).
- *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* **Latency is best-effort (OQ-116 ANSWERED).** The owner's < 1 s camera-to-viewer target applies to WebRTC viewers only; RTMP outputs are best-effort, and their latency is set by the receiving platform (REQ-STR-001). For reference, YouTube Live:
  - offers three latency modes: Normal (no figure), Low ("less than 10 seconds" for most viewers, no 4K) and Ultra-low ("less than 5 seconds" for most viewers, no 4K, more viewer buffering) [K-15]; the API's `latencyPreference` accepts `normal`, `low` and `ultraLow`, and `ultraLow` supports neither closed captions nor resolutions above 1080p [K-16];
  - reasoning (CORRECTED): YouTube documents no sub-second mode and names the player's read-ahead buffer as the main source of stream latency, so encoder settings on PACSCORDER cannot remove it [K-18];
  - recommends a 2 s keyframe frequency ("Do not exceed 4 seconds"), CBR, 2 B-frames, 1 reference frame and CABAC; the B-frame advice conflicts with the B-frame-free live encode WebRTC needs [K-17] (OQ-127, RISK-019).

  Record the destination's latency mode. Other RTMP platforms were not checked for latency modes (research gap, topic K; OQ-007).

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

- GStreamer, documented form [F-33]:
  ```bash
  ... x264enc ! flvmux ! rtmp2sink location=rtmp://<server>/<app>/<key>     # [F-33]
  ```
  - On Pi 4 / CM4 with `v4l2h264enc`, an `h264parse` element is needed before `flvmux` (reasoning) [F-35], because `flvmux` needs `stream-format=avc` (CORRECTED) [F-34].
  - Full pipelines are NEEDS VERIFICATION.
- FFmpeg, documented publish form (shown with a file input) [F-32]:
  ```bash
  ffmpeg -re -i myfile -f flv rtmp://myserver/live/mystream                  # [F-32]
  ```
  The live-capture input options are NEEDS VERIFICATION.
- Play the stream back from the server with a client. The client is UNDEFINED (OQ-007).
- *(Added 2026-10-08.)* **H.265 run (CM4 and CM5) — deferred, not run in current scope (REQ-ENC-002; OQ-103 ANSWERED: H.264 only for now).** Publish H.265 with FFmpeg: `libx265` video and AAC audio to `-f flv`, once without and once with `rtmp_enhanced_codecs` set to `hvc1` [H-26]. The option's command-line form is NEEDS VERIFICATION (OQ-101); whether a destination needs it is OQ-106. Publish to MediaMTX, then to each required destination (OQ-007), and play back. If the owner chooses another path under OQ-107 (backported `eflvmux`, newer GStreamer, or SRT), run that path instead and record it.
- *(Added 2026-10-08.)* **Audio run.** In every run, include AAC audio captured from the `tc358743` card at the source's rate (TEST-AUD-001, step 4). Repeat with a 44.1 kHz and a 48 kHz source, and play back to check pitch, duration and A/V offset.
- *(Added later on 2026-10-08.)* **Shared live encode.** At least once, run this test together with TEST-STR-002 from the same live encode, with the recording encode (TEST-REC-001) also running. Record that RTMP and WebRTC take their video from the one live encode, and record the CPU load.
- *(Added 2026-10-09; OQ-116, OQ-127.)* **RTMP latency — recorded, not judged.** In every run, measure the camera-to-viewer latency at each destination's player, with the destination's latency mode recorded, using the same method as TEST-STR-002 (NEEDS VERIFICATION, OQ-125). There is no pass or fail on RTMP latency (OQ-116 ANSWERED). Also record the live encode's keyframe interval and B-frame setting (TEST-ENC-001 Setup) and whether each destination accepts and plays the B-frame-free stream (OQ-127).
- *(Added 2026-10-09; OQ-117, RISK-028.)* **HDD stall.** Keep this test running during TEST-REC-001 step 5 (HDD stall injection), and record whether the RTMP output stalls, drops frames or disconnects.

### Expected Result

- The server accepts the publish connection. The default RTMP port is TCP 1935 [F-32].
- The stream carries H.264 video (FLV CodecID 7) and, if audio is included, AAC audio [F-31]. `flvmux` needs raw AAC (CORRECTED) [F-34]. *(2026-10-08: audio is required, so AAC audio is expected in every run.)*
- Pi 4 / CM4: in-band SPS/PPS repetition is off by default on the encoder [D-15]. The official pipeline enables it with `repeat_sequence_header=1` [D-37].
- Pass criteria (bitrate, latency, duration): UNDEFINED (OQ-007, OQ-017). *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — RTMP latency is best-effort, set by the receiving platform, and is not a pass criterion (OQ-116 ANSWERED). Bitrate and duration criteria are still UNDEFINED (OQ-005, OQ-007, OQ-017).)*
- *(Added 2026-10-09.)* **Latency, for reference.** YouTube Ultra-low: "less than 5 seconds" for most viewers [K-15]; reasoning (CORRECTED): that is an upper bound for most viewers, not a minimum [K-18]. Reasoning (CORRECTED): the RTMP-to-YouTube output cannot be planned or claimed to meet < 1 s [K-18]. No expectation exists for other destinations (research gap, topic K; OQ-007).
- *(Added 2026-10-09.)* Whether YouTube and the other destinations accept the B-frame-free live encode with acceptable quality is not stated by any source (research open question, topic K; OQ-127).
- *(Added later on 2026-10-08.)* **Live encode.**
  - The stream carries the live encode as H.264 (FLV CodecID 7) [F-31]. Reasoning: because RTMP and WebRTC share one encode, the RTMP stream has the same profile, level and SPS/PPS as the WebRTC track.
  - Whether every required RTMP destination accepts a Constrained Baseline stream is not in the source register: UNKNOWN (OQ-007).
- *(Added 2026-10-08.)* **H.265 (deferred — REQ-ENC-002; not in current scope):**
  - Legacy FLV carries only AVC video; HEVC needs Enhanced RTMP FourCC signalling [F-31]. FFmpeg 7.1.5 writes FourCC `hvc1` and enhanced video headers [H-26].
  - Linking `x265enc` or `h265parse` to GStreamer 1.26.2 `flvmux` is expected to fail, because its sink pad has no H.265 [H-27] ([TROUBLESHOOTING.md](TROUBLESHOOTING.md) §6.7).
  - Any `rtmp_enhanced_codecs` value other than `hvc1`, `av01` or `vp09` fails with `AVERROR_PATCHWELCOME` [H-26].
  - YouTube Live, for reference: recommended 1080p60 bitrate 12 Mbps for H.265 versus 17 Mbps for H.264; 2 s keyframes recommended, not more than 4 s; CBR [H-29]. Whether YouTube accepts FFmpeg's signalling is UNKNOWN (OQ-106).
  - Software H.265 throughput on CM4 and CM5 is unproven (TEST-ENC-001; RISK-022).
- *(Added 2026-10-08.)* **Audio:**
  - AAC only: FFmpeg's FLV muxer has no Opus [H-26]. Without an explicit bitrate, FFmpeg's native `aac` encodes stereo at 128 kb/s; an explicit bitrate selects CBR (CORRECTED) [I-40].
  - Correct pitch and duration only when the capture rate equals the source rate; otherwise the audio is wrong with no error (reasoning) [I-18] (RISK-023).
  - The A/V offset depends on the framework's clock handling [I-35], [I-38] (RISK-024). Tolerance UNDEFINED (OQ-112).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-STR-002 — WebRTC browser playback

| | |
|---|---|
| Verifies | REQ-STR-002 |
| Related risks | RISK-019, RISK-003; added 2026-10-08: RISK-022, RISK-023, RISK-024 (RISK-022 not in current scope — H.265 deferred, REQ-ENC-002); added 2026-10-09: RISK-028 (HDD stalls reaching the live path), RISK-031 (< 1 s target unproven), RISK-032 (hidden default latencies and queue backlog), RISK-033 (NAT traversal and TURN), RISK-034 (WebRTC publishing stack) |
| Related open questions | OQ-008, OQ-063, OQ-073, OQ-074; added 2026-10-08: OQ-103, OQ-108, OQ-111, OQ-112 (OQ-103 ANSWERED 2026-10-08; OQ-108 not in current scope — H.265 deferred, REQ-ENC-002); added later on 2026-10-08: OQ-005 (live encode shared with RTMP); added 2026-10-09: OQ-116 (ANSWERED 2026-10-08: < 1 s for WebRTC viewers), OQ-117 (HDD branch), OQ-125 (measured camera-to-viewer latency), OQ-126 (live-path element latencies and queues), OQ-127 (keyframes and viewer join), OQ-128 (internet viewers), OQ-101 (command syntax) |
| Depends on | TEST-ENC-001; added 2026-10-08: TEST-AUD-001 (audio) |

### Objective

Confirm that live PACSCORDER video plays in each target browser over WebRTC, and record the negotiated H.264 profile and level.

*(Added 2026-10-08.)* Audio is required (OQ-004 ANSWERED 2026-10-07), so every session carries an Opus audio track. If the owner scopes H.265 to WebRTC (OQ-103), also record, per browser, whether an H.265 track is negotiated and decoded (OQ-108). Run on CM4 and CM5. *(Superseded later on 2026-10-08: OQ-103 is ANSWERED — "H.264 only for now", so WebRTC carries H.264 only. The H.265 run is deferred — REQ-ENC-002; not run in current scope.)*

*(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* The owner set the live latency target on 2026-10-08: **under 1 s camera-to-viewer for WebRTC viewers** (OQ-116 ANSWERED; REQ-STR-002). This test therefore also measures camera-to-viewer latency, on CM4 and CM5, with the recording encode running, for LAN viewers and — only if the owner puts them in scope (OQ-008) — for internet viewers, recorded separately (OQ-125, OQ-128). It also checks that an HDD stall in the mirrored recording (ADR-009) does not reach the live path (OQ-117, RISK-028).

### Setup

- As TEST-ENC-001.
- Target browsers, viewer reach (LAN or internet), number of viewers and latency target: UNDEFINED — OWNER DECISION REQUIRED (OQ-008). RISK-019 names Chrome, Firefox and Safari. *(Superseded in part 2026-10-09: owner decision of 2026-10-08 — latency target under 1 s camera-to-viewer for WebRTC viewers (OQ-116 ANSWERED; REQ-STR-002, acceptance DRAFT). Browsers, reach, number of viewers and how the target is judged (statistic, samples, conditions) are still UNDEFINED (OQ-008).)*
- Signalling and NAT traversal: UNDEFINED (OQ-074).
- Packages: `webrtcbin` is in `gstreamer1.0-plugins-bad` (`libgstwebrtc`) [G-27] and needs `gstreamer1.0-nice` at runtime [G-28]. `webrtcbin` has no built-in signalling [F-42].
- *(Added 2026-10-08.)* **Opus encoders.** GStreamer `opusenc` (in `gstreamer1.0-plugins-base`) [I-45] and FFmpeg's `libopus` wrapper [I-41]. Both accept only 48000, 24000, 16000, 12000 or 8000 Hz [I-41], [I-47], so a 44.1 kHz HDMI source needs `audioresample` before `opusenc` [I-47]. FFmpeg's native `opus` encoder is experimental, CELT-only and 48 kHz only; production encoding should use `libopus` [I-41].
- *(Added 2026-10-08.)* **H.265 in WebRTC (deferred — REQ-ENC-002; not in current scope).** GStreamer's `rtph265pay` implements the RFC 7798 HEVC payload; profile, tier and level in its output caps arrived in 1.26.4, after the distribution's 1.26.2 [H-31]. MediaMTX's documentation lists H265 among WebRTC read codecs [H-28]. Viewer browsers and devices for an H.265 run: OQ-008, OQ-108.
- *(Added later on 2026-10-08.)* **Live encode.** WebRTC plays the live encode, which RTMP shares (owner, "Separate record + live", OQ-005; REQ-ENC-001). There is no separate WebRTC video encode. The live encode is set for WebRTC (TEST-ENC-001 Setup, live-encode settings):
  - Constrained Baseline [F-36], which the CM4 encoder offers [D-11];
  - Level 4 or above for 1080p (reasoning) [F-40];
  - no B-frames, because browsers do not accept them in WebRTC (reported by the MediaMTX project; community source) [F-45]; the CM4 encoder produces none [D-14];
  - SPS/PPS in-band [F-36]; off by default on the CM4 encoder [D-15].

  In step 2, also record the profile and level in the encoded SPS, next to the SDP `profile-level-id`. The inspection tool is NEEDS VERIFICATION (OQ-101).
- *(Added 2026-10-09; OQ-116 / research topic K.)* **Publishing route.** The route is part of ADR-007 (OPEN); record it and the versions used.
  - `webrtcsink` and `whipclientsink` (gst-plugins-rs) are not packaged in Debian trixie or in the Raspberry Pi archive (trixie main arm64) as of 2026-10-08 [K-06] (RISK-034). Whether the plugin is on the PACSCORDER image is a BUILD TEST (research open question, topic K).
  - MediaMTX documents RTSP-client publishing as the recommended way for GStreamer to publish to it; WebRTC publishing from GStreamer needs 1.22 or later and, for H.264, the Baseline profile [K-05]. The MediaMTX project reports that it serves WebRTC readers through a browser page and WHEP (community source) [F-45]. WHEP is still an Internet-Draft [K-02]; WHIP is RFC 9725 and covers ingest only [K-01].
- *(Added 2026-10-09; OQ-126, RISK-032.)* **Latency settings to set explicitly and record.** Defaults: `webrtcbin` `latency` 200 ms [K-07]; `rtpbin` and `rtpjitterbuffer` 200 ms [K-08]; `rtspclientsink` and `rtspsrc` 2000 ms, while MediaMTX's own GStreamer reader examples set `rtspsrc latency=0` [K-09]. Reasoning: the jitter-buffer values apply only where a GStreamer element receives RTP inside the live path; a browser viewer uses its own buffer [K-10]. Whether `rtspclientsink`'s 2000 ms adds delay on the sending side is unknown (research open question, topic K; OQ-126). `v4l2src` reports a minimum latency of one frame and a maximum of buffer-pool depth × frame duration [K-37]. Live-encode settings: TEST-ENC-001 Setup. Record the latency property and queue settings of every element in the live path.
- *(Added 2026-10-09; OQ-128, RISK-033.)* **MediaMTX network.** MediaMTX v1.21.1 listens for WebRTC on UDP `:8189` and leaves its TCP listener disabled by default [K-41]. It advertises its interface addresses by default; for LAN clients its docs say to put the server's LAN address in `webrtcAdditionalHosts`, for internet clients its public IP or DNS name; STUN/TURN is "Needed only when local listeners can't be reached by clients" [K-42]. It documents four connection methods — static UDP port (default), static TCP port, random UDP port with STUN hole punching, TURN relay — and recommends TCP transport only for a coturn relay [K-43]. Record the MediaMTX version and its WebRTC settings.
- *(Added 2026-10-09; OQ-128.)* **Viewers.** LAN viewers on the product's network. Internet viewers only if OQ-008 puts them in scope: behind NAT, through a forwarded UDP port with the public address advertised, through STUN, or through a TURN relay. Record each viewer's network path and browser version.
- *(Added 2026-10-09; OQ-125.)* **Measuring camera-to-viewer latency.** No register fact gives a measurement method: NEEDS VERIFICATION (HARDWARE TEST REQUIRED, OQ-125). Proposed method: research names measuring with a flashing-timecode source (research gap, topic K — not a register fact); Claude's proposal for applying it (reasoning; NEEDS VERIFICATION) is to show a running clock or timecode through the HDMI source chain and film it together with the viewer's display, the latency being the difference between the two readings. Viewer-side statistics from the register: the W3C statistics API defines the inbound-rtp metrics `jitterBufferDelay`, `jitterBufferEmittedCount`, `jitterBufferTargetDelay`, `jitterBufferMinimumDelay` and `totalProcessingDelay`; `roundTripTime` on remote-inbound-rtp is a sender-side metric, and a viewer can read the round-trip time from candidate-pair `currentRoundTripTime` or remote-outbound-rtp `roundTripTime` (CORRECTED) [K-12]. Browsers also offer `RTCRtpReceiver.jitterBufferTarget`, a hint of at most 4000 ms that influences but does not set the jitter-buffer target [K-11]. How to derive per-frame figures from these metrics is not in the register: NEEDS VERIFICATION (OQ-101).

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.** No register fact gives a WebRTC pipeline or signalling command, so commands are NEEDS VERIFICATION.

1. Start a WebRTC session from each target browser.
2. Capture the SDP offer and answer. Record `profile-level-id`, `packetization-mode` and `level-asymmetry-allowed`.
3. Play 1080p video for the owner-defined duration. Record whether it decodes, plus latency and stalls. *(2026-10-09: latency is measured as in step 6, against the owner's < 1 s target for WebRTC viewers, OQ-116.)*
4. If audio is required: confirm an Opus or G.711 audio track. *(Superseded 2026-10-08: audio is required. Confirm an Opus track in every session, once with a 48 kHz source and once with a 44.1 kHz source resampled to 48 kHz before the encoder. Check pitch, duration and A/V offset at playback.)*
5. *(Added 2026-10-08.)* **H.265 run** (only if OQ-103 scopes H.265 to WebRTC) — *deferred, not run in current scope: OQ-103 ANSWERED 2026-10-08, H.264 only for now (REQ-ENC-002).* From each target browser, record whether H.265 appears in the SDP offer and answer, and whether it decodes. Keep the H.264 track in the same test, and record the CPU load with both encodes running (OQ-104, OQ-108).
6. *(Added 2026-10-09; OQ-125, OQ-116, RISK-031.)* **Camera-to-viewer latency, LAN viewers.** On CM4 and CM5, with the recording encode running (TEST-REC-001), measure the camera-to-viewer latency of a WebRTC viewer on the LAN, in each target browser, at 1080p30 and at the other live modes, with the method in Setup. Take repeated readings and record the median and the maximum (the number of readings is UNDEFINED; record it). Which statistic, how many samples and which conditions decide the < 1 s target is an open owner decision (OWNER DECISION REQUIRED, OQ-008); this step records the samples only. Read the browser statistics of Setup [K-12] at the start and end of each run. Record every element's latency setting (OQ-126). Optional: repeat once with `jitterBufferTarget` set on the viewer and record its effect, remembering that it is only a hint [K-11].
7. *(Added 2026-10-09; OQ-127.)* **Viewer join time.** Record the time from a viewer starting a session to the first decoded frame. On CM4, compare runs with and without an on-demand keyframe when a viewer joins [K-31]. Reasoning from [K-30]: with the default 60-frame GOP and no on-demand keyframe, a new viewer may wait up to about 2 s at 30 fps. Whether MediaMTX passes a viewer's keyframe request back to an RTSP publisher is undocumented (research open question, topic K; OQ-127).
8. *(Added 2026-10-09; OQ-128, RISK-033.)* **Internet viewers — only if OQ-008 puts them in scope.** Repeat step 6 from outside the LAN, behind NAT, once per path that will be offered: UDP to the forwarded port with the public address advertised [K-42], STUN hole punching, a TURN relay, and TCP if it is enabled [K-41], [K-43]. Record the path the session actually used (how to read it in the browser is NEEDS VERIFICATION, OQ-101) and the latency over a long run, because MediaMTX's configuration says TCP "introduces a progressive delay when network is congested" [K-41]. Record LAN and internet results separately.
9. *(Added 2026-10-09; OQ-117, RISK-028; OQ-126, RISK-032.)* **Live-path isolation.** Keep the step 6 measurement running during TEST-REC-001 step 5 (HDD stall injection), and record latency and frame rate before, during and after each stall. Also run once with an injected slow branch in the pipeline (OQ-126); the injection method is NEEDS VERIFICATION (BUILD TEST REQUIRED).

### Expected Result

- RFC 7742 requires, for H.264 in WebRTC [F-36]:
  - Constrained Baseline;
  - `packetization-mode` 1;
  - `profile-level-id` present in the SDP;
  - SPS/PPS sent in-band, not in `sprop-parameter-sets`.
- Level signalling:
  - `42e01f` is Constrained Baseline Level 3.1 (reasoning) [F-38], and libwebrtc assumes it when `profile-level-id` is absent [F-39].
  - Reasoning: 1080p needs Level 4 or above, and 1080p60 needs Level 4.2, so a strict `42e01f` negotiation does not cover 1080p [F-40].
  - Browser behaviour in that case is UNKNOWN (OQ-073).
- The MediaMTX project reports that browsers do not accept H.264 B-frames in WebRTC [F-45]. The Pi 4 / CM4 encoder produces none [D-14].
- Audio: RFC 7874 requires WebRTC endpoints to implement Opus and G.711; AAC is not a required WebRTC codec, so AAC audio has to be transcoded, typically to Opus, for browser playback [F-41].
- *(Added 2026-10-08.)* **Audio details:**
  - `opusenc` does not accept 44.1 kHz input; a 44.1 kHz source needs `audioresample` before it [I-47] ([TROUBLESHOOTING.md](TROUBLESHOOTING.md) §6.8). `opusenc` defaults to 64000 bit/s, constrained VBR [I-47].
  - Reasoning from [F-41] and [H-26]: if RTMP runs at the same time, the product needs two audio encodes, AAC for RTMP and Opus for WebRTC. *(2026-10-08, later: RTMP and WebRTC now share one live video encode (OQ-005), but audio is still two encodes, AAC and Opus.)*
  - Pitch and duration are correct only when the capture rate equals the source rate (reasoning) [I-18] (RISK-023). A/V tolerance: UNDEFINED (OQ-112).
- *(Added 2026-10-09; OQ-116, OQ-125, RISK-031.)* **Latency (steps 6 to 9):**
  - Target: under 1 s camera-to-viewer for WebRTC viewers (owner decision, 2026-10-08; OQ-116 ANSWERED; REQ-STR-002, acceptance DRAFT). How the target is judged (statistic, number of samples, conditions) is not defined: OWNER DECISION REQUIRED (OQ-008). Whether PACSCORDER meets it is UNKNOWN — HARDWARE TEST REQUIRED (OQ-125).
  - Reasoning (a labelled budget, not a measurement; CORRECTED) [K-45]: for a CM4 1080p30 viewer on a LAN, the documented or extrapolated terms come to about 56 ms typical and about 75 ms worst case — capture readout about 33.3 ms (a captured frame reaches userspace at least one frame's readout time after its first line on CM4 [K-34]), hardware encode about 23 ms, and no frames waiting in V4L2 or GStreamer queues, each of which would add 33.3 ms [K-36]. About 925–945 ms is left for the undocumented terms: HDMI source or ATEM, TC358743, any pixel-format conversion, MediaMTX relay, LAN, browser jitter buffer, decode and render. The encode term assumes the live encode has the CM4 hardware encoder to itself, but the recording encode shares it (OQ-115).
  - CM5: capture costs about one frame's readout time [K-35]; the encode term is unknown until the per-frame x264 time on BCM2712 is measured, as the reasoning entry [K-45] notes (OQ-059); Raspberry Pi documents that Pi 5 software encoders generally have longer latency than the old hardware encoders [K-39].
  - Undocumented terms: MediaMTX gives no numeric WebRTC latency figure [K-03]; no public source checked by the research documents the TC358743's internal buffering latency [K-38]. For reference only, not this stack: a third-party CDN documents WHEP playback "with less than 500 milliseconds of latency" [K-14].
  - Reasoning: a default left in place can exceed the budget on its own: `rtspclientsink`/`rtspsrc` 2000 ms [K-09], if it applies on the sending side (unknown, OQ-126); `x264enc` without explicit settings runs the medium preset with 3 B-frames, rc-lookahead 40 and MB-tree on (CORRECTED) [K-29], and with MB-tree on x264 holds back at least rc-lookahead frames [K-28] — 40 frames, about 1.33 s at 30p (40 ÷ 30) or about 0.67 s at 60p.
  - B-frames: MediaMTX documents that browsers deliberately do not support H.264 B-frames over WebRTC and recommends Baseline with Opus; its WebRTC audio codecs are Opus, G722 and G711 [K-04].
  - Packet loss: MediaMTX documents that it favours real-time delivery, so late packets can be dropped, and a full outgoing circular buffer (`writeQueueSize`, default 512) drops packets and logs "reader is too slow" [K-44].
  - Internet viewers: MediaMTX's configuration says TCP "introduces a progressive delay when network is congested" [K-41]; reachability needs the public address advertised or STUN/TURN [K-42]. Latency per path: UNKNOWN (research open question, topic K; OQ-128).
  - Isolation (step 9): ADR-009 requires that an HDD stall does not reach the live path; the expected latency change is none, but whether the design achieves that is UNKNOWN until measured (OQ-117, RISK-028).
- *(Added 2026-10-08.)* **H.265, per browser (deferred — REQ-ENC-002; not in current scope):**
  - RFC 7742 does not require H.265 in WebRTC [H-32].
  - Chrome 136 and later: enabled by default only where the platform decodes H.265 in hardware, with no software fallback [H-33].
  - Safari 18.0: supports the standard RFC HEVC RTP payload [H-34].
  - Firefox: no evidence of support was found [H-35].
  - Edge 147 on Windows 11: reported in a Microsoft Q&A answer by a moderator not to enable H.265 by default; the workaround given is the launch flags `--enable-features=WebRtcAllowH265Send,WebRtcAllowH265Receive` (community source) [H-36].
  - Reasoning (as in RISK-019): an H.264 WebRTC track has to remain for browser reach; on CM5 that means two concurrent software video encodes (RISK-022, OQ-108). *(2026-10-08, later: the current scope already requires CM5 to run two concurrent software H.264 encodes, recording and live; whether it sustains them is OQ-059 (OQ-005, RISK-003).)*

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-ATEM-001 — ATEM HDMI output capture (scope per OQ-009)

| | |
|---|---|
| Verifies | REQ-ATEM-001, REQ-CAP-008 |
| Related risks | RISK-008; RISK-018 applies only to part (b), which is not in current scope |
| Related open questions | OQ-009 (ANSWERED 2026-10-07), OQ-078, OQ-083, OQ-102. Not in current scope, kept for reference: OQ-075, OQ-077, OQ-079, OQ-080, OQ-081, OQ-082, OQ-084 |
| Depends on | TEST-CAP-001 and TEST-CAP-002 for (a) (TEST-CAP-002 on the 4-lane configuration only); TEST-STR-001 for (c), which is not in current scope |

### Objective

Confirm the ATEM integration in current scope: **capture of the ATEM switcher's HDMI output through the TC358743** — sub-test (a) below (REQ-ATEM-001, REQ-CAP-008).

Scope is the owner's answer to OQ-009 of 2026-10-07: "it can be atem and direct video from camera". Recorded interpretation (Claude; the owner may correct it):

- In scope: (a) HDMI capture of the ATEM output. Cameras connected directly are the other source type of REQ-CAP-008; they are covered by TEST-CAP-001, TEST-CAP-002 and TEST-CAP-004, not here.
- **Not in current scope:** (b) network tally, state and control over UDP 9910, and (c) receiving the ATEM's RTMP stream. Both were offered and not selected. Their procedures are kept below as reference only and are run only if the owner adds them to REQ-ATEM-001. Sending PACSCORDER video into an ATEM setup (OQ-081) and the ATEM USB-C webcam output (OQ-082) were not selected either.
- Which ATEM models and firmware versions must be supported is OPEN (OQ-102). *(Superseded 2026-10-08: OQ-102 was ANSWERED on 2026-10-07 with no model list — any HDMI camera plus ATEM switcher outputs. The test uses whichever ATEM is available as the representative ATEM source.)*

### Setup

- ATEM model and firmware version: UNKNOWN — OWNER DECISION REQUIRED (OQ-102). Record both in every result entry. *(2026-10-08: no owner decision is pending, since OQ-102 is ANSWERED with no model list; the model and firmware are still UNKNOWN until an ATEM is chosen for the test, and must still be recorded.)*
- (a) **HDMI capture of the ATEM output** (in scope).
  - The ATEM Mini Pro HDMI output defaults to multiview, not program. It is changed with the front-panel VIDEO OUT buttons or in ATEM Software Control [F-25]. On ATEM Mini Extreme, HDMI out 1 defaults to program [F-25].
  - Whether PACSCORDER could switch it over the network is OQ-078. Network control is not in current scope, so this procedure has the operator set the output source.
  - Lane configurations: run on both the 2-lane and the 4-lane configuration (REQ-CAP-007). Reasoning from [F-23] and the TEST-CAP-004 matrix: on the 2-lane configuration the ATEM Mini Pro's 1080p59.94 and 1080p60 standards cannot be captured, and 1080p50 only in UYVY [C-37], [C-48]. Whether the ATEM honours the sink EDID or always outputs its own video standard is UNKNOWN (OQ-083).
- (b) **Network state and control (UDP 9910) — not in current scope (reference only).** The protocol is reported as reverse-engineered by the OpenSwitcher project [F-11]; the official SDK does not document it [F-10]. The library is UNDEFINED (OQ-084).
- (c) **Receiving the ATEM's RTMP stream — not in current scope (reference only).** Reasoning: PACSCORDER must run a listening RTMP server [F-46]; GStreamer `rtmp2src` cannot accept incoming publish connections [F-33].

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

- (a) Set the ATEM HDMI output to Program [F-25]. Run TEST-CAP-001 against it at each output standard of the ATEM model(s) listed in OQ-102 *(2026-10-08: of the ATEM used, since OQ-102 has no model list)*. Run TEST-CAP-002 at 1080p60 on the 4-lane configuration, and complete the ATEM rows of the TEST-CAP-004 matrix on both lane configurations.
- (b) *Not in current scope — reference only.* Connect the chosen library. Change program/preview inputs, start and stop recording and streaming on the ATEM, and record the state PACSCORDER receives. Commands depend on OQ-084: NEEDS VERIFICATION.
- (c) *Not in current scope — reference only.* Configure the ATEM to publish to the PACSCORDER server (method OQ-080), receive the stream, and analyse its parameters (OQ-079).

### Expected Result

**(a) HDMI capture** (in scope)

- Frames at the ATEM output standard. ATEM Mini Pro outputs 1080p23.98 to 1080p60 only, with no 1080i or 720p [F-23].
- On the 2-lane configuration, only the standards the link carries are captured; the others are expected to fail at stream start as in TEST-CAP-004 [B-32], [C-48].
- What arrives at the TC358743 is UNKNOWN (OQ-083): pixel encoding, quantisation range, and whether HDCP is asserted. The ATEM's own sampling is 4:2:2 YUV 10-bit Rec 709 [F-23].
- Program audio is embedded on the HDMI output (CORRECTED) [F-24]. Capturing it needs TEST-AUD-001, which applies only if audio is required (OQ-004). *(Superseded 2026-10-08: audio is required — OQ-004 ANSWERED 2026-10-07 — so TEST-AUD-001 is run with the ATEM as one of its sources. What audio format and rate the ATEM's HDMI output sends, and whether it honours the EDID's audio descriptors, is UNKNOWN: research open question, topic I; OQ-110, OQ-083.)*

**(b) Network state** (not in current scope — reference only)

- Tally and state changes are received.
- The atem-connection project reports:
  - tally via `TlSr` [F-17];
  - recording and streaming status commands that need protocol 2.30 or later [F-16];
  - protocol versions defined only up to V9_6 [F-18], while Blackmagic lists ATEM 10.4.1 software [F-02].
- Compatibility with current firmware is UNKNOWN (OQ-077, RISK-018).

**(c) RTMP from the ATEM** (not in current scope — reference only)

A stream with H.264 or H.265 video and AAC audio [F-06]. The exact parameters are UNKNOWN (OQ-079).

**All sub-tests**

Pass criteria are UNDEFINED (OQ-017). The scope question OQ-009 is answered; the ATEM model list is OQ-102. *(2026-10-08: OQ-102 is also answered — no model list.)*

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-BLD-001 — Image build from clean checkout

| | |
|---|---|
| Verifies | REQ-BLD-001, REQ-BLD-002 |
| Related risks | RISK-017, RISK-015 |
| Related open questions | OQ-012 (ANSWERED 2026-10-07), OQ-064, OQ-065, OQ-066, OQ-067, OQ-068, OQ-071, OQ-101 |
| Depends on | A build configuration in this repository (none exists) |

### Objective

Build the PACSCORDER product image from a clean checkout of this repository, by the documented procedure, with pinned kernel, firmware and package versions. Confirm that a later rebuild produces the same image content (REQ-BLD-001).

Confirm also that the product runs this project-built image of its own, not an unmodified stock distribution image (REQ-BLD-002, DRAFT; owner, 2026-10-07: "which is best i need by own one"), and that it boots on the platforms of both lane configurations (REQ-CAP-007).

### Setup

- Build basis: ADR-003 is ACCEPTED (owner, 2026-10-07: "accept ADR-003"; OQ-012 ANSWERED). The product image is built from Raspberry Pi OS packages into the project's own image with `rpi-image-gen`, with a package mirror for reproducibility; Buildroot is the documented alternative, re-evaluated only if measured boot time, image size or reproducibility fails a requirement ([BUILD_SYSTEM.md](BUILD_SYSTEM.md)). Both produce a project-owned image as REQ-BLD-002 requires. The owner delegated the tool choice to Claude's recommendation on 2026-10-07 and accepted ADR-003 the same day.
- `rpi-image-gen`:
  - latest release v2.8.0 [G-37];
  - builds from YAML configs and layers [G-38];
  - supports only native Debian Bookworm or Trixie arm64 hosts; containers and QEMU are "not formally supported" [G-39];
  - minor releases contain breaking changes [G-44].
- Build host: UNKNOWN (OQ-067).
- Package pinning: a mirror or snapshot of `archive.raspberrypi.com` is needed (RISK-017, OQ-067).
- Build configuration in this repository: **none exists** (NOT STARTED).

### Test

**NOT YET RUN.** No build configuration exists, and no register fact gives the `rpi-image-gen` command line, so commands are NEEDS VERIFICATION (OQ-101).

1. Make a clean checkout and run the documented build.
2. Record, as listed in [BUILD_SYSTEM.md](BUILD_SYSTEM.md) §6:
   - the build host OS, architecture and tool versions;
   - the pinned inputs: the `rpi-image-gen` tag or Buildroot release, the kernel and firmware versions, and the package-mirror snapshot;
   - the SBOM and the image checksum.

   The commands that read kernel, firmware and EEPROM versions are NEEDS VERIFICATION (OQ-101).
3. Rebuild later from the same commit and compare SBOMs and image contents.
4. Boot the image on each candidate platform, covering at least one platform of each lane configuration (REQ-CAP-007), and run TEST-PLT-001. Record that the booted image is the one built in step 1 (its checksum from step 2), not the stock bring-up image (REQ-BLD-002). The command that confirms this on the running system is NEEDS VERIFICATION (OQ-101).
5. Check that the image contains:
   - the `tc358743` module for both kernels;
   - the `tc358743` / `tc358743-pi5` / `tc358743-audio` overlays;
   - v4l-utils;
   - the chosen media framework;
   - `i2c-tools`, if TEST-HW-001 is to be run on product images;
   - *(added 2026-10-08)* the H.265 encoder (`libx265-215` and `x265enc` or FFmpeg `libx265`) [H-01], [H-09], [H-11] *(later on 2026-10-08: deferred — REQ-ENC-002; check it only if REQ-ENC-002 is re-activated. `libx265-215` is still pulled in as a dependency of the Raspberry Pi FFmpeg's `libavcodec61` [H-09], so its presence does not mean H.265 is used)*; the chosen AAC and Opus encoders [I-39], [I-44], [I-45], [I-46] (OQ-063); the ALSA capture path for the `tc358743` card. List the encoders the image's FFmpeg actually has: research inferred them from package dependencies, not from the build configuration (research gap, topic I — not a register fact). The listing command is NEEDS VERIFICATION (OQ-101). If the product carries a package from outside the distribution (for example a newer x265 or GStreamer; OQ-105, OQ-107), record its version and source. *(Both examples arise from H.265, which is deferred — REQ-ENC-002; OQ-105 and OQ-107 are not in current scope. The instruction to record any outside package still applies.)*

### Expected Result

- The build completes on a supported host [G-39] and produces an SBOM [G-36], [G-39].
- Image contents. The stock 2026-10-06 Lite image already has:
  - `tc358743.ko.xz` for both kernels [G-16];
  - Unicam, RP1 CFE and `bcm2835-codec` modules (built as modules in both defconfigs [G-17], [G-18]; present in the image's module trees per reasoning on verified inputs, CORRECTED [G-71]);
  - `v4l2-ctl` / `media-ctl` [G-24].

  It does **not** have `i2c-tools` [G-25] or GStreamer [G-32]. The product configuration must add whatever the chosen framework needs.
- One image can boot all four candidates (reasoning, CORRECTED) [G-71]. Reasoning: one product image can therefore serve the platforms of both lane configurations (REQ-CAP-007), although the overlay, its `cam0`/`4lane` parameters and the capture model still differ per board [G-71].
- The booted system is the project-built image of step 1, identified by its recorded checksum and SBOM (REQ-BLD-002).
- A rebuild produces the same package set. Reasoning: this needs pinning, because `apt full-upgrade` changes the kernel and firmware [G-08], and the default kernel series changed in the middle of the trixie release [G-05].
- If Buildroot were chosen instead (OQ-064, OQ-065, OQ-066):
  - it pins kernel 6.12.61 [E-06];
  - its Pi 5 defconfig installs no overlays [E-15], [G-61];
  - `v4l2h264enc` needs `BR2_PACKAGE_GST1_PLUGINS_GOOD_PLUGIN_V4L2_PROBE=y` [E-32], [D-39].
- Image size, RAM use and boot-to-first-frame targets: UNDEFINED (OQ-068).

### Actual Result

NOT STARTED — no build configuration exists; not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## TEST-PERF-001 — Soak: thermal, CPU, CMA, frame drops

| | |
|---|---|
| Verifies | REQ-PERF-001 |
| Related risks | RISK-003, RISK-006, RISK-020; added 2026-10-08: RISK-022 (H.265 soak; not in current scope — H.265 deferred, REQ-ENC-002), RISK-024 (A/V drift); added later on 2026-10-08: RISK-002 (two encodes on the CM4 hardware encoder); added 2026-10-09: RISK-026, RISK-027, RISK-028 (mirrored recording under sustained load, ADR-009) |
| Related open questions | OQ-010, OQ-035, OQ-053, OQ-055, OQ-059, OQ-061, OQ-096; added 2026-10-08: OQ-104 (not in current scope — H.265 deferred, REQ-ENC-002), OQ-112; added later on 2026-10-08: OQ-005, OQ-115 (combined load of two encodes); added 2026-10-09: OQ-117, OQ-121, OQ-122 (mirrored recording, ADR-009) |
| Depends on | TEST-ENC-001 and the selected outputs (TEST-REC-001, TEST-STR-001, TEST-STR-002); added 2026-10-08: TEST-AUD-001 |

### Objective

Run the full pipeline (capture, encode, and record and/or stream) continuously across the operating temperature range. Measure frame drops, CPU load, temperatures and contiguous-memory (CMA) use.

*(Added later on 2026-10-08.)* The owner's answer to OQ-005 ("Separate record + live") defines the full pipeline:
- one capture;
- two H.264 encodes: the recording encode, and the live encode shared by RTMP and WebRTC;
- two audio encodes: AAC for recording and RTMP, Opus for WebRTC;
- all three outputs.

This test measures that combined load on CM4 and CM5.

### Setup

- Full pipeline as selected by the owner. *(Later on 2026-10-08: selected — "Separate record + live", OQ-005. Video: capture → recording encode → recording, and capture → live encode → RTMP and WebRTC. Audio: HDMI audio → AAC (recording, RTMP) and Opus (WebRTC). Bitrates and latency targets are still open (OQ-005).)*
- Soak duration, frame-drop threshold, ambient temperature range and enclosure: UNDEFINED — OWNER DECISION REQUIRED (OQ-010).
- Temperature chamber: required if OQ-010 sets a range beyond room temperature.

### Test

**NOT YET RUN ON PACSCORDER HARDWARE.**

1. Start the pipeline at the highest required mode.
2. Sample `CmaFree` from `/proc/meminfo` while streaming at maximum load. This is the measurement named in the reasoning entry [C-53].
3. Sample per-core CPU load, SoC temperature, frame drops and CSI-2 receiver errors at a regular interval. Commands are NEEDS VERIFICATION. How to read CSI-2 receive errors is UNKNOWN on both receivers (RP1 CFE: OQ-050; Unicam: OQ-095).
4. Pi 4 / CM4: if the owner allows an overclocked configuration to be evaluated, soak it as well; whether an overclock is acceptable in the product is OQ-096.
5. Repeat at the temperature extremes from OQ-010.
6. Pi 5 / CM5: if both OS options are evaluated, repeat on both page sizes. Raspberry Pi OS's default bcm2712 kernel uses 16K pages; Buildroot's Pi 5/CM5 defconfigs force 4K pages [E-09], [G-60] (OQ-055).
7. *(Added 2026-10-08.)* Run the soak on **CM4 and CM5**, with the HDMI audio path active (REQ-CAP-006) and with the H.265 encodes the owner scopes (OQ-103), alongside the H.264 encodes. Record per-core CPU load and throttling with the software H.265 encode running (OQ-104; RISK-022). *(Superseded later on 2026-10-08: OQ-103 is ANSWERED — "H.264 only for now". In the current scope, run the soak on CM4 and CM5 with the HDMI audio path active and the H.264 encodes only. The H.265 soak is deferred — REQ-ENC-002; not run in current scope.)*
8. *(Added 2026-10-08.)* **A/V drift (RISK-024, OQ-112).** At the start and end of the soak, and at regular intervals, measure the audio/video offset in the recorded and streamed outputs with the marker source from TEST-AUD-001 step 9. Record the framework and its clock settings [I-35], [I-38]. Commands are NEEDS VERIFICATION (OQ-101).
9. *(Added later on 2026-10-08.)* **Combined load (OQ-005).** On CM4 and CM5, run recording, RTMP publishing and a WebRTC session at the same time from one capture: two H.264 encodes (recording and live) and two audio encodes (AAC and Opus). The "H.264 encodes" of step 7 are these two.
   - Record, for each encode: output frame rate and dropped frames.
   - Record, for the whole system: per-core CPU load, SoC temperature, throttling and `CmaFree`.
   - Run at the highest required mode (step 1) and at the combinations that the TEST-ENC-001 two-encode run sustained.
   - CM4: OQ-115, RISK-002. CM5: OQ-059, RISK-003. Commands are NEEDS VERIFICATION (OQ-101).
10. *(Added 2026-10-09; ADR-009 / OQ-116 / research topics J and K.)* **Mirrored recording in the soak.** The recording output of step 9 is ADR-009's: fragmented MP4 written to the NVMe SSD and the HDD at the same time, with the storage set up as in TEST-REC-001 (on CM4 the IO Board powered from +12 V [J-05]). During the soak:
    - inspect the kernel log for USB resets, UAS errors, NVMe errors and I/O errors (RISK-027, OQ-122, OQ-121);
    - record whether the HDD-branch buffer ever overflowed and whether either copy has a gap (OQ-117, RISK-028);
    - on CM4, record the other USB devices sharing the hub (RISK-026);
    - sample the WebRTC camera-to-viewer latency at intervals, as in TEST-STR-002 step 6 (OQ-125). The < 1 s target applies to WebRTC viewers (OQ-116 ANSWERED); its behaviour over a long run is unmeasured.

### Expected Result

**CMA**

- Reasoning, CORRECTED [C-53]: four 1080p capture buffers take about 16.6 MB in UYVY or 24.9 MB in RGB888.
- The DT CMA pool is 64 MB, limited to the lower 768 MB on Pi 4 / CM4 and the lower 1 GB on Pi 5 (CORRECTED) [E-47].
- `vc4-kms-v3d-pi4` sets (512 − 4) MB and `vc4-kms-v3d-pi5` sets 64 MB [C-40].
- Whether Pi 5 / CM5 CFE buffers count against CMA is not established (reasoning from kernel source, CORRECTED) [C-53] (OQ-053).
- `CmaFree` must stay above zero (RISK-020). The margin is UNDEFINED (OQ-061).
- *(Added later on 2026-10-08.)* With two encoders, the number of capture and encoder buffers in flight, and so the CMA use, is UNKNOWN until measured (OQ-061; TEST-DMA-001).

**Thermal**

The TC358743XBG is rated −30 to +70 °C ambient [A-41]. Pi thermal limits were not researched: UNKNOWN.

**Capture stability**

- A Raspberry Pi engineer reported that the driver's FIFO trigger level of 374 is an empirical value [A-43]. The driver hard-codes it, with a comment that it suits most modes at 972 Mbps [B-12], [C-44].
- D-PHY timing constants exist only for 594 and 972 Mbps [B-09].
- Long-run and temperature behaviour is therefore UNKNOWN (OQ-035, RISK-006).

**CPU**

Pi 5 / CM5 encode is CPU-bound: about 30–40 % CPU for 1080p30 (official) [G-22] (OQ-059, RISK-003).

*(Added later on 2026-10-08.)* **Two encodes (OQ-005).**
- CM5: reasoning from [G-22]: roughly twice the encode CPU of one encode. Add the UYVY → planar conversion (OQ-060) and the two audio encodes, whose cost is UNKNOWN (OQ-063).
- CM4: the two video encodes share the hardware encoder. Reasoning: two 1080p30 encodes need about 2.0× the specified macroblock rate [D-10], [D-52] (OQ-115, RISK-002).
- No measured value exists for either board.

*(Added 2026-10-08.)* H.265 is software-encoded on CM4 and CM5 [D-24], [D-31]; its sustained cost on either board is unmeasured (OQ-104). A Raspberry Pi engineer stated on the forum that software H.265 encode "is too intensive an operation to perform at any significant resolution" (community source) [H-19]. *(Deferred — REQ-ENC-002; not in current scope: H.264 only for now, OQ-103 ANSWERED 2026-10-08.)*

**A/V drift** *(added 2026-10-08)*

Audio follows the HDMI source's clock through the TC358743's audio PLL [I-26], while video buffers are stamped with `CLOCK_MONOTONIC` [I-33]. Reasoning (as in RISK-024): the offset can therefore drift over a long run; the size of the drift is UNKNOWN until measured, and the tolerance is UNDEFINED (OQ-112). A sample-rate mismatch adds drift (reasoning) [I-18] (RISK-023).

**Storage** *(added 2026-10-09)*

Reasoning: the mirrored recording uses a small share of every storage interface on both boards [J-37], so Claude's reading (as in OQ-117) is that storage problems in a soak are expected from stalls, resets and errors rather than from bandwidth. A Raspberry Pi engineer's forum post reports that some UAS devices stop responding or, rarely, throw write data away (community source, CORRECTED) [J-27] (RISK-027). No measured value exists for either board.

**Thresholds**

UNDEFINED (OQ-010, OQ-017).

### Actual Result

BLOCKED — HARDWARE REQUIRED — not run

### Date

—

### Hardware Revision

—

### Software Version

—

---

## Documentation checks

Documentation consistency checks are recorded in [DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md), not here. Examples are cross-checking that every cited fact ID exists in [REFERENCES.md](REFERENCES.md), and that REQ, ADR, RISK, OQ and TEST IDs agree across documents.

These checks are not product tests: they have no TEST ID and do not change the status of any requirement.

## Test result log

**Append-only (Rule 21).** Add one row per run. Never edit or delete a row; correct a mistake with a new row that references the earlier one.

- "Result" uses only the Rule 10 status words.
- "Evidence" points to the stored logs and measurements.
- Record the actual device nodes, entity names, kernel version and `config.txt` lines in the evidence. For CM5, also record the carrier board (OQ-052). The commands that read kernel, firmware and EEPROM versions are NEEDS VERIFICATION (OQ-101); record the command actually used.

| Date | Test ID | Platform | Hardware revision | Software version | Result | Evidence | By |
|---|---|---|---|---|---|---|---|

No results recorded as of 2026-10-06.

---

## Verification status

### Verified from sources (fact IDs)

The procedures and expected results above rest on these entries of [REFERENCES.md](REFERENCES.md). All of them have verdict `CONFIRMED` or `CORRECTED`; CORRECTED entries are used in their corrected wording only.

| Topic | Fact IDs cited |
|---|---|
| A — TC358743 hardware | A-04, A-05, A-08, A-11, A-12, A-13, A-15, A-16, A-17, A-18, A-19, A-20, A-22, A-23, A-24, A-30, A-31, A-33, A-35, A-41, A-43, A-45, A-46, A-47, A-50 |
| B — tc358743 Linux driver | B-06, B-07, B-09, B-10, B-11, B-12, B-14, B-15, B-16, B-18, B-19, B-21, B-22, B-23, B-24, B-25, B-26, B-27, B-28, B-29, B-30, B-32, B-33, B-34, B-37, B-38, B-39, B-40, B-41, B-42, B-43, B-44, B-46, B-48, B-49 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-08, C-09, C-10, C-11, C-12, C-15, C-16, C-17, C-18, C-19, C-20, C-23, C-24, C-25, C-26, C-27, C-28, C-29, C-31, C-32, C-33, C-34, C-35, C-36, C-37, C-39, C-40, C-41, C-42, C-43, C-44, C-45, C-46, C-47, C-48, C-49, C-50, C-51, C-52, C-53 |
| D — Encoders | D-06, D-07, D-08, D-10, D-11, D-12, D-13, D-14, D-15, D-16, D-17, D-18, D-19, D-20, D-21, D-22, D-23, D-24, D-29, D-30, D-31, D-32, D-34, D-35, D-36, D-37, D-39, D-40, D-43, D-44, D-45, D-48, D-50, D-52, D-53, D-54 (D-11 and D-35 added later on 2026-10-08, two-encode procedures) |
| E — Buildroot and kernel configuration | E-06, E-09, E-15, E-32, E-37, E-39, E-40, E-43, E-44, E-47, E-48 |
| F — ATEM and streaming | F-02, F-06, F-07, F-10, F-11, F-16, F-17, F-18, F-23, F-24, F-25, F-26, F-31, F-32, F-33, F-34, F-35, F-36, F-38, F-39, F-40, F-41, F-42, F-44, F-45, F-46 |
| G — Raspberry Pi OS and image tooling | G-01, G-04, G-05, G-08, G-11, G-12, G-13, G-14, G-15, G-16, G-17, G-18, G-21, G-22, G-24, G-25, G-26, G-27, G-28, G-29, G-32, G-36, G-37, G-38, G-39, G-40, G-44, G-47, G-48, G-60, G-61, G-71 |
| H — H.265 software encoding and transport (added 2026-10-08) | H-01, H-02, H-03, H-04, H-05, H-07, H-08, H-09, H-10, H-11, H-12, H-13, H-14, H-15, H-16, H-17, H-19, H-20, H-21, H-22, H-23, H-24, H-26, H-27, H-28, H-29, H-30, H-31, H-32, H-33, H-34, H-35, H-36, H-37, H-38, H-43 |
| I — HDMI audio path (added 2026-10-08) | I-02, I-03, I-04, I-05, I-06, I-07, I-08, I-09, I-10, I-11, I-12, I-13, I-14, I-15, I-16, I-17, I-18, I-19, I-20, I-21, I-22, I-23, I-24, I-25, I-26, I-27, I-28, I-29, I-30, I-31, I-32, I-33, I-35, I-36, I-37, I-38, I-39, I-40, I-41, I-42, I-44, I-45, I-46, I-47 |
| J — Recording storage and power loss (research of 2026-10-08; cited from 2026-10-09) | J-01, J-02, J-03, J-05, J-06, J-07, J-08, J-09, J-11, J-13, J-14, J-15, J-17, J-18, J-19, J-20, J-21, J-24, J-25, J-26, J-27, J-28, J-29, J-30, J-31, J-32, J-33, J-35, J-36, J-37, J-38, J-39, J-41, J-44, J-45 |
| K — Live latency (research of 2026-10-08; cited from 2026-10-09) | K-01, K-02, K-03, K-04, K-05, K-06, K-07, K-08, K-09, K-10, K-11, K-12, K-14, K-15, K-16, K-17, K-18, K-27, K-28, K-29, K-30, K-31, K-32, K-33, K-34, K-35, K-36, K-37, K-38, K-39, K-41, K-42, K-43, K-44, K-45 |

- `CORRECTED` entries cited: A-22, B-11, B-21, B-25, B-44, C-28, C-36, C-39, C-53, E-40, E-47, F-24, F-34, G-11, G-36, G-71; added 2026-10-08: H-10, H-12, I-40; added 2026-10-09: J-09, J-27, J-30, J-33, J-35, K-12, K-18, K-29, K-45.
- `community` entries cited, worded as reports: A-43, C-28, C-33, C-35, C-41, C-42, C-43, C-45, D-17, D-18, D-50, D-54, F-11, F-16, F-17, F-18, F-44, F-45; added 2026-10-08: H-19, H-20, H-21, H-22, H-36, I-16; added 2026-10-09: J-27, K-33.
- `reasoning` entries cited, labelled as reasoning: A-23, B-10, B-11, B-27, B-33, B-49, C-46, C-47, C-48, C-49, C-50, C-51, C-52, C-53, D-29, D-36, D-52, F-35, F-38, F-40, F-46, G-71; added 2026-10-08: H-23, H-43, I-17, I-18, I-28; added 2026-10-09: J-09, J-31, J-36, J-37, K-10, K-18, K-45, and the reasoning sentence of the `kernel-source` entry K-36. [K-14] is a third-party CDN figure, cited as a reference only.
- Items marked *research gap* or *research open question* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). They are not register facts. Items added on 2026-10-08 and marked *research gap*, *research open question* or *research design risk* with topic H or I come from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json); they are not register facts either. Steps derived from them (for example the suggested `arecord` listing options and the x265 CPU-feature check) are NEEDS VERIFICATION. *(Added 2026-10-09.)* Items marked *research gap*, *research open question* or *research design risk* with topic J or K come from [research/2026-10-08-storage-latency-research.json](research/2026-10-08-storage-latency-research.json) and are not register facts: the suggested `lsusb -t`, `lspci` and `hdparm -B/-S` checks, the encoder queue-to-dequeue timing, the flashing-timecode latency method (the filming arrangement in TEST-STR-002 Setup is Claude's proposal), the drives' volatile write caches, staying at PCIe Gen 2, the HDD-branch queue, the unchecked muxer options on the shipped builds, firmware enablement of the CM5 M.2 link, and the open points on `rtspclientsink`, keyframe-request forwarding and YouTube's acceptance of a B-frame-free stream. Steps derived from them are NEEDS VERIFICATION.
- "Verified from sources" means only that the cited source says so. Under Rule 23, a hardware measurement overrides any of these facts.

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-06). No procedure in this document has been run, and no command in it has been executed on PACSCORDER hardware.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review: combined/bare `dtoverlay` parameter syntax marked NEEDS VERIFICATION (consistent with DEVICE_TREE.md §6.0); EDID-persistence statement re-sourced to [A-31], [B-21] plus a research gap instead of [B-22]; C-28 entity-name wording, C-50 margin wording, WebRTC audio wording (F-41) and encoder UYVY scope (D-40, D-43) corrected; defconfig-versus-image distinction added for G-17/G-18 with [G-71]; page-size citations [E-09], [G-60] and CHIPID citation [A-18] added; 22-pin adapter hazard extended to CM4/CM5 IO Boards as reasoning; related-OQ lists completed from the "Resolving test" fields of OPEN_QUESTIONS.md (TEST-CAP-001, TEST-CAP-002, TEST-PLT-001, TEST-DMA-001, TEST-ENC-001, TEST-STR-002, TEST-ATEM-001) and RISK-012 added to TEST-CAP-001 and TEST-CAP-003 per RISKS.md. | Claude (session 2026-10-06, review) |
| 2026-10-06 | Cross-document consistency fixes: Actual Result of unrun hardware tests now `BLOCKED — HARDWARE REQUIRED — not run` (no "NOT RUN" status; TEST-BLD-001 `NOT STARTED`); `v4l2-ctl -d /dev/v4l-subdevN` and `--clear-edid 0` marked NEEDS VERIFICATION (OQ-101), other unattested command steps linked to OQ-101; bare/combined `config.txt` parameters marked per line (OQ-100); Pi 5 line names `tc358743-pi5` and CM5 split by carrier board with the OQ-052 caveat (no line on the CM4 IO Board), also in TEST-PLT-001; kernel 6.18.39 labelled as the kernel of the [C-33] report only, with 6.18.50 [G-04] / 6.18.55 [E-37] and OQ-097; TEST-CAP-002 notes CM4 CAM0 is 2-lane, that a 4-lane port is necessary but not shown sufficient (OQ-038), and the ADR-008 (PROPOSED; OQ-099) link-frequency proposal and CM4 CAM1 297 MHz evaluation run; TEST-CAP-004 link-frequency text and matrix aligned with ADR-008 and RISK-006; CSI-2 error-counter method linked to OQ-050/OQ-095; TEST-ENC-001 preconditions (TEST-CAP-002 and TEST-DMA-001) aligned with PERFORMANCE.md §10.3, 2-lane 1080p60 limit added [C-01], [C-02], [C-48], optional `gpu_freq=550` run (OQ-056) and overclock acceptability (OQ-096); [D-50] statements attributed to one engineer; TEST-AUD-001 notes audio over CSI-2 [A-05] versus the driver's I2S choice [A-13], with the camera-connector point labelled reasoning; WebRTC audio "typically to Opus" [F-41]; EDID trigger/reload linked to OQ-093; storage facts OQ-098; TEST-BLD-001 evidence list synced with BUILD_SYSTEM.md §6; related-OQ lists extended with OQ-093, OQ-095, OQ-096, OQ-099, OQ-100, OQ-101 per their Resolving-test fields. Final verification pass (same date): TEST-CAP-004 objective notes that the STREAMON lane check compares against the DT `data-lanes` value, not the wiring [B-32], [C-16] (matches REQ-CAP-005 note and CSI_PIPELINE.md §5). | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: "Verifies" (summary and test sections) matched to the README canonical table (TEST-CAP-001, -002, -004, TEST-ATEM-001, TEST-BLD-001 gain REQ-CAP-007/-008, REQ-BLD-002); common setup and test order cover the 2-lane and 4-lane configurations (REQ-CAP-007), ATEM and camera sources (REQ-CAP-008, OQ-102) and the own product image (REQ-BLD-002); TEST-CAP-002 scoped to the 4-lane configuration, 2-lane platforms kept as REQ-CAP-007 candidates; TEST-CAP-004 matrix extended to both configurations, both source types and the other required rates (reasoning [B-28], [B-33], [C-46], [C-47]; [C-17]); TEST-ENC-001 no longer frames OQ-001 as open; TEST-ATEM-001 scope = HDMI capture of the ATEM output (OQ-009 answered), network and RTMP parts marked not in current scope; C-17 and C-46 added to the verification table. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); "Applies to" header, common setup (OS image row), test order and TEST-BLD-001 setup and related open questions now say ADR-003 ACCEPTED and OQ-012 ANSWERED (Buildroot kept as the documented alternative; build configuration still NOT STARTED); TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)" in the summary table and section heading, ID unchanged. | Claude (session 2026-10-07) |
| 2026-10-08 | Second set of owner decisions of 2026-10-07 and research topics H (H.265) and I (HDMI audio) propagated; no test run, no status changed, no step deleted. Header "Applies to" and "Verification" rows: CM4 + CM5 side-by-side bring-up (ADR-004 OPEN), audio required (REQ-CAP-006 DRAFT), H.264 + H.265 (REQ-ENC-001, OQ-103), any HDMI camera plus ATEM (OQ-102 ANSWERED). Conventions: audio/H.265 command attestation note (OQ-101). Common setup: platform row (run on CM4 and CM5), HDMI-source row (OQ-102 statement marked superseded), new encode/audio/streaming package row. Platform reference: rows for H.265 encoder, x265 SIMD paths, HDMI audio CPU DAI, control location and expected PCM name. Test order: TEST-AUD-001 "only if audio is required" marked superseded; A/V-offset and audio-output dependencies added; TEST-ENC-001 H.265 scope note (README title unchanged). TEST-CAP-001/-002/-004 and TEST-ATEM-001: OQ-102 statements marked superseded; ATEM audio now via TEST-AUD-001. TEST-AUD-001: full procedure (card `tc358743`, hardware parameters, sampling-rate and audio-present controls, capture at the source rate, 44.1/48 kHz rate-mismatch check, rate change with `V4L2_EVENT_CTRL`, compressed/multichannel/24-bit sources, A/V offset) on CM4 and CM5; precondition marked superseded; original four steps kept; expected results per board, with CM5 as a bring-up gate (OQ-054); RISK-023, RISK-024, OQ-110 to OQ-112, OQ-114 linked. TEST-DMA-001: H.265 conversion note. TEST-ENC-001: H.265 setup, runs (x265 CPU features, GStreamer and FFmpeg, run matrix, conversion cost, sustained run) and expected results on CM4 and CM5; codec statement marked partly superseded; RISK-022, OQ-103 to OQ-105 linked. TEST-REC-001: HEVC containers and AAC audio; "none registered" risks marked superseded. TEST-STR-001: HEVC over RTMP (FFmpeg enhanced FLV, `flvmux` cannot carry H.265, OQ-106, OQ-107, RISK-025) and AAC audio expectations. TEST-STR-002: Opus rates and resampling, H.265 per-browser expectations (OQ-108); "if audio is required" step marked superseded. TEST-BLD-001: H.265 and audio encoder contents. TEST-PERF-001: H.265 soak and A/V drift steps. Verification table: topics H and I rows, D-30; CORRECTED H-10, H-12, I-40; community H-19 to H-22, H-36, I-16; reasoning H-23, H-43, I-17, I-18, I-28; 2026-10-08 research JSON items labelled. | Claude (session 2026-10-08) |
| 2026-10-08 | Citation verification of the topic H and I additions: TEST-AUD-001 step 5 — "a 32-bit sample format, which both CPU DAIs accept [I-13], [I-14]" overstated CM5; reworded to CM4 accepting S32_LE [I-13] and CM5's RP1 I2S1 formats coming from hardware registers, so the run uses what step 3 reports [I-14]. All other [H-xx] and [I-xx] citations checked against the register; no change needed. No test run; no status changed. | Claude (session 2026-10-08) |
| 2026-10-08 | TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (owner chose H.264 + H.265 on 2026-10-07); ID unchanged. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferred (owner: "H.264 only for now", OQ-103; REQ-ENC-002): header "Applies to" codec statement marked superseded (H.264 only); TEST-ENC-001 retitled "Sustained real-time H.264 encode (H.265 deferred)" in the summary table and section heading (README title; ID unchanged); encode-package row notes x265enc/libx265/x265 are needed only if REQ-ENC-002 is re-activated, that `libx265-215` is still pulled in by the Raspberry Pi FFmpeg's `libavcodec61` [H-09] and that `x265enc` ships in `gstreamer1.0-plugins-bad` with `rtmp2sink`/`webrtcbin` [H-11], [H-12], [G-27]; platform-reference H.265 encoder and x265 SIMD rows labelled deferred; test-order H.265 note marked superseded; TEST-DMA-001 H.265 note deferred; TEST-ENC-001 — related RISK-022, OQ-104, OQ-105 marked not in current scope and OQ-103 ANSWERED, superseding scope note (H.264 only in current scope), codec note superseded, H.265 encoders, H.265 runs and H.265 expected results labelled "deferred — REQ-ENC-002; not in current scope" (runs: not run in current scope), OQ-103 expectation superseded; TEST-REC-001, TEST-STR-001, TEST-STR-002 — related risks/OQs annotated, H.265 objective and codec statements superseded (H.264 only), H.265 setup, runs and expectations labelled deferred, not run in current scope; TEST-BLD-001 step 5 H.265 encoder check only if REQ-ENC-002 is re-activated, outside-package examples noted as H.265-driven; TEST-PERF-001 — RISK-022, OQ-104 not in current scope, step 7 superseded (H.264 encodes only), H.265 CPU note deferred. All H.265 research and evidence kept; no step deleted; no test run; no status, test ID or "Verifies" mapping changed. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): header "Applies to" note (recording encode + one live encode shared by RTMP and WebRTC; OQ-115, OQ-059); platform-reference row for two concurrent encodes (CM4: one M2M encoder [D-06], [D-08], concurrency UNKNOWN, reasoning 2.0× the 1080p30 specification [D-10], [D-52]; CM5: both software, reasoning roughly double the encode CPU [G-22]); test-order note; TEST-DMA-001 — OQ-005, OQ-115 linked, objective, setup, test steps and expected results for one capture buffer feeding two encoders (CM4 imports, CM5 conversion once or per encoder, buffers in flight and `CmaFree`, OQ-061); TEST-ENC-001 — OQ-115 linked, objective note, "number of simultaneous encodes" marked superseded (two), live-encode settings (Constrained Baseline [F-36], [F-39], [D-11]; Level 4 / 4.2 [F-40], [D-12]; no B-frames [F-45], [D-14], [D-35]; in-band SPS/PPS [F-36], [D-15], [D-37]), recording-encode settings, two-encode run on CM4 and CM5 (1080p30 pair, then the capture rate; reasoning 4.0× for two 1080p60 encodes from [D-52]), per-encode measurements, two-encode expected results; TEST-REC-001 — OQ-005 linked, recording-encode setup, step 5 (with the live encode running); TEST-STR-001 — OQ-005 linked, shared live-encode setup, joint run with TEST-STR-002, expectations ([F-31]; destination acceptance of Constrained Baseline UNKNOWN, OQ-007); TEST-STR-002 — OQ-005 linked, live-encode setup (SPS profile and level recorded next to the SDP), audio note (still AAC + Opus), CM5 two-encode note on the deferred H.265 bullet; TEST-PERF-001 — RISK-002, OQ-005, OQ-115 linked, objective and setup define the combined load, step 9 (combined load), CMA and CPU expectations; verification table adds D-11, D-35. No test run; no status, test ID, title or "Verifies" mapping changed; no step deleted. | Claude (session 2026-10-08) |
| 2026-10-08 | Two H.264 encodes (owner: "Separate record + live", OQ-005): verifier pass — TEST-STR-002 H.265 note: "CM5 already runs two concurrent software H.264 encodes" → the current scope requires it; sustaining them is OQ-059. TEST-STR-002 live-encode setup: no-B-frames worded as a MediaMTX report [F-45] (community source). No status changed; no ID added. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage + latency (ADR-009 ACCEPTED, OQ-116 ANSWERED, research topics J and K): header — "Last updated", "Applies to" (fragmented MP4 mirrored to NVMe SSD and self-powered USB-SATA HDD; ext4 a proposal, OQ-120; < 1 s for WebRTC viewers only, RTMP best-effort; OQ-005, OQ-006 still open) and "Verification" (topics J and K); Conventions — storage and latency command attestation note (OQ-101; bare `dtparam=pciex1` form NEEDS VERIFICATION); Common setup — Power row (CM4 IO Board PCIe slot needs +12 V [J-05]; CM5 IO Board supply and 600 mA limit [J-20]); Platform reference — rows for NVMe attachment and the HDD's USB path per board [J-01] to [J-03], [J-05] to [J-08], [J-11], [J-13] to [J-15], [J-17] to [J-21], [J-24]; Test order — dated note on the changed procedures and the added dependencies (order unchanged); TEST-ENC-001 — RISK-019, RISK-031, OQ-116, OQ-125, OQ-127 linked, latency scope note superseded in part, CM5 B-frame bullet superseded in part [K-29], live-encode latency and keyframe settings [K-04], [K-17], [K-27] to [K-31], IDR spacing and on-demand keyframe check, per-frame encode-time measurement [K-32], [K-33], [K-36], expected results [K-27], [K-28], [K-30], [K-32], [K-33], [K-39], [K-45]; TEST-REC-001 — RISK-026 to RISK-030, OQ-117 to OQ-124, OQ-101 linked, objective per ADR-009, OQ-006 setup line superseded in part, storage per board (CM4: NVMe in the IO Board PCIe socket on 12 V, HDD on the USB 2.0 hub, USB controller; CM5: M.2 enablement, HDD on USB 3.0; both: HDD power, kernel, ext4, fragmented-MP4 settings, HDD-branch buffer, data rate, time reference), original steps kept and superseded by an eight-step full procedure (enumeration, mirrored recording, fragmented-MP4 check, power cut with seconds lost, HDD stall injection with live path running, bridge soak, player and editor compatibility, HDD absent at boot), expected results per step [J-02], [J-05], [J-09], [J-11], [J-17], [J-18], [J-24], [J-25], [J-27], [J-28], [J-30], [J-33], [J-35] to [J-39], [J-41], [J-45], [K-36], numeric values UNKNOWN; TEST-STR-001 — RISK-019, RISK-028, OQ-116, OQ-127 linked, RTMP latency best-effort (YouTube modes and encoder advice [K-15] to [K-18]), latency recorded with no pass or fail, HDD-stall run, latency pass criterion superseded in part; TEST-STR-002 — RISK-028, RISK-031 to RISK-034, OQ-116, OQ-117, OQ-125 to OQ-128, OQ-101 linked, objective, latency-target setup line superseded in part, publishing route [K-01], [K-02], [K-05], [K-06], [F-45], latency settings [K-07] to [K-10], [K-37], MediaMTX network [K-41] to [K-43], viewers, measurement method (browser statistics [K-11], [K-12]; filmed clock as a research open question), new steps 6 to 9 (LAN latency, join time, internet viewers, live-path isolation), latency expected results [K-03], [K-04], [K-09], [K-14], [K-29], [K-34], [K-35], [K-38], [K-39], [K-41], [K-42], [K-44], [K-45]; TEST-PERF-001 — RISK-026 to RISK-028, OQ-117, OQ-121, OQ-122 linked, step 10 (mirrored recording and latency sampling in the soak), storage expectation [J-27], [J-37]; Verification status — topic J and K rows, CORRECTED, community and reasoning entries, topic J/K research items. No test run; no status, test ID, title or "Verifies" mapping changed; no step deleted. Verifier pass (same date): every [J-xx]/[K-xx] citation checked against the register (174 in the procedures); fixed — `pciex1` worded as set to on, defaulting to off [J-15], with the `config.txt` line form NEEDS VERIFICATION (the bare `dtparam=pciex1` is quoted by no source); [J-30] named as one Seagate BarraCuda 2.5-inch family, not a general HDD figure (three places, including the 9.5 MB buffer reasoning, now with its arithmetic); "fragmented MP4 stays decodable" attributed to FFmpeg's documentation [J-45] (not stated for GStreamer); `first-moov-then-finalise` output no longer asserted (the register names the mode only [J-39]); remux note limited to aborted `hybrid_fragmented` files [J-45]; `filesink` "only after" removed [J-41]; [J-37] rate qualified as the highest research example; drive write caches labelled a research open question; [J-20] "USB peripherals" → peripheral limit; `usb-storage.quirks` worded as a kernel parameter [J-26] used in `cmdline.txt` [J-27]; [J-27] "bridges" → devices; [K-06] dated 2026-10-08 with the image check as a BUILD TEST; [K-15] Ultra-low marked an upper bound per CORRECTED [K-18]; [K-38] "no public source checked"; [K-41] and [K-44] statements attributed to MediaMTX; latency-method item relabelled (research gap: flashing-timecode source; filming arrangement is Claude's proposal); the x264 default-latency point labelled reasoning with its arithmetic from [K-28], [K-29] (40 frames, about 1.33 s at 30p); TEST-STR-002 setup, step 6 and expected result and the test-order note now say how the < 1 s target is judged is an owner decision (OQ-008) and internet viewers run only if OQ-008 scopes them; the owner's "self-powered enclosure" no longer extended to "or hub" (ADR-009 decision 3's wording noted separately). | Claude (session 2026-10-09) |
