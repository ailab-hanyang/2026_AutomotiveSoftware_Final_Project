# 06. Test Report

## Environment
Ubuntu 22.04 (WSL2), ROS 2 Humble. All test cases were run on commit 05fe1e7.
- TC-U, TC-I, TC-S-002: `colcon test` from the repository root (no simulator).
- TC-S-001: the nodes of `launch.sh` with the map and switches of `Input / Scenario`, started by a script without RViz;
  after each run `tools/save_evaluation.py` saved the evaluation result, and `ros2 bag record` the rosbag
  (`/ego/vehicle_state`, `/ego/track_end`, `/ego/vehicle_command`, `/ego/computation_time/perception`, `/ego/computation_time/control`).
  Rosbag criteria checked with `test/scenario/check_rosbag.py`.

## Summary
| Release (tag) | Commit | Passed | Failed | Not executed |
|---|---|---|---|---|
| final | 05fe1e7 (example, not tagged) | 9 | 1 | 0 |

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
| TC-I-003 | Pass | `results/TC-I-003/test_result.txt` | 1102 messages per node; longest computation time: perception 0.457 ms, control 0.271 ms (period 10 ms) |
| TC-I-004 | Pass | `results/TC-I-004/test_result.txt` | test_tc_i_004_driving_way_from_lane_points |
| TC-S-001 | Fail | `results/TC-S-001/` (`run_1.txt` to `run_3.txt`, `rosbag/`, `rosbag_check.txt`) | Lane Departure: SUCCESS and Max Cross Track Error 0.335, 0.322, 0.328 m; speed 9.89 m/s; accel and brake in [0, 1]: met in all 3 runs. Computation time: one period of perception in run 3 took 12.848 ms (period 10 ms); AN-001 |
| TC-S-002 | Pass | `results/TC-S-002/test_result.txt` | test_tc_s_002_no_command_with_invalid_speed |

## Anomalies
One row per failure. Severity: Critical / Major / Minor. Status: Open / In progress / Closed.

| ID | Test Case | SWR | Severity | Description | Root Cause | Action | Status |
|---|---|---|---|---|---|---|---|
| AN-001 | TC-S-001 | SWR-003 | Minor | Run 3: one period of perception took 12.848 ms at 49.00 s after the start (period 10 ms); its other 9312 periods took at most 2.1 ms. Runs 1 and 2: every period within 10 ms. In a pre-release run on 950de15, both nodes took about 325 ms in the same period (`results/TC-S-001/AN-001_pre-release_950de15.txt`) | Not found in the code. The computation time is measured as elapsed time (`steady_clock`), so it also counts the time the operating system does not run the node; two separate processes stopping for the same 325 ms points to the scheduling of the PC (WSL2, not real-time). Not confirmed | Planned: record the CPU time of each period next to the elapsed time to separate computation from operating-system delays; then TC-S-001 again as a regression test | Open |

## Review
| Release (tag) | Pull Request | Reviewed by | Shared on |
|---|---|---|---|
