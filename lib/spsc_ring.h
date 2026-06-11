#ifndef INCLUDED_CYBERETHER_SPSC_RING_H
#define INCLUDED_CYBERETHER_SPSC_RING_H

// Class SpscRing (Single Producer Single Consumer Ring Buffer)
// one producer(GR scheduler), one consumer(Superluminal), no locking.
// Note: Thread safety is strictly dependent on the 1:1 mapping.

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <vector>

template <typename T>
class SpscRing {
public:
    explicit SpscRing(size_t capacity):
        d_buffer(capacity),
        d_capacity(capacity),
        d_head(0),
        d_tail(0)
    {}

    // Producer side, called only from the GR scheduler thread.
    // Writes up to `n` samples, returns the number actually written.
    size_t push(const T* src, size_t n) {
        const size_t head = d_head.load(std::memory_order_relaxed);
        const size_t tail = d_tail.load(std::memory_order_acquire);
        const size_t free_space = d_capacity - (head - tail);
        const size_t to_write = std::min(n, free_space);

        for (size_t i = 0; i < to_write; ++i) {
            d_buffer[(head + i) % d_capacity] = src[i];
        }

        d_head.store(head + to_write, std::memory_order_release);
        return to_write;
    }

    // Consumer side, called only from the Superluminal thread.
    // Reads up to `n` samples, returns the number actually read.
    size_t pop(T* dst, size_t n) {
        const size_t tail = d_tail.load(std::memory_order_relaxed);
        const size_t head = d_head.load(std::memory_order_acquire);
        const size_t available = head - tail;
        const size_t to_read = std::min(n, available);

        for (size_t i = 0; i < to_read; ++i) {
            dst[i] = d_buffer[(tail + i) % d_capacity];
        }

        d_tail.store(tail + to_read, std::memory_order_release);
        return to_read;
    }


private:
    std::vector<T> d_buffer;
    const size_t d_capacity;
    std::atomic_size_t d_head; // producer-owned: write index
    std::atomic_size_t d_tail; // consumer-owned: read index

};

#endif /* INCLUDED_CYBERETHER_SPSC_RING_H */
