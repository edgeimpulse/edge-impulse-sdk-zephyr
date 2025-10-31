cmake_minimum_required(VERSION 3.20.0)

if(DEFINED CONFIG_EDGE_IMPULSE_SDK)
  include(edge-impulse-sdk/cmake/utils.cmake)
  add_subdirectory(edge-impulse-sdk/cmake/zephyr)

  zephyr_include_directories(.)
endif(CONFIG_EDGE_IMPULSE_SDK)
