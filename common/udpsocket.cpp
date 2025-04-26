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

void UDPSocket::read()
{
    auto callback = [udp = shared_from_this()](ErrorCode const& error, size_t bTransferred) {
        udp->onRead(error, bTransferred);
    };
    m_socket.async_receive_from(ba::buffer(&m_reciveData, sizeof(m_reciveData)), m_address, std::move(callback));
}

void UDPSocket::execute()
{
    read();
}

void UDPSocket::onRead(ErrorCode const& error, size_t bTransferred)
{
    if (error) {
        LOG_ERROR_MESSAGE("udp onRead: {}", error.what());
        close();
        return;
    }
    auto data = m_reciveData;
    execute();
    if ((logicR && logicR(data, error, bTransferred)) || (!logicR && !error)) {
       // execute();
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

UDPSocket::UDPSocket(
    IO_service& service, std::string_view ipAdress, unsigned short port, CBRFuntion lRead, CBFuntion lWrite)
    : m_socket(service, UDPEndpoint(IP::udp::v4(), ipAdress.empty() ? port : 0))
    , m_address(ipAdress.empty() ? UDPEndpoint() : UDPEndpoint(IP::address::from_string(ipAdress.data()), port))
    , logicR{std::move(lRead)}
    , logicW{std::move(lWrite)}
{
    to_non_blocking_mode();
    boost::asio::socket_base::receive_buffer_size optRecvSize(sizeof(MessageStruct) * 100001 * 2);
    ErrorCode eCode;
    m_socket.set_option(optRecvSize, eCode);
    if (eCode) {
        LOG_INFO_MESSAGE("Set socket read buffer failure: {}", eCode.what());
    }
    auto info_string = std::format("local port: {}\nis open: {}\nremoute: {}:{}",
                                   m_socket.local_endpoint().port(),
                                   m_socket.is_open(),
                                   m_address.address().to_string(),
                                   m_address.port());
    LOG_TRACE_MESSAGE(std::move(info_string));
}

} // namespace bclasses
