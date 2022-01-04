#pragma once

#include "Buffer.hpp"
#include "vk_mem_alloc.h"

class VulkanUniformBuffer : public Buffer
{
private:
    VmaAllocator vmaAllocator;
    uint32_t size;

public:
    VulkanUniformBuffer(VmaAllocator vmaAllocator, uint32_t size, const void *data);

    void UpdateDescriptorSet(VkDevice device, VkDescriptorSet descriptorSet);
};
