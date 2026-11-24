/**
 * @file      control_algorithm.hpp
 * @brief     units of the control node (COMP-02), without ROS code
 */

#ifndef __CONTROL_ALGORITHM_HPP__
#define __CONTROL_ALGORITHM_HPP__
#pragma once

#include <cmath>
#include <utility>

#include "interface_lane.hpp"
#include "interface_vehicle.hpp"
#include "control/control_config.hpp"

class ControlAlgorithm {
    public:
        explicit ControlAlgorithm(const ControlConfig& cfg) : cfg_(cfg) {}

        // Steering [rad] by pure pursuit toward the point of the driving way at look_ahead_distance.
        double LateralControl(const interface::PolyfitLane& driving_way) const;

        // (accel, brake) by P control on the speed error, each within [0, 1]; (0, 0) for an invalid speed (NaN).
        std::pair<double, double> LongitudinalControl(const interface::VehicleState& vehicle_state,
                                                      double target_speed) const;

    private:
        ControlConfig cfg_;
};

#endif // __CONTROL_ALGORITHM_HPP__
