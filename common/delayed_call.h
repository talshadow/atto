#pragma once
#include "tracelog.h"
#include "types.hpp"
#include <concepts>

namespace bclasses {

inline constexpr unsigned kDefaultDelay = 300U;
// Note: pTimer should be alive while asynchronous call
template<std::invocable TCallable>
void AsyncDelayCall(ba::deadline_timer* pTimer, TCallable&& callable, unsigned delayTime = kDefaultDelay)
{
    ErrorCode eCode;
    pTimer->expires_from_now(boost::posix_time::milliseconds(delayTime), eCode);
    if (eCode) {
        LOG_ERROR_MESSAGE("DelayedCall Started with error:{} - {} ", eCode.value(), eCode.message());
        return;
    }
    auto callback = [m_callable = std::move(callable)](ErrorCode const& eCode) {
        if (eCode) {
            LOG_ERROR_MESSAGE("DelayedCall finished with error:{} - {} ", eCode.value(), eCode.message());
            return;
        }
        std::invoke(m_callable);
    };
    pTimer->async_wait(std::move(callback));
}
} // namespace bclasses
