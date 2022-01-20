#include "VulkanSwapChain.hpp"
#include "Logger.hpp"
#include "VulkanContext.hpp"
#include "VulkanUtils.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vulkan/vulkan_core.h>


VkSurfaceFormatKHR VulkanSwapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &avaibleFormat : availableFormats) {
        if (avaibleFormat.format == WANTED_FORMAT && avaibleFormat.colorSpace == WANTED_COLORSPACE) {
            return avaibleFormat;
        }
    }
    Logger::Warn("No wanted format found : choosing the first one");
    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &presentMode : availablePresentModes) {
        if (presentMode == WANTED_PRESENT_MODE) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::chooseExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent{
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}


void VulkanSwapchain::createSwapchain()
{
    auto swapChainSupport = querySwapChainSupport(device->getPhysicalDevice(), device->getSurface());

    auto surfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
    auto present = choosePresentMode(swapChainSupport.presentModes);
    auto extents = chooseExtent(window, swapChainSupport.capabilities);

    swapChainExtent = extents;
    swapChainFormat = surfaceFormat.format;

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = device->getSurface(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extents,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    QueueFamilyIndices indices = findQueueFamilies(device->getPhysicalDevice(), device->getSurface());
    uint32_t queueFamilyIndices[] = {indices.graphics.value(), indices.presents.value()};

    if (indices.graphics != indices.presents) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;

    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;    // Optional
        createInfo.pQueueFamilyIndices = nullptr;// Optional
    }

    if (vkCreateSwapchainKHR(device->getDevice(), &createInfo, device->getAlloc(), &swapchain) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to createthe swapchain");
    }
    Logger::Info("Swapchain created");

    vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, swapchainImages.data());
}

void VulkanSwapchain::createSwapchainImageViews()
{
    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
        VkImageViewCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapChainFormat,

        };
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device->getDevice(), &createInfo, device->getAlloc(), &swapchainImageViews[i]) != VK_SUCCESS)
            throw VulkanInitialisationException("Impossible to create the view of an image");
    }
    Logger::Info("Image views created");
}


VulkanSwapchain::VulkanSwapchain(GLFWwindow *window, VulkanDevice* device) : window(window), device(device)
{
    createSwapchain();
    createSwapchainImageViews();
}

VkSwapchainKHR VulkanSwapchain::getSwapchain()
{
    return swapchain;
}

VulkanSwapchain::~VulkanSwapchain()
{
    for (auto imageView : swapchainImageViews) {
        vkDestroyImageView(device->getDevice(), imageView, device->getAlloc());
    }
    vkDestroySwapchainKHR(device->getDevice(), swapchain, device->getAlloc());
}


VkExtent2D VulkanSwapchain::getExtent() {
    return swapChainExtent;
}

VkFormat VulkanSwapchain::getFormat() {
    return swapChainFormat;
}


std::vector<VkImageView> VulkanSwapchain::getViews() {
    return swapchainImageViews;
}
