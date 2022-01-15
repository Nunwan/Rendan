#pragma once

#include <memory>
#include <stb_image.h>
#include <string>
#include <vulkan/vulkan_core.h>
#include "Buffer.hpp"
#include "VulkanCommand.hpp"
#include "vk_mem_alloc.h"

struct AllocatedImage {
    VkImage image;
    VmaAllocation alloc;
};

class Image {
private:
    VmaAllocator vmaAlloc;
    std::shared_ptr<VulkanCommandBuffers> commandBuffers;

    AllocatedImage image;

    int width, height, channels;
    VkDeviceSize imageSize;


public:
    Image(VmaAllocator vmaAlloc, std::shared_ptr<VulkanCommandBuffers> commandBuffers);
    void load(std::string& pathFile);
    virtual ~Image();

};
