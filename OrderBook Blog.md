
# 21-04-2025

## Order Book Progress Summary

I confirmed that the basic functionality of the order book was working through debugging with `std::cout`. This functionality includes, but is not limited to:

- Adding and removing orders from the book
- Handling both limit and market orders
- Matching logic for both partial and full fills
- Priority for limit orders based on price, then time
- Displaying individual orders and full order book depth for debugging purposes
    

I then prepared my code for Catch2 unit tests, allowing me to write more robust test cases. This will support more aggressive refactoring, which I expect to be necessary when integrating lock-free programming into the backend or other error-prone components.

I switched to developing in a Linux environment via WSL for better compatibility with certain tools. I also set up my CMake files, as the project has grown and now requires a scalable build system. CMake also integrates well with Catch2.

I encountered difficulties with testing private members, as I couldn’t find a method I liked for giving unit tests access to private class attributes. These attributes are private because they should not be accessed by other modules once set, so I’m reluctant to make them public. However, writing numerous getters purely for testing would unnecessarily bloat the code. `friend` functions seem to be the best compromise—I’ll finalize the approach in the next session.

---

## Current File Structure



`/OrderBook 
├── CMakeLists.txt 
├── build/ │ 
├── src/ │   
	└── structures/ │       
		├── Book.cpp │       
		├── Book.h │       
		├── Order.cpp │      
		└── Order.h │ 
	├── CMakeLists.txt 
├── tests/ │   
	├── CMakeLists.txt │   
	└── structures/ │       
		└── testOrder.cpp │ `


# 22-04-2025

To kick off, I had to decide how I wanted to give my unit tests access to private attributes (as mentioned in yesterday’s log). I settled on using a **test helper struct** with `friend` access — it felt like the cleanest and most scalable option without cluttering the class with unnecessary getters.

**Order Test Helper Struct**
```cpp
struct OrderTestHelper {

    //Getters

    static int& orderID(Order& o) {return o.orderID;}

    static int& tgtQuantity(Order& o) {return o.tgtQuantity;}

    static double& tgtPrice(Order& o) {return o.tgtPrice;}

    static int& execQuantity(Order& o) {return o.execQuantity;}

    static int& unexecQuantity(Order& o) {return o.unexecQuantity;}

    static double& execPrice(Order& o) {return o.execPrice;}

    static long long timestamp(Order& o) {return o.timestamp;}

    static double tickSize(Order& o){return o.tickSize;}

  

    //Access private functions

    static void exec(Order& o, int& qty, double prc){o.exec(qty,prc);}

};
```

The rest of the day was spent designing unit tests for the `Order` class, covering things like partial execution, full execution, and object initialization.

Next up: continue writing unit tests for the remaining functionality in the project.

# 23-04-2025

Today I focused on finalizing the unit tests for my `Order` class. These tests were designed to cover logical edge cases where the program should throw errors—for example, preventing limit orders from executing out of the money, or disallowing negative prices.

While building these edge case tests, I also refined the related error handling to improve clarity and robustness. Below is an example of the updated error handling logic:

```cpp
// Error handling

if (prc < 0.0 || qty <= 0) {
    throw std::logic_error("Invalid Input: Price or Quantity is negative or zero");
}

if (orderType == OrderType::LIMIT && side == Side::BUY && prc > tgtPrice) {
    throw std::logic_error("Invalid Input: Limit buy order price exceeds target price");
}

if (orderType == OrderType::LIMIT && side == Side::SELL && prc < tgtPrice) {
    throw std::logic_error("Invalid Input: Limit sell order price is below target price");
}

if (qty > unexecQuantity) {
    throw std::logic_error("Invalid Input: Quantity to be executed exceeds outstanding quantity");
}

```
# 24-04-2025

I'm continuing to aim for a full suite of unit tests for my  classes. To begin this process, I first needed to create helper functions and classes. My goal was to design test utilities that **don’t dilute the production logic** — avoiding any injections or changes to the core code just to make testing possible.

To achieve this, I created `TestBook` and `TestOrder` classes. These allow me to borrow behavior from the real classes where needed while selectively overriding functions to track when they're accessed or to alter behavior specifically for testing. This kind of setup is future-friendly, as it should make it easy to quickly write additional unit tests as the project grows. I also considered using dependency injection, which would have been a valid and flexible alternative.

Here’s where I left the helper setup today. The core framework is now in place and ready to build on tomorrow. It took me a little time to re-familiarize myself with the different testing techniques available.

