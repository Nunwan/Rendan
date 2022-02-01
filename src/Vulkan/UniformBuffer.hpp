#pragma once

#include "Buffer.hpp"
#include "Descriptor.hpp"
#include "vk_mem_alloc.h"
#include <optional>

class VulkanUniformBuffer : public Buffer
{
private:
    VmaAllocator vmaAllocator;
    uint32_t size;

public:
    VulkanUniformBuffer(VmaAllocator vmaAllocator, uint32_t size, const void *data);

    WriteDescriptorSet GetWrite(VkDevice device, uint32_t binding);
    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(uint32_t binding, VkShaderStageFlags stage);
};
