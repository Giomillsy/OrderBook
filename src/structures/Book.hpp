#ifndef BOOK_H  
#define BOOK_H

#include <map>
#include <queue>
#include <deque>
#include "Order.hpp"


class Book{
// The order book, which contains and manages the orders
private:
    
    using LimitSellMap = std::map<double, std::deque<Order>, std::less<>>;
    using LimitBuyMap  = std::map<double, std::deque<Order>,std::greater<>>;

    LimitSellMap limitSell;
    LimitBuyMap  limitBuy;

    // Prints out the limit orders on one side of the book
    template <typename Comparator>
    void showLimit(const std::map<double, std::deque<Order>,Comparator>& limitBook);

    virtual void marketMatch(Order& o);

public:

    //Destructor
    virtual ~Book() = default;

friend struct BookTestHelper; //Helper for unit testing

    void addOrder(Order& o);

    void showOrders(); // Prints orders on both side of the book
    
};

#endif