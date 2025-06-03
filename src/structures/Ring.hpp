// The Ring class, which is the buffer that holds the Order objects before they go the matching logic

#pragma once

#include <array>
#include <optional>
#include <cstddef>
#include <atomic>

template<typename, std::size_t>
class SpscQ;

template<typename, std::size_t>
class TestRingHelper;

template<typename O, std::size_t N>
class Ring {
//Buffer for my Single Producer Single consumer ring interface (SpscQ)
    friend class SpscQ<O,N>; //Interface for SpscQ
    friend class TestRingHelper<O,N>; // Helper class for unit tests

    std::array<O, N> buffer;

    //Indivisible under concurrency
    std::atomic<size_t> head = 0;
    std::atomic<size_t> tail = 0;

    bool push(const O& o) {
        //Pushes an Order to the buffer at tail.
        //Moves tail forward one

        // Check if the buffer is full
        const size_t next = (tail + 1) % N;
        if (next == head) {
            return false;
        }
        buffer[tail] = o;
        tail = next;
        return true;
    }

    std::optional<O> pop() {
        //Pops order off at head by moving head forward, not actually deleting any data

        //Checks if buffer is empty
        if (head == tail){return std::nullopt;}

        O o = buffer[head];
        head = (head + 1) % N;

        return o;
    }
};

