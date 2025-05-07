// main.cpp
#include "../include/dispatcher.h"
#include <iostream>
#include <vector>
#include <random>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc != 5 || std::strcmp(argv[1], "-size") != 0 || std::strcmp(argv[3], "-threads") != 0) {
        std::cerr << "Usage: ./app -size <bytes> -threads <num>\n";
        return 1;
    }

    size_t buffer_size = std::stoul(argv[2]);
    size_t thread_count = std::stoul(argv[4]);

    std::cout << "Buffer size " << buffer_size << " bytes\n";
    std::cout << "Starting " << thread_count << " threads\n";

    std::vector<uint8_t> buffer1(buffer_size);
    std::vector<uint8_t> buffer2(buffer_size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    for (auto& byte : buffer1) {
        byte = dist(gen);
    }

    Dispatcher dispatcher(thread_count, buffer1, buffer2);
    dispatcher.start_encoding();
    dispatcher.wait();

    dispatcher.start_verification();
    dispatcher.wait();

    std::cout << "Done\n";
    return 0;
}
