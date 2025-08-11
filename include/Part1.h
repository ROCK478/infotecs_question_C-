#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

#ifdef _WIN32
    #ifdef LOGGER_EXPORTS
        #define LOGGER_API __declspec(dllexport)
    #else
        #define LOGGER_API __declspec(dllimport)
    #endif
#else
    #define LOGGER_API __attribute__((visibility("default")))
#endif

using namespace std;

enum class LogLevel {
    Low = 1,
    Middle = 2,
    High = 3
};

class LOGGER_API ILogger { // Интерфейс для логирования
public:
    virtual ~ILogger() = default;
    virtual void Log(const string& message, LogLevel level) = 0;
    virtual void Log(const string& message, const string& level) = 0;
    virtual void Log(const string& message, int level) = 0;
    virtual bool setLogLevel(LogLevel newLevel) = 0;
    virtual bool setLogLevel(const string& newLevel) = 0;
};

class LOGGER_API Logger : public ILogger { // Сам Логгер
public:
    Logger(const string& filename, LogLevel level = LogLevel::Middle, bool asyncMode = true);
    ~Logger();

    void Log(const string& message, LogLevel level) override;
    void Log(const string& message, const string& level) override;
    void Log(const string& message, int level) override;
    bool setLogLevel(LogLevel newLevel) override;
    bool setLogLevel(const string& newLevel) override;

private:
    void processQueue();
    LogLevel parseLogLevel(const string& levelStr);
    LogLevel parseLogLevel(int level);
    string getCurrentTime();
    bool tryWriteToFile(const string& message, LogLevel messageLevel);

    struct LogTask {
        string message;
        LogLevel level;
    };
    string logFilename;
    LogLevel logLevel;
    queue<LogTask> logQueue;
    mutex mtx;
    condition_variable cv;
    atomic<bool> running;
    thread worker;
};