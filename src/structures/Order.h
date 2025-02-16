#ifndef ORDER_H  
#define ORDER_H

#include <iostream>
#include <chrono>
#include <queue>
#include <map>
#include <set>

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
    OrderType orderType;
    Side side;
    Instrument instrument;  
    double tgtPrice;
    double execPrice; 
    long long timestamp;
    static double tickSize;
    int orderID;
    int tgtQuantity;
    int execQuantity;


    static long long getCurrentTimestamp();

    static double roundToTickSize(double price);

public: 

    friend class Book; //Gives orderbook access to all attributes

    //Constructor
    Order(int id, Instrument instr, Side s, OrderType type, 
        int tgtQ, double tgtPrice = 0.0);   
        
    struct sizeCompare{
        //Used for sorting the multiset. Largest orders sit at the top
        bool operator()(const Order& a, const Order& b) const {
            return a.tgtQuantity < b.tgtQuantity;
        }
    };

    //Prints for debugging
    void printOrder() const;

};

#endif

