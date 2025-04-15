#include "threadpool.hpp"
namespace bclasses {

COutType& Cout = std::cout;

ThreadPool::~ThreadPool() {
  TRACE_LOG;

  for (auto& current : m_workerThreads) {
    current.second.reset();
  }

  m_service.stop();

  for (auto& current : m_workerThreads) {
    if (current.first.joinable()) {
      current.first.join();
    }
  }
}

ThreadPool::ThreadPoolPtr ThreadPool::createInstance(const unsigned count) {
  return ThreadPoolPtr(new ThreadPool(count));
}

ThreadPool::ThreadPool(const unsigned count) : m_workerThreads(count) {
  TRACE_LOG;

  for (auto& current : m_workerThreads) {
    auto functor = [this, &current]() {
      current.second.reset(new Worker(this->m_service));
      m_service.run();
    };

    Thread thr(functor);

    current.first.swap(thr);

    if (thr.joinable()) {
      thr.join();
    }
  }
}

}  // namespace bclasses
