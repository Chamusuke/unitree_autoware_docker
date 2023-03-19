cmake_minimum_required(VERSION 2.8.3)
project(unitree_legged_real)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    geometry_msgs
    unitree_legged_msgs
    sensor_msgs
)

catkin_package()

include_directories(
    include
    ${Boost_INCLUDE_DIR}
    ${catkin_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "-O3")

include_directories(/home/autoware/shared_dir/autoware_docker_ws/src/unitree_legged_sdk-3.4.2/include)
link_directories(/home/autoware/shared_dir/autoware_docker_ws/src/unitree_legged_sdk-3.4.2/lib)
string(CONCAT LEGGED_SDK_NAME libunitree_legged_sdk_amd64.so)
set(EXTRA_LIBS ${LEGGED_SDK_NAME} lcm)

add_executable(lcm_server /home/autoware/shared_dir/autoware_docker_ws/src/unitree_legged_sdk-3.4.2/examples/lcm_server.cpp)
target_link_libraries(lcm_server ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(lcm_server ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(position_lcm src/exe/position_mode.cpp)
target_link_libraries(position_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(position_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(velocity_lcm src/exe/velocity_mode.cpp)
target_link_libraries(velocity_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(velocity_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(torque_lcm src/exe/torque_mode.cpp)
target_link_libraries(torque_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(torque_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(walk_lcm src/exe/walk_mode.cpp)
target_link_libraries(walk_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(walk_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(unitree_autoware_lcm src/exe/unitree_control.cpp)
target_link_libraries(unitree_autoware_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(unitree_autoware_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(test_control_lcm src/exe/test_unitree.cpp)
target_link_libraries(test_control_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(test_control_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(imu_lcm src/exe/imu.cpp)
target_link_libraries(imu_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(imu_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(filter_lcm src/exe/filter.cpp)
target_link_libraries(filter_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(filter_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(key_control_lcm src/exe/teleop_twist_keyboard.cpp)
target_link_libraries(key_control_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(key_control_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(climb_control_lcm src/exe/climbing_unitree.cpp)
target_link_libraries(climb_control_lcm ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(climb_control_lcm ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(read_cmd src/exe/read.cpp)
target_link_libraries(read_cmd ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(read_cmd ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(safety_filter src/exe/safety.cpp)
target_link_libraries(safety_filter ${EXTRA_LIBS} ${catkin_LIBRARIES})
add_dependencies(safety_filter ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})