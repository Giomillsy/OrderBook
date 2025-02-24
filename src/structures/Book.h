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
    
    std::map<double,std::deque<Order>,std::less<double>> limitSell; //Limit sell orders
    std::map<double,std::deque<Order>,std::greater<double>> limitBuy; //Limit buy orders

    // Prints out the limit orders on one side of the book
    template <typename Comparator>
    void showLimit(const std::map<double, std::deque<Order>,Comparator>);

    void marketMatch(Order& o);

public:

#ifdef TESTING_MODE
    friend void testAddOrder();
#endif

    void addOrder(Order o);

    void showOrders(); // Prints orders on both side of the book
    
};

#endif