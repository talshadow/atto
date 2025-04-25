#include "udpsocket.hpp"
namespace bclasses {

UDPSocket::UDPSocketSPtr UDPSocket::instanceServer(IO_service& service,
                                                   unsigned short port,
                                                   CBRFuntion&& lR,
                                                   CBFuntion&& lW)
{
    try {
        auto socket = UDPSocket::UDPSocketSPtr(new UDPSocket(service, std::string_view(), port, std::move(lR), std::move(lW)));
        socket->execute();
        return socket;
    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(e.what());
    }
    return {};
}

UDPSocket::UDPSocketSPtr UDPSocket::instanceClient(
    IO_service& service, std::string_view IPAaddress, unsigned short port, CBRFuntion&& lR, CBFuntion&& lW)
{
    auto socket = UDPSocket::UDPSocketSPtr(new UDPSocket(service, IPAaddress, port, std::move(lR), std::move(lW)));
    socket->execute();
    return socket;
}

void UDPSocket::write(MessageStruct const& data)
{
    sendData = data;
    m_socket.async_send_to(ba::buffer(&sendData, sizeof(sendData)),
                           m_address,
                           [udp = shared_from_this()](ErrorCode const& error, size_t bTransferred) {
                               udp->onWrite(error, bTransferred);
                           });
}

void UDPSocket::execute()
{
    m_socket.async_receive_from(ba::buffer(m_data.data(), m_data.size()),
                                m_address,
                                [udp = shared_from_this()](ErrorCode const& error, size_t bTransferred) {
                                    udp->onRead(error, bTransferred);
                                });
}

void UDPSocket::onRead(ErrorCode const& error, size_t bTransferred)
{
    auto* pData = reinterpret_cast<MessageStruct*>(m_data.data());
    MessageStruct data{*pData};

    if ((logicR && logicR(std::move(data), error, bTransferred)) || (!logicR && !error)) {
        execute();
    } else {
        close();
    }
}

void UDPSocket::onWrite(ErrorCode const& error, size_t bTransferred)
{
    if ((logicW && !logicW(error, bTransferred)) || (!logicW && error)) {
        close();
    }
}

void UDPSocket::close() {}

bool UDPSocket::to_non_blocking_mode()
{
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

UDPSocket::UDPSocket(IO_service& service, std::string_view ipAdress, unsigned short port, CBRFuntion lR, CBFuntion lW)
    : m_socket(service, UDPEndpoint(IP::udp::v4(), ipAdress.empty() ? port : 0))
    , m_address(ipAdress.empty() ? UDPEndpoint() : UDPEndpoint(IP::address::from_string(ipAdress.data()), port))
    , m_data(sizeof(MessageStruct))
    , logicR{std::move(lR)}
    , logicW{std::move(lW)}
{
    to_non_blocking_mode();
    auto info_string = std::format("local port: {}\nis open: {}\nremoute: {}:{}",
                                   m_socket.local_endpoint().port(),
                                   m_socket.is_open(),
                                   m_address.address().to_string(),
                                   m_address.port());
    LOG_TRACE_MESSAGE(std::move(info_string));
}

} // namespace bclasses
