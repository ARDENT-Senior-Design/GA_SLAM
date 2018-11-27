cmake_minimum_required(VERSION 2.8.3)
project(ga_slam_visualization)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(ga_slam_visualization_PKGCONFIG REQUIRED ga_slam_cereal)

find_package(catkin REQUIRED COMPONENTS
    grid_map_core
    grid_map_msgs
    grid_map_ros
    roscpp
    std_msgs
    eigen_conversions
    tf
    tf_conversions
)

find_package(Eigen3 REQUIRED)

catkin_package(
    INCLUDE_DIRS
        ${Eigen_INCLUDE_DIRS}
    CATKIN_DEPENDS
        grid_map_core
        grid_map_ros
        grid_map_msgs
        roscpp
        std_msgs
        eigen_conversions
        tf
        tf_conversions
)

#add_library(
  # ${visualization} src/ga_slam_cereal/ga_slam_cereal/GridMapCereal.cc )##
  
  
include_directories(
    include

    ${catkin_INCLUDE_DIRS}
    ${Eigen_INCLUDE_DIRS}
    ${ga_slam_visualization_PKGCONFIG_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
      src/ga_slam_visualization_node.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${catkin_LIBRARIES}
    ${ga_slam_visualization_PKGCONFIG_LDFLAGS}
   #${visualization}
)

