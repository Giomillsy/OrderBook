
#include "Book.h"
#include <variant>

void Book::addOrder(Order& o){
    
    switch (o.orderType){
    case (OrderType::MARKET):
        this->marketMatch(o);
        o.notify();
        return;    
    case OrderType::LIMIT:
        // Add order to queue based on price    

        // Attempt to cross the order before it's added
        this->marketMatch(o);
        if (o.unexecQuantity != 0){
            limitBuy[o.tgtPrice].push_back(o);
        }
        return;

    }
}

void Book::showOrders(){

    std::cout << "Limit Orders Sell:\n";
    Book::showLimit(limitSell);
    std::cout << "Limit Orders Buy:\n";
    Book::showLimit(limitBuy);

    

}

template <typename Comparator>
void Book::showLimit(const  std::map<double, std::deque<Order>,Comparator> limitBook){
    std::cout << "ID\t\tPrice\t\tSize\n";
    for (const auto& [price, orders] : limitBook) {
        int totalSize = 0;
        for (auto& order : orders) {
            std::cout << order.orderID<< "\t\t" << order.tgtPrice << "\t\t" << order.unexecQuantity << "\n";
        }
    }    
}

void Book::marketMatch(Order& o){
    //Matches a market order with an order in the limit book

    //Holds multiple types solves the maps being functionally similar but semtically the different
    std::variant<
        std::map<double,std::deque<Order>,std::greater<double>>*,
        std::map<double,std::deque<Order>,std::less<double>>*
    > limitMap;

    //Gets the relevant side of the limit book
    switch (o.side){
    case Side::SELL:
        limitMap = &limitBuy;
        break;
    case Side::BUY:
        limitMap = &limitSell;
        break;
    }

    //Visit safely gets the type being used in the variant
    std::visit([&o](auto& lMap){
        int qtyToExec = 0;

        //Iterates through the map
        for (auto it = lMap->begin(); it != lMap->end(); ){
            auto& orderDeque = it->second; 

            //Iterates through the deque and removes limit orders as fully executed
            for (auto orderIt = orderDeque.begin(); orderIt != orderDeque.end();){
                Order& l = *orderIt;

                bool canCross;
                switch (o.side){
                case Side::BUY:
                    canCross = o.tgtPrice>l.tgtPrice;
                    break;
                case Side::SELL:
                    canCross = o.tgtPrice<l.tgtPrice;
                    break;
                }

                if (!canCross){return;}

                if (o.unexecQuantity <= l.unexecQuantity){
                    //market Order gets filled in, but limit doesn't
                    //Or both are filled
                    qtyToExec = o.unexecQuantity;
                    l.exec(qtyToExec,it->first);
                    o.exec(qtyToExec,it->first);

                    //Remove order if fully executed
                    if (l.unexecQuantity == 0) {
                        orderIt = orderDeque.erase(orderIt); 
                    } else {
                        ++orderIt; 
                    }
                    return;

                } else {
                    // Limit Order gets filled in, but market doesn't
                    qtyToExec = l.unexecQuantity;
                    o.exec(qtyToExec, it->first);
                    l.exec(qtyToExec, it->first);
                    orderIt = orderDeque.erase(orderIt);
                }
            }    

            //If the deque is empty, remove the price level
            if (orderDeque.empty()) {
                it = lMap->erase(it);
            } else {
                ++it;
            }        
        
        
        }

        // Failed execution
       
        return;
    },limitMap);

}