#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class VulkanFramebuffers
{
private:
    std::vector<VkFramebuffer> framebuffers;

    std::shared_ptr<VulkanDevice> device;
    std::shared_ptr<VulkanSwapchain> swapchain;
    std::shared_ptr<VulkanRenderPass> renderPass;

    void createFramebuffers();

public:
    VulkanFramebuffers(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain,
                       std::shared_ptr<VulkanRenderPass> renderPass);

    virtual ~VulkanFramebuffers();

    std::vector<VkFramebuffer> getFramebuffers();
};
