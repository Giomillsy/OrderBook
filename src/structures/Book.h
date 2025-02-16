#ifndef BOOK_H  
#define BOOK_H

#include <map>
#include <set>
#include <queue>
#include "Order.h"


class Book{
// The order book, which contains and manages the orders
private:
    std::queue<Order> marketSellQ; // Market sell orders
    std::queue<Order> marketBuyQ; // Queue of market buy orders
    
    std::map<double,std::multiset<Order,Order::sizeCompare>> limitSellM; //Limit sell orders
    std::map<double,std::multiset<Order, Order::sizeCompare>> limitBuyM; //Limit buy orders

    // Prints out the limit orders on one side of the book
    void showLimits(const std::map<double, std::multiset<Order, Order::sizeCompare>>& limitBook);

public:

#ifdef TESTING_MODE
    friend void testAddOrder();
#endif

    void addOrder(Order o);

    void showOrders();
    
};

#endif