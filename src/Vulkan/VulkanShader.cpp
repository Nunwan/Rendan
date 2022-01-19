#include "VulkanShader.hpp"
#include "Logger.hpp"
#include <fstream>
#include <stdexcept>
#include <vector>


VkShaderStageFlagBits StageToVulkanStage(ShaderStage stage)
{
    switch (stage) {
        case VertexStage:
            return VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case FragmentStage:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            throw std::runtime_error("Impossible to convert Shader stage into a vulkan stage");
            break;
    }
}


std::vector<char> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { throw std::runtime_error("Impossible to read the file"); }

    size_t filesize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(filesize);
    file.seekg(0);
    file.read(buffer.data(), filesize);
    file.close();
    return buffer;
}


VkShaderModule createShaderModule(const std::vector<char> &code, VkDevice device, VkAllocationCallbacks *alloc)
{
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data()),
    };
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, alloc, &shaderModule) != VK_SUCCESS) {
        throw VulkanShaderException("Impossible to create the shader module");
    }
    return shaderModule;
}

VulkanShader::VulkanShader(const std::unordered_map<ShaderStage, std::string> shaderFiles,
                           std::shared_ptr<VulkanDevice> device)
    : device(device), shaderFiles(shaderFiles)
{
    for (const auto &shader : shaderFiles) {
        Logger::Info("Create shader module from file : " + shader.second);
        auto code = readFile(shader.second);
        shaderModules.insert({shader.first, createShaderModule(code, device->getDevice(), device->getAlloc())});
    }
}

VulkanShader::~VulkanShader()
{
    for (const auto &shader : shaderModules) {
        vkDestroyShaderModule(device->getDevice(), shader.second, device->getAlloc());
    }
}

VkShaderModule VulkanShader::getShaderModule(ShaderStage stage)
{
    if (shaderModules.count(stage) == 0) { throw VulkanShaderException("Shader not found for this stage"); }
    return shaderModules[stage];
}

std::unordered_map<ShaderStage, VkShaderModule> &VulkanShader::getShaders() { return shaderModules; }

std::vector<VkDescriptorSetLayoutBinding> VulkanShader::getDescriptorBindings()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    for (const auto &uniform : uniforms) {
        VkDescriptorSetLayoutBinding binding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = static_cast<uint32_t>(uniform.second.size()),
            .stageFlags = StageToVulkanStage(uniform.first),
            .pImmutableSamplers = nullptr,
        };
        bindings.push_back(binding);
    }

    return bindings;
}


void VulkanShader::addUniform(ShaderStage stage, uint32_t size)
{
    if (uniforms.count(stage) == 0) { uniforms.insert({stage, std::vector<Uniform>(0)}); }
    uniforms[stage].push_back(Uniform(size));
}
