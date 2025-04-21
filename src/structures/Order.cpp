#include "Order.h"
#include <chrono>
#include <cmath>
#include <map>
#include <set>



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

    if (prc< 0.0 || qty < 0){
        throw std::runtime_error("Invalid Input: Price or Quantity is negative");
    }

    unsigned long value = static_cast<unsigned long>(std::round(prc*qty));
    unsigned long prevValue = static_cast<unsigned long>(std::round(execPrice*execQuantity));
    execPrice = static_cast<double>((value+prevValue)/(execQuantity+qty));
    execQuantity += qty;
    unexecQuantity -= qty;


    switch (orderType){
    case OrderType::LIMIT:
        if (unexecQuantity==0){
            //Checking if this is the final exec, which means communication needed
            this->notify();
        }
        break;
    }

    return;

}

void Order::notify() const{
    /*
    Send message to user who submitted order

    Send different message if the order failed

    For now it prints instead of communicating
    */

    std::string failedOrder = "false";
    if (unexecQuantity != 0 ){
        failedOrder = "true";
    }
    std::cout << "\nID\t\t" << "Exec Price\t" << "Exec Qty\t" << "Failed Order\n";
    std::cout << orderID << "\t\t" << execPrice << "\t\t" << execQuantity << "\t\t" << failedOrder << "\n\n";

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
    timestamp(Order::getCurrentTimestamp()){}


int Order::getPrice() const {return tgtPrice;}


void Order::printOrder() const {
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
