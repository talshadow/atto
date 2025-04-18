#include "tcpsender.hpp"
#include "udpreciver.hpp"
#include <threadpool.hpp>
#include <types.hpp>

int main()
{
    bclasses::TimeCalc timer;
    try {
        unsigned ThreadsCount = std::thread::hardware_concurrency() > 4 ? 4 : std::thread::hardware_concurrency() + 1;
        auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);

        auto sender = TCPSender::instance(pool->service().get_executor());

        auto reciver1 = UDPReciver::instance(sender->dataFunctor(), pool->service(), bclasses::UDPPortFirst);
        auto reciver2 = UDPReciver::instance(sender->dataFunctor(), pool->service(), bclasses::UDPPortSecond);

        LOG_INFO_MESSAGE(std::format("Duration: {}", timer.getDuration()));
        std::cin.get();
        std::cout << "Count of message: " << sender->count() << '\n';

    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(e.what());
    }

    return 0;
}
