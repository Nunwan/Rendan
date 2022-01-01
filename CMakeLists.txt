cmake_minimum_required(VERSION 3.20)
project(Rendan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
include_directories(third_party/VulkanMemoryAllocator/include)

# Libraries

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

find_package(glfw3 3.3 REQUIRED)

find_package(Vulkan REQUIRED)


set(SRC_FILES
    src/Engine.cpp
    src/main.cpp
    src/Engine.hpp
    src/Logger.hpp
    src/Utils.cpp
        src/Vulkan/VulkanUtils.hpp
        src/Vulkan/VulkanContext.cpp
        src/Vulkan/VulkanDevice.cpp
        src/Vulkan/VulkanPlatform.cpp
        src/Vulkan/VulkanSwapChain.cpp
        src/Vulkan/VulkanPipeline.cpp
        src/Vulkan/VulkanRenderPass.cpp
        src/Vulkan/VulkanFramebuffers.cpp
        src/Vulkan/VulkanCommand.cpp
        src/Vulkan/VulkanSemaphore.cpp
        src/Vulkan/VulkanRenderer.cpp
        src/Vulkan/VulkanMesh.cpp
    )


add_executable(Rendan ${SRC_FILES})
target_link_libraries(Rendan PRIVATE spdlog::spdlog glfw vulkan)
