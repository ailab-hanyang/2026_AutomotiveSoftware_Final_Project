################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.5)
project(autonomous_driving)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

################################################################################
# Find and load build settings from external packages
################################################################################
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(ad_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)

## Add include directories
include_directories(
    include
    ${CMAKE_SOURCE_DIR}/../../bsw/system/interface
    ${CMAKE_SOURCE_DIR}/../../bsw/system/bridge
)

## Declare a C++ executable
## autonomous_driving: the provided node, used for manual driving only (launch argument manual:=true)
add_executable(${PROJECT_NAME}
    src/autonomous_driving_node.cpp
)

## Units (calculation functions without ROS): libraries that the nodes and the unit tests both use
add_library(perception_algorithm STATIC src/perception_algorithm.cpp)
add_library(control_algorithm STATIC src/control_algorithm.cpp)

## COMP-01 perception and COMP-02 control: one executable per node
add_executable(perception src/perception_node.cpp)
target_link_libraries(perception perception_algorithm)
add_executable(control src/control_node.cpp)
target_link_libraries(control control_algorithm)

## Add dependencies to the executables
foreach(target ${PROJECT_NAME} perception control)
  ament_target_dependencies(${target}
      rclcpp
      std_msgs
      ad_msgs
      tf2
      tf2_ros
  )
endforeach()

## Install the executable
install(TARGETS ${PROJECT_NAME} perception control
    DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME})

install(PROGRAMS
  scripts/simultaneous_inputs.py
  DESTINATION lib/${PROJECT_NAME}
)

################################################################################
# Find and load build settings from external packages
################################################################################
## Tests: files in test/ at the repository root, registered here so that colcon test runs them
if(BUILD_TESTING)
  set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

  # SWE.4 unit tests (gtest): one ament_add_gtest per test file in test/unit/
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_perception_algorithm
      ${REPO_ROOT}/test/unit/test_perception_algorithm.cpp
  )
  target_link_libraries(test_perception_algorithm perception_algorithm)
  ament_add_gtest(test_control_algorithm
      ${REPO_ROOT}/test/unit/test_control_algorithm.cpp
  )
  target_link_libraries(test_control_algorithm control_algorithm)

  # SWE.5 integration tests (launch_testing): one add_launch_test per test file in test/integration/
  find_package(launch_testing_ament_cmake REQUIRED)
  add_launch_test(${REPO_ROOT}/test/integration/test_control_io.py
      TARGET test_control_io
      TIMEOUT 60
  )
  add_launch_test(${REPO_ROOT}/test/integration/test_perception_control.py
      TARGET test_perception_control
      TIMEOUT 90
  )
  add_launch_test(${REPO_ROOT}/test/integration/test_perception_io.py
      TARGET test_perception_io
      TIMEOUT 60
  )

  # SWE.6 software tests with injected inputs (launch_testing): one add_launch_test per test file in test/scenario/
  add_launch_test(${REPO_ROOT}/test/scenario/test_invalid_speed.py
      TARGET test_invalid_speed
      TIMEOUT 60
  )
endif()

ament_package()