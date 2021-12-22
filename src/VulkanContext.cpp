#include "VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

#include <cstring>
#include <stdexcept>
#include <vector>

#include "Logger.hpp"
#include "Utils.hpp"
#include "VulkanUtils.hpp"

// ===================
// Layers and debugging
// ===================

std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};


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
    for (auto layer : availableLayer) {
        for (auto wantedLayer : validationLayers) {
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
        throw new VulkanInitialisationException("Impossible to create the debug messenger");
    }
}

// ===================
// Instance
// ===================

std::vector<const char *> getRequiredExtensions()
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
        throw new VulkanInitialisationException(
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
    auto extensions = getRequiredExtensions();
    instanceInfo.enabledExtensionCount = extensions.size();
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
        throw new VulkanInitialisationException(
            "Vulkan instance creation failed");
    }
}


// TODO(Nunwan)
bool isDeviceSuitable(VkPhysicalDevice physicalDevice) { return true; }

void VulkanContext::getSuitablePhysicalDevice()
{
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                               nullptr);

    if (physicalDeviceCount == 0) {
        throw new VulkanInitialisationException("No device for Vulkan found");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                               physicalDevices.data());

    for (auto physicalDevice : physicalDevices) {
        if (isDeviceSuitable(physicalDevice)) {
            this->physicalDevice = physicalDevice;
            return;
        }
    }

    throw new std::runtime_error("Impossible to find a suitable device");
}


VulkanContext::VulkanContext() : alloc(nullptr)
{
    createVkInstance();
    setupDebugMessenger();
    getSuitablePhysicalDevice();
    Logger::Info("Creation of the context (instance, debug and physicalDevice)");
}

VulkanContext::~VulkanContext()
{
    if (isInDebug()) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, alloc);
    }
    vkDestroyInstance(instance, alloc);
    Logger::Info("Context destroying");
}
