#include "classes.h"

// Constructor with optional values
Order::Order(int id, Instrument instr, Side s, OrderMode mode, OrderType type,
        double tgtP, int tgtQ, double execP, int execQ, long long ts)
    : orderID(id), 
    instrument(instr), 
    side(s), 
    orderMode(mode), 
    orderType(type),
    tgtPrice(tgtP), 
    tgtQuantity(tgtQ), 
    execPrice(execP), 
    execQuantity(execQ), 
    timestamp(ts) {};

    

void Order::printOrder() const {
    std::cout << "OrderID: " << orderID << "\n"
              << "Instrument: " << (instrument == Instrument::STOCKS ? "Stocks" : "Bonds") << "\n"
              << "Side: " << (side == Side::BUY ? "Buy" : "Sell") << "\n"
              << "Order Mode: " << (orderMode == OrderMode::UNIT ? "Unit" : 
                                    orderMode == OrderMode::VALUE ? "Value" : "Limit") << "\n"
              << "Order Type: " << (orderType == OrderType::LIMIT ? "Limit" : "Market") << "\n"
              << "Target Price: " << tgtPrice << "\n"
              << "Target Quantity: " << tgtQuantity << "\n"
              << "Execution Price: " << execPrice << "\n"
              << "Execution Quantity: " << execQuantity << "\n"
              << "Timestamp: " << timestamp << "\n"
              << "---------------------------\n";
}