#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <cstdint>
#include <cmath>

namespace LGE {

class Profiler {
public:
    using Timestamp = std::chrono::steady_clock::time_point;
    using Duration = std::chrono::duration<double>;
    using Milliseconds = std::chrono::duration<double, std::milli>;
    using Microseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult {
        std::string name;
        Timestamp startTime;
        Timestamp endTime;
        Duration duration;
        std::thread::id threadId;
        size_t callCount;
    };

    struct Statistic {
        std::string name;
        size_t callCount;
        double totalTime;
        double minTime;
        double maxTime;
        double averageTime;
        double medianTime;
        double stdDeviation;
    };

    static Profiler& getInstance() {
        static Profiler instance;
        return instance;
    }

    void beginSession(const std::string& name, const std::string& filepath = "profile.json") {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_currentSession) {
            endSession();
        }
        m_currentSession = std::make_unique<Session>();
        m_currentSession->name = name;
        m_currentSession->filepath = filepath;
        m_currentSession->startTime = std::chrono::steady_clock::now();
        m_results.clear();
    }

    void endSession() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_currentSession) return;

        writeResults();
        m_currentSession.reset();
    }

    void writeProfile(const ProfileResult& result) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_results.push_back(result);
    }

    std::vector<Statistic> getStatistics() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::unordered_map<std::string, std::vector<double>> times;

        for (const auto& result : m_results) {
            times[result.name].push_back(result.duration.count());
        }

        std::vector<Statistic> stats;
        stats.reserve(times.size());

        for (const auto& [name, durations] : times) {
            Statistic stat;
            stat.name = name;
            stat.callCount = durations.size();

            if (durations.empty()) {
                stats.push_back(stat);
                continue;
            }

            stat.totalTime = std::accumulate(durations.begin(), durations.end(), 0.0);
            stat.minTime = *std::min_element(durations.begin(), durations.end());
            stat.maxTime = *std::max_element(durations.begin(), durations.end());
            stat.averageTime = stat.totalTime / stat.callCount;

            std::vector<double> sorted = durations;
            std::sort(sorted.begin(), sorted.end());
            size_t mid = sorted.size() / 2;
            if (sorted.size() % 2 == 0) {
                stat.medianTime = (sorted[mid - 1] + sorted[mid]) / 2.0;
            } else {
                stat.medianTime = sorted[mid];
            }

            double sumSq = 0.0;
            for (double t : durations) {
                double diff = t - stat.averageTime;
                sumSq += diff * diff;
            }
            stat.stdDeviation = std::sqrt(sumSq / stat.callCount);

            stats.push_back(stat);
        }

        std::sort(stats.begin(), stats.end(),
            [](const Statistic& a, const Statistic& b) {
                return a.totalTime > b.totalTime;
            });

        return stats;
    }

    std::string getReport() const {
        std::ostringstream oss;
        auto stats = getStatistics();

        oss << "=== Profiler Report ===\n\n";
        oss << std::left << std::setw(30) << "Name"
            << std::right << std::setw(12) << "Calls"
            << std::right << std::setw(15) << "Total (ms)"
            << std::right << std::setw(15) << "Avg (ms)"
            << std::right << std::setw(15) << "Min (ms)"
            << std::right << std::setw(15) << "Max (ms)"
            << "\n";
        oss << std::string(110, '-') << "\n";

        for (const auto& stat : stats) {
            oss << std::left << std::setw(30) << stat.name
                << std::right << std::setw(12) << stat.callCount
                << std::right << std::setw(15) << std::fixed << std::setprecision(3) << stat.totalTime * 1000.0
                << std::right << std::setw(15) << stat.averageTime * 1000.0
                << std::right << std::setw(15) << stat.minTime * 1000.0
                << std::right << std::setw(15) << stat.maxTime * 1000.0
                << "\n";
        }

        return oss.str();
    }

    void printReport() const {
        std::cout << getReport();
    }

    void saveReport(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file) {
            file << getReport();
        }
    }

    size_t getResultCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_results.size();
    }

    void clearResults() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_results.clear();
    }

