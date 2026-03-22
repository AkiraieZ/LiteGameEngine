#pragma once

#include <chrono>

namespace LGE {

class Timer {
public:
    Timer();
    ~Timer() = default;
    
    void Reset();
    float Elapsed() const;
    float ElapsedMilliseconds() const;
    
private:
    std::chrono::high_resolution_clock::time_point m_Start;
};

class ScopedTimer {
public:
    ScopedTimer(const std::string& name);
    ~ScopedTimer();
    
private:
    std::string m_Name;
    Timer m_Timer;
};

}
