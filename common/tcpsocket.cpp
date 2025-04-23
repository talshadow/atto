#include "tcpsocket.hpp"
#include <algorithm>
#include <exception>
#include "delayed_call.h"

namespace bclasses {

TCPSession::TCPSessionSPtr TCPSession::createInstance(TCPExecutor const& current_executor)
{
    return TCPSessionSPtr(new TCPSession(current_executor));
}

TCPSession::TCPSessionSPtr TCPSession::createInstance(TCPExecutor const& current_executor,
                                                      char const* adress,
                                                      unsigned short port)
{
    auto session = TCPSessionSPtr(new TCPSession(current_executor));
    session->connect(adress, port);
    return session;
}

TCPSession::~TCPSession()
{
    TRACE_LOG;
    if (m_socket.is_open()) {
        LOG_INFO_MESSAGE(
            std::format("Close socket: {} -> {}", m_socket.local_endpoint().port(), m_socket.remote_endpoint().port()));
        close();
    }
}

void TCPSession::onRead(ErrorCode const& error, size_t bytes_transferred)
{
    TRACE_LOG;
    if (error == ba::error::would_block) {
        LOG_ERROR_MESSAGE(error.message());
    }
    if (error || bytes_transferred == 0U) {
        LOG_ERROR_MESSAGE(std::format("Peer lost: {}", error.message()));
        reconnect();
        return;
    }

    LOG_TRACE_MESSAGE(static_cast<char const*>("Read data"));
    dataPrint(bytes_transferred);

    auto callback = [tcp = std::move(shared_from_this())](ErrorCode const& error, size_t bTransferred) {
        tcp->onRead(error, bTransferred);
    };
    m_socket.async_read_some(ba::buffer(m_readBuffer), std::move(callback));
}

void TCPSession::onWrite(ErrorCode const& error, size_t size)
{
    TRACE_LOG;
    LOG_TRACE_MESSAGE(std::format("Write block {} bytes", size));
    if (error == ba::error::would_block) {
        LOG_ERROR_MESSAGE(error.message());
    }
    if (error) {
        LOG_ERROR_MESSAGE(error.message());
        LOG_WARNING_MESSAGE("Peer lost");
        reconnect();
    }
}

void TCPSession::onConnect(ErrorCode const& error)
{
    TRACE_LOG;
    if (error) {
        LOG_ERROR_MESSAGE(error.message());
        reconnect();
        return;
    }
    auto local_endpoint = m_socket.local_endpoint();
    if ( 0 != m_endpoint.port()) {
        m_endpoint = m_socket.remote_endpoint();
    }
    LOG_INFO_MESSAGE(std::format("connected: {}:{} --> {}:{} ",
                                 local_endpoint.address().to_string(),
                                 local_endpoint.port(),
                                 m_endpoint.address().to_string(),
                                 m_endpoint.port()));
    if (m_endpoint.port() == local_endpoint.port() && m_endpoint.address() == local_endpoint.address()) {
        LOG_ERROR_MESSAGE("Circular connection!");
        reconnect();
        return;
    }
    execute();
}

void TCPSession::connect(char const* adress, unsigned short port)
{
    m_endpoint = TCPEndpoint(IP::address::from_string(adress), port);
    connect();
}

void TCPSession::connect()
{
    TRACE_LOG;
    LOG_TRACE_MESSAGE(static_cast<char const*>("connect"));
    if (!m_endpoint.port()) {
        //It is a server socket. The client should initialize the re-connection.
        return;
    }
    if (m_socket.is_open()) {
        LOG_INFO_MESSAGE("Socket is open");
        close();
    }
    auto callback = [session = std::move(shared_from_this())](ErrorCode const& code) { session->onConnect(code); };
    m_socket.async_connect(m_endpoint, std::move(callback));
}

void TCPSession::reconnect()
{
    TRACE_LOG;
    LOG_TRACE_MESSAGE(static_cast<char const*>("connect"));
    if (!m_endpoint.port()) {
        //It is a server socket. The client should initialize the re-connection.
        return;
    }
    if (m_socket.is_open()) {
        LOG_INFO_MESSAGE("Socket is open");
        close();
    }

    auto delayedExecution = [session = std::move(shared_from_this()), this]() {
        connect();
    };

    AsyncDelayCall(m_socket.get_executor(), std::move(delayedExecution), 150U);
}

void TCPSession::execute()
{
    auto callback = [tcp = std::move(shared_from_this())](ErrorCode const& error, size_t bTransferred) {
        tcp->onRead(error, bTransferred);
    };
    m_socket.async_read_some(ba::buffer(m_readBuffer), std::move(callback));
}

void TCPSession::write(DataVector const& data)
{
    ba::async_write(m_socket,
                    ba::buffer(data.data(), data.size()),
                    [tcp = shared_from_this()](ErrorCode const& error, size_t bTranfered) {
                        tcp->onWrite(error, bTranfered);
                    });
}

void TCPSession::close()
{
    if (m_socket.is_open()) {
        ErrorCode eCode;
        m_socket.cancel(eCode);
        if (eCode) {
            LOG_ERROR_MESSAGE(eCode.message());
        }
        m_socket.shutdown(TCPSock::shutdown_both, eCode);
        if (eCode) {
            LOG_ERROR_MESSAGE(eCode.message());
        }
        m_socket.close(eCode);
        if (eCode) {
            LOG_ERROR_MESSAGE(eCode.message());
        }
    }
}

bool TCPSession::to_non_blocking_mode()
{
    TRACE_LOG;
    if (!m_socket.non_blocking()) {
        ErrorCode eCode;
        m_socket.non_blocking(true, eCode);
        if (eCode) {
            LOG_WARNING_MESSAGE(std::format("Socket work in blocking mode: {}", eCode.message()));
            return false;
        }
    }
    return true;
}

TCPSession::TCPSession(TCPExecutor const& current_executor)
    : m_socket(current_executor)
{
    try {
        if (m_socket.is_open()) {
            m_socket.non_blocking(true);
        }
    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(std::format("Socket running in blocking mode: {}", e.what()));
    }
}

void TCPSession::dataPrint(size_t bytes_transferred)
{
    size_t currenPosition = 0;
    while (bytes_transferred > currenPosition) {
        uint8_t* pBegin = m_readBuffer.data() + currenPosition;
        size_t size2copy = ((bytes_transferred - currenPosition) >= sizeof(MessageStruct))
                               ? (sizeof(MessageStruct) - currentPos)
                               : (sizeof(MessageStruct) - (bytes_transferred - currenPosition) - currentPos);
        auto* pEnd = (pBegin + size2copy);
        std::copy(pBegin, pEnd, m_struct.data() + currentPos);
        currentPos += size2copy;
        if (sizeof(MessageStruct) == currentPos) {
            auto* ptr = reinterpret_cast<MessageStruct*>(m_struct.data());
            currentPos = 0;
            LOG_INFO_MESSAGE("{}", bclasses::to_string(*ptr));
        }
        currenPosition += size2copy;
    }
}
} // namespace bclasses
