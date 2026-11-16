/**
 * @copyright Hanyang University, Department of Automotive Engineering, 2024. All rights reserved. 
 *            Subject to limited distribution and restricted disclosure only.
 *            
 * @file      evaluation_node.cpp
 * @brief     autonomous driving algorithm evaluation tool
 * 
 * @date      2018-11-28 created by Eunsan Jo (eunsan.mountain@gmail.com)
 *            2023-08-07 updated by Yuseung Na (yuseungna@hanyang.ac.kr)
 *              : adapt new template
 *            2023-08-20 updated by Yuseung Na (yuseungna@hanyang.ac.kr)
 *              : change to ROS2
 *            2024-11-05 updated by Yuseung Na (yuseungna@hanyang.ac.kr)
 *              : clean up
 *            2024-12-09 update parking evaluation to ROS2 from 2023jasiple (sunghoon8585@gmail.com)
 */

#include "evaluation_node.hpp"

#include <sys/stat.h>

Evaluation::Evaluation(const std::string& node_name, const rclcpp::NodeOptions& options)
    : Node(node_name, options) {
    
    // QoS init
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));

    // Parameter init
    this->declare_parameter("evaluation/loop_rate_hz", 50.0);
    this->declare_parameter("evaluation/lane_id", "");
    this->declare_parameter("evaluation/map", "easy");
    this->declare_parameter("evaluation/parking", true);
    ProcessParams();

    RCLCPP_INFO(this->get_logger(), "loop_rate_hz: %f", cfg_.loop_rate_hz);
    RCLCPP_INFO(this->get_logger(), "lane_id: %s", cfg_.lane_id.c_str());
    RCLCPP_INFO(this->get_logger(), "map: %s", cfg_.map.c_str());
    RCLCPP_INFO(this->get_logger(), "parking: %d", cfg_.use_parking);
    
    std::string dir(getenv("PWD"));
    std::string csv_path("/resources/csv/" + cfg_.map + "/evaluation_lane");
    cfg_.ref_csv_path = dir + csv_path;
    RCLCPP_INFO(this->get_logger(), "ref_csv_path: %s", cfg_.ref_csv_path.c_str());
    struct stat st;
    if (stat(cfg_.ref_csv_path.c_str(), &st) != 0) {
        RCLCPP_FATAL(this->get_logger(), "map '%s' not found (%s); use easy, normal, or hard and run from the repository root",
                     cfg_.map.c_str(), cfg_.ref_csv_path.c_str());
        throw std::runtime_error("map not found");
    }

    // Subscriber init
    s_vehicle_state_ = this->create_subscription<ad_msgs::msg::VehicleState>(
        "/ego/vehicle_state", qos_profile, std::bind(&Evaluation::CallbackVehicleState, this, std::placeholders::_1));
    s_mission_ = this->create_subscription<ad_msgs::msg::Mission>(
        "/ego/mission", qos_profile, std::bind(&Evaluation::CallbackMission, this, std::placeholders::_1));

    // Publisher init
    p_text_evaluation_result_ = this->create_publisher<rviz_2d_overlay_msgs::msg::OverlayText>(
        "/text_evaluation_result", qos_profile);
    p_track_end_ = this->create_publisher<std_msgs::msg::Bool>(
        "/ego/track_end", qos_profile);

    // Timer init
    t_run_node_ = this->create_wall_timer(
        std::chrono::milliseconds((int64_t)(1000 / cfg_.loop_rate_hz)),
        [this]() { this->Run(); });
        
    // Algorithm init
    alg_evaluation_ = std::make_unique<EvaluationAlgorithm>(cfg_);
    time_start_ = this->now().seconds();
    time_prev_ = time_start_;
    
    RCLCPP_WARN_STREAM(this->get_logger(), "Initialize node (Period: " << cfg_.loop_rate_hz << " Hz)");
}

Evaluation::~Evaluation() {}

void Evaluation::ProcessParams() {
    this->get_parameter("evaluation/loop_rate_hz", cfg_.loop_rate_hz);
    this->get_parameter("evaluation/lane_id", cfg_.lane_id);
    this->get_parameter("evaluation/map", cfg_.map);
    this->get_parameter("evaluation/parking", cfg_.use_parking);
}

void Evaluation::Run() {
    auto current_time = this->now();
    RCLCPP_INFO_THROTTLE(this->get_logger(), *get_clock(), 1000, "Running ...");

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
    // Get subscribe variables
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
    if (b_is_simulator_on_ == false) {
        RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Wait for Vehicle State ...");
        time_start_ = current_time.seconds();
        return;
    }
    if (b_is_mission_ == false) {
        RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Wait for Mission ...");
        return;
    }

    interface::VehicleState vehicle_state; {
        std::lock_guard<std::mutex> lock(mutex_vehicle_state_);
        vehicle_state = i_vehicle_state_;
    }

    interface::Mission mission; {
        std::lock_guard<std::mutex> lock(mutex_mission_);
        mission = i_mission_;
    }
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
    // Algorithm
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //  
    double curr_evaluation_time = current_time.seconds();
    double time_dt = curr_evaluation_time - time_prev_;
    time_prev_ = curr_evaluation_time;

    // Run evaluation algorithm
    if (b_is_finished_ == false) {
        cfg_.parking.status = ParkingStatus::APPROACHING;
        time_eval_driving_ = curr_evaluation_time - time_start_;
        b_is_finished_ = alg_evaluation_->RunAlgorithm(vehicle_state, mission, time_eval_driving_, time_dt, 6.0, cfg_);        
    }
    else if (b_is_finished_ == true && cfg_.use_parking == true) {
        // Parking phase after the track end (skipped when parking is switched off)
        cfg_.parking.status = ParkingStatus::IN_PROGRESS;
        alg_evaluation_->RunAlgorithm(vehicle_state, mission, time_eval_driving_, time_dt, 6.0, cfg_);
    }
    std::string evaluation_info = alg_evaluation_->GetEvaluationInfo();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
    // Publish output
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
    p_text_evaluation_result_->publish(ros2_bridge::UpdateEvaluationResult(evaluation_info));
    p_track_end_->publish(ros2_bridge::UpdateTrackEnd(b_is_finished_));
}

int main(int argc, char **argv) {
    std::string node_name = "evaluation";

    // Initialize node
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Evaluation>(node_name));
    rclcpp::shutdown();

    return 0;
}
