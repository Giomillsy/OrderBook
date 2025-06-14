#define CATCH_CONFIG_USE_APPROX
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "structures/Book.hpp" 
#include "structures/Order.hpp"

//These tests only aim to test the methods and attributes of the Book class function correctly. They do not aim to catch underlying malfunctions in sub-classes such as the Order class. Those should be captured elsewhere.

//Testing class, to override Book internals
class TestBook: public Book{
public:
    bool isMarketMatchCalled = false;

    void marketMatch(Order& o) override{
        isMarketMatchCalled = true;
    }
};

//Testing class, to override Order internals
class TestOrder: public Order{
public:
    mutable bool isNotified = false;
    mutable bool isExecd = false;
    mutable int qtyPassed = -1;
    mutable double pPassed = -1;

    TestOrder(int id, Side s, OrderType type,
        int tgtQ, double tgtP = 0)
    : Order(id,s,type,tgtQ,tgtP){}

    void notify() const override{
        isNotified = true;
    }

    void exec(int qty, double p) override{
        isExecd = true;
        qtyPassed = qty;
        pPassed = p;
    }
};

// Helper class to provide access to the internals of Book, retaining encapsulation
struct BookTestHelper{
    //Getters
    static std::map<double,std::deque<Order>,std::less<>>&  limitSell(Book& b){return b.limitSell;}
    static std::map<double,std::deque<Order>,std::greater<>>& limitBuy(Book& b){return b.limitBuy;}

    //Access private methods
    static void marketMatch(Book& b, Order& o){return b.marketMatch(o);}
};


//Happy path test - Add Limit order
TEST_CASE("addOrder: Limit Order added","[Book]"){
    TestBook b;
    
    TestOrder o1(1,Side::BUY,OrderType::LIMIT,100,5.5);

    b.addOrder(o1);

    REQUIRE(b.isMarketMatchCalled);
    
}

//Happy path test - Add Market order
TEST_CASE("addOrder: Market Order Added","[Book]"){
    TestBook b;
    
    TestOrder o(1,Side::BUY,OrderType::MARKET,100,5.5);
    b.addOrder(o);

    REQUIRE(b.isMarketMatchCalled);
    REQUIRE(o.isNotified);

}




// These tests only validate high-level Book behavior assuming correct Order functionality.
// Order-specific behavior (e.g., exec logic) is covered in separate Order unit tests.
TEST_CASE("marketMatch: Standard full market match","[Book]"){
    Book b;


    //Put sell limit order in book
    Order sellOrder(2,Side::SELL,OrderType::LIMIT,100,5.0);
    b.addOrder(sellOrder);

    //Add a market order, it should fully exec
    Order buyOrder(2,Side::BUY,OrderType::MARKET,100);
    b.addOrder(buyOrder);

    REQUIRE(BookTestHelper::limitSell(b)[5.0].empty());
}

//Happy path test - Market order isn't fully executed (no liquidity)
TEST_CASE("marketMatch: Partial market match","[Book]"){
    Book b;

    //Put sell limit order in book
    Order sellOrder(2,Side::SELL,OrderType::LIMIT,100,5.0);
    b.addOrder(sellOrder);   

    //Add a market order, it should partially exec
    Order buyOrder(2,Side::BUY,OrderType::MARKET,75);
    b.addOrder(buyOrder);

    REQUIRE(BookTestHelper::limitSell(b)[5.0].front().getUnexecQty() == 25);
    REQUIRE(BookTestHelper::limitSell(b)[5.0].front().getPrice() == Catch::Approx(5.0));

    
}

//Happy path test - Test book is ordering by price
TEST_CASE("marketMatch: Price priority","[Book]"){
    Book b;

    //Put orders in book
    Order so1(3,Side::BUY,OrderType::LIMIT,50,5);
    Order so2(4,Side::BUY,OrderType::LIMIT,50,10);
    Order so3(5,Side::BUY,OrderType::LIMIT,50,7);
    b.addOrder(so1);
    b.addOrder(so2);
    b.addOrder(so3);

    //Add the market order
    Order bo1(5,Side::SELL,OrderType::MARKET,50);
    b.addOrder(bo1);

    //so2 should get executed fully
    REQUIRE(BookTestHelper::limitBuy(b)[10].empty());

}

//Happy path test - Test book is ordering by time inside price levels
TEST_CASE("marketMatch: Time priority","[Book]"){
    Book b;

    //Put orders in Book
    Order bo1(1,Side::BUY,OrderType::LIMIT,50,5);
    Order bo2(2,Side::BUY,OrderType::LIMIT,50,5);
    b.addOrder(bo1);
    b.addOrder(bo2);

    //Market order to match off
    Order so1(3,Side::SELL,OrderType::MARKET,50);
    b.addOrder(so1);

    REQUIRE(BookTestHelper::limitBuy(b)[5].front().getID() == 2);

}

