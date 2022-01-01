#include "VulkanDevice.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include <cstdint>
#include <memory>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>


void VulkanDevice::createDevice()
{
    auto layers = VulkanContext::getLayers();

    // Queues
    auto indices = findQueueFamilies(context->getPhysicalDevice(), context->getSurface());
    if (!indices.graphics.has_value()) {
        throw VulkanInitialisationException("There is no graphics queue for the logical device");
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphics.value(), indices.presents.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };
        queueCreateInfos.push_back(queueInfo);
    }

    // Features
    VkPhysicalDeviceFeatures deviceFeatures{};
    auto extensions = VulkanContext::getDeviceExtensions();

    // Device
    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &deviceFeatures,
    };


    if (vkCreateDevice(context->getPhysicalDevice(), &createInfo, context->getAlloc(), &device) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create the logical device");
        return;
    }
    Logger::Info("Logical device created");

    vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphicQueue);
    vkGetDeviceQueue(device, indices.presents.value(), 0, &presentQueue);
}


VulkanDevice::VulkanDevice(std::shared_ptr<VulkanContext> context) : context(context)
{
    createDevice();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, context->getAlloc());
}


VkQueue &VulkanDevice::getGraphicQueue()
{
    return graphicQueue;
}


VkQueue &VulkanDevice::getPresentQueue()
{
    return presentQueue;
}

VkDevice VulkanDevice::getDevice()
{
    return device;
}
