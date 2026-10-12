#!/usr/bin/env python3
"""PACSCORDER documentation consistency check.

Usage:  python3 tools/doccheck.py docs

Exit code 0 when there are no PROBLEMS (WARNINGS do not fail the run).

Rebuilt 2026-10-12 from the check list recorded in docs/DEVELOPMENT_LOG.md
(2026-10-06 entry, "Test Results"); the original script lived only in a session
scratchpad and was lost. This is a re-implementation, not the original file, so
its WARNING counts are not directly comparable with runs logged before
2026-10-12.

PROBLEMS (fail the run):
  P1  a cited fact ID ([X-NN]) is not defined in REFERENCES.md
  P2  a REQ/ADR/RISK/OQ/TEST/ARCH ID is used but not defined in its canonical file
  P3  a relative .md link does not resolve
  P4  TESTING.md does not define exactly the canonical test IDs listed in README.md
  P5  a technical document lacks a "Verification" section, or a document lacks
      a "Change history" section
  P6  a Rule 2 / rule-mandated document is missing
  P7  a register's header count disagrees with the register body
WARNINGS (do not fail):
  W1  possible success claim (Rule 10) outside the known false positives
  W2  a register fact is never cited outside REFERENCES.md
  W3  possibly stale status: an ACCEPTED ADR or ANSWERED OQ is described as
      OPEN/PROPOSED on the same line, without a superseding note
"""
import re
import sys
from pathlib import Path

RULE2_FILES = [
    "README.md", "PROJECT_STATUS.md", "ARCHITECTURE.md", "HARDWARE.md",
    "SOFTWARE_ARCHITECTURE.md", "BUILD_SYSTEM.md", "DEVICE_TREE.md",
    "TC358743_DRIVER.md", "V4L2.md", "CSI_PIPELINE.md", "DMA.md",
    "VIDEO_ENCODER.md", "RECORDING.md", "STREAMING.md", "ATEM.md", "TESTING.md",
    "TROUBLESHOOTING.md", "PERFORMANCE.md", "RELEASE.md", "CHANGELOG.md",
    # mandated by other rules
    "DEVELOPMENT_LOG.md", "REQUIREMENTS.md", "DECISIONS.md",
    "ARCHIVED_APPROACHES.md", "TRACEABILITY.md", "ENGINEERING_RULES.md",
]
TECH_DOCS = [
    "ARCHITECTURE.md", "SOFTWARE_ARCHITECTURE.md", "HARDWARE.md",
    "DEVICE_TREE.md", "TC358743_DRIVER.md", "V4L2.md", "CSI_PIPELINE.md",
    "DMA.md", "VIDEO_ENCODER.md", "RECORDING.md", "STREAMING.md", "ATEM.md",
    "BUILD_SYSTEM.md", "RELEASE.md", "TESTING.md", "TROUBLESHOOTING.md",
    "PERFORMANCE.md",
]
# Files exempt from the Change-history requirement: verbatim owner rules, and
# the generated source register (Rule 21: entries are not edited).
NO_HISTORY = {"ENGINEERING_RULES.md", "REFERENCES.md"}

ID_DEFS = {  # prefix -> (canonical file, heading regex capturing the ID)
    "REQ": ("REQUIREMENTS.md", r"^#+ (REQ-[A-Z]+-\d{3})\b"),
    "ADR": ("DECISIONS.md", r"^#+ (ADR-\d{3})\b"),
    "RISK": ("RISKS.md", r"^#+ (RISK-\d{3})\b"),
    "OQ": ("OPEN_QUESTIONS.md", r"^#+ (OQ-\d{3})\b"),
    "TEST": ("TESTING.md", r"^#+ (TEST-[A-Z]+-\d{3})\b"),
    "ARCH": ("ARCHIVED_APPROACHES.md", r"^#+ (ARCH-\d{3})\b"),
}
ID_USE = re.compile(r"\b(REQ-[A-Z]+-\d{3}|ADR-\d{3}|RISK-\d{3}|OQ-\d{3}|TEST-[A-Z]+-\d{3}|ARCH-\d{3})\b")
FACT_DEF = re.compile(r"^### ([A-Z]-\d{2})\s*$", re.M)
FACT_GROUP = re.compile(r"\[([A-Z]-\d{2}(?:\s*,\s*[A-Z]-\d{2})*)\]")
MD_LINK = re.compile(r"\]\(([^)\s#]+\.md)(#[^)]*)?\)")

