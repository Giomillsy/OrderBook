#pragma once

#include <iostream>
#include <chrono>

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


    static long long getCurrentTimestamp() noexcept;

    static double roundToTickSize(double price) noexcept;

    virtual void exec(int qty, double p);

    virtual void notify() const;

public: 
    friend class OrderTestHelper; //Gives my unit tests for Order class access
    friend class Book; //Gives orderbook access to all attributes

    //Default Constructor
    Order()
        : orderType(OrderType::LIMIT), side(Side::BUY), tgtPrice(0.0), execPrice(0.0),
          timestamp(0), orderID(-1), tgtQuantity(0), execQuantity(0), unexecQuantity(0) {}

    //Destructor
    virtual ~Order() = default;

    //Main Constructor
    Order(int id, Side s, OrderType type,
        int tgtQ, double tgtPrice = 0.0);

    // Getters
    [[nodiscard]] double getPrice() const noexcept;
    [[nodiscard]] int getUnexecQty() const noexcept;
    [[nodiscard]] int getID() const noexcept;
        
    //Prints for debugging
    void printOrder() const noexcept;

};


