#include "Time.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>

namespace LGE {

Clock::Clock()
    : m_startTime(std::chrono::steady_clock::now())
    , m_lastUpdateTime(m_startTime)
    , m_pauseStartTime()
    , m_accumulatedTime(Duration::zero())
{
}

void Clock::reset() {
    m_startTime = std::chrono::steady_clock::now();
    m_lastUpdateTime = m_startTime;
    m_pauseStartTime = Timestamp();
    m_accumulatedTime = Duration::zero();
    m_paused = false;
}

void Clock::restart() {
    reset();
}

void Clock::pause() {
    if (!m_paused) {
        m_paused = true;
        m_pauseStartTime = std::chrono::steady_clock::now();
    }
}

void Clock::resume() {
    if (m_paused) {
        m_paused = false;
        auto now = std::chrono::steady_clock::now();
        m_accumulatedTime += now - m_pauseStartTime;
        m_pauseStartTime = Timestamp();
    }
}

void Clock::setPaused(bool paused) {
    if (paused) {
        pause();
    } else {
        resume();
    }
}

double Clock::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    if (m_paused) {
        return std::chrono::duration_cast<Duration>(m_pauseStartTime - m_startTime - m_accumulatedTime).count();
    }
    return std::chrono::duration_cast<Duration>(now - m_startTime - m_accumulatedTime).count() * m_timeScale;
}

double Clock::getElapsedMilliseconds() const {
    return getElapsedTime() * 1000.0;
}

double Clock::getElapsedMicroseconds() const {
    return getElapsedTime() * 1000000.0;
}

double Clock::getElapsedNanoseconds() const {
    return getElapsedTime() * 1000000000.0;
}

void Clock::update() {
    m_lastUpdateTime = std::chrono::steady_clock::now();
}

Timer::Timer()
    : m_duration(1.0)
    , m_elapsed(0.0)
    , m_running(false)
    , m_looping(false)
{
}

Timer::Timer(double duration)
    : m_duration(duration)
    , m_elapsed(0.0)
    , m_running(false)
    , m_looping(false)
{
}

void Timer::start() {
    m_running = true;
    m_elapsed = 0.0;
}

void Timer::start(double duration) {
    m_duration = duration;
    start();
}

void Timer::stop() {
    m_running = false;
}

void Timer::reset() {
    m_elapsed = 0.0;
}

void Timer::restart() {
    m_elapsed = 0.0;
    m_running = true;
}

double Timer::getRemainingTime() const {
    return std::max(0.0, m_duration - m_elapsed);
}

double Timer::getElapsedTime() const {
    return m_elapsed;
}

double Timer::getProgress() const {
    if (m_duration <= 0.0) return 1.0;
    return std::min(1.0, m_elapsed / m_duration);
}

bool Timer::isFinished() const {
    return m_elapsed >= m_duration;
}

void Timer::update(double deltaTime) {
    if (!m_running) return;

    m_elapsed += deltaTime;

    if (m_elapsed >= m_duration) {
        if (m_callback) {
            m_callback();
        }

        if (m_looping) {
            m_elapsed -= m_duration;
            if (m_elapsed < 0.0) m_elapsed = 0.0;
        } else {
            m_running = false;
            m_elapsed = m_duration;
        }
    }
}

Stopwatch::Stopwatch()
    : m_startTime()
    , m_lastLapTime()
    , m_running(false)
{
}

void Stopwatch::start() {
    if (!m_running) {
        m_running = true;
        m_startTime = std::chrono::steady_clock::now();
        m_lastLapTime = m_startTime;
    }
}

void Stopwatch::stop() {
    if (m_running) {
        m_running = false;
        auto now = std::chrono::steady_clock::now();
        m_laps.push_back(std::chrono::duration_cast<Duration>(now - m_lastLapTime).count());
    }
}

void Stopwatch::reset() {
    m_running = false;
    m_laps.clear();
    m_startTime = Timestamp();
    m_lastLapTime = Timestamp();
}

void Stopwatch::lap() {
    if (m_running) {
        auto now = std::chrono::steady_clock::now();
        m_laps.push_back(std::chrono::duration_cast<Duration>(now - m_lastLapTime).count());
        m_lastLapTime = now;
    }
}

