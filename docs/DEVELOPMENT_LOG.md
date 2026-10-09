# Development Log

| | |
|---|---|
| Document status | Active — one entry per significant development session (Rule 3) |
| Last updated | 2026-10-09 |

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

1. Commit this change once the owner approves. *(Done: owner, 2026-10-09: "Commit and push"; committed with this message and pushed.)* Proposed commit (Rule 15):

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

2. The owner decides the OQ-129 remainder (file splitting; a drive absent at the start) and the OQ-008 remainder (browsers, viewer count, sample count and run length).
3. The owner obtains the CM4 + CM5 bring-up hardware, including recording storage.

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
