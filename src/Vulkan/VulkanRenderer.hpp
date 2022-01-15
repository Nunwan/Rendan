#pragma once


#include "UniformBuffer.hpp"
#include "VulkanCommand.hpp"
#include "VulkanFramebuffers.hpp"
#include "VulkanMesh.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPlatform.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanSwapChain.hpp"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include "VulkanContext.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include "VulkanDevice.hpp"
#include "vk_mem_alloc.h"
#include "Gui.hpp"

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
    std::unique_ptr<Gui> gui;
    VmaAllocator vkallocator;

    VmaAllocator createAllocator();

    Mesh* mesh;
    std::vector<VulkanUniformBuffer*> cameras;


public:
    VulkanRenderer(GLFWwindow* window);

    void load();

    void render();

    void updateUniforms(uint32_t imageIndex);

    void end();

    ~VulkanRenderer();

};
