#include "Buffer.hpp"
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>


Buffer::Buffer(VmaAllocator vmaAllocator, uint32_t size, VkBufferUsageFlags usage,
               VmaMemoryUsage memoryUsage)
    : vmaAllocator(vmaAllocator), size(size), data(data)
{
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
    };

    VmaAllocationCreateInfo allocationInfo{.usage = memoryUsage};

    auto result =
        vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocationInfo, &aBuffer.buffer, &aBuffer.allocation, nullptr);
    if (result != VK_SUCCESS) { throw std::runtime_error("Impossible to create the buffer"); }
}

void Buffer::update(const void *newData)
{
    void *data = nullptr;
    vmaMapMemory(vmaAllocator, aBuffer.allocation, &data);
    memcpy(data, newData, size);
    vmaUnmapMemory(vmaAllocator, aBuffer.allocation);
}

Buffer::~Buffer() { vmaDestroyBuffer(vmaAllocator, aBuffer.buffer, aBuffer.allocation); }

VkBuffer Buffer::getBuffer() {
    return aBuffer.buffer;
}
