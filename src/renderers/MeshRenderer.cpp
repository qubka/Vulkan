#include "MeshRenderer.hpp"

#include "../graphics/Device.hpp"
#include "../graphics/Pipeline.hpp"
#include "../graphics/Mesh.hpp"
#include "../graphics/Texture.hpp"
#include "../graphics/Renderer.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"

using Engine::MeshRenderer;

MeshRenderer::MeshRenderer(Device& device, Renderer& renderer) : device{device}, renderer{renderer} {
    createPipelineLayout();
    createPipeline();
}

MeshRenderer::~MeshRenderer() {
    device.getLogical().destroyPipelineLayout(pipelineLayout);
}

void MeshRenderer::createPipelineLayout() {
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{renderer.getGlobalLayoutSet()};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    try {
        pipelineLayout = device.getLogical().createPipelineLayout(pipelineLayoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void MeshRenderer::createPipeline() {
    PipelineConfigInfo configInfo{};
    Pipeline::defaultPipelineConfigInfo(configInfo);
    configInfo.pipelineLayout = pipelineLayout;
    configInfo.renderPass = renderer.getSwapChainRenderPass();
    configInfo.subpass = 0;
    pipeline = std::make_unique<Pipeline>(device, "shaders/mesh.vert.spv", "shaders/mesh.frag.spv", configInfo);
}

void MeshRenderer::render(const FrameInfo& frameInfo) {
    auto& commandBuffer = renderer.getCurrentCommandBuffer();
    pipeline->bind(commandBuffer);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &renderer.getCurrentDescriptorSet(), 0, nullptr);

    auto entities = frameInfo.registry.view<const Transform, const Model>();
    for (auto [entity, transform, model] : entities.each()) {
        PushConstantData push { transform };

        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData), &push);

        model.mesh->bind(commandBuffer);
        model.mesh->draw(commandBuffer);
    }
}
