// Unit tests for the SpscQ and Ring class, to test core functionality

#include "structures/SpscQ.h"
#include "structures/Order.h"
#include "TestHelpers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

TEST_CASE("push: Add order (no wrap)"){
    Ring<Order,10> r;
    using TRH = TestRingHelper<Order,10>;
    using OTH = OrderTestHelper;

    Order o = {5,Side::BUY,OrderType::MARKET,10};


    REQUIRE(TRH::doPush(r,o));
    REQUIRE(OTH::orderID(o) == TRH::getBuffer(r)[0].getID());
    REQUIRE(TRH::getTail(r) == 1);
}

TEST_CASE("push: Add order (full)"){
    Ring<Order,1> r;
    using TRH = TestRingHelper<Order,1>;

    TRH::setTail(r,1);

    Order o = {5,Side::BUY,OrderType::MARKET,1};
    REQUIRE(!TRH::doPush(r,o));
}

TEST_CASE("push(): Add Order (wrap around)"){
    Ring<Order,3> r;
    using TRH = TestRingHelper<Order,3>;

    TestRingHelper<Order,3>::setTail(r,3);
    TRH::setHead(r,2);
    TRH::setTail(r,2);

    Order o = {5,Side::BUY,OrderType::MARKET,1};

    REQUIRE(TRH::doPush(r,o));
    REQUIRE(TRH::getBuffer(r)[2].getID() == o.getID());
    REQUIRE(TRH::getTail(r) == 0);
}

TEST_CASE("pop: Remove order (no wrap)") {
    Ring<Order,10> r;
    using TRH = TestRingHelper<Order,10>;
    using OTH = OrderTestHelper;

    Order o1 = {5, Side::BUY, OrderType::LIMIT, 1000};
    Order o2 = {5, Side::SELL, OrderType::LIMIT, 1100};

    TRH::setBuffer(r, o1, 0);
    TRH::setBuffer(r, o2, 1);
    TRH::setTail(r, 2);

    Order popped = TRH::doPop(r).value();
    REQUIRE(OTH::orderID(o1) == popped.getID());
    REQUIRE(TRH::getHead(r) == 1);
}

TEST_CASE("pop: Remove order (wrap around)") {
    Ring<Order,10> r;
    using TRH = TestRingHelper<Order,10>;
    using OTH = OrderTestHelper;

    Order o1 = {10, Side::BUY, OrderType::LIMIT, 1000};

    TRH::setHead(r, 9);
    TRH::setTail(r, 1);
    TRH::setBuffer(r, o1, 9);

    Order popped = TRH::doPop(r).value();
    REQUIRE(OTH::orderID(o1) == popped.getID());
    REQUIRE(TRH::getHead(r) == 0);
}

TEST_CASE("pop: Remove order (empty buffer)") {
    Ring<Order,10> r;
    using TRH = TestRingHelper<Order,10>;

    auto popped = TRH::doPop(r);
    REQUIRE(!popped.has_value());
}

TEST_CASE("Ring edge cases: zero and one slot capacity") {
    using TRH1 = TestRingHelper<Order, 1>;
    using TRH2 = TestRingHelper<Order, 2>;
    using OTH = OrderTestHelper;

    Ring<Order, 1> r1;
    Ring<Order, 2> r2;

    Order o = {1, Side::BUY, OrderType::LIMIT, 1000,5};

    // Ring with effective capacity of 0 (head == tail after one insert)
    REQUIRE(!TRH1::doPush(r1, o));
    REQUIRE(!TRH1::doPop(r1).has_value());

    // Ring with capacity for 1 real item
    REQUIRE(TRH2::doPush(r2, o));
    REQUIRE(!TRH2::doPush(r2, o));
    auto popped = TRH2::doPop(r2);
    REQUIRE(popped.has_value());
    REQUIRE(OTH::tgtPrice(*popped) == 5);
    REQUIRE(!TRH2::doPop(r2).has_value());
}
