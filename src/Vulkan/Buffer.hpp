#pragma once

#include "Vulkan/VulkanUtils.hpp"
#include "vk_mem_alloc.h"
#include <vulkan/vulkan_core.h>


class Buffer
{
protected:
    VmaAllocator vmaAllocator;
    const void *data;
    uint32_t size;

    AllocatedBuffer aBuffer;

public:
    Buffer(VmaAllocator vmaAllocator, uint32_t size, const void *data, VkBufferUsageFlags usage,
           VmaMemoryUsage memoryUsage);

    void update(const void *newData);

    virtual ~Buffer();
};
