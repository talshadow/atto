#include "udpsender.hpp"
bclasses::Shared_ptr<UDPSender> UDPSender::instance(bclasses::IO_service& service,
                                                    std::vector<bclasses::MessageStruct>&& dataSet,
                                                    std::string_view address,
                                                    unsigned remotePort)
{
    bclasses::Shared_ptr<UDPSender> producer = std::make_shared<UDPSender>(std::move(dataSet));
    try {
        if (producer != nullptr) {
            auto readFunction = std::bind(&UDPSender::doRead,
                                          producer,
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3);
            auto writeFunction = std::bind(&UDPSender::doWrite, producer, std::placeholders::_1, std::placeholders::_2);
            auto sock = bclasses::UDPSocket::instanceClient(service,
                                                            address,
                                                            remotePort,
                                                            std::move(readFunction),
                                                            std::move(writeFunction));
            producer->setSocket(std::move(sock));
            producer->startSendSequence();
        }
    } catch (std::exception const& e) {
        LOG_ERROR_MESSAGE(e.what());
    }
    return producer;
}

UDPSender::UDPSender(DArray&& data)
    : m_data(std::move(data))
    , m_current(m_data.begin())
{}

bool UDPSender::doWrite(bclasses::ErrorCode const& error, size_t bTransferred)
{
    if (error) {
        m_isFinished.set_value(false);
        LOG_ERROR_MESSAGE("{}:{}", __PRETTY_FUNCTION__, error.message());
        m_sock->close();
        return false;
    }

    if (bTransferred == sizeof(*m_current)) {
        ++m_current;
        if (m_current != m_data.end()) {
            m_sock->write(*m_current);
        } else {
            m_isFinished.set_value(true);
            LOG_TRACE_MESSAGE("All data send: {}", m_data.size());
            m_sock->close();
        }
    }
    return true;
}

bool UDPSender::doRead(bclasses::MessageStruct& data, bclasses::ErrorCode const& error, size_t bTransferred)
{
    if (error) {
        LOG_TRACE_MESSAGE("{}:{}", __PRETTY_FUNCTION__, error.message());
    }
    return false;
}

void UDPSender::setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock)
{
    m_sock = std::move(sock);
}

void UDPSender::startSendSequence()
{
    TRACE_LOG;
    m_sock->write(*m_current);
}
