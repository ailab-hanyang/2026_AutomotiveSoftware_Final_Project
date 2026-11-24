/**
 * @file      control_algorithm.cpp
 * @brief     units of the control node (COMP-02)
 */

#include "control/control_algorithm.hpp"

#include <algorithm>

// @req SWR-001
double ControlAlgorithm::LateralControl(const interface::PolyfitLane& driving_way) const {
    double steering = 0.0;

    // Step 1: Look-ahead point (g_x, g_y) on the driving way at the look-ahead distance
    double g_x = cfg_.look_ahead_distance;
    double g_y = driving_way.a0 + driving_way.a1 * g_x + driving_way.a2 * pow(g_x, 2) + driving_way.a3 * pow(g_x, 3);

    // Step 2: Distance between the vehicle and the look-ahead point
    double l_d = sqrt(pow(g_x, 2) + pow(g_y, 2));

    // Step 3: Pure pursuit: steering = atan(2 * L * g_y / l_d^2)
    steering = atan2(2.0 * interface::vehicle_param::wheel_base * g_y, pow(l_d, 2));

    return steering;
}

// @req SWR-002, SWR-004
std::pair<double, double> ControlAlgorithm::LongitudinalControl(
    const interface::VehicleState& vehicle_state, double target_speed) const {
    double accel_command = 0.0;
    double brake_command = 0.0;

    // Step 1: Speed error
    double error = target_speed - vehicle_state.velocity;

    // Step 2: P control
    double output = cfg_.longitudinal_kp * error;
    if (std::isnan(output)) {
        output = 0.0;  // invalid speed (NaN): no command
    }

    // Step 3: Positive output: accel command, negative output: brake command (0 to 1)
    if (output > 0.0) {
        accel_command = std::min(output, 1.0);
    }
    else if (output < 0.0) {
        brake_command = std::min(-output, 1.0);
    }

    return std::make_pair(accel_command, brake_command);
}
