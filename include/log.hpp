#ifndef _BOWSER_LOG_HPP_
#define _BOWSER_LOG_HPP_

enum class LogLevel {
  kInfo,
  kWarning,
  kError
};

void Log(LogLevel level, std::string message);

#endif // _BOWSER_LOG_HPP_