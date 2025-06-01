#Create a library called 'orderBook'
add_library(orderBook
    structures/Order.cpp
    structures/Book.cpp
    structures/Ring.hpp
    structures/SpscQ.hpp
)

#Make headers visible
target_include_directories(orderBook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})