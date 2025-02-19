//File that tests /src/structures/classes.cpp
#include <iostream>
#include "../../src/structures/Book.h"


//Book Tests
void testAddOrder(){

    
    //Adding market orders
    Order oMS = Order{2, Side::SELL, OrderType::MARKET, 200};
    Order oMB = Order{2, Side::BUY, OrderType::MARKET, 100};
    Book book{};
    book.addOrder(oMS);
    book.addOrder(oMB);

    //Adding limit orders
    Order oLS1 = Order{1, Side::SELL, OrderType::LIMIT, 400,50.001};
    Order oLB2 = Order{2, Side::BUY, OrderType::LIMIT, 300,48.009};
    Order oLB3 = Order{3, Side::BUY, OrderType::LIMIT, 300,48.006};
    book.addOrder(oLS1);
    book.addOrder(oLB2);
    book.addOrder(oLB3);

    book.showOrders();


    

}

int main() {

    std::cout << "Testing Book Class: \n";
    testAddOrder();
    
    std::cout << "All tests passed!\n";
    return 0;
}
