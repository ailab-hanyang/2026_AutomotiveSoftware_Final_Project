/**
 * @file      control_node.cpp
 * @brief     control node (COMP-02): receives the inputs, calls the units, publishes the outputs
 */

#include "control/control_node.hpp"

Control::Control(const std::string& node_name, const rclcpp::NodeOptions& options)
    : Node(node_name, options) {
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));

    // Default values: include/control/control_config.hpp; the launch file can override them
    this->declare_parameter("control/ns", cfg_.vehicle_namespace);
    this->declare_parameter("control/loop_rate_hz", cfg_.loop_rate_hz);
    this->declare_parameter("control/look_ahead_distance", cfg_.look_ahead_distance);
    this->declare_parameter("control/target_speed", cfg_.target_speed);
    this->declare_parameter("control/longitudinal_kp", cfg_.longitudinal_kp);
    this->declare_parameter("control/use_manual_inputs", cfg_.use_manual_inputs);
    ProcessParams();
    alg_ = std::make_unique<ControlAlgorithm>(cfg_);

    RCLCPP_INFO(this->get_logger(), "vehicle_namespace: %s", cfg_.vehicle_namespace.c_str());
    RCLCPP_INFO(this->get_logger(), "loop_rate_hz: %f, target_speed: %f", cfg_.loop_rate_hz, cfg_.target_speed);
    RCLCPP_INFO(this->get_logger(), "use_manual_inputs: %d", cfg_.use_manual_inputs);

    // IF-01, IF-03 in; IF-04, IF-06 out
    s_vehicle_state_ = this->create_subscription<ad_msgs::msg::VehicleState>(
        "vehicle_state", qos_profile, std::bind(&Control::CallbackVehicleState, this, std::placeholders::_1));
    s_driving_way_ = this->create_subscription<ad_msgs::msg::PolyfitLaneData>(
        "driving_way", qos_profile, std::bind(&Control::CallbackDrivingWay, this, std::placeholders::_1));
    s_manual_input_ = this->create_subscription<ad_msgs::msg::VehicleCommand>(
        "/manual_input", qos_profile, std::bind(&Control::CallbackManualInput, this, std::placeholders::_1));
    p_vehicle_command_ = this->create_publisher<ad_msgs::msg::VehicleCommand>("vehicle_command", qos_profile);
    p_computation_time_ = this->create_publisher<ad_msgs::msg::ComputationTime>("computation_time/control", qos_profile);

    t_run_node_ = this->create_wall_timer(
        std::chrono::milliseconds((int64_t)(1000 / cfg_.loop_rate_hz)),
        [this]() { this->Run(); });
}

void Control::ProcessParams() {
    this->get_parameter("control/ns", cfg_.vehicle_namespace);
    this->get_parameter("control/loop_rate_hz", cfg_.loop_rate_hz);
    this->get_parameter("control/look_ahead_distance", cfg_.look_ahead_distance);
    this->get_parameter("control/target_speed", cfg_.target_speed);
    this->get_parameter("control/longitudinal_kp", cfg_.longitudinal_kp);
    this->get_parameter("control/use_manual_inputs", cfg_.use_manual_inputs);
}

void Control::Run() {
    auto start = std::chrono::steady_clock::now();

    interface::VehicleCommand vehicle_command;
    if (cfg_.use_manual_inputs == true) {
        // Manual driving (launch argument manual:=true): pass /manual_input through, for checking the simulator only
        if (b_is_manual_input_ == false) {
            RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Wait for Manual Input ...");
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_manual_input_);
        vehicle_command = i_manual_input_;
    }
    else {
        if (b_is_vehicle_state_ == false) {
            RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Wait for Vehicle State ...");
            return;
        }
        if (b_is_driving_way_ == false) {
            RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Wait for Driving Way ...");
            return;
        }
        interface::VehicleState vehicle_state; {
            std::lock_guard<std::mutex> lock(mutex_vehicle_state_);
            vehicle_state = i_vehicle_state_;
        }
        interface::PolyfitLane driving_way; {
            std::lock_guard<std::mutex> lock(mutex_driving_way_);
            driving_way = i_driving_way_;
        }

        // Units
        vehicle_command.steering = alg_->LateralControl(driving_way);                                         // SWR-001
        std::pair<double, double> accel_brake = alg_->LongitudinalControl(vehicle_state, cfg_.target_speed);  // SWR-002, SWR-004
        vehicle_command.accel = accel_brake.first;
        vehicle_command.brake = accel_brake.second;
    }

    p_vehicle_command_->publish(ros2_bridge::UpdateVehicleCommand(vehicle_command));

    // Computation time of this period (SWR-003)
    ad_msgs::msg::ComputationTime computation_time;
    computation_time.period = 1000.0 / cfg_.loop_rate_hz;
    computation_time.computation_time =
        std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    p_computation_time_->publish(computation_time);
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Control>("control"));
    rclcpp::shutdown();
    return 0;
}
