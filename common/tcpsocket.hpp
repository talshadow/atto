#pragma once
#include "types.hpp"

namespace bclasses {

inline constexpr size_t DEFULT_READING_BUFFER_SIZE = 2048;

class TCPSession : public Enable_shared_from_this<TCPSession> {
 public:
  using TCPSessionSPtr = Shared_ptr<TCPSession>;

  TCPSession(TCPSession&) = delete;
  TCPSession(TCPSession&&) = delete;
  TCPSession& operator=(TCPSession&) = delete;
  TCPSession& operator=(TCPSession&&) = delete;
  ~TCPSession();

  static TCPSessionSPtr createInstance(const TCPExecutor& current_executor);
  static TCPSessionSPtr createInstance(const TCPExecutor& current_executor, const char* adress, unsigned short port);

  void onRead(const ErrorCode& error, size_t bytes_transferred);
  void onWrite(const ErrorCode& error);
  void onConnect(const ErrorCode& error);

  void connect(const char* adress, unsigned short port);
  void execute();
  void write(const DataVector& data);
  void close();
  bool to_non_blocking_mode();
  TCPSock& socket() { return m_socket; }

 private:
  TCPSession(const TCPExecutor& service);
  void dataPrint(size_t bytes_transferred);
  void connect();

  TCPEndpoint m_endpoint;
  TCPSock m_socket;
  DataVector m_sendData;
  unsigned currentPos;
  std::array<uint8_t, DEFULT_READING_BUFFER_SIZE> m_readBuffer;
  std::array<uint8_t, sizeof (MessageStruct)> m_struct;

};

class TCPAcceptor : public Enable_shared_from_this<TCPAcceptor> {
 public:
  using TCPAcceptorSPtr = Shared_ptr<TCPAcceptor>;

  TCPAcceptor(TCPAcceptor&) = delete;
  TCPAcceptor(TCPAcceptor&&) = delete;
  TCPAcceptor& operator=(TCPAcceptor&) = delete;
  TCPAcceptor& operator=(TCPAcceptor&&) = delete;
  ~TCPAcceptor();

  static TCPAcceptorSPtr createInstance(IO_service& service, const char* adress, unsigned short port);
  void onAccept(const TCPSession::TCPSessionSPtr& session, const ErrorCode& error);

 private:
  void doAccept();
  TCPAcceptor(IO_service& service, const char* adress, unsigned short port);

  TCPAccept m_acceptor;
};
}  // namespace bclasses
