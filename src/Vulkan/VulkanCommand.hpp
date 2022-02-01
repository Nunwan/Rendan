#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFramebuffers.hpp"
#include "VulkanSwapChain.hpp"
#include <memory>
#include <vector>

class VulkanCommandPool
{
private:
    VulkanDevice *device;

    void createCommandPool();

    VkCommandPool commandPool;

public:
    VulkanCommandPool(VulkanDevice *device);
    virtual ~VulkanCommandPool();

    VkCommandPool getCommandPool();
};

class CommandBuffer
{
private:
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VulkanDevice *device;
    VulkanCommandPool *commandPool;

    void allocCommandBuffer();

public:
    CommandBuffer(VulkanDevice *device, VulkanCommandPool *commandPool);
    virtual ~CommandBuffer();

    VkCommandBuffer &getCommandBuffer();

    void beginRecording();
    void endRecording();

    static VkCommandBuffer beginSingleTimeCommands(VulkanDevice *device, VulkanCommandPool *commandPool);
    static void endSingleTimeCommands(VkCommandBuffer &commandBuffer, VulkanDevice *device,
                                      VulkanCommandPool *commandPool);
};
