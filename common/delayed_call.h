#pragma once
#include "tracelog.h"
#include "types.hpp"
#include <concepts>

namespace bclasses {
template<std::invocable TCallable>
void AsyncDelayCall(IOExecutor const& executor, TCallable&& callable, unsigned delayTime = 300U)
{
    TRACE_LOG;
    class DelayedCallFunctor
    {
    public:
        DelayedCallFunctor(TCallable&& callable, IOExecutor const& executor)
            : m_callable(std::move(callable))
            , m_timer{std::make_unique<ba::deadline_timer>(executor)}
        {
        }

        DelayedCallFunctor(DelayedCallFunctor&& rhs) noexcept
            : m_callable(std::move(rhs.m_callable))
            , m_timer(std::move(rhs.m_timer))
        {}

        DelayedCallFunctor(DelayedCallFunctor const& rhs) = delete;
        DelayedCallFunctor& operator=(DelayedCallFunctor const&) = delete;
        DelayedCallFunctor& operator=(DelayedCallFunctor&&) = delete;
        ~DelayedCallFunctor()
        {
            if (m_timer) {
                LOG_INFO_MESSAGE("DelayedCallFunctor pointer to timer: {}", static_cast<void*>(m_timer.get()));
            }
        }

        void operator()(ErrorCode const& eCode)
        {
            if (eCode) {
                LOG_ERROR_MESSAGE("DelayedCall finished with error:{} - {} ", eCode.value(), eCode.message());
                return;
            }
            std::invoke(m_callable);
        }

        ba::deadline_timer& get_timer() { return *m_timer; }

    private:
        TCallable m_callable;
        std::unique_ptr<ba::deadline_timer> m_timer;
    };

    DelayedCallFunctor delayedCallFunctor(std::move(callable), executor);
    auto&& timer = delayedCallFunctor.get_timer();
    ErrorCode eCode;
    timer.expires_from_now(boost::posix_time::milliseconds(delayTime), eCode);
    if(eCode)
    {
        LOG_ERROR_MESSAGE("DelayedCall Started with error:{} - {} ", eCode.value(), eCode.message());
    }
    timer.async_wait(std::move(delayedCallFunctor));
}

} // namespace bclasses
