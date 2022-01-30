#pragma once

namespace Engine {
    class Pipeline;
    class Device;
    class FrameInfo;

    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& device, VkRenderPass renderPass/*, VkDescriptorSetLayout globalSetLayout*/);
        ~SimpleRenderSystem();
        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem(SimpleRenderSystem&&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

        void renderEntities(const FrameInfo& frameInfo);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
}
