#pragma once

#include <stdexcept>
class VulkanInitialisationException : public std::runtime_error {
  public:
    VulkanInitialisationException(const char* msg) : runtime_error(msg) {}
};
