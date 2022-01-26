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

    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;


    VkPolygonMode polygonMode;
    VkFrontFace frontFace;
    VkPrimitiveTopology topology;
    VkCullModeFlags cullMode;


public:
    GraphicPipeline(VulkanDevice *device, VulkanSwapchain *swapchain, VulkanRenderPass *renderPass,
                    VkPolygonMode polygonMode, VkFrontFace frontFace, VkPrimitiveTopology topology,
                    VkCullModeFlags cullMode);
    ~GraphicPipeline();
    void createPipeline(VulkanShader &shader);

    VkPipeline getPipeline();
    VkPipelineLayout getLayout();
    std::vector<VkDescriptorSet> &getDescriptorSets();
};


class GraphicPipelineCreate
{
public:
    GraphicPipelineCreate(VulkanDevice *device, VulkanSwapchain *swapchain, VulkanRenderPass *renderPass)
        : device(device), swapchain(swapchain), renderPass(renderPass)
    {}

    GraphicPipeline *Create(VulkanShader &shader, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
                            VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                            VkCullModeFlags cullMode = VK_CULL_MODE_NONE)
    {
        auto newPipeline =
            new GraphicPipeline(device, swapchain, renderPass, polygonMode, frontFace, topology, cullMode);
        newPipeline->createPipeline(shader);
        return newPipeline;
    }

private:
    VulkanDevice *device;
    VulkanSwapchain *swapchain;
    VulkanRenderPass *renderPass;
};
