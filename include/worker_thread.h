#pragma once
#include "lockfree_queue.h"
#include <functional>
#include <thread>
#include <atomic>

class WorkerThread {
public:
    WorkerThread();
    ~WorkerThread();

    void start();
    bool try_enqueue(std::function<void()> job);
    bool queue_full() const;

private:
    void run();
    std::atomic<bool> running_;
    LockFreeQueue<std::function<void()>> queue_;
    std::thread thread_;
};
