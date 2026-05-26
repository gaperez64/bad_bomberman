#include "logic/Stopwatch.h"

namespace logic {

Stopwatch& Stopwatch::instance() {
    static Stopwatch inst;
    return inst;
}

Stopwatch::Stopwatch() : m_last{Clock::now()}, m_start{Clock::now()} {}

void Stopwatch::tick() {
    auto now = Clock::now();
    m_delta = std::chrono::duration<float>(now - m_last).count();
    m_elapsed = std::chrono::duration<float>(now - m_start).count();
    m_last = now;
}

float Stopwatch::getDeltaTime() const {
    return m_delta;
}

float Stopwatch::getElapsed() const {
    return m_elapsed;
}

}  // namespace logic
