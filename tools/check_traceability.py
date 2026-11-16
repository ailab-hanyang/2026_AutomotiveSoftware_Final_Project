#!/usr/bin/env python3
"""Check the submission format and the traceability links. Run from the repo root.

ID formats (Lecture 07, LKS example):
  SYS-001  SWR-001  COMP-01  IF-01  TC-U-001  TC-I-001  TC-S-001  RT-001  AN-001

Errors (exit 1):
  - a required file is missing, or a fixed table header was changed
  - an ID in the wrong format, or a duplicated ID
  - an SWR without a source SYS, a COMP, or a test case
  - a test case that verifies an unknown ID, or whose Result is not Pass / Fail / Not executed
  - a test report row or anomaly that refers to an unknown test case
  - an @req / @verifies tag in code that is not defined in the docs
  - docs/traceability.csv: wrong header, unknown ID, or a Result that differs from the test specification
Warnings: the number of SWRs is outside 10 to 20.
"""
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / "docs"

REQUIRED = [
    "README.md", "CLAUDE.md", "AGENTS.md",
    "docs/00_overview.md", "docs/01_system_requirements.md", "docs/02_sw_requirements.md",
    "docs/03_sw_architecture.md", "docs/04_detailed_design.md", "docs/05_test_specification.md",
    "docs/06_test_report.md", "docs/traceability.csv", "docs/ai_usage_log.md",
    ".github/PULL_REQUEST_TEMPLATE.md",
]

# ID kind -> exact format
ID_FORMAT = {
    "SYS": r"SYS-\d{3}", "SWR": r"SWR-\d{3}", "COMP": r"COMP-\d{2}", "IF": r"IF-\d{2}",
    "TC": r"TC-[UIS]-\d{3}", "RT": r"RT-\d{3}", "AN": r"AN-\d{3}",
}
ID_RE = re.compile(r"\b(?:" + "|".join(ID_FORMAT.values()) + r")\b")
TAG_RE = re.compile(r"@(req|verifies)\s+((?:SWR-\d{3}[\s,]*)+)")
CODE_SUFFIXES = {".cpp", ".hpp", ".h", ".c", ".py", ".xml"}
RESULTS = {"Pass", "Fail", "Not executed"}

# (file, table) -> fixed header; the first column holds the IDs of that table
TABLES = {
    "sys": ("01_system_requirements.md",
            ["ID", 'Requirement ("The vehicle shall …")', "Source (Customer / Team)", "Verification Method",
             "Acceptance Criteria"]),
    "swr": ("02_sw_requirements.md",
            ["ID", "Requirement (17-00)", "Source (SYS-ID)", "Priority", "Verification Method", "Acceptance Criteria"]),
    "comp": ("03_sw_architecture.md",
             ["ID", "Name", "Role", "Units (functions)", "Input", "Output", "Allocated SWR"]),
    "if": ("03_sw_architecture.md",
           ["ID", "Name (topic)", "Producer", "Consumer", "Message Type", "Content and Unit", "Rate"]),
    "tc": ("05_test_specification.md",
           ["ID", "Verifies", "Level", "Input / Scenario", "Pass Criteria", "Result"]),
    "report": ("06_test_report.md",
               ["ID", "Result (Pass / Fail / Not executed)", "Evidence (path)", "Notes"]),
    "anomaly": ("06_test_report.md",
                ["ID", "Test Case", "SWR", "Severity", "Description", "Root Cause", "Action", "Status"]),
}
CSV_HEADER = ["SYS-ID", "SWR-ID", "COMP-ID", "IF-ID", "Unit", "TC-ID", "Result", "Owner"]

errors, warnings = [], []


def cells(line):
    return [c.strip() for c in line.strip().strip("|").split("|")]


def read_table(key):
    """Rows (dict by column name) of the table whose header is TABLES[key]; None if the header is missing."""
    name, header = TABLES[key]
    rows, inside = [], False
    for line in (DOCS / name).read_text(encoding="utf-8").splitlines():
        if not line.lstrip().startswith("|"):
            inside = False
            continue
        row = cells(line)
        if row == header:
            inside = True
        elif inside and not set("".join(row)) <= set("-: "):
            rows.append(dict(zip(header, row + [""] * (len(header) - len(row)))))
    if not any(cells(l) == header for l in (DOCS / name).read_text(encoding="utf-8").splitlines()):
        errors.append(f"docs/{name}: fixed table header changed or missing: | {' | '.join(header)} |")
        return None
    return rows


def ids_in(text, *kinds):
    return {m.group(0) for m in ID_RE.finditer(text) if m.group(0).split("-")[0] in kinds}


def check_ids(rows, name, kinds):
    """Keep rows whose ID has a valid format for one of kinds; report the others."""
    good = []
    for r in rows:
        rid = r["ID"]
        if any(re.fullmatch(ID_FORMAT[k], rid) for k in kinds):
            good.append(r)
        elif rid:
            fmt = " or ".join(ID_FORMAT[k].replace(r"\d{3}", "001").replace(r"\d{2}", "01")
                              .replace("[UIS]", "U") for k in kinds)
            errors.append(f"docs/{name}: '{rid}' has the wrong ID format (expected like {fmt})")
    return good


