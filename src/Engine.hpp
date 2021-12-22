#pragma once

#define GLFW_INCLUDE_VULKAN
#include "VulkanContext.hpp"
#include <GLFW/glfw3.h>

class Engine
{
private:
    int width, height;

    GLFWwindow *window;
    VulkanContext* vkContext;

public:
    Engine(int width, int height);

    void init();
    void run();
    void cleanup();
};
