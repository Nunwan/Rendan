#include "Image.hpp"
#include "Logger.hpp"
#include "VulkanCommand.hpp"
#include "VulkanDevice.hpp"
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(VmaAllocator vmaAlloc, VulkanDevice *device, VulkanCommandBuffers *commandBuffers, int height, int width,
             VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
    : height(height), width(width), vmaAlloc(vmaAlloc), commandBuffers(commandBuffers), device(device)
{
    image.image = VK_NULL_HANDLE;
    VkExtent3D imageExtent{
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .depth = 1,
    };

    VkImageCreateInfo imageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = imageExtent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VmaAllocationCreateInfo allocInfo{
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };

    auto res = vmaCreateImage(vmaAlloc, &imageInfo, &allocInfo, &image.image, &image.alloc, nullptr);
    if (res != VK_SUCCESS) {
        Logger::Error(res);
        throw std::runtime_error("Impossible to create allocated image");
    }
}

Image::~Image()
{
    vkDestroyImageView(device->getDevice(), imageView, device->getAlloc());
    vmaDestroyImage(vmaAlloc, image.image, image.alloc);
}


LoadedImage Image::load(const std::string &pathFile)
{
    LoadedImage image{};
    image.pixels = stbi_load(pathFile.c_str(), &image.width, &image.height, &image.channels, STBI_rgb_alpha);
    if (image.pixels == nullptr) { throw std::runtime_error("Impossible to load image from file " + pathFile); }
    Logger::Info("Texture loaded from file " + pathFile);
    return image;
}

void Image::unload(LoadedImage image) { stbi_image_free(image.pixels); }


void Image::write(void *data, VkDeviceSize imageSize)
{
    if (data == nullptr) { throw std::runtime_error("Impossible to write into an image with nullptr data"); }

    VkExtent3D imageExtent{
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .depth = 1,
    };

    Buffer imageBuffer = Buffer(vmaAlloc, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    imageBuffer.update(data);

    auto immediateCmdBuffer = commandBuffers->beginSingleTimeCommands();

    // Copy image

    VkImageSubresourceRange subRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkImageMemoryBarrier imageBarrier_toTransfer{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .image = image.image,
        .subresourceRange = subRange,
    };

    vkCmdPipelineBarrier(immediateCmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

    VkImageSubresourceLayers imageLayers{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel = 0,
        .layerCount = 1,
    };

    VkBufferImageCopy copyRegion{
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = imageLayers,
        .imageExtent = imageExtent,
    };
    vkCmdCopyBufferToImage(immediateCmdBuffer, imageBuffer.getBuffer(), image.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    VkImageMemoryBarrier imageBarrier_toRead = imageBarrier_toTransfer;
    imageBarrier_toRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier_toRead.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier_toRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier_toRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(immediateCmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr, 0, 0, 1, &imageBarrier_toRead);

    // End Commands
    commandBuffers->endSingleTimeCommands(immediateCmdBuffer);
    Logger::Info("Image loaded successfully");
}


void Image::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
{

    VkImageSubresourceRange subRange{
        .aspectMask = aspectFlags,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    VkImageViewCreateInfo viewInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = subRange,
    };

    auto res = vkCreateImageView(device->getDevice(), &viewInfo, device->getAlloc(), &imageView);
    if (res != VK_SUCCESS) { throw std::runtime_error("Impossible to create the view for this image"); }
    Logger::Info("Image View created");
}

VkImageView &Image::getImageView() { return imageView; }

VulkanSampler::VulkanSampler(VulkanDevice *device, Image *image) : device(device), image(image)
{
    VkSamplerCreateInfo samplerInfo{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
    };

    auto res = vkCreateSampler(device->getDevice(), &samplerInfo, device->getAlloc(), &sampler);
    if (res != VK_SUCCESS) { throw std::runtime_error("Impossible to create sampler"); }
}

void VulkanSampler::UpdateDescriptorSet(VkDescriptorSet descriptorSet, VkImageView imageView)
{
    VkDescriptorImageInfo descImage{
        .sampler = sampler,
        .imageView = imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet descWrite{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &descImage,
        .pBufferInfo = nullptr,
    };

    vkUpdateDescriptorSets(device->getDevice(), 1, &descWrite, 0, nullptr);
}

VulkanSampler::~VulkanSampler() { vkDestroySampler(device->getDevice(), sampler, device->getAlloc()); }
