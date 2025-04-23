#define CATCH_CONFIG_USE_APPROX
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "structures/Order.h" 

struct OrderTestHelper {
    //Getters
    static int& orderID(Order& o) {return o.orderID;}
    static int& tgtQuantity(Order& o) {return o.tgtQuantity;}
    static double& tgtPrice(Order& o) {return o.tgtPrice;}
    static int& execQuantity(Order& o) {return o.execQuantity;}
    static int& unexecQuantity(Order& o) {return o.unexecQuantity;}
    static double& execPrice(Order& o) {return o.execPrice;}
    static long long timestamp(Order& o) {return o.timestamp;}
    static double tickSize(Order& o){return o.tickSize;}

    //Access private functions
    static void exec(Order& o, int& qty, double prc){o.exec(qty,prc);}

};


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
