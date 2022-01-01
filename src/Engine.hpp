#pragma once

#include "Vulkan/VulkanCommand.hpp"
#include "Vulkan/VulkanFramebuffers.hpp"
#include "Vulkan/VulkanPipeline.hpp"
#include "Vulkan/VulkanPlatform.hpp"
#include "Vulkan/VulkanRenderer.hpp"
#include "Vulkan/VulkanSemaphore.hpp"
#include "Vulkan/VulkanSwapChain.hpp"
#define GLFW_INCLUDE_VULKAN
#include "Vulkan/VulkanContext.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include "Vulkan/VulkanDevice.hpp"

class Engine
{
private:
    int width, height;

    GLFWwindow *window;
    std::unique_ptr<VulkanRenderer> renderer;



public:
    Engine(int width, int height);

    void init();
    void run();
    void cleanup();
};
