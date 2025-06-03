// Tests Concurrency logic

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "structures/SpscQ.hpp"
#include "structures/Order.hpp"

#include <iostream>
#include <random>
#include <optional>
#include <thread>

TEST_CASE("SpscQ: handles concurrent push/pop correctly", "[SpscQ][Threads]") {
    SpscQ<Order,1024> sq;
    Order o(1,Side::BUY,OrderType::LIMIT,10);
    constexpr int n = 1'000;


    //Start producer thread: pushes 'n' copies of the same order
    std::jthread producer([&] {
       for (int i = 0; i < n; ++i) {
           sq.push(o);
       }
    });

    // Start consumer thread: sums up unexecuted quantity of popped orders
    int realSum = 0;
    std::jthread consumer([&] {
        for (int i = 0; i < n; ++i) {
            std::optional<Order> popped = sq.pop();
            if (popped.has_value()) {
                realSum += popped.value().getUnexecQty();
            }
        }
    });

    producer.join();
    consumer.join();

    // Expect total quantity to be a multiple of 10  since (10 units × n pushes)
    REQUIRE(realSum%10 == 0);
}