## Test Helpers

```cpp
class TestBook : public Book {
public:
    bool isMarketMatchCalled = false;

    void marketMatch(Order& o) override {
        isMarketMatchCalled = true;
    }
};

class TestOrder : public Order {
    mutable bool isNotified = false;

public:
    TestOrder(int id, Side s, OrderType type, int tgtQ, double tgtP)
        : Order(id, s, type, tgtQ, tgtP) {}

    void notify() const override {
        isNotified = true;
    }

    bool wasNotified() const { return isNotified; }
};

struct BookTestHelper {
    // Access internal order books
    static std::map<double, std::deque<Order>, std::less<double>>& limitSell(Book& b) {
        return b.limitSell;
    }

    static std::map<double, std::deque<Order>, std::greater<double>>& limitBuy(Book& b) {
        return b.limitBuy;
    }

    // Access private method for matching
    static void marketMatch(Book& b, Order& o) {
        b.marketMatch(o);
    }
};
```

Once I had written these helper functions, I was able to write a simple base case test to verify that adding a **limit order** to the order book immediately triggers matching — attempting to cross the book if the order arrives in the money.

In the next session, I'll aim to finalize the `Book` unit tests so I can begin refactoring with confidence, knowing that the test suite should catch any regressions or logic errors.

# 26-04-2025

Today, I continued focusing on my unit tests for the `Book` class as I work towards a major refactor.  
I have a lot of improvements in mind — for example, areas where I could use pointers more effectively to increase performance, or opportunities to improve the underlying data structures.

I wrote unit tests covering the happy path for adding orders into the book.  
To do this, I used a similar approach as before: monitoring that methods call the correct underlying functions based on the inputs provided.

For example, when given a limit order that is immediately "in the money," it should call the matching function to check if the order can cross the book.  
When a cross occurs, the notification function should be called to signal a completed order.

One thing I struggled with today was finding a way to monitor the internal state of the object without impacting the production logic.  In some cases, simply checking that the final state of the data structures is correct is sufficient.  However, sometimes it’s important to ensure that the correct intermediate steps have occurred. For example, executing an order might remove the correct `Order` from the map, but could still execute at the wrong quantity or price. For now, I've decided not to get bogged down.  I'll revisit this after the refactor, once I introduce modern C++ standard library pointers to allow cleaner monitoring. I was tempted to begin making changes to the code, but I resisted the urge. Making pointer changes without properly considering the side effects is likely to put a foot-gun I'll regret later in my code, especially when I move onto lock-free concurrency.

I discovered a problem today: I used `double` values as keys in my map.  This was likely a decision made while tired and without fully considering floating-point precision issues. I plan to fix this by refactoring the maps to use integers instead of doubles.  Since my book uses a tick size greater than 1p, I can represent prices as integers (e.g., `100` to represent £1.00). This change will ensure exact matching and eliminate floating-point comparison errors.

## Next steps

Before starting the major refactor, I plan to run a basic performance profile using 1 million orders as a benchmark.  This will give me a baseline to ensure any refactor results in real speed improvements. It’s important that I don’t increase the complexity of the code without measurable benefits.

Strong progress today on clean unit testing.  Identified and planned fixes for potential system stability issues (floating-point keys).  Clear next steps focused on performance and maintainability.

# 27-04-2025

I made significant progress today. Although it was a little tedious to spend my Sunday writing trivial happy-path tests for the matching logic of my `Book` class, it was important. The method that contains my matching logic is called `marketMatch`.

Today, I finalized all unit testing for the logic across the entire project, which is crucial for enabling **brave** refactoring without having to manually comb through the code to hunt for side effects. An example unit test is shown below:

```cpp
TEST_CASE("marketMatch: Price priority", "[Book]") {

    Book b;
    
    // Put orders in book
    Order so1(3, Side::BUY, OrderType::LIMIT, 50, 5);
    Order so2(4, Side::BUY, OrderType::LIMIT, 50, 10);
    Order so3(5, Side::BUY, OrderType::LIMIT, 50, 7);
    b.addOrder(so1);
    b.addOrder(so2);
    b.addOrder(so3);
  
    // Add the market order
    Order bo1(5, Side::SELL, OrderType::MARKET, 50);
    b.addOrder(bo1);

    // so2 should get executed fully
    REQUIRE(BookTestHelper::limitBuy(b)[10].empty());
}
```

