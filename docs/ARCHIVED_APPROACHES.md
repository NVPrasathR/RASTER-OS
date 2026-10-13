# PACSCORDER Archived Approaches

| | |
|---|---|
| Document status | Active — 4 entries (ARCH-001, CM5 dropped as product platform; ARCH-002 to ARCH-004, slice C5 drive writers and their test) |
| Last updated | 2026-10-13 |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 14 (never delete information without recording it), Rule 21 (failures are engineering information) |

This file records every approach that was **attempted and then abandoned**. That covers code, configuration, hardware support and methods. Its purpose is to stop failed experiments being repeated.

Options that were only *considered* and never attempted are not recorded here. Those live in the "Alternatives Considered" section of the relevant ADR in [DECISIONS.md](DECISIONS.md). *(Exception, 2026-10-09: ARCH-001 records the CM5 evaluation platform, which the owner directed be archived here when it was dropped — see that entry's note. It was a documented, planned bring-up path rather than a hardware-attempted one; the decision of record is ADR-004.)*

## Entries

## ARCH-001 — CM5 (BCM2712) as a candidate product platform

- **Date attempted:** — (never attempted on hardware; no hardware ever existed)
- **Date abandoned:** 2026-10-09
- **Related:** ADR-004 (ACCEPTED, CM4), OQ-011 (ANSWERED), REQ-PLT-001; RISK-003, RISK-014; OQ-050 to OQ-054, OQ-059, OQ-100, OQ-108, OQ-124 *(2026-10-12: OQ-049 to OQ-055, OQ-059, OQ-060 and OQ-124 are ANSWERED as made irrelevant by ADR-004 (owner's instruction). Their entries and research stay in the registers; if CM5 is reconsidered, set them back to OPEN with a dated note.)*

### What was attempted

Nothing was built or run. CM5 was carried as a **candidate product platform** alongside CM4, under the owner's 2026-10-07 decision to "evaluate CM4 and CM5 side by side." The plan was to decide the platform from TEST-CAP-002, TEST-CAP-004 and TEST-ENC-001 results on both boards. The side-by-side evaluation was documented throughout the technical documents but never executed, because no hardware exists.

### Why it was attempted

CM5 (BCM2712) has two 4-lane CSI-2 ports and a faster CPU (Cortex-A76 with Neon DotProd), so it was a strong candidate for 1080p60 capture and for software H.265 had that been required. It was kept as a hedge against CM4's unproven 1080p60 hardware encode (RISK-002).

### What happened

No test was run (BLOCKED — HARDWARE REQUIRED throughout). Research recorded that CM5 has **no hardware video encoder** (software H.264/H.265 only, RISK-003, [D-31], [G-22]); that its HDMI-audio path through RP1 I2S1 is **unconfirmed** (RISK-014, OQ-054); that it has **no official Raspberry Pi TC358743 documentation** [C-38]; and that its M.2 storage needs enabling (OQ-124, OQ-100). The live-latency and two-encode behaviour of its software encoder were never measured (OQ-059, OQ-125).

### Why it was abandoned

Owner decision, 2026-10-09 ("stick to CM4"): the product is the CM4, which has the hardware H.264 encoder and official TC358743 documentation. Keeping two platforms doubled the bring-up and risked diverging code; the owner chose to commit to one.

### What replaced it

ADR-004 ACCEPTED: the product platform is the **CM4 (BCM2711)**. All CM5/BCM2712/Pi 5 material in the other documents is kept as reference and marked **not in current scope** (as the deferred H.265 material is), not deleted (Rule 14). If CM5 is ever reconsidered, its research facts in [REFERENCES.md](REFERENCES.md) and the CM5-specific OQs and risks above are still present. *(Note: CM5 was a considered/planned platform rather than a hardware-attempted approach; it is recorded here at the owner's direction, with ADR-004's Alternatives Considered holding the per-ADR analysis.)*

## ARCH-002 — File splits requested from the split timer with `split-now` (slice C5)

- **Date attempted:** 2026-10-12
- **Date abandoned:** 2026-10-12
- **Related:** ADR-016 (PROPOSED), OQ-129, REQ-REC-001; development test `smoke_drives_dev` *(Superseded 2026-10-13 — ADR-016 ACCEPTED.)*

### What was attempted

The first slice C5 writers split their files when the application's split timer emitted `split-now` on each drive's `splitmuxsink` and requested a keyframe; a drive that rejoined started at the next keyframe it received.

### Why it was attempted

It is the simplest use of `splitmuxsink`: each drive splits at "the next keyframe" on request.

### What happened

The first run of `smoke_drives_dev` failed ("pacs-…-00002.mp4 differs between the drives"). A manual repeat showed the rejoining drive's first file holding 221 packets against 234 in the other drive's copy — it started 13 frames later; the following files matched. Reasoning: a natural keyframe was already queued for the active drive when the split was requested, so the two drives split at different keyframes ([DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md), 2026-10-12 tenth entry).

### Why it was abandoned

The two drives' files must start on the same frame (OQ-129: the same files on both drives).

### What replaced it

The split is decided on the keyframe itself, in the recording branch's streaming thread, for all drives at once (`split-at-running-time` with that keyframe's running time; a returning drive is activated on the same keyframe) — ADR-016, [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) §10.2.

## ARCH-003 — Waiting for a rejoining drive's `appsrc` to be ready before activating it (slice C5)

- **Date attempted:** 2026-10-13
- **Date abandoned:** 2026-10-13 (never committed)
- **Related:** ADR-016 (PROPOSED), OQ-133; development test `smoke_drives_dev` *(Superseded 2026-10-13 — ADR-016 ACCEPTED.)*

### What was attempted

A `ready` flag in `app/src/writer.c`: after starting a drive's pipeline, the writer checked that its `appsrc` had reached PAUSED and was activated on a split's keyframe only if it had; otherwise it printed "not ready yet" and waited for a later file.

### Why it was attempted

Hypothesis for the intermittent failure of `smoke_drives_dev` on 2026-10-12 (2 of 17 runs, "differs between the drives"): if the rejoining drive's `appsrc` refused its first frame, its file would start later than the other drive's.

### What happened

In 25 runs of `smoke_drives_dev` the guard never triggered ("not ready yet" 0 times) and no drive refused a frame. Code review found that its retry path could never run: the split timer only restarts drives that are absent, not drives that are joining.

### Why it was abandoned

No evidence for the problem it guarded against, and an unreachable retry path.

### What replaced it

Nothing; the writer is back to activating a joining drive on the split's keyframe. Each writer now logs any frame its `appsrc` refuses, so the hypothesis would be visible if it ever occurred.

## ARCH-004 — Comparing the two drives' copies byte for byte with `cmp` in the development tests (slices C1 and C5)

- **Date attempted:** 2026-10-12
- **Date abandoned:** 2026-10-13
- **Related:** OQ-133, ADR-016 (PROPOSED), ADR-009; development tests `smoke_record_dev`, `smoke_drives_dev` *(Superseded 2026-10-13 — ADR-016 ACCEPTED.)*

### What was attempted

`smoke_record_dev` and `smoke_drives_dev` required each file on one drive to be byte-identical (`cmp`) to its copy on the other.

### Why it was attempted

Both drives receive the same encoded frames, so the copies were expected to be identical.

### What happened

`mp4mux` writes the wall-clock second into the creation and modification times of the `mvhd`, `tkhd` and `mdhd` boxes; muxing the same frames twice, a second or more apart, gives files that differ in exactly those fields (measured 2026-10-13). Each drive has its own muxer (ADR-016), so `cmp` can fail although both copies hold the same frames: a rejoining drive's first file differed from the other drive's copy in its creation times in 11 of 20 rejoin runs with heavy disk writing (0 of 20 idle), every frame matching ([SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) §10.2). The 2 failures of `smoke_drives_dev` on 2026-10-12 had the same symptom; most likely the same cause, not proven (files not kept).

### Why it was abandoned

It tests a property the design does not guarantee; whether the product needs that property is an owner question (OQ-133). *(OQ-133 ANSWERED 2026-10-13: byte-identical copies are not required — the same video is enough.)*

### What replaced it

`tests/mp4_same.py`: byte for byte except those time fields, itself tested by `mp4_same_dev`. The tests print a NOTE when only the time fields differ.

## Entry template

Copy this block for each abandoned approach. Never edit or remove an entry after it is written; add a dated note instead.

```markdown
## ARCH-NNN — <short title>

- **Date attempted:** YYYY-MM-DD
- **Date abandoned:** YYYY-MM-DD
- **Related:** REQ-…, ADR-…, RISK-…, TEST-…

### What was attempted

### Why it was attempted

### What happened

Include test IDs, logs and measurements, or link to them.

### Why it was abandoned

### What replaced it
```

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | File created with template; no entries. | Claude (session 2026-10-06) |
| 2026-10-09 | ARCH-001 added: CM5 (BCM2712) dropped as a candidate product platform (owner, "stick to CM4"; ADR-004 ACCEPTED, CM4). Document status and intro updated. | Claude (session 2026-10-09) |
| 2026-10-12 | ARCH-001 Related line: note that the CM5-only questions are ANSWERED as made irrelevant by ADR-004 (owner's instruction, 2026-10-12) and how to reopen them. | Claude (session 2026-10-12) |
| 2026-10-13 | ARCH-002 (timer-issued `split-now`, slice C5, 2026-10-12 — not archived at the time), ARCH-003 (`appsrc` readiness guard) and ARCH-004 (byte-for-byte `cmp` of the drive copies in the development tests) added. | Claude (session 2026-10-13) |
| 2026-10-13 | ARCH-002 to ARCH-004: dated notes — ADR-016 ACCEPTED; OQ-133 ANSWERED. | Claude (session 2026-10-13) |
