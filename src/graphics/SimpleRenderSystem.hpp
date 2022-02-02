#pragma once

namespace Engine {
    class Pipeline;
    class Device;
    class Mesh;
    class FrameInfo;
    class Renderer;

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& device, const Renderer& renderer);
        ~SimpleRenderSystem();
        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem(SimpleRenderSystem&&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

        void renderEntities(const FrameInfo& frameInfo);

    private:
        void createPipelineLayout(const vk::DescriptorSetLayout& descriptorSetLayout);
        void createPipeline(const vk::RenderPass& renderPass);

        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        std::unique_ptr<Mesh> mesh;
        std::unique_ptr<Mesh> mesh2;
        vk::PipelineLayout pipelineLayout;
    };
}
