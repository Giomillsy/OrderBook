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
