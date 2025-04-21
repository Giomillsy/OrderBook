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
    Order oLB0 = Order{4, Side::BUY, OrderType::LIMIT, 300,49};
    Order oLS1 = Order{1, Side::SELL, OrderType::LIMIT, 400,50.001};
    Order oLB2 = Order{2, Side::BUY, OrderType::LIMIT, 300,48.009};
    Order oLB3 = Order{3, Side::BUY, OrderType::LIMIT, 300,48.006};
    Order oLB4 = Order{0, Side::BUY, OrderType::LIMIT, 300,49};
    book.addOrder(oLS1);
    book.addOrder(oLB2);
    book.addOrder(oLB3);
    book.addOrder(oLB4);
    book.addOrder(oLB0);


    book.showOrders();

}

void testMarketMatching(){
    //Adding limit orders
    std::cout << "----------MarketMatchingTest-------------\n";
    Book book{};

    Order oLB0 = Order{4, Side::BUY, OrderType::LIMIT, 300,49};
    Order oLS1 = Order{1, Side::SELL, OrderType::LIMIT, 400,50.001};
    Order oLB2 = Order{2, Side::BUY, OrderType::LIMIT, 300,48.009};
    Order oLB3 = Order{3, Side::BUY, OrderType::LIMIT, 300,48.006};
    Order oLB4 = Order{0, Side::BUY, OrderType::LIMIT, 300,49};
    book.addOrder(oLS1);
    book.addOrder(oLB2);
    book.addOrder(oLB3);
    book.addOrder(oLB4);
    book.addOrder(oLB0);

    std::cout << "Before Exec: \n\n";
    book.showOrders();

    std::cout << "Exec: \n\n";
    Order oM0 = Order{5,Side::SELL,OrderType::MARKET,10000};
    book.addOrder(oM0);
    
    std::cout << "After Exec: \n\n";
    book.showOrders();

    std::cout << "----------------END OF TEST-----------\n";

}

void testLimitMatching(){
    //Adding limit orders
    Book book{};
    std::cout << "----------LimitMatchingTest-------------\n";

    Order oLB0 = Order{4, Side::BUY, OrderType::LIMIT, 300,49};
    Order oLB2 = Order{2, Side::BUY, OrderType::LIMIT, 300,48.009};
    Order oLB3 = Order{3, Side::BUY, OrderType::LIMIT, 300,48.006};
    Order oLB4 = Order{0, Side::BUY, OrderType::LIMIT, 300,49};
    book.addOrder(oLB2);
    book.addOrder(oLB3);
    book.addOrder(oLB4);
    book.addOrder(oLB0);

    std::cout << "Before Exec: \n\n";
    book.showOrders();

    Order oM0 = Order{5,Side::SELL,OrderType::LIMIT,100,1};
    book.addOrder(oM0);
    std::cout << "After Exec: \n\n";
    book.showOrders();

    std::cout << "----------------END OF TEST-----------\n";

}


int main() {

    std::cout << "Testing Book Class: \n";
    testLimitMatching();
    testMarketMatching();
    
    std::cout << "All tests passed!\n";
    return 0;
}