double Stopwatch::getElapsedTime() const {
    if (m_running) {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<Duration>(now - m_startTime).count();
    }
    if (m_laps.empty()) return 0.0;
    return getTotalLapTime();
}

double Stopwatch::getElapsedMilliseconds() const {
    return getElapsedTime() * 1000.0;
}

double Stopwatch::getElapsedMicroseconds() const {
    return getElapsedTime() * 1000000.0;
}

double Stopwatch::getLapTime(size_t index) const {
    if (index < m_laps.size()) {
        return m_laps[index];
    }
    return 0.0;
}

double Stopwatch::getLastLapTime() const {
    if (m_laps.empty()) return 0.0;
    return m_laps.back();
}

double Stopwatch::getAverageLapTime() const {
    if (m_laps.empty()) return 0.0;
    return getTotalLapTime() / static_cast<double>(m_laps.size());
}

double Stopwatch::getTotalLapTime() const {
    double total = 0.0;
    for (double lap : m_laps) {
        total += lap;
    }
    return total;
}

FrameCounter::FrameCounter()
    : m_sampleSize(60)
    , m_fps(0.0)
    , m_minFPS(std::numeric_limits<double>::max())
    , m_maxFPS(0.0)
    , m_frameTime(0.0)
    , m_minFrameTime(std::numeric_limits<double>::max())
    , m_maxFrameTime(0.0)
    , m_totalFrames(0)
    , m_totalTime(0.0)
{
    m_frameTimes.reserve(m_sampleSize);
}

FrameCounter::FrameCounter(size_t sampleSize)
    : m_sampleSize(sampleSize)
    , m_fps(0.0)
    , m_minFPS(std::numeric_limits<double>::max())
    , m_maxFPS(0.0)
    , m_frameTime(0.0)
    , m_minFrameTime(std::numeric_limits<double>::max())
    , m_maxFrameTime(0.0)
    , m_totalFrames(0)
    , m_totalTime(0.0)
{
    m_frameTimes.reserve(m_sampleSize);
}

void FrameCounter::update(double deltaTime) {
    m_frameTime = deltaTime;
    m_totalFrames++;
    m_totalTime += deltaTime;

    m_frameTimes.push_back(deltaTime);
    if (m_frameTimes.size() > m_sampleSize) {
        m_frameTimes.erase(m_frameTimes.begin());
    }

    if (deltaTime > 0.0) {
        m_fps = 1.0 / deltaTime;
        m_minFPS = std::min(m_minFPS, m_fps);
        m_maxFPS = std::max(m_maxFPS, m_fps);
    }

    m_minFrameTime = std::min(m_minFrameTime, deltaTime);
    m_maxFrameTime = std::max(m_maxFrameTime, deltaTime);
}

void FrameCounter::reset() {
    m_frameTimes.clear();
    m_fps = 0.0;
    m_minFPS = std::numeric_limits<double>::max();
    m_maxFPS = 0.0;
    m_frameTime = 0.0;
    m_minFrameTime = std::numeric_limits<double>::max();
    m_maxFrameTime = 0.0;
    m_totalFrames = 0;
    m_totalTime = 0.0;
}

void FrameCounter::setSampleSize(size_t size) {
    m_sampleSize = size;
    m_frameTimes.reserve(m_sampleSize);
    while (m_frameTimes.size() > m_sampleSize) {
        m_frameTimes.erase(m_frameTimes.begin());
    }
}

double FrameCounter::getAverageFPS() const {
    if (m_frameTimes.empty()) return 0.0;
    double avgFrameTime = 0.0;
    for (double ft : m_frameTimes) {
        avgFrameTime += ft;
    }
    avgFrameTime /= static_cast<double>(m_frameTimes.size());
    return avgFrameTime > 0.0 ? 1.0 / avgFrameTime : 0.0;
}

double FrameCounter::getAverageFrameTime() const {
    if (m_frameTimes.empty()) return 0.0;
    double avg = 0.0;
    for (double ft : m_frameTimes) {
        avg += ft;
    }
    return avg / static_cast<double>(m_frameTimes.size());
}

Date::Date()
    : m_year(2000)
    , m_month(1)
    , m_day(1)
{
}

Date::Date(int year, int month, int day)
    : m_year(year)
    , m_month(month)
    , m_day(day)
{
}

