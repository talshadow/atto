#pragma once
#include <types.hpp>
#include <tcpsocket.hpp>
#include <messagesstore.hpp>

class TCPSender : public bclasses::Enable_shared_from_this<TCPSender> {
 public:
  static bclasses::Shared_ptr<TCPSender> instance(bclasses::IO_service& service);

  bool onSend(const bclasses::ErrorCode&, size_t);
  void write(bclasses::MessageStruct&& data);
  void pushData(bclasses::MessageStruct&& data);
  unsigned long count() const noexcept {return m_packageCounter;}

  bclasses::CBDataFunc dataFunctor();

 private:
  TCPSender(bclasses::IO_service& service);

 private:
  bclasses::AUnsigned m_packageCounter;
  bclasses::TCPSession::TCPSessionSPtr m_session;
  bclasses::MessageStruct m_msg;
  bclasses::AHashMapStore m_store;
};
