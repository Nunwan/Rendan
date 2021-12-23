#include "VulkanDevice.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>


void VulkanDevice::createDevice()
{
    auto layers = VulkanContext::getLayers();

    // Queues
    auto indices = findQueueFamilies(context->getPhysicalDevice());
    if (!indices.graphics.has_value()) {
        throw VulkanInitialisationException("There is no graphics queue for the logical device");
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = indices.graphics.value(),
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };

    // Features
    VkPhysicalDeviceFeatures deviceFeatures {};

    // Device
    VkDeviceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = 0,
        .pEnabledFeatures = &deviceFeatures,
    };


    if (vkCreateDevice(context->getPhysicalDevice(), &createInfo, context->getAlloc(), &device) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create the logical device");
        return;
    }
    Logger::Info("Logical device created");

    vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphicQueue);
}


VulkanDevice::VulkanDevice(std::shared_ptr<VulkanContext> context) : context(context)
{
    createDevice();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, context->getAlloc());
}


VkQueue& VulkanDevice::getGraphicQueue() {
    return graphicQueue;
}
