# PACSCORDER Changelog

| | |
|---|---|
| Document status | Active |
| Last updated | 2026-10-13 |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 4 |

Every meaningful change is recorded here. Versioning is semantic-style (`Unreleased`, `v0.1.0`, `v0.2.0`, `v1.0.0`; see [RELEASE.md](RELEASE.md)). History is never rewritten: a correction is a new entry.

## Unreleased

### Added

**Repository and process**
- Git repository initialised on branch `main` (2026-10-06).
- `.gitignore` for macOS and editor files and for future build outputs.
- `CLAUDE.md`: loads the engineering rules into every Claude Code session.

**Process documents**
- `docs/ENGINEERING_RULES.md`: the owner's engineering and documentation rules, verbatim. The only formatting change is wider outer code fences where code blocks nest.
- `docs/README.md`: documentation index, the Rule 25 question map, documentation conventions and the canonical test IDs.
- `docs/REQUIREMENTS.md`: 17 requirements (11 DRAFT from the rules, 6 PROPOSED from research).
- `docs/DECISIONS.md`: ADR-001 to ADR-008. ADR-001 is ACCEPTED (from the rules); ADR-002, -003, -005, -006 and -008 are PROPOSED; ADR-004 and -007 are OPEN.
- `docs/RISKS.md`: 21 technical risks.
- `docs/OPEN_QUESTIONS.md`: 101 open questions and owner decisions. OQ-001 to OQ-089 were compiled from the research; OQ-090 to OQ-101 were added after the per-document reviews.
- `docs/TRACEABILITY.md`: requirement → design → implementation → test → result.
- `docs/ARCHIVED_APPROACHES.md`: template only; no entries.
- `docs/PROJECT_STATUS.md`, `docs/DEVELOPMENT_LOG.md`, `docs/CHANGELOG.md`.

**Source research**
- `docs/REFERENCES.md`: source register of 377 facts (348 CONFIRMED, 29 CORRECTED) from research topics A–G.
- `docs/research/2026-10-06-source-research.json`: the raw research and verification data behind the register.

