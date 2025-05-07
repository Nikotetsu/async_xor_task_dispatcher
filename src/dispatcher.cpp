// dispatcher.cpp
#include "../include/dispatcher.h"
#include <iostream>
#include <thread>
#include <chrono>

constexpr size_t BlockSize = 256;

Dispatcher::Dispatcher(size_t num_threads, std::span<uint8_t> input, std::span<uint8_t> output)
    : num_threads_(num_threads), input_(input), output_(output) {
    workers_.resize(num_threads_);
    for (auto& worker : workers_) {
        worker.start();
    }
}

void Dispatcher::start_encoding() {
    size_t total_blocks = input_.size() / BlockSize;
    for (size_t i = 0; i < total_blocks; ++i) {
        auto offset = i * BlockSize;
        auto job = [this, offset]() {
            for (size_t j = 0; j < BlockSize; ++j) {
                output_[offset + j] = input_[offset + j] ^ 0xF;
            }
            result_queue_.push([offset]() {});
        };

        while (true) {
            for (auto& w : workers_) {
                if (!w.queue_full() && w.try_enqueue(job)) {
                    goto scheduled;
                }
            }
            std::this_thread::yield();
        scheduled:;
        }
    }
}

void Dispatcher::start_verification() {
    size_t total_blocks = output_.size() / BlockSize;
    for (size_t i = 0; i < total_blocks; ++i) {
        auto offset = i * BlockSize;
        auto job = [this, offset]() {
            bool ok = true;
            for (size_t j = 0; j < BlockSize; ++j) {
                if ((output_[offset + j] ^ 0xF) != input_[offset + j]) {
                    ok = false;
                    break;
                }
            }
            result_queue_.push([offset, ok]() {
                if (ok)
                    std::cout << "OK. address " << offset << "\n";
                else
                    std::cout << "FAIL. address " << offset << "\n";
            });
        };

        while (true) {
            for (auto& w : workers_) {
                if (!w.queue_full() && w.try_enqueue(job)) {
                    goto scheduled;
                }
            }
            std::this_thread::yield();
        scheduled:;
        }
    }
}

void Dispatcher::wait() {
    size_t total_blocks = input_.size() / BlockSize;
    size_t processed = 0;
    std::function<void()> callback;
    while (processed < total_blocks) {
        if (result_queue_.pop(callback)) {
            callback();
            ++processed;
        } else {
            std::this_thread::yield();
        }
    }
}
