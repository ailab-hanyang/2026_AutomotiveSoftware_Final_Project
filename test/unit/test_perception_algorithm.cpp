// SWE.4 unit tests of the perception unit (COMP-01). Test case IDs: docs/05_test_specification.md
#include <gtest/gtest.h>

#include "perception/perception_algorithm.hpp"

namespace {
// Lane points of a left and a right lane marking, y = offset + curvature * x^2 [m],
// one point per meter from x = -5 to 20 m (lane detection ROI), no noise
interface::Lane LanePoints(double left_offset, double right_offset, double curvature) {
    interface::Lane lane_points;
    for (int x = -5; x <= 20; ++x) {
        lane_points.point.push_back({static_cast<double>(x), left_offset + curvature * x * x});
        lane_points.point.push_back({static_cast<double>(x), right_offset + curvature * x * x});
    }
    return lane_points;
}
}  // namespace

class PerceptionAlgorithmTest : public ::testing::Test {
    protected:
        PerceptionAlgorithm alg_;
};

// @verifies FindDrivingWay
TEST_F(PerceptionAlgorithmTest, TC_U_001_StraightLane) {
    interface::PolyfitLane way = alg_.FindDrivingWay(LanePoints(2.5, -1.5, 0.0));
    EXPECT_NEAR(way.a0, 0.5, 0.1);    // center between the markings
    EXPECT_NEAR(way.a1, 0.0, 0.01);
}

// @verifies FindDrivingWay
TEST_F(PerceptionAlgorithmTest, TC_U_001_CurvedLane) {
    interface::PolyfitLane way = alg_.FindDrivingWay(LanePoints(2.0, -2.0, 0.005));
    EXPECT_NEAR(way.a0, 0.0, 0.1);
    EXPECT_NEAR(way.a2, 0.005, 0.001);
}
