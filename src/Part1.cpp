#include "Part1.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <system_error>

Logger::Logger(const string& filename, LogLevel level, bool asyncMode) //Конструктор
    : logFilename(filename), logLevel(level), running(true) {
    if (asyncMode) {
        worker = thread(&Logger::processQueue, this);
    }
}

Logger::~Logger() { //Деструктор
    running = false;
    cv.notify_all();
    if (worker.joinable()) {
        worker.join();
    }
}

string Logger::getCurrentTime() { //Получение текущего времени
    auto now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);
    tm tm = *localtime(&time);

    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool Logger::tryWriteToFile(const string& message, LogLevel messageLevel) {
    try {
        ofstream outFile(logFilename, ios::app);
        if (!outFile.is_open()) {
            return false;
        }

        string levelStr;
        switch (messageLevel) {
        case LogLevel::Low:    levelStr = "LOW";    break;
        case LogLevel::Middle: levelStr = "MIDDLE"; break;
        case LogLevel::High:   levelStr = "HIGH";   break;
        }

        outFile << "[" << getCurrentTime() << "] "
            << "[" << levelStr << "] "
            << message << endl;

        return true;
    }
    catch (const exception& e) {
        cerr << "Ошибка записи в файл: " << e.what() << endl;
        return false;
    }
}

void Logger::processQueue() {
    while (running || !logQueue.empty()) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]() { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            LogTask task = logQueue.front();
            logQueue.pop();
            lock.unlock();

            if (!tryWriteToFile(task.message, task.level)) {
                cerr << "Следующее сообщение не записано: " << task.message << endl;
            }

            lock.lock();
        }
    }
}

LogLevel Logger::parseLogLevel(const string& levelStr) {
    string lowerStr = levelStr;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

    if (lowerStr == "low" || lowerStr == "1") return LogLevel::Low;
    if (lowerStr == "middle" || lowerStr == "2") return LogLevel::Middle;
    if (lowerStr == "high" || lowerStr == "3") return LogLevel::High;

    return LogLevel::Middle;
}

LogLevel Logger::parseLogLevel(int level) {
    switch (level) {
    case 1: return LogLevel::Low;
    case 2: return LogLevel::Middle;
    case 3: return LogLevel::High;
    default: return LogLevel::Middle; // Значение по умолчанию
    }
}

bool Logger::setLogLevel(LogLevel newLevel) {
    logLevel = newLevel;
    return true;
}

bool Logger::setLogLevel(const string& newLevel) {
    LogLevel parsedLevel = parseLogLevel(newLevel);
    if (parsedLevel == LogLevel::Middle && newLevel != "middle" && newLevel != "2") {
        return false;
    }
    logLevel = parsedLevel;
    return true;
}

void Logger::Log(const string& message, LogLevel messageLevel) {
    if (messageLevel < logLevel) return;
    ofstream outFile(logFilename, ios::app);
    if (outFile.is_open()) {
        string levelStr;
        switch (messageLevel) {
        case LogLevel::Low:    levelStr = "LOW"; break;
        case LogLevel::Middle: levelStr = "MIDDLE"; break;
        case LogLevel::High:   levelStr = "HIGH"; break;
        }

        outFile << "[" << getCurrentTime() << "] "
            << "[" << levelStr << "] "
            << message << std::endl;
    }
}

void Logger::Log(const string& message, const string& messageLevel) {
    Log(message, parseLogLevel(messageLevel));
}

void Logger::Log(const string& message, int messageLevel) {
    Log(message, parseLogLevel(messageLevel));
}