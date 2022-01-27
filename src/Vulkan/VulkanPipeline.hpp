#pragma once

#include "UniformBuffer.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanShader.hpp"
#include "VulkanSwapChain.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>


class GraphicPipeline
{
private:
    VulkanDevice *device;
    VulkanSwapchain *swapchain;
    VulkanRenderPass *renderPass;
    std::vector<VulkanUniformBuffer> uniforms;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    std::unique_ptr<VulkanShader> shaders;

    std::vector<VkPipelineShaderStageCreateInfo> stagesCreateInfo;


    VkPolygonMode polygonMode;
    VkFrontFace frontFace;
    VkPrimitiveTopology topology;
    VkCullModeFlags cullMode;

    void createShaders();
    void createDescriptorPool();
    void createPipeline();
    void createPipelineLayout();


public:
    GraphicPipeline(VulkanDevice *device, VulkanSwapchain *swapchain, VulkanRenderPass *renderPass,
                    VkPolygonMode polygonMode, VkFrontFace frontFace, VkPrimitiveTopology topology,
                    VkCullModeFlags cullMode, const std::map<ShaderStage, std::filesystem::path> shaderFiles);
    ~GraphicPipeline();

    VkPipeline getPipeline();
    
    void BindPipeline(VkCommandBuffer& commandBuffer);

    const VkPipelineLayout& getPipelineLayout() const;

    const VkDescriptorPool &getDescriptorPool() const;
    const VkDescriptorSetLayout &getDescriptorSetLayout() const;
};


class GraphicPipelineCreate
{
public:
    GraphicPipelineCreate(VulkanDevice *device, VulkanSwapchain *swapchain, VulkanRenderPass *renderPass)
        : device(device), swapchain(swapchain), renderPass(renderPass)
    {}

    GraphicPipeline *Create(const std::map<ShaderStage, std::filesystem::path> shaderFiles,
                            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
                            VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                            VkCullModeFlags cullMode = VK_CULL_MODE_NONE)
    {
        return new GraphicPipeline(device, swapchain, renderPass, polygonMode, frontFace, topology, cullMode,
                                   shaderFiles);
    }

private:
    VulkanDevice *device;
    VulkanSwapchain *swapchain;
    VulkanRenderPass *renderPass;
};
