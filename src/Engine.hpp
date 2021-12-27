#pragma once

#include "VulkanCommand.hpp"
#include "VulkanFramebuffers.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPlatform.hpp"
#include "VulkanRenderer.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanSwapChain.hpp"
#define GLFW_INCLUDE_VULKAN
#include "VulkanContext.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include "VulkanDevice.hpp"

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
