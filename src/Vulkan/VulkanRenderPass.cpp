#include "VulkanRenderPass.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include <stdexcept>


VulkanRenderPass::VulkanRenderPass(std::shared_ptr<VulkanDevice> device,
                                   std::shared_ptr<VulkanSwapchain> swapchain)
    : device(device), swapchain(swapchain)
{
    createRenderPass();
}

void VulkanRenderPass::createRenderPass()
{
    VkAttachmentDescription colorAttachment{
        .format = swapchain->getFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

    };

    VkAttachmentReference colorAttachmentRef{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDependency dependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkSubpassDescription subpass{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
    };

    VkRenderPassCreateInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    if (vkCreateRenderPass(device->getDevice(), &renderPassInfo, device->getAlloc(), &renderPass) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create the render pass");
    }

    Logger::Info("Render pass created");
}

VulkanRenderPass::~VulkanRenderPass() { vkDestroyRenderPass(device->getDevice(), renderPass, device->getAlloc()); }

VkRenderPass VulkanRenderPass::getRenderPass() { return renderPass; }


void VulkanRenderPass::beginRenderPass(VkCommandBuffer &commandBuffer, VkFramebuffer &framebuffer)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getExtent();
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    Logger::Trace("Begin render pass");
}

void VulkanRenderPass::endRenderPass(VkCommandBuffer &commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
    Logger::Trace("End render pass");
}
