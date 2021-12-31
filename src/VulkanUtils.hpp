#pragma once

#include <vulkan/vulkan_core.h>

#include <stdexcept>
#include "vk_mem_alloc.h"

class VulkanInitialisationException : public std::runtime_error
{
public:
    VulkanInitialisationException(const char *msg) : runtime_error(msg) {}
    VulkanInitialisationException(const VkResult result) : runtime_error("TODO result dispatch message!") {}
};


inline void AssertSuccess(VkResult result)
{
    if (result != VK_SUCCESS) { throw new VulkanInitialisationException("General initialisation of Vulkan failed"); }
}

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
};
