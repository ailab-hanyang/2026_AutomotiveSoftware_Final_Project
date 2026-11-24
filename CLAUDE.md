# CLAUDE.md

Project instructions for AI coding agents (Claude Code reads `CLAUDE.md`, Codex reads `AGENTS.md`; keep the two files identical).
This repository is a team submission for the final project of AUE4039 Automotive Software (Hanyang University).
The team develops one function on top of the provided ROS 2 simulator, following the V-process.
Target: Automotive SPICE (PAM 4.1) capability Level 1 for SWE.1 to SWE.6: every base practice done and every output information item present in this repository (Lecture 07).

## Who does what
- Students decide and own: system and software requirements, architecture (components and interfaces), test cases, judging test results, failure analysis.
- The agent may: draft and polish documents, write the detailed design, write code and test code, run builds and tests.
- Never finalize a requirement, acceptance criterion, or test verdict on your own. Propose it and let a student approve it.
- Every PR adds 3 to 5 lines to `docs/ai_usage_log.md`: what was delegated and which errors were found in the AI output.

## Environment
- Ubuntu 22.04 on x86_64 (Intel/AMD PC or WSL2), ROS 2 Humble, C++14, colcon. Apple Silicon Macs are not supported.
- Build from the repository root:
  ```bash
  source /opt/ros/humble/setup.bash
  colcon build
  ```
- Run (opens 4 terminals with `terminator`): `./launch.sh`
  The order is simulator, mission_manager, evaluation, autonomous_driving.
  The map (`MAP`: `easy`, `normal`, `hard`) and the mission switches are set at the top of `launch.sh` (see "Maps and missions").
- Test: `colcon test && colcon test-result --verbose`
- Format and traceability check: `python3 tools/check_traceability.py`
  Run it before every commit.

## Repository layout
```
src/app/simulator/            provided: vehicle model, lane detection, RViz display   (do not modify)
src/app/mission_manager/      provided: mission scenario (objects, ice, slope, school zone, parking)   (do not modify)
src/app/evaluation/           provided: judges collision, lane departure, time, over-speed, parking   (do not modify)
src/app/autonomous_driving/   team code starts here; more team packages may be added under src/app/
src/bsw/system/               provided interface structs and ROS 2 bridges   (do not modify; add new files instead)
src/rte/msg/ad_msgs/          message definitions   (do not change existing messages; adding new ones is allowed)
resources/                    lane CSV files per map (resources/csv/<map>/), meshes, RViz config
docs/                         V-process documents (fixed format, see below)
test/unit/                    SWE.4 unit tests, gtest (TC-U-xxx)
test/integration/             SWE.5 integration tests, launch_testing (TC-I-xxx)
test/scenario/                SWE.6 software tests (TC-S-xxx): simulator scenarios, and the whole software with injected inputs
results/                      verification data per test case (results/TC-S-001/): logs, rosbag, evaluation logs, videos
tools/check_traceability.py   format and traceability check
tools/save_evaluation.py      saves the evaluation result of one scenario run (TC-S)
```
- The `.a` files in the provided packages are prebuilt libraries. Treat them as a black box: do not modify, replace, or decompile them.
  They are built for x86_64 only (`<name>.x86_64.a`).
- Tests must belong to an ament_cmake package so that `colcon test` finds them (colcon scans the whole repository, including `test/`).

## Interfaces of the provided simulator (namespace `ego`)
| Topic | Type | Direction (from team code) |
|---|---|---|
| `/manual_input` | `ad_msgs/msg/VehicleCommand` | subscribe (manual driving only, global name) |
| `vehicle_state` | `ad_msgs/msg/VehicleState` | subscribe |
| `lane_points` | `ad_msgs/msg/LanePointData` | subscribe |
| `mission` | `ad_msgs/msg/Mission` | subscribe |
| `vehicle_command` | `ad_msgs/msg/VehicleCommand` | publish |
| `driving_way` | `ad_msgs/msg/PolyfitLaneData` | publish (visualization) |
| `poly_lanes` | `ad_msgs/msg/PolyfitLaneDataArray` | publish (visualization) |

Read the `.msg` files in `src/rte/msg/ad_msgs/msg/` for fields and units before using a topic.
Subscribe only to the topics in this table. The simulator also publishes `ROI_lanes` and `csv_lanes` for the RViz display;
team code must not use them: the lanes are detected from `lane_points` by the team.

## Maps and missions
Three maps (3 lanes, 4 m wide, start at the origin heading +x): `easy` (875 m), `normal` (1065 m), `hard` (1100 m).
The positions and parameters of the missions are fixed per map. A team only chooses the map and which missions are on:

