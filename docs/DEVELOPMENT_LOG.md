# Development Log

| | |
|---|---|
| Document status | Active — one entry per significant development session (Rule 3) |
| Last updated | 2026-10-06 |

Entries are appended, newest last, and are never rewritten (Rule 21).

---

## 2026-10-06 — Phase 0 bootstrap: repository, rules, source research and documentation baseline

### Objective

Adopt the owner's engineering rules for PACSCORDER and create the repository and documentation structure they require. Every technical statement is to rest on verified sources, because no hardware or code exists yet.

### Starting State

- The directory `RASTER OS` existed and was empty: 0 files, not a git repository.
- No hardware, no code, no documentation.
- Owner answers given at the start of the session (2026-10-06):

| Question | Owner's answer |
|---|---|
| Target platform | "Undecided — keep all four" (Pi 4 Model B, CM4, Pi 5, CM5) |
| Build system | "which ever is best that raspberry pi os". Interpreted as: recommend the best option, preferring Raspberry Pi OS. Recorded as ADR-003 PROPOSED. |
| Hardware existing today | "No hardware yet" |
| "RASTER OS" vs PACSCORDER | "Folder name only" |

### Changes

1. Initialised git (`main`). Added `.gitignore`.
2. Stored the owner's rules verbatim as `docs/ENGINEERING_RULES.md`. The only formatting change: outer code fences widened to four backticks where code blocks nest (Rules 3 and 9), so the Markdown renders. This is noted at the top of the file.
3. Added `CLAUDE.md`, which imports the rules into every Claude Code session.
4. **Source research.** Seven topics (A–G) were researched with primary sources:
   - A: TC358743 hardware
   - B: Linux driver and Device Tree
   - C: Pi CSI-2 platforms
   - D: encoders
   - E: Buildroot
   - F: ATEM and streaming
   - G: Raspberry Pi OS

   Each topic had one research agent, then one independent fact-checker that tried to refute every claim. The result is 377 facts: 348 CONFIRMED, 29 CORRECTED, 0 UNVERIFIABLE, 0 REFUTED. They were generated into `docs/REFERENCES.md`, with the raw data in `docs/research/2026-10-06-source-research.json`.
5. **Registers.** Wrote:
   - `REQUIREMENTS.md`: 17 requirements; 11 DRAFT from the rules, 6 PROPOSED from research.
   - `DECISIONS.md`: ADR-001 to ADR-007 at first; ADR-008 was added later.
   - `RISKS.md`: 21 risks.
   - `README.md`: index, Rule 25 map, conventions and canonical test IDs.
   - `ARCHIVED_APPROACHES.md`: template only.
6. **Documentation set.** A documentation workflow:
   - compiled `OPEN_QUESTIONS.md` (OQ-001 to OQ-089);
   - had 8 writers produce the Rule 2 technical documents plus `TRACEABILITY.md`;
   - had 8 independent reviewers check every citation against the register and fix errors in place.
7. **Cross-document consistency.**
   - Added OQ-090 to OQ-101 and ADR-008 (CSI-2 link frequency, PROPOSED). Added two resolution markers to the README conventions. Added a "Register notes" section to `REFERENCES.md`; register entries were not edited.
   - Five file-owned fix agents applied the reviewers' cross-document findings, and a final verifier swept all documents.
8. Wrote `PROJECT_STATUS.md`, `CHANGELOG.md` and this log.

### Files Modified

All files are new:

- `.gitignore`, `CLAUDE.md`, `README.md` (repository root)
- `docs/ENGINEERING_RULES.md`, `docs/README.md`, `docs/PROJECT_STATUS.md`, `docs/DEVELOPMENT_LOG.md`, `docs/CHANGELOG.md`
- `docs/REQUIREMENTS.md`, `docs/TRACEABILITY.md`, `docs/DECISIONS.md`, `docs/ARCHIVED_APPROACHES.md`, `docs/RISKS.md`, `docs/OPEN_QUESTIONS.md`
- `docs/REFERENCES.md`, `docs/research/2026-10-06-source-research.json`
- `docs/ARCHITECTURE.md`, `docs/SOFTWARE_ARCHITECTURE.md`, `docs/HARDWARE.md`, `docs/DEVICE_TREE.md`
- `docs/TC358743_DRIVER.md`, `docs/V4L2.md`, `docs/CSI_PIPELINE.md`, `docs/DMA.md`
- `docs/VIDEO_ENCODER.md`, `docs/PERFORMANCE.md`, `docs/RECORDING.md`, `docs/STREAMING.md`, `docs/ATEM.md`
- `docs/BUILD_SYSTEM.md`, `docs/RELEASE.md`, `docs/TESTING.md`, `docs/TROUBLESHOOTING.md`

### Hardware Changes

None. No hardware exists.

### Software Changes

None. No kernel, driver, Device Tree, build or application change was made. Documentation only.

### Commands Used

```bash
git init -b main
# Source research and documentation were produced with Claude Code multi-agent workflows:
#   wf_ccb0962e-cda  platform research A–F (12 agents)
#   wf_5db89b44-ec7  Raspberry Pi OS research G (2 agents)
#   wf_bea36705-bf5  OQ register, 8 writers, 8 reviewers (17 of 18 agents completed;
#                    the cross-document critic was interrupted, see Problems Found)
#   wf_d12d79e1-068  5 consistency fix agents and 1 final verifier
# Register generation and documentation check: Python scripts kept in the session
# scratchpad, not in the repository.
python3 doccheck.py docs/
```

