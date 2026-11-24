// SWE.4 unit tests of the control units (COMP-02). Test case IDs: docs/05_test_specification.md
#include <gtest/gtest.h>

#include <limits>

#include "control/control_algorithm.hpp"

class ControlAlgorithmTest : public ::testing::Test {
    protected:
        ControlConfig cfg_;
        ControlAlgorithm alg_{cfg_};

        interface::VehicleState State(double velocity) {
            interface::VehicleState state;
            state.velocity = velocity;
            return state;
        }
};

// @verifies LongitudinalControl
TEST_F(ControlAlgorithmTest, TC_U_002_NormalCase) {
    auto out = alg_.LongitudinalControl(State(9.0), 10.0);   // speed error 1 m/s
    EXPECT_DOUBLE_EQ(out.first, 0.3);
    EXPECT_DOUBLE_EQ(out.second, 0.0);
}

// @verifies LongitudinalControl
TEST_F(ControlAlgorithmTest, TC_U_002_BoundaryValue) {
    auto out = alg_.LongitudinalControl(State(0.0), 10.0);   // largest speed error at the start
    EXPECT_DOUBLE_EQ(out.first, 1.0);
    EXPECT_DOUBLE_EQ(out.second, 0.0);
    out = alg_.LongitudinalControl(State(10.0), 0.0);        // stop from the target speed
    EXPECT_DOUBLE_EQ(out.first, 0.0);
    EXPECT_DOUBLE_EQ(out.second, 1.0);
}

// @verifies LongitudinalControl
TEST_F(ControlAlgorithmTest, TC_U_002_NegativeTest) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    auto out = alg_.LongitudinalControl(State(nan), 10.0);
    EXPECT_DOUBLE_EQ(out.first, 0.0);
    EXPECT_DOUBLE_EQ(out.second, 0.0);
}

// @verifies LongitudinalControl
TEST_F(ControlAlgorithmTest, TC_U_003_SpeedBelowTarget) {
    auto out = alg_.LongitudinalControl(State(5.0), 10.0);
    EXPECT_GT(out.first, 0.0);
    EXPECT_DOUBLE_EQ(out.second, 0.0);
}

// @verifies LongitudinalControl
TEST_F(ControlAlgorithmTest, TC_U_003_SpeedAboveTarget) {
    auto out = alg_.LongitudinalControl(State(15.0), 10.0);
    EXPECT_DOUBLE_EQ(out.first, 0.0);
    EXPECT_GT(out.second, 0.0);
}

// @verifies LateralControl
TEST_F(ControlAlgorithmTest, TC_U_004_SteerTowardTheCenter) {
    interface::PolyfitLane way;
    EXPECT_DOUBLE_EQ(alg_.LateralControl(way), 0.0);     // on the center
    way.a0 = 1.0;                                        // lane center 1 m to the left
    EXPECT_GT(alg_.LateralControl(way), 0.0);
    way.a0 = -1.0;                                       // lane center 1 m to the right
    EXPECT_LT(alg_.LateralControl(way), 0.0);
}
