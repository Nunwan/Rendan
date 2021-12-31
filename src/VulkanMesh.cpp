#include "VulkanMesh.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <vector>

Vertex::Vertex(glm::vec3 position, glm::vec3 normal) : position(position), normal(normal) {}

Mesh::Mesh(VmaAllocator vmaAllocator, std::vector<Vertex> vertices): vmaAllocator(vmaAllocator), vertices(vertices) {
    VkBufferCreateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = vertices.size() * sizeof(Vertex),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    };

    VmaAllocationCreateInfo allocationInfo {
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU,
    };

    auto result = vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocationInfo, &buffer.buffer, &buffer.allocation, nullptr);
    if (result != VK_SUCCESS) {
        // TODO(Nunwan) do another exception for buffer at runtime
        throw std::runtime_error("Impossible to create the buffer");
    }
}

Mesh::~Mesh() {
    vmaDestroyBuffer(vmaAllocator, buffer.buffer, buffer.allocation);
}

void Mesh::load() {
    void *data;

    vmaMapMemory(vmaAllocator, buffer.allocation, &data);
    memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(vmaAllocator, buffer.allocation);
}

VertexInputDescription Vertex::getDescription() {
    VertexInputDescription description {};

    VkVertexInputBindingDescription mainBinding{
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    description.bindings.push_back(mainBinding);

    VkVertexInputAttributeDescription positionAttr {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, position),
    };

    VkVertexInputAttributeDescription normalAttr {
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, normal),
    };

    description.attributes.push_back(positionAttr);
    description.attributes.push_back(normalAttr);

    return description;


}

VkBuffer& Mesh::getBuffer() {
    return buffer.buffer;
}

std::vector<Vertex> Mesh::getVertices() {
    return vertices;
}
