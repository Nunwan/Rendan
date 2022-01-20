#include "VulkanMesh.hpp"
#include "Logger.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) : position(position), normal(normal), uv(uv) {}

Mesh::Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices)
    : Mesh(vmaAllocator, vertices, std::vector<uint32_t>(0))
{}

Mesh::Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : vmaAllocator(vmaAllocator), vertices(vertices), indices(indices)
{

    // Vertex creation buffer
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = vertices.size() * sizeof(Vertex),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    };

    VmaAllocationCreateInfo allocationInfo{
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU,
    };

    auto result = vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocationInfo, &vertexBuffer.buffer,
                                  &vertexBuffer.allocation, nullptr);
    if (result != VK_SUCCESS) {
        // TODO(Nunwan) do another exception for buffer at runtime
        throw std::runtime_error("Impossible to create the buffer");
    }
    // indices
    if (!indices.empty()) {
        VkBufferCreateInfo indexBufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = indices.size() * sizeof(uint32_t),
            .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        };

        VmaAllocationCreateInfo indexAllocationInfo{
            .usage = VMA_MEMORY_USAGE_GPU_TO_CPU,
        };
        auto result = vmaCreateBuffer(vmaAllocator, &indexBufferInfo, &indexAllocationInfo, &indexBuffer.buffer,
                                      &indexBuffer.allocation, nullptr);
        if (result != VK_SUCCESS) { throw std::runtime_error("Impossible to create the buffer"); }
    }
}

MeshFromObj loadObj(std::string& pathForModelObj) {
    MeshFromObj mesh;

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
    return mesh;
}


Mesh::~Mesh()
{
    vmaDestroyBuffer(vmaAllocator, vertexBuffer.buffer, vertexBuffer.allocation);
    if (!indices.empty()) { vmaDestroyBuffer(vmaAllocator, indexBuffer.buffer, indexBuffer.allocation); }
}

void Mesh::load()
{
    void *data;

    // vertices
    vmaMapMemory(vmaAllocator, vertexBuffer.allocation, &data);
    memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(vmaAllocator, vertexBuffer.allocation);

    // index
    if (!indices.empty()) {
        vmaMapMemory(vmaAllocator, indexBuffer.allocation, &data);
        memcpy(data, indices.data(), indices.size() * sizeof(uint32_t));
        vmaUnmapMemory(vmaAllocator, indexBuffer.allocation);
    }
}

VertexInputDescription Vertex::getDescription()
{
    VertexInputDescription description{};

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

VkBuffer &Mesh::getVertexBuffer() { return vertexBuffer.buffer; }

VkBuffer &Mesh::getIndexBuffer() { return indexBuffer.buffer; }

std::vector<Vertex> Mesh::getVertices() { return vertices; }

std::vector<uint32_t> Mesh::getIndices() { return indices; }
