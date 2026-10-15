# Development Log

| | |
|---|---|
| Document status | Active — one entry per significant development session (Rule 3) |
| Last updated | 2026-10-13 |

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

1. The owner decides OQ-005 (bitrate, latency, number of simultaneous H.264 encodes) and the other owner-decision OQs. *(Done for the number of encodes on 2026-10-08 — see the third entry.)*
2. The owner obtains the CM4 + CM5 bring-up hardware.
3. Commit and push this change once the owner approves. *(Done: `d2d217e`, pushed 2026-10-08.)* Proposed commit (Rule 15):

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

---

## 2026-10-08 (third entry) — Two H.264 encodes (OQ-005), H.265 deferral pushed

### Objective

Commit and push the H.265 deferral as approved, and record the owner's answer on the number of encodes.

### Starting State

`df3591d` on `origin/main`; the H.265 deferral was uncommitted.

### Changes

1. **Commit and push.** Committed `d2d217e` "docs: defer H.265 encoding, H.264 only for now (OQ-103)" with the approved message (23 files, counted from the staged set), then pushed: `df3591d..d2d217e`.
2. **Owner answer to OQ-005** (2026-10-08): "Separate record + live". There are two simultaneous H.264 encodes: one for recording, and one live encode shared by RTMP and WebRTC. Bitrate, rate control and latency remain open (OQ-005 stays OPEN).
3. **Canonical files.**
   - REQ-ENC-001 records the decision, plus the constraints that follow because the live encode also feeds WebRTC (reasoning from [F-36], [F-39], [F-40], [F-45], [D-11], [D-14], [D-15]).
   - New OQ-115 asks whether CM4's single hardware encoder can run two sessions. The register has no fact on it. Reasoning: 2 × 1080p30 ≈ 2.0× the 1080p30 specification [D-10], [D-52].
   - RISK-002 and RISK-003 annotated; RISK-002 now links OQ-115.
   - Notes added to ADR-004 and ADR-007; statuses unchanged.
4. **Propagation workflow:** 3 file-owned agents and a verifier made 92 changes and 14 fixes. The fixes mainly reworded "CM4 runs both encodes" as "would run", because OQ-115 is unknown.

### Files Modified

`docs/REQUIREMENTS.md`, `docs/OPEN_QUESTIONS.md`, `docs/RISKS.md`, `docs/DECISIONS.md`, `docs/PROJECT_STATUS.md`, `docs/CHANGELOG.md`, `docs/DEVELOPMENT_LOG.md`, `docs/VIDEO_ENCODER.md`, `docs/PERFORMANCE.md`, `docs/DMA.md`, `docs/STREAMING.md`, `docs/RECORDING.md`, `docs/ARCHITECTURE.md`, `docs/SOFTWARE_ARCHITECTURE.md`, `docs/TESTING.md`, `docs/TRACEABILITY.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation and git only.

### Commands Used

```bash
git commit && git push origin main    # d2d217e
python3 doccheck.py docs/
```

### Test Results

Documentation consistency check after all edits (exit code 0):

```text
defined: facts=467 REQ=21 ADR=8 RISK=25 OQ=115 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=467/467
PROBLEMS (0):
```

- **Result: TESTED — PASS** for documentation consistency only.
- No hardware or software test was possible.

### Problems Found

None new. The open technical question is OQ-115: whether one CM4 hardware encoder can run two encodes.

### Root Cause

—

### Solution

—

### Current Status

- PARTIAL. Encoding scope is decided: two H.264 encodes, with H.265 deferred. Bitrate and latency are still open.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. The owner decides OQ-005 (bitrate, rate control, latency) and OQ-006 (recording container, storage, power-loss behaviour). *(Done in part later on 2026-10-08: latency target, recording container, storage and power-loss strategy — see the fourth entry. Bitrate, rate control and recording duration remain open.)*
2. The owner obtains the CM4 + CM5 bring-up hardware.
3. Commit and push this change once the owner approves. *(Done: `6efadce`, committed 2026-10-08 17:35 with this message and on `origin`; who ran the commit is not recorded.)* Proposed commit (Rule 15):

```text
Commit title: docs: two H.264 encodes - recording and shared live (OQ-005)
Commit description: Record the owner's answer to OQ-005 ("Separate record + live"): one
  recording encode and one live encode shared by RTMP and WebRTC, with the WebRTC
  constraints on the live encode; add OQ-115 (two concurrent encodes on the CM4 hardware
  encoder); annotate RISK-002, RISK-003, ADR-004, ADR-007; update the encoder, performance,
  DMA, streaming, recording, architecture, testing and traceability documents.
  No hardware or code exists; nothing is tested.
