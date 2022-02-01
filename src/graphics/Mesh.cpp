#include "Mesh.hpp"
#include "Buffer.hpp"
#include "Device.hpp"

using Engine::Mesh;

Mesh::Mesh(Device& device, const Builder& builder) : device{device} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Mesh::~Mesh() {
}

void Mesh::createVertexBuffers(const std::vector<Vertex>& vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    Buffer stagingBuffer{
        device,
        vertexSize,
        vertexCount,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer = std::make_unique<Buffer>(
        device,
        vertexSize,
        vertexCount,
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void Mesh::createIndexBuffers(const std::vector<uint32_t>& indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;

    if (!hasIndexBuffer) {
        return;
    }

    vk::DeviceSize bufferSize = sizeof(indices[0]) * indexCount;
    uint32_t indexSize = sizeof(indices[0]);

    Buffer stagingBuffer {
        device,
        indexSize,
        indexCount,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer = std::make_unique<Buffer>(
        device,
        indexSize,
        indexCount,
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

void Mesh::draw(const vk::CommandBuffer& commandBuffer) const {
    if (hasIndexBuffer) {
        commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
    } else {
        commandBuffer.draw(vertexCount, 1, 0, 0);
    }
}

void Mesh::bind(const vk::CommandBuffer& commandBuffer) const {
    vk::Buffer buffers[] = {vertexBuffer->getBuffer()};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, 1, buffers, offsets);

    if (hasIndexBuffer) {
        commandBuffer.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint32);
    }
}

std::vector<vk::VertexInputBindingDescription> Mesh::Vertex::getBindingDescriptions() {
    return {
        {0, sizeof(Vertex), vk::VertexInputRate::eVertex}
    };
}

std::vector<vk::VertexInputAttributeDescription> Mesh::Vertex::getAttributeDescriptions() {
    return {
        {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
        {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
        {2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord)},
    };
}