#include "MeshRenderer.hpp"

#include "../graphics/Device.hpp"
#include "../graphics/Pipeline.hpp"
#include "../graphics/Mesh.hpp"
#include "../graphics/Texture.hpp"
#include "../graphics/Renderer.hpp"
#include "../graphics/Descriptors.hpp"
#include "../graphics/SwapChain.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"

using Engine::MeshRenderer;

MeshRenderer::MeshRenderer(Device& device, Renderer& renderer) : device{device}, renderer{renderer} {
    createDescriptorSets();
    createPipelineLayout();
    createPipeline();
}

MeshRenderer::~MeshRenderer() {
    device.getLogical().destroyPipelineLayout(pipelineLayout);
}

void MeshRenderer::createDescriptorSets() {
    texturePool = DescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(vk::DescriptorType::eCombinedImageSampler, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

    textureLayout = DescriptorLayout::Builder(device)
            .addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
            .build();

    textureDescriptorSets.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    texture = std::make_unique<Texture>(device, "textures/texture.jpg", vk::Format::eR8G8B8A8Srgb);

    vk::DescriptorImageInfo imageInfo{};
    imageInfo.sampler = texture->getSampler();
    imageInfo.imageView = texture->getView();
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorSet descriptorSet;
        DescriptorWriter(*textureLayout, *texturePool)
                .writeImage(0, imageInfo)
                .build(descriptorSet);
        textureDescriptorSets.push_back(descriptorSet);
    }
}

void MeshRenderer::createPipelineLayout() {
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    std::array<vk::DescriptorSetLayout, 2> descriptorSetLayouts{renderer.getGlobalLayoutSet(), textureLayout->getDescriptorSetLayout()};

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

    std::array<vk::DescriptorSet, 2> descriptorSets{renderer.getCurrentDescriptorSet(), textureDescriptorSets[frameInfo.frameIndex]};

    commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            0,
            static_cast<uint32_t>(descriptorSets.size()),
            descriptorSets.data(),
            0,
            nullptr);

    auto entities = frameInfo.registry.view<const Transform, const Model>();
    for (auto [entity, transform, model] : entities.each()) {
        PushConstantData push { transform };

        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData), &push);

        model.mesh->bind(commandBuffer);
        model.mesh->draw(commandBuffer);
    }
}