private:
    Profiler() = default;
    ~Profiler() {
        if (m_currentSession) {
            endSession();
        }
    }

    struct Session {
        std::string name;
        std::string filepath;
        Timestamp startTime;
    };

    void writeResults() {
        if (!m_currentSession) return;

        std::ofstream file(m_currentSession->filepath);
        if (!file) return;

        file << "{\"otherData\": {},\"traceEvents\":[\n";

        for (size_t i = 0; i < m_results.size(); i++) {
            const auto& result = m_results[i];
            writeProfileResult(file, result);
            if (i != m_results.size() - 1) {
                file << ",";
            }
            file << "\n";
        }

        file << "]}";
    }

    void writeProfileResult(std::ofstream& file, const ProfileResult& result) {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        std::string threadId = oss.str();

        double startTimeMs = std::chrono::duration_cast<Milliseconds>(
            result.startTime - m_currentSession->startTime
        ).count();

        double durationMs = result.duration.count() * 1000.0;

        file << "{";
        file << "\"cat\":\"function\",";
        file << "\"dur\":" << durationMs << ",";
        file << "\"name\":\"" << result.name << "\",";
        file << "\"ph\":\"X\",";
        file << "\"pid\":0,";
        file << "\"tid\":\"" << threadId << "\",";
        file << "\"ts\":" << startTimeMs;
        file << "}";
    }

    std::unique_ptr<Session> m_currentSession;
    std::vector<ProfileResult> m_results;
    mutable std::mutex m_mutex;
};

class ProfileTimer {
public:
    explicit ProfileTimer(const std::string& name)
        : m_name(name)
        , m_startTime(std::chrono::steady_clock::now())
        , m_stopped(false)
    {
    }

    ~ProfileTimer() {
        if (!m_stopped) {
            stop();
        }
    }

    void stop() {
        if (m_stopped) return;

        auto endTime = std::chrono::steady_clock::now();
        Profiler::ProfileResult result;
        result.name = m_name;
        result.startTime = m_startTime;
        result.endTime = endTime;
        result.duration = endTime - m_startTime;
        result.threadId = std::this_thread::get_id();
        result.callCount = 1;

        Profiler::getInstance().writeProfile(result);
        m_stopped = true;
    }

    double getElapsedTime() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(now - m_startTime).count();
    }

    double getElapsedMilliseconds() const {
        return getElapsedTime() * 1000.0;
    }

private:
    std::string m_name;
    Profiler::Timestamp m_startTime;
    bool m_stopped;
};

class ScopedProfile {
public:
    explicit ScopedProfile(const char* name)
        : m_name(name)
        , m_startTime(std::chrono::steady_clock::now())
    {
    }

    ~ScopedProfile() {
        auto endTime = std::chrono::steady_clock::now();
        Profiler::ProfileResult result;
        result.name = m_name;
        result.startTime = m_startTime;
        result.endTime = endTime;
        result.duration = endTime - m_startTime;
        result.threadId = std::this_thread::get_id();
        result.callCount = 1;
        Profiler::getInstance().writeProfile(result);
    }

private:
    const char* m_name;
    Profiler::Timestamp m_startTime;
};

class FrameProfiler {
public:
    FrameProfiler() : m_frameCount(0), m_totalFrameTime(0.0) {}

    void beginFrame() {
        m_frameStartTime = std::chrono::steady_clock::now();
    }

    void endFrame() {
        auto endTime = std::chrono::steady_clock::now();
        double frameTime = std::chrono::duration<double>(endTime - m_frameStartTime).count();
        
        m_frameCount++;
        m_totalFrameTime += frameTime;
        m_frameTimes.push_back(frameTime);
        
        if (m_frameTimes.size() > m_maxSamples) {
            m_frameTimes.erase(m_frameTimes.begin());
        }

        m_minFrameTime = std::min(m_minFrameTime, frameTime);
        m_maxFrameTime = std::max(m_maxFrameTime, frameTime);
    }

    uint64_t getFrameCount() const { return m_frameCount; }

    double getAverageFrameTime() const {
        if (m_frameTimes.empty()) return 0.0;
        double sum = std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0);
        return sum / m_frameTimes.size();
    }

    double getFPS() const {
        double avg = getAverageFrameTime();
        return avg > 0.0 ? 1.0 / avg : 0.0;
    }

    double getMinFrameTime() const { return m_minFrameTime; }
    double getMaxFrameTime() const { return m_maxFrameTime; }

    double getTotalFrameTime() const { return m_totalFrameTime; }

    void setMaxSamples(size_t count) { m_maxSamples = count; }
    size_t getMaxSamples() const { return m_maxSamples; }

    void reset() {
        m_frameCount = 0;
        m_totalFrameTime = 0.0;
        m_frameTimes.clear();
        m_minFrameTime = std::numeric_limits<double>::max();
        m_maxFrameTime = 0.0;
    }

    std::string getReport() const {
        std::ostringstream oss;
        oss << "Frame Profiler Report\n";
        oss << "=====================\n";
        oss << "Frame Count: " << m_frameCount << "\n";
        oss << "Total Time: " << std::fixed << std::setprecision(3) << m_totalFrameTime << "s\n";
        oss << "Average FPS: " << std::fixed << std::setprecision(1) << getFPS() << "\n";
        oss << "Average Frame Time: " << std::fixed << std::setprecision(3) << getAverageFrameTime() * 1000.0 << "ms\n";
        oss << "Min Frame Time: " << std::fixed << std::setprecision(3) << m_minFrameTime * 1000.0 << "ms\n";
        oss << "Max Frame Time: " << std::fixed << std::setprecision(3) << m_maxFrameTime * 1000.0 << "ms\n";
        return oss.str();
    }

