#include "VulkanCommand.hpp"
#include "Logger.hpp"
#include "VulkanContext.hpp"
#include "VulkanUtils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VulkanCommandPool::VulkanCommandPool(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device)
    : context(context), device(device)
{
    createCommandPool();
}

void VulkanCommandPool::createCommandPool()
{
    auto queueFamilyIndices = findQueueFamilies(context->getPhysicalDevice(), context->getSurface());

    VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = 0,
        .queueFamilyIndex = queueFamilyIndices.graphics.value(),
    };

    if (vkCreateCommandPool(device->getDevice(), &createInfo, context->getAlloc(), &commandPool) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create command pool");
    }
    Logger::Info("Command pool created");
}

VulkanCommandPool::~VulkanCommandPool() { vkDestroyCommandPool(device->getDevice(), commandPool, context->getAlloc()); }

VkCommandPool VulkanCommandPool::getCommandPool() { return commandPool; }

// VulkanCommandBuffers


VulkanCommandBuffers::VulkanCommandBuffers(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device,
                                           std::shared_ptr<VulkanFramebuffers> framebuffers,
                                           std::shared_ptr<VulkanCommandPool> commandPool)
    : context(context), device(device), framebuffers(framebuffers), commandPool(commandPool)
{
    allocCommandBuffers();
}

void VulkanCommandBuffers::allocCommandBuffers()
{
    commandBuffers.resize(framebuffers->getFramebuffers().size());
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool->getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) commandBuffers.size(),
    };

    if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to allocate command buffers");
    }
    Logger::Info("Command buffer allocated");
}

void VulkanCommandBuffers::beginRecording(VkCommandBuffer &commandBuffer)
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

void VulkanCommandBuffers::endRecording(VkCommandBuffer &commandBuffer)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

VulkanCommandBuffers::~VulkanCommandBuffers()
{
    vkFreeCommandBuffers(device->getDevice(), commandPool->getCommandPool(), commandBuffers.size(),
                         commandBuffers.data());
}


std::vector<VkCommandBuffer> VulkanCommandBuffers::getCommandBuffers() { return commandBuffers; }


VkCommandBuffer VulkanCommandBuffers::beginSingleTimeCommands()
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
    VulkanCommandBuffers::beginRecording(commandBuffer);
    return commandBuffer;
}

void VulkanCommandBuffers::endSingleTimeCommands(VkCommandBuffer &commandBuffer)
{
    VulkanCommandBuffers::endRecording(commandBuffer);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->getGraphicQueue());

    vkFreeCommandBuffers(device->getDevice(), commandPool->getCommandPool(), 1, &commandBuffer);
}
