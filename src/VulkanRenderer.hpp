#pragma once


#include "VulkanCommand.hpp"
#include "VulkanFramebuffers.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPlatform.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanSwapChain.hpp"
#define GLFW_INCLUDE_VULKAN
#include "VulkanContext.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include "VulkanDevice.hpp"

class VulkanRenderer {
private:
    GLFWwindow* window;
    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<VulkanDevice> device;
    std::shared_ptr<VulkanSwapchain> swapchain;
    std::shared_ptr<VulkanRenderPass> renderPass;
    std::shared_ptr<VulkanFramebuffers> framebuffers;
    std::shared_ptr<GraphicPipeline> graphicPipeline;
    std::shared_ptr<VulkanCommandPool> commandPool;
    std::shared_ptr<VulkanCommandBuffers> commandBuffer;
    std::shared_ptr<VulkanSemaphores> semaphores;


public:
    VulkanRenderer(GLFWwindow* window);

    void render();

    void present();

    void end();

    ~VulkanRenderer();

};
