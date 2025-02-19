
#include "Book.h"

void Book::addOrder(Order o){
    
    switch (o.orderType){
    case (OrderType::MARKET):
        switch (o.side){
        case (Side::BUY):
            marketBuyQ.push(o);
            return;
        case (Side::SELL):
            marketSellQ.push(o);
            return;    
        }
        break;    
    case OrderType::LIMIT:
        // Add order to queue based on price    

        switch(o.side){
        case (Side::BUY):
            limitBuyM[o.tgtPrice].push_back(o);
            return;
        case (Side::SELL):
            limitSellM[o.tgtPrice].push_back(o);
            return;
        }
        break;

    }
}

void Book::showOrders(){
    std::cout << marketSellQ.size() <<" market sell orders in queue \n";
    std::cout << marketBuyQ.size() <<" market buy orders in queue \n";

    std::cout << "Limit Orders Sell:\n";
    Book::showLimits(limitSellM);
    std::cout << "Limit Orders Buy:\n";
    Book::showLimits(limitBuyM);

    

}

void Book::showLimits(const  std::map<double, std::deque<Order>> limitBook){
    std::cout << "ID\t\tPrice\t\tSize\n";
    for (const auto& [price, orders] : limitBook) {
        int totalSize = 0;
        for (auto& order : orders) {
            std::cout << order.orderID<< "\t\t" << order.tgtPrice << "\t\t" << order.unexecQuantity << "\n";
        }
    }    
}