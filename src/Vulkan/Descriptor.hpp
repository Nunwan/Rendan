#pragma once



#include <optional>
#include <vulkan/vulkan_core.h>
class WriteDescriptorSet
{
private:
    VkWriteDescriptorSet writeDescriptor;
    std::optional<VkDescriptorBufferInfo> bufferInfo;
    std::optional<VkDescriptorImageInfo> imageInfo;

public:
    WriteDescriptorSet(const VkWriteDescriptorSet &writeDescriptorSet, const VkDescriptorBufferInfo &bufferInfo)
        : writeDescriptor(writeDescriptorSet), bufferInfo(bufferInfo), imageInfo(std::nullopt)
    {
        writeDescriptor.pBufferInfo = &this->bufferInfo.value();
    }

    WriteDescriptorSet(const VkWriteDescriptorSet &writeDescriptorSet, const VkDescriptorImageInfo &imageInfo)
        : writeDescriptor(writeDescriptorSet), bufferInfo(std::nullopt), imageInfo(imageInfo)
    {
        writeDescriptor.pImageInfo = &this->imageInfo.value();
    }

    VkWriteDescriptorSet &getWriteDescriptor() { return writeDescriptor; }
};
