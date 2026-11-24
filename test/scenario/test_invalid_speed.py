"""SWE.6 software test TC-S-002: the whole software (perception and control) with an invalid vehicle speed (NaN).

The simulator cannot create an invalid speed, so the test injects it: fake lane points and a vehicle state
with velocity NaN (negative test of SWR-004)."""
import math
import time
import unittest

import launch
import launch_ros.actions
import launch_testing.actions
import pytest
import rclpy
from ad_msgs.msg import LanePointData, VehicleCommand, VehicleState
from geometry_msgs.msg import Point


@pytest.mark.launch_test
def generate_test_description():
    perception = launch_ros.actions.Node(
        package='autonomous_driving', executable='perception', name='perception', namespace='ego',
        parameters=[{'perception/ns': 'ego', 'perception/loop_rate_hz': 100.0}])
    control = launch_ros.actions.Node(
        package='autonomous_driving', executable='control', name='control', namespace='ego',
        parameters=[{'control/ns': 'ego', 'control/loop_rate_hz': 100.0}])
    return launch.LaunchDescription([perception, control, launch_testing.actions.ReadyToTest()])


def straight_lanes():
    """Points of two lane markings 2 m left and right of the vehicle, 21 points each from -2 to 18 m (frame ego/body)."""
    msg = LanePointData(frame_id='ego/body', id='')
    for y in (-2.0, 2.0):
        msg.point += [Point(x=float(x), y=y, z=0.0) for x in range(-2, 19)]
    return msg


class TestInvalidSpeed(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        self.node = rclpy.create_node('test_invalid_speed')
        self.commands = []
        self.node.create_subscription(VehicleCommand, '/ego/vehicle_command', self.commands.append, 100)
        self.state_pub = self.node.create_publisher(VehicleState, '/ego/vehicle_state', 10)
        self.lanes_pub = self.node.create_publisher(LanePointData, '/ego/lane_points', 10)

    def tearDown(self):
        self.node.destroy_node()

    # @verifies SWR-004
    def test_tc_s_002_no_command_with_invalid_speed(self):
        state, lanes = VehicleState(velocity=math.nan), straight_lanes()
        end, last = time.time() + 3.0, 0.0
        while time.time() < end:
            if time.time() - last > 1.0 / 30.0:   # inputs at 30 Hz
                self.state_pub.publish(state)
                self.lanes_pub.publish(lanes)
                last = time.time()
            rclpy.spin_once(self.node, timeout_sec=0.005)
        self.assertGreaterEqual(len(self.commands), 10)
        for cmd in self.commands:
            self.assertEqual(cmd.accel, 0.0)
            self.assertEqual(cmd.brake, 0.0)
