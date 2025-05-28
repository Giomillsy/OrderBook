#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <random>
#include <thread>

#include "structures/Book.h"
#include "structures/Order.h"
#include "structures/SpscQ.h"

int getRandInt(std::mt19937& gen,int topBound){

    //Range of prices
    std::uniform_int_distribution<int> dist(1,topBound);
    return dist(gen);
}


void addLimits(Book& b, int numOrders){

    //Generate random key
    std::random_device rd;
    std::mt19937 gen(rd());

    std::bernoulli_distribution coinFlip(0.5);


    for (int i = 0;i<numOrders;i++){

        int randInt = getRandInt(gen,1'000'000);
        int randQty = getRandInt(gen,1'000'000);

        //Decide  if order is sell or buy
        Side side;
        if (coinFlip(gen)){side = Side::SELL;}
        else {side  = Side::BUY;}

        Order o = {i,side,OrderType::LIMIT,randQty,static_cast<double>(randInt)};
        b.addOrder(o);
    }


}

std::vector<Order> makeOrders(const int numOrders){
    //Generate random key
    std::random_device rd;
    std::mt19937 gen(rd());

    std::bernoulli_distribution coinFlip(0.5);

    std::vector<Order> randOrders(numOrders);

    for (int i = 0;i<numOrders;i++) {
        Side side;
        OrderType ot;
        int randQty = static_cast<double>(getRandInt(gen,1'000'000));

        //Pick order type
        if (coinFlip(gen)){ot = OrderType::LIMIT;}
        else {ot = OrderType::MARKET;}

        if (coinFlip(gen)){side = Side::SELL;}
        else {side  = Side::BUY;}

        //If it's Limit it needs a price
        Order o;
        if (ot == OrderType::LIMIT) {
            auto prc = static_cast<double>(getRandInt(gen,1'000'000));
            o = {i,side,ot,randQty,prc};
        }else {
            o = {i,side,ot,randQty};
        }

        randOrders[i] = o;

    }

    return randOrders;

}
int main(){

    const int numOrders = 100'000'000;
    const int startingLimits = 1'000'000; //Intial number of orders in the book

    //Put some intial limit orders in the book, some of these will instantly cross the book
    Book b;
    addLimits(b,startingLimits);

    //Make a vector of orders to be added
    std::vector<Order> orders = makeOrders(numOrders);

    //Setup SpscQ buffer
    constexpr int buffSize = 65536;
    SpscQ<Order,buffSize> sq;

    //Init the Vector to store Benchmark times, by id.
    // 0 Represents the Order was not executed    // Vector rather than array due to being too big to go in cache
    using Clock = std::chrono::time_point<std::chrono::high_resolution_clock>;
    std::vector<Clock> startTimes(numOrders); //Only prodcuer accesses
    std::vector<Clock> endTimes(numOrders); // Only consumer accesses
    std::vector<long long> matchTimes(numOrders);


    Clock startBench = std::chrono::high_resolution_clock::now();
    //Producer thread
    std::jthread producer([&] {
        for (int i = 0;i<numOrders;i++) {
            while (!sq.push(orders[i])){}
            Clock insertTime = std::chrono::high_resolution_clock::now();
            startTimes[i] = insertTime;
        }
    });

    //Consumer thread
    std::jthread consumer([&] {
        int count = 0;
        while (count < 1'000) {
            auto popped = sq.pop();
            if (popped.has_value()) {
                count = 0;
                Order o = popped.value();
                b.addOrder(o);
                endTimes[o.getID()] = std::chrono::high_resolution_clock::now();
            }else{count++;}
        }
    });

    producer.join();
    consumer.join();
    auto endBench = std::chrono::high_resolution_clock::now();

    // Calculate latencies
    for (int i = 0;i<numOrders;i++) {
        if (endTimes[i] == Clock{}) {
            continue;
        }
        matchTimes[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(endTimes[i] - startTimes[i]).count();
    }

    // Calculate Performance statistics

    // Calculate Average
    long long total = std::accumulate(matchTimes.begin(), matchTimes.end(), 0LL);
    double averageNs = static_cast<double>(total) / (matchTimes.size()* 1'000'000);

    // Calculate P99
    std::sort(matchTimes.begin(), matchTimes.end());
    double p99Ns = matchTimes[static_cast<size_t>(0.99 * matchTimes.size())]/1'000'000;

    // Throughput
    double totalBenchmarkNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endBench - startBench).count();
    double throughput = (matchTimes.size() * 1'000'000'000.0) / totalBenchmarkNs; // matches per second

    // Print without scientific notation    std::cout << std::fixed << std::setprecision(2); // control number formatting
    std::cout << "Average Match Latency: " << averageNs << " ms\n";
    std::cout << "P99 Match Latency: " << p99Ns << " ms\n";
    std::cout << "Throughput: " << throughput << " matches/sec\n";


}