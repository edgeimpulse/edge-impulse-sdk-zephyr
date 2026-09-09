cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/model)

# This firmware uses X-Nucleo IKS02A1 shield
set(SHIELD x_nucleo_iks02a1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(inference-example)

include(utils/cmake/utils.cmake)

# Needed for colorful output
zephyr_compile_options(-fdiagnostics-color=always)

# Project include directories
set(INCLUDES
    .
    src
    )
include_directories(${INCLUDES})

# add all sources to the project
target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/sensors/ei_accelerometer.cpp)
target_sources(app PRIVATE src/sensors/ei_inertial.cpp)
target_sources(app PRIVATE src/inference/inferencing.cpp)
