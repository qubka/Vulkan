#include "SimpleRenderSystem.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "FrameInfo.hpp"

using Engine::SimpleRenderSystem;

struct PushConstantData {
    glm::mat4 viewProj{1};
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, const vk::RenderPass& renderPass/*, const std::vector<vk::UniqueDescriptorSetLayout>& globalSetLayout*/) : device{device} {
    createPipelineLayout();
    createPipeline(renderPass);

    texture = std::make_unique<Texture>(device, "textures/texture.jpg");

    Mesh::Builder builder;

    builder.vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    builder.indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
    };


    mesh = std::make_unique<Mesh>(device, builder);
}

SimpleRenderSystem::~SimpleRenderSystem() {
}

void SimpleRenderSystem::createPipelineLayout(/*const std::vector<vk::UniqueDescriptorSetLayout>& globalSetLayout*/) {
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 0;//static_cast<uint32_t>(globalSetLayout.size());
    //pipelineLayoutInfo.pSetLayouts = &*globalSetLayout[0];
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    try {
        pipelineLayout = device()->createPipelineLayoutUnique(pipelineLayoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(const vk::RenderPass& renderPass) {
    PipelineConfigInfo configInfo{};
    Pipeline::defaultPipelineConfigInfo(configInfo);
    configInfo.pipelineLayout = *pipelineLayout;
    configInfo.renderPass = renderPass;
    configInfo.subpass = 0;
    pipeline = std::make_unique<Pipeline>(device, "shaders/mesh.vert.spv", "shaders/mesh.frag.spv", configInfo);
}

void SimpleRenderSystem::renderEntities(const FrameInfo& frameInfo) {
    pipeline->bind(frameInfo.commandBuffer);

    /*frameInfo.commandBuffer.bindDescriptorSets(
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);*/

    /*PushConstantData push{};
    push.modelMatrix = frameInfo.camera.getViewProjection() * transform.mat4();

    commandBuffer.pushConstants(
            *pipelineLayout,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0,
            sizeof(PushConstantData),
            &push);*/

    mesh->bind(frameInfo.commandBuffer);
    mesh->draw(frameInfo.commandBuffer);
}
