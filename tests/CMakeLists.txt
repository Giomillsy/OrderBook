# Sanitizer options
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if (ENABLE_TSAN AND ENABLE_ASAN)
    message(FATAL_ERROR "Cannot enable both TSAN and ASAN at the same time.")
endif()

#Catch2 Setup
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
    structures/testRing.cpp
    structures/TestHelpers.h
    structures/testThreads.cpp
)

# Link against main library and Catch2
target_link_libraries(orderTests PRIVATE orderBook Catch2::Catch2WithMain)



if (ENABLE_TSAN)
    message(STATUS "ThreadSanitizer enabled")
    target_compile_options(orderTests PRIVATE -fsanitize=thread -g -O1)
    target_link_options(orderTests PRIVATE -fsanitize=thread)
elseif (ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    target_compile_options(orderTests PRIVATE -fsanitize=address -g -O1)
    target_link_options(orderTests PRIVATE -fsanitize=address)
endif()


if (ENABLE_TSAN)
    add_test(NAME orderTests COMMAND orderTests)
else()
    include(Catch)
    catch_discover_tests(orderTests)
endif()
