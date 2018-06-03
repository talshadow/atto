#pragma once
#include "types.hpp"
#include "sharedmutex.hpp"

//
// Time to output: 197 ms
// Elements inserted: 256

namespace bclasses {
template <typename MAP, typename MutexT = SharedMutex>
class MessagesStore {
 public:
  bool findSet(const MessageStruct& msg) {
    if (!find(msg.MessageId)) {  // if not found
      LockGuard<MutexT> _lock(m_mutex);
      if (!findFunc(msg.MessageId)) {
        m_store.insert(std::make_pair(msg.MessageId, msg));
        return true;
      }
    }
    return false;
  }

  bool find(decltype(MessageStruct::MessageId) id) {
    ReaderMutex mtx(m_mutex);
    LockGuard<ReaderMutex> _lock(mtx);
    return findFunc(id);
  }

  unsigned size() const {
    ReaderMutex mtx(m_mutex);
    LockGuard<ReaderMutex> _lock(mtx);
    return m_store.size();
  }

 private:
  bool findFunc(decltype(MessageStruct::MessageId) id) { return m_store.find(id) != m_store.end(); }
  mutable MutexT m_mutex;
  MAP m_store;
};

//using MapStore = MessagesStore<Map, SharedMutex>;
using AHashMapStore= MessagesStore<Unordered_map, SharedMutex>;

}  // namespace bclasses
