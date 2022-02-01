#pragma once

#include "UniformBuffer.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include <filesystem>
#include <glslang/Public/ShaderLang.h>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

enum ShaderStage
{
    VertexStage,
    FragmentStage,
};

VkShaderStageFlagBits StageToVulkanStage(ShaderStage stage);


class VulkanShaderException : public std::runtime_error
{
public:
    VulkanShaderException(const char *msg) : runtime_error(msg) {}
};


std::string readFile(const std::filesystem::path &filename);


VkShaderModule createShaderModule(const std::string &code, VkDevice device, VkAllocationCallbacks *alloc);


class VulkanShader
{
private:
    class Uniform
    {
        friend class VulkanShader;

    private:
        uint32_t binding;
        uint32_t size;
        ShaderStage stage;

    public:
        Uniform(uint32_t binding, ShaderStage stage, uint32_t size = 0) : binding(binding), stage(stage), size(size) {}
    };

    class Sampler
    {
        friend class VulkanShader;

    private:
        uint32_t binding;
        ShaderStage stage;

    public:
        Sampler(uint32_t binding, ShaderStage stage) : binding(binding), stage(stage) {}
    };


    std::map<ShaderStage, std::filesystem::path> shaderFiles;
    std::unordered_map<ShaderStage, VkShaderModule> shaderModules;
    VulkanDevice *device;

    std::unordered_map<std::string, Uniform> uniforms;
    std::unordered_map<std::string, Sampler> samplers;


    VkShaderModule CompileAndCreateShaderModule(const std::filesystem::path &sourcePath, ShaderStage stage);

    void LoadProgram(const glslang::TProgram &program, ShaderStage stage);

public:
    VulkanShader(const std::map<ShaderStage, std::filesystem::path> shaderFiles, VulkanDevice *device);


    VkShaderModule getShaderModule(ShaderStage stage);
    std::unordered_map<ShaderStage, VkShaderModule> &getShaders();
    const uint32_t getUniformBinding(std::string name) const;
    const uint32_t getSamplerBinding(std::string name) const;

    std::vector<VkDescriptorSetLayoutBinding> getDescriptorBindings();
    std::vector<VkDescriptorPoolSize> getDescriptorPoolSizes();


    virtual ~VulkanShader();
};