| Switch (`launch.sh`) | What happens | Seen in |
|---|---|---|
| `ICE` | an icy region: lower lateral tire force | `mission.road_condition == "Ice"` inside the region |
| `SLOPE` | an uphill and a downhill region (10 % grade) | `mission.road_slope` (`"Up"`, `"Down"`) inside the region |
| `SCHOOL_ZONE` | a 30 kph school zone; over-speed inside it is penalized | `mission.speed_limit` [m/s] drops from 50 m before the zone to its end |
| `SCC` | three vehicles ahead (one per lane) that speed up from 30 to 50 kph (60 kph on `hard`) | `mission.objects` (within 20 m) |
| `MERGE` | the center lane and one side lane are closed; vehicles drive in the open lane; the open side (left or right) is random on every start | `mission.objects` (within 20 m) |
| `PARKING` | after the end of the track, stop in one of the 3 parking slots for 3 s | `mission.parking` becomes true at the track end |
| `LANE_LOSS` | lane markings are worn away in some sections (also not drawn in RViz) | no `lane_points` there |

Fixed for every team (not configurable): lane detection ROI (front 20 m, rear 5 m, left and right 4 m, noise 0.1 m),
detection range of mission objects (20 m), lane departure threshold (4.5 m), time limit (`easy` 180 s, `normal` 230 s, `hard` 250 s).
Every simulator scenario (`TC-S`) states the map and the switches in `Input / Scenario`, so that it can be re-run the same way.

## Units, tests, and evidence (SWE.3 to SWE.6)
### What each test level verifies
Each level checks the work product of its own process (Lecture 07). The `Verifies` column of `docs/05` and the
`@verifies` tag of the test name the same items.

| Level | Process | Verifies | Coverage (checked by `tools/check_traceability.py`) |
|---|---|---|---|
| `TC-U` unit test | SWE.4 | units of `docs/04`, by name | every unit has a TC-U |
| `TC-I` integration test | SWE.5 | components and interfaces of `docs/03` (`COMP-01`, `IF-01`) | every COMP and IF has a TC-I |
| `TC-S` software test | SWE.6 | software requirements of `docs/02` (`SWR-001`) | every SWR has a TC-S |

So the Verification Method of an SWR is always a software test on the whole software: a simulator scenario,
data recorded in a scenario run, or the whole software with injected inputs. Unit and integration tests check the design.

### Units (SWE.3)
- A unit is one function listed in `docs/04_detailed_design.md`, with `// @req SWR-xxx` on the line above it.
- Keep units free of ROS code (e.g. `src/<node>/<node>_algorithm.cpp`, one folder per node as in the provided `simulator` package),
  so that a gtest can call them without ROS.
  The node file only subscribes, calls the units, and publishes.
- Put the default parameter values in one config header; the node declares its ROS parameters with these values,
  and `docs/04` lists each parameter with its value, unit, and file.
- `docs/04` gives the valid range and the physical unit of every input and output of a unit, and its preconditions.

### Registering tests (SWE.4 to SWE.6)
A test file in `test/` runs only if a team package registers it. In the `CMakeLists.txt` of that package:
```cmake
if(BUILD_TESTING)
  set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)    # repository root, for a package in src/app/<package>
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_lane_detection ${REPO_ROOT}/test/unit/test_lane_detection.cpp)
  target_link_libraries(test_lane_detection lane_detection_algorithm)   # library with the units under test
  find_package(launch_testing_ament_cmake REQUIRED)
  add_launch_test(${REPO_ROOT}/test/integration/test_control_io.py TARGET test_control_io TIMEOUT 60)
endif()
```
In `package.xml`: `<test_depend>` for `ament_cmake_gtest`, `launch_testing_ament_cmake`, `launch_ros`, and `rclpy`.
After `colcon test`, check that every test case appears in `colcon test-result --verbose`:
a test file that is not registered never runs and never fails.
- Unit tests (gtest) call one unit with input values: normal, boundary, and invalid inputs.
- Integration tests (launch_testing) start the nodes under test without the simulator,
  publish fake inputs from the test, and check the output topics.

### Software tests (SWE.6)
- A simulator scenario: run it 3 times with the map and switches of its `Input / Scenario`. It passes only if all 3 runs pass.
- A test of the whole software with injected inputs (all team nodes started by launch_testing, fake simulator topics)
  covers what the simulator cannot create, e.g. an invalid speed (negative test). Keep it in `test/scenario/`
  and register it with `add_launch_test`.
- After each run, while the evaluation node is still running: `python3 tools/save_evaluation.py TC-S-001`
  saves `results/TC-S-001/run_<n>.txt` with the time and the git commit.
- A criterion the evaluation node does not judge (e.g. speed) needs recorded data:
  `ros2 bag record -o results/TC-S-001/rosbag/run_<n> <topics>`, checked by a script kept in `test/scenario/`.

### Evidence and regression
- `results/<test case ID>/` holds the evidence of each test case: for TC-U and TC-I the lines of
  `colcon test-result --verbose`, with the date and the git commit; for TC-S the files above.
  `docs/06_test_report.md` cites the path.
