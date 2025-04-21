#pragma once
#include <chrono>
namespace bclasses {

class TimeCalc
{
public:
    using clock = std::chrono::high_resolution_clock;
    using TimeValue = decltype(clock::now());

    TimeCalc()
        : m_value{clock::now()}
    {}
    void reset() noexcept { m_value = clock::now(); }
    [[nodiscard]] long getDuration() const
    {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - m_value)).count();
    }

private:
    TimeValue m_value;
};

} // namespace bclasses