I made a deliberate decision not to get bogged down in hunting for rare edge cases and writing exhaustive tests for them. I want to keep the project moving forward. The tests might become less relevant as the design evolves — or worse, slow down progress if they constantly need updating alongside code changes.

However, TDD remains a core principle of my design process, and as the project moves closer to its final state, I will rigorously expand and harden the test coverage.

## Next Steps

I plan to write some tests to benchmark performance and assess how successful the refactoring has been.


# 29-04-2025

Commit hash - `f4ade06f95ba4a52be3796822cf4568d40e76bd2`

Today I wrote a **quick-and-dirty benchmark** for the matching logic in my order book.  
I’m deliberately keeping the harness simple so I can keep the project moving; premature “engineering perfection” would only slow iteration. Once I have hard evidence of performance pain points, I’ll decide whether a larger refactor is justified—and by then I’ll have a much clearer picture of what the new design should look like.

---

## Timing loop

```cpp
auto startBenchmark = std::chrono::high_resolution_clock::now();

for (int i = 0; i < numMarketOrders; ++i) {
    auto startMatch = std::chrono::high_resolution_clock::now();

    Order o { i + numLimitOrders,
              Side::BUY,
              OrderType::MARKET,
              mQtys[i] };                       // random quantity
    book.addOrder(o);                           // matching happens here

    auto endMatch = std::chrono::high_resolution_clock::now();
    matchTimes.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            endMatch - startMatch).count());
}

auto endBenchmark = std::chrono::high_resolution_clock::now();
```

## Metric calculation

```cpp
// Mean latency
long long totalNs = std::accumulate(matchTimes.begin(), matchTimes.end(), 0LL);
double meanNs     = static_cast<double>(totalNs) / matchTimes.size();

// P99 latency
std::sort(matchTimes.begin(), matchTimes.end());
double p99Ns = matchTimes[static_cast<size_t>(0.99 * matchTimes.size())];

// Throughput (matches / second)
double benchDurationNs =
    std::chrono::duration_cast<std::chrono::nanoseconds>(
        endBenchmark - startBenchmark).count();
double throughput = matchTimes.size() * 1'000'000'000.0 / benchDurationNs;
```

---

## Benchmark set-up

- **Limit-order book:** 100,000 random limit orders (price and quantity uniform in \[1 … 1,000,000\]); book never exhausts liquidity, so market orders are never rejected trivially.  
- **Market-order flow:** 50,000 random BUY market orders, quantities also uniform in \[1 … 1,000,000\].  

| Metric                      |                Result |
| --------------------------- | --------------------: |
| Mean latency (market order) |            **620 ns** |
| 99-percentile latency       |            **2.1 µs** |
| Sustained throughput        | **1.5 M matches / s** |

These numbers form the baseline I’ll use to judge future optimizations.

---

## Next steps

1. **Light refactor & obvious fixes**
   - eliminate unnecessary copies (pass by reference / `std::move`)
   - tighten data-structure access paths
2. **Profile cache behaviour**
   - use `perf` / `valgrind` / cachegrind to locate hot paths
   - measure L1/L2 miss rates before attempting cache-layout tweaks
3. **Stress tests**
   - deeper book (1M+ orders)
   - mixed cancels / amendments
   - eventual multi-threaded intake

---

# 30-04-2025

Today I was cleaning up my code: adding comments, ensuring I was using pointers where possible for speed improvements, and removing or simplifying redundant logic. Essentially, I was reviewing my code with fresh eyes — not diving deep for micro-optimizations, but scanning for obvious improvements. 

To be frank, it was quite straightforward work — or so I thought.

