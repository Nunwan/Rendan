#pragma once

#include <memory>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanContext.hpp"

class VulkanDevice
{
private:
    std::shared_ptr<VulkanContext> context;
    VkDevice device;
    VkQueue graphicQueue;
    VkQueue presentQueue;

    void createDevice();

public:
    VulkanDevice(std::shared_ptr<VulkanContext> context);
    virtual ~VulkanDevice();

    VkQueue& getGraphicQueue();
    VkQueue& getPresentQueue();
};
