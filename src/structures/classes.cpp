#include <iostream>

enum class Instrument{
    // Instrument being traded
    STOCKS,
    BONDS
};

enum class Side{
    //Whether the instrument is being bought or sold
    BUY,
    SELL
};

enum class OrderMode{
    // Whether it is a value/unit order. If it's limit it's unique
    UNIT,
    VALUE,
    LIMIT
};

enum class OrderType{
    // Two types of orders we're defining
    LIMIT,
    MARKET
};

class Order{
    
    std::string orderID;
    Instrument instrument;
    Side side;
    OrderMode orderMode;
    OrderType orderType;
    double tgtPrice;
    int tgtQuantity;
    double execPrice;
    int execQuantity;
    long long timestamp;
    


};