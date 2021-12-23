#include "VulkanPlatform.hpp"

#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>


VulkanPlatform::VulkanPlatform(VkInstance instance, VkAllocationCallbacks* alloc): instance(instance), alloc(alloc), surface(VK_NULL_HANDLE) {}

void VulkanPlatform::createSurface(GLFWwindow *window)
{
    if (glfwCreateWindowSurface(instance, window, alloc, &surface) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create window surface");
    }
    Logger::Info("Surface created");
}

VulkanPlatform::~VulkanPlatform()
{
    vkDestroySurfaceKHR(instance, surface, alloc);
}

VkSurfaceKHR &VulkanPlatform::getSurface()
{
    return surface;
}
