# 00. Overview

> Worked example: the lane keeping example (LKS) of Lecture 07. Replace this document with your own.
> The two nodes of Lecture 07 (perception and control) are implemented with simple units (map easy only),
> and every test case was run.

## Team
| Student ID | Name | Role | Owned SWRs |
|---|---|---|---|
| 2026XXXXXX | Member A | perception (COMP-01) | SWR-003 |
| 2026XXXXXX | Member B | control (COMP-02) | SWR-001, SWR-002 |
| 2026XXXXXX | Member C | tests and documents | SWR-004 |

## Topic
Lane keeping (LKS) on the map easy: the vehicle follows the center of its lane at a target speed of 10 m/s.
This is the running example of Lecture 07.

## Operational Scenario
- Map `easy`, all mission switches off: drive to the end of the track within the lane.
- Success: the acceptance criteria of `01_system_requirements.md`, judged by the evaluation node and recorded data.

## Scope
### In scope
- Map `easy`: driving way (lane center line) from `lane_points`, lateral control, speed control, command range limit.
- Manual driving (`manual:=true`: COMP-02 passes `/manual_input` through) for checking the simulator only.

### Out of scope
- Maps `normal` and `hard`: not tested; on tight curves, splitting the lane points by the sign of y can put points on the wrong lane.
- `ICE`, `SLOPE`, `SCHOOL_ZONE`, `SCC`, `MERGE`, `PARKING`, `LANE_LOSS`: not part of this topic.

## Simulator Features Used
| Feature | Topic / Source | Used for |
|---|---|---|
| Vehicle state | `vehicle_state` (IF-01) | speed for control |
| Lane points | `lane_points` (IF-02) | lane fitting and the driving way |
| Evaluation result | evaluation node | TC-S-001 |
| Rosbag | `ros2 bag record` | TC-S-001 (speed, computation time, commands) |
