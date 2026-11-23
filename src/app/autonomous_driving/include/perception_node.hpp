/**
 * @file      perception_node.hpp
 * @brief     perception node (COMP-01): lane points in, driving way out
 */

#ifndef __PERCEPTION_NODE_HPP__
#define __PERCEPTION_NODE_HPP__
#pragma once

#include <memory>
#include <mutex>
#include <string>

#include <ad_msgs/msg/computation_time.hpp>

#include "ros2_bridge_lane.hpp"

#include "perception_algorithm.hpp"
#include "perception_config.hpp"

class Perception : public rclcpp::Node {
    public:
        explicit Perception(const std::string& node_name, const rclcpp::NodeOptions& options = rclcpp::NodeOptions());

        void ProcessParams();
        void Run();

    private:
        inline void CallbackLanePoints(const ad_msgs::msg::LanePointData::SharedPtr msg) {
            std::lock_guard<std::mutex> lock(mutex_lane_points_);
            i_lane_points_ = ros2_bridge::GetLanePoints(*msg);
            b_is_lane_points_ = true;
        }

        // Subscriber, input, mutex
        rclcpp::Subscription<ad_msgs::msg::LanePointData>::SharedPtr s_lane_points_;
        interface::Lane i_lane_points_;
        std::mutex mutex_lane_points_;

        // Publisher
        rclcpp::Publisher<ad_msgs::msg::PolyfitLaneData>::SharedPtr p_driving_way_;
        rclcpp::Publisher<ad_msgs::msg::ComputationTime>::SharedPtr p_computation_time_;

        // Timer
        rclcpp::TimerBase::SharedPtr t_run_node_;

        // Configuration and units
        PerceptionConfig cfg_;
        std::unique_ptr<PerceptionAlgorithm> alg_;

        bool b_is_lane_points_ = false;
};

#endif // __PERCEPTION_NODE_HPP__
