#pragma once

namespace Engine {
    class Device;
    class Buffer;
    class Mesh {
    public:
        struct Vertex {
            glm::vec2 position{};
            glm::vec3 color{};

            static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
            static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();

            /*bool operator==(const Vertex &other) const {
                return position == other.position && color == other.color && normal == other.normal &&
                       uv == other.uv;
            }*/
        };

        struct Builder {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            //void loadModel(const std::string &filepath);
        };

        Mesh(Device& device, const Builder& builder);
        ~Mesh();
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        void bind(const vk::CommandBuffer& commandBuffer) const;
        void draw(const vk::CommandBuffer& commandBuffer) const;

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        Device& device;

        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount;
        bool hasIndexBuffer = false;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount;
    };
}

