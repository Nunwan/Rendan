#pragma once

#include "VulkanUtils.hpp"
#include <glm/vec3.hpp>
#include <vector>

struct VertexInputDescription {

    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;

    Vertex(glm::vec3 position, glm::vec3 normal);
    static VertexInputDescription getDescription();
};


class Mesh
{
private:
    VmaAllocator vmaAllocator;
    std::vector<Vertex> vertices;
    AllocatedBuffer buffer;

public:
    Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices);

    void load();

    virtual ~Mesh();

    VkBuffer& getBuffer();

    std::vector<Vertex> getVertices();

};