SUCCESS = re.compile(
    r"\b(?:is|are|now|was|were|been|be)\s+(?:working|fixed|verified)\b"
    r"|\bproduction[- ]ready\b", re.I)
NEGATION = re.compile(
    r"\b(?:not|never|no|nothing|none|until|unless|without|cannot|neither|nor)\b"
    r"|n't\b", re.I)
# Known false positives (see DEVELOPMENT_LOG.md, 2026-10-06 and 2026-10-09):
# quoted datasheet / muxer wording and a quoted old requirement value.
SUCCESS_ALLOW = re.compile(
    r"fixed up|TDM is fixed|fixed at \d|is fixed by|are fixed by"
    r"|verified from sources|will be verified|to be verified", re.I)

STALE_WORDS = re.compile(r"\b(OPEN|PROPOSED)\b")
SUPERSEDE_MARK = re.compile(
    r"Superseded|superseded|ACCEPTED|ANSWERED|was OPEN|was PROPOSED|until "
    r"|opened |reopen|\(O\)\s*→|O → A|O->A", re.I)


def read(path):
    return path.read_text(encoding="utf-8")


def section_lines(text, heading_re):
    """Yield (lineno, line) not inside a section whose heading matches heading_re."""
    skip = False
    for n, line in enumerate(text.splitlines(), 1):
        if line.startswith("#"):
            skip = bool(re.match(heading_re, line))
        if not skip:
            yield n, line


