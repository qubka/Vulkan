#pragma once

#include "RendererSystemBase.hpp"

namespace Engine {
    class Pipeline;
    class Device;
    class FrameInfo;
    class Renderer;

    struct PushConstantData {
        glm::mat4 model{1};
    };

    class MeshRenderer : public RendererSystemBase {
    public:
        MeshRenderer(Device& device, const vk::RenderPass& renderPass, const vk::DescriptorSetLayout& setLayout);
        ~MeshRenderer() override;
        MeshRenderer(const MeshRenderer&) = delete;
        MeshRenderer(MeshRenderer&&) = delete;
        MeshRenderer& operator=(const MeshRenderer&) = delete;
        MeshRenderer& operator=(MeshRenderer&&) = delete;

        void render(const FrameInfo& frameInfo) override;

    private:
        void createPipelineLayout(const vk::DescriptorSetLayout& setLayout);
        void createPipeline(const vk::RenderPass& renderPass);

        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        vk::PipelineLayout pipelineLayout;
    };
}
