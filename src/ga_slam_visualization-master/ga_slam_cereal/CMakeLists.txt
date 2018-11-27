###############
### General ###
###############
cmake_minimum_required(VERSION 3.5.1)  # Ubuntu 16.04 (Xenial)

project(ga_slam_cereal)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include("${PROJECT_SOURCE_DIR}/cmake/functions.cmake")

set(TARGET_NAME ${PROJECT_NAME})
set(CMAKE_INCLUDE_DIR ${PROJECT_SOURCE_DIR})
set(CMAKE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${TARGET_NAME})

configure_build_type()
configure_compiler_flags()

####################
### Dependencies ###
####################
include(ExternalProject)
find_package(Eigen3 REQUIRED)
find_package(grid_map_core REQUIRED)

##############
### Cereal ###
##############
set(CEREAL_PROJECT_NAME cereal)

ExternalProject_Add(${CEREAL_PROJECT_NAME}
    PREFIX ${CMAKE_BINARY_DIR}/${CEREAL_PROJECT_NAME}
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(${CEREAL_PROJECT_NAME} source_dir)
set(CEREAL_INCLUDE_DIR ${source_dir}/include/)

#############
### Build ###
#############
include_directories(
    ${CMAKE_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
    ${grid_map_core_INCLUDE_DIRS}
    ${CEREAL_INCLUDE_DIR}
)

add_library(${TARGET_NAME} SHARED
    ${CMAKE_SOURCE_DIR}/GridMapCereal.cc
)

# Make the build wait for this dependency
add_dependencies(${TARGET_NAME}
    ${CEREAL_PROJECT_NAME}
)

target_link_libraries(${TARGET_NAME}
    ${grid_map_core_LIBRARIES}
)

##################
### Pkg Config ###
##################
configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/${TARGET_NAME}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc @ONLY
)

###############
### Install ###
###############
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc
    DESTINATION lib/pkgconfig
)

install(
    TARGETS ${TARGET_NAME}
    LIBRARY DESTINATION lib
)

install(
    DIRECTORY ${CMAKE_INCLUDE_DIR}/${TARGET_NAME}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CEREAL_INCLUDE_DIR}/cereal/
    DESTINATION include/${CEREAL_PROJECT_NAME}
)

