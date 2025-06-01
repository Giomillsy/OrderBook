// The Ring class, which is the buffer that holds the Order objects before they go the matching logic

#ifndef RING_H
#define RING_H

#include <array>
#include <optional>
#include <cstddef>
#include <iostream>
#include <atomic>

template<typename, std::size_t>
class SpscQ;

template<typename, std::size_t>
class TestRingHelper;

template<typename O, std::size_t N>
class Ring {
    friend class SpscQ<O,N>;
    friend class TestRingHelper<O,N>;

    std::array<O, N> buffer;
    std::atomic<size_t> head = 0;
    std::atomic<size_t> tail = 0;

    bool push(const O& o) {
        //Pushes an Order to the buffer
        const size_t next = (tail + 1) % N;
        if (next == head) {
            return false;
        }
        buffer[tail] = o;
        tail = next;
        return true;
    }

    std::optional<O> pop() {
        if (head == tail){return std::nullopt;}
        O o = buffer[head];
        head = (head + 1) % N;

        return o;
    }
};

#endif //RING_H
