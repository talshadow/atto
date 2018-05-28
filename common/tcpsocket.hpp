#pragma once
#include "types.hpp"

namespace bclasses {

class TCPSession : public Enable_shared_from_this<TCPSession> {
 public:
  using TCPSessionSPtr = Shared_ptr<TCPSession>;

  TCPSession(TCPSession&) = delete;
  TCPSession(TCPSession&&) = delete;
  TCPSession& operator=(TCPSession&) = delete;
  TCPSession& operator=(TCPSession&&) = delete;
  ~TCPSession() = default;

  static TCPSessionSPtr createInstance(IO_service& service);
  static TCPSessionSPtr createInstance(IO_service& service, const char* adress, unsigned short port);

  void onRead(const ErrorCode& error, size_t bytes_transferred);
  void onWrite(const ErrorCode& error);
  void onConnect(const ErrorCode& error);

  void connect(const char* adress, unsigned short port);
  void execute();
  void write(const DataVector& data);
  void close();
  TCPSock& socket() { return m_socket; }

 private:
  TCPSession(IO_service& service);

  TCPSock m_socket;
  DataVector m_sendData;
  std::array<uint8_t, 1024> m_readBuffer;
};

class TCPAcceptor : public Enable_shared_from_this<TCPAcceptor> {
 public:
  using TCPAcceptorSPtr = Shared_ptr<TCPAcceptor>;

  TCPAcceptor(TCPAcceptor&) = delete;
  TCPAcceptor(TCPAcceptor&&) = delete;
  TCPAcceptor& operator=(TCPAcceptor&) = delete;
  TCPAcceptor& operator=(TCPAcceptor&&) = delete;

  static TCPAcceptorSPtr createInstance(IO_service& service, const char* adress, unsigned short port);
  void onAccept(const TCPSession::TCPSessionSPtr& session, const ErrorCode& error);

 private:
  void doAccept();
  TCPAcceptor(IO_service& service, const char* adress, unsigned short port);

  TCPAccept m_acceptor;
};
}  // namespace bclasses