void Date::set(int year, int month, int day) {
    m_year = year;
    m_month = month;
    m_day = day;
}

bool Date::isValid() const {
    if (m_month < 1 || m_month > 12) return false;
    if (m_day < 1 || m_day > getDaysInMonth()) return false;
    return true;
}

int Date::getDayOfWeek() const {
    int y = m_year;
    int m = m_month;
    int d = m_day;

    if (m < 3) {
        m += 12;
        y--;
    }

    int k = y % 100;
    int j = y / 100;

    int h = (d + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    
    return (h + 5) % 7 + 1;
}

std::string Date::getDayOfWeekName() const {
    static const char* names[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
    };
    int dow = getDayOfWeek() - 1;
    if (dow >= 0 && dow < 7) {
        return names[dow];
    }
    return "";
}

int Date::getDayOfYear() const {
    int days = m_day;
    for (int m = 1; m < m_month; m++) {
        days += getDaysInMonth(m_year, m);
    }
    return days;
}

bool Date::isLeapYear() const {
    return isLeapYear(m_year);
}

bool Date::isLeapYear(int year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    return year % 400 == 0;
}

int Date::getDaysInMonth() const {
    return getDaysInMonth(m_year, m_month);
}

int Date::getDaysInMonth(int year, int month) {
    static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month < 1 || month > 12) return 0;
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

void Date::addDays(int days) {
    if (days == 0) return;

    if (days > 0) {
        while (days > 0) {
            int daysInMonth = getDaysInMonth();
            if (m_day + days <= daysInMonth) {
                m_day += days;
                days = 0;
            } else {
                days -= (daysInMonth - m_day + 1);
                m_day = 1;
                if (m_month == 12) {
                    m_month = 1;
                    m_year++;
                } else {
                    m_month++;
                }
            }
        }
    } else {
        days = -days;
        while (days > 0) {
            if (m_day > days) {
                m_day -= days;
                days = 0;
            } else {
                days -= m_day;
                if (m_month == 1) {
                    m_month = 12;
                    m_year--;
                } else {
                    m_month--;
                }
                m_day = getDaysInMonth();
            }
        }
    }
}

void Date::addMonths(int months) {
    if (months == 0) return;

    int totalMonths = m_year * 12 + m_month + months - 1;
    m_year = totalMonths / 12;
    m_month = (totalMonths % 12) + 1;

    int daysInMonth = getDaysInMonth();
    if (m_day > daysInMonth) {
        m_day = daysInMonth;
    }
}

void Date::addYears(int years) {
    m_year += years;
    if (!isLeapYear() && m_month == 2 && m_day == 29) {
        m_day = 28;
    }
}

std::string Date::toString(const std::string& format) const {
    std::ostringstream oss;
    for (size_t i = 0; i < format.size(); i++) {
        if (format[i] == '%' && i + 1 < format.size()) {
            switch (format[i + 1]) {
                case 'Y':
                    oss << std::setfill('0') << std::setw(4) << m_year;
                    break;
                case 'y':
                    oss << std::setfill('0') << std::setw(2) << (m_year % 100);
                    break;
                case 'm':
                    oss << std::setfill('0') << std::setw(2) << m_month;
                    break;
                case 'd':
                    oss << std::setfill('0') << std::setw(2) << m_day;
                    break;
                default:
                    oss << format[i] << format[i + 1];
                    break;
            }
            i++;
        } else {
            oss << format[i];
        }
    }
    return oss.str();
}

Date Date::today() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif
    return Date(localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday);
}

Date Date::fromString(const std::string& str, const std::string& format) {
    Date date;
    int year = 2000, month = 1, day = 1;
    
    size_t i = 0, j = 0;
    while (i < format.size() && j < str.size()) {
        if (format[i] == '%' && i + 1 < format.size()) {
            switch (format[i + 1]) {
                case 'Y':
                    year = std::stoi(str.substr(j, 4));
                    j += 4;
                    break;
                case 'y':
                    year = 2000 + std::stoi(str.substr(j, 2));
                    j += 2;
                    break;
                case 'm':
                    month = std::stoi(str.substr(j, 2));
                    j += 2;
                    break;
                case 'd':
                    day = std::stoi(str.substr(j, 2));
                    j += 2;
                    break;
            }
            i += 2;
        } else {
            i++;
            j++;
        }
    }
    
    date.set(year, month, day);
    return date;
}

