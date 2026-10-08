# Development Log

| | |
|---|---|
| Document status | Active — one entry per significant development session (Rule 3) |
| Last updated | 2026-10-08 |

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

---

## 2026-10-07 — Owner decisions, ADR-003 accepted, commit squash

### Objective

Record the owner's decisions on the most decisive open questions, propagate them through the documentation, and act on the owner's commit instruction.

### Starting State

Phase 0 documentation baseline (2026-10-06 entry); nothing committed; no hardware.

### Changes

1. **First set of owner answers** (owner's words quoted):

   | Question | Owner's answer | Recorded as |
   |---|---|---|
   | OQ-001: Is 1080p60 mandatory? | "i need 2 lane and 4 lane with all frame rate" | OQ-001 ANSWERED; new REQ-CAP-007 (DRAFT). Interpretation: 1080p60 on 4-lane; 2-lane bounded by the link [C-37], [C-48]. |
   | OQ-009: ATEM scope | "it can be atem and direct video from camera" | OQ-009 ANSWERED; new REQ-CAP-008 (DRAFT). HDMI capture only; network integration out of scope. |
   | OS / build | "which is best i need by own one" | New REQ-BLD-002 (own OS image); ADR-003 recommendation unchanged. |
   | Commit | "Not yet" | No commit. |

2. **ADR-003 accepted.** The owner wrote "accept ADR-003". Status became ACCEPTED, OQ-012 ANSWERED, and the change was propagated to 18 documents (83 wording changes).
3. **Second set of owner answers:**

   | Question | Owner's answer | Recorded as |
   |---|---|---|
   | Platform | "CM4 + CM5 side by side" | ADR-004 owner input; still OPEN until measured |
   | Audio | "Yes, audio required" | REQ-CAP-006 PROPOSED → DRAFT; OQ-004 ANSWERED |
   | Codec | "H.264 + H.265 (HEVC)" | REQ-ENC-001; new OQ-103 and RISK-022 |
   | Sources | "Any HDMI camera (generic)" | OQ-102 ANSWERED |

4. **Propagation.** Two workflows propagated the first set of decisions (103 changes, plus 20 verifier fixes) and the ADR-003 acceptance (83 changes) into the technical documents.
5. **Git.** At 18:23 the owner made two local commits, "update doc" and "update". Both messages break Rule 15. I checked the GitHub remote: it had no branches, so nothing had been pushed. At the owner's request ("Squash into one commit") I squashed them into `7107a39`. The tree hash was identical before and after (`7dcff252…`). The old history is kept on the local branch `backup/pre-squash-2026-10-07`.

### Files Modified

`docs/REQUIREMENTS.md`, `docs/DECISIONS.md`, `docs/OPEN_QUESTIONS.md`, `docs/RISKS.md`, `docs/README.md`, `docs/PROJECT_STATUS.md`, `docs/CHANGELOG.md`, and, through the workflows, every technical document in `docs/`.

### Hardware Changes

None.

### Software Changes

None. Documentation and git history only.

### Commands Used

```bash
git ls-remote --heads origin     # empty: nothing pushed
git branch backup/pre-squash-2026-10-07 12d9dae
git reset --soft 5012470 && git commit --amend   # squash; tree 7dcff252… unchanged
python3 doccheck.py docs/        # documentation check
```

### Test Results

- Documentation consistency check after each propagation: 0 problems.
- No hardware or software test was possible.

### Problems Found

1. **Miscount in my own summaries.** On 2026-10-06 I reported 17 requirements as "11 DRAFT, 6 PROPOSED". The correct split was 12 DRAFT and 5 PROPOSED. Found by a verifier agent on 2026-10-07. Corrected in `PROJECT_STATUS.md` and noted in `CHANGELOG.md`; the 2026-10-06 entry above is left as written (Rule 21).
2. **Overstated claim to the owner.** On 2026-10-06 I told the owner the stock image "already ships" the TC358743 overlays. The verified register does not establish that, so the documents say NEEDS VERIFICATION.
3. **Overstated reasoning in my ADR-003 note.** I wrote that REQ-CAP-007 "spans the Pi 4 and Pi 5 families". It may not: both configurations could sit on one family, for example CM4 CAM0 plus CAM1. Corrected by a verifier.
4. **Rule 15 violation in the owner's commit messages.** Resolved by the squash described above.

### Root Cause

- Problems 1 to 3: I summarised without re-counting, or stated more than the source supported.
- Problem 4: the commits were made manually, outside the documented process.

### Solution

- Problems 1 to 3: corrections recorded openly. Counts are now taken from the files by script.
- Problem 4: squash with a Rule 15 message, history backed up.

### Current Status

- PARTIAL. Owner decisions recorded and propagated; documentation check passes.
- Product work remains BLOCKED — HARDWARE REQUIRED.

### Next Step

Research what the 2026-10-07 decisions imply, namely H.265 and the HDMI audio path, which the 2026-10-06 research did not cover. See the 2026-10-08 entry.

---

## 2026-10-08 — H.265 and HDMI audio research (topics H and I) and propagation

### Objective

Establish, from verified sources, how H.265 encoding and transport, and the HDMI audio path, can work on CM4 and CM5. Then fold those facts and the 2026-10-07 decisions into the documentation.

### Starting State

Commit `7107a39`, plus uncommitted 2026-10-07 edits. Source register: 377 facts (topics A–G).

### Changes

1. **Research workflow, two topics** (researcher plus independent adversarial verifier each):
   - Topic H, H.265/HEVC: 43 facts, 40 CONFIRMED and 3 CORRECTED.
   - Topic I, HDMI audio: 47 facts, 46 CONFIRMED and 1 CORRECTED.
   - They were appended to `REFERENCES.md` (now 467 facts) without changing existing entries. Raw data is in `docs/research/2026-10-08-hevc-audio-research.json`.
2. **Propagation workflow** (registers, then 5 file-owned document agents, then a verifier):
   - OQ-104 to OQ-114 and RISK-023 to RISK-025 added; RISK-014, RISK-015, RISK-019 and RISK-022 extended.
   - Evidence added to REQ-CAP-006, REQ-ENC-001, REQ-REC-001, REQ-STR-001, REQ-STR-002, ADR-004 and ADR-007, with no status changed.
   - About 114 changes across the technical documents, including a full TEST-AUD-001 procedure.
   - The verifier checked 1460 citations and fixed 14 overstatements.
3. TEST-ENC-001 retitled "Sustained real-time H.264 / H.265 encode" (ID unchanged).
4. My documentation-check script (session scratchpad) now covers topics H and I, and fact IDs inside multi-ID brackets such as `[H-10, H-13]`.
5. `PROJECT_STATUS.md` and `CHANGELOG.md` brought up to date.

### Files Modified

`docs/REFERENCES.md` (appended), `docs/research/2026-10-08-hevc-audio-research.json` (new), and these documents:

`OPEN_QUESTIONS.md`, `RISKS.md`, `REQUIREMENTS.md`, `DECISIONS.md`, `README.md`, `VIDEO_ENCODER.md`, `PERFORMANCE.md`, `DMA.md`, `STREAMING.md`, `RECORDING.md`, `ATEM.md`, `HARDWARE.md`, `DEVICE_TREE.md`, `TC358743_DRIVER.md`, `V4L2.md`, `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `BUILD_SYSTEM.md`, `RELEASE.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `PROJECT_STATUS.md`, `CHANGELOG.md` and `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation only.

### Commands Used

```bash
caffeinate -im -t 10800      # keep the Mac awake during long research runs
python3 doccheck.py docs/
```

### Test Results

See "Final documentation check (2026-10-08)" below. No hardware or software test was possible.

### Problems Found

1. **Two failed research attempts.** The first (2026-10-07) lost the internet connection: "Can't reach the API server (ENOTFOUND)". The second (2026-10-08) ended when the host went to sleep. Neither produced results, and nothing was written to the repository.
2. **H.265 feasibility.** No candidate has a hardware HEVC encoder [D-24], [D-31]. A Raspberry Pi engineer reported software HEVC encode as too intensive (community) [H-19]. The published benchmarks are community results, not 1080p60 measurements [H-20], [H-21], [H-22]. Real-time 1080p H.265 is therefore doubtful (RISK-022), and owner decision OQ-103 is now the most urgent.
3. **HEVC over RTMP.** It is possible with FFmpeg 7.1.5 but not with the distribution's GStreamer 1.26.2 [H-26], [H-27] (RISK-025).
4. **Audio.** The kernel does not track HDMI audio sample-rate changes (reasoning from source) [I-18] (RISK-023). CM5 operation is unconfirmed [I-05], [I-06], [I-07] (RISK-014).

### Root Cause

- Problem 1: host network loss and sleep during multi-hour agent runs.
- Problems 2 to 4: properties of the platforms and software stacks, from sources.

### Solution

- Problem 1: rerun with `caffeinate` keeping the host awake; the third attempt completed.
- Problems 2 to 4: recorded as risks and open questions, with resolving tests (TEST-ENC-001, TEST-AUD-001, TEST-STR-001, TEST-STR-002). No design decision was taken without the owner.

### Current Status

- PARTIAL. Documentation is up to date with all owner decisions and 467 verified facts; the check passes.
- All product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. The owner decides OQ-103: which outputs need H.265, and whether software-only H.265 is acceptable.
2. The owner obtains the CM4 + CM5 bring-up hardware (`PROJECT_STATUS.md`).
3. Commit the uncommitted 2026-10-07/08 work, once the owner approves. Proposed commit (Rule 15):

```text
Commit title: docs: record owner decisions, accept ADR-003, add H.265 and audio research
Commit description: Record the owner decisions of 2026-10-07 (2-lane and 4-lane, all frame
  rates; ATEM and camera HDMI sources; own OS image; CM4 + CM5 evaluation; audio required;
  H.264 + H.265; any HDMI camera) and the acceptance of ADR-003. Add source-register topics
  H (H.265, 43 facts) and I (HDMI audio, 47 facts) and propagate them: OQ-102..OQ-114,
  RISK-022..RISK-025, REQ-CAP-007, REQ-CAP-008, REQ-BLD-002, TEST-AUD-001 procedure.
  No hardware or code exists; nothing is tested.
Files changed: docs/** (29 Markdown files) + docs/research/2026-10-08-hevc-audio-research.json
Reason: Rules 1, 11, 13, 17, 22, 23 — owner decisions and their evidence documented
Tests: documentation consistency check — see DEVELOPMENT_LOG.md 2026-10-08
```

### Final documentation check (2026-10-08)

Run on 2026-10-08 after every file in this entry was written (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=467 REQ=20 ADR=8 RISK=25 OQ=114 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=467/467
PROBLEMS (0):
WARNINGS (2):   (both the known false positive from a datasheet fact in REFERENCES.md; see the 2026-10-06 entry)
```

- **Result: TESTED — PASS** for documentation consistency only. This is not a test of any product function.
- The check now verifies IDs inside multi-ID brackets as well. All 467 register facts are cited at least once.

---

## 2026-10-08 (second entry) — H.265 deferred (OQ-103), commit and push

### Objective

Record the owner's answer to OQ-103 and the owner's commit and push instruction, then propagate the decision.

### Starting State

Uncommitted 2026-10-07/08 work on top of `7107a39`; documentation check passing.

### Changes

1. **Owner answers** (2026-10-08):
   - OQ-103: "H.264 only for now".
   - Commit: "Commit and push to GitHub".
2. **Commit and push.**
   - The 2026-10-07/08 work was committed as `df3591d` "docs: record owner decisions, accept ADR-003, add H.265 and audio research", using the message the owner approved.
   - Before pushing, I amended the message to correct a file count I had got wrong: it said 25 Markdown files and the commit holds 26. The commit was unpushed at the time.
   - Pushed with `git push -u origin main`. The push output showed `7107a39..df3591d`, so GitHub already had `7107a39`: it was pushed between the squash and this session's push, not by me. The earlier `git ls-remote` that found no branches ran before the squash.
   - The local branch `backup/pre-squash-2026-10-07` was not pushed.
3. **Decision recorded** in the canonical files:
   - REQ-ENC-001 is now H.264 only for all outputs.
   - New REQ-ENC-002, "H.265 (HEVC) encoding (deferred)", with a new acceptance value `DEFERRED` defined in REQUIREMENTS.md.
   - OQ-103 ANSWERED, and scope notes added to OQ-104 to OQ-109.
   - RISK-022 and RISK-025 marked not in current scope; they stay OPEN, and severities are unchanged.
   - Scope note added to ADR-007.
   - TEST-ENC-001 retitled "Sustained real-time H.264 encode (H.265 deferred)".
4. **Propagation workflow:** 4 file-owned agents and a verifier made 105 changes and 6 fixes across the technical documents. The H.265 research is kept and labelled deferred, so it stays evidence for REQ-ENC-002.

### Files Modified

`docs/REQUIREMENTS.md`, `docs/OPEN_QUESTIONS.md`, `docs/RISKS.md`, `docs/README.md`, `docs/DECISIONS.md`, `docs/PROJECT_STATUS.md`, `docs/CHANGELOG.md`, `docs/DEVELOPMENT_LOG.md`, and 15 technical documents (`VIDEO_ENCODER`, `PERFORMANCE`, `DMA`, `STREAMING`, `RECORDING`, `ATEM`, `HARDWARE`, `DEVICE_TREE`, `ARCHITECTURE`, `SOFTWARE_ARCHITECTURE`, `BUILD_SYSTEM`, `RELEASE`, `TESTING`, `TRACEABILITY`, `TROUBLESHOOTING`).

### Hardware Changes

None.

### Software Changes

None. Documentation and git only.

### Commands Used

```bash
git commit                      # df3591d (message as approved, file count corrected before push)
git push -u origin main         # 7107a39..df3591d
git ls-remote --heads origin    # refs/heads/main = df3591d
python3 doccheck.py docs/
```

### Test Results

Documentation consistency check after all edits (exit code 0):

```text
defined: facts=467 REQ=21 ADR=8 RISK=25 OQ=114 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=467/467
PROBLEMS (0):
```

- **Result: TESTED — PASS** for documentation consistency only. The remaining warnings are the known datasheet-phrase false positive.
- No hardware or software test was possible.

### Problems Found

The file count in the approved commit message was wrong (25 instead of 26). It was corrected before the push.

### Root Cause

I wrote the count from an earlier status listing instead of counting the commit's contents.

### Solution

Counted with `git show --name-only` and amended the unpushed message. Counts are now taken from the commit itself.

### Current Status

- PARTIAL. Scope is H.264 only, documented and consistent.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. The owner decides OQ-005 (bitrate, latency, number of simultaneous H.264 encodes) and the other owner-decision OQs.
2. The owner obtains the CM4 + CM5 bring-up hardware.
3. Commit and push this change once the owner approves. Proposed commit (Rule 15):

```text
Commit title: docs: defer H.265 encoding, H.264 only for now (OQ-103)
Commit description: Record the owner's answer to OQ-103 ("H.264 only for now"): REQ-ENC-001 is
  H.264 only for all outputs; add REQ-ENC-002 (H.265, DEFERRED) and the DEFERRED acceptance
  value; mark OQ-104..OQ-109, RISK-022 and RISK-025 not in current scope; retitle TEST-ENC-001;
  label the H.265 research deferred across the technical documents (evidence kept).
  No hardware or code exists; nothing is tested.
Files changed: docs/** (23 Markdown files)
Reason: Rules 1, 11, 13, 14, 21 — owner decision documented, deferred work recorded, not deleted
Tests: documentation consistency check — 0 problems (DEVELOPMENT_LOG.md 2026-10-08, second entry)
```

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created with the Phase 0 bootstrap entry. | Claude (session 2026-10-06) |
| 2026-10-07 | Entry added: owner decisions, ADR-003 accepted, commit squash (written 2026-10-08). | Claude (session 2026-10-07/08) |
| 2026-10-08 | Entry added: topics H and I research and propagation. | Claude (session 2026-10-08) |
| 2026-10-08 | Second 2026-10-08 entry added: H.265 deferred (OQ-103), commit `df3591d` and push. | Claude (session 2026-10-08) |
