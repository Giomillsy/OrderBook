#ifndef BOOK_H  
#define BOOK_H

#include <map>
#include <queue>
#include <deque>

#include "Order.hpp"


class Book{
// The order book, which contains and manages the orders constructed by the Order class
private:
    
    // Creating an alias type for both sides of the order book
    using LimitSellMap = std::map<double, std::deque<Order>, std::less<>>;
    using LimitBuyMap  = std::map<double, std::deque<Order>,std::greater<>>;
    
    // Each side of the order book is a map organised price levels
    // With each price level being FIFO queue.
    LimitSellMap limitSell;
    LimitBuyMap  limitBuy;

    // Prints out the limit orders on one side of the book
    template <typename Comparator>
    void showLimit(const std::map<double, std::deque<Order>,Comparator>& limitBook);

    // Market matching logic. For crossing two orders
    virtual void marketMatch(Order& o);

public:

    virtual ~Book() = default;

    friend struct BookTestHelper; //Helper for unit testing

    void addOrder(Order& o); //Adds an order to the Limit book.

    void showOrders(); // Prints orders on both side of the book
    
};

#endif