bool Date::operator==(const Date& other) const {
    return m_year == other.m_year && m_month == other.m_month && m_day == other.m_day;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

bool Date::operator<(const Date& other) const {
    if (m_year != other.m_year) return m_year < other.m_year;
    if (m_month != other.m_month) return m_month < other.m_month;
    return m_day < other.m_day;
}

bool Date::operator<=(const Date& other) const {
    return *this < other || *this == other;
}

bool Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const {
    return !(*this < other);
}

Date Date::operator+(int days) const {
    Date result = *this;
    result.addDays(days);
    return result;
}

Date Date::operator-(int days) const {
    Date result = *this;
    result.addDays(-days);
    return result;
}

Date& Date::operator+=(int days) {
    addDays(days);
    return *this;
}

Date& Date::operator-=(int days) {
    addDays(-days);
    return *this;
}

int64_t Date::operator-(const Date& other) const {
    Date earlier = *this < other ? *this : other;
    Date later = *this < other ? other : *this;
    
    int64_t days = 0;
    while (earlier < later) {
        earlier.addDays(1);
        days++;
    }
    
    return *this < other ? -days : days;
}

TimeOfDay::TimeOfDay()
    : m_hours(0)
    , m_minutes(0)
    , m_seconds(0)
    , m_milliseconds(0)
{
}

TimeOfDay::TimeOfDay(int hours, int minutes, int seconds, int milliseconds)
    : m_hours(hours)
    , m_minutes(minutes)
    , m_seconds(seconds)
    , m_milliseconds(milliseconds)
{
}

void TimeOfDay::set(int hours, int minutes, int seconds, int milliseconds) {
    m_hours = hours;
    m_minutes = minutes;
    m_seconds = seconds;
    m_milliseconds = milliseconds;
}

bool TimeOfDay::isValid() const {
    if (m_hours < 0 || m_hours >= 24) return false;
    if (m_minutes < 0 || m_minutes >= 60) return false;
    if (m_seconds < 0 || m_seconds >= 60) return false;
    if (m_milliseconds < 0 || m_milliseconds >= 1000) return false;
    return true;
}

double TimeOfDay::getTotalHours() const {
    return m_hours + m_minutes / 60.0 + m_seconds / 3600.0 + m_milliseconds / 3600000.0;
}

double TimeOfDay::getTotalMinutes() const {
    return m_hours * 60.0 + m_minutes + m_seconds / 60.0 + m_milliseconds / 60000.0;
}

double TimeOfDay::getTotalSeconds() const {
    return m_hours * 3600.0 + m_minutes * 60.0 + m_seconds + m_milliseconds / 1000.0;
}

int64_t TimeOfDay::getTotalMilliseconds() const {
    return m_hours * 3600000LL + m_minutes * 60000LL + m_seconds * 1000LL + m_milliseconds;
}

void TimeOfDay::addHours(int hours) {
    m_hours += hours;
    while (m_hours < 0) m_hours += 24;
    m_hours %= 24;
}

void TimeOfDay::addMinutes(int minutes) {
    int totalMinutes = m_hours * 60 + m_minutes + minutes;
    while (totalMinutes < 0) totalMinutes += 1440;
    totalMinutes %= 1440;
    m_hours = totalMinutes / 60;
    m_minutes = totalMinutes % 60;
}

void TimeOfDay::addSeconds(int seconds) {
    int64_t totalSeconds = m_hours * 3600LL + m_minutes * 60LL + m_seconds + seconds;
    while (totalSeconds < 0) totalSeconds += 86400LL;
    totalSeconds %= 86400LL;
    m_hours = static_cast<int>(totalSeconds / 3600LL);
    m_minutes = static_cast<int>((totalSeconds % 3600LL) / 60LL);
    m_seconds = static_cast<int>(totalSeconds % 60LL);
}

void TimeOfDay::addMilliseconds(int milliseconds) {
    int64_t total = getTotalMilliseconds() + milliseconds;
    const int64_t dayMs = 86400000LL;
    while (total < 0) total += dayMs;
    total %= dayMs;
    m_hours = static_cast<int>(total / 3600000LL);
    m_minutes = static_cast<int>((total % 3600000LL) / 60000LL);
    m_seconds = static_cast<int>((total % 60000LL) / 1000LL);
    m_milliseconds = static_cast<int>(total % 1000LL);
}

std::string TimeOfDay::toString(const std::string& format) const {
    std::ostringstream oss;
    for (size_t i = 0; i < format.size(); i++) {
        if (format[i] == '%' && i + 1 < format.size()) {
            switch (format[i + 1]) {
                case 'H':
                    oss << std::setfill('0') << std::setw(2) << m_hours;
                    break;
                case 'M':
                    oss << std::setfill('0') << std::setw(2) << m_minutes;
                    break;
                case 'S':
                    oss << std::setfill('0') << std::setw(2) << m_seconds;
                    break;
                case 's':
                    oss << std::setfill('0') << std::setw(3) << m_milliseconds;
                    break;
                default:
                    oss << format[i] << format[i + 1];
                    break;
            }
            i++;
        } else {
            oss << format[i];
        }
    }
    return oss.str();
}

TimeOfDay TimeOfDay::now() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif
    auto duration = now - std::chrono::system_clock::from_time_t(time);
    int ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    return TimeOfDay(localTime.tm_hour, localTime.tm_min, localTime.tm_sec, ms);
}

