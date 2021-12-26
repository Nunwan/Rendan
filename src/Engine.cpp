#include "Engine.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "Logger.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPlatform.hpp"
#include "VulkanRenderPass.hpp"
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
        context = std::make_shared<VulkanContext>(window);
        device = std::make_shared<VulkanDevice>(context);
        swapchain = std::make_shared<VulkanSwapchain>(window, context, device);
        renderPass = std::make_shared<VulkanRenderPass>(context, device, swapchain);
        graphicPipeline = std::make_shared<GraphicPipeline>(context, device, swapchain, renderPass);
    } catch (VulkanInitialisationException &e) {
        Logger::Error(e.what());
        throw std::runtime_error("Impossible to initialiaze Vulkan");
    }
}

void Engine::cleanup()
{
    graphicPipeline.reset();
    renderPass.reset();
    swapchain.reset();
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
