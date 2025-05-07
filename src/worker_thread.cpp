#include "../include/worker_thread.h"
#include <thread>
#include <chrono>

WorkerThread::WorkerThread() : running_(true) {}

WorkerThread::~WorkerThread() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void WorkerThread::start() {
    thread_ = std::thread([this]() { run(); });
}

void WorkerThread::run() {
    std::function<void()> job;
    while (running_) {
        if (queue_.pop(job)) {
            job();
        } else {
            std::this_thread::yield();
        }
    }
}

bool WorkerThread::try_enqueue(std::function<void()> job) {
    return queue_.push(std::move(job));
}

bool WorkerThread::queue_full() const {
    return queue_.size() >= 10;
}
