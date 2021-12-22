#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

#include "Logger.hpp"
#include "VulkanContext.hpp"

Engine::Engine(int width, int height) : width(width), height(height) {}

void Engine::init()
{
    Logger::Info("Engine initialisation");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, "Rendan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    // Vulkan
    vkContext = new VulkanContext();
}

void Engine::cleanup()
{
    delete vkContext;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::run()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
