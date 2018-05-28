#pragma once
#include "types.hpp"
namespace bclasses {

template <typename T, typename Mutex>
class EAPointer {
  Shared_ptr<Mutex> mutex;
  Shared_ptr<T> ptr;

  void lock() const { mutex->lock(); }
  void unlock() const { mutex->unlock(); }

 public:
  class Proxy {
    Unique_lock<Mutex> lock;
    T* const ptr;

   public:
    Proxy(T* const p, Mutex& mtx) : lock(mtx), ptr(p) { Cout << "acquire\n"; }
    Proxy(Proxy&& px) : lock(std::move(px.lock)), ptr(px.ptr) {}
    ~Proxy() { Cout << "release\n"; }

    Proxy(Proxy& px) = delete;
    Proxy& operator=(Proxy&) = delete;
    Proxy& operator=(Proxy&&) = delete;

    T* operator->() { return ptr; }
    const T* operator->() const { return ptr; }
  };

  EAPointer();
  EAPointer(EAPointer&) = delete;
  EAPointer(EAPointer&&) = delete;

  EAPointer& operator=(EAPointer&) = delete;
  EAPointer& operator=(EAPointer&&) = delete;

  template <typename... Args>
  EAPointer(Args... args) : mutex(make_shared<Mutex>()), ptr(make_shared<T>(args...)) {}

  Proxy operator->() { return proxy(ptr.get(), *mutex); }
  const Proxy operator->() const { return proxy(ptr.get(), *mutex); }
  ~EAPointer();
};
}  // namespace bclasses
