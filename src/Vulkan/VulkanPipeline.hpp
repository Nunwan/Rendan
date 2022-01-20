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
    VulkanDevice* device;
    VulkanSwapchain* swapchain;
    VulkanRenderPass* renderPass;
    std::vector<VulkanUniformBuffer> uniforms;

    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;


public:
    GraphicPipeline(VulkanDevice* device, VulkanSwapchain* swapchain,
                    VulkanRenderPass* renderPass);
    ~GraphicPipeline();
    void createPipeline(VulkanShader &shader);

    VkPipeline getPipeline();
    VkPipelineLayout getLayout();
    std::vector<VkDescriptorSet> &getDescriptorSets();
};
