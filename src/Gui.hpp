#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

class Gui
{
private:
    const VkDescriptorPoolSize pool_sizes[11] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
    };
    VulkanDevice* device;


    VkDescriptorPool descriptorPool;

public:
    Gui(VulkanDevice* device);
    void initImgui(GLFWwindow *window, VkInstance instance, VkDevice device, VkPhysicalDevice physicalDevice,
                   VkQueue graphicQueue, VkAllocationCallbacks *alloc, VkRenderPass renderPass);

    void loadFont(VkCommandBuffer cmdBuffer);
    virtual ~Gui();

    void render(VkCommandBuffer commandBuffer);
    void prepare();
};
