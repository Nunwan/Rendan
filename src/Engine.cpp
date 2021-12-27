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
    try {
        context = std::make_shared<VulkanContext>(window);
        device = std::make_shared<VulkanDevice>(context);
        semaphores = std::make_shared<VulkanSemaphores>(context, device);
        swapchain = std::make_shared<VulkanSwapchain>(window, context, device);
        renderPass = std::make_shared<VulkanRenderPass>(context, device, swapchain);
        framebuffers = std::make_shared<VulkanFramebuffers>(context, device, swapchain, renderPass);
        graphicPipeline = std::make_shared<GraphicPipeline>(context, device, swapchain, renderPass);
        commandPool = std::make_shared<VulkanCommandPool>(context, device);
        commandBuffer = std::make_shared<VulkanCommandBuffers>(context, device, framebuffers, commandPool);

    } catch (VulkanInitialisationException &e) {
        Logger::Error(e.what());
        throw std::runtime_error("Impossible to initialiaze Vulkan");
    }
}

void Engine::cleanup()
{
    commandBuffer.reset();
    commandPool.reset();
    graphicPipeline.reset();
    framebuffers.reset();
    renderPass.reset();
    swapchain.reset();
    semaphores.reset();
    device.reset();
    context.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::drawFrame()
{
    uint32_t imageIndex;
    auto commandBuffers = commandBuffer->getCommandBuffers();
    vkAcquireNextImageKHR(device->getDevice(), swapchain->getSwapchain(), UINT64_MAX,
                          semaphores->getAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);

    VkSemaphore waitSemaphores[] = {semaphores->getAvailableSemaphore()};
    VkSemaphore signalSemaphores[] = {semaphores->getFinishedSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffers[imageIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    if (vkQueueSubmit(device->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Impossible to submit draw command buffer");
    }

    VkSwapchainKHR swapChains[] = {swapchain->getSwapchain()};
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,// Optional
    };
    vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
    vkQueueWaitIdle(device->getPresentQueue());
}

void Engine::run()
{
    auto commandBuffers = commandBuffer->getCommandBuffers();
    auto _framebuffer = framebuffers->getFramebuffers();
    for (int i = 0; i < _framebuffer.size(); ++i) {
        VulkanCommandBuffers::beginRecording(commandBuffers[i]);
        renderPass->beginRenderPass(commandBuffers[i], _framebuffer[i]);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline->getPipeline());
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        renderPass->endRenderPass(commandBuffers[i]);
        VulkanCommandBuffers::endRecording(commandBuffers[i]);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(device->getDevice());
}
