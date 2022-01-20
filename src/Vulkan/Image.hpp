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
    VulkanCommandBuffers* commandBuffers;
    VulkanDevice* device;

    AllocatedImage image;
    VkImageView imageView;

    int width, height, channels;
    VkDeviceSize imageSize;


public:
    Image(VmaAllocator vmaAlloc, VulkanDevice* device, VulkanCommandBuffers* commandBuffers);
    VkImageView getImageView();
    void load(std::string &pathFile);
    virtual ~Image();
};

class VulkanSampler
{
private:
    VulkanDevice* device;
    Image* image;
    VkSampler sampler;

public:
    VulkanSampler(VulkanDevice* device, Image* image);
    virtual ~VulkanSampler();
    void UpdateDescriptorSet(VkDescriptorSet descriptorSet);
};
