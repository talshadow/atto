#include "udpreciver.hpp"

UDPReciver::UDPReciver(bclasses::CBDataFunc&& filterFunction)
    : m_filterFunction{std::move(filterFunction)}
    , m_socket{nullptr}
{
    LOG_TRACE_MESSAGE("Counter {} V {}", m_packageCounter.load(), m_limitValue.load());
}

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
    if (code) {
        LOG_ERROR_MESSAGE(code.message());
        return false;
    }
    if (sizeof(bclasses::MessageStruct) != readSize) {
        LOG_ERROR_MESSAGE("Wrong message size");
    }
    // #ifdef PERFOMANCE_TEST
    updateCounters(msg.MessageType, msg.MessageData);
    // #endif
    m_filterFunction(std::move(msg));
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
// #ifdef PERFOMANCE_TEST
void UDPReciver::updateCounters(uint8_t packageType, unsigned amount) {

    if (packageType == bclasses::ServicePackageType) {
        m_limitValue.fetch_add(amount+1, std::memory_order_relaxed);
        LOG_INFO_MESSAGE("Update counter to: {} ", amount+1);
    }

    auto limitValue = m_limitValue.load(std::memory_order_acquire);


    if (limitValue != 0) {
        m_packageCounter.fetch_add(1U, std::memory_order_relaxed);
        auto counter = m_packageCounter.load(std::memory_order_acquire);

        if (limitValue <= counter) {
            m_limitValue.store(0, std::memory_order_relaxed);
            LOG_INFO_MESSAGE("recived all data: {} ", m_socket->socket().local_endpoint().port());
            m_finished.set_value(true);
        }
    };
}
// #endif
