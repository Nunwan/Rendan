#include "VulkanFramebuffers.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanUtils.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

VulkanFramebuffers::VulkanFramebuffers(VulkanDevice *device, VulkanSwapchain *swapchain, VulkanRenderPass *renderPass,
                                       VkImageView depthImageView)
    : device(device), swapchain(swapchain), renderPass(renderPass)
{
    createFramebuffers(depthImageView);
}

VulkanFramebuffers::~VulkanFramebuffers()
{
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(device->getDevice(), framebuffer, device->getAlloc());
    }
}

void VulkanFramebuffers::createFramebuffers(VkImageView depthImageView)
{
    auto views = swapchain->getViews();
    framebuffers.resize(views.size());

    for (int i = 0; i < views.size(); ++i) {
        std::array<VkImageView, 2> attachments = {views[i], depthImageView};
        VkFramebufferCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass->getRenderPass(),
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments = attachments.data(),
            .width = swapchain->getExtent().width,
            .height = swapchain->getExtent().height,
            .layers = 1,
        };
        if (vkCreateFramebuffer(device->getDevice(), &createInfo, device->getAlloc(), &framebuffers[i]) != VK_SUCCESS) {
            throw VulkanInitialisationException("Impossible to created framebuffers");
        }
    }
}


std::vector<VkFramebuffer> VulkanFramebuffers::getFramebuffers() { return framebuffers; }
