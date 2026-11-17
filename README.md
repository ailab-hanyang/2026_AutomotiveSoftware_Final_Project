# Team XX: <Project Title>

AUE4039 Automotive Software, Final Project (2026 Fall)

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
