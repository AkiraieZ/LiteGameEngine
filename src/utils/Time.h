#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <vector>
#include <functional>

namespace LGE {

using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;
using Duration = std::chrono::duration<double>;
using Milliseconds = std::chrono::duration<double, std::milli>;
using Microseconds = std::chrono::duration<double, std::micro>;
using Nanoseconds = std::chrono::duration<double, std::nano>;

class Clock {
public:
    Clock();
    ~Clock() = default;

    void reset();
    void restart();
    void pause();
    void resume();
    void setPaused(bool paused);

    double getElapsedTime() const;
    double getElapsedMilliseconds() const;
    double getElapsedMicroseconds() const;
    double getElapsedNanoseconds() const;

    bool isPaused() const { return m_paused; }

    void setTimeScale(double scale) { m_timeScale = scale; }
    double getTimeScale() const { return m_timeScale; }

    Timestamp getStartTime() const { return m_startTime; }
    Timestamp getLastUpdateTime() const { return m_lastUpdateTime; }

    void update();

private:
    Timestamp m_startTime;
    Timestamp m_lastUpdateTime;
    Timestamp m_pauseStartTime;
    Duration m_accumulatedTime;
    double m_timeScale = 1.0;
    bool m_paused = false;
};

class Timer {
public:
    Timer();
    explicit Timer(double duration);
    ~Timer() = default;

    void start();
    void start(double duration);
    void stop();
    void reset();
    void restart();

    void setDuration(double duration) { m_duration = duration; }
    double getDuration() const { return m_duration; }

    void setLooping(bool loop) { m_looping = loop; }
    bool isLooping() const { return m_looping; }

    double getRemainingTime() const;
    double getElapsedTime() const;
    double getProgress() const;

    bool isRunning() const { return m_running; }
    bool isFinished() const;

    void setCallback(std::function<void()> callback) { m_callback = callback; }
    void clearCallback() { m_callback = nullptr; }

    void update(double deltaTime);

private:
    double m_duration = 1.0;
    double m_elapsed = 0.0;
    bool m_running = false;
    bool m_looping = false;
    std::function<void()> m_callback;
};

class Stopwatch {
public:
    Stopwatch();
    ~Stopwatch() = default;

    void start();
    void stop();
    void reset();
    void lap();

    bool isRunning() const { return m_running; }

    double getElapsedTime() const;
    double getElapsedMilliseconds() const;
    double getElapsedMicroseconds() const;

    size_t getLapCount() const { return m_laps.size(); }
    double getLapTime(size_t index) const;
    double getLastLapTime() const;
    double getAverageLapTime() const;
    double getTotalLapTime() const;

    const std::vector<double>& getLaps() const { return m_laps; }

private:
    Timestamp m_startTime;
    Timestamp m_lastLapTime;
    std::vector<double> m_laps;
    bool m_running = false;
};

class FrameCounter {
public:
    FrameCounter();
    explicit FrameCounter(size_t sampleSize);
    ~FrameCounter() = default;

    void update(double deltaTime);
    void reset();

    void setSampleSize(size_t size);
    size_t getSampleSize() const { return m_sampleSize; }

    double getFPS() const { return m_fps; }
    double getAverageFPS() const;
    double getMinFPS() const { return m_minFPS; }
    double getMaxFPS() const { return m_maxFPS; }

    double getFrameTime() const { return m_frameTime; }
    double getAverageFrameTime() const;
    double getMinFrameTime() const { return m_minFrameTime; }
    double getMaxFrameTime() const { return m_maxFrameTime; }

    uint64_t getTotalFrames() const { return m_totalFrames; }
    double getTotalTime() const { return m_totalTime; }

    const std::vector<double>& getFrameTimes() const { return m_frameTimes; }

private:
    std::vector<double> m_frameTimes;
    size_t m_sampleSize = 60;
    double m_fps = 0.0;
    double m_minFPS = std::numeric_limits<double>::max();
    double m_maxFPS = 0.0;
    double m_frameTime = 0.0;
    double m_minFrameTime = std::numeric_limits<double>::max();
    double m_maxFrameTime = 0.0;
    uint64_t m_totalFrames = 0;
    double m_totalTime = 0.0;
};

class Date {
public:
    Date();
    Date(int year, int month, int day);
    ~Date() = default;

