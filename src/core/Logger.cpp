#include "Logger.h"
#include <ctime>

namespace LGE {

std::ofstream Logger::s_LogFile;
bool Logger::s_Initialized = false;

void Logger::Init(const std::string& logFile) {
    if (s_Initialized) return;
    
    s_LogFile.open(logFile, std::ios::out | std::ios::trunc);
    s_Initialized = true;
    
    Info("Logger initialized");
}

void Logger::Shutdown() {
    if (!s_Initialized) return;
    
    Info("Logger shutting down");
    
    if (s_LogFile.is_open()) {
        s_LogFile.close();
    }
    s_Initialized = false;
}

void Logger::Log(LogLevel level, const std::string& message, const char* file, int line) {
    std::string timestamp = GetTimestamp();
    std::string levelStr = LogLevelToString(level);
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] [" + 
                             std::string(file) + ":" + std::to_string(line) + "] " + message;
    
    std::cout << logMessage << std::endl;
    
    if (s_LogFile.is_open()) {
        s_LogFile << logMessage << std::endl;
        s_LogFile.flush();
    }
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_s(&localTime, &time);
    
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

}
