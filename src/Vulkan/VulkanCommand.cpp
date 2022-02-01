#include "VulkanCommand.hpp"
#include "Logger.hpp"
#include "VulkanContext.hpp"
#include "VulkanUtils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VulkanCommandPool::VulkanCommandPool(VulkanDevice *device) : device(device) { createCommandPool(); }

void VulkanCommandPool::createCommandPool()
{
    auto queueFamilyIndices = findQueueFamilies(device->getPhysicalDevice(), device->getSurface());

    VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = 0,
        .queueFamilyIndex = queueFamilyIndices.graphics.value(),
    };

    if (vkCreateCommandPool(device->getDevice(), &createInfo, device->getAlloc(), &commandPool) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create command pool");
    }
    Logger::Info("Command pool created");
}

VulkanCommandPool::~VulkanCommandPool() { vkDestroyCommandPool(device->getDevice(), commandPool, device->getAlloc()); }

VkCommandPool VulkanCommandPool::getCommandPool() { return commandPool; }

// CommandBuffer


CommandBuffer::CommandBuffer(VulkanDevice *device, VulkanCommandPool *commandPool)
    : device(device), commandPool(commandPool)
{
    allocCommandBuffer();
}

void CommandBuffer::allocCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool->getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to allocate command buffer");
    }
    Logger::Info("Command buffer allocated");
}

void CommandBuffer::beginRecording()
{
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,                 // Optional
        .pInheritanceInfo = nullptr,// Optional
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void CommandBuffer::endRecording()
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(device->getDevice(), commandPool->getCommandPool(), 1, &commandBuffer);
}


VkCommandBuffer &CommandBuffer::getCommandBuffer() { return commandBuffer; }


VkCommandBuffer CommandBuffer::beginSingleTimeCommands(VulkanDevice* device, VulkanCommandPool* commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool->getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    auto err = vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer);
    if (err != VK_SUCCESS) { throw std::runtime_error("Impossible to create a one time command buffer"); }
    // Begin reccording
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,                 // Optional
        .pInheritanceInfo = nullptr,// Optional
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}

void CommandBuffer::endSingleTimeCommands(VkCommandBuffer &commandBuffer, VulkanDevice* device, VulkanCommandPool* commandPool)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->getGraphicQueue());

    vkFreeCommandBuffers(device->getDevice(), commandPool->getCommandPool(), 1, &commandBuffer);
}
