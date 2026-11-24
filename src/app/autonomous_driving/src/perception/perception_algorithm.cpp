/**
 * @file      perception_algorithm.cpp
 * @brief     units of the perception node (COMP-01)
 */

#include "perception/perception_algorithm.hpp"

#include <cmath>
#include <vector>

#include <eigen3/Eigen/Dense>

// @req SWR-001
interface::PolyfitLane PerceptionAlgorithm::FindDrivingWay(const interface::Lane& lane_points) const {
    interface::PolyfitLane driving_way;
    driving_way.frame_id = lane_points.frame_id;
    driving_way.id = "driving_way";

    // Step 1: Separate the lane points into the left lane (y > 0) and the right lane (y <= 0)
    std::vector<double> left_x, left_y, right_x, right_y;
    for (const auto& point : lane_points.point) {
        if (point.y > 0.0) {
            left_x.push_back(point.x);
            left_y.push_back(point.y);
        }
        else {
            right_x.push_back(point.x);
            right_y.push_back(point.y);
        }
    }

    // Step 2: Fit a 3rd order polynomial to each lane: A = (X^T X)^-1 X^T Y
    Eigen::MatrixXd X_left(left_x.size(), 4);
    Eigen::VectorXd Y_left(left_x.size());
    for (size_t i = 0; i < left_x.size(); i++) {
        X_left.row(i) << 1.0, left_x[i], pow(left_x[i], 2), pow(left_x[i], 3);
        Y_left(i) = left_y[i];
    }
    Eigen::MatrixXd X_right(right_x.size(), 4);
    Eigen::VectorXd Y_right(right_x.size());
    for (size_t i = 0; i < right_x.size(); i++) {
        X_right.row(i) << 1.0, right_x[i], pow(right_x[i], 2), pow(right_x[i], 3);
        Y_right(i) = right_y[i];
    }
    Eigen::VectorXd A_left = (X_left.transpose() * X_left).inverse() * X_left.transpose() * Y_left;
    Eigen::VectorXd A_right = (X_right.transpose() * X_right).inverse() * X_right.transpose() * Y_right;

    // Step 3: Driving way: the center line between the left and right lane
    driving_way.a0 = (A_left(0) + A_right(0)) / 2.0;
    driving_way.a1 = (A_left(1) + A_right(1)) / 2.0;
    driving_way.a2 = (A_left(2) + A_right(2)) / 2.0;
    driving_way.a3 = (A_left(3) + A_right(3)) / 2.0;

    return driving_way;
}
