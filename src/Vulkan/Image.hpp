#pragma once

#include "Buffer.hpp"
#include "Descriptor.hpp"
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
    stbi_uc *pixels;
    int width, height, channels;
};

class Image
{
private:
    VmaAllocator vmaAlloc;
    VulkanCommandPool *commandPool;
    VulkanDevice *device;

    AllocatedImage image;
    VkImageView imageView;

    int width, height;


public:
    Image(VmaAllocator vmaAlloc, VulkanDevice *device, VulkanCommandPool *commandPool, int height, int width,
          VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
    VkImageView &getImageView();

    static LoadedImage load(const std::string &pathFile);
    static void unload(LoadedImage image);

    void write(void *data, VkDeviceSize imageSize);

    virtual ~Image();
};

class VulkanSampler
{
private:
    VulkanDevice *device;
    Image *image;
    VkSampler sampler;

public:
    VulkanSampler(VulkanDevice *device, Image *image);
    virtual ~VulkanSampler();
    WriteDescriptorSet GetWrite(VkImageView imageView, uint32_t binding);
    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(uint32_t binding, VkShaderStageFlags stage);
};
