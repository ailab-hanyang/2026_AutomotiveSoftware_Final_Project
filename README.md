# Team XX: Lane Keeping on the Map Easy (Example)

AUE4039 Automotive Software, Final Project (2026 Fall)

> Worked example of Lecture 07 (lane keeping, LKS). Replace it with your own function, documents, code, tests, and results;
> keep the table headers, rules, and folders of `CLAUDE.md`.

## Team
| Student ID | Name | Role |
|---|---|---|
| 2026XXXXXX | Member A | perception (COMP-01) |
| 2026XXXXXX | Member B | control (COMP-02) |
| 2026XXXXXX | Member C | tests and documents |

## Topic
Lane keeping on the map easy: the vehicle follows the center of its lane at 10 m/s.
The perception node fits the lane points to the driving way, and the control node follows it (pure pursuit) at the target speed.
Details in [docs/00_overview.md](docs/00_overview.md).

## Build and Run
Requires Ubuntu 22.04 on x86_64 (Intel/AMD PC or WSL2) and ROS 2 Humble. Apple Silicon Macs are not supported.
```bash
source /opt/ros/humble/setup.bash
colcon build
./launch.sh          # map (MAP: easy, normal, hard) and mission switches at the top of launch.sh
```

## Test
```bash
colcon test && colcon test-result --verbose
python3 tools/check_traceability.py
```

## Result Summary
<!-- Key results per system-level TC with links to results/. -->
| TC-ID | Result | Evidence |
|---|---|---|
| TC-S-001 | Fail: one period over 10 ms in run 3 (AN-001); lane keeping, speed, and commands met in 3 of 3 runs | [results/TC-S-001](results/TC-S-001/) |
| TC-S-002 | Pass | [results/TC-S-002](results/TC-S-002/test_result.txt) |

## Documents
- [Overview](docs/00_overview.md)
- [System Requirements](docs/01_system_requirements.md)
- [SW Requirements](docs/02_sw_requirements.md)
- [SW Architecture](docs/03_sw_architecture.md)
- [Detailed Design](docs/04_detailed_design.md)
- [Test Specification](docs/05_test_specification.md)
- [Test Report](docs/06_test_report.md)
- [Traceability](docs/traceability.csv)
- [AI Usage Log](docs/ai_usage_log.md)
