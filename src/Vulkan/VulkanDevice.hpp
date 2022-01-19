#pragma once

#include <memory>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanContext.hpp"

class VulkanDevice
{

private:
    VulkanContext* context;
    VkDevice device;
    VkQueue graphicQueue;
    VkQueue presentQueue;

    void createDevice();

public:
    VulkanDevice(GLFWwindow *window);
    virtual ~VulkanDevice();

    VkQueue &getGraphicQueue();
    VkQueue &getPresentQueue();
    VkDevice getDevice();

    VkInstance &getInstance();
    VkPhysicalDevice &getPhysicalDevice();
    VkAllocationCallbacks *getAlloc();
    VkSurfaceKHR &getSurface();
};
