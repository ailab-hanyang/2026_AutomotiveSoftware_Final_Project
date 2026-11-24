# 02. Software Requirements (SWE.1)

Keep 10 to 20 SWRs. Each SWR traces to at least one SYS (`traceability.csv`).

## Requirements and Attributes
One verifiable statement per row. Priority: High / Medium / Low.
Verification Method: a software test (SWE.6, `TC-S`) on the whole software, e.g. Scenario test (evaluation node), Scenario test (rosbag),
Software test (injected inputs). Unit and integration tests verify the design (`docs/04`, `docs/03`), not the SWRs.

<!-- Example: 4 SWRs to keep the example short. A team writes 10 to 20 (the check script warns otherwise). -->

| ID | Requirement | Source (SYS-ID) | Priority | Verification Method | Acceptance Criteria |
|---|---|---|---|---|---|
| SWR-001 | The software shall keep the vehicle within its lane. | SYS-001 | High | Scenario test (evaluation node) | `Lane Departure: SUCCESS` and `Max Cross Track Error` ≤ 0.5 m on the map easy |
| SWR-002 | The software shall keep the target speed. | SYS-001 | Medium | Scenario test (rosbag) | Speed within 10 ± 1 m/s from 10 s after the start to the end of the track, on the map easy |
| SWR-003 | The software shall finish every periodic computation within its period. | SYS-001 | High | Scenario test (rosbag) | For each component, computation time ≤ period in every period of a run on the map easy (`03_sw_architecture.md`, Dynamic Aspects) |
| SWR-004 | The software shall keep the accel and brake commands within [0, 1]. | SYS-001 | High | Scenario test (rosbag), Software test (injected inputs) | accel and brake in [0, 1] in every vehicle command of a run on the map easy; accel and brake 0 while the vehicle speed is invalid (NaN) |

## Analysis Results
### Feasibility and correctness
- SYS-001 needs lane keeping (SWR-001) at a steady speed (SWR-002), on time (SWR-003),
  with accel and brake commands the vehicle model accepts (SWR-004).
- SWR-003 is Non-functional (a property of every component). The other SWRs are Functional (each is produced by specific units).
- Every SWR is verified on the whole software: SWR-001 by the evaluation node, SWR-002 to SWR-004 by data recorded in the same run;
  an invalid speed cannot be created in the simulator, so SWR-004 also has a test with injected inputs.
- All inputs come from the provided topics `vehicle_state` and `lane_points`.

### Assumptions
- Lane points are detected up to 20 m ahead with 0.1 m noise (fixed by the simulator).
- The vehicle model expects accel and brake in [0, 1].

### Risks
- Only the map easy is in scope. On the tight curves of `normal` and `hard`, splitting the lane points by the sign of y can put points on the wrong lane.
- Lane loss (`LANE_LOSS`) is not handled: without lane points the driving way is not valid. It is out of scope (`00_overview.md`).

## Review
Requirements are reviewed in a Pull Request and merged before the tag `req-v1`.

| Gate | Pull Request | Reviewed by | Merged on |
|---|---|---|---|

## Change History
Record every change made after Gate 1 (`req-v1`).

| Date | ID | Change | Reason |
|---|---|---|---|
