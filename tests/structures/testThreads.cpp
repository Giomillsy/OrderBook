// Tests Concurrency logic

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "structures/SpscQ.hpp"
#include "structures/Order.hpp"

#include <iostream>
#include <random>
#include <optional>
#include <thread>




TEST_CASE("SpscQ: Quantity Summation check") {
    SpscQ<Order,1024> sq;
    Order o(1,Side::BUY,OrderType::LIMIT,10);
    constexpr int n = 1'000;


    //Producer thread
    std::jthread producer([&] {
       for (int i = 0; i < n; ++i) {
           sq.push(o);
       }
    });

    //Consumer Thread
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

    REQUIRE(realSum%10 == 0);
}