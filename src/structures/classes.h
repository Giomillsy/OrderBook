#ifndef ORDER_H  
#define ORDER_H

#include <iostream>
#include <chrono>

enum class Instrument{
    // Instrument being traded
    STOCKS,
    BONDS
};

enum class Side{
    //Whether the instrument is being bought or sold
    BUY,
    SELL
};

enum class OrderType{
    // Two types of orders we're defining
    LIMIT,
    MARKET
};

class Order{
// Defines my data structure for the orders

private:
    int orderID;
    Instrument instrument;
    Side side;
    OrderType orderType;
    int tgtQuantity;
    int tgtPrice;
    double execPrice;
    int execQuantity;
    long long timestamp;
    static double tickSize;

    static long long getCurrentTimestamp();

    static long roundToTickSize(long price);

public:

    //Constructor
    Order(int id, Instrument instr, Side s, OrderType type, 
        int tgtQ, double tgtPrice = 0.0);    

    //Prints for debugging
    void printOrder() const;

};

#endif

/*
class Book{
// The order book, which contains and manages the orders
private:
}
*/
