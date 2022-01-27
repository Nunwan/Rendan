#include "UniformBuffer.hpp"


VulkanUniformBuffer::VulkanUniformBuffer(VmaAllocator vmaAllocator, uint32_t size, const void *data)
    : Buffer(vmaAllocator, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU)
{}

WriteDescriptorSet VulkanUniformBuffer::GetWrite(VkDevice device)
{
    VkDescriptorBufferInfo bufferInfo{
        .buffer = aBuffer.buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };


    VkWriteDescriptorSet descriptorWrite{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        // .pBufferInfo = &bufferInfo,
        .pTexelBufferView = nullptr,
    };

    return {descriptorWrite, bufferInfo};
}
