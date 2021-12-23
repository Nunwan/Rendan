#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
};
QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice);


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
    VkAllocationCallbacks* getAlloc();
    static std::vector<const char*> getRequiredExtensions();
    static std::vector<const char*> getLayers();

};
