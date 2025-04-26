#include "tcpsender.hpp"
#include "udpreciver.hpp"
#include <threadpool.hpp>
#include <types.hpp>

int main()
{
    // #ifdef PERFOMANCE_TEST
    bclasses::TimeCalc timer;
    // #endif
    try {
        unsigned ThreadsCount = std::thread::hardware_concurrency() > 4 ? 4 : std::thread::hardware_concurrency() + 1;
        auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);

        auto sender = TCPSender::instance(pool->service().get_executor());

        auto reciver1 = UDPReciver::instance(sender->dataFunctor(), pool->service(), bclasses::UDPPortFirst);
        auto reciver2 = UDPReciver::instance(sender->dataFunctor(), pool->service(), bclasses::UDPPortSecond);
        // #ifdef PERFOMANCE_TEST
        //auto completeReciver1 = reciver1->getFutureComlete();
        //auto completeReciver2 = reciver2->getFutureComlete();
        //completeReciver1.get();
        //completeReciver2.get();

        // #endif

        std::cin.get();
        LOG_INFO_MESSAGE(std::format("Duration: {}", timer.getDuration()));
        auto amountPort1 = reciver1->getRecevedPackageCount();
        auto amountPort2 = reciver2->getRecevedPackageCount();
        LOG_INFO_MESSAGE(std::format("recive port {} : {}", bclasses::UDPPortFirst, amountPort1));
        LOG_INFO_MESSAGE(std::format("recive port {} : {}", bclasses::UDPPortSecond, amountPort2));
        std::cout << "Count of message: " << sender->count() << '\n';


    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(e.what());
    }

    return 0;
}
