#include "VulkanContext.hpp"

#include <cstdint>
#include <memory>
#include <vulkan/vulkan_core.h>

#include <cstring>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "Logger.hpp"
#include "Utils.hpp"
#include "VulkanPlatform.hpp"
#include "VulkanUtils.hpp"

// ===================
// Layers and debugging
// ===================

std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


bool areWantedLayersDisponible()
{

    // Release mode does not have layers
    if (!isInDebug()) return true;

    // Check of the number of layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (layerCount == 0) return false;

    // Verification of the validity of the wanted layers
    std::vector<VkLayerProperties> availableLayer(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayer.data());
    int nbLayerFound = 0;
    for (auto &layer : availableLayer) {
        for (auto &wantedLayer : validationLayers) {
            if (strcmp(wantedLayer, layer.layerName) == 0) {
                nbLayerFound++;
            }
        }
    }

    return nbLayerFound == validationLayers.size();
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData)
{
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Logger::Error(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Logger::Warn(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Logger::Trace(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Logger::Info(pCallbackData->pMessage);
            break;
        default:
            break;
    }

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT *createInfo)
{
    if (createInfo == nullptr) {
        Logger::Error("Impossible to populate vulkan Debug Info");
        return;
    }
    *createInfo = {};
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void VulkanContext::setupDebugMessenger()
{
    if (!isInDebug()) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, alloc, &debugMessenger) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create the debug messenger");
    }
}

// ===================
// Instance
// ===================

std::vector<const char *> VulkanContext::getRequiredExtensions()
{
    uint32_t extensionsCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + extensionsCount);
    if (isInDebug()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}


void VulkanContext::createVkInstance()
{
    if (!areWantedLayersDisponible()) {
        throw VulkanInitialisationException(
            "Vulkan needs layers that are not available");
    }

    // Creation of the application
    VkApplicationInfo appInfo{
        .pApplicationName = "Rendan Renderer",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "Rendan",
        .engineVersion = VK_MAKE_VERSION(0, 0, 0),
        .apiVersion = VK_API_VERSION_1_2,
    };

    // Creation of the instance
    VkInstanceCreateInfo instanceInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &appInfo,
    };

    // Layers ; Release mode is hardcoded as without layer
    if (isInDebug()) {
        instanceInfo.enabledLayerCount = validationLayers.size();
        instanceInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = nullptr;
    }

    // Extension support
    std::vector<const char *> extensions = getRequiredExtensions();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    if (isInDebug()) {
        Logger::Info("Create Debugging callback");
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        populateDebugMessengerCreateInfo(&debugCreateInfo);
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    }

    auto result = vkCreateInstance(&instanceInfo, alloc, &instance);
    // TODO(Nunwan) better support of VkResult ?
    if (result != VK_SUCCESS) {
        throw VulkanInitialisationException(
            "Vulkan instance creation failed");
    }
}

bool checkExtensionsSupport(const VkPhysicalDevice &physicalDevice)
{
    uint32_t deviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
    if (deviceExtensionCount == 0 && deviceExtensions.size() != deviceExtensionCount)
        throw VulkanInitialisationException("There is no extensions supported");

    std::vector<VkExtensionProperties> extensionsProperties(deviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, extensionsProperties.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto &extension : extensionsProperties) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, nullptr);
    if (presentCount != 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, details.presentModes.data());
    }
    return details;
}


bool isDeviceSuitable(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface)
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    bool extensionSupported = checkExtensionsSupport(physicalDevice);
    bool swapChainAdequate = false;
    if (extensionSupported) {
        auto swDetail = querySwapChainSupport(physicalDevice, surface);
        swapChainAdequate = !swDetail.formats.empty() && !swDetail.presentModes.empty();
    }
    return indices.isComplete() && extensionSupported && swapChainAdequate;
}

void VulkanContext::getSuitablePhysicalDevice()
{
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                               nullptr);

    if (physicalDeviceCount == 0) {
        throw VulkanInitialisationException("No device for Vulkan found");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                               physicalDevices.data());

    for (auto &physicalDevice : physicalDevices) {
        if (isDeviceSuitable(physicalDevice, platform->getSurface())) {
            this->physicalDevice = physicalDevice;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Impossible to find a suitable device");
}


VulkanContext::VulkanContext(GLFWwindow *window) : alloc(nullptr), physicalDevice(VK_NULL_HANDLE)
{
    createVkInstance();
    setupDebugMessenger();
    platform = std::make_shared<VulkanPlatform>(instance, alloc);
    platform->createSurface(window);
    getSuitablePhysicalDevice();
    Logger::Info("Vulkan Context created");
}

VulkanContext::~VulkanContext()
{
    if (isInDebug()) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, alloc);
    }
    platform.reset();
    vkDestroyInstance(instance, alloc);
    Logger::Info("Context destroying");
}


VkInstance &VulkanContext::getInstance()
{
    return instance;
}

VkPhysicalDevice &VulkanContext::getPhysicalDevice()
{
    return physicalDevice;
}


VkAllocationCallbacks *VulkanContext::getAlloc()
{
    return alloc;
}

std::vector<const char *> VulkanContext::getLayers()
{
    if (isInDebug())
        return validationLayers;

    return std::vector<const char *>(0);
}


QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface)
{
    if (physicalDevice == VK_NULL_HANDLE)
        throw VulkanInitialisationException("Impossible to find queues without physicalDevice");
    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);

    if (familyCount == 0)
        throw VulkanInitialisationException("There is no queue available for this physical device");

    std::vector<VkQueueFamilyProperties> queueProperties(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueProperties.data());

    QueueFamilyIndices indices;

    int i = 0;
    for (auto &queueProperty : queueProperties) {
        if (queueProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics = i;
        }
        VkBool32 presentMode = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentMode);
        if (presentMode) {
            indices.presents = i;
        }
        ++i;
    }
    return indices;
}

VkSurfaceKHR &VulkanContext::getSurface()
{
    if (platform == nullptr) {
        throw VulkanInitialisationException("Impossible to retrieve surface as platform is not initialiazed");
    }
    auto surface = platform->getSurface();
    if (surface == VK_NULL_HANDLE) {
        throw VulkanInitialisationException("Surface is not created");
    }
    return platform->getSurface();
}

std::vector<const char *> VulkanContext::getDeviceExtensions()
{
    return deviceExtensions;
}
