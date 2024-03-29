cmake_minimum_required(VERSION 3.20)
project(Rendan)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)

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
    src/VulkanUtils.hpp
    src/VulkanRenderer.cpp)


add_executable(Rendan ${SRC_FILES})
target_link_libraries(Rendan PRIVATE spdlog::spdlog glfw vulkan)
