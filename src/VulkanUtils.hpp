#pragma once

#include <vulkan/vulkan_core.h>

#include <stdexcept>
class VulkanInitialisationException : public std::runtime_error
{
public:
    VulkanInitialisationException(const char *msg) : runtime_error(msg) {}
    VulkanInitialisationException(const VkResult result)
        : runtime_error("TODO result dispatch message!") {}
};
