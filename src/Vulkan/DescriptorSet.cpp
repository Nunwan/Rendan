#include "DescriptorSet.hpp"
#include "Logger.hpp"
#include "VulkanPipeline.hpp"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>


DescriptorSet::DescriptorSet(VulkanDevice *device, const GraphicPipeline &pipeline)
    : pipelineLayout(pipeline.getPipelineLayout()), descriptorPool(pipeline.getDescriptorPool()), device(device)
{
    auto layouts = pipeline.getDescriptorSetLayout();

    VkDescriptorSetAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layouts,
    };

    auto res = vkAllocateDescriptorSets(device->getDevice(), &allocateInfo, &descriptorSet);
    if (res != VK_SUCCESS) { throw std::runtime_error("Impossible to allocate descriptorSet"); }
}

DescriptorSet::~DescriptorSet() { vkFreeDescriptorSets(device->getDevice(), descriptorPool, 1, &descriptorSet); }

void DescriptorSet::Bind(const VkCommandBuffer &commandBuffer) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0,
                            nullptr);
}

void DescriptorSet::Update(std::vector<WriteDescriptorSet> &descriptorWrites) {
    // For now, here, could be move outside to a handler
    std::vector<VkWriteDescriptorSet> descriptorWriteSet;

    for (auto& descriptorWrite : descriptorWrites) {
        auto writeDesc = descriptorWrite.getWriteDescriptor();
        writeDesc.dstSet = descriptorSet;
        descriptorWriteSet.push_back(writeDesc);
    }

    vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(descriptorWriteSet.size()), descriptorWriteSet.data(), 0, nullptr);
    Logger::Trace("descriptorWrite updated");
}
