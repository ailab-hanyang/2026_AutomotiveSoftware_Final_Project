"""SWE.5 integration test TC-I-002: the control node (COMP-02) alone, with fake inputs (no simulator)."""
import time
import unittest

import launch
import launch_ros.actions
import launch_testing.actions
import pytest
import rclpy
from ad_msgs.msg import PolyfitLaneData, VehicleCommand, VehicleState


@pytest.mark.launch_test
def generate_test_description():
    node = launch_ros.actions.Node(
        package='autonomous_driving', executable='control',
        name='control', namespace='ego',
        parameters=[{'control/ns': 'ego', 'control/loop_rate_hz': 100.0}])
    return launch.LaunchDescription([node, launch_testing.actions.ReadyToTest()])


class TestControlIO(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        self.node = rclpy.create_node('test_control_io')
        self.received = []
        self.node.create_subscription(VehicleCommand, '/ego/vehicle_command', self.received.append, 10)
        self.state_pub = self.node.create_publisher(VehicleState, '/ego/vehicle_state', 10)
        self.way_pub = self.node.create_publisher(PolyfitLaneData, '/ego/driving_way', 10)

    def tearDown(self):
        self.node.destroy_node()

    def spin(self, seconds, send_way=True, until=None):
        end = time.time() + seconds
        state = VehicleState(velocity=5.0)
        way = PolyfitLaneData(frame_id='ego/body', a0=0.5)
        while time.time() < end and not (until and until()):
            self.state_pub.publish(state)
            if send_way:
                self.way_pub.publish(way)
            rclpy.spin_once(self.node, timeout_sec=0.01)

    # @verifies COMP-02, IF-01, IF-03, IF-04
    def test_tc_i_002_publishes_command(self):
        self.spin(1.0, send_way=False)    # vehicle_state only: no command while driving_way is missing
        self.assertEqual(len(self.received), 0)
        self.spin(5.0, until=lambda: len(self.received) >= 10)
        self.assertGreaterEqual(len(self.received), 10)
        for cmd in self.received:
            self.assertTrue(0.0 <= cmd.accel <= 1.0)
            self.assertTrue(0.0 <= cmd.brake <= 1.0)
