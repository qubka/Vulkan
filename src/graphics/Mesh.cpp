#include "Mesh.hpp"

using Engine::Mesh;

std::vector<vk::VertexInputBindingDescription> Mesh::Vertex::getBindingDescriptions() {
    return {
        {0, sizeof(Vertex), vk::VertexInputRate::eVertex}
    };
}

std::vector<vk::VertexInputAttributeDescription> Mesh::Vertex::getAttributeDescriptions() {
    return {
        {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
        {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
        {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
        {3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
    };
}