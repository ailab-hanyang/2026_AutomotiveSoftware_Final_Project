# 04. Detailed Design (SWE.3)

AI agents may draft this document. The team reviews it before merging.
Each unit is a function in `src/` with `// @req SWR-xxx` on the line above it.
Give the valid range and the physical unit of every input and output, and the preconditions of the unit.
Copy the `COMP-01` section below for each component in `03_sw_architecture.md`.

<!-- Example: all units are implemented in src/app/autonomous_driving/src/<node>/, one <node>_algorithm.cpp per node.
     The node files only receive the inputs, call the units, and publish the outputs. -->

## COMP-01: perception

### Units
| Unit | File | Input (format, unit) | Output (format, unit) | Implements (SWR-ID) |
|---|---|---|---|---|
| FindDrivingWay | `src/perception/perception_algorithm.cpp` | `interface::Lane` (lane points) in `ego/body`: x from -5 to 20 m, y from -4 to 4 m (lane detection ROI). Precondition: at least 4 points on each lane (on the map easy, at least 6 per lane were recorded) | `interface::PolyfitLane` (driving way): center line y = a0 + a1 x + a2 x² + a3 x³, a0 [m], a1 [-], a2 [1/m], a3 [1/m²]. The points with y > 0 form the left lane and the others the right lane; a 3rd order polynomial is fitted to each lane by least squares, and the driving way is the mean of the two | SWR-001 |

### Units and their interaction (dynamic aspects)
Every 10 ms: FindDrivingWay on the newest IF-02, published as IF-03.
The period and its computation time go to IF-05.

### Parameters
No parameters besides the period: the fit uses all lane points of IF-02 (the lane detection ROI is fixed by the simulator).

| Name | Value | Unit | Where (config file) |
|---|---|---|---|
| `loop_rate_hz` | 100.0 | Hz | `launch/autonomous_driving.launch.xml` |

### Coding principles
Coding guideline of Practice 09. No ROS code in `*_algorithm.cpp`; one unit is one function.

## COMP-02: control

### Units
| Unit | File | Input (format, unit) | Output (format, unit) | Implements (SWR-ID) |
|---|---|---|---|---|
| LateralControl | `src/control/control_algorithm.cpp` | `interface::PolyfitLane` (driving way): a0 [m], a1 [-], a2 [1/m], a3 [1/m²] | steering [rad] by pure pursuit, from -π/2 to π/2 (not limited here; the vehicle limits it to ±30°): look-ahead point (g_x, g_y) on the driving way at g_x = `look_ahead_distance`, l_d = √(g_x² + g_y²), steering = atan(2 L g_y / l_d²) with the wheel base L | SWR-001 |
| LongitudinalControl | `src/control/control_algorithm.cpp` | `interface::VehicleState` velocity [m/s] (NaN: invalid), `target_speed` [m/s], ≥ 0 | (accel, brake), each in [0, 1]: `longitudinal_kp` · speed error as accel (positive) or brake (negative); (0, 0) for an invalid speed (NaN) | SWR-002, SWR-004 |

### Units and their interaction (dynamic aspects)
Every 10 ms: LateralControl on the newest driving way, then LongitudinalControl with `target_speed`.
The period and its computation time go to IF-06.

### Parameters
Default values are in the config file; the node declares its ROS parameters with these values, and the launch file can override them.

| Name | Value | Unit | Where (config file) |
|---|---|---|---|
| `look_ahead_distance` | 4.0 | m | `include/control/control_config.hpp` |
| `target_speed` | 10.0 | m/s | `include/control/control_config.hpp`, `launch/autonomous_driving.launch.xml` |
| `longitudinal_kp` | 0.3 | 1/(m/s) | `include/control/control_config.hpp` |
| `use_manual_inputs` | false (true with the launch argument `manual:=true`) | - | `include/control/control_config.hpp`, `launch/autonomous_driving.launch.xml` |
| `loop_rate_hz` | 100.0 | Hz | `launch/autonomous_driving.launch.xml` |

### Coding principles
Coding guideline of Practice 09. No ROS code in `*_algorithm.cpp`; one unit is one function;
the node file only receives inputs, calls the units, and publishes the outputs.
