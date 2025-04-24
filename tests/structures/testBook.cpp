#define CATCH_CONFIG_USE_APPROX
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "structures/Book.h" 
#include "structures/Order.h"

//These tests only aim to test the methods and attributes of the Book class function correctly. They do not aim to catch underlying malfunctions in sub-classes such as the Order class. Those should be captured elsewhere.

class TestBook: public Book{
public:
    bool isMarketMatchCalled = false;

    void marketMatch(Order& o) override{
        isMarketMatchCalled = true;
    }
};

class TestOrder: public Order{
    mutable bool isNotified = false;

public:
    TestOrder(int id, Side s, OrderType type,
        int tgtQ, double tgtP)
    : Order(id,s,type,tgtQ,tgtP){}

    void notify() const override{
        isNotified = true;
    }
};

struct BookTestHelper{
    //Getters
    static std::map<double,std::deque<Order>,std::less<double>>&  limitSell(Book& b){return b.limitSell;}
    static std::map<double,std::deque<Order>,std::greater<double>>& limitBuy(Book& b){return b.limitBuy;}

    //Access private methods
    static void marketMatch(Book& b, Order& o){return b.marketMatch(o);}
};

TEST_CASE("Base case addOrder: Limit Order added","[Book]"){
    TestBook b;
    
    TestOrder o(1,Side::BUY,OrderType::LIMIT,100,5.5);

    b.addOrder(o);

    REQUIRE(b.isMarketMatchCalled);
    
}

/*
TEST_CASE("Base case addOrder: Add order partial fill","[Book]"){
    //Test order gets added correctly to book
    //Test market orders get executed
}

TEST_CASE("Base case addOrder: Add order failed order","[Book]"){
    //Test order gets added correctly to book
    //Test market orders get executed
}

*/

