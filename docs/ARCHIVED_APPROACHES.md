# PACSCORDER Archived Approaches

| | |
|---|---|
| Document status | Active — 1 entry (ARCH-001, CM5 dropped as product platform) |
| Last updated | 2026-10-09 |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 14 (never delete information without recording it), Rule 21 (failures are engineering information) |

This file records every approach that was **attempted and then abandoned**. That covers code, configuration, hardware support and methods. Its purpose is to stop failed experiments being repeated.

Options that were only *considered* and never attempted are not recorded here. Those live in the "Alternatives Considered" section of the relevant ADR in [DECISIONS.md](DECISIONS.md). *(Exception, 2026-10-09: ARCH-001 records the CM5 evaluation platform, which the owner directed be archived here when it was dropped — see that entry's note. It was a documented, planned bring-up path rather than a hardware-attempted one; the decision of record is ADR-004.)*

## Entries

## ARCH-001 — CM5 (BCM2712) as a candidate product platform

- **Date attempted:** — (never attempted on hardware; no hardware ever existed)
- **Date abandoned:** 2026-10-09
- **Related:** ADR-004 (ACCEPTED, CM4), OQ-011 (ANSWERED), REQ-PLT-001; RISK-003, RISK-014; OQ-050 to OQ-054, OQ-059, OQ-100, OQ-108, OQ-124

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
