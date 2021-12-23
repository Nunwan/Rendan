#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>
#include <memory>

class VulkanPlatform
{
private:
    VkSurfaceKHR surface;
    VkInstance instance;
    VkAllocationCallbacks* alloc;

public:
    VulkanPlatform(VkInstance instance, VkAllocationCallbacks* alloc);
    virtual ~VulkanPlatform();

    void createSurface(GLFWwindow* window);

    VkSurfaceKHR& getSurface();
};
