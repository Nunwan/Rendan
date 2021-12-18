#define DEBUG
#include "VulkanRenderer.hpp"

#include <vulkan/vulkan_core.h>

#include <cstring>
#include <vector>

#include "Utils.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"

std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

void AssertSuccess(VkResult result) {
  if (result != VK_SUCCESS) {
    throw new VulkanInitialisationException("General initialisation of Vulkan failed");
  }
}

bool areWantedLayersDisponible() {
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

std::vector<const char*> getRequiredExtensions() {
  uint32_t extensionsCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionsCount);
  if (isInDebug()) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                            void *pUserData) {
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
    default:
      break;

  }

  return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
  createInfo = {};
  createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo->pfnUserCallback = debugCallback;
}





VulkanDevice_t initVulkanStruct() {
  VulkanDevice_t vulkanProps;
  vulkanProps.alloc = nullptr;
  return vulkanProps;
}

VkInstance creationVkInstance() {
  if (!areWantedLayersDisponible()) {
    throw new VulkanInitialisationException("Vulkan needs layers that are not available");
  }
  VkInstance instance;

  // Creation of the application
  VkApplicationInfo appInfo{};
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  appInfo.pApplicationName = "Rendan Renderer";
  appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.pEngineName = "Rendan";

  // Creation of the instance
  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = nullptr;
  instanceInfo.pApplicationInfo = &appInfo;

  // Layers ; Release mode is hardcoded as without layer
  if (isInDebug()) {
  instanceInfo.enabledLayerCount = validationLayers.size();
  instanceInfo.ppEnabledExtensionNames = validationLayers.data();
  } else {
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
  }

  // Extension support
  auto extensions = getRequiredExtensions();
  instanceInfo.enabledExtensionCount = extensions.size();
  instanceInfo.ppEnabledExtensionNames = extensions.data();
  if (isInDebug()) {
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    populateDebugMessengerCreateInfo(&debugCreateInfo);
    instanceInfo.pNext = &debugCreateInfo;
  }


  auto result = vkCreateInstance(&instanceInfo, nullptr, &instance);
  // TODO(Nunwan) better support of VkResult ?
  if (result != VK_SUCCESS) {
    throw new VulkanInitialisationException("Vulkan instance creation failed");
  }
  return instance;
}






VulkanDevice_t InitVulkan() {
  auto vDevice = initVulkanStruct();
  vDevice.instance = creationVkInstance();

  return vDevice;
}
