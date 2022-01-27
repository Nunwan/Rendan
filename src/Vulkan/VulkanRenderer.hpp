#pragma once


#include "DescriptorSet.hpp"
#include "Image.hpp"
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
    VulkanDevice* device;
    VulkanSwapchain* swapchain;
    VulkanRenderPass* renderPass;
    VulkanFramebuffers* framebuffers;
    GraphicPipeline* graphicPipeline;
    VulkanCommandPool* commandPool;
    VulkanCommandBuffers* commandBuffer;
    VulkanSemaphores* semaphores;
    std::unique_ptr<Gui> gui;
    VmaAllocator vkallocator;

    VmaAllocator createAllocator();

    Image* depthImage;

    Mesh* mesh;

    VulkanUniformBuffer* cameraBuffer;
    DescriptorSet* descriptor;

    Image* loadedImage;
    VulkanSampler* sampler;


public:
    VulkanRenderer(GLFWwindow* window);

    void load();

    void render();

    void updateUniforms(uint32_t imageIndex);

    void end();

    ~VulkanRenderer();

};
