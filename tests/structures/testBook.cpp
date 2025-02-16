//File that tests /src/structures/classes.cpp
#include <iostream>
#include "../../src/structures/Book.h"


//Book Tests
void testAddOrder(){

    
    //Adding market orders
    Order oMS = Order{2, Instrument::BONDS, Side::SELL, OrderType::MARKET, 200};
    Order oMB = Order{2, Instrument::BONDS, Side::BUY, OrderType::MARKET, 100};
    Book book{};
    book.addOrder(oMS);
    book.addOrder(oMB);

    //Adding limit orders
    Order oLS = Order{2, Instrument::BONDS, Side::SELL, OrderType::LIMIT, 400,50.001};
    Order oLB = Order{2, Instrument::STOCKS, Side::BUY, OrderType::LIMIT, 300,48.009};
    book.addOrder(oLS);
    book.addOrder(oLB);

    book.showOrders();


    

}

int main() {

    std::cout << "Testing Book Class: \n";
    testAddOrder();
    
    std::cout << "All tests passed!\n";
    return 0;
}
