#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace LGE {

enum class LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger {
public:
    static void Init(const std::string& logFile = "engine.log");
    static void Shutdown();
    
    static void Log(LogLevel level, const std::string& message, const char* file, int line);
    
    template<typename... Args>
    static void Debug(const std::string& format, Args... args) {
        Log(LogLevel::DEBUG, Format(format, args...), __FILE__, __LINE__);
    }
    
    template<typename... Args>
    static void Info(const std::string& format, Args... args) {
        Log(LogLevel::INFO, Format(format, args...), __FILE__, __LINE__);
    }
    
    template<typename... Args>
    static void Warning(const std::string& format, Args... args) {
        Log(LogLevel::WARNING, Format(format, args...), __FILE__, __LINE__);
    }
    
    template<typename... Args>
    static void Error(const std::string& format, Args... args) {
        Log(LogLevel::ERROR, Format(format, args...), __FILE__, __LINE__);
    }
    
    template<typename... Args>
    static void Fatal(const std::string& format, Args... args) {
        Log(LogLevel::FATAL, Format(format, args...), __FILE__, __LINE__);
    }

private:
    template<typename... Args>
    static std::string Format(const std::string& format, Args... args) {
        size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
        std::unique_ptr<char[]> buffer(new char[size]);
        snprintf(buffer.get(), size, format.c_str(), args...);
        return std::string(buffer.get(), buffer.get() + size - 1);
    }
    
    static std::string GetTimestamp();
    static std::string LogLevelToString(LogLevel level);
    
    static std::ofstream s_LogFile;
    static bool s_Initialized;
};

}

#define LGE_LOG_DEBUG(...) ::LGE::Logger::Debug(__VA_ARGS__)
#define LGE_LOG_INFO(...) ::LGE::Logger::Info(__VA_ARGS__)
#define LGE_LOG_WARNING(...) ::LGE::Logger::Warning(__VA_ARGS__)
#define LGE_LOG_ERROR(...) ::LGE::Logger::Error(__VA_ARGS__)
#define LGE_LOG_FATAL(...) ::LGE::Logger::Fatal(__VA_ARGS__)