- After a change to code or parameters that a passed test case covers, re-run the affected test cases,
  add an `RT-xxx` row to `docs/05` that names them and the change, and update the evidence and the results.

## V-process documents
| File | Content | IDs |
|---|---|---|
| `docs/00_overview.md` | team, topic, operational scenario, scope | |
| `docs/01_system_requirements.md` | system requirements written by the team | `SYS-001` |
| `docs/02_sw_requirements.md` | SWE.1: requirements and attributes, analysis results, review, change history; 10 to 20 SWRs | `SWR-001` |
| `docs/03_sw_architecture.md` | SWE.2: components, interfaces, dynamic aspects, mermaid diagram, analysis results | `COMP-01`, `IF-01` |
| `docs/04_detailed_design.md` | SWE.3: units per component, data formats and units, parameters, coding principles | |
| `docs/05_test_specification.md` | SWE.4 to SWE.6: verification measures, integration sequence, selection set | `TC-U-001`, `TC-I-001`, `TC-S-001`, `RT-001` |
| `docs/06_test_report.md` | verification results: summary per release, results, anomalies | `AN-001` |
| `docs/traceability.csv` | consistency evidence: SYS, SWR, COMP, IF, unit, test case, result, owner per row | |
| `docs/ai_usage_log.md` | AI usage per PR | |

Where each process lives and which gate covers it (Lecture 07):
| Process | Question | Files | Gate |
|---|---|---|---|
| SWE.1 | What must the software do? | `docs/02_sw_requirements.md`, `docs/traceability.csv` | `req-v1` |
| SWE.2 | Which components, which interfaces? | `docs/03_sw_architecture.md` | `req-v1` |
| SWE.3 | How does each unit work? | `docs/04_detailed_design.md`, `src/` (`// @req`) | `impl-v1` |
| SWE.4 | Does each unit match its design? | `test/unit`, `results/` | `impl-v1` |
| SWE.5 | Do the components work together? | `test/integration`, `results/` | `final` |
| SWE.6 | Does the software meet the SWRs? | `docs/05_test_specification.md`, `docs/06_test_report.md`, `test/scenario` | `final` |

Rules:
- ID formats: `SYS-001`, `SWR-001`, `COMP-01`, `IF-01`, `TC-U-001` (unit), `TC-I-001` (integration), `TC-S-001` (software test), `RT-001` (regression), `AN-001` (anomaly).
  Never reuse or renumber an ID; mark removed items as removed instead.
- Do not change the table headers. The check script and the grading tools parse them.
- Every requirement is testable: write a quantitative acceptance criterion with units.
- Every SWR has a source SYS, at least one COMP in `Allocated SWR`, and at least one TC-S that verifies it.
  Every unit in `docs/04` has a TC-U, and every COMP and IF in `docs/03` has a TC-I.
- Every test case has a `Result`: `Pass`, `Fail`, or `Not executed`. Once it is run, `docs/06_test_report.md` gives the same result and an evidence path.
- Every failure gets an anomaly (`AN-xxx`) with root cause, action, and status in `docs/06_test_report.md`.
- Reviews are Pull Requests merged before the gate tag; link them in the `Review` sections.
- Each SWR has one owner (a team member) in `docs/traceability.csv`.
- After Gate 1 (`req-v1`), any requirement change gets a line in the change history of `02_sw_requirements.md`.
- Write all documents in English.

## Traceability tags
- Code that implements a requirement: `// @req SWR-012` on the line above the function.
- Tests: `// @verifies` (`# @verifies` in Python) with what the test case verifies: the unit name for a TC-U
  (`// @verifies LaneDetection`), COMP and IF IDs for a TC-I (`# @verifies COMP-02, IF-04`), SWR IDs for a TC-S (`# @verifies SWR-012`);
  and the test case ID in the test name, e.g. `TEST(LaneDetection, TC_U_003_RightLaneOnly)`.
- Commits: start the message with `[SWR-012]` or a test case ID such as `[TC-U-003]`.
- PRs: fill in `.github/PULL_REQUEST_TEMPLATE.md`; the related SWR and test case IDs are required.

## Before you finish a change
1. `colcon build` passes without new warnings in team packages.
2. `colcon test` passes, and every test case in `docs/05` that runs with `colcon test` appears in `colcon test-result --verbose`.
3. `python3 tools/check_traceability.py` prints `OK`.
4. Docs, tags, and `docs/traceability.csv` match the code (the same result for a test case everywhere).

## Milestones (git tags)
| Tag | Contents |
|---|---|
| `req-v1` | SYS, SWR, architecture, test specification drafts; format check passes |
| `impl-v1` | build passes, traceability tags, some unit tests |
| `final` | everything; due 12/10 23:59; the repository is frozen after this tag |

`req-v1` and `impl-v1` are baselines for feedback. They are not graded and have no submission deadline.

Push a tag with `git tag final && git push origin final`. Before tagging, check that the build, the tests, and the format check pass.