TimeOfDay TimeOfDay::fromString(const std::string& str, const std::string& format) {
    TimeOfDay time;
    int hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
    
    size_t i = 0, j = 0;
    while (i < format.size() && j < str.size()) {
        if (format[i] == '%' && i + 1 < format.size()) {
            switch (format[i + 1]) {
                case 'H':
                    hours = std::stoi(str.substr(j, 2));
                    j += 2;
                    break;
                case 'M':
                    minutes = std::stoi(str.substr(j, 2));
                    j += 2;
                    break;
                case 'S':
                    seconds = std::stoi(str.substr(j, 2));
                    j += 2;
                    break;
                case 's':
                    milliseconds = std::stoi(str.substr(j, 3));
                    j += 3;
                    break;
            }
            i += 2;
        } else {
            i++;
            j++;
        }
    }
    
    time.set(hours, minutes, seconds, milliseconds);
    return time;
}

bool TimeOfDay::operator==(const TimeOfDay& other) const {
    return m_hours == other.m_hours && m_minutes == other.m_minutes &&
           m_seconds == other.m_seconds && m_milliseconds == other.m_milliseconds;
}

bool TimeOfDay::operator!=(const TimeOfDay& other) const {
    return !(*this == other);
}

bool TimeOfDay::operator<(const TimeOfDay& other) const {
    if (m_hours != other.m_hours) return m_hours < other.m_hours;
    if (m_minutes != other.m_minutes) return m_minutes < other.m_minutes;
    if (m_seconds != other.m_seconds) return m_seconds < other.m_seconds;
    return m_milliseconds < other.m_milliseconds;
}

bool TimeOfDay::operator<=(const TimeOfDay& other) const {
    return *this < other || *this == other;
}

bool TimeOfDay::operator>(const TimeOfDay& other) const {
    return !(*this <= other);
}

bool TimeOfDay::operator>=(const TimeOfDay& other) const {
    return !(*this < other);
}

DateTime::DateTime()
    : m_date()
    , m_time()
{
}

DateTime::DateTime(const Date& date, const TimeOfDay& time)
    : m_date(date)
    , m_time(time)
{
}

DateTime::DateTime(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds)
    : m_date(year, month, day)
    , m_time(hours, minutes, seconds, milliseconds)
{
}

std::string DateTime::toString(const std::string& format) const {
    std::string result;
    for (size_t i = 0; i < format.size(); i++) {
        if (format[i] == '%' && i + 1 < format.size()) {
            switch (format[i + 1]) {
                case 'Y':
                case 'y':
                case 'm':
                case 'd': {
                    std::string dateFormat = std::string("%") + format[i + 1];
                    result += m_date.toString(dateFormat);
                    break;
                }
                case 'H':
                case 'M':
                case 'S':
                case 's': {
                    std::string timeFormat = std::string("%") + format[i + 1];
                    result += m_time.toString(timeFormat);
                    break;
                }
                default:
                    result += format[i];
                    result += format[i + 1];
                    break;
            }
            i++;
        } else {
            result += format[i];
        }
    }
    return result;
}

