#define CATCH_CONFIG_USE_APPROX
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "structures/Order.hpp"
#include "TestHelpers.h"

TEST_CASE("Basic Order Construction", "[Order]") {
    Order o(1, Side::BUY, OrderType::LIMIT, 150,1.1);
    
    REQUIRE(OrderTestHelper::orderID(o) == 1);
    REQUIRE(OrderTestHelper::tgtQuantity(o) == 150);
    REQUIRE(OrderTestHelper::tgtPrice(o) == Catch::Approx(1.1));
}

TEST_CASE("Basic Full Exec", "[Order]"){

    

    //Create an order
    Order o(1, Side::BUY, OrderType::LIMIT, 100,1.1);

    //Execute the order
    int execQty = 100;
    double execPrc = 1.05;
    OrderTestHelper::exec(o,execQty ,execPrc);

    //Check output
    REQUIRE(OrderTestHelper::execPrice(o) == Catch::Approx(execPrc));
    REQUIRE(OrderTestHelper::unexecQuantity(o) == 0);
    REQUIRE(OrderTestHelper::execQuantity(o) == 100);

}

TEST_CASE("Basic Partial Exec", "[Order]"){


    //Create an order
    Order o(2, Side::SELL, OrderType::MARKET, 100,0.9);

    //Execute half the order
    int execQty = 50;
    double execPrc = 1;
    OrderTestHelper::exec(o,execQty ,execPrc);

    //Check output
    REQUIRE(OrderTestHelper::execPrice(o) == Catch::Approx(execPrc));
    REQUIRE(OrderTestHelper::unexecQuantity(o) == 50);
    REQUIRE(OrderTestHelper::execQuantity(o) == 50);

    //Execute the last half of the order
    execQty = 50;
    execPrc = 0.5;
    OrderTestHelper::exec(o,execQty ,execPrc);

    //Check output
    REQUIRE(OrderTestHelper::execPrice(o) == Catch::Approx(0.75));
    REQUIRE(OrderTestHelper::unexecQuantity(o) == 0);
    REQUIRE(OrderTestHelper::execQuantity(o) == 100);

}

TEST_CASE("Invalid Limit Order Execution", "[Order]"){

    Order o(4,Side::BUY,OrderType::LIMIT,25,25.2);

    int qty = 20;
    double prc = 26;

    //Require that if a limit order tries to buy above it's tgt price
    // that an error is thrown
    REQUIRE_THROWS_AS(OrderTestHelper::exec(o,qty,prc),std::logic_error);
}

TEST_CASE("Invalid Price Execution", "[Order]"){
    Order o(3,Side::BUY,OrderType::MARKET,1,10);

    int qty = 1;
    double prc = -10;

     //Price can never be negative, if it is an error has occured.
    REQUIRE_THROWS_AS(OrderTestHelper::exec(o,qty,prc),std::logic_error);
}

TEST_CASE("Invalid Quantity of Execution", "[Order]"){
    Order o(3,Side::BUY,OrderType::MARKET,1,10);

    int qty = 2;
    double prc = 10;

    //Can't execute more quantity than an order has
    REQUIRE_THROWS_AS(OrderTestHelper::exec(o,qty,prc),std::logic_error);
}
