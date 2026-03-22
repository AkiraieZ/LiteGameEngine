#include "Timer.h"
#include "Logger.h"

namespace LGE {

Timer::Timer() {
    Reset();
}

void Timer::Reset() {
    m_Start = std::chrono::high_resolution_clock::now();
}

float Timer::Elapsed() const {
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - m_Start;
    return duration.count();
}

float Timer::ElapsedMilliseconds() const {
    return Elapsed() * 1000.0f;
}

ScopedTimer::ScopedTimer(const std::string& name)
    : m_Name(name) {
}

ScopedTimer::~ScopedTimer() {
    LGE_LOG_INFO("ScopedTimer '%s' took %.3f ms", m_Name.c_str(), m_Timer.ElapsedMilliseconds());
}

}
