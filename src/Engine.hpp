#pragma once

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
    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<VulkanDevice> device;

public:
    Engine(int width, int height);

    void init();
    void run();
    void cleanup();
};
