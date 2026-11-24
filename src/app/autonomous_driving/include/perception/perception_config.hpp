/**
 * @file      perception_config.hpp
 * @brief     parameters of the perception node (COMP-01), listed in docs/04_detailed_design.md
 */

#ifndef __PERCEPTION_CONFIG_HPP__
#define __PERCEPTION_CONFIG_HPP__
#pragma once

#include <string>

typedef struct {
    std::string vehicle_namespace{""};
    double loop_rate_hz{100.0};
} PerceptionConfig;

#endif // __PERCEPTION_CONFIG_HPP__
