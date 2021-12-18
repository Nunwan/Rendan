#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

typedef struct VulkanDevice {
  VkAllocationCallbacks* alloc;
  VkInstance instance;
} VulkanDevice_t;

void AssertSuccess(VkResult result);

VulkanDevice_t initVulkanStruct();

bool areWantedLayersDisponible();

std::vector<const char*> getRequiredExtensions();


VkInstance creationVkInstance();

VulkanDevice_t InitVulkan();
