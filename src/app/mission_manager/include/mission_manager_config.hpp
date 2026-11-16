/**
 * @copyright Hanyang University, Department of Automotive Engineering, 2024. All rights reserved. 
 *            Subject to limited distribution and restricted disclosure only.
 *            
 * @file      mission_manager_config.hpp
 * @brief     mission manager configuration
 * 
 * @date      2024-10-18 created by Yuseung Na (yuseungna@hanyang.ac.kr)
 */

#ifndef __MISSION_MANAGER_CONFIG_HPP__
#define __MISSION_MANAGER_CONFIG_HPP__
#pragma once

// STD Header
#include <string>
#include <cmath>

typedef struct {
    std::string vehicle_namespace{""};
    double loop_rate_hz{100.0};
    std::string ref_csv_path;

    // map (resources/csv/<map>/) and mission switches; mission positions and parameters
    // per map are fixed inside the mission libraries
    std::string map{"easy"};            // easy, normal, or hard
    bool use_ice{true};                 // icy region (Mission.road_condition = "Ice")
    bool use_slope{true};               // uphill and downhill regions (Mission.road_slope)
    bool use_school_zone{true};         // school zone (Mission.speed_limit, reported 50 m ahead)
    bool use_scc{true};                 // vehicles ahead to follow (Mission.objects)
    bool use_merge{true};               // lane closure and merging vehicles (Mission.objects)
    bool use_parking{true};             // Mission.parking turns true at the end of the track
} MissionManagerConfig;

#endif // __MISSION_MANAGER_CONFIG_HPP__