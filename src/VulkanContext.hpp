#pragma once

#include "VulkanPlatform.hpp"
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> presents;

    bool isComplete() {
        return graphics.has_value() && presents.has_value();
    }
};
QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR& surface);


class VulkanContext
{
private:
    VkAllocationCallbacks *alloc;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    std::shared_ptr<VulkanPlatform> platform;

    void createVkInstance();
    void getSuitablePhysicalDevice();
    void setupDebugMessenger();
public:
    VulkanContext(GLFWwindow* window);
    virtual ~VulkanContext();

    VkInstance& getInstance();
    VkPhysicalDevice& getPhysicalDevice();
    VkAllocationCallbacks* getAlloc();
    VkSurfaceKHR& getSurface();
    static std::vector<const char*> getRequiredExtensions();
    static std::vector<const char*> getLayers();

};