**Technical documents**
- Written from the source register; none describes implemented or tested functionality: `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `HARDWARE.md`, `DEVICE_TREE.md`, `TC358743_DRIVER.md`, `V4L2.md`, `CSI_PIPELINE.md`, `DMA.md`, `VIDEO_ENCODER.md`, `PERFORMANCE.md`, `RECORDING.md`, `STREAMING.md`, `ATEM.md`, `BUILD_SYSTEM.md`, `RELEASE.md`, `TESTING.md`, `TROUBLESHOOTING.md`.

**2026-10-12 — tools, research topic L, encoder test stage**
- `tools/doccheck.py`: the documentation consistency check, rebuilt from the check list in DEVELOPMENT_LOG.md (2026-10-06) and now version-controlled. The original lived only in a session scratchpad and was lost. Adds three checks: register header counts, uncited facts, and possibly stale ADR/OQ statuses. README convention 10 makes it part of committing documentation.
- `tools/encoder_capacity_test.sh`: TEST-ENC-001 Stage 0, an encoder-only capacity run on a CM4 with a synthetic source (no TC358743). **NOT YET RUN ON PACSCORDER HARDWARE**; dry-run only, with stand-in commands on a development computer.
- `docs/REFERENCES.md` topic L: 63 facts (56 CONFIRMED, 7 CORRECTED) on H.264 level bit-rate ceilings (ITU-T H.264 V16), CM4 and CM4 IO Board Ethernet, `bcm2835-codec` concurrency, and encoder-test commands. Register total 620 facts. Raw data: `docs/research/2026-10-12-bitrate-network-encoder-research.json`.
- `docs/TESTING.md` TEST-ENC-001: Stage 0 procedure (run matrix R1–R7, up to two 1080p60 encodes).

**2026-10-13 — slice C5: the drive copies compared by content; failure evidence kept**
- `tests/mp4_same.py`: compares two MP4 files byte for byte except the header creation and modification times, which each drive's `mp4mux` takes from the wall clock. `tests/mp4_same_check.sh` (CTest `mp4_same_dev`) tests it. TESTED — PASS on the development machine only.
- `tests/smoke_drives.sh`: keeps logs and recordings of a failed run in `$PACS_TEST_LOGDIR`. The writers log the running time at which each file starts and any frame a drive refuses.
- **OQ-133** (owner question): must the two drives' copies be byte-identical? ARCH-002 to ARCH-004 recorded.

**2026-10-12 — slice C5: drive failure, return and HDD decoupling**
- `app/src/writer.c/.h`: one writer pipeline per recording drive; ALERT and continue on the other drive when a drive fails, fills or is missing; a returning drive rejoins at the next file on the same keyframe as the other drive; a drive more than `--writer-buffer-seconds` (default 10 s, OQ-117) behind is dropped instead of blocking; files named `pacs-<session>-<index>.mp4` (a new recording no longer overwrites an earlier one). IMPLEMENTED — NOT TESTED on PACSCORDER hardware.
- `tests/smoke_drives.sh` (CTest `smoke_drives_dev`): a drive missing at start that returns; a RAM disk that fills. TESTED — PASS on the development machine only.
- **ADR-016 (PROPOSED)**; **ADR-015 ACCEPTED** (owner). *(Superseded 2026-10-13 — ADR-016 ACCEPTED.)*

**2026-10-12 — slice C2: live publishing**
- `app/src/publisher.c/.h`: RTSP publishing to MediaMTX and RTMP publishing (`flvmux` + `rtmp2sink`), each in its own pipeline fed from the live encode, restarted 5 s after an error, keyframe requested on (re)start. Options `--rtsp-url`, `--rtmp-url`. IMPLEMENTED — NOT TESTED on PACSCORDER hardware.
- `tests/smoke_publish.sh` (CTest `smoke_publish_dev`): publishes to a private MediaMTX and reads both streams back; checks the recording survives a dead publisher target. TESTED — PASS on the development machine only.
- **ADR-015 (PROPOSED):** publishers in separate pipelines so a network failure cannot stop the recording.

**2026-10-12 — first application code (slice C1)**
- `CMakeLists.txt`, `app/CMakeLists.txt`, `tests/CMakeLists.txt`: CMake build and CTest (ADR-014).
- `app/src/` (`main.c`, `config.c/.h`, `pipeline.c/.h`, `h264_level.c/.h`): the PACSCORDER recorder in C on GStreamer — capture, recording encode with fragmented MP4 every 30 minutes mirrored to two directories, live encode at half the capture rate above 30 fps; `cm4` backend (`v4l2src`, `v4l2h264enc` with the owner's settings) and `dev` backend (test source, `x264enc`). IMPLEMENTED — NOT TESTED on PACSCORDER hardware.
- `tests/test_h264_level.c`, `tests/smoke_record.sh`: development tests, TESTED — PASS on the development machine only.
- `scripts/bringup/tc358743.sh`: CM4 bring-up helper (EDID, DV timings, format, capture check). NOT YET RUN ON PACSCORDER HARDWARE.
- **ADR-014 (ACCEPTED):** CMake (owner's choice over the suggested Meson).

### Changed

- 2026-10-13: **Owner decisions** — **ADR-016 ACCEPTED** (recording drives in their own pipelines) and **OQ-133 ANSWERED**: the two drives' copies must hold the same video; byte-identical copies are not required.
- 2026-10-13: `smoke_record_dev` and `smoke_drives_dev` compare the two drives' copies with `tests/mp4_same.py` instead of `cmp`, and print a NOTE when only the header time stamps differ (ARCH-004). Reason: a rejoining drive's first file can carry a creation time one second earlier than the other drive's copy — reproduced in 11 of 20 runs with heavy disk writing, 0 of 20 idle (SOFTWARE_ARCHITECTURE §10.2). `writer.h` comment corrected (no `queue` in the writer pipeline).
- 2026-10-12: Slice C5 documented (SOFTWARE_ARCHITECTURE §10.2, RECORDING, TESTING, OQ-117, OQ-129, RISK-028 mitigation, REQ-REC-001). Slice C2 was committed and pushed as `bc3c398`.
- 2026-10-12: Slice C2 documented (SOFTWARE_ARCHITECTURE §10.1, BUILD_SYSTEM §2A, TESTING "Development tests", STREAMING); REQ-STR-001 and REQ-STR-002 now IMPLEMENTED — NOT TESTED (partial). Slice C1 was committed and pushed as `9ab0b4e`.
- 2026-10-12: **Coding started** — slice C1 documented in SOFTWARE_ARCHITECTURE §10, BUILD_SYSTEM §2A and TESTING "Development tests"; REQ-ARCH-001, REQ-ENC-001, REQ-REC-001 now IMPLEMENTED — NOT TESTED (partial). The GStreamer/C decisions were committed and pushed as `27bb305`.
- 2026-10-12, latest: **Owner decisions — GStreamer (ADR-007 ACCEPTED) and C (ADR-013 ACCEPTED)**; Claude's Python proposal (ADR-012) SUPERSEDED BY ADR-013. OQ-015 and OQ-132 ANSWERED. 65 stale ADR-007 status lines annotated.
- 2026-10-12, latest: **ADR-007 proposal — GStreamer** (OPEN → PROPOSED; reasons: FFmpeg's CM4 encoder wrapper cannot set bitrate mode or level [L-43]; documented fan-out, frame-rate halving, fragmented 30-minute recording, RTSP publishing to MediaMTX) with a pipeline sketch. **ADR-012 (PROPOSED):** application language Python 3 with PyGObject, proposed repository layout; new OQ-132. The ADR decisions were committed and pushed as `78b6d70`.
- 2026-10-12, later still: **Owner decisions** — ADR-010 ACCEPTED (legacy video-node control on the CM4; ADR-006 SUPERSEDED BY ADR-010; OQ-014 ANSWERED), ADR-008 ACCEPTED (486 MHz; OQ-099 ANSWERED), ADR-011 ACCEPTED (bring-up boards: 2 × C790, 1 × X1301, B102 if obtainable). 77 stale status lines annotated across 19 documents; test procedures that used the sub-device node for EDID/DV timings got a note to use the video node. `tools/doccheck.py` W3 now also covers SUPERSEDED ADRs. Topic M round committed and pushed as `7d7d2de`.
- 2026-10-12, later: **Research topic M** (60 facts: 47 CONFIRMED, 13 CORRECTED; register 680 facts) on TC358743 bridge boards for the CM4 IO Board, its camera connectors and cables, and `videorate` frame-rate conversion; propagated to OQ-018 to OQ-021, OQ-024, OQ-025, OQ-131, HARDWARE, DEVICE_TREE, SOFTWARE_ARCHITECTURE, TESTING and TROUBLESHOOTING. **ADR-011 (PROPOSED):** bring-up boards — 2 × Geekworm C790, 1 × X1301, Auvidea B102 if obtainable; HW REV A chosen only after the bench tests. **ADR-010 (PROPOSED):** legacy video-node control on the CM4, the alternative to ADR-006; ADR-008 recommended for acceptance as written. **RISK-035** added (HDMI audio on non-Auvidea boards). The non-1080 decision was committed and pushed as `b8091fd`.
- 2026-10-12, later still: **Non-1080 sources keep their resolution** on the live encode (owner; OQ-131). Reasoning: a 720p live encode at 17 Mbit/s must signal at least Level 3.2, because Level 3.1 caps Constrained Baseline at 14 Mbit/s. OQ-131 stays OPEN only for the 60 Hz method and its cost. The 25 fps decision was committed and pushed as `91ff869`. *(Confirmed by the owner later on 2026-10-12 ("okay"): 720p sources follow the 30/25 fps rule — 720p60 goes out as 720p30, 720p50 as 720p25.)*
- 2026-10-12, later still: **50 Hz-family sources — live encode at 25 fps** (owner; OQ-131 partly answered): 1080p50 halved, 1080p25 passed through; with a 1080p50 recording the CM4 encoder carries 2.5× its 1080p30 specification (reasoning). TEST-ENC-001 Stage 0 gains run R9 (1080p50 + 1080p25); the product runs are R4, R9 and R3. The previous decisions were committed and pushed as `c9d7e19`.
- 2026-10-12, later: **Live encode decided — 1080p30** (owner; OQ-005), shared by RTMP and WebRTC. With the recording at the capture rate, the CM4 encoder must carry 3.0× (4-lane) or 2.67× (2-lane) its 1080p30 specification (reasoning). TEST-ENC-001 Stage 0 gains run R8 (1080p50 recording + 1080p30 live), and R4 and R8 are the product runs. New **OQ-131**: how the live path reaches 1080p30 from other input modes.
- 2026-10-12, later: **CM5-only questions ANSWERED** as made irrelevant by ADR-004 (OQ-049 to OQ-055, OQ-059, OQ-060, OQ-124), following the register's rule for dropped platforms (owner's instruction). OQ-108 is an H.265 question, mislabelled as CM5 in the 2026-10-09 ADR-004 text, and stays OPEN. Register: 131 OQs (108 OPEN, 23 ANSWERED).
- 2026-10-12, later: Git — the hand commit `3c823b6` "mmit" was reworded to its Rule 15 message (`27c44d9`); the 2026-10-12 work was committed (`1f5c0e0`) and both were pushed (`049d1ee..1f5c0e0`). `tools/doccheck.py` W3 now looks only within the same clause, which removes false positives where OPEN/PROPOSED belongs to a neighbouring ID.
- 2026-10-12: **ADR-004 scope sweep.** CM5 / Pi 5 / BCM2712 / Pi 4 Model B material in 22 documents is marked "not in current scope" and kept as reference (Rule 14). Every statement that still showed ADR-004 (or ADR-003, ADR-009, or an answered OQ) as open got a dated superseding note. Insertions only; the only removed text is "Last updated" dates and the DECISIONS.md header counts.
- 2026-10-12: **OQ-129 drive-return decision propagated** into the design and test content (RECORDING, ARCHITECTURE, SOFTWARE_ARCHITECTURE, STREAMING, PERFORMANCE, TESTING TEST-REC-001). The 2026-10-09 answer had reached the registers but not the design text or the tests.
- 2026-10-12: **Research topic L propagated.** OQ-073: the bit-rate part is resolved from ITU-T H.264 — 25 Mbit/s High Level 4.2 is 40 % of the ceiling; 17 Mbit/s CBR fits every level used. OQ-098: CM4 Ethernet is an on-board Gigabit PHY, so 102 Mbit/s of egress is 10.2 % of the link. OQ-115: kernel-source part done — one firmware component per session, no driver limit, scheduling in firmware. VIDEO_ENCODER §6: Level 4.1 does not cover 1080p50; Level 4.2 is needed. RISK-002 evidence extended. All three OQs stay OPEN for their hardware parts.
- 2026-10-12: OQ-011 / ADR-004 correction on which items are CM5-specific (RISK-014's CM4 path in scope; OQ-100 not CM5-specific). ADR-006 and ADR-008 get rationale-review notes, because their CM5-based reasons no longer apply. No decision changed.
- 2026-10-09: **Product platform decided — CM4 (ADR-004 ACCEPTED); CM5 dropped.**
  - Owner: "stick to CM4". The product platform is the **Compute Module 4 (CM4, BCM2711)**; OQ-011 ANSWERED. **CM5, BCM2712, Pi 5 and Pi 4 Model B are dropped as product boards** and kept only as reference (Rule 14); CM5 is archived as **ARCH-001** in [ARCHIVED_APPROACHES.md](ARCHIVED_APPROACHES.md).
  - CM4 carries the TC358743 on CAM1 (4-lane, 1080p60) and CAM0 (2-lane), with the hardware H.264 encoder; both lane configurations (REQ-CAP-007) are served by CM4.
  - Consequence: the product rides entirely on the CM4 hardware encoder, so 1080p60 hardware encode (RISK-002) and two concurrent encodes on it (OQ-115) are now single-platform-critical with no fallback.
  - CM5-only risks (RISK-003; the CM5 parts of RISK-014) and questions (OQ-050 to OQ-054, OQ-059, OQ-100, OQ-108, OQ-124) are kept OPEN but **not in current scope**, as the deferred H.265 material is. Registers: ADRs now 4 ACCEPTED (ADR-001, ADR-003, ADR-004, ADR-009); OQs 130 (117 OPEN, 13 ANSWERED).
  - Authoritative + reader-facing docs updated (DECISIONS ADR-004, OPEN_QUESTIONS OQ-011, REQUIREMENTS REQ-PLT-001, RISKS, ARCHIVED_APPROACHES, HARDWARE, TESTING, PROJECT_STATUS, TRACEABILITY, and the VIDEO_ENCODER/ARCHITECTURE/SOFTWARE_ARCHITECTURE/PERFORMANCE headers). Deep inline CM5 content in the other documents is kept as reference under the ADR-004 scope rule (not deleted, Rule 14) and can be swept later. Uncommitted, pending owner approval.
- 2026-10-09: **Owner decision: which drive and statistic the recording < 1 s target is judged against (OQ-130 ANSWERED).**
  - The recording capture-to-file **< 1 s glass-to-disk** target is judged against the **primary recording target — the NVMe SSD — at the 95th percentile** (consistent with the WebRTC criterion, OQ-008); the HDD is a **lagging mirror** not bound by the target.
  - This refines ADR-009's mirror into a latency-bound primary plus a lagging mirror (both drives still receive the full recording); the lagging mirror is the decoupled HDD branch (OQ-117). The HDD cannot meet < 1 s during a standby-to-ready stall of up to about 3.0 s [J-30] (RISK-028).
  - Registers: 130 OQs (118 OPEN, 12 ANSWERED). Propagated to the registers, ADR-009 Consequences, REQUIREMENTS, RECORDING, VIDEO_ENCODER, PERFORMANCE, TESTING (TEST-REC-001) and TRACEABILITY. Uncommitted, pending owner approval.
- 2026-10-09: **Owner decisions: recording encode, recording latency, WebRTC measurement run (OQ-005 and OQ-008 ANSWERED).**
  - Recording encode (OQ-005): H.264 **High profile, Level 4.2, no B-frames**, uniform on CM4 and CM5. CM4 emits no B-frames [D-14]; CM5 `x264enc` is forced B-frame-free [K-29]; Level 4.2 because 1080p60 exceeds Level 4.0's macroblock rate [F-40]. Whether 25 Mbit/s fits Level 4.2 is OQ-073.
  - Recording latency (OQ-005): a **capture-to-file latency target of under 1 s glass-to-disk**. New **OQ-130**: against which drive, and over what statistic, it is judged, given the HDD standby-to-ready stall of up to about 3.0 s [J-30] (RISK-028, OQ-117).
  - WebRTC measurement run (OQ-008): the 95th-percentile camera-to-viewer latency is measured over a **30-minute run at 1 sample per second (about 1800 samples)** with the recording running.
  - With these, **OQ-005 and OQ-008 are fully ANSWERED.** Registers: 130 OQs (119 OPEN, 11 ANSWERED).
  - Propagated to the technical documents (REQUIREMENTS, OPEN_QUESTIONS, VIDEO_ENCODER, RECORDING, STREAMING, PERFORMANCE, ARCHITECTURE, SOFTWARE_ARCHITECTURE, TESTING, TRACEABILITY, PROJECT_STATUS). TEST-ENC-001 sets the recording profile/level/B-frames; TEST-REC-001 measures the capture-to-file latency; TEST-STR-002 records the ~1800-sample 95th percentile. Uncommitted, pending owner approval.
- 2026-10-09: **Owner decisions: drive return, viewers and browsers.**
  - OQ-129 ANSWERED: a drive that drops out and returns is used again from the next 30-minute file. Full-disk muxer behaviour moves to OQ-118.
  - OQ-008: up to 5 simultaneous LAN viewers; Chrome, Safari and Firefox. OQ-008 stays open only for the sample count and run length of the measurement.
  - Reasoning: about 102 Mbit/s of video egress with one RTMP destination, above a 100 Mbit/s Fast Ethernet link. The Ethernet speed is not researched (OQ-098).
- 2026-10-09: **Owner decisions: drive missing at start, 30-minute file splitting (OQ-129).**
  - A recording started with one mirrored drive missing runs on the available drive with an operator alert.
  - Recordings are split into a new file every 30 minutes, about 5.67 GB per file per drive at 25 Mbit/s (reasoning, [J-36]). `splitmuxsink` is a candidate mechanism [J-44]; fragmented-mode compatibility is NEEDS VERIFICATION (OQ-118).
  - OQ-129 stays open for drive return and full-disk muxer behaviour.
  - Propagated to 10 technical documents; TEST-REC-001 steps 8 and 9 c have defined expectations.
- 2026-10-09: Pushed `b142871` (bitrates and drive-failure policy) to `origin/main`.
- 2026-10-09: **Owner decisions: bitrate and drive failure.**
  - Bitrates (OQ-005): live encode (RTMP and WebRTC) CBR 17 Mbit/s; recording encode 25 Mbit/s VBR. OQ-005 stays open for the recording encode's profile, level and B-frames and a capture-to-file latency target. Profile/level bitrate fit is NEEDS VERIFICATION (OQ-073).
  - Drive failure (OQ-129): if one mirrored drive fills, is missing or fails during a recording, recording continues on the other drive and the operator is alerted. OQ-129 stays open for file splitting, alert method, drive return and a drive absent at the start.
  - Propagated to 13 technical documents. TEST-ENC-001 sets and measures both bitrates; TEST-REC-001 step 9 expected behaviour is defined for the policy part.
- 2026-10-09: Pushed `6ad83d4` (latency criterion, LAN-only viewers, recording duration) to `origin/main`.
- 2026-10-09: **Owner decisions: latency criterion, viewer reach, recording duration.**
  - The < 1 s WebRTC target is judged at the 95th percentile: 95 % of camera-to-viewer samples under 1 s, over a sustained run with the recording running (OQ-008). Sample count and run length are still open.
  - WebRTC viewers are LAN only. Internet viewers (OQ-128, RISK-033) are not in current scope and are kept as reference.
  - Recordings run until stopped or the disk is full: **OQ-006 ANSWERED**.
  - New OQ-129: mirror behaviour when one drive fills, is absent or fails; file splitting.
  - Affected statements in the technical documents are marked superseded. TEST-STR-002 now states the criterion; TEST-REC-001 gains step 9.
- 2026-10-09: Pushed `f39e661` (documentation catch-up) to `origin/main` at the owner's request.
- 2026-10-09: **Documentation catch-up for `54269bf`, and verification of topics J and K.**
  - ADR-009 (fragmented MP4 mirrored to NVMe SSD and HDD), the < 1 s WebRTC target (RTMP best-effort; OQ-116) and the topic J/K facts are now in every affected technical document:
    - `RECORDING`, `PERFORMANCE`, `DMA`, `STREAMING`, `VIDEO_ENCODER`, `HARDWARE`, `DEVICE_TREE`, `BUILD_SYSTEM`, `RELEASE`, `ARCHITECTURE`, `SOFTWARE_ARCHITECTURE`, `V4L2`, `CSI_PIPELINE`, `TC358743_DRIVER`, `TESTING`, `TRACEABILITY` and `TROUBLESHOOTING`;
    - this includes a full TEST-REC-001 procedure, latency steps in TEST-STR-001 and TEST-STR-002, and 12 new troubleshooting signatures. No test ID was added or renamed.
  - Verification pass over about 2,200 J/K citations, including the `54269bf` register text, which had never been verified.
    - Corrections were added as dated notes: CM4 encoder sharing stated as fact (OQ-115); one HDD family's figures generalised [J-30]; "under 5 s at best" for YouTube [K-15], [K-18]; and others.
    - No requirement, ADR, risk or OQ status, score or ID changed.
  - `REFERENCES.md`: dated register notes on [K-06], [K-26], [J-23], tier labels and MediaMTX tiers. No entry was edited.
  - New open items inside existing OQs:
    - how the < 1 s target is judged (OQ-008, owner decision);
    - RP1 CFE behaviour when no buffer is queued (OQ-126);
    - the CM5 `x264enc` on-demand keyframe (OQ-127).
  - `README.md`: fact-ID range extended to `K-NN`; project phase corrected to PHASE 1. `PROJECT_STATUS.md` brought up to date.
  - All 557 register facts are now cited at least once.
- 2026-10-08 *(recorded 2026-10-09)*: **Recording storage and live latency.**
  - Owner decisions:
    - live latency under 1 s camera-to-viewer for WebRTC viewers only, with RTMP best-effort (OQ-116 added and ANSWERED);
    - recording in MP4, written fragmented and mirrored to a PCIe NVMe SSD and a USB-to-SATA HDD in a self-powered enclosure (**ADR-009 ACCEPTED**; ext4 is Claude's proposal within it).
  - Source register extended with topic J (recording storage and power loss, 45 facts) and topic K (live latency, 45 facts): 78 CONFIRMED and 12 CORRECTED, 557 facts in total. Raw data: `docs/research/2026-10-08-storage-latency-research.json`.
  - Register changes:
    - OQ-117 to OQ-128 and RISK-026 to RISK-034 added;
    - evidence notes added to REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002, ADR-004 and ADR-007, with no status changed.
  - Only the registers were updated. The work was committed on 2026-10-09 as `54269bf` "update 9oct", which is on `origin/main`. That message does not follow Rule 15 and was left unchanged on the shared remote. This entry and the log entry were missing until 2026-10-09.
- 2026-10-08 *(recorded 2026-10-09)*: `6efadce` "docs: two H.264 encodes - recording and shared live (OQ-005)" committed with the approved message and on `origin/main`.
- 2026-10-08: **Two H.264 encodes.** The owner answered OQ-005 with "Separate record + live": one recording encode, and one live encode shared by RTMP and WebRTC. REQ-ENC-001 now records this together with the WebRTC constraints on the live encode. OQ-115 was added (two concurrent encodes on the CM4 hardware encoder), and RISK-002 and RISK-003 were annotated. Technical documents updated.
- 2026-10-08: Pushed `d2d217e` (H.265 deferral) to `origin/main`.

- 2026-10-08: **H.265 deferred.** The owner answered OQ-103 with "H.264 only for now". REQ-ENC-001 is now H.264 only, and REQ-ENC-002 (H.265, new acceptance value DEFERRED) was added. OQ-104 to OQ-109, RISK-022 and RISK-025 are marked not in current scope. TEST-ENC-001 was retitled "Sustained real-time H.264 encode (H.265 deferred)". The technical documents were updated, with the H.265 evidence kept and labelled deferred.
- 2026-10-08: Pushed `main` to `origin` (github.com/NVPrasathR/RASTER-OS) at the owner's request: `7107a39`, `df3591d`. The local backup branch was not pushed.

- 2026-10-08: **Source register extended** with topic H (H.265/HEVC software encoding and transport, 43 facts) and topic I (HDMI audio path, 47 facts): 90 facts, 86 CONFIRMED and 4 CORRECTED. The register now holds 467 facts; existing entries are unchanged. Raw data: `docs/research/2026-10-08-hevc-audio-research.json`.
- 2026-10-08: Owner decisions of 2026-10-07 (second set) and the topic H/I facts propagated to the registers and technical documents.
  - OQ-104 to OQ-114 added: H.265 throughput, SIMD, RTMP, WebRTC and licensing; audio formats, sample rate, A/V sync, AAC licensing and GPIO allocation.
  - RISK-023 (audio sample-rate mismatch), RISK-024 (A/V sync across clock domains) and RISK-025 (HEVC over RTMP may force a split GStreamer/FFmpeg design) added.
  - RISK-014, RISK-015, RISK-019 and RISK-022 extended.
  - TEST-AUD-001 written out in full; TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (ID unchanged).
- 2026-10-07: Git history: the owner's local commits "update doc" and "update" (18:23, never pushed) were squashed, at the owner's request, into `7107a39` "docs: bootstrap PACSCORDER rules, source register and documentation baseline", to meet Rule 15. File contents are identical (same tree). The old history is kept on local branch `backup/pre-squash-2026-10-07`.

- 2026-10-07: Owner decisions recorded.
  - OQ-001 and OQ-009 are ANSWERED.
  - REQ-CAP-001 and REQ-ATEM-001 are updated: 1080p60 is required on 4-lane; ATEM scope is HDMI capture only.
  - New DRAFT requirements: REQ-CAP-007 (2-lane and 4-lane, all frame rates each link carries), REQ-CAP-008 (ATEM and camera HDMI sources), REQ-BLD-002 (own OS image).
  - Owner input added to ADR-003 and ADR-004; no decision status changed.
  - OQ-102 added (which ATEM models and cameras).
  - The affected technical documents were updated to match.
- 2026-10-07: **ADR-003 ACCEPTED** by the owner. The OS/image basis is decided: Raspberry Pi OS Lite for bring-up, own product image built with `rpi-image-gen`, Buildroot as the documented alternative. OQ-012 is ANSWERED. All documents were updated to the accepted status.
- 2026-10-07: **Correction.** The 2026-10-06 entry under Added says "17 requirements (11 DRAFT from the rules, 6 PROPOSED from research)". The correct split was 12 DRAFT and 5 PROPOSED. The original line is left as written (Rule 21). As of 2026-10-07 there are 20 requirements: 15 DRAFT, 5 PROPOSED.
- 2026-10-07: Second set of owner answers recorded.
  - Bring-up evaluates CM4 and CM5 side by side (ADR-004 input; still OPEN).
  - Audio is required: REQ-CAP-006 moved from PROPOSED to DRAFT; OQ-004 is ANSWERED.
  - Codecs are H.264 and H.265 (REQ-ENC-001). OQ-103 added; RISK-022 added because H.265 is software-only on every candidate.
  - Sources are any HDMI camera with no model list; OQ-102 is ANSWERED.
- 2026-10-07: TEST-ATEM-001 retitled "ATEM HDMI output capture (scope per OQ-009)"; the ID is unchanged.

### Fixed

- 2026-10-13: Intermittent failure of `smoke_drives_dev` (2 of 17 runs on 2026-10-12, "differs between the drives"): the test compared header time stamps that the design does not keep equal; it now compares the frames and file structure. Most likely cause reproduced; the 2026-10-12 runs themselves were not kept. DEVELOPMENT_LOG.md had "Last updated 2026-10-09" despite later entries; corrected.
- Nothing (first entry). *(Superseded 2026-10-12 — see below.)*
- 2026-10-12: DECISIONS.md header said "3 ACCEPTED, 4 PROPOSED, 2 OPEN" after ADR-004 was accepted; corrected to 4 / 4 / 1 (found by the rebuilt documentation check).
- 2026-10-12: **Missing change-history rows** for the 2026-10-09 edits committed by hand (`98779a4`, `9670f71`, `2d76092` "update by ai"; `049d1ee` "up date by ai"; `27c44d9` (committed by hand as "mmit", reworded before push on 2026-10-12)) added as dated catch-up rows in 11 documents. Where `3c823b6` (now `27c44d9`) rewrote text in place, the rows record it — the TESTING.md Platform row and the REQUIREMENTS.md REQ-PLT-001 Notes text verbatim; the "Applies to" headers and the TRACEABILITY `ADR-004 (O)` → `(A)` cells by reference to git (Rule 21).
- 2026-10-12: Stale statements of answered OQs (OQ-005, OQ-008) in REQUIREMENTS.md, and the [C-27] citation for a CM4 hazard (it is a CM5 fact), annotated.

### Known Issues

- *(Added 2026-10-13.)* The two recording drives' copies of a file are the same frames but not guaranteed byte-identical (header time stamps); whether they must be is OQ-133. *(OQ-133 ANSWERED 2026-10-13: not required — the same video is enough.)*
- Procedure steps whose command syntax is not in the source register are marked NEEDS VERIFICATION (OQ-101).

- No hardware exists. Every hardware test is BLOCKED — HARDWARE REQUIRED.
- No code exists. Every requirement is NOT STARTED.
- No requirement is ACCEPTED (21 requirements: 16 DRAFT, 4 PROPOSED, 1 DEFERRED). Four PROPOSED ADRs (ADR-002, -005, -006, -008) and one OPEN ADR (ADR-007) await owner decision; ADR-001, ADR-003, ADR-004 (CM4) and ADR-009 are ACCEPTED.
- H.265 is deferred (REQ-ENC-002). If re-activated, it is software-only on every candidate, and real-time 1080p H.265 is doubtful (RISK-022, OQ-104).
- The < 1 s WebRTC latency target is unproven on both modules; most of the latency budget is undocumented (RISK-031, OQ-125). It is judged at the 95th percentile over a 30-minute run at 1 sample/second (~1800 samples), with the recording running (owner, 2026-10-09; OQ-008 ANSWERED).
- The recording capture-to-file < 1 s glass-to-disk target (owner, 2026-10-09) is judged against the primary recording target — the NVMe SSD — at the 95th percentile; the HDD is a lagging mirror not bound by the target (OQ-130 ANSWERED). The HDD cannot meet < 1 s during a standby-to-ready stall of up to about 3.0 s [J-30] (RISK-028), so its branch must be decoupled or buffered (OQ-117).
- Network egress with 5 WebRTC viewers and one RTMP destination is about 102 Mbit/s of video (reasoning); the boards' Ethernet link speed has not been researched (OQ-098). *(Superseded 2026-10-12: the CM4 has an on-board Gigabit PHY [L-15]; 102 Mbit/s of payload is 10.2 % of the link, overhead not included [L-22]. Measured throughput is HARDWARE TEST REQUIRED.)*
- *(Added 2026-10-12.)* No 4-lane TC358743 board has a published CM4 test report; the documented-good Auvidea B102 is not obtainable, and HDMI audio on the obtainable boards is unproven (RISK-035, ADR-011).
- Recording storage (ADR-009): on CM4 the NVMe SSD takes the only PCIe lane and the HDD shares USB 2.0 (RISK-026); UAS bridge faults (RISK-027); HDD stalls could back-pressure the shared encoder and live path (RISK-028); fragmented MP4 compatibility (RISK-029); residual loss on a power cut (RISK-030).
- The GStreamer WebRTC publishing elements are not packaged for Raspberry Pi OS trixie, and WHEP is still a draft (RISK-034).
- Commit `54269bf` has a non-Rule-15 message ("update 9oct"); it is on the shared remote and was not rewritten.
- Commits `fe26dd2` ("correcting by ai"), `98779a4`, `9670f71`, `2d76092` ("update by ai") and `049d1ee` ("up date by ai") — on the shared remote — have non-Rule-15 messages. The hand commit `3c823b6` ("mmit") was reworded to a Rule 15 message before push on 2026-10-12 (owner's choice) and is on `origin` as `27c44d9`. They were not rewritten; their content is recorded by the 2026-10-09 log entries and the 2026-10-12 catch-up rows.
- The two-encode design rests on the CM4 hardware encoder, whose only official capacity is 1080p30 [L-29]; a Raspberry Pi engineer's opinion is that two 1080p30 streams are probably too many [L-30] (community). TEST-ENC-001 Stage 0 measures it on a CM4 alone (OQ-115, RISK-002).
- HDMI audio on CM5 is unconfirmed (RISK-014). *(2026-10-12: the CM5 part is not in current scope — ADR-004 ACCEPTED, CM4.)* Audio sample-rate changes are not tracked by the kernel (RISK-023).
- 1080p60 capture is not possible on a 2-lane link (RISK-001). Since 2026-10-07, 1080p60 is required on 4-lane configurations only, and Pi 4 Model B remains a 2-lane candidate (REQ-CAP-007). 1080p60 encode is unproven on every candidate platform (RISK-002, RISK-003).

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created with the Phase 0 bootstrap entry. | Claude (session 2026-10-06) |
| 2026-10-07 | Unreleased/Changed: owner decisions of 2026-10-07. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 acceptance, requirement-count correction, TEST-ATEM-001 retitle; RISK-001 known-issue line updated for REQ-CAP-007. | Claude (session 2026-10-07) |
| 2026-10-07 | Second set of owner answers; Known Issues updated (ADR counts, RISK-022). | Claude (session 2026-10-07) |
| 2026-10-08 | Topics H and I; propagation of the second owner decisions; commit squash recorded; Known Issues extended. | Claude (session 2026-10-08) |
| 2026-10-08 | H.265 deferral and push recorded. | Claude (session 2026-10-08) |
| 2026-10-08 | Two-encode decision; push of `d2d217e`. | Claude (session 2026-10-08) |
| 2026-10-09 | Storage and latency (topics J and K, ADR-009, OQ-116; commit `54269bf`) and the `6efadce` commit recorded; Known Issues updated (ADR-009 ACCEPTED, RISK-026 to RISK-031, RISK-034, commit message). 2026-10-09 documentation catch-up and verification entry added. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions of 2026-10-09 (latency criterion, LAN-only viewers, recording duration; OQ-006 ANSWERED; OQ-129) and the push of `f39e661` recorded; Known Issues updated. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions on bitrate (OQ-005) and drive failure (OQ-129), and the push of `6ad83d4`, recorded. | Claude (session 2026-10-09) |
| 2026-10-09 | OQ-129 decisions (drive missing at start; 30-minute file splitting) and the push of `b142871` recorded. Drive-return, viewer and browser decisions added; Known Issues: mirror item replaced by the network-egress item. | Claude (session 2026-10-09) |
| 2026-10-09 | Owner decisions on the recording encode (H.264 High profile, Level 4.2, no B-frames), the recording capture-to-file latency (< 1 s glass-to-disk) and the WebRTC measurement run (30-minute, ~1800 samples) recorded; OQ-005 and OQ-008 ANSWERED; new OQ-130 added; Known Issues updated. Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-09 | OQ-130 ANSWERED: recording capture-to-file < 1 s judged against the primary recording target (NVMe SSD) at the 95th percentile, the HDD a lagging mirror; ADR-009 mirror refined. Counts 130 OQs (118 OPEN, 12 ANSWERED). Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-09 | Product platform decided: CM4 (ADR-004 ACCEPTED); CM5, BCM2712, Pi 5 and Pi 4 Model B dropped as product boards (CM5 archived, ARCH-001); OQ-011 ANSWERED. Counts: 4 ACCEPTED ADRs; 130 OQs (117 OPEN, 13 ANSWERED). Authoritative and reader-facing docs updated; deep inline CM5 content kept as reference. Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-12 | Added: `tools/doccheck.py`, `tools/encoder_capacity_test.sh`, research topic L, TEST-ENC-001 Stage 0. Changed: ADR-004 scope sweep, OQ-129 propagation, topic L propagation, OQ-011 correction, ADR-006/ADR-008 review notes. Fixed: DECISIONS header counts, catch-up change-history rows for the hand commits, stale OQ statements. Known Issues: network egress, hand-commit messages, CM4 two-encode capacity, CM5 audio scope. | Claude (session 2026-10-12) |
| 2026-10-12 | Later on 2026-10-12: live encode 1080p30 and OQ-131; CM5-only OQs ANSWERED; reword of `3c823b6` to `27c44d9`, commit `1f5c0e0` and push; doccheck W3 refinement. | Claude (session 2026-10-12) |
| 2026-10-12 | Later still on 2026-10-12: OQ-131 50 Hz decision (25 fps), run R9; commit `c9d7e19` and push recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | Later still on 2026-10-12: OQ-131 non-1080 decision; commit `91ff869` and push recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | Later on 2026-10-12: topic M, ADR-010, ADR-011, RISK-035; commit `b8091fd` recorded; Known Issues: bridge-board supply and audio. | Claude (session 2026-10-12) |
| 2026-10-12 | Later still: ADR-006/008/010/011 decisions; doccheck W3 extension; `7d7d2de` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | Latest: ADR-007 and ADR-012 proposals; OQ-132; `78b6d70` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | Latest: ADR-007 and ADR-013 accepted; ADR-012 superseded. | Claude (session 2026-10-12) |
| 2026-10-12 | Added: application slice C1, development tests, bring-up script, ADR-014. Changed: requirement implementation statuses; `27bb305` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | Added: slice C2, `smoke_publish_dev`, ADR-015. Changed: REQ-STR-001/002 statuses; `9ab0b4e` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | Added: slice C5, `smoke_drives_dev`, ADR-016; ADR-015 accepted; `bc3c398` recorded. | Claude (session 2026-10-12) |
| 2026-10-13 | Added: comparison of the drive copies (`mp4_same.py`, `mp4_same_dev`), failure evidence, OQ-133, ARCH-002 to ARCH-004; Changed/Fixed/Known Issues for the intermittent drive-test failure. | Claude (session 2026-10-13) |
| 2026-10-13 | Owner decisions: ADR-016 ACCEPTED, OQ-133 ANSWERED (Changed; notes in Added and Known Issues). | Claude (session 2026-10-13) |
