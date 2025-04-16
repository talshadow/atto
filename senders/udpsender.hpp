#pragma once
#include <atomic>
#include <types.hpp>
#include <udpsocket.hpp>

constexpr auto kDataSize = 100001;

namespace bclasses {
extern MessageStruct IncomeData1[kDataSize];
extern MessageStruct IncomeData2[kDataSize];
extern MessageStruct IncomeData3[kDataSize];
extern MessageStruct IncomeData4[kDataSize];
} // namespace bclasses
using DArray = decltype(bclasses::IncomeData1);

class UDPSender : public bclasses::Enable_shared_from_this<UDPSender>
{
public:
    static bclasses::Shared_ptr<UDPSender> instance(bclasses::IO_service& service, unsigned dataSet);

private:
    UDPSender(DArray& data);
    bool doWrite(bclasses::ErrorCode const& error, size_t bTransferred);
    bool doRead(bclasses::MessageStruct&& data, bclasses::ErrorCode const& error, size_t bTransferred);
    void setSocket(bclasses::UDPSocket::UDPSocketSPtr&& sock);

private:
    void startSendSequence();
    DArray& m_data;
    using iteratorType = decltype(std::begin(m_data));

    iteratorType const m_begin;
    iteratorType m_current;
    iteratorType const m_end;
    bclasses::UDPSocket::UDPSocketSPtr m_sock;
};
