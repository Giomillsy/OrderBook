
#include "Book.hpp"
#include <variant>

void Book::addOrder(Order& o){
    // Adds an Order to the Order Book.
    // Limit orders, either go into the book or instantly cross
    // Market orders, either are rejected (no liquidity) or are executed
    
    switch (o.orderType){
    case (OrderType::MARKET):
        //Market order, needs to be executed immediately

        this->marketMatch(o);
        o.notify();
        return;    
    case OrderType::LIMIT:
        // Limit order, needs to either instantly cross the book or be added

        this->marketMatch(o); // Attempts to cross the book
        if (o.unexecQuantity != 0) {
            // Didn't cross the book
            switch (o.side){
                case Side::SELL:
                    limitSell[o.tgtPrice].push_back(o);
                    break;
                case Side::BUY:
                    limitBuy[o.tgtPrice].push_back(o);
                    break;
            }
        }
    }
}

void Book::showOrders(){
    // Mainly for debugging
    //Prints the current limit orders in the book to terminal

    std::cout << "Limit Orders Sell:\n";
    showLimit(limitSell);
    std::cout << "Limit Orders Buy:\n";
    showLimit(limitBuy);

}

template <typename Comparator>
void Book::showLimit(const  std::map<double, std::deque<Order>,Comparator>& limitBook){
    //Mainly for debugging
    //Prints one side of the limit book to console

    std::cout << "ID\t\tPrice\t\tSize\n";
    for (const auto& [price, orders] : limitBook) {
        for (auto& order : orders) {
            std::cout << order.orderID<< "\t\t" << order.tgtPrice << "\t\t" << order.unexecQuantity << "\n";
        }
    }    
}

void Book::marketMatch(Order& o){
    //Two main flows of logic
    //  - Matches a market order with a limit order
    //  - tries to match two limit orders, if it doesn't skip forward

    //LimitBuyMap and LimitSellMap are functionally similar but semantically the different. Perfect for variant
    using LimitMapVariant = std::variant<LimitBuyMap*, LimitSellMap*>;
    LimitMapVariant limitMap;

    //Finds the relevant side of the limit book
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

        //Iterates through the different prices
        for (auto it = lMap -> begin(); it != lMap -> end(); ){
            auto& orderDeque = it -> second;


            //Iterates through the orders at the price level, removing as them as they're filled
            for (auto orderIt = orderDeque.begin(); orderIt != orderDeque.end();){
                auto& limit = *orderIt;

                // Checks if it can cross the book instantly (for limit orders)
                bool canCross;
                switch (o.side){
                case Side::BUY:
                    canCross = o.tgtPrice >= limit.tgtPrice;
                    break;
                case Side::SELL:
                    canCross = o.tgtPrice <= limit.tgtPrice;
                    break;
                default:
                    throw std::logic_error("Invalid Input: Wasn't buy or sell in side");
                }

                //If it can't cross the Book, the execution fails.
                //All market orders cross since they have arbitrarily high or low tgtPrices
                if (!canCross){return;}

                // Amount of quantity that can be executed with this limit order
                int qtyToExec = std::min(o.unexecQuantity,limit.unexecQuantity);
                limit.exec(qtyToExec,it -> first);
                o.exec(qtyToExec,it -> first);

                //If the limit order full executed, it needs to be removed
                // Or the iterator needs to go forward one
                if (limit.unexecQuantity == 0) {
                    orderIt = orderDeque.erase(orderIt);
                } else {
                    ++orderIt;
                }
    
                //If the liqudity searching order is fully executed, end matching
                if (o.unexecQuantity == 0) return;
            }    

            //If no more orders at price level, remove the price level
            if (orderDeque.empty()) {
                it = lMap -> erase(it);
            } else {
                ++it;
            }
        }
        // Failed execution. Liquidity exhausted
    },limitMap);
}