DateTime DateTime::now() {
    return DateTime(Date::today(), TimeOfDay::now());
}

DateTime DateTime::fromString(const std::string& str, const std::string& format) {
    return DateTime(Date::fromString(str, format), TimeOfDay::fromString(str, format));
}

DateTime DateTime::fromTimestamp(int64_t timestamp) {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(timestamp)
    );
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif
    auto duration = tp - std::chrono::system_clock::from_time_t(time);
    int ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    return DateTime(
        localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec, ms
    );
}

int64_t DateTime::toTimestamp(const DateTime& dt) {
    std::tm tm = {};
    tm.tm_year = dt.getYear() - 1900;
    tm.tm_mon = dt.getMonth() - 1;
    tm.tm_mday = dt.getDay();
    tm.tm_hour = dt.getHours();
    tm.tm_min = dt.getMinutes();
    tm.tm_sec = dt.getSeconds();
    
    std::time_t time;
#ifdef _WIN32
    time = _mkgmtime(&tm);
#else
    time = timegm(&tm);
#endif
    
    auto tp = std::chrono::system_clock::from_time_t(time);
    tp += std::chrono::milliseconds(dt.getMilliseconds());
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()
    ).count();
}

bool DateTime::operator==(const DateTime& other) const {
    return m_date == other.m_date && m_time == other.m_time;
}

bool DateTime::operator!=(const DateTime& other) const {
    return !(*this == other);
}

bool DateTime::operator<(const DateTime& other) const {
    if (m_date != other.m_date) return m_date < other.m_date;
    return m_time < other.m_time;
}

bool DateTime::operator<=(const DateTime& other) const {
    return *this < other || *this == other;
}

bool DateTime::operator>(const DateTime& other) const {
    return !(*this <= other);
}

bool DateTime::operator>=(const DateTime& other) const {
    return !(*this < other);
}

Duration DateTime::operator-(const DateTime& other) const {
    int64_t days = m_date - other.m_date;
    int64_t ms1 = m_time.getTotalMilliseconds();
    int64_t ms2 = other.m_time.getTotalMilliseconds();
    double totalSeconds = days * 86400.0 + (ms1 - ms2) / 1000.0;
    return Duration(totalSeconds);
}

Cooldown::Cooldown()
    : m_duration(1.0)
    , m_remaining(0.0)
    , m_active(false)
{
}

Cooldown::Cooldown(double duration)
    : m_duration(duration)
    , m_remaining(0.0)
    , m_active(false)
{
}

void Cooldown::start() {
    m_remaining = m_duration;
    m_active = true;
}

void Cooldown::start(double duration) {
    m_duration = duration;
    start();
}

void Cooldown::reset() {
    m_remaining = 0.0;
    m_active = false;
}

bool Cooldown::isReady() const {
    return !m_active || m_remaining <= 0.0;
}

double Cooldown::getRemainingTime() const {
    return std::max(0.0, m_remaining);
}

double Cooldown::getElapsedTime() const {
    return std::max(0.0, m_duration - m_remaining);
}

double Cooldown::getProgress() const {
    if (m_duration <= 0.0) return 1.0;
    return 1.0 - (m_remaining / m_duration);
}

void Cooldown::update(double deltaTime) {
    if (m_active && m_remaining > 0.0) {
        m_remaining -= deltaTime;
        if (m_remaining <= 0.0) {
            m_remaining = 0.0;
            m_active = false;
        }
    }
}

Tween::Tween()
    : m_from(0.0)
    , m_to(1.0)
    , m_duration(1.0)
    , m_progress(0.0)
    , m_value(0.0)
    , m_easing(Easing::Linear)
    , m_running(false)
    , m_looping(false)
    , m_yoyo(false)
    , m_reversed(false)
{
}

Tween::Tween(double duration, Easing easing)
    : m_from(0.0)
    , m_to(1.0)
    , m_duration(duration)
    , m_progress(0.0)
    , m_value(0.0)
    , m_easing(easing)
    , m_running(false)
    , m_looping(false)
    , m_yoyo(false)
    , m_reversed(false)
{
}