def main(docdir):
    d = Path(docdir)
    problems, warnings = [], []
    files = sorted(p.name for p in d.glob("*.md"))
    texts = {f: read(d / f) for f in files}

    # P6 required files
    for f in RULE2_FILES:
        if f not in texts:
            problems.append(f"missing required document: {f}")

    # fact definitions
    refs = texts.get("REFERENCES.md", "")
    facts = set(FACT_DEF.findall(refs))
    topics = sorted({f[0] for f in facts})

    # ID definitions
    defined = {}
    for prefix, (fname, rx) in ID_DEFS.items():
        defined[prefix] = set(re.findall(rx, texts.get(fname, ""), re.M))

    # canonical tests from README table "Canonical test IDs"
    readme = texts.get("README.md", "")
    m = re.search(r"### 9\. Canonical test IDs(.*?)(?:\n## |\n### )", readme, re.S)
    canon = set(re.findall(r"^\| (TEST-[A-Z]+-\d{3}) \|", m.group(1), re.M)) if m else set()
    if canon != defined["TEST"]:
        problems.append(
            "TESTING.md test IDs differ from README canonical list: "
            f"missing={sorted(canon - defined['TEST'])} extra={sorted(defined['TEST'] - canon)}")

    cited = set()
    for f, t in texts.items():
        # P1 fact citations (REFERENCES.md cites only inside its own evidence)
        for g in FACT_GROUP.findall(t):
            for fid in re.split(r"\s*,\s*", g):
                if f != "REFERENCES.md":
                    cited.add(fid)
                if fid not in facts:
                    problems.append(f"{f}: cites undefined fact [{fid}]")
        # P2 IDs
        if f not in ("ENGINEERING_RULES.md",):
            for n, line in enumerate(t.splitlines(), 1):
                for uid in ID_USE.findall(line):
                    prefix = uid.split("-")[0]
                    if uid not in defined[prefix]:
                        problems.append(f"{f}:{n}: uses undefined ID {uid}")
        # P3 links
        for target, _anchor in MD_LINK.findall(t):
            if target.startswith(("http:", "https:")):
                continue
            if not (d / target).resolve().exists():
                problems.append(f"{f}: broken link → {target}")
        # P5 sections
        if f in TECH_DOCS and not re.search(r"^#+ .*Verification", t, re.M | re.I):
            problems.append(f"{f}: no Verification section")
        if f not in NO_HISTORY and not re.search(r"^#+ Change history", t, re.M):
            problems.append(f'{f}: no "Change history" section')
        # W1 success claims (REFERENCES.md is skipped: its success words are
        # verbatim source quotes, and the register is not edited, Rule 21)
        if f not in ("ENGINEERING_RULES.md", "REFERENCES.md"):
            for n, line in enumerate(t.splitlines(), 1):
                if SUCCESS.search(line) and not NEGATION.search(line) \
                        and not SUCCESS_ALLOW.search(line):
                    warnings.append(f"{f}:{n}: possible success claim: {line.strip()[:110]}")

    # W2 uncited facts
    uncited = sorted(facts - cited, key=lambda x: (x[0], int(x[2:])))
    for fid in uncited:
        warnings.append(f"REFERENCES.md: fact [{fid}] is not cited by any other document")

    # P7 register header counts
    dec = texts.get("DECISIONS.md", "")
    idx = dict(re.findall(r"^\| (ADR-\d{3}) \|[^|]*\| (\w+)", dec, re.M))
    counts = {s: list(idx.values()).count(s) for s in set(idx.values())}
    hm = re.search(r"Document status \| Active — (.*?) \|", dec)
    if hm:
        for num, word in re.findall(r"(\d+) (ACCEPTED|PROPOSED|OPEN|SUPERSEDED)", hm.group(1)):
            if counts.get(word, 0) != int(num):
                problems.append(f"DECISIONS.md header says {num} {word}, index has {counts.get(word, 0)}")
    oq = texts.get("OPEN_QUESTIONS.md", "")
    oq_status = {}
    for sec in re.split(r"^## (?=OQ-\d{3})", oq, flags=re.M)[1:]:
        sid = sec[:6]
        sm = re.search(r"^- \*\*Status:\*\* (\w+)", sec, re.M)
        oq_status[sid] = sm.group(1) if sm else "?"
    hm = re.search(r"Document status \| Active — (\d+) questions registered: (\d+) OPEN, (\d+) ANSWERED", oq)
    if hm:
        tot, op, an = map(int, hm.groups())
        vals = list(oq_status.values())
        if (tot, op, an) != (len(vals), vals.count("OPEN"), vals.count("ANSWERED")):
            problems.append(
                f"OPEN_QUESTIONS.md header says {tot}/{op} OPEN/{an} ANSWERED, body has "
                f"{len(vals)}/{vals.count('OPEN')} OPEN/{vals.count('ANSWERED')} ANSWERED")
    rk = texts.get("RISKS.md", "")
    hm = re.search(r"Document status \| Active — (\d+) risks", rk)
    if hm and int(hm.group(1)) != len(defined["RISK"]):
        problems.append(f"RISKS.md header says {hm.group(1)} risks, body defines {len(defined['RISK'])}")

    # W3 possibly stale statuses
    settled = {a for a, s in idx.items() if s == "ACCEPTED"} | \
              {o for o, s in oq_status.items() if s == "ANSWERED"}
    for f, t in texts.items():
        if f in ("DEVELOPMENT_LOG.md", "CHANGELOG.md", "REFERENCES.md", "ENGINEERING_RULES.md"):
            continue
        for n, line in section_lines(t, r"^#+ Change history"):
            if SUPERSEDE_MARK.search(line):
                continue
            for uid in settled:
                pos = line.find(uid)
                if pos < 0:
                    continue
                window = line[pos:pos + len(uid) + 40]
                if STALE_WORDS.search(window):
                    warnings.append(f"{f}:{n}: {uid} may be shown with a stale status: "
                                    f"…{window.strip()[:80]}…")

    print(f"defined: facts={len(facts)} topics={''.join(topics)} "
          + " ".join(f"{k}={len(v)}" for k, v in defined.items())
          + f" TEST(canon)={len(canon)}")
    print(f"files={len(files)} distinct facts cited={len(facts & cited)}/{len(facts)}")
    print(f"PROBLEMS ({len(problems)}):")
    for p in problems:
        print("  ", p)
    print(f"WARNINGS ({len(warnings)}):")
    for w in warnings:
        print("  ", w)
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1] if len(sys.argv) > 1 else "docs"))
