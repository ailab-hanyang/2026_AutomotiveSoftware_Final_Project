"""SWE.5 integration test TC-I-004: the perception node (COMP-01) alone, with fake lane points (no simulator)."""
import time
import unittest

import launch
import launch_ros.actions
import launch_testing.actions
import pytest
import rclpy
from ad_msgs.msg import LanePointData, PolyfitLaneData
from geometry_msgs.msg import Point


@pytest.mark.launch_test
def generate_test_description():
    node = launch_ros.actions.Node(
        package='autonomous_driving', executable='perception',
        name='perception', namespace='ego',
        parameters=[{'perception/ns': 'ego', 'perception/loop_rate_hz': 100.0}])
    return launch.LaunchDescription([node, launch_testing.actions.ReadyToTest()])


def straight_lanes():
    """Points of two lane markings 2 m left and right of the vehicle, 21 points each from -2 to 18 m (frame ego/body)."""
    msg = LanePointData(frame_id='ego/body', id='')
    for y in (-2.0, 2.0):
        msg.point += [Point(x=float(x), y=y, z=0.0) for x in range(-2, 19)]
    return msg


class TestPerceptionIO(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def setUp(self):
        self.node = rclpy.create_node('test_perception_io')
        self.ways = []
        self.node.create_subscription(PolyfitLaneData, '/ego/driving_way', self.ways.append, 100)
        self.lanes_pub = self.node.create_publisher(LanePointData, '/ego/lane_points', 10)

    def tearDown(self):
        self.node.destroy_node()

    def spin(self, seconds, send_lanes):
        end = time.time() + seconds
        lanes, last = straight_lanes(), 0.0
        while time.time() < end:
            if send_lanes and time.time() - last > 1.0 / 30.0:   # lane points at 30 Hz, like lane_detection
                self.lanes_pub.publish(lanes)
                last = time.time()
            rclpy.spin_once(self.node, timeout_sec=0.005)

    # @verifies COMP-01, IF-02, IF-03
    def test_tc_i_004_driving_way_from_lane_points(self):
        self.spin(1.0, send_lanes=False)   # no lane points yet: no driving way
        self.assertEqual(len(self.ways), 0)
        self.spin(2.0, send_lanes=True)
        self.assertGreaterEqual(len(self.ways), 100)                   # 100 Hz
        self.assertAlmostEqual(self.ways[-1].a0, 0.0, delta=0.1)       # center between the markings
