#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Logger.hpp"
#include "VulkanCommand.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFramebuffers.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPlatform.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSemaphore.hpp"
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
    renderer = std::make_unique<VulkanRenderer>(window);
}

void Engine::cleanup()
{
    renderer.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}


void Engine::run()
{
    renderer->render();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderer->present();
    }
    renderer->end();
}
