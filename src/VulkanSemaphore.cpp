#include "VulkanSemaphore.hpp"
#include "VulkanUtils.hpp"
#include <vulkan/vulkan_core.h>


VulkanSemaphores::VulkanSemaphores(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device)
    : context(context), device(device)
{
    createSemaphores();
}

void VulkanSemaphores::createSemaphores()
{
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    if (vkCreateSemaphore(device->getDevice(), &createInfo, context->getAlloc(), &imageAvailableSemaphore) !=
            VK_SUCCESS ||
        vkCreateSemaphore(device->getDevice(), &createInfo, context->getAlloc(), &renderFinishedSemaphore) !=
            VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create semaphores");
    }
}

VkSemaphore VulkanSemaphores::getAvailableSemaphore() {
    return imageAvailableSemaphore;
}

VkSemaphore VulkanSemaphores::getFinishedSemaphore() {
    return renderFinishedSemaphore;
}

VulkanSemaphores::~VulkanSemaphores() {
    vkDestroySemaphore(device->getDevice(), imageAvailableSemaphore, context->getAlloc());
    vkDestroySemaphore(device->getDevice(), renderFinishedSemaphore, context->getAlloc());
}
