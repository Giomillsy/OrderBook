#include "Order.h"
#include <chrono>
#include <cmath>
#include <map>
#include <set>
#include <limits>


long long Order::getCurrentTimestamp(){
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

double Order::roundToTickSize(double price){
    return std::round(price/tickSize)*tickSize;
}

void Order::exec(int qty, double prc){
    /* 3 flows:
    - Final execution that fully fills the rest of the order
    - Partial exec
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

    // Finds the cost of the execution
    double newValue = (execPrice*execQuantity)+(prc*qty);
    execPrice = newValue/(execQuantity+qty);
    execQuantity += qty;
    unexecQuantity -= qty;


    if (unexecQuantity == 0 && orderType == OrderType::LIMIT){
        //Checking if this is the final exec, which means communication needed
        this->notify();
    }
    return;

}

void Order::notify() const{
    /*
    Send message to user who submitted order

    Send different message if the order failed

    For now it does nothing
    */

    std::string failedOrder = "false";
    if (unexecQuantity != 0 ){
        failedOrder = "true";
    }

    return;
}

// Constructor with optional values
Order::Order(int id, Side s, OrderType type,
        int tgtQ, double tgtP)
    : orderID(id), 
    side(s),
    orderType(type),
    tgtQuantity(tgtQ), 
    tgtPrice(Order::roundToTickSize(tgtP)),
    execPrice(0),
    execQuantity(0),
    unexecQuantity(tgtQ),
    timestamp(Order::getCurrentTimestamp()){
        if (type == OrderType::MARKET){
            //Makes it always match at all prices
            if (s == Side::BUY){tgtPrice = std::numeric_limits<double>::max();}
            if (s == Side::SELL){tgtPrice = -std::numeric_limits<double>::max();}
        }
    }


int Order::getPrice() const {return tgtPrice;}

int Order::getUnexecQty() const {return unexecQuantity;}

int Order::getID() const {return orderID;}

void Order::printOrder() const {
    //Prints the order for debugging

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
