#include "Order.hpp"

#include <chrono>
#include <cmath>
#include <map>
#include <set>
#include <limits>


long long Order::getCurrentTimestamp() noexcept {
    // Returns the current time, using chrono

    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

double Order::roundToTickSize(const double price) noexcept {
    // Used to round all order prices to the nearest tick, defined by tick size

    return std::round(price/tickSize)*tickSize;
}

void Order::exec(int qty, double prc){
    /* 3 flows:
    - Final execution that fully fills the rest with the order
    - Partial exec
    - Error caught
    */


    // Error catches
    if (prc< 0.0 || qty <= 0){
        throw std::logic_error("Invalid Input: Price or Quantity is negative or zero");
    }
    if (orderType == OrderType::LIMIT && side == Side::BUY && prc > tgtPrice){
        throw std::logic_error("Invalid Input: Invalid price, prc > tgtPrice for limit buy");        
    }
    if (orderType == OrderType::LIMIT && side == Side::SELL && prc < tgtPrice){
        throw std::logic_error("Invalid Input: Invalid price, prc < tgtPrice for limit sell");          
    }
    if (qty > unexecQuantity){
        throw std::logic_error("Invalid Input: Quantity to be executed greater than oustanding quantity");
    }

    // Finds the value/quantity of the orders executed
    const double newValue = (execPrice*execQuantity)+(prc*qty);
    execPrice = newValue/(execQuantity+qty);
    execQuantity += qty;
    unexecQuantity -= qty;

    //Checking if this is the final exec, which means communication needed
    if (unexecQuantity == 0 && orderType == OrderType::LIMIT){
        // If it's a fully executed order, notify the person who placed the order
        this->notify();
    }

}

void Order::notify() const{
    /*
    Send message to user who submitted order

    Send different message if the order failed

    For now it does nothing
    */

}

// Constructor
Order::Order(const int id, const Side s, const OrderType type,
        const int tgtQ,const double tgtP)
    : orderType(type),
    side(s),
    tgtPrice(roundToTickSize(tgtP)),
    execPrice(0),
    timestamp(getCurrentTimestamp()),
    orderID(id),
    tgtQuantity(tgtQ),
    execQuantity(0),
    unexecQuantity(tgtQ){
        if (type == OrderType::MARKET){
            //Market orders provide liquidity at all prices, this ensures that
            if (s == Side::BUY){tgtPrice = std::numeric_limits<double>::max();}
            if (s == Side::SELL){tgtPrice = -std::numeric_limits<double>::max();}
        }
    }

//Getters
double Order::getPrice() const noexcept {return tgtPrice;}
int Order::getUnexecQty() const noexcept {return unexecQuantity;}
int Order::getID() const noexcept {return orderID;}

//Prints the order for debugging
void Order::printOrder() const noexcept{

    std::cout << "OrderID: " << orderID << "\n"
              << "Side: " << (side == Side::BUY ? "Buy" : "Sell") << "\n"
              << "Order Type: " << (orderType == OrderType::LIMIT ? "Limit" : "Market") << "\n"
              << "Target Price: " << tgtPrice << "\n"
              << "Target Quantity: " << tgtQuantity << "\n"
              << "Execution Price: " << execPrice << "\n"
              << "Execution Quantity: " << execQuantity << "\n"
              << "Timestamp: " << timestamp << "\n"
              << "---------------------------\n";
}
