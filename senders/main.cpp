#include <future>
#include <iostream>

#include "udpsender.hpp"
#include <threadpool.hpp>

int main()
{
    bclasses::TimeCalc timer;
    try {
        unsigned ThreadsCount = std::thread::hardware_concurrency() + 1;
        auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);

        UDPSender::instance(pool->service(), 1);
        UDPSender::instance(pool->service(), 2);
        UDPSender::instance(pool->service(), 3);
        UDPSender::instance(pool->service(), 4);
        LOG_INFO_MESSAGE(std::format("Duration: {}", timer.getDuration()));
        std::cin.get();

    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(e.what());
    }

    return 0;
}
