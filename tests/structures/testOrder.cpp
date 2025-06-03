#define CATCH_CONFIG_USE_APPROX

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "structures/Order.hpp"
#include "TestHelpers.h"

TEST_CASE("Construction: Happy Path Order init", "[Order]") {
    Order o(1, Side::BUY, OrderType::LIMIT, 150,1.1);
    
    REQUIRE(OrderTestHelper::orderID(o) == 1);
    REQUIRE(OrderTestHelper::tgtQuantity(o) == 150);
    REQUIRE(OrderTestHelper::tgtPrice(o) == Catch::Approx(1.1));
}

TEST_CASE("exec: Happy path Full Exec", "[Order]"){

    Order o(1, Side::BUY, OrderType::LIMIT, 100,1.1);

    int execQty = 100;
    double execPrc = 1.05;
    OrderTestHelper::exec(o,execQty ,execPrc);

    //Check output
    REQUIRE(OrderTestHelper::execPrice(o) == Catch::Approx(execPrc));
    REQUIRE(OrderTestHelper::unexecQuantity(o) == 0);
    REQUIRE(OrderTestHelper::execQuantity(o) == 100);

}

TEST_CASE("exec: Partial Exec", "[Order]"){


    Order o(2, Side::SELL, OrderType::MARKET, 100,0.9);

    //Execute half the order
    int execQty = 50;
    double execPrc = 1;
    OrderTestHelper::exec(o,execQty ,execPrc);

    REQUIRE(OrderTestHelper::execPrice(o) == Catch::Approx(execPrc));
    REQUIRE(OrderTestHelper::unexecQuantity(o) == 50);
    REQUIRE(OrderTestHelper::execQuantity(o) == 50);

    //Execute the last half of the order
    execQty = 50;
    execPrc = 0.5;
    OrderTestHelper::exec(o,execQty ,execPrc);

    REQUIRE(OrderTestHelper::execPrice(o) == Catch::Approx(0.75));
    REQUIRE(OrderTestHelper::unexecQuantity(o) == 0);
    REQUIRE(OrderTestHelper::execQuantity(o) == 100);

}

TEST_CASE("exec: limit buy rejects execution above target price", "[Order]"){

    Order o(4,Side::BUY,OrderType::LIMIT,25,25.2);

    int qty = 20;
    double prc = 26;

    // Limit BUY at 25.2 should not allow execution at 26.0
    REQUIRE_THROWS_AS(OrderTestHelper::exec(o,qty,prc),std::logic_error);
}

TEST_CASE("exec: negative price throws exception", "[Order]"){
    Order o(3,Side::BUY,OrderType::MARKET,1,10);

    int qty = 1;
    double prc = -10;

    REQUIRE_THROWS_AS(OrderTestHelper::exec(o,qty,prc),std::logic_error);
}

TEST_CASE("exec: quantity greater than order throws exception", "[Order]"){
    Order o(3,Side::BUY,OrderType::MARKET,1,10);

    int qty = 2;
    double prc = 10;

    REQUIRE_THROWS_AS(OrderTestHelper::exec(o,qty,prc),std::logic_error);
}

TEST_CASE("exec: zero quantity throws exception", "[Order]") {
    Order o(6, Side::SELL, OrderType::LIMIT, 10, 1.0);

    int qty = 0;
    double prc = 1.0;

    REQUIRE_THROWS_AS(OrderTestHelper::exec(o, qty, prc), std::logic_error);
}