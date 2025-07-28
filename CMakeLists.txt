cmake_minimum_required(VERSION 3.13.1)

include(edge-impulse-sdk/cmake/utils.cmake)
add_subdirectory(edge-impulse-sdk/cmake/zephyr)

zephyr_include_directories(.)
