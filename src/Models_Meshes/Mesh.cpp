#include "Mesh.hpp"
#include "Logger.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) : position(position), normal(normal), uv(uv) {}

Mesh::Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices)
    : Mesh(vmaAllocator, vertices, std::vector<uint32_t>(0))
{}

Mesh::Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : vertices(vertices), indices(indices)
{
    vertexBuffer = std::make_unique<Buffer>(vmaAllocator, vertices.size() * sizeof(Vertex),
                                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    if (!indices.empty()) {
        indexBuffer = std::make_unique<Buffer>(vmaAllocator, indices.size() * sizeof(uint32_t),
                                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU);
    }
}

Shape loadObj(std::string &pathForModelObj)
{
    Shape mesh;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathForModelObj.c_str())) {
        Logger::Warn(warn);
        Logger::Error(err);
        throw std::runtime_error("Impossible to load models from obj");
    }

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
            };
            vertex.uv = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
            };
            vertex.normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2],
            };

            mesh.vertices.push_back(vertex);
            mesh.indices.push_back(mesh.indices.size());
        }
    }
    Logger::Info("Model loaded from file" + pathForModelObj);
    return mesh;
}


Mesh::~Mesh() {}

void Mesh::load()
{
    vertexBuffer->update(vertices.data());
    if (!indices.empty()) { indexBuffer->update(indices.data()); }
}

VulkanVertexInputDescription Vertex::getVulkanDescription()
{
    VulkanVertexInputDescription description{};

    VkVertexInputBindingDescription mainBinding{
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    description.bindings.push_back(mainBinding);

    VkVertexInputAttributeDescription positionAttr{
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, position),
    };

    VkVertexInputAttributeDescription normalAttr{
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, normal),
    };


    VkVertexInputAttributeDescription uvAttr{
        .location = 2,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, uv),
    };

    description.attributes.push_back(positionAttr);
    description.attributes.push_back(normalAttr);
    description.attributes.push_back(uvAttr);

    return description;
}

Buffer& Mesh::getVertexBuffer() { return *vertexBuffer; }

Buffer& Mesh::getIndexBuffer() { return *indexBuffer; }

std::vector<Vertex> Mesh::getVertices() { return vertices; }

std::vector<uint32_t> Mesh::getIndices() { return indices; }

bool Mesh::Render(CommandBuffer &commandBuffer)
{
    auto cmdBuffer = commandBuffer.getCommandBuffer();

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffer->getBufferPtr(), &offset);
    if (indices.empty()) {
        vkCmdDraw(cmdBuffer, vertices.size(), 1, 0, 0);
    } else {
        vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->getBuffer(), offset, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }
    return true;
}
