#ifndef ORDER_H  
#define ORDER_H

#include <iostream>
#include <chrono>
#include <queue>
#include <map>
#include <set>

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
    OrderType orderType;
    Side side;
    double tgtPrice;
    double execPrice; 
    long long timestamp;
    static constexpr double tickSize = 0.05;
    int orderID;
    int tgtQuantity;
    int execQuantity;
    int unexecQuantity;


    static long long getCurrentTimestamp();

    static double roundToTickSize(double price);

    virtual void exec(int qty, double p);

    virtual void notify() const;

public: 
    friend class OrderTestHelper; //Gives my unit tests for Order class access
    friend class Book; //Gives orderbook access to all attributes

    

    //Constructor
    Order(int id, Side s, OrderType type, 
        int tgtQ, double tgtPrice = 0.0);   

    int getPrice() const;

    int getUnexecQty() const;

    int getID() const;
        
    //Prints for debugging
    void printOrder() const;

};

#endif

