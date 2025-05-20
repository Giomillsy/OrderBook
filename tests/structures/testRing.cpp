// Unit tests for the SpscQ and Ring class, to test core functionality

#include "structures/SpscQ.h"
#include "structures/Order.h"
#include "TestHelpers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

TEST_CASE("push: Add order (no wrap)"){
    Ring<Order,10> r;
    TestRingHelper<Order,10> trh;

    Order o = {5,Side::BUY,OrderType::MARKET,10};


    REQUIRE(trh.doPush(r,o));
    REQUIRE(OrderTestHelper::orderID(o) == trh.getBuffer(r)[0].getID());
    REQUIRE(trh.getTail(r) == 1);
}