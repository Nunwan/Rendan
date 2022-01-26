#include "VulkanPipeline.hpp"
#include "Logger.hpp"
#include "UniformBuffer.hpp"
#include "VulkanMesh.hpp"
#include "VulkanShader.hpp"
#include "VulkanUtils.hpp"
#include <fstream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>


GraphicPipeline::GraphicPipeline(VulkanDevice *device, VulkanSwapchain *swapchain, VulkanRenderPass *renderPass,
                                 VkPolygonMode polygonMode, VkFrontFace frontFace, VkPrimitiveTopology topology,
                                 VkCullModeFlags cullMode)
    : device(device), swapchain(swapchain), renderPass(renderPass), pipeline(VK_NULL_HANDLE), polygonMode(polygonMode),
      frontFace(frontFace), topology(topology), cullMode(cullMode)
{}


void GraphicPipeline::createPipeline(VulkanShader &shaders)
{
    // Shader creation
    auto name = "main";
    std::vector<VkPipelineShaderStageCreateInfo> stagesCreateInfo;
    for (auto &shader : shaders.getShaders()) {
        VkPipelineShaderStageCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = StageToVulkanStage(shader.first),
            .module = shader.second,
            .pName = name,
        };
        stagesCreateInfo.push_back(createInfo);
    }

    auto bindings = shaders.getDescriptorBindings();
    // Descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes{
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            static_cast<uint32_t>(swapchain->getViews().size()),
        },
        {
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            static_cast<uint32_t>(bindings.size()),
        },
    };

    // TODO(Nunwan) reflect

    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<uint32_t>(swapchain->getViews().size() + bindings.size()),
        .poolSizeCount = static_cast<uint32_t>(bindings.size()),
        .pPoolSizes = poolSizes.data(),
    };

    if (vkCreateDescriptorPool(device->getDevice(), &poolInfo, device->getAlloc(), &descriptorPool)) {
        throw VulkanInitialisationException("Impossible to create the descriptor Pool");
    }
    Logger::Info("Descriptor Pool created");

    // Descriptor layout
    VkDescriptorSetLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };

    if (vkCreateDescriptorSetLayout(device->getDevice(), &layoutInfo, device->getAlloc(), &descriptorSetLayout) !=
        VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create the descriptor layout");
    }

    // Allocate layout
    std::vector<VkDescriptorSetLayout> layouts(swapchain->getViews().size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(swapchain->getViews().size()),
        .pSetLayouts = layouts.data(),
    };

    descriptorSets.resize(swapchain->getViews().size());
    auto res = vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets.data());
    if (res != VK_SUCCESS) {
        Logger::Error(res);
        throw VulkanInitialisationException("Impossible to allocate descriptor sets");
    }


    // Vertex input

    VertexInputDescription vertexDescription = Vertex::getDescription();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexDescription.bindings.size()),
        .pVertexBindingDescriptions = vertexDescription.bindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexDescription.attributes.size()),
        .pVertexAttributeDescriptions = vertexDescription.attributes.data(),
    };

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
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
        .polygonMode = polygonMode,
        .cullMode = cullMode,
        .frontFace = frontFace,
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
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,// Optional
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
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
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, device->getAlloc(), &pipelineLayout) !=
        VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create pipelineLayout");
    }

    Logger::Info("Pipeline layout created");

    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(stagesCreateInfo.size()),
        .pStages = stagesCreateInfo.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .pDynamicState = nullptr,// Optional
        .layout = pipelineLayout,
        .renderPass = renderPass->getRenderPass(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,// Optional
        .basePipelineIndex = -1,             // Optional
    };

    if (vkCreateGraphicsPipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, device->getAlloc(),
                                  &pipeline) != VK_SUCCESS) {
        throw VulkanInitialisationException("Impossible to create a graphic pipeline");
    }

    Logger::Info("Pipeline created");
}

GraphicPipeline::~GraphicPipeline()
{
    vkDestroyPipeline(device->getDevice(), pipeline, device->getAlloc());
    vkDestroyPipelineLayout(device->getDevice(), pipelineLayout, device->getAlloc());
    vkDestroyDescriptorSetLayout(device->getDevice(), descriptorSetLayout, device->getAlloc());
    vkDestroyDescriptorPool(device->getDevice(), descriptorPool, device->getAlloc());
}

VkPipeline GraphicPipeline::getPipeline()
{
    if (pipeline == VK_NULL_HANDLE) {
        throw VulkanInitialisationException("Trying to get pipeline without having created one");
    }
    return pipeline;
}

VkPipelineLayout GraphicPipeline::getLayout() { return pipelineLayout; }


std::vector<VkDescriptorSet> &GraphicPipeline::getDescriptorSets() { return descriptorSets; }



// ===================
// GraphicPipelineCreate
// ===================







