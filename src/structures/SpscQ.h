//Acts as an interface to the Ring class

#ifndef SPSCQ_H
#define SPSCQ_H

#include "Ring.h"
#include <optional>

template<typename O, size_t N>
class SpscQ {
public:
    Ring<O, N> ring[N];

    bool push(const O& o) {
        return ring.push(o);
    }

    std::optional<O> pop() {
        return ring.pop();
    }
};

#endif //SPSCQ_H
