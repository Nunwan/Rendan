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
    VulkanDevice* device;

    void createCommandPool();

    VkCommandPool commandPool;

public:
    VulkanCommandPool(VulkanDevice* device);
    virtual ~VulkanCommandPool();

    VkCommandPool getCommandPool();
};

class VulkanCommandBuffers
{
private:
    std::vector<VkCommandBuffer> commandBuffers;

    VulkanDevice* device;
    VulkanFramebuffers* framebuffers;
    VulkanCommandPool* commandPool;

    void allocCommandBuffers();

public:
    VulkanCommandBuffers(VulkanDevice* device, VulkanFramebuffers* framebuffers,
                         VulkanCommandPool* commandPool);
    virtual ~VulkanCommandBuffers();

    std::vector<VkCommandBuffer> getCommandBuffers();

    static void beginRecording(VkCommandBuffer &commandBuffer);
    static void endRecording(VkCommandBuffer &commandBuffer);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer &commandBuffer);
};
