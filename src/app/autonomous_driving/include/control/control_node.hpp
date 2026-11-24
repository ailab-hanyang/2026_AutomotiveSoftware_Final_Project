/**
 * @file      control_node.hpp
 * @brief     control node (COMP-02): vehicle state and driving way in, vehicle command out
 */

#ifndef __CONTROL_NODE_HPP__
#define __CONTROL_NODE_HPP__
#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <string>

#include <ad_msgs/msg/computation_time.hpp>

#include "ros2_bridge_lane.hpp"
#include "ros2_bridge_vehicle.hpp"

#include "control/control_algorithm.hpp"
#include "control/control_config.hpp"

class Control : public rclcpp::Node {
    public:
        explicit Control(const std::string& node_name, const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

        void ProcessParams();
        void Run();

    private:
        inline void CallbackManualInput(const ad_msgs::msg::VehicleCommand::SharedPtr msg) {
            std::lock_guard<std::mutex> lock(mutex_manual_input_);
            i_manual_input_ = ros2_bridge::GetVehicleCommand(*msg);
            b_is_manual_input_ = true;
        }
        inline void CallbackVehicleState(const ad_msgs::msg::VehicleState::SharedPtr msg) {
            std::lock_guard<std::mutex> lock(mutex_vehicle_state_);
            i_vehicle_state_ = ros2_bridge::GetVehicleState(*msg);
            b_is_vehicle_state_ = true;
        }
        inline void CallbackDrivingWay(const ad_msgs::msg::PolyfitLaneData::SharedPtr msg) {
            std::lock_guard<std::mutex> lock(mutex_driving_way_);
            i_driving_way_.frame_id = msg->frame_id;
            i_driving_way_.id = msg->id;
            i_driving_way_.a0 = msg->a0;
            i_driving_way_.a1 = msg->a1;
            i_driving_way_.a2 = msg->a2;
            i_driving_way_.a3 = msg->a3;
            b_is_driving_way_ = true;
        }

        // Subscriber, input, mutex
        rclcpp::Subscription<ad_msgs::msg::VehicleState>::SharedPtr    s_vehicle_state_;
        rclcpp::Subscription<ad_msgs::msg::VehicleCommand>::SharedPtr  s_manual_input_;
        rclcpp::Subscription<ad_msgs::msg::PolyfitLaneData>::SharedPtr s_driving_way_;
        interface::VehicleState i_vehicle_state_;
        interface::VehicleCommand i_manual_input_;
        interface::PolyfitLane  i_driving_way_;
        std::mutex mutex_vehicle_state_;
        std::mutex mutex_manual_input_;
        std::mutex mutex_driving_way_;

        // Publisher
        rclcpp::Publisher<ad_msgs::msg::VehicleCommand>::SharedPtr p_vehicle_command_;
        rclcpp::Publisher<ad_msgs::msg::ComputationTime>::SharedPtr p_computation_time_;

        // Timer
        rclcpp::TimerBase::SharedPtr t_run_node_;

        // Configuration and units
        ControlConfig cfg_;
        std::unique_ptr<ControlAlgorithm> alg_;

        bool b_is_vehicle_state_ = false;
        bool b_is_manual_input_ = false;
        bool b_is_driving_way_ = false;
};

#endif // __CONTROL_NODE_HPP__
