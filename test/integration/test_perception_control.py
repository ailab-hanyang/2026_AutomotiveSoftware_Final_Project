"""SWE.5 integration tests TC-I-001 and TC-I-003: perception (COMP-01) and control (COMP-02) together,
with fake vehicle_state and lane_points (no simulator)."""
import time
import unittest

import launch
import launch_ros.actions
import launch_testing.actions
import pytest
import rclpy
from ad_msgs.msg import ComputationTime, LanePointData, PolyfitLaneData, VehicleCommand, VehicleState
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


class TestPerceptionControl(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        self.node = rclpy.create_node('test_perception_control')
        self.ways, self.commands = [], []
        self.times = {'perception': [], 'control': []}
        self.node.create_subscription(PolyfitLaneData, '/ego/driving_way', self.ways.append, 10)
        self.node.create_subscription(VehicleCommand, '/ego/vehicle_command', self.commands.append, 10)
        for name in self.times:
            self.node.create_subscription(ComputationTime, '/ego/computation_time/' + name,
                                          lambda msg, n=name: self.times[n].append(msg), 100)
        self.state_pub = self.node.create_publisher(VehicleState, '/ego/vehicle_state', 10)
        self.lanes_pub = self.node.create_publisher(LanePointData, '/ego/lane_points', 10)

    def tearDown(self):
        self.node.destroy_node()

    def spin(self, seconds):
        end = time.time() + seconds
        state, lanes = VehicleState(velocity=10.0), straight_lanes()
        last = 0.0
        while time.time() < end:
            if time.time() - last > 1.0 / 30.0:   # inputs at 30 Hz, like lane_detection
                self.state_pub.publish(state)
                self.lanes_pub.publish(lanes)
                last = time.time()
            rclpy.spin_once(self.node, timeout_sec=0.005)

    # @verifies COMP-01, COMP-02, IF-01, IF-02, IF-03, IF-04
    def test_tc_i_001_lane_center_to_command(self):
        self.spin(5.0)
        self.assertGreaterEqual(len(self.ways), 10)
        self.assertGreaterEqual(len(self.commands), 10)
        self.assertAlmostEqual(self.ways[-1].a0, 0.0, delta=0.1)       # center between the markings
        self.assertAlmostEqual(self.commands[-1].steering, 0.0, delta=0.01)

    # @verifies COMP-01, COMP-02, IF-05, IF-06
    def test_tc_i_003_computation_time(self):
        self.spin(10.0)
        for name, msgs in self.times.items():
            self.assertGreaterEqual(len(msgs), 500, name)              # about 100 Hz for 10 s
            longest = max(msg.computation_time for msg in msgs)
            print('TC-I-003 %s: %d periods of %.1f ms, longest %.3f ms' % (name, len(msgs), msgs[0].period, longest))
            for msg in msgs:
                self.assertLessEqual(msg.computation_time, msg.period, name)
