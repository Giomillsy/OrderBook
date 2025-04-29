cmake_minimum_required(VERSION 3.14)
project(OrderBookProject)

set(CMAKE_CXX_STANDARD 23)

# For debugging
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

enable_testing()

# Include your source and test directories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(benchmarks)