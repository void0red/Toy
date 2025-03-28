//
// Created by ikaros on 2025/3/28.
//

#ifndef LOG_HPP
#define LOG_HPP
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

namespace Toy {

enum class LogLevel { DEBUG, INFO, WARN, ERROR, FATAL };

class Logger {
public:
  static Logger &instance() {
    static Logger instance;
    return instance;
  }

  explicit Logger(LogLevel level = LogLevel::INFO,
                  std::ostream &out = std::cout)
      : currentLevel(level), outputStream(out) {}

  void setLogLevel(LogLevel level) { currentLevel = level; }

  template <typename... Args>
  void log(LogLevel level, const char *format, Args... args) {
    if (level < currentLevel)
      return;

    std::lock_guard lock(logMutex);
    writeHeader(level);
    writeFormattedMessage(format, args...);
    outputStream << std::endl;
  }

  template <typename... Args> void debug(const char *format, Args... args) {
    log(LogLevel::DEBUG, format, args...);
  }

  template <typename... Args> void info(const char *format, Args... args) {
    log(LogLevel::INFO, format, args...);
  }

  template <typename... Args> void warn(const char *format, Args... args) {
    log(LogLevel::WARN, format, args...);
  }

  template <typename... Args> void error(const char *format, Args... args) {
    log(LogLevel::ERROR, format, args...);
  }

  template <typename... Args> void fatal(const char *format, Args... args) {
    log(LogLevel::FATAL, format, args...);
  }

private:
  LogLevel currentLevel;
  std::ostream &outputStream;
  std::mutex logMutex;

  void writeHeader(LogLevel level) {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);

    outputStream << "[" << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "] ";
    outputStream << "[" << toString(level) << "] ";
  }

  template <typename... Args>
  void writeFormattedMessage(const char *format, Args... args) {
    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer), format, args...);
    outputStream << buffer;
  }

  static std::string toString(LogLevel level) {
    switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::FATAL:
      return "FATAL";
    default:
      return "UNKNOWN";
    }
  }
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CONCAT_FILE_LINE __FILE__ ":" TOSTRING(__LINE__) ": "
#define LOG_DEBUG(...)                                                         \
  Toy::Logger::instance().debug(CONCAT_FILE_LINE __VA_ARGS__)
#define LOG_INFO(...) Toy::Logger::instance().info(CONCAT_FILE_LINE __VA_ARGS__)
#define LOG_WARN(...) Toy::Logger::instance().warn(CONCAT_FILE_LINE __VA_ARGS__)
#define LOG_ERROR(...)                                                         \
  Toy::Logger::instance().error(CONCAT_FILE_LINE __VA_ARGS__)
#define LOG_FATAL(...)                                                         \
  Toy::Logger::instance().fatal(CONCAT_FILE_LINE __VA_ARGS__)

} // namespace Toy
#endif // LOG_HPP
