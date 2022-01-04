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
    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<VulkanDevice> device;
    std::shared_ptr<VulkanSwapchain> swapchain;
    std::shared_ptr<VulkanRenderPass> renderPass;
    std::vector<VulkanUniformBuffer> uniforms;

    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;


public:
    GraphicPipeline(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain, std::shared_ptr<VulkanRenderPass> renderPass);
    ~GraphicPipeline();
    void createPipeline(VulkanShader& shader);

    VkPipeline getPipeline();
    VkPipelineLayout getLayout();
    std::vector<VkDescriptorSet>& getDescriptorSets();
};
