#ifndef ORDER_H  
#define ORDER_H

#include <iostream>

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

enum class OrderMode{
    // Whether it is a value/unit order. If it's limit it's unique
    UNIT,
    VALUE,
    LIMIT
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
    OrderMode orderMode;
    OrderType orderType;
    double tgtPrice;
    int tgtQuantity;
    double execPrice;
    int execQuantity;
    long long timestamp;

public:

    //Constructor
    Order(int id, Instrument instr, Side s, OrderMode mode, OrderType type,
        double tgtP = 0.0, int tgtQ = 0, double execP = 0.0, int execQ = 0, long long ts = 0);    

    //Prints for debugging
    void printOrder() const;

};

#endif