def main():
    for f in REQUIRED:
        if not (ROOT / f).is_file():
            errors.append(f"missing file: {f}")
    if errors:
        return

    t = {k: read_table(k) for k in TABLES}
    if any(v is None for v in t.values()):
        return
    sys_rows = check_ids(t["sys"], TABLES["sys"][0], ["SYS"])
    swr_rows = check_ids(t["swr"], TABLES["swr"][0], ["SWR"])
    comp_rows = check_ids(t["comp"], TABLES["comp"][0], ["COMP"])
    if_rows = check_ids(t["if"], TABLES["if"][0], ["IF"])
    tc_rows = check_ids(t["tc"], TABLES["tc"][0], ["TC", "RT"])
    report_rows = check_ids(t["report"], TABLES["report"][0], ["TC", "RT"])
    an_rows = check_ids(t["anomaly"], TABLES["anomaly"][0], ["AN"])

    defined = {}
    for rows in (sys_rows, swr_rows, comp_rows, if_rows, tc_rows, an_rows):
        for r in rows:
            if r["ID"] in defined:
                errors.append(f"duplicated ID: {r['ID']}")
            defined[r["ID"]] = r
    sys_ids = {r["ID"] for r in sys_rows}
    swr_ids = {r["ID"] for r in swr_rows}
    comp_ids = {r["ID"] for r in comp_rows}
    if_ids = {r["ID"] for r in if_rows}
    tc_ids = {r["ID"] for r in tc_rows}

    # SYS -> SWR
    for r in swr_rows:
        src = ids_in(r["Source (SYS-ID)"], "SYS")
        if not src:
            errors.append(f"{r['ID']}: no source SYS ID")
        for s in sorted(src - sys_ids):
            errors.append(f"{r['ID']}: source {s} is not defined")

    # SWR -> COMP, interfaces between defined components
    allocated = set()
    for r in comp_rows:
        swrs = ids_in(r["Allocated SWR"], "SWR")
        allocated |= swrs
        for s in sorted(swrs - swr_ids):
            errors.append(f"{r['ID']}: allocated {s} is not defined")
    for r in if_rows:
        for col in ("Producer", "Consumer"):
            for c in sorted(ids_in(r[col], "COMP") - comp_ids):
                errors.append(f"{r['ID']}: {col.lower()} {c} is not defined")

    # SWR -> test case -> result
    verified = set()
    for r in tc_rows:
        targets = ids_in(r["Verifies"], "SWR")
        if not targets:
            errors.append(f"{r['ID']}: verifies no SWR")
        for s in sorted(ids_in(r["Verifies"], "SWR", "COMP", "IF") - swr_ids - comp_ids - if_ids):
            errors.append(f"{r['ID']}: verifies undefined {s}")
        verified |= targets
        if r["Result"] not in RESULTS:
            errors.append(f"{r['ID']}: Result must be Pass, Fail, or Not executed (got '{r['Result']}')")
    for s in sorted(swr_ids):
        if s not in allocated:
            errors.append(f"{s}: not allocated to any COMP")
        if s not in verified:
            errors.append(f"{s}: not verified by any test case")

    # test report and anomalies refer to defined test cases
    for r in report_rows:
        if r["ID"] not in tc_ids:
            errors.append(f"docs/06_test_report.md: result for undefined test case {r['ID']}")
        res = r["Result (Pass / Fail / Not executed)"]
        if res not in RESULTS:
            errors.append(f"docs/06_test_report.md: {r['ID']}: Result must be Pass, Fail, or Not executed")
        elif r["ID"] in defined and res != defined[r["ID"]]["Result"]:
            errors.append(f"{r['ID']}: Result differs between 05_test_specification.md and 06_test_report.md")
    for r in an_rows:
        for c in sorted(ids_in(r["Test Case"], "TC", "RT") - tc_ids):
            errors.append(f"{r['ID']}: test case {c} is not defined")
        for s in sorted(ids_in(r["SWR"], "SWR") - swr_ids):
            errors.append(f"{r['ID']}: {s} is not defined")

    # @req / @verifies tags in code
    for base in ("src", "test"):
        for p in (ROOT / base).rglob("*"):
            if p.suffix in CODE_SUFFIXES and p.is_file():
                for m in TAG_RE.finditer(p.read_text(encoding="utf-8", errors="ignore")):
                    for s in sorted(ids_in(m.group(2), "SWR") - swr_ids):
                        errors.append(f"{p.relative_to(ROOT)}: @{m.group(1)} {s} is not defined in docs")

    # traceability.csv (13-51 Consistency Evidence)
    with open(DOCS / "traceability.csv", newline="", encoding="utf-8") as f:
        rows = list(csv.reader(f))
    if not rows or [c.strip() for c in rows[0]] != CSV_HEADER:
        errors.append("docs/traceability.csv: header must be " + ",".join(CSV_HEADER))
    else:
        for i, row in enumerate(rows[1:], start=2):
            row = dict(zip(CSV_HEADER, [c.strip() for c in row]))
            for col in ("SYS-ID", "SWR-ID", "COMP-ID", "IF-ID", "TC-ID"):
                for m in ID_RE.finditer(row.get(col, "")):
                    if m.group(0) not in defined:
                        errors.append(f"docs/traceability.csv line {i}: {m.group(0)} is not defined")
            tcs = ids_in(row.get("TC-ID", ""), "TC", "RT")
            if len(tcs) == 1 and row.get("Result"):
                tc = next(iter(tcs))
                if tc in defined and row["Result"] != defined[tc]["Result"]:
                    errors.append(f"docs/traceability.csv line {i}: Result of {tc} differs from 05_test_specification.md")

    if swr_ids and not 10 <= len(swr_ids) <= 20:
        warnings.append(f"{len(swr_ids)} SWRs defined (expected 10 to 20)")
    print(f"SYS {len(sys_ids)}, SWR {len(swr_ids)}, COMP {len(comp_ids)}, IF {len(if_ids)}, "
          f"test cases {len(tc_ids)}, anomalies {len(an_rows)}")


if __name__ == "__main__":
    main()
    for w in warnings:
        print("WARNING:", w)
    for e in errors:
        print("ERROR:", e)
    print("FAILED" if errors else "OK")
    sys.exit(1 if errors else 0)
