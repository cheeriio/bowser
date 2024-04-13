#include <iostream>

#include <log.hpp>

namespace {

  std::string INFO = "\033[94m",
              WARNING = "\033[93m",
              ERROR = "\033[91m",
              END_COLOR = "\033[0m";

  std::string header(LogLevel level) {
    std::string color_start;
    switch(level) {
      case LogLevel::kInfo:
        return INFO + "info: " + END_COLOR;
      case LogLevel::kWarning:
        return WARNING + "warning: " + END_COLOR;
      case LogLevel::kError:
        return ERROR + "error: " + END_COLOR;
      default:
        return "";
    }
  }
}

void Log(LogLevel level, std::string message) {
  // TODO(me) Add debug target which changes implementation
  std::cerr << header(level) << message << "\n";
}
