/**
 * @file      perception_node.cpp
 * @brief     perception node (COMP-01): receives the inputs, calls the units, publishes the outputs
 */

#include "perception_node.hpp"

#include <chrono>

Perception::Perception(const std::string& node_name, const rclcpp::NodeOptions& options)
    : Node(node_name, options) {
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));

    // Default values: include/perception_config.hpp; the launch file can override them
    this->declare_parameter("perception/ns", cfg_.vehicle_namespace);
    this->declare_parameter("perception/loop_rate_hz", cfg_.loop_rate_hz);
    ProcessParams();
    alg_ = std::make_unique<PerceptionAlgorithm>();

    RCLCPP_INFO(this->get_logger(), "vehicle_namespace: %s", cfg_.vehicle_namespace.c_str());
    RCLCPP_INFO(this->get_logger(), "loop_rate_hz: %f", cfg_.loop_rate_hz);

    // IF-02 in; IF-03, IF-05 out
    s_lane_points_ = this->create_subscription<ad_msgs::msg::LanePointData>(
        "lane_points", qos_profile, std::bind(&Perception::CallbackLanePoints, this, std::placeholders::_1));
    p_driving_way_ = this->create_publisher<ad_msgs::msg::PolyfitLaneData>("driving_way", qos_profile);
    p_computation_time_ = this->create_publisher<ad_msgs::msg::ComputationTime>("computation_time/perception", qos_profile);

    t_run_node_ = this->create_wall_timer(
        std::chrono::milliseconds((int64_t)(1000 / cfg_.loop_rate_hz)),
        [this]() { this->Run(); });
}

void Perception::ProcessParams() {
    this->get_parameter("perception/ns", cfg_.vehicle_namespace);
    this->get_parameter("perception/loop_rate_hz", cfg_.loop_rate_hz);
}

void Perception::Run() {
    auto start = std::chrono::steady_clock::now();

    if (b_is_lane_points_ == false) {
        RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Wait for Lane Points ...");
        return;
    }
    interface::Lane lane_points; {
        std::lock_guard<std::mutex> lock(mutex_lane_points_);
        lane_points = i_lane_points_;
    }

    // Unit (SWR-001)
    interface::PolyfitLane driving_way = alg_->FindDrivingWay(lane_points);
    p_driving_way_->publish(ros2_bridge::UpdatePolyfitLane(driving_way));

    // Computation time of this period (SWR-003)
    ad_msgs::msg::ComputationTime computation_time;
    computation_time.period = 1000.0 / cfg_.loop_rate_hz;
    computation_time.computation_time =
        std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    p_computation_time_->publish(computation_time);
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Perception>("perception"));
    rclcpp::shutdown();
    return 0;
}
