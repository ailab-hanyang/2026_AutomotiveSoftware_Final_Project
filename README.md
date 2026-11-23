# Team XX: <Project Title>

AUE4039 Automotive Software, Final Project (2026 Fall)

> **Worked example (replace it with your own function).** The template comes filled in with the lane keeping example (LKS)
> of Lecture 07, on the map easy only. Read it, then replace it with your function: your IDs, documents, nodes, tests, and results.
> Keep the table headers, rules, and folders (`CLAUDE.md`).
>
> | Where | What the example shows |
> |---|---|
> | `docs/` | every document filled in for LKS, with the results of every test case |
> | `src/app/autonomous_driving/` | two nodes added to the package: `perception` (COMP-01) and `control` (COMP-02), each with simple units in `<node>_algorithm.cpp` (`// @req`); the provided node `autonomous_driving` is kept for manual driving |
> | `launch/autonomous_driving.launch.xml` | starts perception and control; `manual:=true` starts the provided node instead |
> | `src/rte/msg/ad_msgs/msg/ComputationTime.msg` | a message added by the team (IF-05, IF-06); existing messages stay unchanged |
> | `test/unit/`, `test/integration/` | gtest (TC-U-001 to TC-U-004) and launch_testing (TC-I-001 to TC-I-003) |
> | `CMakeLists.txt`, `package.xml` of autonomous_driving | how the nodes and the tests are registered |
> | `results/` | evidence of every test case: test output, evaluation results, rosbags |
> | `tools/save_evaluation.py` | helper that saves the evaluation result of one scenario run |

## Team
| Student ID | Name | Role |
|---|---|---|

## Topic
<!-- One paragraph: what the function does and in which situation. Details in docs/00_overview.md. -->

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
