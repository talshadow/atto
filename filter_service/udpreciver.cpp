#include "udpreciver.hpp"

UDPReciver::UDPReciver(bclasses::CBDataFunc&& filterFunction)
    : m_filterFunction{std::move(filterFunction)}
    , m_socket{nullptr}
{}

bclasses::Shared_ptr<UDPReciver> UDPReciver::instance(bclasses::CBDataFunc&& filterFunction,
                                                      bclasses::IO_service& service,
                                                      unsigned short port)
{
    try {
        auto reciver = bclasses::Shared_ptr<UDPReciver>(new UDPReciver(std::move(filterFunction)));
        auto socket = bclasses::UDPSocket::instanceServer(service, port, reciver->dataReader());
        reciver->setSocket(std::move(socket));
        return reciver;
    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE(e.what());
    }
    return {};
}

bool UDPReciver::onRead(bclasses::MessageStruct&& msg, bclasses::ErrorCode const& code, size_t readSize)
{
    if (!code) {
        if (sizeof(bclasses::MessageStruct) == readSize) {
            m_filterFunction(std::move(msg));
        } else {
            LOG_ERROR_MESSAGE(static_cast<char const*>("Wrong message size"));
        }
    } else {
        LOG_ERROR_MESSAGE(code.message());
        return false;
    }
    return true;
}

void UDPReciver::setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock)
{
    m_socket = std::move(sock);
}

bclasses::CBRFuntion UDPReciver::dataReader()
{
    return bclasses::CBRFuntion{std::bind(&UDPReciver::onRead,
                                          shared_from_this(),
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3)};
}
