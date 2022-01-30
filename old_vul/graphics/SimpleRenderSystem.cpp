#include "SimpleRenderSystem.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "FrameInfo.hpp"

#include "../components/TransformComponent.hpp"
#include "../components/Model.hpp"

using Engine::SimpleRenderSystem;

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass/*, VkDescriptorSetLayout globalSetLayout*/) : device{device} {
    createPipelineLayout();
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    //std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "shaders/mesh.vert.spv", "shaders/mesh.frag.spv", pipelineConfig);
}

void SimpleRenderSystem::renderEntities(const FrameInfo& frameInfo) {
    pipeline->bind(frameInfo.commandBuffer);

    /*vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);*/

    int i =0;
    auto entities = frameInfo.registry.view<const TransformComponent, const Model>();
    for (auto [entity, transform, model] : entities.each()) {

        SimplePushConstantData push{};
        push.modelMatrix = frameInfo.camera.getViewProjection() * transform.mat4();
        push.color = {0, i + 0.5, 0};
        vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
        i++;
        model.mesh->bind(frameInfo.commandBuffer);
        model.mesh->draw(frameInfo.commandBuffer);
    }
}
