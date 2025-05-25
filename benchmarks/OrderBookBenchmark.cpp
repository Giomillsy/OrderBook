#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "structures/Book.h" 
#include "structures/Order.h"

int getRandInt(std::mt19937& gen,int topBound){
    
    //Range of prices
    std::uniform_int_distribution<int> dist(1,topBound);
    return dist(gen);
}


void addLimits(Book& b, int numOrders){

    //Generate random key
    std::random_device rd;
    std::mt19937 gen(rd());


    for (int i = 0;i<numOrders;i++){

        int randInt = getRandInt(gen,1'000'000);
        int randQty = getRandInt(gen,1'000'000);
        Order o = {i,Side::SELL,OrderType::LIMIT,randQty,static_cast<double>(randInt)};
        b.addOrder(o);
    }

}

int main(){
    std::vector<long long> matchTimes;

    const int numMarketOrders = 10'000'000;
    const int numLimitOrders = 20'000'000; //Prevent liquidity running out, that would be near instant matches


    matchTimes.reserve(numMarketOrders);

    //Fill the book with limit orders 100k random orders on both sides
    Book b;
    addLimits(b,numLimitOrders);

    //Generate random key
    std::random_device rd;
    std::mt19937 gen(rd());


    //Get randquantities for market ordrs
    std::vector<int> mQtys;
    mQtys.reserve(numMarketOrders);
    for (int i = 0;i<numMarketOrders;i++){
        mQtys.push_back(getRandInt(gen,1'000'000));
    }


    auto startBenchmark =   std::
        chrono::high_resolution_clock::now();
    for (int i = 0;i<numMarketOrders;i++){
        auto startMatch = std::chrono::high_resolution_clock::now();
        Order o = {i+numLimitOrders,Side::BUY,OrderType::MARKET,mQtys[i]};
        b.addOrder(o);
        auto endMatch = std::chrono::high_resolution_clock::now();

        matchTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(endMatch - startMatch).count());        
    }

    auto endBenchmark =   std::
        chrono::high_resolution_clock::now();

    //Calculate Average
    long long total = std::accumulate(matchTimes.begin(), matchTimes.end(), 0LL);
    double averageNs = static_cast<double>(total) / matchTimes.size();
    
    // Calculate P99
    std::sort(matchTimes.begin(), matchTimes.end());
    double p99Ns = matchTimes[static_cast<size_t>(0.99 * matchTimes.size())];
    
    // Throughput
    double totalBenchmarkNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endBenchmark - startBenchmark).count();
    double throughput = (matchTimes.size() * 1'000'000'000.0) / totalBenchmarkNs; // matches per second
    
    std::cout << "Average Match Latency: " << averageNs << " ns\n";
    std::cout << "P99 Match Latency: " << p99Ns << " ns\n";
    std::cout << "Throughput: " << throughput << " matches/sec\n";


}