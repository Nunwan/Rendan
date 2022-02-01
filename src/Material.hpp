#pragma once

#include "Image.hpp"
#include "VulkanPipeline.hpp"
#include <filesystem>
#include <memory>

class Material
{
protected:
    std::unique_ptr<GraphicPipeline> materialPipeline();

public:
    Material() = default;
    virtual ~Material() = default;

    virtual void CreatePipeline() = 0;

    virtual void Bind() = 0;
};
