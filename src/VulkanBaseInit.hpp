#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

// ===================
// Utils
// ===================

void AssertSuccess(VkResult result);

// ===================
// Structures
// ===================

typedef struct VulkanDevice {
    VkAllocationCallbacks *alloc;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
} VulkanDevice_t;

VulkanDevice_t initVulkanStruct();

// ===================
// Layers and extension
// ===================

bool areWantedLayersDisponible();

std::vector<const char *> getRequiredExtensions();

// ===================
// Instance
// ===================

VkInstance creationVkInstance();

// ===================
// Physical device
// ===================

VkPhysicalDevice getSuitablePhysicalDevice(const VulkanDevice &vulkanInit);

// ----------------------
// General init

VulkanDevice_t InitVulkan();
