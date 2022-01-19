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
    std::shared_ptr<VulkanDevice> device;

    void createCommandPool();

    VkCommandPool commandPool;

public:
    VulkanCommandPool(std::shared_ptr<VulkanDevice> device);
    virtual ~VulkanCommandPool();

    VkCommandPool getCommandPool();
};

class VulkanCommandBuffers
{
private:
    std::vector<VkCommandBuffer> commandBuffers;

    std::shared_ptr<VulkanDevice> device;
    std::shared_ptr<VulkanFramebuffers> framebuffers;
    std::shared_ptr<VulkanCommandPool> commandPool;

    void allocCommandBuffers();

public:
    VulkanCommandBuffers(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanFramebuffers> framebuffers,
                         std::shared_ptr<VulkanCommandPool> commandPool);
    virtual ~VulkanCommandBuffers();

    std::vector<VkCommandBuffer> getCommandBuffers();

    static void beginRecording(VkCommandBuffer &commandBuffer);
    static void endRecording(VkCommandBuffer &commandBuffer);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer &commandBuffer);
};
