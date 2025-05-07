#pragma once
#include <atomic>
#include <optional>
#include <array>

template<typename T, size_t Capacity = 1024>
class LockFreeQueue {
public:
    LockFreeQueue() : head_(0), tail_(0) {}

    bool push(const T& value) {
        size_t current_tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Capacity;
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // очередь полна
        }
        buffer_[current_tail] = value;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool pop(T& result) {
        size_t current_head = head_.load(std::memory_order_relaxed);
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return false; // очередь пуста
        }
        result = buffer_[current_head];
        head_.store((current_head + 1) % Capacity, std::memory_order_release);
        return true;
    }

    size_t size() const {
        size_t t = tail_.load(std::memory_order_acquire);
        size_t h = head_.load(std::memory_order_acquire);
        return (t >= h) ? (t - h) : (Capacity - h + t);
    }

private:
    std::array<T, Capacity> buffer_;
    std::atomic<size_t> head_, tail_;
};
