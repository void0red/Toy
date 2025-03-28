//
// Created by ikaros on 2025/3/28.
//

#ifndef LOG_HPP
#define LOG_HPP
#include "magic_enum/magic_enum.hpp"
#include <format>
#include <iomanip>
#include <iostream>
#include <mutex>

namespace Toy {
class NullStreamBuf : public std::streambuf {
protected:
  std::streambuf::int_type overflow(std::streambuf::int_type c) override {
    return c;
  }
  int sync() override { return 0; }
};

class NullStream : public std::ostream {
private:
  NullStreamBuf buffer;

public:
  NullStream() : std::ostream(&buffer) {}

  NullStream(const NullStream &) = delete;
  NullStream &operator=(const NullStream &) = delete;
};
static NullStream nullStream;

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
  std::ostream &log(LogLevel level, std::format_string<Args...> fmt,
                    Args... args) {
    if (level < currentLevel) {
      return nullStream;
    }

    std::lock_guard lock(logMutex);
    writeHeader(level);
    outputStream << std::format(fmt, args...) << std::endl;
    return outputStream;
  }

  template <typename... Args>
  std::ostream &debug(std::format_string<Args...> fmt, Args... args) {
    return log(LogLevel::DEBUG, fmt, args...);
  }

  template <typename... Args>
  std::ostream &info(std::format_string<Args...> fmt, Args... args) {
    return log(LogLevel::INFO, fmt, args...);
  }

  template <typename... Args>
  std::ostream &warn(std::format_string<Args...> fmt, Args... args) {
    return log(LogLevel::WARN, fmt, args...);
  }

  template <typename... Args>
  std::ostream &error(std::format_string<Args...> fmt, Args... args) {
    return log(LogLevel::ERROR, fmt, args...);
  }

  template <typename... Args>
  std::ostream &fatal(std::format_string<Args...> fmt, Args... args) {
    return log(LogLevel::FATAL, fmt, args...);
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
    outputStream << "[" << magic_enum::enum_name(level) << "] ";
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
