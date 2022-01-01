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

    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<VulkanDevice> device;
    std::shared_ptr<VulkanSwapchain> swapchain;
    void createRenderPass();

public:
    VulkanRenderPass(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device,
                     std::shared_ptr<VulkanSwapchain> swapchain);

    virtual ~VulkanRenderPass();

    VkRenderPass getRenderPass();

    void beginRenderPass(VkCommandBuffer &commandBuffer, VkFramebuffer &framebuffer);
    void endRenderPass(VkCommandBuffer& commandBuffer);
};
