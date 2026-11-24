# AI Usage Log

Add 3 to 5 lines per PR: what you delegated, and the errors you found in the AI output.

| Date | PR | Tool (Claude Code / Codex) | Delegated Task | Errors Found in AI Output | Action Taken |
|---|---|---|---|---|---|
| 2026-11-23 | #1 (example) | Claude Code | Write the perception and control nodes, their units, and the gtest and launch_testing tests from docs/04 | The first draft named the units differently from the names the team agreed on (FindDrivingWay, LateralControl, LongitudinalControl) | Renamed the units and their tests; the team reviewed the change |
| 2026-11-23 | #2 (example) | Claude Code | Port FindDrivingWay (lane fitting) and LateralControl (pure pursuit) of Practice 11 to the units | None found in the port | The team checked the port with TC-U-001, TC-U-004, and TC-S-001 |
