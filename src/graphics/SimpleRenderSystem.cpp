#include "SimpleRenderSystem.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"

using Engine::SimpleRenderSystem;

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, const vk::RenderPass& renderPass/*, VkDescriptorSetLayout globalSetLayout*/) : device{device} {
    createPipelineLayout();
    createPipeline(renderPass);

    Mesh::Builder builder;
    builder.vertices = {
        {{0.0, -0.5}, {1.0, 1.0, 1.0}},
        {{0.5, 0.5}, {1.0, 1.0, 1.0}},
        {{-0.5, 0.5}, {1.0, 1.0, 1.0}},
    };

    mesh = std::make_unique<Mesh>(device, builder);

    builder.vertices = {
            {{0.8, -0.8}, {1.0, 0.0, 0.0}},
            {{0.8, 0.8}, {1.0, 0.0, 0.0}},
            {{-0.8, 0.8}, {1.0, 0.0, 0.0}},
    };

    builder.indices = { 0, 1, 2 };

    mesh2 = std::make_unique<Mesh>(device, builder);
}

SimpleRenderSystem::~SimpleRenderSystem() {
    device.getDevice()->destroyPipelineLayout(pipelineLayout);
}

void SimpleRenderSystem::createPipelineLayout() {
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    try {
        pipelineLayout = device.getDevice()->createPipelineLayout(pipelineLayoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(const vk::RenderPass& renderPass) {
    PipelineConfigInfo configInfo{};
    Pipeline::defaultPipelineConfigInfo(configInfo);
    configInfo.pipelineLayout = pipelineLayout;
    configInfo.renderPass = renderPass;
    configInfo.subpass = 0;
    pipeline = std::make_unique<Pipeline>(device, "shaders/mesh.vert.spv", "shaders/mesh.frag.spv", configInfo);
}

void SimpleRenderSystem::renderEntities(const vk::CommandBuffer& commandBuffer) {
    pipeline->bind(commandBuffer);
    mesh->bind(commandBuffer);
    mesh->draw(commandBuffer);

    mesh2->bind(commandBuffer);
    mesh2->draw(commandBuffer);
}
