cmake_minimum_required(VERSION 3.20)
project(Rendan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
include_directories(src/Vulkan)
include_directories(third_party/VulkanMemoryAllocator/include)
include_directories(third_party/stb)

# Libraries

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

set(IMGUI_DIR third_party/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
include_directories(third_party/tinyobjloader)
set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    )


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
    src/Vulkan/Buffer.cpp
    src/Vulkan/UniformBuffer.cpp
    src/Vulkan/VulkanShader.cpp
    src/Gui.cpp
    src/Vulkan/Image.cpp
    )


add_executable(Rendan ${SRC_FILES} ${IMGUI_SRC})
target_link_libraries(Rendan PRIVATE spdlog::spdlog glfw vulkan)
add_custom_target(run
    COMMAND Rendan
    DEPENDS Rendan
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