I had already run my battery of unit tests to confirm the book was likely functioning correctly. Then, just before clocking out for the night (it's currently 12:10 AM and I have work tomorrow), I decided to run my benchmark. To my surprise, it was definitely about **25% slower**. I ran it multiple times to confirm. 

I committed the changes to a feature branch and clearly marked it with the performance regression.

## Next Steps

- Identify which part of the refactor introduced the slowdown.
- I'm currently using `std::map` as the core data structure. While maps are great for sorted lookups, they suffer from poor cache locality because their nodes are scattered in memory (unlike vectors which are contiguous).
- I originally chose maps for flexibility, but I'm going to explore better-suited data structures. The most obvious alternative that comes to mind is a **priority queue**, since heaps are well-suited for matching engines.

## End of Day Benchmark

| Metric                      |                Result |
| --------------------------- | --------------------: |
| Mean latency (market order) |            **764 ns** |
| 99-percentile latency       |            **2.5 µs** |
| Sustained throughput        | **1.2 M matches / s** |

# 01-05-2025

Today I started off needing to find the cause of a ~25% performance slowdown.

I had a method in the `Order` class that previously printed to the console. While it currently just logs messages, the plan is to eventually reuse it to notify the user. When writing my performance benchmarks, I commented this function out entirely to avoid flooding the console with 100,000+ print statements, which would severely impact performance due to slow I/O.

During a later refactor, I removed the print statements permanently but added a new state variable to the `Order` class. This variable tracks the order's status when it cannot execute due to insufficient liquidity. I also intended it to handle other terminal states, such as expiry — for example, some limit orders in real markets are "good till close."

I confirmed this was the issue by comparing the current commit with the previous one in the repository. I examined the diff and isolated the relevant section of code. After replacing the new implementation with the original version, I re-ran my unit tests to ensure correctness, then executed the benchmark. The performance returned to baseline, confirming that this change — specifically the state mutation within the `notify()` function — was responsible for the slowdown.


The performance is now:

| Metric                      |                Result |     |
| --------------------------- | --------------------: | --- |
| Mean latency (market order) |            **629 ns** |     |
| 99-percentile latency       |            **2.1 µs** |     |
| Sustained throughput        | **1.5 M matches / s** |     |

This matches my original performance test, which is what I expected since I hadn't made any tangible changes to the core logic.

## Learnings

This experience taught me that even very small state changes lying on the critical path of core logic can have huge performance implications. Later in the project, I’ll be more mindful about introducing or modifying state — and will actively check for any redundant variables that can be removed.

# 10-05-2025

I've updated to a new computer (Linux) and therefore my benchmark statistics have changed. Here is the current benchmark. It was expected to be significantly quicker, due to these factors:
- Faster CPU (AMD Ryzen 7 7700)
- 2x 16GB DDR5 RAM running at 4800 MT/s
- Linux OS (Pop_OS!)
- 1TB SSD

| Metric                      |                Result |
| --------------------------- | --------------------: |
| Mean latency (market order) |             **573ns** |
| 99-percentile latency       |            **1.7 µs** |
| Sustained throughput        | **1.5 M matches / s** |

# 13-05-2025

## SPSC Queue Lab Test

Over the past few days, I've been focusing on developing my concurrency skills so I can integrate concurrency into this project. The Order Book is currently single-threaded, which was fine for a proof-of-concept focused mostly on matching logic. However, once I add I/O channels from an externally exposed API, I’ll need a way to control the flow of orders into the book in a structured and scalable manner.

At the moment, the project doesn’t have any buffering for order intake. If more than ~1.5 million orders were to arrive, excess orders would simply be dropped—though the system would be able to notify users of this. This zero-buffer model is fast, since each order flows directly into the matching logic with no queuing, but it's not scalable. That’s why I’ve implemented a lock-free **Single Producer Single Consumer (SPSC)** queue in a separate test repository. The code there is intentionally written quickly and informally—it's a sandbox for me to explore ideas, not production-grade.

### `Ring` Class

`Ring Class`
```cpp
template<typename O, std::size_t N>
class Ring{
public:
    std::array<O,N> buffer;
    std::atomic<size_t> head = 0;
    std::atomic<size_t> tail = 0;
 

    bool push(const O& o){
        const size_t nTail = (tail+1)%N;
        if (nTail == head){return false;} // Buffer is full
        buffer[tail] = o; 
        tail = nTail; 
        return true;
    }

    std::optional<O> pop(){

        if (head == tail) {return std::nullopt;} //Empty buffer
        O o = buffer[head];
        head = (head+1)%N;

        return o;
    }
};
```

This is a fixed-size ring buffer that holds `Order`s before they're passed to the matching logic. It’s fast because it uses `std::array`, which has fixed size at compile time and excellent cache locality due to contiguous memory layout. Importantly, I never delete from the array; instead, I update pointers (`head` and `tail`) to indicate where valid data begins and ends. This ensures that only one thread modifies any part of the data structure.

The lock-free nature is safe because one thread is the producer and one is the consumer. The use of atomic variables ensures `head` and `tail` updates are indivisible, and since they never point to the same cell unless the buffer is full, there's no data race. I’ve confirmed the correctness of this under significant load (>10 million orders) using **ThreadSanitizer (TSAN)** and **AddressSanitizer (ASAN)**.

### `SpscQ` Class
```cpp

#include "Ring.hpp"

template<typename O>
class SpscQ{
public:
    Ring<O,10'000> r;

    bool push(const O& o){
        return r.push(o);
    } 

    std::optional<O> pop(){
       return r.pop(); 
    }
};
```
The `SpscQ` wrapper separates interface logic from the core ring buffer implementation. In the future, this abstraction will make it easier to plug in networking logic. For now, though, it could be removed without affecting behaviour.

### Benchmarking and Flamegraph analysis

The lab also has some unit tests and performance bench-marking logic. The results of which is shown below, along with some findings. 

My first-benchmark I ran had the following result, which at the time was very disappointing, since if implemented it would increase latency (p99 and mean) by many multiples, when compared to the no-buffered Order intake approach.

**8:53am - 10/05/2025** **- commit** (`7363d18f`)

| Metric                      |                Result |
| --------------------------- | --------------------: |
| Mean latency (market order) |                9108ns |
| 99-percentile latency       |            **202 µs** |
| Sustained throughput        | **1.6 M matches / s** |

After investigation, I realized I hadn’t compiled with the appropriate optimization flags. Once I enabled `-O2 -g -fno-omit-frame-pointer -fno-inline`, the performance improved significantly, as shown below along with the corresponding Flamegraph.

| Metric                      |              Result |
| --------------------------- | ------------------: |
| Mean latency (market order) |               280ns |
| 99-percentile latency       |           **4.0µs** |
| Sustained throughput        | **14M matches / s** |
![[flamegraph 3.svg]]


On the far-left the, you can see the programme took many samples while inside sorting logic, which I immediately knew was not correct, or Id made a mistake. I eventually found that, this was including the performance benchmarking itself (the table above). To find p99 latency, you need to sort the latency outcomes, which is what is showin-up in the Flamegraph. Once I removed that, this gave me the following

![[flamegraph 2.svg]]

Looking further, I found a significant portion of the time was spent in `genRandOrder`, a function used to generate dummy orders for testing. This wouldn’t exist in a real-world implementation and only affects the `push` side of the queue. Removing or optimizing it would yield further gains, but for this lab test, I'm satisfied with the results.

**Next Steps**: integrating the SPSC queue into the main order book logic.


# 22-05-2025

Finalised unit tests to check for safety of concurrency. This included running a quantity sum on sufficent load. I also confirmed that my logic is safe via, ASAN and TSAN. This is in addition to the safe logic being directly ported over from the lab environment, then adapted to the live Order Book.

## Next steps

Run a flamefraph, before the addition of the Ring buffer then one after.

# 23-05-2025

On the benchmarking setup from commit `c237bfc`, pre-intergration of the ring-buffer. Therefore in this test, I'm profiing raw matching logic. I ran under flags '-o3 -g', so it was optimised under o3. I ran with `1e7` market orders and `2e7` limit orders. This resulted in these performance statistics. This run was also perf'd with a sampling frequency of 1000hz.

To generate perf correctly, I had to enable DWARF mode, since perf was profiling itself due to a known issue with my distro (Pop OS).

The o3 flags, added a significant performance boost. As they did with the lab. 

| Metric                      |               Result |
| --------------------------- | -------------------: |
| Mean latency (market order) |             **97ns** |
| 99-percentile latency       |            **560ns** |
| Sustained throughput        | **8.5M matches / s** |


The flamegraph produced:
![[flamegraph 3.svg]]

# 25-05-2025

## **Saturating the Pipeline: Pushing 100M Orders Through SPSC Queue**

Today’s goal was to **stress-test the full producer-consumer pipeline** in my order book engine using an `SpscQ` (Single Producer, Single Consumer) lock-free ring buffer. The benchmark was to process **100 million orders** as realistically as possible under heavy load — with real matching logic and queue pressure.

---

## Key Changes

- Rewrote the **consumer thread loop** to consume exactly `numOrders` rather than exiting early after 10 empty pops. This fixed a critical bug where the consumer exited prematurely, giving artificially fast results (~409ms for 100M, not reasonable).
- Implemented a **push-retry loop** in the producer adding to the buffer faster, than the consumer can take orders

```cpp
    while (!sq.push(orders[i])) {}
    ```

    This prevents order drops when the ring buffer fills up, and yields control to let the consumer drain the queue. I could add something, to prevent the tight spin loop, trashing the CPU
- Verified behavior through **debug printouts of matched orders** in `Book::addOrder`, confirming correct matching, order flow, and dynamic book state evolution.
- Benchmarked **100 million orders in ~6.4 seconds** — that's ~15.6 million orders/sec with matching logic running in parallel. The system saturated **two CPU cores at 100%**, exactly as expected for an SPSC model. This performance is roughly in-line with the performance of the SpscQ and matching logic tested sepperately
- Ran everything with **basic correctness** checks. The logic was sanitised with ASAN and TSAN inside my unit tests, also verified to work in the ORder Book Lab repo, under load.
    

---

##  Reflections

This was a big step forward — I moved from a naive benchmark that looked fast (but was broken), to a **realistic concurrent pipeline** where every order is matched or inserted. Watching `htop` light up two cores and knowing the consumer was the bottleneck (as it should be) was genuinely satisfying.

I've now got:
- A functioning **ring-buffer queue** under load, which was verified with TSAN and ASAN in testing
- Real matching behavior visible in logs, now removed
- A sense of where the next bottlenecks will come from (probably the `Book` structure itself)
    

---

### Next Steps

- Build a **latency profiler** (p50, p99, throughput)
    

---

This session felt like a real breakthrough — first time I’ve seen the system under true load with meaningful output. Committed this version with clear notes for future refinement.

# 28-05-2025

### **Latency & Throughput Benchmarking with `SpscQ`**

Today I added **average and P99 latency tracking** to the Order Book benchmark, now integrated with a custom `SpscQ` (Single Producer, Single Consumer) lock-free queue. The benchmark processed **10 million market orders** with a **buffer size of 65,536**.

To ensure correctness under pressure, the producer waits if the buffer is full, preventing order drops and eliminating artificially low latencies caused by skipped processing. All timing is recorded externally using vectors indexed by `orderId`, allowing full separation from the core `Order` structure, to prevent data races in the performance measuring.

The benchmark was compiled with `-O3` optimizations enabled.

---

### Benchmark Results

| Metric                      | Result                       |
| --------------------------- | ---------------------------- |
| Mean latency (market order) | **5.01 ms**                  |
| 99th percentile latency     | **5.55 ms**                  |
| Sustained throughput        | **13.1 million matches/sec** |

---

This marks a solid step forward — the system now delivers accurate latency metrics, high throughput, and stable behavior under concurrent load. The engine is beginning to perform like a real-time matching system under pressure.

### Next steps

- Updated flamegraph which includes the ring buffer


# 31-05-2025 

To reduce end-to-end latency, I experimented with decreasing the ring buffer size. Since the latency timer starts when an order is enqueued, a smaller buffer allows orders to reach the front of the queue more quickly, reducing in-queue delay during bursts.

**Buffer size reduced**: 65,536 → 16,384 entries

|Metric|Result|
|---|---|
|Mean latency (market order)|1.2 ms|
|99th percentile latency|1.5 ms|
|Sustained throughput|14.0 million matches/sec|

This confirms that smaller buffers can reduce latency at the cost of some burst capacity. This tradeoff is acceptable in low-latency environments where responsiveness is more important than sustained queue depth.

# 02-06-2025


Tested build and execution on a system without admin permissions.
- Core engine and benchmarks compiled successfully without modification.
- Unit tests failed to build due to Catch2 not being fetchable (requires Git access or pre-installed dependency).
- Benchmarks using 100 million orders failed to run, likely due to system memory constraints or CPU resource limits.
- Reducing the benchmark load to 100,000 orders allowed successful execution.
-
## File Structure

```objectivec
/OrderBook
├── CMakeLists.txt
├── benchmarks/
│   ├── CMakeLists.txt
│   └── OrderBookBenchmark.cpp
├── src/
│   ├── CMakeLists.txt
│   └── structures/
│       ├── Book.cpp / Book.hpp
│       ├── Order.cpp
│       ├── Ring.hpp
│       └── SpscQ.hpp
├── tests/
│   ├── CMakeLists.txt
│   └── structures/
│       ├── testBook.cpp
│       ├── testOrder.cpp
│       ├── testRing.cpp
│       ├── testThreads.cpp
│       └── testHelpers.h
```



## Observations
- Machines with smaller CPU caches may struggle with large static buffers (e.g., 64k+ entries).
- Allocation of vectors with 100 million order objects can bottleneck or fail on machines with limited memory or slow disk I/O.
- Unit tests depend on a working Catch2 setup; fallback or offline packaging may be required for constrained environments.

## Default Benchmark Configuration

Final configuration reduces the default benchmark workload to one million orders. This is a practical default that executes reliably across a wide range of hardware while still providing meaningful performance metrics.