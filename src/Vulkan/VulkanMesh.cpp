#include "VulkanMesh.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

Vertex::Vertex(glm::vec3 position, glm::vec3 normal) : position(position), normal(normal) {}

Mesh::Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices) : vmaAllocator(vmaAllocator), vertices(vertices), indices(std::vector<uint32_t>(0))
{
}

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

    description.attributes.push_back(positionAttr);
    description.attributes.push_back(normalAttr);

    return description;
}

VkBuffer &Mesh::getVertexBuffer() { return vertexBuffer.buffer; }

VkBuffer &Mesh::getIndexBuffer() { return indexBuffer.buffer; }

std::vector<Vertex> Mesh::getVertices() { return vertices; }

std::vector<uint32_t> Mesh::getIndices() { return indices; }
