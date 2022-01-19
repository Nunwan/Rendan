#include "VulkanFramebuffers.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanUtils.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

VulkanFramebuffers::VulkanFramebuffers(std::shared_ptr<VulkanDevice> device,
                                       std::shared_ptr<VulkanSwapchain> swapchain,
                                       std::shared_ptr<VulkanRenderPass> renderPass)
    : device(device), swapchain(swapchain), renderPass(renderPass)
{
    createFramebuffers();
}

VulkanFramebuffers::~VulkanFramebuffers()
{
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(device->getDevice(), framebuffer, device->getAlloc());
    }
}

void VulkanFramebuffers::createFramebuffers()
{
    auto views = swapchain->getViews();
    framebuffers.resize(views.size());

    for (int i = 0; i < views.size(); ++i) {
        VkImageView attachment[]{views[i]};
        VkFramebufferCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass->getRenderPass(),
            .attachmentCount = 1,
            .pAttachments = attachment,
            .width = swapchain->getExtent().width,
            .height = swapchain->getExtent().height,
            .layers = 1,
        };
        if (vkCreateFramebuffer(device->getDevice(), &createInfo, device->getAlloc(), &framebuffers[i]) !=
            VK_SUCCESS) {
            throw VulkanInitialisationException("Impossible to created framebuffers");
        }
    }
}


std::vector<VkFramebuffer> VulkanFramebuffers::getFramebuffers() { return framebuffers; }
