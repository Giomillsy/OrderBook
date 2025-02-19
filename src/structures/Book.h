#ifndef BOOK_H  
#define BOOK_H

#include <map>
#include <set>
#include <queue>
#include <deque>
#include "Order.h"


class Book{
// The order book, which contains and manages the orders
private:
    std::queue<Order> marketSellQ; // Market sell orders
    std::queue<Order> marketBuyQ; // Queue of market buy orders
    
    std::map<double,std::deque<Order>> limitSellM; //Limit sell orders
    std::map<double,std::deque<Order>> limitBuyM; //Limit buy orders

    // Prints out the limit orders on one side of the book
    void showLimits(const std::map<double, std::deque<Order>>);

public:

#ifdef TESTING_MODE
    friend void testAddOrder();
#endif

    void addOrder(Order o);

    void showOrders();
    
};

#endif