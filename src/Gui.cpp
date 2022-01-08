#include "Gui.hpp"
#include "VulkanContext.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>


Gui::Gui(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanContext> context)
    : device(device), context(context)
{
    VkDescriptorPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1000,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes)),
        .pPoolSizes = pool_sizes,
    };

    if (vkCreateDescriptorPool(device->getDevice(), &createInfo, context->getAlloc(), &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Impossible to create descriptor pool for imgui");
    }
}


void Gui::initImgui(GLFWwindow *window, VkInstance instance, VkDevice device, VkPhysicalDevice physicalDevice,
                    VkQueue graphicQueue, VkAllocationCallbacks *alloc, VkRenderPass renderPass)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo initInfo{
        .Instance = instance,
        .PhysicalDevice = physicalDevice,
        .Device = device,
        .Queue = graphicQueue,
        .DescriptorPool = descriptorPool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .Allocator = alloc,
    };
    ImGui_ImplVulkan_Init(&initInfo, renderPass);
}


void Gui::loadFont(VkCommandBuffer cmdBuffer) { ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer); }

Gui::~Gui()
{
    vkDestroyDescriptorPool(device->getDevice(), descriptorPool, context->getAlloc());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}