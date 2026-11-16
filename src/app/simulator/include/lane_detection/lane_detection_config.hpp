/**
 * @copyright Hanyang University, Department of Automotive Engineering, 2024. All rights reserved. 
 *            Subject to limited distribution and restricted disclosure only.
 *            
 * @file      lane_detection_config.hpp
 * @brief     lane detection configuration
 * 
 * @date      2023-08-07 created by Yuseung Na (yuseungna@hanyang.ac.kr)
 */

#ifndef __LANE_DETECTION_CONFIG_HPP__
#define __LANE_DETECTION_CONFIG_HPP__
#pragma once

// STD Header
#include <string>
#include <cmath>

typedef struct {
    std::string vehicle_namespace{""};
    std::string ref_csv_path{""};

    double loop_rate_hz{30.0};
    // the ROI box is fixed in the lane detection library
    // (front 20 m, rear 5 m, left 4 m, right 4 m in the body frame)

    // map (resources/csv/<map>/) and lane loss switch; the lane loss zones are fixed per map
    // in the lane detection library
    std::string map{"easy"};
    bool use_lane_loss{true};

} LaneDetectionConfig;

#endif // __LANE_DETECTION_CONFIG_HPP__