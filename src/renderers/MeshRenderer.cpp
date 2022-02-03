#include "MeshRenderer.hpp"

#include "../graphics/Device.hpp"
#include "../graphics/Pipeline.hpp"
#include "../graphics/Mesh.hpp"
#include "../graphics/Texture.hpp"
#include "../graphics/Renderer.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"

using Engine::MeshRenderer;

MeshRenderer::MeshRenderer(Device& device, const vk::RenderPass& renderPass, const vk::DescriptorSetLayout& setLayout) : device{device} {
    createPipelineLayout(setLayout);
    createPipeline(renderPass);
}

MeshRenderer::~MeshRenderer() {
    device.getLogical().destroyPipelineLayout(pipelineLayout);
}

void MeshRenderer::createPipelineLayout(const vk::DescriptorSetLayout& setLayout) {
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &setLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    try {
        pipelineLayout = device.getLogical().createPipelineLayout(pipelineLayoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void MeshRenderer::createPipeline(const vk::RenderPass& renderPass) {
    PipelineConfigInfo configInfo{};
    Pipeline::defaultPipelineConfigInfo(configInfo);
    configInfo.pipelineLayout = pipelineLayout;
    configInfo.renderPass = renderPass;
    configInfo.subpass = 0;
    pipeline = std::make_unique<Pipeline>(device, "shaders/mesh.vert.spv", "shaders/mesh.frag.spv", configInfo);
}

void MeshRenderer::render(const FrameInfo& frameInfo) {
    pipeline->bind(frameInfo.cb);

    frameInfo.cb.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            0,
            1,
            &frameInfo.ds,
            0,
            nullptr);

    auto entities = frameInfo.registry.view<const Transform, const Model>();
    for (auto [entity, transform, model] : entities.each()) {
        PushConstantData push { transform };

        frameInfo.cb.pushConstants(
                pipelineLayout,
                vk::ShaderStageFlagBits::eVertex,
                0,
                sizeof(PushConstantData),
                &push);

        model.mesh->bind(frameInfo.cb);
        model.mesh->draw(frameInfo.cb);
    }
}
