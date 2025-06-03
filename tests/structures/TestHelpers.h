// Helper structs to access private members of Order and Ring for unit testing.
// These should only be used in test environments to avoid violating encapsulation in production code.


#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <optional>

// Provides access to private fields and functions of the Order class
// Used to check for internal state changes during unit testing
class OrderTestHelper {
public:
    //Getters
    static int& orderID(Order& o) {return o.orderID;}
    static int& tgtQuantity(Order& o) {return o.tgtQuantity;}
    static double& tgtPrice(Order& o) {return o.tgtPrice;}
    static int& execQuantity(Order& o) {return o.execQuantity;}
    static int& unexecQuantity(Order& o) {return o.unexecQuantity;}
    static double& execPrice(Order& o) {return o.execPrice;}
    static long long timestamp(const Order& o) {return o.timestamp;}
    static double tickSize(){return Order::tickSize;}

    //Access private functions
    static void exec(Order& o, const int& qty, const double prc){o.exec(qty,prc);}

};

template<typename O, std::size_t N>
class Ring;

template<typename O, std::size_t N>
class TestRingHelper {
public:
    //Getters
    static const std::array<O, N>& getBuffer(const Ring<O, N>& r) { return r.buffer; }
    static std::size_t getHead(const Ring<O, N>& r) { return r.head; }
    static std::size_t getTail(const Ring<O, N>& r) { return r.tail; }

    //Setters
    static void setHead(Ring<O, N>& r, std::size_t h) { r.head = h; }
    static void setTail(Ring<O, N>& r, std::size_t t) { r.tail = t; }
    static void setBuffer(Ring<O, N>& r, Order& o, size_t pos) { r.buffer[pos] = o; }

    static bool doPush(Ring<O, N>& r,Order& o) {return r.push(o); }
    static std::optional<O> doPop(Ring<O, N>& r) { return r.pop(); }
};

#endif //TESTHELPERS_H
