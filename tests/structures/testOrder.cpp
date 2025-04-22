#define CATCH_CONFIG_USE_APPROX
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "structures/Order.h" 

struct OrderTestHelper {
    static int& orderID(Order& o) {return o.orderID;}
    static int& tgtQuantity(Order& o) {return o.tgtQuantity;}
    static double& tgtPrice(Order& o) {return o.tgtPrice;}
};


TEST_CASE("Basic Order Construction", "[Order]") {
    Order o(1, Side::BUY, OrderType::LIMIT, 150,1.1);
    
    REQUIRE(OrderTestHelper::orderID(o) == 1);
    REQUIRE(OrderTestHelper::tgtQuantity(o) == 150);
    REQUIRE(OrderTestHelper::tgtPrice(o) == Catch::Approx(1.1));
}


