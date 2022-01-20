#pragma once

#include "Buffer.hpp"
#include "VulkanCommand.hpp"
#include "vk_mem_alloc.h"
#include <memory>
#include <stb_image.h>
#include <string>
#include <vulkan/vulkan_core.h>

struct AllocatedImage {
    VkImage image;
    VmaAllocation alloc;
};

class Image
{
private:
    VmaAllocator vmaAlloc;
    std::shared_ptr<VulkanCommandBuffers> commandBuffers;
    std::shared_ptr<VulkanDevice> device;

    AllocatedImage image;
    VkImageView imageView;

    int width, height, channels;
    VkDeviceSize imageSize;


public:
    Image(VmaAllocator vmaAlloc, std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanCommandBuffers> commandBuffers);
    // TODO(Nunwan) destroy image view ??
    VkImageView getImageView();
    void load(std::string &pathFile);
    virtual ~Image();
};

class VulkanSampler
{
private:
    std::shared_ptr<VulkanDevice> device;
    Image* image;
    VkSampler sampler;

public:
    VulkanSampler(std::shared_ptr<VulkanDevice> device, Image* image);
    virtual ~VulkanSampler();
    void UpdateDescriptorSet(VkDescriptorSet descriptorSet);
};
