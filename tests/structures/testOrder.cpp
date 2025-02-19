
#include "../../src/structures/Order.h"

//Order Tests
void testLimitOrder() {
    std::cout << "Running Test: Creating Limit Order\n";
    Order order(1, Side::BUY, OrderType::LIMIT, 
                50, 100.614); // Buy 50 stocks at $100.614

    order.printOrder();
    std::cout << "Limit Order Passed\n\n";
}

void testMarketUnitOrder() {
    std::cout << "Running test: Market Unit Order\n";
    Order order(2, Side::SELL, OrderType::MARKET, 
                200); // Sell 200 bonds at market price

    order.printOrder();
    std::cout << "Market Unit Order Passed\n\n";
}

int main() {

    std::cout << "Testing Book Class: \n";
    testLimitOrder();
    testMarketUnitOrder();
    
    std::cout << "All tests passed!\n";
    return 0;
}