void Tween::start() {
    m_running = true;
    m_progress = 0.0;
    m_value = m_from;
}

void Tween::start(double from, double to) {
    m_from = from;
    m_to = to;
    start();
}

void Tween::start(double from, double to, double duration) {
    m_from = from;
    m_to = to;
    m_duration = duration;
    start();
}

void Tween::stop() {
    m_running = false;
}

void Tween::reset() {
    m_progress = 0.0;
    m_value = m_from;
    m_reversed = false;
}

bool Tween::isFinished() const {
    return m_progress >= 1.0 && !m_looping && !m_yoyo;
}

void Tween::update(double deltaTime) {
    if (!m_running) return;

    double direction = m_reversed ? -1.0 : 1.0;
    m_progress += (deltaTime / m_duration) * direction;

    if (m_progress >= 1.0) {
        if (m_yoyo) {
            m_progress = 1.0;
            m_reversed = true;
        } else if (m_looping) {
            m_progress = 0.0;
        } else {
            m_progress = 1.0;
            m_running = false;
        }
    } else if (m_progress <= 0.0) {
        if (m_yoyo && m_reversed) {
            m_progress = 0.0;
            m_reversed = false;
            if (!m_looping) {
                m_running = false;
            }
        } else if (m_looping) {
            m_progress = 1.0;
        } else {
            m_progress = 0.0;
            m_running = false;
        }
    }

    double t = std::max(0.0, std::min(1.0, m_progress));
    double eased = applyEasing(t, m_easing);
    m_value = m_from + (m_to - m_from) * eased;

    if (m_updateCallback) {
        m_updateCallback(m_value);
    }

    if (!m_running && m_callback) {
        m_callback();
    }
}

