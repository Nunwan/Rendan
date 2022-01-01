#pragma once

#include "VulkanUtils.hpp"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
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

struct MeshConstant {
    glm::mat4 cameraMatrix;
};


class Mesh
{
private:
    VmaAllocator vmaAllocator;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;

public:
    Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices);
    Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void load();

    virtual ~Mesh();

    VkBuffer& getVertexBuffer();
    VkBuffer& getIndexBuffer();

    std::vector<Vertex> getVertices();
    std::vector<uint32_t> getIndices();

};
