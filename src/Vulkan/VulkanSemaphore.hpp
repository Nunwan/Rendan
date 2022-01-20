#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan_core.h>

class VulkanSemaphores
{
private:
    VulkanDevice* device;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    void createSemaphores();

public:
    VulkanSemaphores(VulkanDevice* device);
    ~VulkanSemaphores();

    VkSemaphore getAvailableSemaphore();
    VkSemaphore getFinishedSemaphore();

};
