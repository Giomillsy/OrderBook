add_executable(bookBenchmark
  OrderBookBenchmark.cpp
  ${PROJECT_SOURCE_DIR}/src/structures/Book.cpp
  ${PROJECT_SOURCE_DIR}/src/structures/Order.cpp
)

target_include_directories(bookBenchmark PRIVATE ${PROJECT_SOURCE_DIR}/src)
