#include "Image.hpp"
#include "Logger.hpp"
#include "VulkanCommand.hpp"
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(VmaAllocator vmaAlloc, std::shared_ptr<VulkanCommandBuffers> commandBuffers)
    : height(0), width(0), channels(0), vmaAlloc(vmaAlloc), commandBuffers(commandBuffers)
{}

Image::~Image() { vmaDestroyImage(vmaAlloc, image.image, image.alloc); }

void Image::load(std::string &pathFile)
{
    stbi_uc *pixels = stbi_load(pathFile.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (pixels == nullptr) { throw std::runtime_error("Impossible to load image from file " + pathFile); }
    Logger::Info("Texture loaded from file " + pathFile);
    imageSize = width * height * 4;

    Buffer imageBuffer = Buffer(vmaAlloc, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    imageBuffer.update(pixels);

    stbi_image_free(pixels);
    pixels = nullptr;

    VkExtent3D imageExtent{
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .depth = 1,
    };

    VkImageCreateInfo imageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .extent = imageExtent,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    };

    VmaAllocationCreateInfo allocInfo{
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    };

    auto res = vmaCreateImage(vmaAlloc, &imageInfo, &allocInfo, &image.image, &image.alloc, nullptr);
    if (res != VK_SUCCESS) { throw std::runtime_error("Impossible to create allocated image"); }

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