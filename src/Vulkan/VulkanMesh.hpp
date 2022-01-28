#pragma once

#include "Buffer.hpp"
#include "VulkanUtils.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

struct VertexInputDescription {

    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv);
    static VertexInputDescription getDescription();
};

struct Shape {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct MeshConstant {
    glm::mat4 cameraMatrix;
};

Shape loadObj(std::string &pathForModelObj);


class Mesh
{
private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;


public:
    Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices);
    Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void load();

    virtual ~Mesh();

    VkBuffer getVertexBuffer();
    VkBuffer getIndexBuffer();

    std::vector<Vertex> getVertices();
    std::vector<uint32_t> getIndices();

    bool Render(VkCommandBuffer &commandBuffer);
};
