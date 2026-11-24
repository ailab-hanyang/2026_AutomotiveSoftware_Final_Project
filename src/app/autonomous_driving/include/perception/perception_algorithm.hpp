/**
 * @file      perception_algorithm.hpp
 * @brief     units of the perception node (COMP-01), without ROS code
 */

#ifndef __PERCEPTION_ALGORITHM_HPP__
#define __PERCEPTION_ALGORITHM_HPP__
#pragma once

#include "interface_lane.hpp"

class PerceptionAlgorithm {
    public:
        // Driving way (center line) from the lane points: a 3rd order polynomial fitted to the left lane (y > 0)
        // and to the right lane (y <= 0), then the mean of the two.
        interface::PolyfitLane FindDrivingWay(const interface::Lane& lane_points) const;
};

#endif // __PERCEPTION_ALGORITHM_HPP__