double Tween::applyEasing(double t, Easing easing) {
    t = std::max(0.0, std::min(1.0, t));
    
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;
    const double c3 = c1 + 1;
    const double c4 = (2 * 3.1415926535) / 3;
    const double c5 = (2 * 3.1415926535) / 4.5;
    const double n1 = 7.5625;
    const double d1 = 2.75;

    switch (easing) {
        case Easing::Linear:
            return t;

        case Easing::EaseIn:
            return t * t;

        case Easing::EaseOut:
            return t * (2 - t);

        case Easing::EaseInOut:
            return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;

        case Easing::EaseInQuad:
            return t * t;

        case Easing::EaseOutQuad:
            return t * (2 - t);

        case Easing::EaseInOutQuad:
            return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;

        case Easing::EaseInCubic:
            return t * t * t;

        case Easing::EaseOutCubic:
            return (--t) * t * t + 1;

        case Easing::EaseInOutCubic:
            return t < 0.5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;

        case Easing::EaseInQuart:
            return t * t * t * t;

        case Easing::EaseOutQuart:
            return 1 - (--t) * t * t * t;

        case Easing::EaseInOutQuart:
            return t < 0.5 ? 8 * t * t * t * t : 1 - 8 * (--t) * t * t * t;

        case Easing::EaseInQuint:
            return t * t * t * t * t;

        case Easing::EaseOutQuint:
            return 1 + (--t) * t * t * t * t;

        case Easing::EaseInOutQuint:
            return t < 0.5 ? 16 * t * t * t * t * t : 1 + 16 * (--t) * t * t * t * t;

        case Easing::EaseInSine:
            return 1 - std::cos(t * 3.1415926535 / 2);

        case Easing::EaseOutSine:
            return std::sin(t * 3.1415926535 / 2);

        case Easing::EaseInOutSine:
            return -(std::cos(3.1415926535 * t) - 1) / 2;

        case Easing::EaseInExpo:
            return t == 0 ? 0 : std::pow(2, 10 * (t - 1));

        case Easing::EaseOutExpo:
            return t == 1 ? 1 : 1 - std::pow(2, -10 * t);

        case Easing::EaseInOutExpo:
            if (t == 0) return 0;
            if (t == 1) return 1;
            if (t < 0.5) return std::pow(2, 20 * t - 10) / 2;
            return (2 - std::pow(2, -20 * t + 10)) / 2;

        case Easing::EaseInCirc:
            return 1 - std::sqrt(1 - t * t);

        case Easing::EaseOutCirc:
            return std::sqrt(1 - (--t) * t);

        case Easing::EaseInOutCirc:
            return t < 0.5 ? (1 - std::sqrt(1 - 4 * t * t)) / 2 : (std::sqrt(1 - (--t) * t * 4) + 1) / 2;

        case Easing::EaseInBack:
            return c3 * t * t * t - c1 * t * t;

        case Easing::EaseOutBack:
            return 1 + c3 * (--t) * t * t + c1 * t * t;

        case Easing::EaseInOutBack:
            return t < 0.5 ? (std::pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2 :
                   (std::pow(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;

        case Easing::EaseInElastic:
            if (t == 0) return 0;
            if (t == 1) return 1;
            return -std::pow(2, 10 * t - 10) * std::sin((t * 10 - 10.75) * c4);

        case Easing::EaseOutElastic:
            if (t == 0) return 0;
            if (t == 1) return 1;
            return std::pow(2, -10 * t) * std::sin((t * 10 - 0.75) * c4) + 1;

        case Easing::EaseInOutElastic:
            if (t == 0) return 0;
            if (t == 1) return 1;
            if (t < 0.5) return -(std::pow(2, 20 * t - 10) * std::sin((20 * t - 11.125) * c5)) / 2;
            return (std::pow(2, -20 * t + 10) * std::sin((20 * t - 11.125) * c5)) / 2 + 1;

        case Easing::EaseInBounce:
            return 1 - applyEasing(1 - t, Easing::EaseOutBounce);

        case Easing::EaseOutBounce: {
            if (t < (1 / d1)) {
                return n1 * t * t;
            } else if (t < (2 / d1)) {
                t -= (1.5 / d1);
                return n1 * t * t + 0.75;
            } else if (t < (2.5 / d1)) {
                t -= (2.25 / d1);
                return n1 * t * t + 0.9375;
            } else {
                t -= (2.625 / d1);
                return n1 * t * t + 0.984375;
            }
        }

        case Easing::EaseInOutBounce:
            return t < 0.5 ? (1 - applyEasing(1 - 2 * t, Easing::EaseOutBounce)) / 2 :
                   (1 + applyEasing(2 * t - 1, Easing::EaseOutBounce)) / 2;

        default:
            return t;
    }
}

Scheduler::Scheduler()
    : m_nextId(1)
{
}

Scheduler::~Scheduler() {
}

Scheduler::TaskId Scheduler::schedule(std::function<void()> task, double delay) {
    Task t;
    t.id = m_nextId++;
    t.callback = std::move(task);
    t.delay = delay;
    t.interval = 0.0;
    t.elapsed = 0.0;
    t.repeating = false;
    t.active = true;
    m_tasks.push_back(t);
    return t.id;
}

Scheduler::TaskId Scheduler::scheduleRepeating(std::function<void()> task, double interval, double initialDelay) {
    Task t;
    t.id = m_nextId++;
    t.callback = std::move(task);
    t.delay = initialDelay;
    t.interval = interval;
    t.elapsed = 0.0;
    t.repeating = true;
    t.active = true;
    m_tasks.push_back(t);
    return t.id;
}

void Scheduler::cancel(TaskId id) {
    for (auto& task : m_tasks) {
        if (task.id == id) {
            task.active = false;
        }
    }
}

void Scheduler::cancelAll() {
    m_tasks.clear();
}

bool Scheduler::isScheduled(TaskId id) const {
    for (const auto& task : m_tasks) {
        if (task.id == id && task.active) {
            return true;
        }
    }
    return false;
}

void Scheduler::update(double deltaTime) {
    std::vector<TaskId> toRemove;

    for (auto& task : m_tasks) {
        if (!task.active) continue;

        task.elapsed += deltaTime;

        if (task.elapsed >= task.delay) {
            task.callback();

            if (task.repeating) {
                task.elapsed -= task.delay;
                task.delay = task.interval;
                if (task.elapsed < 0) task.elapsed = 0;
            } else {
                task.active = false;
                toRemove.push_back(task.id);
            }
        }
    }

    m_tasks.erase(
        std::remove_if(m_tasks.begin(), m_tasks.end(),
            [](const Task& t) { return !t.active; }),
        m_tasks.end()
    );
}

}
