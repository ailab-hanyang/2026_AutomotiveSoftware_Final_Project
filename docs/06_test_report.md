# 06. Test Report

## Environment
Ubuntu 22.04 (WSL2), ROS 2 Humble. All test cases were run on commit 2509c63.
- TC-U, TC-I, TC-S-002: `colcon test` from the repository root (no simulator).
- TC-S-001: the nodes of `launch.sh` with the map and switches of `Input / Scenario`, started by a script without RViz;
  after each run `tools/save_evaluation.py` saved the evaluation result, and `ros2 bag record` the rosbag
  (`/ego/vehicle_state`, `/ego/track_end`, `/ego/vehicle_command`, `/ego/computation_time/perception`, `/ego/computation_time/control`).
  Rosbag criteria checked with `test/scenario/check_rosbag.py`.

## Summary
| Release (tag) | Commit | Passed | Failed | Not executed |
|---|---|---|---|---|
| final | 2509c63 (example, not tagged) | 10 | 0 | 0 |

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
| TC-I-003 | Pass | `results/TC-I-003/test_result.txt` | 1000 messages per node; longest computation time: perception 0.538 ms, control 0.427 ms (period 10 ms) |
| TC-I-004 | Pass | `results/TC-I-004/test_result.txt` | test_tc_i_004_driving_way_from_lane_points |
| TC-S-001 | Pass | `results/TC-S-001/` (`run_1.txt` to `run_3.txt`, `rosbag/`, `rosbag_check.txt`) | Max Cross Track Error 0.330, 0.330, 0.326 m; speed 9.89 m/s from 10 s to the track end; longest computation time perception 6.213, 5.078, 8.264 ms and control 3.167, 1.268, 7.436 ms (period 10 ms); accel and brake in [0, 1] in all commands; in all 3 runs |
| TC-S-002 | Pass | `results/TC-S-002/test_result.txt` | test_tc_s_002_no_command_with_invalid_speed |

Note: in run 3 of TC-S-001 the longest computation time of perception (8.264 ms) left 1.7 ms of the 10 ms period.
The criterion of SWR-003 is met; the margin is small and is watched in the next runs.

## Anomalies
One row per failure. Severity: Critical / Major / Minor. Status: Open / In progress / Closed.

<!-- Example: no test case failed in the final runs, so the table is empty. -->

| ID | Test Case | SWR | Severity | Description | Root Cause | Action | Status |
|---|---|---|---|---|---|---|---|

## Review
| Release (tag) | Pull Request | Reviewed by | Shared on |
|---|---|---|---|
