#pragma once
#include "types.hpp"

namespace bclasses {

inline constexpr size_t DEFULT_READING_BUFFER_SIZE = 2048;

class TCPSession : public EnableSharedFromThis<TCPSession>
{
public:
    using TCPSessionSPtr = Shared_ptr<TCPSession>;

    TCPSession(TCPSession&) = delete;
    TCPSession(TCPSession&&) = delete;
    TCPSession& operator=(TCPSession&) = delete;
    TCPSession& operator=(TCPSession&&) = delete;
    ~TCPSession();

    static TCPSessionSPtr createInstance(TCPExecutor const& current_executor);
    static TCPSessionSPtr createInstance(TCPExecutor const& current_executor, char const* adress, unsigned short port);

    void onRead(ErrorCode const& error, size_t bytes_transferred);
    void onWrite(ErrorCode const& error, size_t size);
    void onConnect(ErrorCode const& error);

    void connect(char const* adress, unsigned short port);
    void execute();
    void write(DataVector const& data);
    void close();
    void read();
    bool to_non_blocking_mode();
    TCPSock& socket() { return m_socket; }

private:
    TCPSession(TCPExecutor const& service);
    void dataPrint(size_t bytes_transferred);
    void connect();
    void reconnect();

    TCPSock m_socket;
    ba::deadline_timer m_Timer;
    TCPEndpoint m_endpoint;
    DataVector m_sendData;
    unsigned currentPos {0};
    std::array<uint8_t, DEFULT_READING_BUFFER_SIZE> m_readBuffer{};
    std::array<uint8_t, sizeof(MessageStruct)> m_struct{};
};


} // namespace bclasses
