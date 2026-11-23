/**
 * @file      control_config.hpp
 * @brief     parameters of the control node (COMP-02), listed in docs/04_detailed_design.md
 */

#ifndef __CONTROL_CONFIG_HPP__
#define __CONTROL_CONFIG_HPP__
#pragma once

#include <string>

typedef struct {
    std::string vehicle_namespace{""};
    double loop_rate_hz{100.0};
    double look_ahead_distance{4.0};  // [m] distance of the look-ahead point of pure pursuit
    double target_speed{10.0};        // [m/s]
    double longitudinal_kp{0.3};      // [1/(m/s)] accel (or brake) per m/s of speed error
} ControlConfig;

#endif // __CONTROL_CONFIG_HPP__
