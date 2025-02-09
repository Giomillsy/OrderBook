//File that tests /src/structures/classes.cpp
#include <iostream>
#include "../../src/structures/classes.h"


void testLimitOrder() {
    std::cout << "Running Test: Creating Limit Order\n";
    Order order(1, Instrument::STOCKS, Side::BUY, OrderType::LIMIT, 
                50, 100.614); // Buy 50 stocks at $100

    order.printOrder();
    std::cout << "Limit Order Passed\n\n";
}

void testMarketUnitOrder() {
    std::cout << "Running test: Market Unit Order\n";
    Order order(2, Instrument::BONDS, Side::SELL, OrderType::MARKET, 
                200); // Sell 200 bonds at market price

    order.printOrder();
    std::cout << "Market Unit Order Passed\n\n";
}

int main() {

    std::cout << "Testing Order Class: \n";
    testLimitOrder();
    testMarketUnitOrder();
    
    std::cout << "All tests passed!\n";
    return 0;
}