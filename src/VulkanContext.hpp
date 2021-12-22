#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

// ===================
// Structures
// ===================

class VulkanContext
{
private:
    VkAllocationCallbacks *alloc;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;

    void createVkInstance();
    void getSuitablePhysicalDevice();
    void setupDebugMessenger();
public:
    VulkanContext();
    virtual ~VulkanContext();
    VkInstance& getInstance();
    VkPhysicalDevice& getPhysicalDevice();
};
