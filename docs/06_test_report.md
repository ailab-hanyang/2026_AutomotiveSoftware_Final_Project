# 06. Test Report

## Environment
Ubuntu 22.04 (WSL2), ROS 2 Humble.
- TC-U, TC-I: `colcon test` from the repository root (no simulator).
- TC-S: the nodes of `launch.sh` with the map and switches of `Input / Scenario`, started by a script without RViz;
  after each run `tools/save_evaluation.py` saved the evaluation result, and `ros2 bag record` the rosbag
  (`/ego/vehicle_state`, `/ego/track_end`). Rosbag criteria checked with `test/scenario/check_rosbag.py`.

## Summary
| Release (tag) | Commit | Passed | Failed | Not executed |
|---|---|---|---|---|
| final | 74096a5 (example, not tagged) | 8 | 0 | 0 |

## Results
Raw data: gtest logs, rosbag files, evaluation logs, and driving videos in `results/<test case ID>/`.
Each result is the same as the `Result` in `05_test_specification.md`.

| ID | Result (Pass / Fail / Not executed) | Evidence (path) | Notes |
|---|---|---|---|
| TC-U-001 | Pass | `results/TC-U-001/test_result.txt` | 2 gtest cases: straight and curved lane |
| TC-U-002 | Pass | `results/TC-U-002/test_result.txt` | 3 gtest cases: normal, boundary, negative |
| TC-U-003 | Pass | `results/TC-U-003/test_result.txt` | 2 gtest cases |
| TC-U-004 | Pass | `results/TC-U-004/test_result.txt` | 1 gtest case |
| TC-I-001 | Pass | `results/TC-I-001/test_result.txt` | test_tc_i_001_lane_center_to_command |
| TC-I-002 | Pass | `results/TC-I-002/test_result.txt` | test_tc_i_002_publishes_command |
| TC-I-003 | Pass | `results/TC-I-003/test_result.txt` | longest computation time: perception 0.491 ms, control 0.128 ms (period 10 ms) |
| TC-S-001 | Pass | `results/TC-S-001/` | Max Cross Track Error 0.325, 0.331, 0.360 m; speed 9.89 m/s from 10 s to the track end in all 3 runs |

## Anomalies
One row per failure. Severity: Critical / Major / Minor. Status: Open / In progress / Closed.

<!-- Example: no test case failed in the final runs, so the table is empty. -->

| ID | Test Case | SWR | Severity | Description | Root Cause | Action | Status |
|---|---|---|---|---|---|---|---|

## Review
| Release (tag) | Pull Request | Reviewed by | Shared on |
|---|---|---|---|
