#pragma once


#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

class VulkanRenderPass
{
private:
    VkRenderPass renderPass;

    VulkanDevice* device;
    VulkanSwapchain* swapchain;
    void createRenderPass();

public:
    VulkanRenderPass(VulkanDevice* device,
                     VulkanSwapchain* swapchain);

    virtual ~VulkanRenderPass();

    VkRenderPass getRenderPass();

    void beginRenderPass(VkCommandBuffer &commandBuffer, VkFramebuffer &framebuffer);
    void endRenderPass(VkCommandBuffer& commandBuffer);
};
