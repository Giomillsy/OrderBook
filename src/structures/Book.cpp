
#include "Book.hpp"
#include <variant>

void Book::addOrder(Order& o){
    //Adds an order to the book class
    
    switch (o.orderType){
    case (OrderType::MARKET):
        //Market order to match

        this->marketMatch(o);
        o.notify();
        return;    
    case OrderType::LIMIT:
        // Add order to queue based on price    

        // Attempt to cross the order before it's added
        this->marketMatch(o);
        if (o.unexecQuantity != 0){
            switch (o.side){
            case Side::SELL:
                limitSell[o.tgtPrice].push_back(o);
                break;
            case Side::BUY:
                limitBuy[o.tgtPrice].push_back(o);
                break;
            }
        }
        return;

    }
}

void Book::showOrders(){
    //Prints the current limit orders in the book

    std::cout << "Limit Orders Sell:\n";
    Book::showLimit(limitSell);
    std::cout << "Limit Orders Buy:\n";
    Book::showLimit(limitBuy);

    

}

template <typename Comparator>
void Book::showLimit(const  std::map<double, std::deque<Order>,Comparator>& limitBook){
    //Prints one side of the limit book to console

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

    //Holds multiple types solves the maps being functionally similar but semantically the different
    using LimitMapVariant = std::variant<LimitBuyMap*, LimitSellMap*>;
    LimitMapVariant limitMap;


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
        for (auto it = lMap -> begin(); it != lMap -> end(); ){
            auto& orderDeque = it->second; 


            //Iterates through the deque and removes limit orders as fully executed
            for (auto orderIt = orderDeque.begin(); orderIt != orderDeque.end();){
                auto& l = *orderIt;

                // Checks if it can cross the book instantly
                bool canCross;
                switch (o.side){
                case Side::BUY:
                    canCross = o.tgtPrice>l.tgtPrice;
                    break;
                case Side::SELL:
                    canCross = o.tgtPrice<l.tgtPrice;
                    break;
                default:
                    throw std::logic_error("Invalid Input: Wasn't buy or sell in side");
                }

                //If it can't cross the Book, the execution fails. All market orders cross with arbitarily high or low tgtPrices
                if (!canCross){return;}

                // Amount of quantity that can be executed with this limit order
                int qtyToExec = std::min(o.unexecQuantity,l.unexecQuantity);
                l.exec(qtyToExec,it->first);
                o.exec(qtyToExec,it->first);

                //If the limit order full executed, it needs to be removed
                // Or the itterator needs to go forward one
                if (l.unexecQuantity == 0) {
                    orderIt = orderDeque.erase(orderIt);
                } else {
                    ++orderIt;
                }
    
                //If the liqudity searching order is fully executed, end matching
                if (o.unexecQuantity == 0) return;
            }    

            //If the deque is empty, remove the price level
            if (orderDeque.empty()) {
                it = lMap -> erase(it);
            } else {
                ++it;
            }        
        
        
        }

        // Failed execution. Liquidity exhausted
       
        return;
    },limitMap);

}