    void setYear(int year) { m_year = year; }
    int getYear() const { return m_year; }

    void setMonth(int month) { m_month = month; }
    int getMonth() const { return m_month; }

    void setDay(int day) { m_day = day; }
    int getDay() const { return m_day; }

    void set(int year, int month, int day);

    bool isValid() const;

    int getDayOfWeek() const;
    std::string getDayOfWeekName() const;

    int getDayOfYear() const;

    bool isLeapYear() const;
    static bool isLeapYear(int year);

    int getDaysInMonth() const;
    static int getDaysInMonth(int year, int month);

    void addDays(int days);
    void addMonths(int months);
    void addYears(int years);

    std::string toString(const std::string& format = "%Y-%m-%d") const;

    static Date today();
    static Date fromString(const std::string& str, const std::string& format = "%Y-%m-%d");

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;

    Date operator+(int days) const;
    Date operator-(int days) const;
    Date& operator+=(int days);
    Date& operator-=(int days);

    int64_t operator-(const Date& other) const;

private:
    int m_year = 2000;
    int m_month = 1;
    int m_day = 1;
};

class TimeOfDay {
public:
    TimeOfDay();
    TimeOfDay(int hours, int minutes, int seconds = 0, int milliseconds = 0);
    ~TimeOfDay() = default;

    void setHours(int hours) { m_hours = hours; }
    int getHours() const { return m_hours; }

    void setMinutes(int minutes) { m_minutes = minutes; }
    int getMinutes() const { return m_minutes; }

    void setSeconds(int seconds) { m_seconds = seconds; }
    int getSeconds() const { return m_seconds; }

    void setMilliseconds(int milliseconds) { m_milliseconds = milliseconds; }
    int getMilliseconds() const { return m_milliseconds; }

    void set(int hours, int minutes, int seconds = 0, int milliseconds = 0);

    bool isValid() const;

    double getTotalHours() const;
    double getTotalMinutes() const;
    double getTotalSeconds() const;
    int64_t getTotalMilliseconds() const;

    void addHours(int hours);
    void addMinutes(int minutes);
    void addSeconds(int seconds);
    void addMilliseconds(int milliseconds);

    std::string toString(const std::string& format = "%H:%M:%S") const;

    static TimeOfDay now();
    static TimeOfDay fromString(const std::string& str, const std::string& format = "%H:%M:%S");

    bool operator==(const TimeOfDay& other) const;
    bool operator!=(const TimeOfDay& other) const;
    bool operator<(const TimeOfDay& other) const;
    bool operator<=(const TimeOfDay& other) const;
    bool operator>(const TimeOfDay& other) const;
    bool operator>=(const TimeOfDay& other) const;

private:
    int m_hours = 0;
    int m_minutes = 0;
    int m_seconds = 0;
    int m_milliseconds = 0;
};

class DateTime {
public:
    DateTime();
    DateTime(const Date& date, const TimeOfDay& time);
    DateTime(int year, int month, int day, int hours = 0, int minutes = 0, int seconds = 0, int milliseconds = 0);
    ~DateTime() = default;

    void setDate(const Date& date) { m_date = date; }
    const Date& getDate() const { return m_date; }

    void setTime(const TimeOfDay& time) { m_time = time; }
    const TimeOfDay& getTime() const { return m_time; }

    void setYear(int year) { m_date.setYear(year); }
    int getYear() const { return m_date.getYear(); }

    void setMonth(int month) { m_date.setMonth(month); }
    int getMonth() const { return m_date.getMonth(); }

    void setDay(int day) { m_date.setDay(day); }
    int getDay() const { return m_date.getDay(); }

    void setHours(int hours) { m_time.setHours(hours); }
    int getHours() const { return m_time.getHours(); }

    void setMinutes(int minutes) { m_time.setMinutes(minutes); }
    int getMinutes() const { return m_time.getMinutes(); }

    void setSeconds(int seconds) { m_time.setSeconds(seconds); }
    int getSeconds() const { return m_time.getSeconds(); }

    void setMilliseconds(int milliseconds) { m_time.setMilliseconds(milliseconds); }
    int getMilliseconds() const { return m_time.getMilliseconds(); }

