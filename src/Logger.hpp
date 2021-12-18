#pragma once

#include <utility>

#include "spdlog/spdlog.h"

class Logger {
 public:
  template <typename T>
  static void Trace(const T &msg) {
    spdlog::trace(msg);
  }

  template <typename... Args>
  static void Info(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::info(fmt, std::forward<Args>(args)...);
  }

  template <typename T>
  static void Info(const T &msg) {
    spdlog::info(msg);
  }

  template <typename... Args>
  static void Warn(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::warn(fmt, std::forward<Args>(args)...);
  }

  template <typename T>
  static void Warn(const T &msg) {
    spdlog::warn(msg);
  }

  template <typename... Args>
  static void Error(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::error(fmt, std::forward<Args>(args)...);
  }

  template <typename T>
  static void Error(const T &msg) {
    spdlog::error(msg);
  }
};
