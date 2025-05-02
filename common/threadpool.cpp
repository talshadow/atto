#include "threadpool.hpp"
#include <algorithm>
namespace bclasses {

ThreadPool::~ThreadPool()
{
    TRACE_LOG;
    auto hasBrokenThread = std::all_of(m_workerThreads.begin(), m_workerThreads.end(), [](Thread const& thread) {
        return !thread.joinable();
    });
    if (hasBrokenThread) {
        LOG_ERROR_MESSAGE("Some thread is not joinable");
    }
    m_guard.reset();
    m_service.stop();
    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}
ThreadPool::ThreadPoolPtr ThreadPool::createInstance(unsigned const count)
{
    return ThreadPoolPtr(new ThreadPool(count));
}

ThreadPool::ThreadPool(unsigned const count)
    : m_guard{m_service.get_executor()}
{
    TRACE_LOG;
    m_workerThreads.reserve(count);
    for (unsigned index = 0; index < count; ++index) {
        m_workerThreads.emplace_back(&ThreadPool::threadRutine, this);
    }
}

void ThreadPool::threadRutine()
{
    ErrorCode errCode;
    m_service.run(errCode);
    if(errCode)
    {
        LOG_ERROR_MESSAGE(std::format("The thread from pool finished with exception: {}", errCode.what()));
    }
}

} // namespace bclasses