    bool isValid() const { return m_date.isValid() && m_time.isValid(); }

    std::string toString(const std::string& format = "%Y-%m-%d %H:%M:%S") const;

    static DateTime now();
    static DateTime fromString(const std::string& str, const std::string& format = "%Y-%m-%d %H:%M:%S");
    static DateTime fromTimestamp(int64_t timestamp);
    static int64_t toTimestamp(const DateTime& dt);

    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;

    Duration operator-(const DateTime& other) const;

private:
    Date m_date;
    TimeOfDay m_time;
};

class Cooldown {
public:
    Cooldown();
    explicit Cooldown(double duration);
    ~Cooldown() = default;

    void start();
    void start(double duration);
    void reset();

    void setDuration(double duration) { m_duration = duration; }
    double getDuration() const { return m_duration; }

    bool isReady() const;
    bool isOnCooldown() const { return !isReady(); }

    double getRemainingTime() const;
    double getElapsedTime() const;
    double getProgress() const;

    void update(double deltaTime);

private:
    double m_duration = 1.0;
    double m_remaining = 0.0;
    bool m_active = false;
};

class Tween {
public:
    enum class Easing {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        EaseInQuad,
        EaseOutQuad,
        EaseInOutQuad,
        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
        EaseInQuart,
        EaseOutQuart,
        EaseInOutQuart,
        EaseInQuint,
        EaseOutQuint,
        EaseInOutQuint,
        EaseInSine,
        EaseOutSine,
        EaseInOutSine,
        EaseInExpo,
        EaseOutExpo,
        EaseInOutExpo,
        EaseInCirc,
        EaseOutCirc,
        EaseInOutCirc,
        EaseInBack,
        EaseOutBack,
        EaseInOutBack,
        EaseInElastic,
        EaseOutElastic,
        EaseInOutElastic,
        EaseInBounce,
        EaseOutBounce,
        EaseInOutBounce
    };

    Tween();
    Tween(double duration, Easing easing = Easing::Linear);
    ~Tween() = default;

    void start();
    void start(double from, double to);
    void start(double from, double to, double duration);
    void stop();
    void reset();

    void setFrom(double from) { m_from = from; }
    double getFrom() const { return m_from; }

    void setTo(double to) { m_to = to; }
    double getTo() const { return m_to; }

    void setDuration(double duration) { m_duration = duration; }
    double getDuration() const { return m_duration; }

    void setEasing(Easing easing) { m_easing = easing; }
    Easing getEasing() const { return m_easing; }

    void setLooping(bool loop) { m_looping = loop; }
    bool isLooping() const { return m_looping; }

    void setYoyo(bool yoyo) { m_yoyo = yoyo; }
    bool isYoyo() const { return m_yoyo; }

    double getValue() const { return m_value; }
    double getProgress() const { return m_progress; }

    bool isRunning() const { return m_running; }
    bool isFinished() const;

    void setCallback(std::function<void()> callback) { m_callback = callback; }
    void setUpdateCallback(std::function<void(double)> callback) { m_updateCallback = callback; }

    void update(double deltaTime);

    static double applyEasing(double t, Easing easing);

private:
    double m_from = 0.0;
    double m_to = 1.0;
    double m_duration = 1.0;
    double m_progress = 0.0;
    double m_value = 0.0;
    Easing m_easing = Easing::Linear;
    bool m_running = false;
    bool m_looping = false;
    bool m_yoyo = false;
    bool m_reversed = false;
    std::function<void()> m_callback;
    std::function<void(double)> m_updateCallback;
};

class Scheduler {
public:
    Scheduler();
    ~Scheduler();

    using TaskId = uint64_t;

    TaskId schedule(std::function<void()> task, double delay = 0.0);
    TaskId scheduleRepeating(std::function<void()> task, double interval, double initialDelay = 0.0);

    void cancel(TaskId id);
    void cancelAll();

    bool isScheduled(TaskId id) const;

    void update(double deltaTime);

    size_t getTaskCount() const { return m_tasks.size(); }

private:
    struct Task {
        TaskId id;
        std::function<void()> callback;
        double delay;
        double interval;
        double elapsed;
        bool repeating;
        bool active;
    };

    std::vector<Task> m_tasks;
    TaskId m_nextId = 1;
};

}
