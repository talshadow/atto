#include "tcpsocket.hpp"

namespace bclasses {

TCPSession::TCPSessionSPtr TCPSession::createInstance(IO_service& service) {
  return TCPSessionSPtr(new TCPSession(service));
}

TCPSession::TCPSessionSPtr TCPSession::createInstance(IO_service& service, const char* adress, unsigned short port) {
   auto session = TCPSessionSPtr(new TCPSession(service));
   session->connect(adress,port);
   return session;
}

void TCPSession::onRead(const ErrorCode& error, size_t bytes_transferred) {
  if ((!error) && bytes_transferred) {
    TRACE_LOG_MESSAGE(static_cast<char const*>("Read data"));
    m_socket.async_read_some(ba::buffer(m_readBuffer), std::bind(&TCPSession::onRead, shared_from_this(),
                                                                 std::placeholders::_1, std::placeholders::_2));
  }
}

void TCPSession::onWrite(const ErrorCode& error) {
  if (error) {
    ERROR_LOG_MESSAGE(error.message());
  }
}

void TCPSession::onConnect(const ErrorCode& error) {
  if (!error) {
    TRACE_LOG_MESSAGE(static_cast<char const*>("connected"));
    execute();
  } else {
    TRACE_LOG_MESSAGE(static_cast<char const*>("reconnect"));
    connect(m_socket.remote_endpoint().address().to_string().c_str(),m_socket.remote_endpoint().port());
  }
}

void TCPSession::connect(const char *adress, unsigned short port) {
  TCPEndpoint endpoint(IP::address::from_string(adress), port);
  m_socket.async_connect(endpoint, std::bind(&TCPSession::onConnect,shared_from_this(),std::placeholders::_1));
}

void TCPSession::execute() {
  m_socket.async_read_some(ba::buffer(m_readBuffer), std::bind(&TCPSession::onRead, shared_from_this(),
                                                               std::placeholders::_1, std::placeholders::_2));
}

void TCPSession::write(const DataVector& data) {
  ba::async_write(m_socket, ba::buffer(data.data(), data.size()),
                  std::bind(&TCPSession::onWrite, shared_from_this(), std::placeholders::_1));
}

void TCPSession::close() {
  ErrorCode err;
  m_socket.shutdown(TCPSock::shutdown_both, err);
  if (err) {
    ERROR_LOG_MESSAGE(err.message());
  }
  m_socket.close();
}

TCPSession::TCPSession(IO_service& service) : m_socket(service), m_readBuffer() {}

TCPAcceptor::TCPAcceptorSPtr TCPAcceptor::createInstance(IO_service& service, const char* adress, unsigned short port) {
  auto instance = Shared_ptr<TCPAcceptor>(new TCPAcceptor(service, adress, port));
  instance->doAccept();
  return instance;
}

void TCPAcceptor::onAccept(const TCPSession::TCPSessionSPtr& session, const ErrorCode& error) {
  if (!error) {
    TRACE_LOG_MESSAGE(static_cast<char const*>("accept socket \n"));
    session->execute();
    doAccept();
  }
}

void TCPAcceptor::doAccept() {
  auto soc = TCPSession::createInstance(m_acceptor.get_io_service());
  m_acceptor.async_accept(soc->socket(),
                          std::bind(&TCPAcceptor::onAccept, shared_from_this(), soc, std::placeholders::_1));
}

TCPAcceptor::TCPAcceptor(IO_service& service, const char* adress, unsigned short port)
  : m_acceptor(service, TCPEndpoint(IP::address::from_string(adress), port)) {}

}  // namespace bclasses