#pragma once

#include "RendererSystemBase.hpp"

namespace Engine {
    class Texture;
    class Pipeline;
    class Device;
    class FrameInfo;
    class Renderer;
    class DescriptorPool;
    class DescriptorLayout;

    struct PushConstantData {
        glm::mat4 model{1};
    };

    class MeshRenderer : public RendererSystemBase {
    public:
        MeshRenderer(Device& device, Renderer& renderer);
        ~MeshRenderer() override;
        MeshRenderer(const MeshRenderer&) = delete;
        MeshRenderer(MeshRenderer&&) = delete;
        MeshRenderer& operator=(const MeshRenderer&) = delete;
        MeshRenderer& operator=(MeshRenderer&&) = delete;

        void render(const FrameInfo& frameInfo) override;

    private:
        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        Device& device;
        Renderer& renderer;

        std::vector<vk::DescriptorSet> textureDescriptorSets;
        std::unique_ptr<DescriptorPool> texturePool;
        std::unique_ptr<DescriptorLayout> textureLayout;
        std::unique_ptr<Texture> texture;
        std::unique_ptr<Pipeline> pipeline;
        vk::PipelineLayout pipelineLayout;
    };
}
