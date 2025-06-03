//Acts as an interface to the Ring class

#pragma once

#include "Ring.hpp"
#include <optional>

// SpscQ class
// Single Producer Single Consumer Queue
// Provides a simple interface to the underlying Ring buffer
template<typename O, size_t N>
class SpscQ {
public:

    // Instantiates a Ring buffer of size N for Order (O)
    Ring<O, N> ring;

    bool push(const O& o) {
        return ring.push(o);
    }

    std::optional<O> pop() {
        return ring.pop();
    }
};


