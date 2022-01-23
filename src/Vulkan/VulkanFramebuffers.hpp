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

    VulkanDevice* device;
    VulkanSwapchain* swapchain;
    VulkanRenderPass* renderPass;

    void createFramebuffers(VkImageView depthImageView);

public:
    VulkanFramebuffers(VulkanDevice* device, VulkanSwapchain* swapchain,
                       VulkanRenderPass* renderPass, VkImageView depthImageView);

    virtual ~VulkanFramebuffers();

    std::vector<VkFramebuffer> getFramebuffers();
};
