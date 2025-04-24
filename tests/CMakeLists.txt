include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1 
)

FetchContent_MakeAvailable(Catch2)

add_executable(orderTests
    structures/testOrder.cpp
    structures/testBook.cpp
)

target_link_libraries(orderTests PRIVATE orderBook Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(orderTests)
