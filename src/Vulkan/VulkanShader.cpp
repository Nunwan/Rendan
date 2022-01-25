#include "VulkanShader.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <fstream>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <map>
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


std::string readFile(const std::filesystem::path &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { throw std::runtime_error("Impossible to read the file"); }

    size_t filesize = static_cast<size_t>(file.tellg());
    std::string buffer;
    buffer.resize(filesize);
    file.seekg(0);
    file.read(buffer.data(), filesize);
    file.close();
    return buffer;
}


VkShaderModule createShaderModule(const std::string &code, VkDevice device, VkAllocationCallbacks *alloc)
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

EShLanguage GetEshLanguage(VkShaderStageFlags stageFlag)
{
    switch (stageFlag) {
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;
        default:
            return EShLangCount;
    }
}

TBuiltInResource GetResources()
{
    TBuiltInResource resources = {};
    resources.maxLights = 32;
    resources.maxClipPlanes = 6;
    resources.maxTextureUnits = 32;
    resources.maxTextureCoords = 32;
    resources.maxVertexAttribs = 64;
    resources.maxVertexUniformComponents = 4096;
    resources.maxVaryingFloats = 64;
    resources.maxVertexTextureImageUnits = 32;
    resources.maxCombinedTextureImageUnits = 80;
    resources.maxTextureImageUnits = 32;
    resources.maxFragmentUniformComponents = 4096;
    resources.maxDrawBuffers = 32;
    resources.maxVertexUniformVectors = 128;
    resources.maxVaryingVectors = 8;
    resources.maxFragmentUniformVectors = 16;
    resources.maxVertexOutputVectors = 16;
    resources.maxFragmentInputVectors = 15;
    resources.minProgramTexelOffset = -8;
    resources.maxProgramTexelOffset = 7;
    resources.maxClipDistances = 8;
    resources.maxComputeWorkGroupCountX = 65535;
    resources.maxComputeWorkGroupCountY = 65535;
    resources.maxComputeWorkGroupCountZ = 65535;
    resources.maxComputeWorkGroupSizeX = 1024;
    resources.maxComputeWorkGroupSizeY = 1024;
    resources.maxComputeWorkGroupSizeZ = 64;
    resources.maxComputeUniformComponents = 1024;
    resources.maxComputeTextureImageUnits = 16;
    resources.maxComputeImageUniforms = 8;
    resources.maxComputeAtomicCounters = 8;
    resources.maxComputeAtomicCounterBuffers = 1;
    resources.maxVaryingComponents = 60;
    resources.maxVertexOutputComponents = 64;
    resources.maxGeometryInputComponents = 64;
    resources.maxGeometryOutputComponents = 128;
    resources.maxFragmentInputComponents = 128;
    resources.maxImageUnits = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resources.maxCombinedShaderOutputResources = 8;
    resources.maxImageSamples = 0;
    resources.maxVertexImageUniforms = 0;
    resources.maxTessControlImageUniforms = 0;
    resources.maxTessEvaluationImageUniforms = 0;
    resources.maxGeometryImageUniforms = 0;
    resources.maxFragmentImageUniforms = 8;
    resources.maxCombinedImageUniforms = 8;
    resources.maxGeometryTextureImageUnits = 16;
    resources.maxGeometryOutputVertices = 256;
    resources.maxGeometryTotalOutputComponents = 1024;
    resources.maxGeometryUniformComponents = 1024;
    resources.maxGeometryVaryingComponents = 64;
    resources.maxTessControlInputComponents = 128;
    resources.maxTessControlOutputComponents = 128;
    resources.maxTessControlTextureImageUnits = 16;
    resources.maxTessControlUniformComponents = 1024;
    resources.maxTessControlTotalOutputComponents = 4096;
    resources.maxTessEvaluationInputComponents = 128;
    resources.maxTessEvaluationOutputComponents = 128;
    resources.maxTessEvaluationTextureImageUnits = 16;
    resources.maxTessEvaluationUniformComponents = 1024;
    resources.maxTessPatchComponents = 120;
    resources.maxPatchVertices = 32;
    resources.maxTessGenLevel = 64;
    resources.maxViewports = 16;
    resources.maxVertexAtomicCounters = 0;
    resources.maxTessControlAtomicCounters = 0;
    resources.maxTessEvaluationAtomicCounters = 0;
    resources.maxGeometryAtomicCounters = 0;
    resources.maxFragmentAtomicCounters = 8;
    resources.maxCombinedAtomicCounters = 8;
    resources.maxAtomicCounterBindings = 1;
    resources.maxVertexAtomicCounterBuffers = 0;
    resources.maxTessControlAtomicCounterBuffers = 0;
    resources.maxTessEvaluationAtomicCounterBuffers = 0;
    resources.maxGeometryAtomicCounterBuffers = 0;
    resources.maxFragmentAtomicCounterBuffers = 1;
    resources.maxCombinedAtomicCounterBuffers = 1;
    resources.maxAtomicCounterBufferSize = 16384;
    resources.maxTransformFeedbackBuffers = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances = 8;
    resources.maxCombinedClipAndCullDistances = 8;
    resources.maxSamples = 4;
    resources.limits.nonInductiveForLoops = true;
    resources.limits.whileLoops = true;
    resources.limits.doWhileLoops = true;
    resources.limits.generalUniformIndexing = true;
    resources.limits.generalAttributeMatrixVectorIndexing = true;
    resources.limits.generalVaryingIndexing = true;
    resources.limits.generalSamplerIndexing = true;
    resources.limits.generalVariableIndexing = true;
    resources.limits.generalConstantMatrixVectorIndexing = true;
    return resources;
}

