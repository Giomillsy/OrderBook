//File that tests /src/structures/classes.cpp
#include <iostream>
#include "src/structures/classes.h"

void testLimitOrder() {
    std::cout << "Running Test: Creating Limit Order\n";
    Order order(1, Instrument::STOCKS, Side::BUY, OrderMode::LIMIT, OrderType::LIMIT, 
                100.0, 50); // Buy 50 stocks at $100

    order.printOrder();
    std::cout << "Limit Order Passed\n\n";
}

void testMarketUnitOrder() {
    std::cout << "Running test: Market Unit Order\n";
    Order order(2, Instrument::BONDS, Side::SELL, OrderMode::UNIT, OrderType::MARKET, 
                0.0, 200); // Sell 200 bonds at market price

    order.printOrder();
    std::cout << "Market Unit Order Passed\n\n";
}

void testMarketValueOrder() {
    std::cout << "Running test: Market Value Order\n";
    Order order(3, Instrument::STOCKS, Side::BUY, OrderMode::VALUE, OrderType::MARKET, 
                0.0, 0, 0.0, 0, 1700000002); // Buy £10,000 worth of stocks

    order.printOrder();
    std::cout << "Market Value Order Test Passed\n\n";
}

int main() {

    std::cout << "Testing Order Class: \n";
    testLimitOrder();
    testMarketUnitOrder();
    testMarketValueOrder();
    
    std::cout << "All tests passed!\n";
    return 0;
}