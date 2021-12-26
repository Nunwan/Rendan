#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
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

    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline pipeline;

    void createPipeline();
    void createRenderPass();

public:
    GraphicPipeline(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);
    ~GraphicPipeline();
};
