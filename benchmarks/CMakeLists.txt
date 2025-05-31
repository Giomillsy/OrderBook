#Add the exe for benchmarking book
add_executable(bookBenchmark
        OrderBookBenchmark.cpp
        ${PROJECT_SOURCE_DIR}/src/structures/Book.cpp
        ${PROJECT_SOURCE_DIR}/src/structures/Order.cpp
)

#
target_include_directories(bookBenchmark PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Perf tuning flags for this target only
target_compile_options(bookBenchmark PRIVATE -O3 -g -fno-omit-frame-pointer)
set_target_properties(bookBenchmark PROPERTIES POSITION_INDEPENDENT_CODE OFF)
