#include "UniformBuffer.hpp"


VulkanUniformBuffer::VulkanUniformBuffer(VmaAllocator vmaAllocator, uint32_t size, const void *data)
    : Buffer(vmaAllocator, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU)
{}

void VulkanUniformBuffer::UpdateDescriptorSet(VkDevice device, VkDescriptorSet descriptorSet)
{
    VkDescriptorBufferInfo bufferInfo{
        .buffer = aBuffer.buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };


    VkWriteDescriptorSet descriptorWrite{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}
