#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "VulkanDevice.hpp"
#include "vk_mem_alloc.h"
#include <stdexcept>

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

VkFormat findSupportedFormat(VulkanDevice *device, const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                             VkFormatFeatureFlags features);


VkFormat findDepthFormat(VulkanDevice *device);

inline bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
