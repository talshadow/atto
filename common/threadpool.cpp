#include "threadpool.hpp"
#include <exception>
namespace bclasses {

COutType& Cout = std::cout;

ThreadPool::~ThreadPool()
{
    TRACE_LOG;
    for (auto& current : m_workerThreads) {
        if(current.second)
        {
            current.second->reset();
            current.second.reset();
        }
    }

    m_service.stop();

    for (auto&& current : m_workerThreads) {
        if (current.first.has_value() && current.first.value().joinable()) {
            current.first.value().join();
        }
    }
}
ThreadPool::ThreadPoolPtr ThreadPool::createInstance(unsigned const count)
{
    return ThreadPoolPtr(new ThreadPool(count));
}

ThreadPool::ThreadPool(unsigned const count)
    : m_workerThreads(count)
{
    TRACE_LOG;
    for (auto& current : m_workerThreads) {
        auto functor = [](WorkerPtr& worker, IO_service& io_service) {
            worker = std::make_unique<Worker>(io_service.get_executor());
            try {
                io_service.run();
            } catch (std::exception& e) {
                LOG_ERROR_MESSAGE(std::format("The thread from pool finished with exception: {}", e.what()));
            }
        };
        current.first = Thread(functor, std::ref(current.second), std::ref(m_service));
    }
}

} // namespace bclasses
