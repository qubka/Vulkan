#pragma once

namespace Engine {
    class Pipeline;
    class Device;
    class Mesh;

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& device, const vk::RenderPass& renderPass/*, VkDescriptorSetLayout globalSetLayout*/);
        ~SimpleRenderSystem();
        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem(SimpleRenderSystem&&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

        void renderEntities(const vk::CommandBuffer& commandBuffer);

    private:
        void createPipelineLayout();
        void createPipeline(const vk::RenderPass& renderPass);

        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        std::unique_ptr<Mesh> mesh;
        std::unique_ptr<Mesh> mesh2;
        vk::PipelineLayout pipelineLayout;
    };
}
