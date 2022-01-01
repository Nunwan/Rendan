#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "Logger.hpp"
#include "Vulkan/VulkanCommand.hpp"
#include "Vulkan/VulkanContext.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanFramebuffers.hpp"
#include "Vulkan/VulkanPipeline.hpp"
#include "Vulkan/VulkanPlatform.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanSemaphore.hpp"
#include "Vulkan/VulkanUtils.hpp"

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
