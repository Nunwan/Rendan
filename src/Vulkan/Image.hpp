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

struct LoadedImage {
    stbi_uc* pixels;
    int width, height, channels;
};

class Image
{
private:
    VmaAllocator vmaAlloc;
    VulkanCommandBuffers* commandBuffers;
    VulkanDevice* device;

    AllocatedImage image;
    VkImageView imageView;

    int width, height;


public:
    Image(VmaAllocator vmaAlloc, VulkanDevice* device, VulkanCommandBuffers* commandBuffers, int width, int height);

    void createImageView();
    VkImageView& getImageView();

    static LoadedImage load(std::string &pathFile);
    static void unload(LoadedImage image);

    void write(void* data, VkDeviceSize imageSize);

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
    void UpdateDescriptorSet(VkDescriptorSet descriptorSet, VkImageView imageView);
};
