#pragma once

#include "Descriptor.hpp"
#include "VulkanPipeline.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>



class DescriptorSet
{
private:
    VkPipelineLayout pipelineLayout;
    VkDescriptorPool descriptorPool;
    VulkanDevice* device;

    VkDescriptorSet descriptorSet;

public:
    DescriptorSet(VulkanDevice* device, const GraphicPipeline& pipeline);
    virtual ~DescriptorSet();

    void Bind(const VkCommandBuffer& commandBuffer) const;

    void Update(std::vector<WriteDescriptorSet> &descriptorWrites);
};