### Test Results

No hardware or software test was possible; every hardware test is `BLOCKED — HARDWARE REQUIRED`.

The documentation consistency check (`doccheck.py`, final run on 2026-10-06 after all files were written) checks that:

- every cited fact ID exists in `REFERENCES.md`;
- every REQ/ADR/RISK/OQ/TEST ID used is defined in its canonical file;
- every relative `.md` link resolves;
- `TESTING.md` contains exactly the canonical test IDs;
- the required "Verification status" and "Change history" sections are present;
- no success claims are made;
- all Rule 2 files exist.

**Result:** see the "Final documentation check" subsection below, recorded from the actual run.

### Problems Found

**Technical findings.** These come from sources; none has been observed on hardware.

- 1080p60 (REQ-CAP-001) needs a 4-lane CSI-2 link [C-37], [C-48], so Pi 4 Model B cannot meet it (RISK-001). On 4 lanes at the default rate, 1080p60 UYVY uses 3 of 4 lanes, which is unproven (OQ-038).
- Pi 4/CM4 hardware H.264 encode is specified for 1080p30 only [D-10] (RISK-002). Pi 5/CM5 have no hardware encoder [G-22] (RISK-003).
- HDMI sources see no display until userspace loads an EDID [A-33], [B-21] (RISK-010, REQ-CAP-003).
- A wrong reference-clock rate in the Device Tree leads to a kernel `BUG_ON` instead of a clean probe failure [A-22], [B-11] (RISK-007).
- Pi 5/CM5 TC358743 capture has no official Raspberry Pi documentation; the procedure rests on community reports [C-33], [C-38] (RISK-012).

**Process problems.**

1. Several writers stated facts beyond what their citations supported. The 8 reviewers found and fixed 234 such issues, for example "EDID RAM is volatile" cited to a fact that does not say so.
2. My early drafts of `REQUIREMENTS.md`, `DECISIONS.md` and `RISKS.md` used non-ID citations such as "[G gaps]".
3. The README marker list omitted two markers that the OQ register used.
4. The cross-document critic agent was interrupted when the host computer went to sleep. It had already partly edited `REQUIREMENTS.md`, `DECISIONS.md`, `RISKS.md` and `SOFTWARE_ARCHITECTURE.md`.

### Root Cause

- Process 1: writers paraphrased register entries more broadly than their wording.
- Process 2 and 3: those files were written before `OPEN_QUESTIONS.md` existed.
- Process 4: host sleep during a long-running agent.

### Solution

- Process 1: per-cluster adversarial review, then a cross-document fix pass with 14 agreed wording treatments (139 fixes), then a final verifier (11 fixes).
- Process 2 and 3: references replaced with OQ IDs; markers added to `README.md`.
- Process 4: the interrupted agent's edits were inspected (each file had a Change-history row describing them). The remaining work was rerun as a separate workflow told not to redo or duplicate those edits.

### Current Status

- PARTIAL. The Phase 0 documentation baseline is complete and passes the documentation check.
- No requirement or PROPOSED/OPEN ADR has been reviewed by the owner.
- All product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. The owner reviews the owner-decision questions (OQ-001 to OQ-017, OQ-090 to OQ-092, OQ-094, OQ-096, OQ-099) and the PROPOSED ADRs (ADR-002, -003, -005, -006, -008).
2. The owner chooses bring-up hardware (OQ-018 to OQ-021); see [PROJECT_STATUS.md](PROJECT_STATUS.md).
3. Commit the baseline. Proposed commit, for the owner to approve (Rule 15):

```text
Commit title: docs: bootstrap PACSCORDER engineering rules, source register and documentation baseline
Commit description: Add the owner's engineering rules (verbatim) and CLAUDE.md. Add the
  verified source register (377 facts, research topics A–G) with raw research data.
  Add requirements (17), ADRs (8), risks (21), open questions (101) and the full Rule 2
  documentation set written from the register and reviewed. No hardware or code exists;
  nothing is tested.
Files changed: .gitignore, CLAUDE.md, README.md, docs/** (29 Markdown files +
  docs/research/2026-10-06-source-research.json)
Reason: Phase 0 — establish the mandatory documentation and evidence baseline (Rules 1–25)
Tests: documentation consistency check (doccheck.py) — see DEVELOPMENT_LOG.md 2026-10-06
```

### Final documentation check

Run on 2026-10-06 after every file in this entry was written (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=377 REQ=17 ADR=8 RISK=21 OQ=101 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=377/377
PROBLEMS (0):
WARNINGS (2):
   DEVELOPMENT_LOG.md: no "Change history" section
   REFERENCES.md: possible success claim: - **Fact:** Audio output is either I2S or TDM; ...
```

- **Result: TESTED — PASS** for documentation consistency only. This is not a test of any product function.
- **Warning 1:** fixed after the run by adding the Change history table below.
- **Warning 2:** a false positive. The checker's pattern matched "TDM is fixed at 8 channels" in a datasheet fact. `REFERENCES.md` is generated and is not edited (Rule 21).
- **Not checked mechanically:** the technical correctness of each sentence against its cited fact. That was done by the 8 per-cluster reviewers and the final verifier agent, not by this script.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created with the Phase 0 bootstrap entry. | Claude (session 2026-10-06) |
