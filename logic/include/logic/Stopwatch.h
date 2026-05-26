#pragma once

#include <chrono>

namespace logic {

class Stopwatch {
public:
    static Stopwatch& instance();

    void tick();
    float getDeltaTime() const;
    float getElapsed() const;

    // Non-copyable singleton
    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;

private:
    Stopwatch();

    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    TimePoint m_last;
    TimePoint m_start;
    float m_delta{0.f};
    float m_elapsed{0.f};
};

}  // namespace logic
