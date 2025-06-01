#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>

#include "structures/Book.hpp"
#include "structures/Order.hpp"
#include "structures/SpscQ.hpp"

int getRandInt(std::mt19937& gen, const int topBound){
    // Gets a random integer from up to topBound

    //Range of prices
    std::uniform_int_distribution<int> dist(1,topBound);
    return dist(gen);
}


void addLimits(Book& b, const int numOrders){
    // Adds numOrders many Limit Orders to Book (b).
    // These limit orders are randomly generated.
    //Chances:
    //  50% Sell, 50% Buy
    //  1 to 1M on Price (Uniform distribution)
    //  1 to 1M on Quantity (Uniform Distribution)

    //Generate random key
    std::random_device rd;
    std::mt19937 gen(rd());

    // Creates function to 'flip a coin'
    std::bernoulli_distribution coinFlip(0.5);

    for (int i = 0;i<numOrders;i++){

        const int randInt = getRandInt(gen,1'000'000);
        const int randQty = getRandInt(gen,1'000'000);

        //Decide  if order is sell or buy
        Side side;
        if (coinFlip(gen)){side = Side::SELL;}
        else {side  = Side::BUY;}

        Order o = {i,side,OrderType::LIMIT,randQty,static_cast<double>(randInt)};
        b.addOrder(o);
    }
}

std::vector<Order> makeOrders(const int numOrders){
    // Adds numOrders many Orders to a vector, which returned
    // These orders are randomly generated.
    //Chances:
    //  50% Sell, 50% Buy
    //  50% Limit, 50% Market
    //  1 to 1M on Price (Uniform distribution)
    //  1 to 1M on Quantity (Uniform Distribution)

    //Generate random key
    std::random_device rd;
    std::mt19937 gen(rd());

    std::bernoulli_distribution coinFlip(0.5);

    std::vector<Order> randOrders(numOrders);

    for (int i = 0;i<numOrders;i++) {
        Side side;
        OrderType ot;
        int randQty = getRandInt(gen,1'000'000);

        //Pick order type
        if (coinFlip(gen)){ot = OrderType::LIMIT;}
        else {ot = OrderType::MARKET;}

        //Pick side of Order
        if (coinFlip(gen)){side = Side::SELL;}
        else {side  = Side::BUY;}

        //If it's Limit it needs a price, if it's market it's just quantity
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
    constexpr int numOrders = 100'000'000; // Total number of orders to execute
    constexpr int startingLimits = 1'000'000; //Initial number of Limit orders in the book

    //Put some initial limit orders in the book, some of these will instantly cross the book
    Book b;
    addLimits(b,startingLimits);

    //Make a vector of orders to be added
    const std::vector<Order> orders = makeOrders(numOrders);

    //Setup SpscQ buffer
    constexpr int buffSize = 16'384;
    SpscQ<Order,buffSize> sq;

    //Init the Vector to store Benchmark times, by id.
    // 0 Represents the Order was not executed
    // Vector rather than array due to being too big to go in cache
    using Clock = std::chrono::time_point<std::chrono::high_resolution_clock>;
    std::vector<Clock> startTimes(numOrders); //Only producer accesses
    std::vector<Clock> endTimes(numOrders); // Only consumer accesses
    std::vector<long long> matchTimes(numOrders);


    const Clock startBench = std::chrono::high_resolution_clock::now();
    //Producer thread
    std::jthread producer([&] {
        for (int i = 0;i<numOrders;i++) {
            while (!sq.push(orders[i])){}
            const Clock insertTime = std::chrono::high_resolution_clock::now();
            startTimes[i] = insertTime;
        }
    });

    //Consumer thread
    std::jthread consumer([&] {
        int count = 0;
        constexpr int maxSpinAttempts = 1'000;
        while (count < maxSpinAttempts) {
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
    const auto endBench = std::chrono::high_resolution_clock::now();

    // Calculate latencies
    for (int i = 0;i<numOrders;i++) {
        if (endTimes[i] == Clock{}) {
            continue;
        }
        matchTimes[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(endTimes[i] - startTimes[i]).count();
    }

    // Calculate Performance statistics

    // Calculate Average
    long long total = 0;
    size_t matchedCount = 0;
    for (const auto& t : matchTimes) {
        if (t != 0) {
            total += t;
            matchedCount++;
        }
    }
    const double averageNs = static_cast<double>(total) /
                             (static_cast<double>(matchedCount) * 1'000);


    // Calculate P99
    std::sort(matchTimes.begin(), matchTimes.end());
    const double p99Ns = static_cast<double>(matchTimes[static_cast<size_t>(0.99 * static_cast<double>(matchTimes.size()))]) /
                    1'000;

    // Throughput - matches per second
    const auto totalBenchmarkNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endBench - startBench).count();
    const double throughput = static_cast<double>(matchTimes.size()) * 1'000'000'000.0 / static_cast<double>(totalBenchmarkNs);

    // Print without scientific notation
    std::cout << std::fixed << std::setprecision(2); // control number formatting
    std::cout << "Average Match Latency: " << averageNs << " μs\n";
    std::cout << "P99 Match Latency: " << p99Ns << " μs\n";
    std::cout << "Throughput: " << throughput << " matches/sec\n";

}