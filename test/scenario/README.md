# Software tests (SWE.6)
Test case IDs `TC-S-xxx`. A software test verifies SWR IDs of `docs/02` on the whole software; tag it with `# @verifies SWR-xxx`.
- Simulator scenario judged by the evaluation node: run it 3 times; after each run, save the evaluation result with
  `python3 tools/save_evaluation.py <test case ID>` (`results/<test case ID>/run_<n>.txt`).
  A criterion the evaluation node does not judge needs a rosbag of the run, checked by a script kept here.
- Whole software with injected inputs (launch_testing), for what the simulator cannot create: keep the file here and
  register it with `add_launch_test`, so that `colcon test` runs it.

Example (`docs/05_test_specification.md`):
- TC-S-001, map easy, run 3 times. During each run:
  `ros2 bag record -o results/TC-S-001/rosbag/run_<n> /ego/vehicle_state /ego/track_end /ego/vehicle_command /ego/computation_time/perception /ego/computation_time/control`.
  After each run: `python3 tools/save_evaluation.py TC-S-001` (SWR-001) and
  `python3 test/scenario/check_rosbag.py results/TC-S-001/rosbag/run_<n>` (SWR-002, SWR-003, SWR-004).
- TC-S-002, `test_invalid_speed.py`: perception and control with a vehicle speed of NaN (SWR-004), run by `colcon test`.
