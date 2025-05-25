cmake_minimum_required(VERSION 3.14)
project(OrderBookProject)

# Set default to Release if not provided
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 23)

# Apply non-PIE and frame pointer flags globally
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -fno-omit-frame-pointer")
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

# Enable testing
enable_testing()

# Subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(benchmarks)

# Debug output
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")
