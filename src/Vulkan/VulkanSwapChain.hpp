#pragma once

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include <memory>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WANTED_FORMAT VK_FORMAT_B8G8R8A8_SRGB
#define WANTED_COLORSPACE VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
#define WANTED_PRESENT_MODE VK_PRESENT_MODE_MAILBOX_KHR

class VulkanSwapchain
{
public:
    static VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    static VkExtent2D chooseExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities);

    VulkanSwapchain(GLFWwindow *window, VulkanDevice* device);
    virtual ~VulkanSwapchain();

    VkSwapchainKHR getSwapchain();

    VkExtent2D getExtent();
    VkFormat getFormat();

    std::vector<VkImageView> getViews();

private:
    GLFWwindow *window;
    VkSwapchainKHR swapchain;
    VkFormat swapChainFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    VulkanDevice* device;


    void createSwapchain();
    void createSwapchainImageViews();
};
