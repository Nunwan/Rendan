#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "Logger.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanUtils.hpp"

Engine::Engine(int width, int height) : width(width), height(height) {}

void Engine::init()
{
    Logger::Info("Engine initialisation");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, "Rendan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    // Vulkan
    try {
        context = std::make_shared<VulkanContext>();
        device = std::make_shared<VulkanDevice>(context);
    } catch (VulkanInitialisationException& e) {
        Logger::Error(e.what());
        throw std::runtime_error("Impossible to initialiaze Vulkan");
    }
}

void Engine::cleanup()
{
    device.reset();
    context.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::run()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