private:
    Profiler::Timestamp m_frameStartTime;
    uint64_t m_frameCount;
    double m_totalFrameTime;
    std::vector<double> m_frameTimes;
    double m_minFrameTime = std::numeric_limits<double>::max();
    double m_maxFrameTime = 0.0;
    size_t m_maxSamples = 60;
};

class Counter {
public:
    explicit Counter(const std::string& name) : m_name(name), m_value(0) {}

    void increment(int64_t amount = 1) {
        m_value += amount;
    }

    void decrement(int64_t amount = 1) {
        m_value -= amount;
    }

    void set(int64_t value) {
        m_value = value;
    }

    int64_t get() const { return m_value; }
    const std::string& getName() const { return m_name; }

    void reset() { m_value = 0; }

private:
    std::string m_name;
    std::atomic<int64_t> m_value;
};

class CounterManager {
public:
    static CounterManager& getInstance() {
        static CounterManager instance;
        return instance;
    }

    Counter& getCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_counters.find(name);
        if (it == m_counters.end()) {
            auto result = m_counters.emplace(name, std::make_unique<Counter>(name));
            return *result.first->second;
        }
        return *it->second;
    }

    void increment(const std::string& name, int64_t amount = 1) {
        getCounter(name).increment(amount);
    }

    void decrement(const std::string& name, int64_t amount = 1) {
        getCounter(name).decrement(amount);
    }

    int64_t getValue(const std::string& name) {
        return getCounter(name).get();
    }

    void reset(const std::string& name) {
        getCounter(name).reset();
    }

    void resetAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& [name, counter] : m_counters) {
            counter->reset();
        }
    }

    std::string getReport() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::ostringstream oss;
        oss << "Counter Report\n";
        oss << "==============\n";
        for (const auto& [name, counter] : m_counters) {
            oss << std::left << std::setw(30) << name
                << std::right << std::setw(15) << counter->get() << "\n";
        }
        return oss.str();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_counters.clear();
    }

private:
    CounterManager() = default;
    std::unordered_map<std::string, std::unique_ptr<Counter>> m_counters;
    mutable std::mutex m_mutex;
};

class MemoryTracker {
public:
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }

    void allocate(size_t size, const char* file = nullptr, int line = 0) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_totalAllocated += size;
        m_currentAllocated += size;
        m_allocationCount++;
        m_peakAllocated = std::max(m_peakAllocated, m_currentAllocated);

        if (file) {
            m_allocations[reinterpret_cast<uintptr_t>(this)] = { size, file, line };
        }
    }

    void deallocate(size_t size) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentAllocated -= size;
        m_deallocationCount++;
    }

    size_t getTotalAllocated() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_totalAllocated;
    }

    size_t getCurrentAllocated() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_currentAllocated;
    }

    size_t getPeakAllocated() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_peakAllocated;
    }

    size_t getAllocationCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_allocationCount;
    }

    size_t getDeallocationCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_deallocationCount;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_totalAllocated = 0;
        m_currentAllocated = 0;
        m_peakAllocated = 0;
        m_allocationCount = 0;
        m_deallocationCount = 0;
        m_allocations.clear();
    }

    std::string getReport() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::ostringstream oss;
        oss << "Memory Tracker Report\n";
        oss << "====================\n";
        oss << "Total Allocated: " << formatSize(m_totalAllocated) << "\n";
        oss << "Current Allocated: " << formatSize(m_currentAllocated) << "\n";
        oss << "Peak Allocated: " << formatSize(m_peakAllocated) << "\n";
        oss << "Allocation Count: " << m_allocationCount << "\n";
        oss << "Deallocation Count: " << m_deallocationCount << "\n";
        oss << "Live Allocations: " << (m_allocationCount - m_deallocationCount) << "\n";
        return oss.str();
    }

private:
    MemoryTracker() = default;

    static std::string formatSize(size_t bytes) {
        const char* sizes[] = { "B", "KB", "MB", "GB", "TB" };
        int i = 0;
        double size = static_cast<double>(bytes);
        while (size >= 1024.0 && i < 4) {
            size /= 1024.0;
            i++;
        }
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << sizes[i];
        return oss.str();
    }

    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
    };

    mutable std::mutex m_mutex;
    size_t m_totalAllocated = 0;
    size_t m_currentAllocated = 0;
    size_t m_peakAllocated = 0;
    size_t m_allocationCount = 0;
    size_t m_deallocationCount = 0;
    std::unordered_map<uintptr_t, AllocationInfo> m_allocations;
};

}
