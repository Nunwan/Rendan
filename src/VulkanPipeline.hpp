#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class VulkanShaderException : std::runtime_error
{
public:
    VulkanShaderException(const char *msg) : runtime_error(msg) {}
};


std::vector<char> readFile(const std::string &filename);


VkShaderModule createShaderModule(const std::vector<char> &code, VkDevice device, VkAllocationCallbacks *alloc);

class GraphicPipeline
{
private:
    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<VulkanDevice> device;
    std::shared_ptr<VulkanSwapchain> swapchain;
    std::shared_ptr<VulkanRenderPass> renderPass;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    void createPipeline();

public:
    GraphicPipeline(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain, std::shared_ptr<VulkanRenderPass> renderPass);
    ~GraphicPipeline();
};
