#pragma once

namespace Engine {
    class Pipeline;
    class Device;
    class Mesh;
    class Texture;
    class FrameInfo;

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& device, const vk::RenderPass& renderPass/*, const std::vector<vk::UniqueDescriptorSetLayout>& globalSetLayout*/);
        ~SimpleRenderSystem();
        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem(SimpleRenderSystem&&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

        void renderEntities(const FrameInfo& frameInfo);

    private:
        void createPipelineLayout(/*const std::vector<vk::UniqueDescriptorSetLayout>& globalSetLayout*/);
        void createPipeline(const vk::RenderPass& renderPass);

        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        std::unique_ptr<Mesh> mesh;
        std::unique_ptr<Mesh> mesh2;
        std::unique_ptr<Texture> texture;
        vk::UniquePipelineLayout pipelineLayout;
    };
}
