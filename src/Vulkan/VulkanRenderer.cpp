#include "VulkanRenderer.hpp"
#include "Gui.hpp"
#include "Image.hpp"
#include "Logger.hpp"
#include "UniformBuffer.hpp"
#include "VulkanMesh.hpp"
#include "VulkanShader.hpp"
#include "VulkanUtils.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <string>
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
        device = new VulkanDevice(window);
        vkallocator = createAllocator();
        semaphores = new VulkanSemaphores(device);
        swapchain = new VulkanSwapchain(window, device);
        renderPass = new VulkanRenderPass(device, swapchain);
        framebuffers = new VulkanFramebuffers(device, swapchain, renderPass);

        // Shaders
        std::unordered_map<ShaderStage, std::string> shaderFiles = {
            {ShaderStage::VertexStage, "shaders/09_shader_base.vert.spv"},
            {ShaderStage::FragmentStage, "shaders/09_shader_base.frag.spv"}};
        VulkanShader shaders = VulkanShader(shaderFiles, device);
        for (int i = 0; i < swapchain->getViews().size(); i++) {
            cameras.push_back(new VulkanUniformBuffer(vkallocator, sizeof(MeshConstant), nullptr));
        }
        shaders.addUniform(ShaderStage::VertexStage, sizeof(MeshConstant));
        shaders.addSampler(ShaderStage::FragmentStage);

        graphicPipeline = new GraphicPipeline(device, swapchain, renderPass);
        graphicPipeline->createPipeline(shaders);


        commandPool = new VulkanCommandPool(device);
        commandBuffer = new VulkanCommandBuffers(device, framebuffers, commandPool);

        std::string imagePath = std::string("../textures/viking_room.png");
        LoadedImage texture = Image::load(imagePath);

        loadedImage = new Image(vkallocator, device, commandBuffer, texture.width, texture.height);
        loadedImage->write(texture.pixels, texture.height * texture.width * 4);
        Image::unload(texture);
        loadedImage->createImageView();

        sampler = new VulkanSampler(device, loadedImage);

        gui = std::make_unique<Gui>(device);
        gui->initImgui(window, device->getInstance(), device->getDevice(), device->getPhysicalDevice(),
                       device->getGraphicQueue(), device->getAlloc(), renderPass->getRenderPass());


    } catch (VulkanInitialisationException &e) {
        Logger::Error(e.what());
        throw std::runtime_error("Impossible to initialiaze Vulkan");
    }
}

VulkanRenderer::~VulkanRenderer()
{
    gui.reset();
    for (const auto &camera : cameras) { delete camera; }
    delete sampler;
    delete loadedImage;
    delete mesh;
    delete commandBuffer;
    delete commandPool;
    delete graphicPipeline;
    delete framebuffers;
    delete renderPass;
    delete swapchain;
    delete semaphores;
    vmaDestroyAllocator(vkallocator);
    delete device;
}

void VulkanRenderer::updateUniforms(uint32_t imageIndex)
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
    cameras[imageIndex]->update(&newCamera);
}

void VulkanRenderer::render()
{
    gui->prepare();



    uint32_t imageIndex;
    auto commandBuffers = commandBuffer->getCommandBuffers();
    auto _framebuffers = framebuffers->getFramebuffers();
    vkAcquireNextImageKHR(device->getDevice(), swapchain->getSwapchain(), UINT64_MAX,
                          semaphores->getAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);


    updateUniforms(imageIndex);

    // render

    if (vkResetCommandPool(device->getDevice(), commandPool->getCommandPool(), 0) != VK_SUCCESS) {
        throw std::runtime_error("Impossible to reset command pool");
    }
    auto currentCmdBuffer = commandBuffers[imageIndex];
    auto currentFrameBuffer = _framebuffers[imageIndex];
    auto descriptorSets = graphicPipeline->getDescriptorSets();
    VulkanCommandBuffers::beginRecording(currentCmdBuffer);
    renderPass->beginRenderPass(currentCmdBuffer, currentFrameBuffer);

    gui->render(currentCmdBuffer);

    cameras[imageIndex]->UpdateDescriptorSet(device->getDevice(), descriptorSets[imageIndex]);
    sampler->UpdateDescriptorSet(descriptorSets[imageIndex], loadedImage->getImageView());


    vkCmdBindPipeline(currentCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline->getPipeline());

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(currentCmdBuffer, 0, 1, &mesh->getVertexBuffer(), &offset);
    if (mesh->getIndices().empty()) {
        Logger::Trace("Drawing without index");
        vkCmdDraw(currentCmdBuffer, mesh->getVertices().size(), 1, 0, 0);
    } else {
        Logger::Trace("Drawing with index");
        vkCmdBindDescriptorSets(currentCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline->getLayout(), 0, 1,
                                &descriptorSets[imageIndex], 0, nullptr);
        vkCmdBindIndexBuffer(currentCmdBuffer, mesh->getIndexBuffer(), offset, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(currentCmdBuffer, static_cast<uint32_t>(mesh->getIndices().size()), 1, 0, 0, 0);
    }


    renderPass->endRenderPass(currentCmdBuffer);
    VulkanCommandBuffers::endRecording(currentCmdBuffer);


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


    // Present

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

void VulkanRenderer::load()
{
    // Create the verte
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    };

    const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
    auto descriptorSets = graphicPipeline->getDescriptorSets();

    std::string pathModel = "../models/viking_room.obj";
    MeshFromObj meshObj = loadObj(pathModel);
    mesh = new Mesh(vkallocator, meshObj.vertices, meshObj.indices);
    mesh->load();

    auto fontCmdBuffer = commandBuffer->beginSingleTimeCommands();
    gui->loadFont(fontCmdBuffer);
    commandBuffer->endSingleTimeCommands(fontCmdBuffer);
}

void VulkanRenderer::end() { vkDeviceWaitIdle(device->getDevice()); }


VmaAllocator VulkanRenderer::createAllocator()
{
    if (device == nullptr) {
        throw VulkanInitialisationException("Impossible to create the allocator");
    }
    VmaAllocatorCreateInfo allocatorInfo = {
        .physicalDevice = device->getPhysicalDevice(),
        .device = device->getDevice(),
        .instance = device->getInstance(),
        .vulkanApiVersion = VK_API_VERSION_1_1,
    };

    VmaAllocator vkallocator;
    vmaCreateAllocator(&allocatorInfo, &vkallocator);
    return vkallocator;
}
