#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan_core.h>

class VulkanSemaphores
{
private:
    std::shared_ptr<VulkanDevice> device;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    void createSemaphores();

public:
    VulkanSemaphores(std::shared_ptr<VulkanDevice> device);
    ~VulkanSemaphores();

    VkSemaphore getAvailableSemaphore();
    VkSemaphore getFinishedSemaphore();

};
