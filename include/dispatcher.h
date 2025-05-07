#pragma once
#include "worker_thread.h"
#include "lockfree_queue.h"
#include <vector>
#include <span>

class Dispatcher {
public:
    Dispatcher(size_t num_threads, std::span<uint8_t> input, std::span<uint8_t> output);
    void start_encoding();
    void start_verification();
    void wait();

private:
    size_t num_threads_;
    std::span<uint8_t> input_;
    std::span<uint8_t> output_;
    std::vector<WorkerThread> workers_;
    LockFreeQueue<std::function<void()>> result_queue_;
};
