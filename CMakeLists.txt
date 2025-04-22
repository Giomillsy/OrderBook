cmake_minimum_required(VERSION 3.14)
project(OrderBookProject)

set(CMAKE_CXX_STANDARD 23)
enable_testing()

# Include your source and test directories
add_subdirectory(src)
add_subdirectory(tests)