# PACSCORDER — project instructions for Claude Code

The engineering rules in @docs/ENGINEERING_RULES.md are mandatory for all work in this repository. That file is authoritative; the summary below is only a reminder.

- **Session start (Rule 19):** read `docs/PROJECT_STATUS.md`, `docs/DEVELOPMENT_LOG.md`, `docs/CHANGELOG.md`, `docs/ARCHITECTURE.md`, `docs/REQUIREMENTS.md`, then run `git status`. Report CURRENT PROJECT STATE / CURRENT PHASE / LAST COMPLETED STEP / CURRENT PROBLEM / CURRENT OBJECTIVE / NEXT ACTION.
- **Status words (Rule 10):** only `IMPLEMENTED — NOT TESTED`, `TESTED — PASS`, `TESTED — FAIL`, `PARTIALLY VERIFIED`, `BLOCKED — HARDWARE REQUIRED`. Never say working / fixed / verified / production ready without test evidence.
- **Unknowns (Rules 8, 22):** write `UNKNOWN — VERIFICATION REQUIRED` and name the information needed to resolve it. Never invent hardware facts.
- **Source priority (Rule 23):** hardware measurement > datasheet > official Raspberry Pi docs > Linux kernel source > existing working driver > Buildroot docs > project docs > Claude's reasoning.
- **Before a major change (Rule 16):** state what, why, affected files, risks, test plan. **After (Rule 17):** build, test, record, then update DEVELOPMENT_LOG, CHANGELOG, technical docs, PROJECT_STATUS.
- **Abandoned approaches** go in `docs/ARCHIVED_APPROACHES.md`; **decisions** go in `docs/DECISIONS.md` as ADRs; **requirements** keep IDs in `docs/REQUIREMENTS.md` (`REQ-<AREA>-NNN`).
- **Git (Rule 15):** conventional commit titles (`feat(driver): …`, `docs(dt): …`). Propose title / description / files / reason / tests before committing; commit only when the project owner asks.
- **Session end (Rule 20):** report WHAT CHANGED / FILES CHANGED / TESTS RUN / TEST RESULTS / KNOWN ISSUES / DOCUMENTATION UPDATED / GIT STATUS / NEXT STEP.
