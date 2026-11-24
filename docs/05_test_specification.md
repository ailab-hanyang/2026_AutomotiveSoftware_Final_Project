# 05. Test Specification (SWE.4 to SWE.6)

## Verification Measures
Each level verifies the work product of its process. `Verifies` names those items:

| Level | ID | Folder | Process | Verifies |
|---|---|---|---|---|
| Unit | `TC-U-xxx` | `test/unit` (gtest) | SWE.4 | units of `04_detailed_design.md`, by name; every unit has a TC-U |
| Integration | `TC-I-xxx` | `test/integration` (launch_testing) | SWE.5 | COMP and IF IDs of `03_sw_architecture.md`; every COMP and IF has a TC-I |
| Software | `TC-S-xxx` | `test/scenario` (simulator scenario, or the whole software with injected inputs) | SWE.6 | SWR IDs of `02_sw_requirements.md`; every SWR has a TC-S |
| Regression | `RT-xxx` | re-run of an earlier test case | SWE.4 to SWE.6 | what the re-run test cases verify |

Result is `Pass`, `Fail`, or `Not executed` (write `Not executed` until the test is run).
Simulator scenarios (`TC-S`) state the map and the mission switches turned on in `Input / Scenario`.
Regression tests (`RT`) state the re-run test case and the change, e.g. `TC-S-001 again after changing GenerateDrivingWay`.

<!-- Example: TC-U and TC-I run with colcon test (test/unit, test/integration).
     TC-S were run with ./launch.sh settings (map and switches below); every TC-S is run 3 times and passes only if all 3 runs pass. -->

| ID | Verifies | Level | Input / Scenario | Pass Criteria | Result |
|---|---|---|---|---|---|
| TC-U-001 | FindDrivingWay | Unit | lane points every 1 m from x = -5 to 20 m, no noise: straight markings at y = 2.5 and -1.5 m; curved markings at y = ±2 + 0.005 x² m | straight: a0 0.5 m (±0.1 m), a1 0 (±0.01); curved: a0 0 m (±0.1 m), a2 0.005 1/m (±0.001) | Pass |
| TC-U-002 | LongitudinalControl | Unit | (speed, target speed) = (9, 10), (0, 10), (10, 0), (NaN, 10) m/s | (accel, brake) = (0.3, 0), (1, 0), (0, 1), (0, 0) | Pass |
| TC-U-003 | LongitudinalControl | Unit | speed 5 m/s and 15 m/s, target 10 m/s | accel > 0 and brake 0 below the target; accel 0 and brake > 0 above it | Pass |
| TC-U-004 | LateralControl | Unit | straight driving way with a0 = 0, 1, -1 m | steering 0, > 0, < 0 | Pass |
| TC-I-001 | COMP-01, COMP-02, IF-01, IF-02, IF-03, IF-04 | Integration | perception and control started; fake IF-01 (10 m/s) and IF-02 (markings at ±2 m) at 30 Hz for 5 s | ≥ 10 messages on IF-03 and on IF-04; last IF-03 a0 within ±0.1 m; last steering within ±0.01 rad | Pass |
| TC-I-002 | COMP-02, IF-01, IF-03, IF-04 | Integration | control alone; 1 s with fake IF-01 only, then fake IF-01 and IF-03 until 10 messages arrive on IF-04 (at most 5 s) | no IF-04 while IF-03 is missing; then ≥ 10 messages on IF-04, accel and brake in [0, 1] | Pass |
| TC-I-003 | COMP-01, COMP-02, IF-05, IF-06 | Integration | perception and control with fake IF-01 and IF-02 for 10 s; computation time from IF-05 and IF-06 | ≥ 500 messages per node (period 10 ms); in every message, computation time ≤ period | Pass |
| TC-I-004 | COMP-01, IF-02, IF-03 | Integration | perception alone; 1 s without input, then fake IF-02 (markings at ±2 m) at 30 Hz for 2 s | no IF-03 before IF-02 arrives; then ≥ 100 messages on IF-03 (100 Hz), last a0 within ±0.1 m | Pass |
| TC-S-001 | SWR-001, SWR-002, SWR-003, SWR-004 | Software | map easy, all mission switches false; rosbag of IF-01, IF-04, IF-05, IF-06 | `Lane Departure: SUCCESS` and `Max Cross Track Error` ≤ 0.5 m; speed 10 ± 1 m/s from 10 s to the track end; computation time ≤ period and accel, brake in [0, 1] in every message; in all 3 runs | Pass |
| TC-S-002 | SWR-004 | Software | perception and control started; fake IF-02 (markings at ±2 m) and IF-01 with velocity NaN at 30 Hz for 3 s (negative test) | ≥ 10 messages on IF-04; accel 0 and brake 0 in every message | Pass |

## Integration Sequence
1. TC-I-004: perception alone with fake inputs.
2. TC-I-002: control alone with fake inputs.
3. TC-I-001: perception and control together.
4. TC-I-003: computation time of both nodes.

## Selection Set
| Release (tag) | Test cases run | Kept for regression |
|---|---|---|
| final | TC-U-001 to TC-U-004, TC-I-001 to TC-I-004, TC-S-001, TC-S-002 | TC-U-001 to TC-U-004, TC-I-001 to TC-I-004, TC-S-002 |
