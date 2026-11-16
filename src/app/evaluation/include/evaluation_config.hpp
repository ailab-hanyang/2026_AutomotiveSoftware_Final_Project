/**
 * @copyright Hanyang University, Department of Automotive Engineering, 2024. All rights reserved. 
 *            Subject to limited distribution and restricted disclosure only.
 *            
 * @file      evaluation_config.hpp
 * @brief     evaluation configuration
 * 
 * @date      2023-08-07 created by Yuseung Na (yuseungna@hanyang.ac.kr)
 *            2024-12-09 update parking evaluation to ROS2 from 2023jasiple (sunghoon8585@gmail.com)
 */

#ifndef __EVALUATION_CONFIG_HPP__
#define __EVALUATION_CONFIG_HPP__
#pragma once

// STD Header
#include <string>
#include <cmath>

typedef enum {
    APPROACHING = 0,
    IN_PROGRESS,
    SUCCESS,
    FAIL
} ParkingStatus;

typedef struct {
    unsigned char status = ParkingStatus::APPROACHING;
    // slot positions and the success threshold are fixed per map in the evaluation library
} ParkingSlotParams;


typedef struct {
    std::string ref_csv_path;
    std::string lane_id;

    double loop_rate_hz;

    // map (resources/csv/<map>/) and parking switch; the time limit (per map) and the
    // lane departure threshold (4.5 m) are fixed in the evaluation library
    std::string map{"easy"};
    bool use_parking{true};

    ParkingSlotParams parking;
} EvaluationConfig;


#endif // __EVALUATION_CONFIG_HPP__