Files changed: docs/** (16 Markdown files)
Reason: Rules 1, 11, 13, 22 - owner decision and its consequences documented
Tests: documentation consistency check - 0 problems (DEVELOPMENT_LOG.md 2026-10-08, third entry)
```

---

## 2026-10-08 (fourth entry) — Storage and latency research (topics J and K), ADR-009, OQ-116 *(written 2026-10-09)*

### Note on this entry

The session that did this work (2026-10-08 evening to 2026-10-09 morning) wrote no log entry, changelog entry or status update. This entry was written on 2026-10-09 by the next session and is reconstructed from:

- commit `54269bf` (its diff);
- `docs/research/2026-10-08-storage-latency-research.json`;
- the dated text and change-history rows that session added to the four registers;
- that session's scratchpad: the workflow script `storage-latency-propagation.js` (file time 2026-10-09 10:20) and `edit_decisions.py` (10:43).

Where the evidence does not say why something happened, this entry says so.

### Objective

Establish from verified sources what the owner's 2026-10-08 decisions on recording storage and live latency imply for CM4 and CM5, record the owner's follow-up decisions, and propagate them.

### Starting State

`6efadce` (two H.264 encodes) on `origin/main`. Source register: 467 facts (topics A–I). OQ-005 and OQ-006 OPEN.

### Changes

1. **Owner decisions** (2026-10-08; owner's words as recorded in ADR-009, OQ-005, OQ-006 and OQ-116):

   | Question | Owner's answer | Recorded as |
   |---|---|---|
   | OQ-006: recording container and storage | "MP4"; "pcie nvme and usb to sata hdd" | REQ-REC-001 note; OQ-006 owner input |
   | OQ-005: live latency | "Under 1 second" (camera-to-viewer) | REQ-STR-002 note; new OQ-116 |
   | OQ-116: does the target apply to RTMP? | "WebRTC viewers only" | OQ-116 ANSWERED; RTMP best-effort |
   | Use of the two drives | "Record to both at once (mirror)" | ADR-009 |
   | HDD power | "Self-powered enclosure" | ADR-009 |
   | Power-loss strategy | "Fragmented MP4" | ADR-009 ACCEPTED |

   ext4 on the recording volumes was written into ADR-009 as Claude's proposal, not as an owner decision.
2. **Research workflow** `wf_2f325497-f51` (researcher plus independent adversarial verifier per topic):
   - Topic J, recording storage and power loss: 45 facts, 39 CONFIRMED and 6 CORRECTED.
   - Topic K, sub-second live latency: 45 facts, 39 CONFIRMED and 6 CORRECTED.
   - Appended to `REFERENCES.md` (now 557 facts) without changing existing entries; raw data in `docs/research/2026-10-08-storage-latency-research.json`.
3. **Registers** (propagation workflow `storage-latency-propagation.js`, phase "Registers"):
   - DECISIONS.md: ADR-009 added, ACCEPTED; dated evidence in ADR-004 and ADR-007, no status changed.
   - OPEN_QUESTIONS.md: OQ-116 added and ANSWERED; OQ-117 to OQ-128 added; Known-so-far notes on existing OQs. 128 OQs: 121 OPEN, 7 ANSWERED.
   - RISKS.md: RISK-026 to RISK-034 added (34 risks, all OPEN).
   - REQUIREMENTS.md: dated notes on REQ-ENC-001, REQ-REC-001, REQ-STR-001 and REQ-STR-002; no wording, acceptance or status changed.
   - The script records that the registers agent was interrupted by a network loss after OQ-117 to OQ-128 and RISK-026 to RISK-034 were written, and was resumed for REQUIREMENTS.md and DECISIONS.md. `edit_decisions.py` contains the ADR-004 and ADR-007 additions as scripted replacements.
4. **Not done.** The workflow's "Docs" phase (five file-owned agents for the technical documents) and "Verify" phase never ran. The evidence does not record why. `PROJECT_STATUS.md`, `CHANGELOG.md`, this log and every technical document were left as they were.
5. **Commit.** At 2026-10-09 10:46 the working tree was committed as `54269bf` "update 9oct" under the owner's git identity, with no Rule 15 proposal (6 files: the four registers, `REFERENCES.md` and the new research JSON) and it is on `origin/main`. The message does not follow Rule 15. It was not rewritten, because the commit is already on the shared remote and rewriting it would need a force push.

### Files Modified

`docs/DECISIONS.md`, `docs/OPEN_QUESTIONS.md`, `docs/REFERENCES.md` (appended), `docs/REQUIREMENTS.md`, `docs/RISKS.md`, `docs/research/2026-10-08-storage-latency-research.json` (new).

### Hardware Changes

None.

### Software Changes

None. Documentation only.

### Commands Used

Not recorded by that session. Known from the evidence: research workflow `wf_2f325497-f51`; propagation workflow script `storage-latency-propagation.js`; `git commit` for `54269bf`.

### Test Results

No documentation-check result was recorded. Run on 2026-10-09 against `54269bf`, before any edit (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=557 REQ=21 ADR=9 RISK=34 OQ=128 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=552/557
PROBLEMS (0):
```

The five uncited facts were K-19, K-20, K-22, K-23 and K-25. The check is mechanical; it does not show that the register text matched its citations. The planned verifier pass for that text never ran (see the 2026-10-09 entry for what it later found).

### Problems Found

1. Rules 5, 17 and 20 not met: no technical document reflected ADR-009, OQ-116 or topics J and K, and the status, changelog and log were stale (for example, `PROJECT_STATUS.md` still said the two-encode decision was uncommitted).
2. Rule 15: commit message "update 9oct".
3. The register text written from topics J and K was never verified.

### Root Cause

- Problems 1 and 3: the propagation workflow stopped after its first phase, and the session ended without the Rule 20 close-out. Why it stopped is not recorded.
- Problem 2: the commit was made outside the documented process (no proposed title, description, files, reason and tests; Rule 15). As with the 2026-10-07 commits, it appears to have been made by hand; the evidence does not show who ran it.

### Solution

The 2026-10-09 documentation catch-up (next entry).

### Current Status

- PARTIAL. Decisions and research recorded in the registers only.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

Complete the propagation and verification (2026-10-09 entry).

---

## 2026-10-09 — Documentation catch-up for `54269bf`: ADR-009 and the latency decision propagated, topics J and K verified

### Objective

Bring every document up to date with commit `54269bf` (topics J and K, ADR-009, OQ-116), run the verification pass that the previous session never ran, and complete the Rule 20 close-out that was missing.

### Starting State

- `54269bf` on `origin/main`; working tree clean.
- Documentation check: 0 problems, 552 of 557 facts cited (see the fourth 2026-10-08 entry).
- ADR-009, OQ-116, OQ-117 to OQ-128 and RISK-026 to RISK-034 appeared only in the four registers. `PROJECT_STATUS.md`, `CHANGELOG.md` and this log were stale.

### Changes

1. **Session-start report (Rule 19)** found the state above. One item in that report was wrong: it called line 1967 of `OPEN_QUESTIONS.md` stale ("109 of 116 entries are OPEN"). That line already carries a dated note with the current count, in the append-only style of Rule 21, so it was correct.
2. **Propagation.** Six file-owned agents (Claude Code `Agent` tool, run in parallel) updated the technical documents:
   - `RECORDING`, `PERFORMANCE` and `DMA`: ADR-009 recorder design, CM4 and CM5 storage paths, storage write budget, live-latency budget.
   - `STREAMING` and `VIDEO_ENCODER`: latency per output, WHIP/WHEP, MediaMTX, element defaults, HLS/LL-HLS, live-encode settings.
   - `HARDWARE`, `DEVICE_TREE`, `BUILD_SYSTEM` and `RELEASE`: storage, USB and power per board; CM5 `pciex1`; kernel options; NVMe boot; WebRTC packages.
   - `ARCHITECTURE` and `SOFTWARE_ARCHITECTURE`: two-writer recorder, candidate live path, latency rules.
   - `V4L2`, `CSI_PIPELINE` and `TC358743_DRIVER`: capture-side latency.
   - `TESTING`, `TRACEABILITY` and `TROUBLESHOOTING`: full TEST-REC-001 procedure; latency steps in TEST-STR-002 and TEST-STR-001; new trace rows; 12 new failure signatures.

   Outdated text was kept and marked superseded. Each file received one 2026-10-09 change-history row.
3. **Verification.** Five verifier agents, each owning its own files, checked every in-text J/K citation against its register entry, recomputed every number and checked the rules. About 2,200 citation instances were checked:

   | Files | Citations checked |
   |---|---|
   | ARCHITECTURE, SOFTWARE_ARCHITECTURE, DMA, V4L2, CSI_PIPELINE, TC358743_DRIVER | 460 |
   | RECORDING, PERFORMANCE, HARDWARE, DEVICE_TREE, BUILD_SYSTEM, RELEASE | 592 |
   | STREAMING, VIDEO_ENCODER | 289 |
   | TESTING, TROUBLESHOOTING, TRACEABILITY | 360 |
   | DECISIONS, OPEN_QUESTIONS, REQUIREMENTS, RISKS (the `54269bf` text) | 498 |

   Every fix is recorded in the "Verifier pass (same date)" sentence of the file's 2026-10-09 change-history row. Register corrections are dated notes; no status, score or ID changed.
4. **Main-session register work** (the registers had no owning writer agent):
   - corrections in `REQUIREMENTS.md`, `OPEN_QUESTIONS.md` and `DECISIONS.md` (see Problems Found);
   - six dated "Register notes" in `REFERENCES.md`; register entries were not edited;
   - OQ-008 now records the open owner decision on how the < 1 s target is judged;
   - OQ-126 records the unknown RP1 CFE behaviour when no buffer is queued;
   - OQ-127 names the missing CM5 `x264enc` on-demand-keyframe fact;
   - the OQ-050 summary row gained the marker its entry already names.
5. **`README.md`:** the fact-ID range now runs to `K-NN`; the "Project phase" header said PHASE 0 and now says PHASE 1; the recording row points to ADR-009.
6. **Consistency sweep** after the verifiers:
   - "self-powered enclosure or hub", where it was stated as the owner decision, reworded to the owner's words, "self-powered enclosure" (21 places in 5 files);
   - overwritten "no hardware exists as of" dates restored in 3 files (Rule 21).
7. `PROJECT_STATUS.md` and `CHANGELOG.md` brought up to date. The fourth 2026-10-08 entry above and this entry were written.

### Files Modified

All in `docs/`: `ARCHITECTURE.md`, `BUILD_SYSTEM.md`, `CHANGELOG.md`, `CSI_PIPELINE.md`, `DECISIONS.md`, `DEVELOPMENT_LOG.md`, `DEVICE_TREE.md`, `DMA.md`, `HARDWARE.md`, `OPEN_QUESTIONS.md`, `PERFORMANCE.md`, `PROJECT_STATUS.md`, `README.md`, `RECORDING.md`, `REFERENCES.md` (register notes only), `RELEASE.md`, `REQUIREMENTS.md`, `RISKS.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TC358743_DRIVER.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `V4L2.md`, `VIDEO_ENCODER.md`.

Not modified: `ATEM.md`, `ARCHIVED_APPROACHES.md` (nothing abandoned), `ENGINEERING_RULES.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation only.

### Commands Used

```bash
git status; git log; git show --stat 54269bf
cp <2026-10-06 session scratchpad>/doccheck.py <this session's scratchpad>/   # the check script is not in the repository
python3 -I doccheck.py docs                  # before, during and after
git diff -U0 --word-diff=plain -- docs       # Rule 21 audit of deleted text
```

### Test Results

Documentation consistency check after all edits (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=557 REQ=21 ADR=9 RISK=34 OQ=128 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=557/557
PROBLEMS (0):
WARNINGS (7):
```

All seven warnings are false positives of the success-word pattern:

- datasheet and muxer phrases quoted from the register ("the mdat size is fixed up", "TDM is fixed at 8 channels") in `REFERENCES.md` (2), `RECORDING.md` and `TROUBLESHOOTING.md`;
- a 2026-10-08 change-history row in `REQUIREMENTS.md` that quotes the old wording "fixed at 0";
- two lines of this log that quote those phrases.

- **Result: TESTED — PASS** for documentation consistency only. This is not a test of any product function.
- All 557 register facts are now cited; before this session, K-19, K-20, K-22, K-23 and K-25 were not.
- No hardware or software test was possible.

### Problems Found

1. **Errors in the unverified `54269bf` register text**, corrected with dated notes:
   - **CM4 encoder sharing stated as fact.** "The recording encode shares it" should read "would share it", because OQ-115 is open. Found in ADR-004, OQ-115, OQ-125, RISK-024, RISK-031 and REQ-STR-002.
   - **[J-30] generalised.** One Seagate 2.5-inch family's figures were written as general HDD figures, and its standby-to-ready times were called spin-up times. Found in ADR-009, OQ-006, OQ-023 and RISK-028. [J-32] (3.5-inch) had the same problem.
   - **Decodability of fragmented MP4 after interruption.** Only FFmpeg's documentation states it [J-45], but it was cited to [J-39] or applied to GStreamer (ADR-009, RISK-030).
   - **YouTube latency.** "Under 5 s at best" and "cannot reach the target" go beyond [K-15] and [K-18] (REQ-STR-001, OQ-007, OQ-116, OQ-125, RISK-031).
   - **Recording rate.** "The highest recording rate" is only the top of an assumed 8–25 Mbit/s range, because bitrate is still open (OQ-005).
   - **Wrong board.** [J-19]'s shared VBUS belongs to the CM5 IO Board, not the module.
   - **Unsourced config line.** The bare `dtparam=pciex1` line is not quoted by any source; [J-15] says to set `pciex1` to "on".
   - **ADR-009 owner attribution.** "Or hub" and ext4 were presented as owner decisions.
   - **Single route.** ADR-007 presented one GStreamer route to browser viewers as the only one.
   - **x264 B-frames.** [K-29]'s rule for what removes B-frames was misstated (OQ-005, RISK-032).
   - **Budget wording.** [K-45]'s "documented or extrapolated" terms were called "documented".
   - **Readout wording.** [K-34] says *at least* one frame's readout on CM4; the text said "about".
   - **Stale acceptance items.** The latency target (REQ-ENC-001, REQ-STR-002), and the container, storage medium and power-loss behaviour (REQ-REC-001), were still listed as undefined without a superseded mark.
2. **Register entry caveats**, recorded as register notes rather than edits:
   - [K-06]'s Evidence says the Raspberry Pi archive was not checked, but the verifier's note shows it was.
   - [K-26] and [J-23] cite the wrong input IDs.
   - The tier labels of [K-31], [K-32], [K-36], [K-37] and [I-37] do not match their sources.
   - MediaMTX sources are tiered `community` in topic F but `vendor-other` in topic K.
3. **The writer agents repeated the same patterns** in the technical documents, especially CM4 "shares", the [J-30] generalisation and the bare `pciex1` line. They also made these mistakes:
   - they left out the open acceptance criterion for the latency target;
   - they wrote troubleshooting symptoms that no source describes, without a reasoning label;
   - they described x264's default B-frame behaviour incorrectly;
   - they overwrote "as of" dates;
   - they made one arithmetic slip (2 × 1.52 shown as 3.05).

   The verifiers corrected all of these.
4. **My own over-correction.** Of OQ-123 I first wrote that [J-02] does not support "often fixes" for `pci=nomsi`. The registers verifier found that [J-02]'s Evidence quotes the datasheet's "often fixes the issue"; my note follows the Fact's narrower wording. A verifier note in OQ-123 records this.
5. **Left open (recorded, not fixed):**
   - Some RISK → OQ links are one-way (RISK-024, RISK-028, RISK-031, RISK-032), as are 18 older links.
   - OPEN_QUESTIONS.md Appendix B does not map OQ-116, nor some of the new OQs to ADR-007, REQ-STR-002 and REQ-BLD-002.
   - ADR-007's Decision paragraph names only two publishing routes.
   - The "by default" qualifier on the CM5 M.2 Gen 2 x1 rate is missing in ADR-004, OQ-011 and REQ-REC-001.
   - TEST-PLT-001 and TEST-BLD-001 have no steps for OQ-124 and OQ-120; TEST-REC-001 step 1 covers the M.2 check instead.

### Root Cause

- Problems 1 and 2: the `54269bf` register text was never verified (see the fourth 2026-10-08 entry).
- Problem 3: the writers copied wording from the uncorrected registers. The register corrections were made while they were running.
- Problem 4: I compared against the Fact text only, not the Evidence.

### Solution

The corrections are in place as dated notes, plus the verifier passes. Text is now checked against the Fact and the register notes. The register notes tell future writers how to read the affected entries.

### Current Status

- PARTIAL. The documentation matches every owner decision up to 2026-10-08, and the check passes.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. The owner reviews this catch-up and decides whether to commit. *(Done: the owner approved on 2026-10-09 ("Approve"); committed with this message, not pushed at the time.)* *(Pushed later on 2026-10-09 at the owner's request: `54269bf..f39e661`.)* Proposed commit (Rule 15):

```text
Commit title: docs: propagate ADR-009 and live-latency decision, verify topics J and K
Commit description: Complete the documentation for 54269bf (research topics J and K,
  ADR-009 ACCEPTED, OQ-116 ANSWERED), which had updated only the registers.
  Technical documents: fragmented-MP4 recorder mirrored to NVMe SSD and HDD,
  CM4/CM5 storage paths, storage and latency budgets, WebRTC < 1 s target (RTMP
  best-effort), live-encode settings, capture latency, TEST-REC-001 procedure,
  TEST-STR-001/002 latency steps, 12 troubleshooting signatures. Verification
  pass over ~2,200 J/K citations; register corrections as dated notes; register
  notes in REFERENCES.md. OQ-008 now records how the < 1 s target is judged
  (owner decision). Reconstructed log entry for 54269bf; status, changelog and
  README brought up to date. No hardware or code exists; nothing is tested.
Files changed: docs/** (26 Markdown files)
Reason: Rules 5, 17, 20, 21, 22 - documentation must match decisions and sources
Tests: documentation consistency check - 0 problems, 557/557 facts cited
  (DEVELOPMENT_LOG.md 2026-10-09)
```

2. The owner decides OQ-008 (WebRTC reach and how the < 1 s target is judged), OQ-005 (bitrate and rate control) and OQ-006 (maximum recording duration). *(Owner, 2026-10-09: "okay"; the decisions themselves are still open.)* *(Decided in part later on 2026-10-09: OQ-008 criterion and reach, OQ-006 duration — see the second 2026-10-09 entry. OQ-005 is still open.)*
3. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage. *(Owner, 2026-10-09: "okay".)*


---

## 2026-10-09 (second entry) — Commit and push of the catch-up; owner decisions on latency criterion, viewer reach and recording duration

### Objective

Commit and push the catch-up as approved, record the owner's answers on how the < 1 s target is judged, where WebRTC viewers are and how long a recording may run, and propagate them.

### Starting State

The 2026-10-09 catch-up was complete and uncommitted. The documentation check passed (0 problems, 557 of 557 facts cited).

### Changes

1. **Commit and push.**
   - The owner approved the proposed commit ("Approve"). It was committed as `f39e661` "docs: propagate ADR-009 and live-latency decision, verify topics J and K", with the approved message (26 files, counted from the staged set).
   - The owner answered "Push now": pushed `54269bf..f39e661`. `git ls-remote` confirmed `refs/heads/main = f39e661`.
2. **Owner decisions** (2026-10-09). Each was asked as a multiple-choice question; the owner's choice is quoted:

   | Question | Owner's choice | Recorded as |
   |---|---|---|
   | How is the < 1 s WebRTC target judged? (OQ-008) | "95th percentile < 1 s" (presented as: 95 % of samples under 1 s over a sustained run with the recording running) | OQ-008 owner input; REQ-STR-002 latency-criterion line. OQ-008 stays OPEN for browsers, viewer count, sample count and run length. |
   | Where are WebRTC viewers? (OQ-008) | "LAN only" | OQ-008 owner input; REQ-STR-002 acceptance item superseded; OQ-128 and RISK-033 marked not in current scope (kept OPEN as reference); OQ-074 scope note |
   | Maximum length of one recording? (OQ-006) | "Until stopped / disk full" | **OQ-006 ANSWERED** (every part now decided); REQ-REC-001 acceptance item superseded; ADR-009 Consequences note |

   The 95th-percentile option was my recommendation, labelled as my reasoning when it was presented. The other two questions had no recommended option.
3. **New OQ-129** (OPEN): what the recorder does when one mirrored drive fills, is absent or fails before the other, and whether long recordings are split into files. With no duration limit, a drive filling becomes a normal end state, and the mirror's drives may differ in size. Known so far: [J-36] sizing reasoning, [J-44] `splitmuxsink`, [J-33] absent-disk boot delay. Register: 129 OQs, 121 OPEN, 8 ANSWERED.
4. **Propagation.** Four file-owned agents marked every outdated statement with a dated note:
   - `STREAMING`, `PERFORMANCE` and `VIDEO_ENCODER`;
   - `ARCHITECTURE` and `SOFTWARE_ARCHITECTURE`;
   - `RECORDING` and `HARDWARE`;
   - `TESTING`, `TRACEABILITY` and `TROUBLESHOOTING`.

   They made no new fact claims; the only citations used were existing ones ([J-36], [J-43], [J-44]). The internet-viewer material is kept and labelled not in current scope, as was done for the deferred H.265 material. TEST-STR-002 now states the 95th-percentile criterion; its internet-viewer step is kept as reference. TEST-REC-001 gains step 9 (one drive fills first; the HDD is removed during a recording; file splitting if chosen). Its required behaviour is UNDEFINED until OQ-129 is decided. `ATEM.md` and `DMA.md` were checked and needed no change.
5. **Main session.** I updated the registers (`OPEN_QUESTIONS`, `REQUIREMENTS`, `RISKS`, `DECISIONS`) and `PROJECT_STATUS.md`. I also reviewed the agents' diffs and swept for unmarked outdated statements; one was left in REQ-REC-001, and it is now marked.

### Files Modified

All in `docs/`: `ARCHITECTURE.md`, `CHANGELOG.md`, `DECISIONS.md`, `DEVELOPMENT_LOG.md`, `HARDWARE.md`, `OPEN_QUESTIONS.md`, `PERFORMANCE.md`, `PROJECT_STATUS.md`, `RECORDING.md`, `REQUIREMENTS.md`, `RISKS.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `VIDEO_ENCODER.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation and git only.

### Commands Used

```bash
git add docs/ && git commit -F -          # f39e661, approved message
git push origin main                      # 54269bf..f39e661
git ls-remote --heads origin main         # f39e661
python3 -I doccheck.py docs
git diff -U0 --word-diff=porcelain -- docs   # Rule 21 audit against f39e661
```

### Test Results

Documentation consistency check after all edits (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=557 REQ=21 ADR=9 RISK=34 OQ=129 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=557/557
PROBLEMS (0):
WARNINGS (7):
```

- The seven warnings are the same false positives as in the first 2026-10-09 entry.
- Rule 21 audit against `f39e661`: no committed change-history row was modified. In the registers and technical documents, every deleted word chunk is an in-place extension, a status field or a count. `PROJECT_STATUS.md` and the changelog's Known Issues are current-state lists and were rewritten in place, as in earlier sessions; each change is recorded in its change history.
- **Result: TESTED — PASS** for documentation consistency only. No hardware or software test was possible.

### Problems Found

1. **My error, corrected before commit.** I first appended today's register notes to change-history rows already committed in `f39e661` (`RISKS.md`, `REQUIREMENTS.md`, `DECISIONS.md`), which would have rewritten history. I moved them into new rows; the audit shows no committed row changed.
2. One outdated duration statement in REQ-REC-001 was missed by the register pass and found by the sweep. It is now marked.

### Root Cause

1. I reused the "append to this session's row" pattern after the row had been committed.

### Solution

1. New change-history rows for each new change set once the earlier rows are committed; the agents were briefed the same way.

### Current Status

- PARTIAL. The decisions are recorded and propagated, and the documentation check passes.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Commit this change once the owner approves. *(Done: owner, 2026-10-09: "Commit and push"; committed with this message and pushed — `6ad83d4`, `f39e661..6ad83d4`.)* Proposed commit (Rule 15):

```text
Commit title: docs: record latency criterion, LAN-only viewers and recording duration
Commit description: Record the owner decisions of 2026-10-09: the < 1 s WebRTC
  target is judged at the 95th percentile (OQ-008; sample count and run length
  still open); WebRTC viewers are LAN only (OQ-128 and RISK-033 kept as
  reference, not in current scope); recordings run until stopped or the disk is
  full (OQ-006 ANSWERED). Add OQ-129 (mirror behaviour when one drive fills or
  fails; file splitting). Mark the affected statements in the technical
  documents; TEST-STR-002 criterion and TEST-REC-001 step 9. Record the push of
  f39e661. No hardware or code exists; nothing is tested.
Files changed: docs/** (17 Markdown files)
Reason: Rules 1, 11, 13, 21, 22 - owner decisions and their consequences
  documented, outdated text marked, not deleted
Tests: documentation consistency check - 0 problems, 557/557 facts cited
  (DEVELOPMENT_LOG.md 2026-10-09, second entry)
```

2. The owner decides OQ-005 (bitrate and rate control) and OQ-129 (mirror behaviour when one drive fills or fails; file splitting).
3. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage.

---

## 2026-10-09 (third entry) — Commit and push of the decisions update; owner decisions on bitrate and drive failure

### Objective

Commit and push the second 2026-10-09 change as approved, record the owner's answers on bitrate, rate control and mirrored-drive failure, and propagate them.

### Starting State

The second 2026-10-09 change was complete and uncommitted. The documentation check passed.

### Changes

1. **Commit and push.** The owner answered "Commit and push". The change was committed as `6ad83d4` "docs: record latency criterion, LAN-only viewers and recording duration" (17 files, counted from the staged set) and pushed `f39e661..6ad83d4`; `git ls-remote` confirmed it.
2. **Owner decisions** (2026-10-09). Each question had options based on [D-13] (the CM4 encoder accepts 25 kbit/s to 25 Mbit/s, default 10 Mbit/s, VBR or CBR) and [H-29], [K-17] (YouTube: H.264 1080p60 at 17 Mbit/s recommended, 6 Mbit/s minimum, CBR). The options I recommended are marked; the owner chose each recommended option:

   | Question | Owner's choice | Recorded as |
   |---|---|---|
   | Live encode (RTMP and WebRTC): bitrate and rate control (OQ-005) | "CBR 17 Mbit/s" (recommended) | OQ-005 owner input; REQ-ENC-001 acceptance "bitrate" superseded; OQ-007 note (the RTMP video bitrate is set by the live encode) |
   | Recording encode bitrate (OQ-005); VBR was stated in the question | "25 Mbit/s" (recommended) | OQ-005 owner input; REQ-REC-001 note |
   | One mirrored drive fills, is missing or fails during a recording (OQ-129) | "Continue on the other drive" (recommended; alert the operator) | OQ-129 owner input; ADR-009 Consequences; RISK-028 note |

3. **What stays open**, recorded in the registers:
   - **OQ-005:** the recording encode's profile, level and B-frame settings, which the technical documents had listed under OQ-005, and a capture-to-file latency target, if one is required. The live encode's settings follow the WebRTC constraints (§7 of VIDEO_ENCODER.md).
   - **OQ-073:** whether 17 and 25 Mbit/s fit the maximum bitrate of the H.264 profile and level each encoder signals. The register holds only frame-size and macroblock-rate limits [F-40]: NEEDS VERIFICATION (DATASHEET REQUIRED).
   - **OQ-129:** file splitting; how the operator is alerted (OQ-091); whether a returning drive is used again; and a drive that is already absent when a recording starts. The question was worded "during a recording", so that case is not covered by the choice; where the documents assume it, the assumption is labelled as Claude's reading.
   - Encoder load at these rates: CM5 CPU (OQ-059) and CM4 with two hardware encodes (OQ-115).
   - No per-mode values were given, so whether 17 and 25 Mbit/s apply unchanged to, for example, 1080p30 or 720p is not stated.
4. **Propagation.** Three file-owned agents updated the documents:
   - `VIDEO_ENCODER`, `STREAMING` and `PERFORMANCE`: settings tables, the RTMP bitrate section, WebRTC per-viewer load as labelled reasoning, and the storage budget at 25 Mbit/s from [J-36] (about 3.15 MB/s, 11.34 GB/h and 88 h per 1 TB per drive; about 6.30 MB/s mirrored).
   - `RECORDING`, `HARDWARE`, `ARCHITECTURE`, `SOFTWARE_ARCHITECTURE` and `DMA`: recorder design requirement that a failing writer ends only its own copy and raises an operator alert; storage capacity at the decided rate.
   - `TESTING`, `TRACEABILITY` and `TROUBLESHOOTING`: TEST-ENC-001 sets and measures both bitrates; TEST-REC-001 step 9 expected behaviour is defined for the policy part, with a new step 9 d (a drive that returns, UNDEFINED); TEST-STR-001 and TEST-STR-002 use the live bitrate.

   No new fact IDs were cited.
5. **Main-session alignment** after the agents finished:
   - Two agents wrote that OQ-005 "no longer names" the recording encode's profile, level and B-frames, leaving them without an open question. The register keeps them under OQ-005, and the 37 phrases saying OQ-005 was open "only for" the latency target were aligned with it.
   - One agent stated a drive "absent when the recording starts" as decided. It is now labelled as Claude's reading (OQ-129).
6. `PROJECT_STATUS.md` updated.

### Files Modified

All in `docs/`: `ARCHITECTURE.md`, `CHANGELOG.md`, `DECISIONS.md`, `DEVELOPMENT_LOG.md`, `DMA.md`, `HARDWARE.md`, `OPEN_QUESTIONS.md`, `PERFORMANCE.md`, `PROJECT_STATUS.md`, `RECORDING.md`, `REQUIREMENTS.md`, `RISKS.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `VIDEO_ENCODER.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation and git only.

### Commands Used

```bash
git add docs/ && git commit -F -          # 6ad83d4, approved message
git push origin main                      # f39e661..6ad83d4
python3 -I doccheck.py docs
git diff -U0 --word-diff=plain -- docs    # Rule 21 audit against 6ad83d4
```

### Test Results

Documentation consistency check after all edits (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=557 REQ=21 ADR=9 RISK=34 OQ=129 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=557/557
PROBLEMS (0):
WARNINGS (7):
```

- The seven warnings are the same known false positives.
- Rule 21 audit against `6ad83d4`: no committed change-history row was modified. In the registers and technical documents, every deleted word chunk is an in-place extension.
- **Result: TESTED — PASS** for documentation consistency only. No hardware or software test was possible.

### Problems Found

1. **Orphaned settings.** Two agents dropped the recording encode's profile, level and B-frames from any open question. Fixed by keeping them under OQ-005.
2. **Owner's wording extended.** "Absent at start" was treated as covered by the owner's choice. Now labelled as Claude's reading and recorded as an open item of OQ-129.

### Root Cause

1. My register text first said OQ-005 stayed open "only" for a capture-to-file target, and the agents followed it.
2. My OQ-129 entry lists "absent at start" among the cases, but the question put to the owner said "during a recording".

### Solution

Register text corrected before propagation finished. The question wording and the recorded decision are now compared explicitly.

### Current Status

- PARTIAL. Encoding and recording parameters are decided except the items above, and the documentation check passes.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Commit this change once the owner approves. *(Done: owner, 2026-10-09: "Commit and push"; committed with this message and pushed — `b142871`, `6ad83d4..b142871`.)* Proposed commit (Rule 15):

```text
Commit title: docs: record live and recording bitrates and drive-failure policy
Commit description: Record the owner decisions of 2026-10-09: live encode CBR
  17 Mbit/s, recording encode 25 Mbit/s VBR (OQ-005, which stays open for the
  recording encode's profile, level and B-frames and a capture-to-file
  latency target); if one mirrored drive fills, is missing or fails during a
  recording, recording continues on the other drive and the operator is
  alerted (OQ-129 stays open for file splitting, alert method, drive return
  and a drive absent at the start). Profile/level bitrate fit is NEEDS
  VERIFICATION (OQ-073). Propagate to the encoder, streaming, performance,
  recording, hardware, architecture, DMA, testing, traceability and
  troubleshooting documents. Record the push of 6ad83d4.
  No hardware or code exists; nothing is tested.
Files changed: docs/** (18 Markdown files)
Reason: Rules 1, 11, 13, 21, 22 - owner decisions and their consequences
  documented, open items kept visible
Tests: documentation consistency check - 0 problems, 557/557 facts cited
  (DEVELOPMENT_LOG.md 2026-10-09, third entry)
```

2. The owner decides the OQ-129 remainder (file splitting; a drive absent at the start) and the OQ-008 remainder (browsers, viewer count, sample count and run length). *(OQ-129 items decided later on 2026-10-09 — see the fourth entry. The OQ-008 remainder is still open.)*
3. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage.

---

## 2026-10-09 (fourth entry) — Commit and push of the bitrate update; drive missing at start, 30-minute file splitting, drive return, viewers and browsers

### Objective

Commit and push the third 2026-10-09 change as approved, record the owner's answers on the remaining OQ-129 items and on WebRTC viewers and browsers (OQ-008), and propagate them.

### Starting State

The third 2026-10-09 change was complete and uncommitted. The documentation check passed.

### Changes

1. **Commit and push.** The owner answered "Commit and push". The change was committed as `b142871` "docs: record live and recording bitrates and drive-failure policy" (18 files) and pushed `6ad83d4..b142871`; `git ls-remote` confirmed it.
2. **Owner decisions** (2026-10-09, on OQ-129):

   | Question | Owner's choice | Recorded as |
   |---|---|---|
   | One recording drive is already missing when a recording is started | "Start on the available drive" (recommended; alert the operator) | OQ-129 owner input; REQ-REC-001 and ADR-009 notes. The earlier labels "absent at start is Claude's reading" are marked superseded. |
   | Should long recordings be split into several files? | "Split every 30 minutes" (no option was recommended) | OQ-129 owner input; OQ-118 note |
   | Commit the update so far? | "Not yet" | Not committed; the next decisions were added to the same change |
   | A drive drops out during a recording and comes back: use it again? | "Resume at the next 30-min file" (recommended) | OQ-129 owner input; **OQ-129 ANSWERED**; full-disk muxer behaviour carried to OQ-118; RISK-027 note |
   | How many simultaneous WebRTC viewers? (OQ-008) | "Up to 5 viewers" (no option was recommended) | OQ-008 owner input |
   | Which browsers? (OQ-008, multiple choice) | Firefox, Safari, Chrome | OQ-008 owner input; REQ-STR-002 acceptance "target browsers" superseded; RISK-019 note. OQ-008 stays OPEN only for the sample count and run length of the measurement. |

3. **Consequences recorded:**
   - **File size.** Reasoning from [J-36]: about 5.67 GB per 30-minute file per drive, at 25 Mbit/s plus the research's assumed 192 kbit/s AAC.
   - **Splitting mechanism.** GStreamer `splitmuxsink` is a candidate, not a decision (ADR-007 OPEN). It starts a new file at a video keyframe before `max-size-time` is crossed, and it can request keyframes upstream [J-44]. Whether it works with `mp4mux` in fragmented mode, and how FFmpeg would split, is NEEDS VERIFICATION (OQ-118).
   - **What stays open.** OQ-129 is ANSWERED; the muxers' behaviour on a full disk is a build test under OQ-118, and the alert method belongs to OQ-091.
   - **Network egress.** Reasoning (not a measurement): 5 viewers at about 17 Mbit/s each plus one RTMP destination at about 17 Mbit/s is about 102 Mbit/s of video egress before audio and overhead. That is above a 100 Mbit/s Fast Ethernet link, so each board's Ethernet speed matters; it has not been researched (OQ-098 note). Register: 129 OQs, 120 OPEN, 9 ANSWERED.
4. **Propagation.** Three file-owned agents added dated notes:
   - `RECORDING`, `VIDEO_ENCODER`, `PERFORMANCE` and `HARDWARE`;
   - `ARCHITECTURE`, `SOFTWARE_ARCHITECTURE` and `STREAMING`;
   - `TESTING`, `TRACEABILITY` and `TROUBLESHOOTING`.

   TEST-REC-001 step 8 now has a defined expectation (the recording starts on the available drive, with an alert). Step 9 c checks a new file about every 30 minutes on each drive, each decodable, with no frames lost or duplicated at a boundary; the boundary tolerance is UNDEFINED (OQ-017). Three test additions are labelled as Claude's proposals:
   - starting with the NVMe SSD missing, where the board can still boot;
   - recording for more than 60 minutes;
   - noting frame drops at file boundaries during the soak.

   No new fact IDs were cited.
5. **Main session.** I updated the registers (`OPEN_QUESTIONS`, `REQUIREMENTS`, `DECISIONS`) and `PROJECT_STATUS.md`, reviewed the agents' diffs, and added the full-disk muxer item to OQ-129's open list.

### Files Modified

All in `docs/`: `ARCHITECTURE.md`, `CHANGELOG.md`, `DECISIONS.md`, `DEVELOPMENT_LOG.md`, `HARDWARE.md`, `OPEN_QUESTIONS.md`, `PERFORMANCE.md`, `PROJECT_STATUS.md`, `RECORDING.md`, `REQUIREMENTS.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `VIDEO_ENCODER.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation and git only.

### Commands Used

```bash
git add docs/ && git commit -F -          # b142871, approved message
git push origin main                      # 6ad83d4..b142871
python3 -I doccheck.py docs
git diff -U0 --word-diff=plain -- docs    # Rule 21 audit against b142871
```

### Test Results

Documentation consistency check after all edits (`python3 doccheck.py docs`, exit code 0):

```text
defined: facts=557 REQ=21 ADR=9 RISK=34 OQ=129 TEST(canon)=17 TEST(in TESTING.md)=17
files=29 distinct facts cited=557/557
PROBLEMS (0):
WARNINGS (7):
```

- The seven warnings are the same known false positives.
- Rule 21 audit against `b142871`: no committed change-history row was modified. In the registers and technical documents, every deleted word chunk is an in-place extension.
- **Result: TESTED — PASS** for documentation consistency only. No hardware or software test was possible.

### Problems Found

None new.

### Root Cause

—

### Solution

—

### Current Status

- PARTIAL. The recording behaviour is decided except for drive return; the documentation check passes.
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Commit this change once the owner approves. Proposed commit (Rule 15):

```text
Commit title: docs: record mirror-drive handling, file splitting, viewers and browsers
Commit description: Record the owner decisions of 2026-10-09. OQ-129
  ANSWERED: a recording started with one mirrored drive missing runs on the
  available drive with an operator alert; recordings are split into a new
  file every 30 minutes (about 5.67 GB per file per drive at 25 Mbit/s,
  reasoning from J-36; splitmuxsink is a candidate, J-44; fragmented-mode
  compatibility and full-disk muxer behaviour NEEDS VERIFICATION, OQ-118);
  a returning drive is used again from the next file. OQ-008: up to 5 LAN
  viewers; Chrome, Safari and Firefox; video egress about 102 Mbit/s with one
  RTMP destination (reasoning; Ethernet speed not researched, OQ-098).
  Propagate to the technical documents (TEST-REC-001 steps 8, 9 c, 9 d;
  TEST-STR-002 browsers and viewers). Record the push of b142871.
  No hardware or code exists; nothing is tested.
Files changed: docs/** (<N> Markdown files)
Reason: Rules 1, 11, 13, 21, 22 - owner decisions and their consequences
  documented, outdated text marked, not deleted
Tests: documentation consistency check - 0 problems, 557/557 facts cited
  (DEVELOPMENT_LOG.md 2026-10-09, fourth entry)
```

2. The owner decides the remaining owner-decision items:
   - OQ-008: the sample count and run length of the measurement;
   - OQ-005: the recording encode's profile, level and B-frames, and a capture-to-file target if one is needed.
3. Proposed (Claude): research the Ethernet facts of CM4, CM5 and their IO Boards (OQ-098).
4. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage.

## 2026-10-09 (fifth entry) — Recording encode (profile/level/B-frames), recording capture-to-file latency, WebRTC measurement run; OQ-005 and OQ-008 answered, OQ-130 opened

### Objective

Record and propagate the owner's answers to the last owner-decision remainders of OQ-005 and OQ-008: the recording encode's profile, level and B-frames; a capture-to-file latency target for recordings; and the sample count and run length of the WebRTC 95th-percentile latency measurement.

### Starting State

The fourth 2026-10-09 change (OQ-129 drive return; viewers and browsers) was complete and **uncommitted** (owner: "Not yet" on the fourth entry; on session start the owner chose to leave it uncommitted and work the remaining OQs). OQ-005 was answered except for the recording encode's profile/level/B-frames and a possible capture-to-file latency target; OQ-008 was answered except for the measurement's sample count and run length. `doccheck.py` is not in the repository or the session workspace.

### Changes

Owner decisions (2026-10-09), gathered by three `ask_user` questions:

| Question | Owner's choice | Recorded as |
|---|---|---|
| WebRTC 95th-percentile measurement run and sampling (OQ-008) | "30-minute run, 1 sample/second (~1800 samples)" | OQ-008 owner input (third); **OQ-008 ANSWERED** |
| Recording encode profile, level and B-frames (OQ-005) | "High profile, Level 4.2, no B-frames — uniform across CM4 and CM5" | OQ-005 owner input (third) |
| Recording capture-to-file latency target (OQ-005) | "Require a bounded capture-to-file latency (e.g. < 1 s glass-to-disk)" | OQ-005 owner input (third); **OQ-005 ANSWERED** |

Consequences recorded:

- **Recording encode** = H.264 High profile, Level 4.2, no B-frames, the same on CM4 and CM5. Reasoning: High is the CM4 encoder's default [K-30] and one it offers [D-11]; Level 4.2 because 1080p60 exceeds Level 4.0's macroblock rate [F-40]; no B-frames so the two platforms produce comparable files — CM4 emits none [D-14] and CM5's `x264enc` must be forced B-frame-free (`bframes=0`, caps `profile=baseline`, or `tune=zerolatency`) because its medium-preset default is 3 B-frames [K-29]. Whether 25 Mbit/s fits Level 4.2's maximum bitrate is OQ-073.
- **Recording capture-to-file latency** = under 1 s glass-to-disk. New **OQ-130** (OPEN): against which drive (the fast NVMe copy, the HDD copy, or both) and over what statistic and sample count the bound is judged, given that the HDD branch can stall up to about 3.0 s from standby to ready [J-30] (RISK-028) and must be decoupled or buffered (OQ-117).
- **WebRTC measurement run** = a 30-minute run at 1 sample per second (about 1800 samples), with the recording encode and the mirrored HDD writer running. Reasoning: about 1800 samples make a 95th percentile stable, a 30-minute run matches the recording file-split boundary (OQ-129) and covers an HDD stall of up to about 3.0 s [J-30].
- Registers: **130 OQs (119 OPEN, 11 ANSWERED)**; OQ-005 and OQ-008 ANSWERED; OQ-130 added. Requirements (21), risks (34) and ADRs (9) unchanged. No requirement moved from DRAFT (OQ-017); no ADR status changed.

### Files Modified

All in `docs/`: `OPEN_QUESTIONS.md`, `REQUIREMENTS.md`, `TESTING.md`, `VIDEO_ENCODER.md`, `RECORDING.md`, `STREAMING.md`, `PERFORMANCE.md`, `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `TRACEABILITY.md`, `CHANGELOG.md`, `PROJECT_STATUS.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation only.

### Commands Used

```bash
git --no-pager status
grep / view / edit via the assistant's tools
```

### Test Results

**Documentation consistency check: NOT RUN — tool unavailable.** `doccheck.py`, referenced by earlier entries, is not committed to the repository and is not in the session workspace, so the "557/557 facts cited, 0 problems" check could not be reproduced this session. Status of that check: UNKNOWN — VERIFICATION REQUIRED (the checker tool or its specification is needed). Edits were kept consistent by hand: every superseded statement was annotated with a dated note rather than deleted (Rule 21), and no new fact IDs were cited (only existing [D-11], [D-14], [F-40], [J-30], [K-29], [K-30] were reused).

### Problems Found

- `doccheck.py` is not under version control, so the documented verification step cannot be run from a clean checkout. This breaks Rule 24 traceability for the documentation check. Recommended follow-up: commit the checker (or its specification) to the repository, or record in BUILD_SYSTEM/TESTING how to obtain it.
- Pre-existing count drift between documents: [TRACEABILITY.md](TRACEABILITY.md) carried "8 ANSWERED" in earlier 2026-10-09 notes while [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) carried "9 ANSWERED". The new TRACEABILITY note states the OPEN_QUESTIONS header (130: 119 OPEN, 11 ANSWERED) is authoritative; the earlier interim notes were left in place (Rule 21).

### Root Cause

—

### Solution

—

### Current Status

- PARTIAL. All owner-decision remainders of OQ-005 and OQ-008 are decided and documented; OQ-130 is the only new open item they raised.
- The documentation consistency check could not be run (tool missing).
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Commit the uncommitted 2026-10-09 documentation once the owner approves. This change stacks on the uncommitted fourth-entry change. Proposed commit (Rule 15):

```text
Commit title: docs: record recording-encode profile/level, recording latency and WebRTC measurement run
Commit description: Record the owner decisions of 2026-10-09. OQ-005 ANSWERED:
  the recording encode is H.264 High profile, Level 4.2, with no B-frames,
  the same on CM4 and CM5 (CM4 emits no B-frames, D-14; CM5 x264 forced
  B-frame-free, K-29; Level 4.2 for 1080p60, F-40; 25 Mbit/s vs the level
  ceiling is OQ-073); and the recording path has a capture-to-file latency
  target of under 1 s glass-to-disk. OQ-008 ANSWERED: the < 1 s / 95th-
  percentile WebRTC latency is measured over a 30-minute run at 1 sample per
  second (about 1800 samples) with the recording running; up to 5 LAN
  viewers; Chrome, Safari, Firefox. New OQ-130: against which drive, and over
  what statistic, the recording capture-to-file bound is judged, given the
  HDD stall (J-30, RISK-028, OQ-117). Propagate to the technical documents.
  No hardware or code exists; the documentation consistency check could not
  be run (doccheck.py not in the repository).
Files changed: docs/** (13 Markdown files, plus the fourth-entry files)
Reason: Rules 1, 11, 13, 16, 17, 21, 22 - owner decisions and consequences
  documented, superseded text annotated not deleted
Tests: documentation consistency check NOT RUN - doccheck.py unavailable
  (see this entry, Test Results)
```

2. The owner decides OQ-130 (which drive and which statistic the recording latency is judged against) and the remaining recording-design items (OQ-118 to OQ-121).
3. Proposed (Claude): commit `doccheck.py` (or its specification) so the documentation check is reproducible; research the Ethernet facts (OQ-098).
4. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage.

## 2026-10-09 (sixth entry) — OQ-130 answered: recording capture-to-file < 1 s judged against the primary drive (NVMe) at the 95th percentile

### Objective

Record and propagate the owner's answer to OQ-130, raised by the fifth entry: against which drive, and over what statistic, the recording capture-to-file `< 1 s` glass-to-disk target is judged.

### Starting State

The fifth 2026-10-09 change was complete and uncommitted (it stacks on the external "update by ai" commits, see below). OQ-130 was OPEN.

### Changes

Owner decisions (2026-10-09), by two `ask_user` questions:

| Question | Owner's choice | Recorded as |
|---|---|---|
| Against which drive is the recording < 1 s judged? (OQ-130) | "Whichever drive is the primary target; the other is a lagging mirror" | OQ-130 owner input |
| Over what statistic? (OQ-130) | "95th percentile, consistent with the WebRTC criterion (OQ-008)" | OQ-130 owner input; **OQ-130 ANSWERED** |

Consequences recorded:

- The `< 1 s` bound is judged against the **primary recording target at the 95th percentile**; the other drive is a **lagging mirror** not bound by the target.
- Reasoning (and Claude's reading, pending owner confirmation): the primary must be the **NVMe SSD**, because the USB-to-SATA HDD can stall up to about 3.0 s from standby to ready [J-30] (RISK-028) and so cannot sustain `< 1 s`; if the HDD were designated the primary, the target could not be guaranteed. The lagging mirror is then the decoupled or buffered HDD branch (OQ-117).
- This **refines ADR-009's mirror** into a latency-bound primary plus a lagging mirror; both drives still receive the full recording. Recorded in ADR-009 Consequences (ADR-009 stays ACCEPTED — a refinement, not a new decision).
- Registers: **130 OQs (118 OPEN, 12 ANSWERED)**. No requirement moved from DRAFT (OQ-017); no ADR status changed.

### Files Modified

All in `docs/`: `OPEN_QUESTIONS.md`, `DECISIONS.md`, `REQUIREMENTS.md`, `RECORDING.md`, `VIDEO_ENCODER.md`, `PERFORMANCE.md`, `TESTING.md`, `TRACEABILITY.md`, `CHANGELOG.md`, `PROJECT_STATUS.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation only.

### Commands Used

```bash
grep / view / edit via the assistant's tools
git --no-pager diff --stat
```

### Test Results

**Documentation consistency check: NOT RUN** — `doccheck.py` is not in the repository or session workspace (see the fifth entry). Consistency kept by hand; no new fact IDs cited (reused [J-30]). Status of that check: UNKNOWN — VERIFICATION REQUIRED.

### Problems Found

- During this session an external **"update by ai"** process committed the working tree partway through the fifth-entry edits (commits `fe26dd2`, `98779a4`, `9670f71`), so the documentation change is split between those commits and the working tree. All content is present and consistent; no edits were lost. This bypasses Rule 15 (owner-approved commits) and contradicts the owner's "leave uncommitted" choice.

### Root Cause

—

### Solution

—

### Current Status

- PARTIAL. OQ-130 is decided and propagated; the only product work remaining in Phase 1 is other owner decisions and hardware.
- The documentation consistency check could not be run (tool missing).
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Commit the uncommitted 2026-10-09 documentation once the owner approves (it stacks on the fifth-entry change and the external "update by ai" commits). Proposed commit (Rule 15):

```text
Commit title: docs: judge recording capture-to-file < 1 s against the primary drive (OQ-130)
Commit description: Record the owner decision of 2026-10-09 (OQ-130 ANSWERED):
  the recording capture-to-file < 1 s glass-to-disk target is judged against
  the primary recording target - the NVMe SSD, since the HDD can stall ~3.0 s
  (J-30, RISK-028) - at the 95th percentile, consistent with the WebRTC
  criterion (OQ-008); the HDD is a lagging mirror not bound by the target.
  Refine ADR-009's mirror into a latency-bound primary plus a lagging mirror
  (the decoupled HDD branch, OQ-117); both drives still receive the full
  recording. Propagate to the registers and technical documents.
  No hardware or code exists; the documentation consistency check could not
  be run (doccheck.py not in the repository).
Files changed: docs/** (11 Markdown files, plus the fifth-entry files)
Reason: Rules 1, 13, 21, 22 - owner decision and consequences documented
Tests: documentation consistency check NOT RUN - doccheck.py unavailable
```

2. The owner decides the remaining recording-design items (OQ-118 to OQ-121) and the operator interface (OQ-091).
3. Proposed (Claude): commit `doccheck.py` (or its specification); research the Ethernet facts (OQ-098); reconcile the split commit/working-tree state created by the external committer.
4. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage.

## 2026-10-09 (seventh entry) — Product platform decided: CM4; CM5, BCM2712, Pi 5 and Pi 4 Model B dropped (ADR-004 ACCEPTED)

### Objective

Record and propagate the owner's platform decision: "stick to CM4". Resolve ADR-004 to CM4, answer OQ-011, and archive CM5 (Rule 14).

### Starting State

ADR-004 was OPEN ("evaluate CM4 and CM5 side by side", owner 2026-10-07); OQ-011 OPEN. The CM5 footprint across the documentation is large (about 2,173 literal "CM5" mentions, plus Pi 5 / BCM2712 / x264 / x265).

### Changes

- Owner decision (2026-10-09), after I surfaced that CM4's hardware 1080p60 encode (RISK-002) and two-session behaviour (OQ-115) are unproven and single-platform-critical: **the product platform is the CM4 (BCM2711)**; CM5, BCM2712, Pi 5 and Pi 4 Model B are dropped as product boards.
- **ADR-004 ACCEPTED (CM4)**: Status, Decision and Consequences written; summary-table row O → A; ADR-004 change-history row.
- **OQ-011 ANSWERED**: owner-input, Answer/Evidence fields, summary row; header counts 130 (117 OPEN, 13 ANSWERED).
- **ARCH-001** added to [ARCHIVED_APPROACHES.md](ARCHIVED_APPROACHES.md): CM5 recorded as a considered/planned platform dropped before any hardware attempt (the file's intro was extended to cover this at the owner's direction; the per-ADR analysis stays in ADR-004 Alternatives).
- **REQ-PLT-001** scoped to CM4 (acceptance stays DRAFT, OQ-017).
- **RISK-003** (CM5 no hardware encoder) marked not in current scope (kept OPEN as reference); **RISK-014** scoped so its CM4 audio path stays in scope and only its CM5 parts are out of scope.
- **Scope rule** (mirrors the H.265 / REQ-ENC-002 precedent): CM5/BCM2712/Pi 5 material in the other documents is kept as reference and marked "not in current scope"; it is not deleted (Rule 14). The CM5-only OQs (OQ-050 to OQ-054, OQ-059, OQ-100, OQ-108, OQ-124) stay OPEN but out of scope.
- Reader-facing docs updated: PROJECT_STATUS (phase plan, Current Phase, Current Objective, Completed, Known Problems, Hardware, Next Step), HARDWARE (platform header), TESTING (platform row — run on CM4), TRACEABILITY (ADR-004 O → A, REQ-PLT-001 row, top note), and the VIDEO_ENCODER / ARCHITECTURE / SOFTWARE_ARCHITECTURE / PERFORMANCE "Applies to" headers. CHANGELOG entry and known-issue ADR line updated.

### Files Modified

All in `docs/`: `DECISIONS.md`, `OPEN_QUESTIONS.md`, `ARCHIVED_APPROACHES.md`, `REQUIREMENTS.md`, `RISKS.md`, `HARDWARE.md`, `TESTING.md`, `PROJECT_STATUS.md`, `TRACEABILITY.md`, `VIDEO_ENCODER.md`, `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `PERFORMANCE.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

None. Documentation only.

### Commands Used

```bash
grep / view / edit via the assistant's tools
perl -0pi -e 's/ADR-004 \(O\)/ADR-004 (A)/g' docs/TRACEABILITY.md
```

### Test Results

**Documentation consistency check: NOT RUN** — `doccheck.py` is not in the repository or session workspace (see earlier entries). Kept consistent by hand. Status of that check: UNKNOWN — VERIFICATION REQUIRED.

### Problems Found

- **Scale and residual CM5 references.** CM5 is referenced ~2,173 times. This entry updated the authoritative and reader-facing documents and set the scope rule, but did **not** rewrite every inline "CM4 and CM5" mention in the deep technical documents (DEVICE_TREE, DMA, CSI_PIPELINE, V4L2, TC358743_DRIVER, BUILD_SYSTEM, RELEASE, TROUBLESHOOTING, STREAMING, RECORDING, ATEM, and the deeper sections of VIDEO_ENCODER/ARCHITECTURE/SOFTWARE_ARCHITECTURE/PERFORMANCE). Those remain correct as *reference* material under the ADR-004 scope rule but still read as if CM5 were in scope in places. A follow-up sweep is recommended; it is tracked here rather than done in one pass to avoid mass inconsistency.
- `doccheck.py` is still not version-controlled.
- The external "update by ai" automation continued to commit the working tree mid-session.

### Root Cause

—

### Solution

—

### Current Status

- PARTIAL. The platform decision is made, archived and propagated to the authoritative and reader-facing documents; deep inline CM5 references remain as reference under the scope rule (follow-up sweep recommended).
- Product work is BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Commit the uncommitted 2026-10-09 documentation once the owner approves. Proposed commit (Rule 15): *(2026-10-12: committed by the owner by hand as `27c44d9` (committed by hand as "mmit", reworded before push on 2026-10-12) rather than with this message; not pushed as of 2026-10-12.)* *(Later on 2026-10-12: reworded to this message as `27c44d9` before push, at the owner's choice; content unchanged.)*

```text
Commit title: docs: decide product platform CM4, drop CM5 (ADR-004 ACCEPTED)
Commit description: Record the owner decision of 2026-10-09 ("stick to CM4"):
  the product platform is the Compute Module 4 (BCM2711); ADR-004 ACCEPTED;
  OQ-011 ANSWERED. Drop CM5, BCM2712, Pi 5 and Pi 4 Model B as product boards,
  keeping them as reference (Rule 14); archive CM5 as ARCH-001. Mark CM5-only
  risks (RISK-003, CM5 parts of RISK-014) and questions not in current scope,
  as the deferred H.265 material is. Update the authoritative and reader-facing
  documents and set the scope rule for the deep inline CM5 content.
  No hardware or code exists; the documentation consistency check could not be
  run (doccheck.py not in the repository).
Files changed: docs/** (15 Markdown files)
Reason: Rules 1, 11, 13, 14, 16, 21 - major decision, archived not deleted
Tests: documentation consistency check NOT RUN - doccheck.py unavailable
```

2. Proposed (Claude): sweep the remaining inline CM5 references in the deep technical documents under the ADR-004 scope rule. *(2026-10-12: done — see the 2026-10-12 entry.)*
3. The owner obtains the **CM4** bring-up hardware, including recording storage, and records it in HARDWARE.md as HW REV A.

## 2026-10-12 — Documentation check rebuilt, ADR-004 scope sweep, hand-commit catch-up, research topic L, TEST-ENC-001 Stage 0

### Objective

The owner asked "what next" and then "start" on the desk work proposed at session start: (1) rebuild the documentation check and keep it in the repository; (2) fix stale status and finish the ADR-004 (CM4-only) sweep; (3) resolve from sources what can be resolved without hardware — the bit-rate part of OQ-073, the CM4 Ethernet part of OQ-098 and the kernel-source part of OQ-115; (4) prepare the first hardware test so it runs the day a CM4 exists.

### Starting State

- `main` at `3c823b6` (later reworded to `27c44d9` before push — see Next Step) ("mmit", hand-committed 2026-10-12 10:19, not pushed; `origin/main` at `049d1ee`). Working tree clean.
- Since `b142871` six commits had non-Rule-15 messages: `fe26dd2` "correcting by ai", `98779a4` / `9670f71` / `2d76092` "update by ai", `049d1ee` "up date by ai", `27c44d9` (committed by hand as "mmit", reworded before push on 2026-10-12). Their content matched the 2026-10-09 fourth to seventh log entries, so nothing was undocumented in the log, but PROJECT_STATUS still called those decisions "uncommitted".
- The seventh 2026-10-09 entry (CM4 decision) was PARTIAL: deep inline CM5 content not swept.
- `doccheck.py` was gone: it had never been committed, and every scratchpad copy had been wiped. Its last run was in the fourth 2026-10-09 entry.
- No hardware, no code, nothing tested.

### Changes

1. **`tools/doccheck.py`** (new, version-controlled). Rebuilt from the check list in the 2026-10-06 entry. Problems: undefined fact citations; undefined REQ/ADR/RISK/OQ/TEST/ARCH IDs; broken relative `.md` links; TESTING.md test IDs differing from the README canonical list; missing Verification or Change-history sections; missing Rule 2 files; **new:** register header counts that disagree with the register body. Warnings: success words (Rule 10; REFERENCES.md is skipped because its success words are source quotes); **new:** register facts that nothing cites; **new:** lines where an ACCEPTED ADR or ANSWERED OQ may be shown as OPEN/PROPOSED. It is a re-implementation, so its warning counts are not comparable with runs logged before 2026-10-12.
2. **Baseline run** of the rebuilt check on `3c823b6` (same tree as `27c44d9`): 2 PROBLEMS (DECISIONS.md header "3 ACCEPTED, 2 OPEN" — the index had 4 and 1 since ADR-004 was accepted) and 91 stale-status warnings (mostly `ADR-004 (OPEN)`, also ADR-003, ADR-009 and answered OQs).
3. **ADR-004 scope sweep** (8 parallel agents, one shared specification, disjoint file sets; 22 documents). Append-only notes: "*(Superseded — ADR-004 ACCEPTED 2026-10-09 …)*" on stale statements; "not in current scope" markers on CM5/Pi 5/BCM2712-only sections and tables; header "Applies to" notes; one change-history row per file. DECISIONS.md header counter corrected to 4 ACCEPTED, 4 PROPOSED, 1 OPEN. OQ-049, OQ-055 and OQ-060 got scope notes as CM5-only; OQ-100 was left in scope (it asks whether a `[pi4]` section matches a CM4).
4. **OQ-129 propagation** (1 agent): the drive-return answer of 2026-10-09 had reached the registers but not the design text or the tests. Added to RECORDING, ARCHITECTURE, SOFTWARE_ARCHITECTURE, STREAMING, PERFORMANCE and TESTING TEST-REC-001 (new steps 9 e–g and Expected Result bullets; alert method and thresholds UNDEFINED, OQ-091, OQ-017).
5. **Corrections found by the sweep** (main session): OQ-011 answer and ADR-004 Consequences listed RISK-014 and OQ-100 as CM5-specific and omitted OQ-049/055/060 — dated correction notes. ADR-006 and ADR-008 — "Review note (2026-10-12)" subsections, because their CM5-based reasons no longer apply (decisions and status unchanged, owner decisions OQ-014 and OQ-099). REQUIREMENTS.md stale OQ-005/OQ-008 statements superseded. HARDWARE.md hazard 10 cited [C-27], a CM5 fact, for the CM4 — annotated DATASHEET REQUIRED (OQ-026). RECORDING.md Level 4.2 got the [D-12] caveat. "Uniform on CM4 and CM5" in the recorded OQ-005 answer annotated (CM5 dropped later that day). VIDEO_ENCODER.md §9 got an OQ-130 row.
6. **Catch-up change-history rows** (Rules 1, 21) in 11 documents for the 2026-10-09 edits that the hand commits carried without a row, computed from `git show` per commit and file. Where `3c823b6` (now `27c44d9`) rewrote text in place — the TESTING.md common-setup Platform row, the REQ-PLT-001 Notes line, five "Applies to" headers, and the TRACEABILITY `ADR-004 (O)` → `(A)` cells (`perl -0pi`) — the rows say so. The two procedural texts are quoted verbatim; the others reference `git show 27c44d9^`.
7. **Research topic L** (1 research agent, 2 independent adversarial verifiers): 63 facts, 56 CONFIRMED, 7 CORRECTED, 0 UNVERIFIABLE, 0 REFUTED. Appended to REFERENCES.md by a generator script (scratchpad), existing entries untouched; raw data `docs/research/2026-10-12-bitrate-network-encoder-research.json`. Register: 620 facts.
8. **Topic L propagation:** OQ-073 (bit-rate part resolved from ITU-T H.264 V16: 25 Mbit/s High L4.2 = 40 % of the VCL ceiling; 17 Mbit/s CBR fits CB/Main/High at L4.0–4.2, tightest 85 %), OQ-098 (CM4 on-board BCM54210PE Gigabit PHY, MAC on the SoC; 102 Mbit/s egress = 10.2 % of the link), OQ-115 (one firmware `ril.video_encode` per session, no driver limit, scheduling in firmware), OQ-056, OQ-057, OQ-101; RISK-002; VIDEO_ENCODER §3.5–3.7, §5, §6 (the 1080p50 NEEDS VERIFICATION answered: Level 4.1 does not suffice); HARDWARE §4.12; STREAMING; PERFORMANCE; ARCHITECTURE; TRACEABILITY state note; README fact range.
9. **TEST-ENC-001 Stage 0** (TESTING.md) and **`tools/encoder_capacity_test.sh`**: an encoder-only capacity run on a CM4 with a synthetic `videotestsrc` source — no TC358743, no storage. Pre-flight checks the encoder control names (a misspelt `extra-controls` field is silently skipped [L-55]) and input formats; generator control; run matrix R1–R7 (1.0× to 4.0× the 1080p30 specification); throughput and real-time modes; SoC clock, temperature and throttling at 1 Hz; per-session timeout (an unverified 2022 community report of a CBR hang is in the [L-63] verifier note). It prints measurements, not PASS/FAIL.
10. **README.md:** convention 5 superseded (CM4 product platform), `ARCH-NNN` in the ID table, fact range `A-NN` … `L-NN`, new convention 10 (run the check before committing documentation).

### Files Modified

- New: `tools/doccheck.py`, `tools/encoder_capacity_test.sh`, `docs/research/2026-10-12-bitrate-network-encoder-research.json`.
- `docs/`: `ARCHITECTURE.md`, `ATEM.md`, `BUILD_SYSTEM.md`, `CHANGELOG.md`, `CSI_PIPELINE.md`, `DECISIONS.md`, `DEVELOPMENT_LOG.md`, `DEVICE_TREE.md`, `DMA.md`, `HARDWARE.md`, `OPEN_QUESTIONS.md`, `PERFORMANCE.md`, `PROJECT_STATUS.md`, `README.md`, `RECORDING.md`, `REFERENCES.md`, `RELEASE.md`, `REQUIREMENTS.md`, `RISKS.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TC358743_DRIVER.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `V4L2.md`, `VIDEO_ENCODER.md`.
- Not modified: `ENGINEERING_RULES.md`, `ARCHIVED_APPROACHES.md`, `CLAUDE.md`.

### Hardware Changes

None. No hardware exists.

### Software Changes

No product software. Two repository tools (documentation check; encoder test script for future hardware).

### Commands Used

```bash
python3 tools/doccheck.py docs                      # baseline, after each step, final
git show --stat <commit> -- docs                    # hand-commit audit (fe26dd2 … 3c823b6)
git show <commit> --word-diff=plain -- docs/<file>  # in-place rewrites in 3c823b6
git diff --word-diff=porcelain -- docs              # Rule 21 audit: every removed word
bash -n tools/encoder_capacity_test.sh              # syntax
# dry run of the test script with stand-in gst-launch-1.0 / v4l2-ctl / vcgencmd / timeout on macOS
# research: Claude Code Agent tool — 1 researcher, 2 verifiers (L-01..L-31, L-32..L-63)
# sweep: 8 agents + 1 OQ-129 agent, shared written specification, disjoint files
```

### Test Results

- **Documentation consistency check — TESTED — PASS** (documentation only, not a product function). Final run, exit code 0:

```text
defined: facts=620 topics=ABCDEFGHIJKL REQ=21 ADR=9 RISK=34 OQ=130 TEST=17 ARCH=1 TEST(canon)=17
files=29 distinct facts cited=620/620
PROBLEMS (0):
WARNINGS (0):
```

- **Rule 21 audit** (`git diff --word-diff=porcelain` against `3c823b6`, same tree as `27c44d9`): the only removed tokens are the "Last updated" dates, the DECISIONS.md header counts ("3", "2"), and the REFERENCES.md header/total cells ("K)", "557", "512", "45") that the topic L append updated. Table column counts were compared before and after for every edited file: no table was broken.
- **`tools/encoder_capacity_test.sh`:** `bash -n` passes. A dry run on macOS with stand-in commands exercised the pre-flight checks (including stopping on a missing control name), both modes, result parsing and CSV output. **This tests the script's logic only. The script is NOT YET RUN ON PACSCORDER HARDWARE, and no encoder behaviour has been measured.**
- No hardware or product software test was possible.

### Problems Found

1. Hand-committed sessions had left 2026-10-09 edits without change-history rows, and `3c823b6` (now `27c44d9`) rewrote some text in place instead of appending (Rule 21). Recorded, not reverted.
2. The ADR-004 decision had not reached the status table of VIDEO_ENCODER.md or about 90 other statements; the DECISIONS.md header counts were stale.
3. The OQ-129 drive-return answer had not reached any design text or test.
4. The OQ-011 answer misclassified RISK-014 and OQ-100 as CM5-specific.
5. ADR-006 and ADR-008 (PROPOSED) rest partly on CM5 reasons that no longer apply.
6. One sweep note and one catch-up row I wrote first stated that two files lacked the OQ-130 answer; `git log -S` showed they had it. Corrected before commit.
7. **Register rule conflict (owner decision needed):** OPEN_QUESTIONS.md "How to use this register" says a question made irrelevant by a dropped platform is marked ANSWERED; the 2026-10-09 seventh entry instead kept the CM5-only OQs OPEN, not in current scope. Left as is.
8. Technical (from topic L): the only official CM4 encode capacity is 1080p30 [L-29], and a Raspberry Pi engineer's opinion is that two 1080p30 streams are probably too many [L-30] (community). FFmpeg's `h264_v4l2m2m` sets neither the bitrate mode nor the level [L-43].

### Root Cause

1–3: sessions stopped mid-work and were committed by hand with the "update by ai" automation; the earlier sweep was deliberately partial. 4: written quickly in the seventh entry. 6: my own unchecked assumption.

### Solution

See Changes. The check is now in the repository, so the stale-status and count problems are caught mechanically.

### Current Status

- PASS (documentation consistency). PARTIAL overall: the owner decisions listed in PROJECT_STATUS remain, and the commit is pending owner approval.
- Product work: BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Owner: approve the commit below, or ask for changes. Decide whether to push `3c823b6` and this commit. *(Committed 2026-10-12 on the owner's instruction ("procced"), with the proposed message; not pushed.)* *(Then, at the owner's choice: `3c823b6` "mmit" reworded to its Rule 15 message as `27c44d9`, this commit replayed on top of it, its references to `3c823b6` updated to `27c44d9`, and both pushed to `origin` on 2026-10-12.)*
2. Owner: obtain a CM4 (with the IO Board and 12 V supply) and run `tools/encoder_capacity_test.sh` (TEST-ENC-001 Stage 0). This is the cheapest measurement that settles whether the two-encode design is feasible (OQ-115, RISK-002).
3. Owner decisions surfaced today: the live encode's resolution and frame rate (it sets whether Stage 0 needs R3, R4 or R5 to pass; OQ-115); the dropped-platform OQ rule (Problems Found 7); ADR-006 and ADR-008 review.
4. Proposed commit (Rule 15):

```text
Commit title: docs: rebuild doc check, finish CM4 scope sweep, add topic L and encoder test stage
Commit description: Add tools/doccheck.py (rebuilt documentation consistency check, now
  version-controlled) and tools/encoder_capacity_test.sh (TEST-ENC-001 Stage 0, encoder-only
  CM4 capacity run; not yet run on hardware). Finish the ADR-004 (CM4) scope sweep across 22
  documents with append-only notes; propagate the OQ-129 drive-return answer into design and
  tests; add catch-up change-history rows for the hand-committed 2026-10-09 edits. Add research
  topic L (63 verified facts: ITU-T H.264 bit-rate ceilings, CM4 Ethernet, bcm2835-codec
  concurrency, encoder-test commands) and propagate it to OQ-073, OQ-098, OQ-115, RISK-002
  and the technical documents. No hardware or product code exists; nothing is tested on hardware.
Files changed: tools/doccheck.py, tools/encoder_capacity_test.sh, docs/** (27 Markdown files),
  docs/research/2026-10-12-bitrate-network-encoder-research.json
Reason: Rules 1, 3, 10, 14, 16, 17, 21, 22, 23, 24
Tests: python3 tools/doccheck.py docs → PROBLEMS 0, WARNINGS 0; Rule 21 word-diff audit;
  table-integrity check; bash -n and a stand-in dry run of the test script
```

---

## 2026-10-12 (second entry) — Commit, reword and push; live encode 1080p30 (OQ-131 opened); CM5-only questions ANSWERED

### Objective

Act on the owner's "procced" and answers: commit the 2026-10-12 work, reword the hand commit `3c823b6` before pushing, push; record the live-encode decision and the dropped-platform OQ rule.

### Starting State

- `main` at `d16adfd` (the first 2026-10-12 commit, made on "procced") on top of `3c823b6` "mmit"; both local, `origin/main` at `049d1ee`.
- Owner answers (2026-10-12, via the question tool): **"Reword 3c823b6, then push"**; live encode **"1080p30"**; CM5-only OQs **"Mark them ANSWERED"**.

### Changes

1. **Git.** `3c823b6` amended to its Rule 15 message (the one proposed in the seventh 2026-10-09 entry, plus a line recording the reword) → `27c44d9`; author unchanged (the owner); tree identical. `d16adfd` replayed on top (identical tree) and amended to replace its references to `3c823b6` with `27c44d9` (24 lines, all hash references, verified by diff) → `1f5c0e0`. `git fetch` confirmed `origin/main` was still `049d1ee`; pushed fast-forward `049d1ee..1f5c0e0`. No force push. The old objects `3c823b6` and `d16adfd` exist only in the local reflog.
2. **Live encode 1080p30** (owner; OQ-005). Recorded as owner input on OQ-005 (and its Answer), OQ-115 (the combinations the encoder must sustain: 3.0× the 1080p30 specification with a 1080p60 recording, 2.67× with 1080p50, 2.0× for 1080p30 sources — reasoning from [L-14], [D-10]) and a note on OQ-073 (Level 4). Propagated to REQ-ENC-001, VIDEO_ENCODER §1 and §7.1, RISK-002, PERFORMANCE, TRACEABILITY and new "Owner decisions of 2026-10-12" boxes in ARCHITECTURE, SOFTWARE_ARCHITECTURE and STREAMING.
3. **New OQ-131** — how the live path reaches 1080p30 from 60 Hz and 50 Hz captures and from non-1080 inputs (owner decision for 50 Hz sources: 25 or 30 fps; build/hardware test for the element and its cost).
4. **TEST-ENC-001 Stage 0:** run **R8** added (1080p50 recording + 1080p30 live, 652,800 MB/s, 2.67×) to the matrix and to `tools/encoder_capacity_test.sh`; dated notes that **R4 and R8 are the product runs** (R3 for 1080p30 sources) and that real-time mode should run at least R4 and R8.
5. **CM5-only OQs ANSWERED** (owner's instruction, following the register's own rule): OQ-049 to OQ-055, OQ-059, OQ-060, OQ-124 — Answer ("made irrelevant by ADR-004"; how to reopen), Evidence, Answered on/by, status and summary rows. **OQ-108 was not closed:** it asks whether browsers receive H.265 over WebRTC, which is H.265 material, not CM5; the 2026-10-09 ADR-004 text called it "CM5 H.264-beside-H.265" and Claude repeated that in the question put to the owner. Notes added to OQ-011, the category 6 header, the summary-table note, DECISIONS (ADR-004 Consequences) and ARCH-001. Register: **131 OQs (108 OPEN, 23 ANSWERED)**.
6. **`tools/doccheck.py` W3:** now looks for OPEN/PROPOSED only in the same clause as the settled ID (stops at a sentence end, a table-cell boundary, a semicolon or another ID). Closing the CM5 OQs produced five false positives where the word belonged to a neighbouring ADR or to the next sentence; a synthetic regression file confirmed that the real cases (`ADR-004 (OPEN)`, `OQ-008 stays OPEN`, `ADR-004, OPEN)`, `ADR-009 PROPOSED`) are still flagged and the two false-positive shapes are not.

### Files Modified

- Pushed in `27c44d9` / `1f5c0e0`: see the first 2026-10-12 entry.
- Uncommitted (this entry): `docs/OPEN_QUESTIONS.md`, `DECISIONS.md`, `ARCHIVED_APPROACHES.md`, `REQUIREMENTS.md`, `VIDEO_ENCODER.md`, `RISKS.md`, `PERFORMANCE.md`, `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TESTING.md`, `TRACEABILITY.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`; `tools/doccheck.py`, `tools/encoder_capacity_test.sh`.

### Hardware Changes

None.

### Software Changes

Tools only (W3 refinement; run R8).

### Commands Used

```bash
git checkout --detach 3c823b6 && git commit --amend -F <message>     # -> 27c44d9
git cherry-pick d16adfd                                              # -> replayed, same tree
git commit --amend --no-edit                                         # hash references -> 1f5c0e0
git branch -f main HEAD && git checkout main
git fetch origin && git merge-base --is-ancestor origin/main main && git push origin main
python3 tools/doccheck.py docs
```

### Test Results

- Documentation consistency check — **TESTED — PASS** (documentation only): `facts=620 … OQ=131`, `distinct facts cited=620/620`, `PROBLEMS (0)`, `WARNINGS (0)`.
- Rule 21 audit for this round: removed tokens are the OQ header counts, the 10 status lines and 10 summary-row status cells changed from OPEN to ANSWERED by the register's closing procedure, and one note inserted inside a parenthesis in TESTING.md. Tables intact.
- `bash -n tools/encoder_capacity_test.sh` passes; stand-in dry run of R8 (levels: recording 4.2, live 4). **NOT YET RUN ON PACSCORDER HARDWARE.**
- Push: `049d1ee..1f5c0e0  main -> main`.

### Problems Found

- OQ-108 mislabelled as CM5-specific (see Changes 5).
- The "Counts unchanged" line of the first 2026-10-12 TRACEABILITY state note became out of date within the day; a later state note gives the new counts.

### Root Cause

The OQ-108 label was carried forward from the 2026-10-09 ADR-004 text without checking the entry.

### Solution

Each OQ's question text was read before closing it.

### Current Status

- PASS (documentation consistency). Later 2026-10-12 decisions uncommitted, pending owner approval.
- Product work: BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Owner: approve the commit below.
2. Owner: OQ-131 — for 50 Hz sources, should the live encode run at 25 or 30 fps? And for non-1080 inputs, scale to 1080p or keep the source resolution?
3. Owner: review ADR-006 and ADR-008 (their CM5-based reasons no longer apply).
4. Owner: obtain a CM4 + IO Board + 12 V supply and run TEST-ENC-001 Stage 0 — the product runs are R4 and R8.
5. Proposed commit (Rule 15):

```text
Commit title: docs: decide 1080p30 live encode, close CM5-only questions
Commit description: Record the owner decisions of 2026-10-12: the live encode (shared by RTMP
  and WebRTC) is 1080p30 (OQ-005), with new OQ-131 for converting other input modes; and the
  CM5-only open questions (OQ-049 to OQ-055, OQ-059, OQ-060, OQ-124) are ANSWERED as made
  irrelevant by ADR-004, per the register's rule (OQ-108, an H.265 question, stays OPEN).
  Add TEST-ENC-001 Stage 0 run R8 (1080p50 recording + 1080p30 live) and mark R4/R8 as the
  product runs. Refine the doc check's stale-status warning to the same clause. Record the
  reword of 3c823b6 to 27c44d9 and the push of 049d1ee..1f5c0e0.
Files changed: docs/** (15 Markdown files), tools/doccheck.py, tools/encoder_capacity_test.sh
Reason: Rules 1, 3, 11, 12, 13, 15, 21, 22
Tests: python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0; W3 regression file;
  Rule 21 audit; table check; bash -n and stand-in dry run of R8
```

---

## 2026-10-12 (third entry) — Commit and push of the second set; 50 Hz sources go out at 25 fps (OQ-131)

### Objective

Act on the owner's answers: "Commit and push" the second 2026-10-12 set; OQ-131 — "25 fps" for 50 Hz-family sources.

### Starting State

`main` at `1f5c0e0` (= `origin/main`); the second-entry changes uncommitted.

### Changes

1. Committed the second-entry changes with the proposed message as **`c9d7e19`**; `git fetch` confirmed a fast-forward; pushed `1f5c0e0..c9d7e19`.
2. **OQ-131 owner input:** 50 Hz-family sources → live encode at **25 fps** (1080p50 halved, 1080p25 passed through; no non-integer 50 → 30 conversion). Reasoning from [L-14], [D-10]: with a 1080p50 recording the encoder carries 612,000 macroblocks/s, 2.5× the 1080p30 specification; at 25 fps the frame period is 40 ms, so the frame-period terms of the [K-45] budget grow (reasoning). OQ-131 stays OPEN for the 60 Hz method (dropping every other frame is the obvious choice, not yet decided or tested) and non-1080 inputs.
3. Propagated to the 2026-10-12 owner-decision boxes (ARCHITECTURE, SOFTWARE_ARCHITECTURE, STREAMING), REQ-ENC-001, VIDEO_ENCODER §7.1, RISK-002, PERFORMANCE, TRACEABILITY.
4. TEST-ENC-001 Stage 0 and `tools/encoder_capacity_test.sh`: run **R9** (1080p50 recording + 1080p25 live, 2.5×) added; the product runs are **R4** (60 Hz family), **R9** (50 Hz family) and **R3** (1080p30 sources); R8 becomes a margin reference.

### Files Modified

`docs/`: `OPEN_QUESTIONS.md`, `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `REQUIREMENTS.md`, `VIDEO_ENCODER.md`, `RISKS.md`, `PERFORMANCE.md`, `TESTING.md`, `TRACEABILITY.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`; `tools/encoder_capacity_test.sh`.

### Hardware Changes

None.

### Software Changes

Test script: run R9.

### Commands Used

```bash
git commit -F <message> && git fetch origin && git push origin main   # c9d7e19
python3 tools/doccheck.py docs
bash -n tools/encoder_capacity_test.sh   # plus stand-in dry run of R9
```

### Test Results

- Documentation consistency check — **TESTED — PASS** (documentation only): PROBLEMS 0, WARNINGS 0; 620/620 facts cited; 131 OQs.
- Tables intact; insertions only (no removed text besides "Last updated"-type fields — none this round).
- Script: syntax passes; stand-in dry run of R9 sets recording Level 4.2 and live Level 4. **NOT YET RUN ON PACSCORDER HARDWARE.**

### Problems Found

None new.

### Root Cause

—

### Solution

—

### Current Status

- PASS (documentation consistency). The 25 fps decision is uncommitted, pending owner approval.
- Product work: BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Owner: approve the commit below.
2. Owner: OQ-131 remainder — non-1080 inputs (scale to 1080p, or keep the source resolution?); ADR-006 and ADR-008 review.
3. Owner: obtain a CM4 + IO Board + 12 V supply and run TEST-ENC-001 Stage 0 (product runs R4, R9, R3).
4. Proposed commit (Rule 15):

```text
Commit title: docs: live encode at 25 fps for 50 Hz sources (OQ-131)
Commit description: Record the owner decision of 2026-10-12 that 50 Hz-family sources go out
  at 25 fps on the live encode (1080p50 halved, 1080p25 passed through); OQ-131 stays OPEN
  for the 60 Hz method and non-1080 inputs. Add TEST-ENC-001 Stage 0 run R9 (1080p50
  recording + 1080p25 live, 2.5x the 1080p30 specification); product runs are R4, R9, R3.
  Record the commit and push of c9d7e19.
Files changed: docs/** (13 Markdown files), tools/encoder_capacity_test.sh
Reason: Rules 1, 3, 11, 12, 15, 21, 22
Tests: python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0; table check; bash -n and
  stand-in dry run of R9
```

---

## 2026-10-12 (fourth entry) — Commit and push of the 25 fps decision; non-1080 sources keep their resolution (OQ-131)

### Objective

Act on the owner's answers: "Commit and push" the third-entry changes; OQ-131 — "Keep source resolution" for non-1080 inputs.

### Starting State

`main` at `c9d7e19` (= `origin/main`); the third-entry changes uncommitted.

### Changes

1. Committed the third-entry changes with the proposed message as **`91ff869`**; fast-forward push `c9d7e19..91ff869`.
2. **OQ-131 owner input (second):** non-1080 inputs keep their resolution on the live encode (no scaling step). Recorded as Claude's reading, **to be confirmed by the owner**: the frame-rate rule (60 Hz → 30 fps, 50 Hz → 25 fps) also applies, so a 720p60 source goes out as 720p30. *(Confirmed by the owner later on 2026-10-12 ("okay"): 720p sources follow the 30/25 fps rule — 720p60 goes out as 720p30, 720p50 as 720p25.)*
3. **Consequence found (reasoning from [L-01], [L-02], [L-06]):** 720p30 fits Level 3.1 by macroblock rate, but Level 3.1 caps Constrained Baseline at 14 Mbit/s — below the 17 Mbit/s live bitrate — so a 720p live encode must signal at least Level 3.2 (20 Mbit/s, 85 %). Level 3.1 is libwebrtc's default `42e01f` [F-39], so the browser test (OQ-073) must cover 720p at Level 3.2. Whether 17 Mbit/s applies unchanged at 720p was never decided (OQ-005: no per-mode values).
4. Propagated to OQ-073, the 2026-10-12 owner-decision boxes (ARCHITECTURE, SOFTWARE_ARCHITECTURE, STREAMING), REQ-ENC-001, VIDEO_ENCODER §6, TRACEABILITY. OQ-131 stays OPEN only for the 60 Hz method and its cost. No new Stage 0 run: 720p sessions are lighter than the runs already in the matrix (R6, R7 cover a 720p live encode beside a 1080p60 recording).

### Files Modified

`docs/`: `OPEN_QUESTIONS.md`, `ARCHITECTURE.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `REQUIREMENTS.md`, `VIDEO_ENCODER.md`, `TRACEABILITY.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

None.

### Commands Used

```bash
git commit -F <message> && git fetch origin && git push origin main   # 91ff869
python3 tools/doccheck.py docs
```

### Test Results

- Documentation consistency check — **TESTED — PASS** (documentation only): PROBLEMS 0, WARNINGS 0.
- Insertions only; tables intact.

### Problems Found

None new. One point needs the owner's confirmation (Changes 2).

### Root Cause

—

### Solution

—

### Current Status

- PASS (documentation consistency). The non-1080 decision is uncommitted, pending owner approval. *(Committed and pushed as `b8091fd` on the owner's "okay", `91ff869..b8091fd`.)*
- Product work: BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Owner: approve the commit below, and confirm that 720p sources follow the 30/25 fps rule. *(Confirmed by the owner later on 2026-10-12 ("okay"): 720p sources follow the 30/25 fps rule — 720p60 goes out as 720p30, 720p50 as 720p25.)*
2. Owner: review ADR-006 and ADR-008.
3. Owner: obtain a CM4 + IO Board + 12 V supply and run TEST-ENC-001 Stage 0 (product runs R4, R9, R3).
4. Proposed commit (Rule 15):

```text
Commit title: docs: live encode keeps non-1080 source resolution (OQ-131)
Commit description: Record the owner decision of 2026-10-12 that non-1080 inputs keep their
  resolution on the live encode (no scaling). Note that a 720p live encode at 17 Mbit/s must
  signal at least Level 3.2, because Level 3.1 caps Constrained Baseline at 14 Mbit/s
  (reasoning from ITU-T H.264 Table A-1, [L-01], [L-02], [L-06]); OQ-073's browser test must
  cover it. OQ-131 stays OPEN for the 60 Hz method. Record the commit and push of 91ff869.
Files changed: docs/** (10 Markdown files)
Reason: Rules 1, 3, 11, 12, 15, 21, 22
Tests: python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0; table check
```

---

## 2026-10-12 (fifth entry) — ADR-010 and ADR-008 recommendations; research topic M (bridge boards); ADR-011 and RISK-035

### Objective

The owner said "continue" without choosing among the three desk tasks offered. Claude chose by impact: research the TC358743 bridge boards (OQ-018 to OQ-021 block the Device Tree and every capture test), with the OQ-131 frame-rate conversion folded in, and write the CM4-only recommendations for ADR-006 and ADR-008 while the research ran.

### Starting State

`main` at `b8091fd` (= `origin/main`). The session was interrupted by a Claude Code restart while the topic M research agent was running; it had written no output, and was resumed from its saved transcript. Git was clean apart from this session's uncommitted ADR-010 work; no hand commit had been made.

### Changes

1. **ADR-010 (PROPOSED):** capture control model on the CM4 — legacy (video-node) mode, the stock overlay default [B-43], [C-10] — as the CM4-only alternative to ADR-006 (owner decision OQ-014). Reasons: officially documented path [C-37]; no extra overlay parameter; avoids the Media Controller `ENUM_FMT` question (OQ-046); the cross-platform reason for ADR-006 disappeared with ADR-004. Cross-referenced in ADR-006, OQ-014, V4L2.md and the TRACEABILITY design cells of REQ-CAP-001 to REQ-CAP-004.
2. **ADR-008:** recommendation to accept as written — its decision does not depend on the CM5 reason [C-48], [C-49]; the 3-of-4-lane concern stays the thing to test [C-43].
3. **Research topic M** (1 researcher, resumed once; 2 independent verifiers): 60 facts, 47 CONFIRMED, 13 CORRECTED, 0 UNVERIFIABLE, 0 REFUTED; register 680 facts. Verifiers added the Geekworm C792, corrected the X1301 RESET/INT routing claim (not shown either way), the B102 cable-side conflict, the forum dates and the `videorate` buffer details. The generator was fixed to flatten line breaks in verified statements (M-58 contained a list) and re-run on a clean copy of REFERENCES.md.
4. **Propagation:** OQ-018, OQ-019, OQ-020, OQ-021, OQ-024, OQ-025, OQ-131 (dated "Known so far" bullets); RISK-004 (supply snapshot), RISK-021 (FFC sidedness, shared CAM_GPIO); HARDWARE §4.4, §4.6.1, §4.7, §4.8, §4.10 (candidate facts, PACSCORDER values still UNKNOWN — Rule 8), §6 candidate rows, §7 hazards 18–21; DEVICE_TREE §5.3, §5.4, §5.8, §6.2; SOFTWARE_ARCHITECTURE (OQ-131 design input); TESTING setup notes in TEST-HW-001, TEST-DRV-002, TEST-CAP-002, TEST-AUD-001; TROUBLESHOOTING §7, §8; README fact range `M-NN`.
5. **RISK-035 (High):** HDMI audio may not work on non-Auvidea bridge boards [M-45], [M-46]; retired only by TEST-AUD-001 on the candidate board.
6. **ADR-011 (PROPOSED):** bring-up boards — 2 × Geekworm C790 (CAM1 4-lane, CAM0 2-lane), 1 × Geekworm X1301 (second design), an Auvidea B102 if obtainable (audio reference); HW REV A chosen only after TEST-HW-001, TEST-DRV-002, TEST-CAP-002 and TEST-AUD-001 pass. About USD 98 at the 2026-10-12 snapshot.
7. Bookkeeping: commit `b8091fd` recorded in PROJECT_STATUS and this log.

### Files Modified

`docs/`: `REFERENCES.md`, `research/2026-10-12-bridge-boards-research.json` (new), `DECISIONS.md`, `RISKS.md`, `OPEN_QUESTIONS.md`, `HARDWARE.md`, `DEVICE_TREE.md`, `SOFTWARE_ARCHITECTURE.md`, `TESTING.md`, `TROUBLESHOOTING.md`, `V4L2.md`, `TRACEABILITY.md`, `README.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None. No hardware exists; nothing has been bought.

### Software Changes

None.

### Commands Used

```bash
python3 <scratchpad>/gen_topic_M.py        # register append (re-run after git checkout -- docs/REFERENCES.md)
python3 tools/doccheck.py docs
```

### Test Results

- Documentation consistency check — **TESTED — PASS** (documentation only): `facts=680 topics=ABCDEFGHIJKLM … ADR=11 RISK=35 OQ=131`, `distinct facts cited=680/680`, PROBLEMS 0, WARNINGS 0.
- Register format: no stray lines in topics L and M (every entry field on one line). Tables intact in every edited file.
- No hardware test possible.

### Problems Found

- The first topic M register append wrote M-58's multi-line verified statement as separate lines; caught by a format check before commit, fixed by flattening and regenerating.
- Vendor documents disagree with each other or with themselves: B102 cable contact side [M-43], B102 audio connector pin count [M-06], [M-07], X1301 GPIO20 header pin [M-23]; vendor mode claims for 4-lane boards differ [M-05], [M-12], [M-31].

### Root Cause

Generator assumed single-line statements.

### Solution

`flat()` in the generator; documented here.

### Current Status

- PASS (documentation consistency). Uncommitted, pending owner approval.
- Product work: BLOCKED — HARDWARE REQUIRED.

### Next Step

1. Owner: approve the commit below.
2. Owner: decide ADR-011 (what to buy), ADR-010 vs ADR-006, and ADR-008.
3. Owner: buy the CM4 + IO Board + 12 V supply (Stage 0 needs only these) and the ADR-011 boards; record them as HW REV A candidates in HARDWARE.md.
4. Proposed commit (Rule 15):

```text
Commit title: docs: research bridge boards (topic M), propose bring-up boards and CM4 control model
Commit description: Add research topic M (60 verified facts: TC358743 bridge boards for the CM4 IO
  Board, its camera connectors and cables, videorate frame-rate conversion) and propagate it to
  OQ-018 to OQ-021, OQ-024, OQ-025, OQ-131, HARDWARE, DEVICE_TREE, SOFTWARE_ARCHITECTURE,
  TESTING and TROUBLESHOOTING. Propose ADR-011 (bring-up boards: 2x Geekworm C790, 1x X1301,
  Auvidea B102 if obtainable; HW REV A chosen only after bench tests) and ADR-010 (legacy
  video-node control on the CM4, alternative to ADR-006); recommend accepting ADR-008. Add
  RISK-035 (HDMI audio on non-Auvidea boards). Record commit b8091fd.
Files changed: docs/** (16 files incl. docs/research/2026-10-12-bridge-boards-research.json)
Reason: Rules 1, 3, 8, 11, 13, 15, 21, 22, 23
Tests: python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0 (680/680 facts cited);
  register format and table checks
```

---

## 2026-10-12 (sixth entry) — Commit and push of topic M; ADR-010, ADR-008 and ADR-011 ACCEPTED

### Objective

Act on the owner's answers: commit and push the fifth-entry round; "Accept ADR-011"; "ADR-010 legacy mode"; "Accept ADR-008".

### Starting State

`main` at `b8091fd` (= `origin/main`); the fifth-entry changes uncommitted.

### Changes

1. Committed the fifth-entry round as **`7d7d2de`** and pushed `b8091fd..7d7d2de` (fast-forward after `git fetch`).
2. **ADR-008 ACCEPTED** (OQ-099 ANSWERED), **ADR-010 ACCEPTED** with **ADR-006 SUPERSEDED BY ADR-010** (OQ-014 ANSWERED), **ADR-011 ACCEPTED** (OQ-018 owner input; stays OPEN for HW REV A). Status notes appended to each ADR's Status section; index status cells; header 7 ACCEPTED, 2 PROPOSED, 1 OPEN, 1 SUPERSEDED. OQ counts 131: 106 OPEN, 25 ANSWERED.
3. **`tools/doccheck.py` W3** now treats SUPERSEDED ADRs as settled, so a superseded ADR still described as PROPOSED is flagged. The check then listed 78 lines (ADR-006 ~30, ADR-008 ~40, ADR-010/011 8); 77 were annotated by a script from the check's own output with dated "Superseded 2026-10-12 — …" notes (plain bracketed text inside code blocks). Two of those rows were indented list tables, where the note first landed outside the last cell; the table check caught both and the notes were moved inside the cell.
4. **Content notes for ADR-010:** Setup notes in TEST-DRV-001, TEST-DRV-002, TEST-CAP-001, TEST-CAP-002, TEST-CAP-003 and TEST-PERF-001, and section notes in V4L2 §6, TC358743_DRIVER §5, SOFTWARE_ARCHITECTURE §1 and TROUBLESHOOTING §2.5: EDID, DV timings and format go through the Unicam video node [B-25], [C-36], [C-37]; the Media Controller text is kept as the ADR-006 record.
5. Change-history rows in every touched file; TRACEABILITY state note; PROJECT_STATUS and CHANGELOG.

### Files Modified

`docs/`: `DECISIONS.md`, `OPEN_QUESTIONS.md`, `ARCHITECTURE.md`, `ATEM.md`, `BUILD_SYSTEM.md`, `CSI_PIPELINE.md`, `DEVICE_TREE.md`, `HARDWARE.md`, `PERFORMANCE.md`, `REQUIREMENTS.md`, `RISKS.md`, `SOFTWARE_ARCHITECTURE.md`, `TC358743_DRIVER.md`, `TESTING.md`, `TRACEABILITY.md`, `TROUBLESHOOTING.md`, `V4L2.md`, `VIDEO_ENCODER.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`; `tools/doccheck.py`.

### Hardware Changes

None yet; ADR-011 authorises the bring-up purchase.

### Software Changes

`tools/doccheck.py` (W3 covers SUPERSEDED ADRs).

### Commands Used

```bash
git commit -F <message> && git fetch origin && git push origin main   # 7d7d2de
python3 tools/doccheck.py docs                                        # before/after annotation
```

### Test Results

- Documentation consistency check — **TESTED — PASS** (documentation only): `ADR=11 … OQ=131`, 680/680 facts cited, PROBLEMS 0, WARNINGS 0.
- Table check: two broken rows found after annotation and corrected; all tables intact.
- No hardware test possible.

### Problems Found

The annotation script did not recognise table rows indented inside a list (leading spaces); caught by the table check.

### Root Cause

`startswith('|')` instead of `lstrip().startswith('|')`.

### Solution

Notes moved inside the last cell; recorded here.

### Current Status

- PASS (documentation consistency). The decisions are uncommitted, pending owner approval.
- Product work: BLOCKED — HARDWARE REQUIRED; the bring-up purchase is now authorised (ADR-011).

### Next Step

1. Owner: approve the commit below.
2. Owner: buy the CM4, CM4 IO Board, 12 V supply, the ADR-011 boards and the recording storage; then TEST-ENC-001 Stage 0 and TEST-HW-001.
3. Proposed commit (Rule 15):

```text
Commit title: docs: accept ADR-010, ADR-008 and ADR-011 (CM4 control model, link frequency, bring-up boards)
Commit description: Record the owner decisions of 2026-10-12: ADR-010 ACCEPTED (legacy video-node
  control on the CM4) with ADR-006 SUPERSEDED BY ADR-010 (OQ-014 ANSWERED); ADR-008 ACCEPTED
  (486 MHz; OQ-099 ANSWERED); ADR-011 ACCEPTED (bring-up boards; OQ-018 stays OPEN for HW REV A).
  Annotate the 77 stale status lines and point the test procedures at the video node. Extend the
  doc check's stale-status warning to SUPERSEDED ADRs. Record commit 7d7d2de.
Files changed: docs/** (21 Markdown files), tools/doccheck.py
Reason: Rules 1, 3, 10, 13, 15, 21
Tests: python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0; table check
```

---

## 2026-10-12 (seventh entry) — Commit and push of the ADR decisions; ADR-007 (GStreamer) and ADR-012 (Python 3) proposed

### Objective

The owner asked when coding starts. Claude answered that the application waits on ADR-007 (framework, OPEN), offered to write it up, and the owner said "write".

### Starting State

`main` at `7d7d2de`; the sixth-entry decisions were then committed and pushed as **`78b6d70`** (`7d7d2de..78b6d70`) on the owner's "Commit and push".

### Changes

1. **ADR-007:** a dated "Proposal (2026-10-12)" section appended — **GStreamer**; status OPEN → PROPOSED (status note appended; index cell). It explains why the decision need not wait for bring-up (those tests measure hardware both frameworks use alike; the platform question was settled by ADR-004), gives seven sourced reasons ([L-43] FFmpeg's CM4 encoder wrapper cannot set bitrate mode or level; [M-56], [M-57] fan-out; [M-53], [M-54], [M-60] frame-rate halving; [J-39], [J-41], [J-44] fragmented 30-minute recording, with the note that fragmented mode disables robust muxing; [K-05], [F-45], [F-33], [K-31] publishing; [I-44], [I-45], [I-46] audio; [D-37] official example), a pipeline sketch, the known gaps ([K-32], OQ-118, OQ-112, OQ-131, the `rtspclientsink` package) and the alternatives. Evidence tables unchanged.
2. **ADR-012 (PROPOSED):** application language Python 3 with PyGObject; C and Rust as alternatives; shell for bring-up scripts; proposed repository layout (`app/`, `tests/`, `scripts/`, `image/`, `tools/`); development on a Linux machine with `videotestsrc` and a software encoder before the hardware arrives. No register fact covers the bindings — BUILD TEST REQUIRED.
3. **OQ-132** (application language) added; OQ-015 note. DECISIONS header 7 ACCEPTED, 4 PROPOSED, 0 OPEN, 1 SUPERSEDED; OQ header 132 (107 OPEN, 25 ANSWERED).
4. Notes in SOFTWARE_ARCHITECTURE and TRACEABILITY; PROJECT_STATUS and CHANGELOG; commit `78b6d70` recorded.
5. Not annotated on purpose: lines elsewhere that call ADR-007 "OPEN" — it is still undecided; they will be annotated when the owner accepts or rejects the proposal (the doc check flags them then).
6. **Owner decisions, the same exchange:** "Accept GStreamer" — **ADR-007 ACCEPTED** (OQ-015 ANSWERED); language **"C"** — not the proposed Python, so **ADR-012 SUPERSEDED BY ADR-013** and **ADR-013 (C, GStreamer native API) added as ACCEPTED** (OQ-132 ANSWERED), keeping the record that Python was proposed and C chosen (Rule 13). DECISIONS header 9 ACCEPTED, 2 PROPOSED, 0 OPEN, 2 SUPERSEDED; OQ header 132 (105 OPEN, 27 ANSWERED). The doc check then listed 65 stale lines (ADR-007 ×64, ADR-012 ×1); all annotated by script, with change-history rows in the 13 files touched. Owner: "Commit, push, start coding".

### Files Modified

`docs/`: `DECISIONS.md`, `OPEN_QUESTIONS.md`, `SOFTWARE_ARCHITECTURE.md`, `TRACEABILITY.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

None (no code yet).

### Commands Used

```bash
python3 tools/doccheck.py docs
```

### Test Results

- Documentation consistency check — **TESTED — PASS** (documentation only): `ADR=12 … OQ=132`, 680/680 facts cited, PROBLEMS 0, WARNINGS 0.

### Problems Found

None.

### Root Cause

—

### Solution

—

### Current Status

- PASS (documentation consistency). Committed and pushed on the owner's "Commit, push, start coding" (hash recorded in the next entry).

### Next Step

1. Owner: accept or change ADR-007 (GStreamer) and ADR-012 (Python 3 + PyGObject).
2. If accepted: create the repository layout and start the bring-up scripts and the application skeleton (IMPLEMENTED — NOT TESTED until a CM4 exists).
3. Owner: buy the bring-up hardware (ADR-011).
4. Proposed commit (Rule 15):

```text
Commit title: docs: accept GStreamer (ADR-007) and C (ADR-013) for the application
Commit description: Add the ADR-007 proposal for GStreamer (FFmpeg's CM4 encoder wrapper cannot set
  bitrate mode or level; documented fan-out, frame-rate halving, fragmented 30-minute recording and
  RTSP publishing to MediaMTX) and record its acceptance (OQ-015 ANSWERED). Record Claude's Python
  proposal as ADR-012 and the owner's choice of C as ADR-013 (ACCEPTED; ADR-012 SUPERSEDED;
  OQ-132 added and ANSWERED). Annotate the 65 stale ADR-007 status lines. Record commit 78b6d70.
Files changed: docs/** (17 Markdown files)
Reason: Rules 1, 3, 13, 15, 22, 23
Tests: python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0
```

---

## 2026-10-12 (eighth entry) — Coding started: application slice C1 (C, GStreamer, CMake), development tests, bring-up script

### Objective

The owner chose GStreamer (ADR-007) and C (ADR-013) and said "Commit, push, start coding". Write the first application slice, build it, test it on the development machine, and document it (Rules 16, 17, 24).

### Starting State

- The seventh-entry decisions were committed and pushed as **`27bb305`** (`78b6d70..27bb305`).
- No code existed. This Mac had Apple clang, CMake, ninja, pkg-config and Homebrew, but no GStreamer, Meson or container runtime. The owner chose **CMake** for the build (ADR-014; Claude had suggested Meson) and allowed `brew install gstreamer`.
- The first install attempt failed because the Xcode licence was not accepted (`brew` and `cc` both refused); the owner ran `sudo xcodebuild -license accept`, then GStreamer **1.28.7** was installed.

### Changes

1. **ADR-014 (ACCEPTED):** CMake + pkg-config + CTest.
2. **Application slice C1** (`app/src/`): `main.c` (init, options, Ctrl-C/SIGTERM → end of stream), `config.c/.h` (options; owner decisions as defaults; live rate = capture rate halved above 30 fps, OQ-131), `pipeline.c/.h` (ADR-007 pipeline: capture → tee → recording `encoder → h264parse → tee → queue → splitmuxsink (fragmented mp4mux)` per directory, and live `videorate drop-only → encoder → h264parse → fakesink`; per-branch frame counters; file open/close log), `h264_level.c/.h` (lowest H.264 level from ITU-T Table A-1 [L-01] to [L-07]). Backends: `cm4` (`v4l2src`, `v4l2h264enc` with caps profile/level and `extra-controls` [L-61] to [L-63]) and `dev` (`videotestsrc`, `x264enc`).
3. **Development tests** (`tests/`): `h264_level` (11 cases) and `smoke_record_dev` (12 s run, 4 s files, two mirrored directories; checks clean end of stream, file count, byte-identical mirrors, fragmented MP4, frame counts, profile and start time).
4. **Bring-up script** `scripts/bringup/tc358743.sh`: EDID (built-in `hdmi` type or a file [B-24]), `--set-dv-bt-timings query` [C-37], format, capture with fps readout [L-46], [L-48]; logs each run.
5. **Fixes found by testing on the development machine:** (a) `x264enc` with `ultrafast` produced Constrained Baseline despite `profile=high` caps — the `dev` recording encoder now uses `superfast`; (b) the option table's terminator warning — `G_OPTION_ENTRY_NULL`; (c) the smoke test's first version required the first sample at exactly 0 — a run started at 0.000167 s, so it now allows one frame period. A suspected problem — `gst-discoverer-1.0` showing cumulative durations — turned out to be the tool: `ffprobe` shows each file starting at 0 with its own length.
6. **Documentation:** SOFTWARE_ARCHITECTURE §10 (implementation), BUILD_SYSTEM §2A (application build), TESTING "Development tests" and bring-up-script pointers in TEST-DRV-002 and TEST-CAP-001; REQUIREMENTS, TRACEABILITY and ARCHITECTURE implementation statuses (REQ-ARCH-001, REQ-ENC-001, REQ-REC-001: IMPLEMENTED — NOT TESTED, partial); docs/README source-code index; root README layout and build steps; PROJECT_STATUS; CHANGELOG.

### Files Modified

- New: `CMakeLists.txt`, `app/CMakeLists.txt`, `app/src/{main.c,config.c,config.h,pipeline.c,pipeline.h,h264_level.c,h264_level.h}`, `tests/{CMakeLists.txt,test_h264_level.c,smoke_record.sh}`, `scripts/bringup/tc358743.sh`.
- Changed: `README.md`; `docs/`: `DECISIONS.md`, `SOFTWARE_ARCHITECTURE.md`, `BUILD_SYSTEM.md`, `TESTING.md`, `REQUIREMENTS.md`, `TRACEABILITY.md`, `ARCHITECTURE.md`, `README.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None. Development machine only (macOS 26.6.2 arm64).

### Software Changes

First application code (slice C1); development tests; bring-up script. Homebrew GStreamer 1.28.7 installed on the development machine (owner's permission).

### Commands Used

```bash
brew install gstreamer                    # after: sudo xcodebuild -license accept (owner)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
bash tests/smoke_record.sh build/app/pacscorder      # 5 runs
./build/app/pacscorder --width 1280 --height 720 --fps 60 --split-seconds 3 --rec-dir <dir>   # Ctrl-C after 5 s
ffprobe -v error -select_streams v:0 -show_entries stream=profile,start_time,duration <file>
python3 tools/doccheck.py docs
```

### Test Results

- Build: no warnings with `-Wall -Wextra` after the fixes above.
- `h264_level`: **TESTED — PASS** (11/11), development machine.
- `smoke_record_dev`: **TESTED — PASS**, 5 of 5 runs, development machine (e.g. 755 frames captured, 755 recorded, 378 live; 3–4 files per run; mirrors byte-identical; High profile; fragmented).
- By hand: Ctrl-C stop completes the open file (3.0 s + 2.0 s, High); bad options exit 2 with a message; `--backend cm4` exits 1 ("'v4l2src' is not available").
- **None of this is a test of PACSCORDER hardware.** The `cm4` backend, the bring-up script and every canonical test remain BLOCKED — HARDWARE REQUIRED. The development machine runs GStreamer 1.28.7, not the CM4's 1.26.2.
- Documentation consistency check — TESTED — PASS (PROBLEMS 0, WARNINGS 0).

### Problems Found

See Changes 5. Also: the writer queues use GStreamer's default blocking limits, so an HDD stall would still back-pressure the recording (OQ-117, RISK-028) — documented as open work, not hidden.

### Root Cause

(a) x264's `ultrafast` preset disables High-profile features and x264 signals the profile it actually used; (c) live-source start offsets of a fraction of a frame.

### Solution

As in Changes 5.

### Current Status

- PARTIAL: slice C1 IMPLEMENTED — NOT TESTED on hardware; development tests PASS on the development machine.
- Uncommitted, pending owner approval.

### Next Step

1. Owner: approve the commit below.
2. Slices C2 (RTSP to MediaMTX, RTMP), C3 (EDID/DV timings and source changes in the application), C4 (audio), C5 (drive failure/return, HDD decoupling) — each with development tests.
3. Owner: buy the bring-up hardware (ADR-011); then TEST-ENC-001 Stage 0, the bring-up script, and the first `--backend cm4` run.
4. Proposed commit (Rule 15):

```text
Commit title: feat(app): add recorder slice C1 (GStreamer, C, CMake) with development tests
Commit description: Add the PACSCORDER recorder in C on GStreamer (ADR-007, ADR-013) with a CMake
  build (ADR-014): capture, a recording encode written as fragmented MP4 in 30-minute files
  mirrored to two directories, and a live encode at half the capture rate above 30 fps; cm4
  (v4l2src, v4l2h264enc with the owner's settings) and dev (videotestsrc, x264enc) backends. Add
  development tests (H.264 level selection; 12 s smoke run) and a CM4 bring-up script. Document the
  implementation, build and tests; mark REQ-ARCH-001, REQ-ENC-001, REQ-REC-001 IMPLEMENTED — NOT
  TESTED (partial). Development machine only; nothing has run on PACSCORDER hardware.
Files changed: CMakeLists.txt, app/**, tests/**, scripts/bringup/tc358743.sh, README.md,
  docs/** (12 Markdown files)
Reason: Rules 1, 3, 10, 15, 16, 17, 24
Tests: ctest — h264_level PASS, smoke_record_dev PASS (5/5 runs) on macOS + GStreamer 1.28.7;
  python3 tools/doccheck.py docs -> PROBLEMS 0, WARNINGS 0
```

---

## 2026-10-12 (ninth entry) — Slice C2: live publishing (RTSP to MediaMTX, RTMP), fault-isolated from the recording

### Objective

The owner approved slice C1 ("Commit and push") and chose slice C2 next: publish the live encode to MediaMTX (for the LAN WebRTC viewers) and over RTMP, tested on the development machine.

### Starting State

Slice C1 committed and pushed as **`9ab0b4e`** (`27bb305..9ab0b4e`). The live branch ended in a `fakesink`. `rtspclientsink`, `rtmp2sink` and `flvmux` were present in the development machine's GStreamer 1.28.7; MediaMTX was not installed.

### Changes

1. **MediaMTX 1.21.0** installed with Homebrew on the development machine (owner's permission), used only by the test.
2. **Design (ADR-015, PROPOSED):** each publisher is its own pipeline fed from an `appsink` at the end of the live branch, so a publisher error cannot end the main pipeline and stop the recording; failed publishers restart after 5 s; publishers share the main pipeline's clock and base time; a keyframe is requested on (re)start [K-31]. ADR-007's sketch had the publishers in the main pipeline.
3. **Code:** `app/src/publisher.c/.h` (new); `pipeline.c` (appsink, running-time conversion, publisher start on PLAYING, restart counts at exit); `config.c/.h` (`--rtsp-url`, `--rtmp-url` up to 4); CMake: `gstreamer-app-1.0`, `gstreamer-video-1.0`.
4. **Faults found by testing and fixed:** (a) RTMP: MediaMTX closed the connection and no stream appeared; `GST_DEBUG=flvmux:5` showed timestamps at 1000 h — the encoder's offset, described in its segment, which the publisher's `appsrc` (segment from 0) did not know, so `rtmp2sink` waited. Fixed by converting each sample to running time before pushing. A hand-run `gst-launch-1.0` RTMP publish to the same MediaMTX had worked, which narrowed it to the application. (b) GStreamer's "Pipeline construction is invalid, please add queues" warning — a `queue` added before the sink side of each publisher.
5. **Test:** `tests/smoke_publish.sh` (CTest `smoke_publish_dev`, skipped with exit 77 if MediaMTX or `ffprobe` is missing). MediaMTX writes a generated TLS certificate and **private key** (`auto.crt`, `auto.key`) into its working directory; the first test runs, started from the repository root, left them there. They were deleted before any commit (never in git history), and the test now starts MediaMTX inside its temporary directory.
6. **Documentation:** ADR-015; SOFTWARE_ARCHITECTURE §10.1; BUILD_SYSTEM §2A; TESTING "Development tests"; STREAMING box; REQUIREMENTS, TRACEABILITY and ARCHITECTURE (REQ-STR-001, REQ-STR-002: IMPLEMENTED — NOT TESTED, partial); PROJECT_STATUS; CHANGELOG.

### Files Modified

- New: `app/src/publisher.c`, `app/src/publisher.h`, `tests/smoke_publish.sh`.
- Changed: `CMakeLists.txt`, `app/CMakeLists.txt`, `app/src/{config.c,config.h,pipeline.c}`, `tests/CMakeLists.txt`; `docs/`: `DECISIONS.md`, `SOFTWARE_ARCHITECTURE.md`, `BUILD_SYSTEM.md`, `TESTING.md`, `STREAMING.md`, `REQUIREMENTS.md`, `TRACEABILITY.md`, `ARCHITECTURE.md`, `PROJECT_STATUS.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None.

### Software Changes

Slice C2; MediaMTX on the development machine.

### Commands Used

```bash
brew install mediamtx
cmake --build build && ctest --test-dir build --output-on-failure
bash tests/smoke_publish.sh build/app/pacscorder        # 3 runs
GST_DEBUG=flvmux:5,rtmp2sink:4 ./build/app/pacscorder … --rtmp-url rtmp://127.0.0.1:11935/rtmplive
gst-launch-1.0 videotestsrc … ! x264enc … ! h264parse ! flvmux streamable=true ! rtmp2sink location=…   # reference
ffprobe -rtsp_transport tcp … rtsp://127.0.0.1:18554/<path>
```

### Test Results

- `smoke_publish_dev`: **TESTED — PASS**, 3 of 3 runs plus CTest (development machine): RTSP and RTMP both `h264, Constrained Baseline, 1280x720, 30/1` read back through MediaMTX; with the RTSP target down, the recording completed (3 files) and the publisher retried.
- By hand: MediaMTX started 6 s after the recorder — the RTSP publisher failed twice, connected on the third attempt, the stream became readable; recording unbroken (1364 frames captured and recorded, 682 live).
- `h264_level`, `smoke_record_dev`: still PASS.
- **Not a hardware test.** The WebRTC/WHEP side was not exercised (needs a browser; TEST-STR-002 on hardware). Documentation check: PASS.

### Problems Found

See Changes 4. Open: RTMP is video-only until audio (C4); whether destinations accept that is not in the register (OQ-007).

### Root Cause

(a) The encoder's segment offset was not carried across the appsink/appsrc boundary.

### Solution

Running-time conversion in `pipeline.c` (`to_running_time`).

### Current Status

- PARTIAL: slice C2 IMPLEMENTED — NOT TESTED on hardware; development tests PASS.
- Uncommitted, pending owner approval.

### Next Step

1. Owner: approve the commit below; confirm ADR-015.
2. Next slices: C3 (EDID / DV timings / source changes in the application), C4 (audio), C5 (drive failure/return, HDD decoupling).
3. Proposed commit (Rule 15):

```text
Commit title: feat(stream): publish the live encode over RTSP and RTMP in isolated pipelines (slice C2)
Commit description: Add live publishing: RTSP to MediaMTX (which serves the WebRTC viewers) and RTMP,
  each in its own pipeline fed from the live encode so a network failure cannot stop the recording
  (ADR-015, PROPOSED); failed publishers restart after 5 s and request a keyframe. Convert samples to
  running time for the publishers (fixes RTMP sending nothing). Add the smoke_publish_dev test with a
  local MediaMTX. Document C2; REQ-STR-001/002 IMPLEMENTED - NOT TESTED (partial). Development
  machine only.
Files changed: CMakeLists.txt, app/**, tests/**, docs/** (11 Markdown files)
Reason: Rules 1, 3, 10, 13, 15, 16, 17, 24
Tests: ctest - 3/3 PASS (h264_level, smoke_record_dev, smoke_publish_dev) on macOS + GStreamer
  1.28.7 + MediaMTX 1.21.0; reconnection checked by hand; doccheck PROBLEMS 0, WARNINGS 0
```

---

## 2026-10-12 (tenth entry) — Slice C5: drive failure, drive return and HDD decoupling

### Objective

The owner approved slice C2 ("Commit and push"), accepted ADR-015 and chose slice C5: keep recording when a drive fails, fills, is missing or stalls, and reuse a returning drive from the next file (OQ-129, OQ-117, RISK-028).

### Starting State

Slice C2 committed and pushed as **`bc3c398`** (`9ab0b4e..bc3c398`); ADR-015 recorded as ACCEPTED (three stale status lines annotated). In slice C1 both drives were written inside the main pipeline (one drive's error would stop both copies; a stalled drive would block the encoder), and file names restarted at `pacs-00000.mp4`, so a second recording in the same directory would overwrite the first.

### Changes

1. **`app/src/writer.c/.h`** (new): one writer pipeline per drive (`appsrc ─ splitmuxsink` with fragmented `mp4mux`); states absent/joining/active/finishing; ALERT on error or full disk; stall detection from the `appsrc` backlog (`--writer-buffer-seconds`, default 10 s, OQ-117); `format-location` names files `pacs-<session>-<index>.mp4`.
2. **`pipeline.c`:** recording branch ends in an `appsink` feeding the writers; a timer every `--split-seconds` retries absent drives, marks a split pending and forces a keyframe; the next keyframe is the split point for all drives (`split-at-running-time`) and the join point for returning drives; the run ends when every drive has closed its file; per-drive file and failure counts at exit.
3. **Faults found by testing and fixed:** (a) with `split-now` from the timer, a rejoining drive's first file started 13 frames later than the other drive's (a natural keyframe was already queued for the active drive) — splits are now decided on the keyframe in the streaming thread; (b) a drive still joining when the recording stopped tried to open a file on end-of-stream — joining drives now stop without end-of-stream.
4. **ADR-016 (PROPOSED)** *(Superseded 2026-10-13 — ADR-016 ACCEPTED.)* recorded; documentation: SOFTWARE_ARCHITECTURE §10.2 (and superseding notes in §10), RECORDING, TESTING, OQ-117, OQ-129, RISK-028 (mitigation; still OPEN), REQ-REC-001, TRACEABILITY, PROJECT_STATUS, CHANGELOG.

### Files Modified

- New: `app/src/writer.c`, `app/src/writer.h`, `tests/smoke_drives.sh`.
- Changed: `app/CMakeLists.txt`, `app/src/{config.c,config.h,pipeline.c,pipeline.h}`, `tests/CMakeLists.txt`; `docs/`: `DECISIONS.md`, `SOFTWARE_ARCHITECTURE.md`, `RECORDING.md`, `TESTING.md`, `OPEN_QUESTIONS.md`, `RISKS.md`, `REQUIREMENTS.md`, `TRACEABILITY.md`, `PROJECT_STATUS.md`, `STREAMING.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`.

### Hardware Changes

None. A RAM disk on the development machine stood in for a full drive.

### Software Changes

Slice C5.

### Commands Used

```bash
cmake --build build && ctest --test-dir build --output-on-failure
bash tests/smoke_drives.sh build/app/pacscorder                       # 3 runs
hdiutil attach -nomount ram://16384; diskutil erasevolume HFS+ <name> <dev>   # 8 MB RAM disk (full-drive case)
ffprobe -select_streams v:0 -show_entries packet=pts_time,size,flags …   # frame-by-frame comparison of the two drives
```

### Test Results

- `smoke_drives_dev`: **TESTED — PASS**, 3 of 3 runs plus CTest (development machine): a drive missing at start rejoined at the next split with 3 byte-identical files (drive A 5 files); a filling RAM disk raised "No space left on the resource", drive A recorded 3 files, the partial file on the full disk stayed readable (2.7 s). *(Note 2026-10-13: after this entry was written, the test failed in 2 of the next 13 runs on 2026-10-12 — **TESTED — FAIL**, intermittent; see the 2026-10-13 entry.)*
- By hand: a drive becoming available exactly when the recording stops leaves no empty file; exit 0.
- `h264_level`, `smoke_record_dev`, `smoke_publish_dev`: still PASS (4/4 with CTest).
- **Not tested:** a real HDD stall (the stall path is implemented, not exercised); anything on the CM4, an NVMe SSD or an HDD.
- Documentation check: PASS.

### Problems Found

See Changes 3. Also: a full drive fails again at every split, with an alert each time (by design under OQ-129; alert delivery is OQ-091).

### Root Cause

(a) The split was requested asynchronously while a keyframe was already in a drive's queue.

### Solution

Split and join decided on the keyframe in the recording branch's streaming thread.

### Current Status

- PARTIAL: slice C5 IMPLEMENTED — NOT TESTED on hardware; development tests PASS.
- Uncommitted, pending owner approval.

### Next Step

1. Owner: approve the commit below; confirm ADR-016.
2. Remaining slices: C3 (EDID / DV timings / source changes), C4 (audio) — both need the CM4 to test for real.
3. Owner: buy the bring-up hardware (ADR-011).
4. Proposed commit (Rule 15): *(Superseded 2026-10-13 — not committed; replaced by the proposal in the 2026-10-13 entry.)*

```text
Commit title: feat(rec): isolate each recording drive and handle failure, return and stalls (slice C5)
Commit description: Write each recording drive in its own pipeline (ADR-016, PROPOSED): a failed, full or
  missing drive raises an ALERT while recording continues on the other; a returning drive rejoins at
  the next file on the same keyframe as the other drive; a drive more than --writer-buffer-seconds
  behind is dropped instead of blocking the encoder (OQ-117, RISK-028). Decide file splits on the
  keyframe for all drives; name files by session so recordings never overwrite each other. Add the
  smoke_drives_dev test (returning drive; full RAM disk). Record ADR-015 ACCEPTED and commit bc3c398.
  Development machine only.
Files changed: app/**, tests/**, docs/** (12 Markdown files)
Reason: Rules 1, 3, 10, 13, 15, 16, 17, 24
Tests: ctest - 4/4 PASS on macOS + GStreamer 1.28.7 (+ MediaMTX 1.21.0); smoke_drives_dev 3/3 runs;
  doccheck PROBLEMS 0, WARNINGS 0
```

---

## 2026-10-13 — Slice C5: intermittent drive-test failure explained; drive copies compared by content

### Objective

The owner asked for the status ("status") and then to continue ("continue"): find out why `smoke_drives_dev` failed intermittently after the tenth 2026-10-12 entry was written, and fix it before slice C5 is proposed for commit.

### Starting State

Slice C5 uncommitted (HEAD `bc3c398` = `origin/main`). The tenth 2026-10-12 entry recorded `smoke_drives_dev` as TESTED — PASS (3 of 3 runs plus CTest). After that entry was written, on 2026-10-12, the test failed in 2 of the next 13 runs (2 of 17 in all), both in case A: once under CTest after 17.03 s (the end of case A) with no message kept, and once with "pacs-…-00002.mp4 differs between the drives" (the rejoining drive's first file). The test's cleanup deleted both runs' recordings.

### Changes

1. **Evidence first.** `tests/smoke_drives.sh` keeps its logs and recordings in `$PACS_TEST_LOGDIR` when it fails and that variable is set. The writers log the running time at which each file starts ("opened … (starts at …)"; "split: file N starts at …" from the main pipeline) and any frame a drive's `appsrc` refuses.
2. **Hypothesis 1 — the rejoining drive's `appsrc` not yet accepting data — tested and dropped.** A guard that activated a joining drive only once its `appsrc` had reached PAUSED never triggered in 25 runs, no frame was refused, and its retry path was unreachable (the split timer only restarts absent drives). Removed (ARCH-003).
3. **Cause found and reproduced.**
   - `mp4mux` writes the wall-clock time, in whole seconds, into the creation times of `mvhd`, `tkhd` and `mdhd` when the muxer is created or reset, and into their modification times when it rewrites the header at the end of a file. Source: Claude's reading of the GStreamer 1.28.7 source — `isomp4/atoms.c` `atoms_get_current_qt_time`, `gstqtmux.c` `gst_qt_mux_reset` / `gst_qt_mux_send_moov`; `libs/gst/base/gstaggregator.c` calls `stop` on PAUSED→READY. Not a register fact.
   - Measured: one encoded stream muxed twice, 2 s apart, gives files that differ in exactly those 6 fields; all 240 frames and 9 fragments are otherwise identical.
   - At each split, `splitmuxsink` resets the active drive's muxer, so its creation time is taken at the split keyframe.
   - The GStreamer state log (`GST_DEBUG=GST_STATES:5`) shows a rejoining drive's muxer is **not** reset before its first file. It reaches PLAYING when its pipeline is created by the split timer, so its creation time is taken then: 0.30–0.32 s before the other drive's reset when idle (2 runs) and 0.58 s with heavy disk writing (1 of 2 loaded runs; 0.30 s in the other).
   - GLib fires `g_timeout_add_seconds` timers at a fixed microsecond phase of its monotonic clock. Source: GLib 2.88.3 `gmain.c`; on macOS `mach_absolute_time`; the phase offset is a hash of `DBUS_SESSION_BUS_ADDRESS` or `HOSTNAME`, neither set here. Measured: the split timer fired 0.623–0.630 s into the wall-clock second in 6 of 6 runs.
   - Result: the rejoining drive's first file carries a creation time one second earlier than the other drive's copy whenever the timer phase plus the gap crosses a second boundary — here about 0.63 + 0.31 ≈ 0.94 s when idle, so idle runs did not cross; with disk load the gap grows.
4. **Tests changed.**
   - `tests/mp4_same.py` compares two MP4 files byte for byte except those time fields.
   - `smoke_record_dev` and `smoke_drives_dev` use it instead of `cmp`, and print a NOTE when only the time fields differ.
   - New CTest `mp4_same_dev` (`tests/mp4_same_check.sh`): two muxes 1.2 s apart differ for `cmp` and are the same for `mp4_same.py`; one changed media byte is reported.
   - The `writer.h` comment is corrected: the writer pipeline is `appsrc ─ splitmuxsink`, with no `queue`.
5. **Owner question OQ-133:** must the two drives' copies be byte-identical, or is the same frames enough? Recommendation: the same frames. With one muxer per drive, byte identity cannot be guaranteed.
6. **Records:**
   - ARCH-002 (the timer-issued `split-now` of 2026-10-12, not archived at the time), ARCH-003 and ARCH-004 (byte-for-byte `cmp` of the copies).
   - Dated notes in ADR-016, SOFTWARE_ARCHITECTURE §10.2, RECORDING, TESTING (TEST-REC-001 mirror note; development tests), BUILD_SYSTEM §2A (`python3`), OPEN_QUESTIONS, CHANGELOG and PROJECT_STATUS.
   - The tenth entry annotated, and this file's "Last updated" (which still said 2026-10-09) corrected.

7. **Owner decisions (later on 2026-10-13):** commit and push slice C5 ("Commit and push"); **ADR-016 ACCEPTED** ("Accept ADR-016"); **OQ-133 ANSWERED** — the same video is enough, byte-identical copies are not required ("Same video is enough"). Recorded in DECISIONS (status, summary row, header 12 ACCEPTED / 2 PROPOSED) and OPEN_QUESTIONS (Answer/Evidence fields, summary row, header 105 OPEN / 28 ANSWERED); stale ADR-016 "PROPOSED" lines (14) and OQ-133 "owner question" lines (8) annotated with dated notes; CHANGELOG, PROJECT_STATUS; the `writer.h` comment no longer says PROPOSED. The proposed commit message below was updated before the commit.
### Files Modified

- New: `tests/mp4_same.py`, `tests/mp4_same_check.sh`.
- Changed (all still uncommitted slice C5 work): `app/src/writer.c`, `app/src/writer.h`, `app/src/pipeline.c`, `tests/smoke_drives.sh`, `tests/smoke_record.sh`, `tests/CMakeLists.txt`; `docs/`: `ARCHIVED_APPROACHES.md`, `BUILD_SYSTEM.md`, `CHANGELOG.md`, `DECISIONS.md`, `DEVELOPMENT_LOG.md`, `OPEN_QUESTIONS.md`, `PROJECT_STATUS.md`, `RECORDING.md`, `SOFTWARE_ARCHITECTURE.md`, `TESTING.md`.

### Hardware Changes

None.

### Software Changes

- Writer logging (file start times, refused frames).
- Readiness guard added, then removed.
- Mirror comparison changed in the tests; new test `mp4_same_dev`.

### Commands Used

```bash
PACS_TEST_LOGDIR=<dir> bash tests/smoke_drives.sh build/app/pacscorder        # 25 runs (guard and cmp)
gst-launch-1.0 videotestsrc num-buffers=240 ! … ! x264enc … ! h264parse ! matroskamux ! filesink location=in.mkv
gst-launch-1.0 filesrc location=in.mkv ! matroskademux ! h264parse ! mp4mux fragment-duration=1000 ! filesink location=frN.mp4   # twice, 2 s apart
cmp -l fr1.mp4 fr2.mp4                                                        # offsets 55/59, 171/175, 271/275 only
curl -sfL https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/1.28.7/subprojects/{gst-plugins-good/gst/isomp4/atoms.c,gst-plugins-good/gst/isomp4/gstqtmux.c,gst-plugins-good/gst/multifile/gstsplitmuxsink.c,gstreamer/libs/gst/base/gstaggregator.c}
curl -sfL https://gitlab.gnome.org/GNOME/glib/-/raw/2.88.3/glib/gmain.c
GST_DEBUG=qtmux:5 build/app/pacscorder --backend dev … --rec-dir a --rec-dir blk/b    # rejoin scenario: 20 runs idle, 20 with `dd bs=1m count=400; sync` in a loop
GST_DEBUG=GST_STATES:5,qtmux:5 build/app/pacscorder …                        # muxer state changes (2 loaded runs, 1 idle)
build/app/pacscorder … 2>&1 | python3 -u -c '…print(time.time() % 1)…'        # split-timer phase, 6 runs
rm -rf build && cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
python3 tools/doccheck.py docs
```

### Test Results

All on the development machine (macOS 26.6.2, GStreamer 1.28.7, GLib 2.88.3); nothing on PACSCORDER hardware.

- **Before any change, earlier on 2026-10-13:** 48 rejoin runs, all copies identical. These were 10 manual runs comparing the first file only; 12 with two recorders running at once (a CPU-load helper meant for this set failed to start); 14 with two recorders under 8 CPU-bound processes; and 12 of the real script.
- **With the guard and `cmp`:** `smoke_drives_dev` 25 of 25 PASS (idle); "not ready yet" 0 times, "refused a frame" 0 times.
- **Mux experiment:** two muxes of the same frames, 2 s apart, differ in the 6 time fields only.
- **Rejoin scenario, 40 runs.** Idle, the first file's copies differed (`cmp`) in **0 of 20** runs; with heavy disk writing, in **11 of 20**. `mp4_same.py` found the copies the same in 40 of 40. All 11 differences are the three creation times only: drive B one second earlier, modification times equal. Later files never differed. The "starting file" log gap (0.7–37 ms) turned out not to be the stamp gap: the state log shows the real one (Changes 3).
- **After the change:** a clean rebuild with no warnings. `ctest` 5 of 5 PASS, twice (`h264_level`, `smoke_record_dev`, `smoke_publish_dev`, `smoke_drives_dev`, `mp4_same_dev`). `smoke_drives_dev` 5 of 5 PASS with heavy disk writing; in run 4 a NOTE showed that file 00002's copies differed only in the header time stamps, which the old `cmp` check would have failed. No MediaMTX key files were written into the repository.
- Documentation check: PROBLEMS 0, WARNINGS 0 (680 of 680 facts cited; 133 OQs; 4 ARCH entries).

### Problems Found

1. The tenth entry recorded a pass after 3 runs; the intermittent failure needed more runs to show. The drive test is now repeated many times before a result is recorded.
2. The two drives' copies are not guaranteed byte-identical (OQ-133).
3. Errors of mine in this session, corrected before any commit:
   - My status message said "58 consecutive passes"; the count before the guard was 48.
   - I first wrote the cause into the drafts of OQ-133, SOFTWARE_ARCHITECTURE §10.2 and the ADR-016 note before measuring it, then found the measured gap seemed too small to explain the failures, and toned them down. The GStreamer state log then showed that my reading of the `splitmuxsink` source was wrong for a rejoining drive's first file: its muxer is not reset there. That reading had produced the misleading 1–2 ms estimate. All of these now give the measured explanation.

### Root Cause

- **Reproduced:** a rejoining drive's first file is stamped when its muxer is created, at the split timer; the other drive's copy is stamped when its muxer is reset at the split keyframe, 0.3–0.6 s later. A second boundary between the two makes the copies differ in their creation times, though every frame is identical. The whole-second timer's fixed phase decides how often that happens: never when idle on this machine on 2026-10-13, about half the time under heavy disk writing.
- **Most likely, not proven:** that this caused the 2026-10-12 failures. They had the same symptom on the same file; the timer's phase within the wall-clock second can differ between sessions (reasoning); the files were not kept.

### Solution

- The tests compare what the design guarantees: the same frames and file structure (`tests/mp4_same.py`).
- A NOTE shows any header-only difference, and a failed drive test keeps its evidence.
- Whether the product must make the copies byte-identical is the owner's decision (OQ-133). The design is unchanged.

### Current Status

- PARTIAL: slice C5 IMPLEMENTED — NOT TESTED on hardware; development tests TESTED — PASS (CTest 5 of 5; drive test 5 of 5 under disk load).
- Uncommitted, pending owner approval; ADR-016 and OQ-133 await the owner. *(Later on 2026-10-13: the owner approved the commit and push, accepted ADR-016 and answered OQ-133 — Changes 7.)*

### Next Step

1. Owner: approve the commit below; confirm ADR-016; answer OQ-133. *(Done 2026-10-13 — Changes 7.)*
2. Remaining slices: C3 (EDID / DV timings / source changes), C4 (audio) — both need the CM4 to test for real.
3. Owner: buy the bring-up hardware (ADR-011).
4. Proposed commit (Rule 15), replacing the one in the tenth 2026-10-12 entry:

```text
Commit title: feat(rec): isolate each recording drive and handle failure, return and stalls (slice C5)
Commit description: Write each recording drive in its own pipeline (ADR-016, ACCEPTED): a failed, full or
  missing drive raises an ALERT while recording continues on the other; a returning drive rejoins at
  the next file on the same keyframe as the other drive; a drive more than --writer-buffer-seconds
  behind is dropped instead of blocking the encoder (OQ-117, RISK-028). Decide file splits on the
  keyframe for all drives; name files by session so recordings never overwrite each other. Add the
  smoke_drives_dev test (returning drive; full RAM disk). Compare the drives' copies by content
  (tests/mp4_same.py, tested by mp4_same_dev): each drive's mp4mux stamps its header with the
  wall-clock second, and a rejoining drive's first file is stamped earlier, so the copies can differ
  in those fields only - the most likely cause of the test's intermittent failures. Owner decisions:
  ADR-015 and ADR-016 ACCEPTED; OQ-133 ANSWERED (the same video is enough). Record commit bc3c398
  and ARCH-002 to ARCH-004. Development machine only.
Files changed: app/**, tests/**, docs/** (14 Markdown files)
Reason: Rules 1, 3, 10, 13, 14, 15, 16, 17, 21, 24
Tests: ctest 5/5 PASS on macOS + GStreamer 1.28.7 (+ MediaMTX 1.21.0); smoke_drives_dev 5/5 with
  heavy disk writing (1 header-only NOTE); rejoin scenario 40 runs, frames identical in 40/40;
  doccheck PROBLEMS 0, WARNINGS 0
```

---

## 2026-10-13 (second entry) — Linux check in Debian trixie with GStreamer 1.26; RTMP URL fault found and fixed

### Objective

The owner asked "what to do now" and chose the recommended work while the bring-up hardware is on order: build and test the application on the CM4's distribution and GStreamer series, in a container on the development machine. The owner also stated that the bring-up hardware has already been ordered ("Already ordered").

### Starting State

Slice C5 committed and pushed on the owner's instruction as **`f932c0a`** (`bc3c398..f932c0a`); working tree clean. The application had only been built and tested on macOS with GStreamer 1.28.7; the CM4's distribution ships 1.26.2 [G-26], [G-27].

### Changes

1. **Docker on the Mac:** Colima 0.10.3 and the Docker CLI 29.9.0 installed with Homebrew (owner's choice "Linux check first", which named this installation). The VM was started by hand (`colima start --vm-type vz --arch aarch64 --cpu 4 --memory 4 --disk 30`); it is not a login service.
2. **`tools/linux_check/Dockerfile`, `tools/linux_check.sh`** (new):
   - The image is `debian:trixie` arm64 with GStreamer, CMake, GCC, FFmpeg and Python from Debian.
   - MediaMTX v1.21.0 comes from its GitHub release, checked against the release's SHA-256.
   - The script mounts the source read-only, builds inside the container and runs CTest.
   - An 8 MB tmpfs serves as the full drive.
3. **`tests/smoke_drives.sh`:** case B uses `PACS_TEST_FULL_DIR` (any small, empty filesystem) when it is set, otherwise the macOS RAM disk as before; so the full-drive case runs on Linux.
4. **First Linux run: 4 of 5.** `smoke_publish_dev` FAILED: the RTMP publisher reported "Failed to connect: Host is not set" in a loop; RTSP worked.
5. **Cause:**
   - In GStreamer 1.26.2, `rtmp2sink`'s URI handler requires `rtmp[s]://host[:port]/application/stream`. A URL without the stream part fails with "URI lacks stream", the handler then resets the host to NULL, and the connection fails with "Host is not set".
   - 1.28.7 accepts a URL without the stream part and uses an empty stream name. Source: Claude's comparison of `gst-plugins-bad/gst/rtmp2/gstrtmp2locationhandler.c` at 1.26.2 and 1.28.7.
   - Reproduced in the container with `gst-launch-1.0 … ! flvmux ! rtmp2sink location=…`: the one-part URL gives "Host is not set"; the two-part URL does not.
   - The test had used `rtmp://127.0.0.1:11935/rtmplive`, so the fault only appeared on 1.26.
6. **Fix (`app/src/config.c`):** every `--rtmp-url` must be `rtmp://` or `rtmps://` with a host, an application and a stream. Anything else stops the program at start with exit code 2 and the message "--rtmp-url number N is not rtmp[s]://host[:port]/application/stream". The URL itself is not printed, because it carries the stream key. The help text names the form. Checked by hand:
   - accepted: `rtmp://127.0.0.1:11935/live/rtmp` and `rtmps://a.example.com/live2/abc` (exit 0);
   - refused with exit 2: `…/rtmplive`, `rtmp://h/app/`, `rtmp:///a/b`, `rtmp://h:1935`, `http://h/a/b`, `rtmp://:1935/a/b`, `rtmp://h//b`.
7. **`tests/smoke_publish.sh`:** a new run 0 checks that a one-part RTMP URL is refused (exit 2), with no MediaMTX needed. The RTMP run publishes to `rtmp://127.0.0.1:11935/live/rtmp` and reads `live/rtmp` back.
8. **Documentation:**
   - BUILD_SYSTEM §2A (Linux check: procedure, versions, results, limits); SOFTWARE_ARCHITECTURE §10.1 and the STREAMING implementation box (the RTMP URL rule; the stream key in log lines as a known issue); TESTING development tests.
   - HARDWARE §8: the order, with its contents marked UNKNOWN — VERIFICATION REQUIRED.
   - CHANGELOG, PROJECT_STATUS (`f932c0a`, Next Step 18), root README.

### Files Modified

- New: `tools/linux_check.sh`, `tools/linux_check/Dockerfile`.
- Changed: `app/src/config.c`, `tests/smoke_drives.sh`, `tests/smoke_publish.sh`, `README.md`; `docs/`: `BUILD_SYSTEM.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`, `HARDWARE.md`, `PROJECT_STATUS.md`, `SOFTWARE_ARCHITECTURE.md`, `STREAMING.md`, `TESTING.md`.

### Hardware Changes

None. The owner reports that the bring-up hardware has been ordered; its contents are not yet recorded.

### Software Changes

RTMP URL check at start; Linux check tooling; test changes above.

### Commands Used

```bash
brew install colima docker
colima start --vm-type vz --arch aarch64 --cpu 4 --memory 4 --disk 30
tools/linux_check.sh                                   # twice: before and after the fix
docker run --rm pacscorder-linux-check gst-launch-1.0 videotestsrc num-buffers=30 ! x264enc ! h264parse ! flvmux streamable=true ! rtmp2sink location=<url>
curl -sfL https://gitlab.freedesktop.org/gstreamer/gstreamer/-/raw/{1.26.2,1.28.7}/subprojects/gst-plugins-bad/gst/rtmp2/gstrtmp2locationhandler.c
docker run --rm pacscorder-linux-check dpkg-query -W …   # package versions
cmake --build build && ctest --test-dir build --output-on-failure
python3 tools/doccheck.py docs
```

### Test Results

- **Linux check** (Debian 13 trixie aarch64, `debian@sha256:913f6706…c2d5`; GStreamer 1.26.2, GLib 2.84.4, GCC 14.2.0, CMake 3.31.6, MediaMTX v1.21.0):
  - The build produced no compiler warnings.
  - First run: **TESTED — FAIL**, 4 of 5 (`smoke_publish_dev`, RTMP).
  - After the fix: **TESTED — PASS, 5 of 5**:
    - `smoke_record_dev`: 3 files, the same in both directories; 718 frames captured, 718 recorded, 359 live.
    - `smoke_publish_dev`: RTSP and RTMP both `h264, Constrained Baseline, 1280x720, 30/1`; with the RTSP target down the recording completed with 3 files; the one-part URL was refused.
    - `smoke_drives_dev`: case A as on the Mac; case B on the 8 MB tmpfs: drive filled up, alert raised, drive A recorded 3 files.
    - `mp4_same_dev` and `h264_level` passed.
- **macOS** (GStreamer 1.28.7) after the change: `ctest` 5 of 5 PASS; `smoke_drives.sh` (case B via the RAM disk) PASS.
- Documentation check: PROBLEMS 0, WARNINGS 0.
- Not tested: anything on Raspberry Pi OS or a CM4. The Raspberry Pi archive's own build of `gstreamer1.0-plugins-bad` [G-27] may differ from Debian's.

### Problems Found

1. RTMP publishing would have failed on the CM4's GStreamer series with the test's one-part URL. Fixed by refusing such URLs at start.
2. The RTMP publisher's log lines print the full URL, including the stream key (a secret, if logs are shared). Not changed; for the owner.
3. My test data was the cause: a one-part RTMP URL is not a realistic destination, and GStreamer 1.28 hid the fault.

### Root Cause

`rtmp2sink` URI parsing differs between GStreamer 1.26.2 and 1.28.7 (see Changes 5).

### Solution

Validate RTMP URLs at start against the 1.26 rule; test with a realistic two-part URL; run the tests on the CM4's GStreamer series before hardware arrives.

### Current Status

- PARTIAL: application slices C1, C2, C5 IMPLEMENTED — NOT TESTED on hardware; development tests TESTED — PASS on macOS and in Debian trixie with GStreamer 1.26.2.
- Uncommitted, pending owner approval.

### Next Step

1. Owner: approve the commit below; say whether the publisher should stop logging the RTMP stream key; list the ordered hardware for HARDWARE.md. *(Later on 2026-10-13: the owner chose "Commit and push" for the commit below and "Hide the key in logs" — done as its own change, next entry. The hardware list is still to come.)*
2. Remaining slices: C3 (EDID / DV timings / source changes), C4 (audio; owner questions OQ-063, OQ-111, OQ-112, OQ-113).
3. When the hardware arrives: TEST-ENC-001 Stage 0, then TEST-HW-001.
4. Proposed commit (Rule 15):

```text
Commit title: fix(stream): refuse RTMP URLs that GStreamer 1.26 cannot use; add a Debian trixie Linux check
Commit description: Add tools/linux_check.sh and tools/linux_check/Dockerfile: build the application and
  run the development tests in Debian trixie arm64 with Debian's GStreamer 1.26.2 (the CM4's series) in
  a container. Its first run found that rtmp2sink 1.26.2 refuses an RTMP URL without a stream part and
  then fails with "Host is not set" (1.28 accepts it): --rtmp-url is now checked at start against
  rtmp[s]://host[:port]/application/stream (exit 2, URL not printed), and smoke_publish_dev uses a
  two-part URL and checks the refusal. smoke_drives_dev case B can use any small filesystem
  (PACS_TEST_FULL_DIR). Record commit f932c0a and the owner's hardware order. Development machine only.
Files changed: app/src/config.c, tests/**, tools/**, README.md, docs/** (8 Markdown files)
Reason: Rules 1, 3, 10, 15, 16, 17, 22, 24
Tests: Linux check (Debian 13, GStreamer 1.26.2): no warnings, ctest 5/5 PASS (first run 4/5 - the
  RTMP fault); macOS (GStreamer 1.28.7): ctest 5/5 PASS; doccheck PROBLEMS 0, WARNINGS 0
```

---

## 2026-10-13 (third entry) — RTMP stream key and other URL secrets kept out of the log

### Objective

Owner decision of 2026-10-13: "Hide the key in logs" — the RTMP publisher printed the full URL, including the destination's stream key, in its log lines.

### Starting State

The Linux check and the RTMP URL fix committed and pushed on the owner's instruction as **`fc76e9b`** (`f932c0a..fc76e9b`); working tree clean.

### Changes

1. **`app/src/url_redact.c/.h`** (new, in `pacs_core`, plain C): `pacs_url_redact` returns a copy of a URL with any `user:password@` replaced by `***@` and any query by `?***`. For `rtmp://` and `rtmps://` it also replaces the last path component, the stream key, by `***`.
2. **`app/src/publisher.c/.h`, `pipeline.c`:** the publisher keeps only the redacted URL (`log_url`); the real URL is passed only to the sink's `location`. Every log line and error message about a publisher uses the redacted form, including the restart count at exit (`pacs_publisher_log_url`). Limit: an error text written by a GStreamer element itself is printed as is; none seen in the tests contained a URL.
3. **Tests:**
   - New unit test `url_redact` (11 cases).
   - `smoke_publish_dev` requires "RTMP publisher started: rtmp://127.0.0.1:11935/live/***" and fails if the key appears anywhere in the log.
4. **Found by the Linux check:**
   - The first version used `strdup`, which the project's strict C11 setting does not declare with Debian's GCC; Apple clang accepted it. It now uses a local copy function.
   - `tools/linux_check.sh` printed "no compiler warnings" and ran the tests after a failed build. Under `pipefail` the failed build turned `… | grep … || echo` into the "no warnings" branch. A failed build now prints its log and stops the check. The earlier runs were not affected: their builds succeeded and their tests ran.
5. **Documentation:** SOFTWARE_ARCHITECTURE §10.1, TESTING, BUILD_SYSTEM §2A, CHANGELOG, PROJECT_STATUS.

### Files Modified

- New: `app/src/url_redact.c`, `app/src/url_redact.h`, `tests/test_url_redact.c`.
- Changed: `app/CMakeLists.txt`, `app/src/publisher.c`, `app/src/publisher.h`, `app/src/pipeline.c`, `tests/CMakeLists.txt`, `tests/smoke_publish.sh`, `tools/linux_check.sh`; `docs/`: `BUILD_SYSTEM.md`, `CHANGELOG.md`, `DEVELOPMENT_LOG.md`, `PROJECT_STATUS.md`, `SOFTWARE_ARCHITECTURE.md`, `TESTING.md`.

### Hardware Changes

None.

### Software Changes

URL redaction in the publisher's log lines; Linux-check runner fix.

### Commands Used

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
build/tests/test_url_redact
tools/linux_check.sh                       # failed to build (strdup), then 6 of 6
python3 tools/doccheck.py docs
```

### Test Results

- macOS (GStreamer 1.28.7): build with no warnings; `ctest` **6 of 6 PASS** (`h264_level`, `url_redact` 11/11, `smoke_record_dev`, `smoke_publish_dev` with the no-key check, `smoke_drives_dev`, `mp4_same_dev`).
- Linux check (Debian trixie, GStreamer 1.26.2): first run **TESTED — FAIL** (build error, `strdup`); after the fix, build with no warnings and **6 of 6 PASS**.
- Documentation check: PROBLEMS 0, WARNINGS 0.

### Problems Found

1. Strict C portability: `strdup` is not ISO C. The Mac's compiler did not catch it; the Linux check did.
2. My `tools/linux_check.sh` from the second entry could hide a failed build behind "no compiler warnings". Fixed before it was relied on; the earlier results stand, because their tests ran against a built binary.

### Root Cause

See Problems Found.

### Solution

See Changes.

### Current Status

- PARTIAL: application slices C1, C2, C5 IMPLEMENTED — NOT TESTED on hardware; development tests TESTED — PASS on macOS and in Debian trixie with GStreamer 1.26.2.
- Uncommitted, pending owner approval.

### Next Step

1. Owner: approve the commit below; list the ordered hardware for HARDWARE.md.
2. Remaining slices: C3, C4 (owner questions OQ-063, OQ-111, OQ-112, OQ-113).
3. Proposed commit (Rule 15):

```text
Commit title: fix(stream): keep the RTMP stream key and other URL secrets out of the log
Commit description: Print publisher URLs through pacs_url_redact (app/src/url_redact.c): the RTMP
  stream key, any user name and password, and any query are replaced by ***, so log lines show
  rtmp://host/application/***. The publisher keeps only the redacted URL; the real one goes only
  to the sink. Add the url_redact unit test and a no-key check in smoke_publish_dev (owner
  decision, 2026-10-13). Make tools/linux_check.sh stop on a failed build (it reported "no
  compiler warnings"), and avoid strdup, which strict C on Debian does not declare (both found by
  the Linux check). Record commit fc76e9b.
Files changed: app/**, tests/**, tools/linux_check.sh, docs/** (6 Markdown files)
Reason: Rules 1, 3, 10, 15, 16, 17, 24
Tests: macOS (GStreamer 1.28.7) ctest 6/6 PASS; Linux check (Debian 13, GStreamer 1.26.2) ctest
  6/6 PASS after the strdup fix (first run: build failure); doccheck PROBLEMS 0, WARNINGS 0
```

---

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created with the Phase 0 bootstrap entry. | Claude (session 2026-10-06) |
| 2026-10-07 | Entry added: owner decisions, ADR-003 accepted, commit squash (written 2026-10-08). | Claude (session 2026-10-07/08) |
| 2026-10-08 | Entry added: topics H and I research and propagation. | Claude (session 2026-10-08) |
| 2026-10-08 | Second 2026-10-08 entry added: H.265 deferred (OQ-103), commit `df3591d` and push. | Claude (session 2026-10-08) |
| 2026-10-08 | Third 2026-10-08 entry added: two H.264 encodes (OQ-005), push of `d2d217e`. | Claude (session 2026-10-08) |
| 2026-10-09 | Fourth 2026-10-08 entry added (storage and latency research, ADR-009, OQ-116; reconstructed from evidence because that session wrote none). 2026-10-09 entry added: documentation catch-up and verification. The third 2026-10-08 entry's Next Step items annotated as done or done in part. | Claude (session 2026-10-09) |
| 2026-10-09 | Second 2026-10-09 entry added: commit and push of `f39e661`; owner decisions on latency criterion, viewer reach and recording duration; OQ-129. The first 2026-10-09 entry's Next Step items annotated (pushed; decided in part). | Claude (session 2026-10-09) |
| 2026-10-09 | Third 2026-10-09 entry added: commit and push of `6ad83d4`; owner decisions on bitrate (OQ-005) and mirrored-drive failure (OQ-129). | Claude (session 2026-10-09) |
| 2026-10-09 | Fourth 2026-10-09 entry added: commit and push of `b142871`; owner decisions on a drive missing at start and 30-minute file splitting (OQ-129). Third entry's Next Step items annotated. Extended the same day (uncommitted, owner "Not yet") with the drive-return, viewer and browser decisions. | Claude (session 2026-10-09) |
| 2026-10-09 | Fifth 2026-10-09 entry added: owner decisions on the recording encode (High profile, Level 4.2, no B-frames), the recording capture-to-file latency (< 1 s glass-to-disk) and the WebRTC measurement run (30-minute, ~1800 samples); OQ-005 and OQ-008 ANSWERED; OQ-130 opened. Documentation consistency check NOT RUN (doccheck.py unavailable). Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-09 | Sixth 2026-10-09 entry added: OQ-130 ANSWERED (recording capture-to-file < 1 s judged against the primary drive, the NVMe SSD, at the 95th percentile; the HDD a lagging mirror; ADR-009 mirror refined). Documentation consistency check NOT RUN (doccheck.py unavailable). Noted the external "update by ai" commits that committed the working tree mid-session. Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-09 | Seventh 2026-10-09 entry added: product platform decided — CM4 (ADR-004 ACCEPTED); CM5, BCM2712, Pi 5 and Pi 4 Model B dropped as product boards (CM5 archived, ARCH-001); OQ-011 ANSWERED. Authoritative and reader-facing docs updated; scope rule set for deep inline CM5 content (follow-up sweep recommended). Documentation consistency check NOT RUN (doccheck.py unavailable). Uncommitted, pending owner approval. | Claude (session 2026-10-09) |
| 2026-10-12 | 2026-10-12 entry added (doc check rebuilt, ADR-004 scope sweep, OQ-129 propagation, hand-commit catch-up, research topic L, TEST-ENC-001 Stage 0). Seventh 2026-10-09 entry's Next Step items 1 and 2 annotated (committed by hand as `3c823b6` "mmit", reworded before push to `27c44d9`; sweep done). | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 second entry added: commit `1f5c0e0`, reword of `3c823b6` to `27c44d9`, push `049d1ee..1f5c0e0`; live encode 1080p30 and OQ-131; CM5-only OQs ANSWERED; doccheck W3 refinement. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 third entry added: commit and push of `c9d7e19`; OQ-131 50 Hz decision (25 fps); Stage 0 run R9. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 fourth entry added: commit and push of `91ff869`; OQ-131 non-1080 decision (keep source resolution; 720p at Level 3.2 or above). | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 fifth entry added: ADR-010, ADR-008 recommendation, research topic M, RISK-035, ADR-011; commit `b8091fd` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 sixth entry added: commit and push of `7d7d2de`; ADR-010, ADR-008, ADR-011 ACCEPTED; ADR-006 SUPERSEDED; W3 extension; 77 status notes. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 seventh entry added: `78b6d70` recorded; ADR-007 GStreamer proposal; ADR-012; OQ-132. | Claude (session 2026-10-12) |
| 2026-10-12 | Seventh entry extended: owner decisions (ADR-007 ACCEPTED; C chosen — ADR-013 ACCEPTED, ADR-012 SUPERSEDED); commit message updated before commit. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 eighth entry added: coding started — slice C1, ADR-014, development tests, bring-up script; `27bb305` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 ninth entry added: slice C2 (live publishing), ADR-015; `9ab0b4e` recorded. | Claude (session 2026-10-12) |
| 2026-10-12 | 2026-10-12 tenth entry added: slice C5 (drive failure, return, HDD decoupling), ADR-016, ADR-015 accepted; `bc3c398` recorded. | Claude (session 2026-10-12) |
| 2026-10-13 | 2026-10-13 entry added: slice C5 intermittent drive-test failure explained and reproduced (header time stamps), comparison changed, OQ-133, ARCH-002 to ARCH-004. Tenth 2026-10-12 entry annotated (later failures; proposed commit superseded). "Last updated" corrected from 2026-10-09. | Claude (session 2026-10-13) |
| 2026-10-13 | 2026-10-13 entry extended: owner decisions (commit and push; ADR-016 ACCEPTED; OQ-133 ANSWERED); proposed commit message updated before commit; tenth entry's ADR-016 line annotated. | Claude (session 2026-10-13) |
| 2026-10-13 | 2026-10-13 second entry added: Linux check (Debian trixie, GStreamer 1.26.2), RTMP URL fault found and fixed, `f932c0a` recorded, hardware ordered (owner). | Claude (session 2026-10-13) |
| 2026-10-13 | Second entry: owner decisions noted (commit and push; hide the RTMP stream key in logs). | Claude (session 2026-10-13) |
| 2026-10-13 | 2026-10-13 third entry added: RTMP stream key and URL secrets hidden in log lines; Linux-check runner and `strdup` fixes; `fc76e9b` recorded. | Claude (session 2026-10-13) |
