cmake_minimum_required(VERSION 3.14)
project(OrderBookProject)

set(CMAKE_CXX_STANDARD 23)

# Add source directory include path
include_directories(${CMAKE_SOURCE_DIR}/src)

# Book class tests
add_executable(testBook
    tests/structures/testBook.cpp
    src/structures/Book.cpp
    src/structures/Order.cpp
)

# Order Class tests
add_executable(testOrder
    tests/structures/testOrder.cpp
    src/structures/Order.cpp
)

# Add testBook and testOrder to ctest
enable_testing()
add_test(NAME RunTestBook COMMAND testBook)
add_test(NAME RunTestOrder COMMAND testOrder)
