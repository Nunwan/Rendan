#include "VulkanRenderer.hpp"
#include "Logger.hpp"
#include "VulkanMesh.hpp"
#include "VulkanShader.hpp"
#include "VulkanUtils.hpp"
#include <glm/gtc/constants.hpp>
#include <unordered_map>
#include <vector>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

VulkanRenderer::VulkanRenderer(GLFWwindow *window) : window(window)
{
    try {
        context = std::make_shared<VulkanContext>(window);
        device = std::make_shared<VulkanDevice>(context);
        vkallocator = createAllocator();
        semaphores = std::make_shared<VulkanSemaphores>(context, device);
        swapchain = std::make_shared<VulkanSwapchain>(window, context, device);
        renderPass = std::make_shared<VulkanRenderPass>(context, device, swapchain);
        framebuffers = std::make_shared<VulkanFramebuffers>(context, device, swapchain, renderPass);

        // Shaders
        std::unordered_map<ShaderStage, std::string> shaderFiles = {{ShaderStage::VertexStage, "shaders/09_shader_base.vert.spv"}, {ShaderStage::FragmentStage, "shaders/09_shader_base.frag.spv"}};
        VulkanShader shaders = VulkanShader(shaderFiles, context, device);
        camera = new VulkanUniformBuffer(vkallocator, sizeof(MeshConstant), nullptr);
        shaders.addUniform(ShaderStage::VertexStage, camera);

        graphicPipeline = std::make_shared<GraphicPipeline>(context, device, swapchain, renderPass);
        graphicPipeline->createPipeline(shaders);


        commandPool = std::make_shared<VulkanCommandPool>(context, device);
        commandBuffer = std::make_shared<VulkanCommandBuffers>(context, device, framebuffers, commandPool);
    } catch (VulkanInitialisationException &e) {
        Logger::Error(e.what());
        throw std::runtime_error("Impossible to initialiaze Vulkan");
    }
}

VulkanRenderer::~VulkanRenderer()
{
    delete camera;
    delete mesh;
    commandBuffer.reset();
    commandPool.reset();
    graphicPipeline.reset();
    framebuffers.reset();
    renderPass.reset();
    swapchain.reset();
    semaphores.reset();
    vmaDestroyAllocator(vkallocator);
    device.reset();
    context.reset();
}

void VulkanRenderer::updateUniforms()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    auto model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto proj = glm::perspective(glm::radians(45.0f),
                                 swapchain->getExtent().width / (float) swapchain->getExtent().height, 0.1f, 10.0f);
    proj[1][1] *= -1.f;
    MeshConstant newCamera{
        .cameraMatrix = proj * view * model,
    };
    camera->update(&newCamera);
}

void VulkanRenderer::present()
{
    uint32_t imageIndex;
    auto commandBuffers = commandBuffer->getCommandBuffers();
    vkAcquireNextImageKHR(device->getDevice(), swapchain->getSwapchain(), UINT64_MAX,
                          semaphores->getAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);

    updateUniforms();


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

void VulkanRenderer::render()
{
    // Create the verte
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}},

    };

    const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
    auto descriptorSets = graphicPipeline->getDescriptorSets();
    camera->UpdateDescriptorSet(device->getDevice(), descriptorSets[0]);

    mesh = new Mesh(vkallocator, vertices, indices);
    mesh->load();

    auto commandBuffers = commandBuffer->getCommandBuffers();
    auto _framebuffer = framebuffers->getFramebuffers();
    for (int i = 0; i < _framebuffer.size(); ++i) {
        VulkanCommandBuffers::beginRecording(commandBuffers[i]);
        renderPass->beginRenderPass(commandBuffers[i], _framebuffer[i]);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline->getPipeline());
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &mesh->getVertexBuffer(), &offset);
        if (mesh->getIndices().empty()) {
            Logger::Info("Drawing without index");
            vkCmdDraw(commandBuffers[i], mesh->getVertices().size(), 1, 0, 0);
        } else {
            Logger::Info("Drawing with index");
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline->getLayout(), 0, 1, &descriptorSets[0], 0, nullptr);
            vkCmdBindIndexBuffer(commandBuffers[i], mesh->getIndexBuffer(), offset, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(mesh->getIndices().size()), 1, 0, 0, 0);
        }
        renderPass->endRenderPass(commandBuffers[i]);
        VulkanCommandBuffers::endRecording(commandBuffers[i]);
    }
}

void VulkanRenderer::end() { vkDeviceWaitIdle(device->getDevice()); }


VmaAllocator VulkanRenderer::createAllocator()
{
    if (context == nullptr || device == nullptr) {
        throw VulkanInitialisationException("Impossible to create the allocator");
    }
    VmaAllocatorCreateInfo allocatorInfo = {
        .physicalDevice = context->getPhysicalDevice(),
        .device = device->getDevice(),
        .instance = context->getInstance(),
        .vulkanApiVersion = VK_API_VERSION_1_1,
    };

    VmaAllocator vkallocator;
    vmaCreateAllocator(&allocatorInfo, &vkallocator);
    return vkallocator;
}
