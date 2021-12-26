#include "VulkanPipeline.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

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


GraphicPipeline::GraphicPipeline(std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanDevice> device,
                                 std::shared_ptr<VulkanSwapchain> swapchain,
                                 std::shared_ptr<VulkanRenderPass> renderPass)
    : context(context), device(device), swapchain(swapchain), renderPass(renderPass)
{
    createPipeline();
}


void GraphicPipeline::createPipeline()
{
    // Pour le moment hardcoder :

    auto vertShaderCode = readFile("shaders/09_shader_base.vert.spv");
    auto fragShaderCode = readFile("shaders/09_shader_base.frag.spv");

    auto vertShader = createShaderModule(vertShaderCode, device->getDevice(), context->getAlloc());
    auto fragShader = createShaderModule(fragShaderCode, device->getDevice(), context->getAlloc());


    VkPipelineShaderStageCreateInfo vertShaderCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShader,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo fragShaderCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShader,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo stagesCreateInfo[] = {vertShaderCreateInfo, fragShaderCreateInfo};

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,// Optional
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,// Optional
    };

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    // Viewport & scissor
    VkViewport viewport{
        viewport.x = 0.0f,
        viewport.y = 0.0f,
        viewport.width = (float) swapchain->getExtent().width,
        viewport.height = (float) swapchain->getExtent().height,
        viewport.minDepth = 0.0f,
        viewport.maxDepth = 1.0f,
    };

    VkRect2D scissor{
        .offset = {0, 0},
        .extent = swapchain->getExtent(),
    };

    VkPipelineViewportStateCreateInfo viewportState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,// Optional
        .depthBiasClamp = 0.0f,         // Optional
        .depthBiasSlopeFactor = 0.0f,   // Optional
        .lineWidth = 1.0f,
    };

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,         // Optional
        .pSampleMask = nullptr,           // Optional
        .alphaToCoverageEnable = VK_FALSE,// Optional
        .alphaToOneEnable = VK_FALSE,     // Optional
    };

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,// Optional
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    // Dynamic states

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH,
    };

    VkPipelineDynamicStateCreateInfo dynamicState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamicStates,
    };

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,           // Optional
        .pSetLayouts = nullptr,        // Optional
        .pushConstantRangeCount = 0,   // Optional
        .pPushConstantRanges = nullptr,// Optional
    };

    if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, context->getAlloc(), &pipelineLayout) !=
        VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create pipelineLayout");
    }

    Logger::Info("Pipeline layout created");

    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = stagesCreateInfo,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = nullptr,// Optional
        .pColorBlendState = &colorBlending,
        .pDynamicState = nullptr,// Optional
        .layout = pipelineLayout,
        .renderPass = renderPass->getRenderPass(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,// Optional
        .basePipelineIndex = -1,             // Optional
    };

    if (vkCreateGraphicsPipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, context->getAlloc(),
                                  &pipeline) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create a graphic pipeline");
    }

    Logger::Info("Pipeline created");


    // Destroying shaders
    vkDestroyShaderModule(device->getDevice(), vertShader, context->getAlloc());
    vkDestroyShaderModule(device->getDevice(), fragShader, context->getAlloc());
}

GraphicPipeline::~GraphicPipeline()
{
    vkDestroyPipeline(device->getDevice(), pipeline, context->getAlloc());
    vkDestroyPipelineLayout(device->getDevice(), pipelineLayout, context->getAlloc());
}