VkShaderModule CompileAndCreateShaderModule(const std::filesystem::path &sourcePath, VkShaderStageFlags stage,
                                            VkDevice device, VkAllocationCallbacks *alloc)
{
    glslang::InitializeProcess();
    auto language = GetEshLanguage(stage);
    glslang::TProgram program;
    glslang::TShader shader(language);
    auto resources = GetResources();

    auto glMessages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);
    if (isInDebug()) { glMessages = static_cast<EShMessages>(glMessages | EShMsgDebugInfo); }

    auto shaderName = sourcePath.string();
    auto shaderNameC = shaderName.c_str();
    auto shaderSource = readFile(sourcePath);
    auto shaderSourceC = shaderSource.c_str();
    shader.setStrings(&shaderSourceC, 1);

    auto defaultVersion = glslang::EShTargetOpenGL_450;
    shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, 120);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
    // Hardcoded for Vulkan 1.1
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);


    /*if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, glMessages, &str, includer)) {
        Logger::Info(shader.getInfoLog());
        Logger::Info(shader.getInfoDebugLog());
        throw VulkanShaderException("Impossible to preprocess SPRIV");
    }*/

    if (!shader.parse(&resources, defaultVersion, false, glMessages)) {
        Logger::Info(shader.getInfoLog());
        Logger::Info(shader.getInfoDebugLog());
        throw VulkanShaderException("SPRIV shader parse failed!");
    }

    program.addShader(&shader);

    if (!program.link(glMessages) || !program.mapIO()) {
        throw VulkanShaderException("Error while linking shader program");
    }

    program.buildReflection();

    glslang::SpvOptions spvOptions;
#ifndef NDEBUG
    spvOptions.generateDebugInfo = true;
    spvOptions.disableOptimizer = true;
    spvOptions.optimizeSize = false;
#else
    spvOptions.generateDebugInfo = false;
    spvOptions.disableOptimizer = false;
    spvOptions.optimizeSize = true;
#endif
    spv::SpvBuildLogger logger;
    std::vector<uint32_t> spirv;
    GlslangToSpv(*program.getIntermediate(static_cast<EShLanguage>(language)), spirv, &logger, &spvOptions);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = spirv.size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode = spirv.data();

    VkShaderModule shaderModule;
    auto res = vkCreateShaderModule(device, &shaderModuleCreateInfo, alloc, &shaderModule);
    if (res != VK_SUCCESS) {}


    return shaderModule;
}


VulkanShader::VulkanShader(const std::map<ShaderStage, std::filesystem::path> shaderFiles, VulkanDevice *device)
    : device(device), shaderFiles(shaderFiles)
{
    for (const auto &shader : shaderFiles) {
        shaderModules.insert(
            {shader.first, CompileAndCreateShaderModule(shader.second, StageToVulkanStage(shader.first),
                                                        device->getDevice(), device->getAlloc())});
        Logger::Info("Create shader module from file : " + shader.second.string());
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

    for (const auto &sampler : samplers) {
        VkDescriptorSetLayoutBinding binding{
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = static_cast<uint32_t>(sampler.second.size()),
            .stageFlags = StageToVulkanStage(sampler.first),
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


void VulkanShader::addSampler(ShaderStage stage)
{
    if (samplers.count(stage) == 0) { samplers.insert({stage, std::vector<Sampler>(0)}); }
    samplers[stage].push_back(Sampler());
}
