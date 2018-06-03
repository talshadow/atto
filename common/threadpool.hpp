#pragma once
#include "types.hpp"

namespace bclasses {

class ThreadPool {
 public:
  using ThreadPoolPtr = Unique_ptr<ThreadPool>;
  ~ThreadPool();
  ThreadPool(ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  template <typename Callable>
  void operator()(Callable&& fuctor) {
    m_service.post(std::forward<Callable>(fuctor));
  }

  IO_service& service() noexcept {return m_service;}
  static ThreadPoolPtr createInstance(unsigned count);

 private:
  ThreadPool(unsigned count);

  WorkerThreads m_workerThreads;
  IO_service m_service;
};
}  // namespace bclasses
