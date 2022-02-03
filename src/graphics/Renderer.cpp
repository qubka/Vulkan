#include "Renderer.hpp"
#include "Window.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Pipeline.hpp"
#include "AllocatedBuffer.hpp"
#include "Texture.hpp"
#include "Font.hpp"

using Engine::Renderer;
using Engine::AllocatedBuffer;

Renderer::Renderer(Window& window, Device& device) : window{window}, device{device} {
    recreateSwapChain();
    //createTextures();
    createCommandBuffers();
    //createTextureSampler();
    //createDescriptorSetLayout();
    //createUniformBuffers();
    //createDescriptorPool();
    //createDescriptorSets();
}

Renderer::~Renderer() {
   // device.getLogical().destroySampler(textureSampler);
   // device.getLogical().destroyDescriptorSetLayout(descriptorSetLayout);
    //device.getDevice().freeDescriptorSets(descriptorPool, descriptorSets);
    //device.getLogical().destroyDescriptorPool(descriptorPool);
    device.getLogical().freeCommandBuffers(device.getCommandPool(), commandBuffers);
}

/*void Renderer::createTextures() {
    textures.push_back(std::make_shared<Texture>(device, "textures/texture.jpg", vk::Format::eR8G8B8A8Srgb));
    textures.push_back(std::make_shared<Texture>(device, "textures/texture.jpg", vk::Format::eR8G8B8A8Srgb));

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("Failed to init FreeType");
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
        throw std::runtime_error("Failed to load font: \"fonts/arial.ttf\"");
    }

    a12 = std::make_unique<Font>(device, face, 48);
    textures.push_back(a12->getTexture());

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}*/

void Renderer::createCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

    try {
        commandBuffers = device.getLogical().allocateCommandBuffers(allocInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

/*void Renderer::createTextureSampler() {
    vk::PhysicalDeviceProperties properties = device.getPhysical().getProperties();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

    try {
        textureSampler = device.getLogical().createSampler(samplerInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void Renderer::createDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = vk::DescriptorType::eSampler;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

    vk::DescriptorSetLayoutBinding imageLayoutBinding{};
    imageLayoutBinding.binding = 2;
    imageLayoutBinding.descriptorCount = static_cast<uint32_t>(textures.size());
    imageLayoutBinding.descriptorType = vk::DescriptorType::eSampledImage;
    imageLayoutBinding.pImmutableSamplers = nullptr;
    imageLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

    std::array<vk::DescriptorSetLayoutBinding, 3> bindings = {uboLayoutBinding, samplerLayoutBinding, imageLayoutBinding};

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    try {
        descriptorSetLayout = device.getLogical().createDescriptorSetLayout(layoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Renderer::createUniformBuffers() {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT); i++) {
        auto uboBuffer = std::make_unique<AllocatedBuffer>(
            device,
            bufferSize,
            1,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        );

        uboBuffer->map();
        uniformBuffers.push_back(std::move(uboBuffer));
    }
}

void Renderer::createDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    poolSizes[1].type = vk::DescriptorType::eSampler;
    poolSizes[1].descriptorCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    poolSizes[2].type = vk::DescriptorType::eSampledImage;
    poolSizes[2].descriptorCount = SwapChain::MAX_FRAMES_IN_FLIGHT * static_cast<uint32_t>(textures.size());

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = SwapChain::MAX_FRAMES_IN_FLIGHT;

    try {
        descriptorPool = device.getLogical().createDescriptorPool(poolInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Renderer::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(SwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    try {
        descriptorSets = device.getLogical().allocateDescriptorSets(allocInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i]->get();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        vk::DescriptorImageInfo	samplerInfo{};
        samplerInfo.sampler = textureSampler;

        std::vector<vk::DescriptorImageInfo> imageInfos(textures.size());
        for (size_t j = 0; j < textures.size(); j++) {
            imageInfos[j].sampler = nullptr;
            imageInfos[j].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageInfos[j].imageView = textures[j]->getView();
        }

        std::array<vk::WriteDescriptorSet, 3> descriptorWrites{};
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &samplerInfo;

        descriptorWrites[2].dstSet = descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = vk::DescriptorType::eSampledImage;
        descriptorWrites[2].descriptorCount = static_cast<uint32_t>(imageInfos.size());
        descriptorWrites[2].pImageInfo = imageInfos.data();

        device.getLogical().updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}*/

void Renderer::recreateSwapChain() {
    auto extent = vk::Extent2D{static_cast<uint32_t>(window.getWidth()), static_cast<uint32_t>(window.getHeight())};
    while (extent.width == 0 || extent.height == 0) {
        extent = vk::Extent2D{static_cast<uint32_t>(window.getWidth()), static_cast<uint32_t>(window.getHeight())};
        glfwWaitEvents();
    }

    device.getLogical().waitIdle();

    if (swapChain == nullptr) {
        swapChain = std::make_unique<SwapChain>(device, extent);
    } else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
        swapChain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*swapChain)) {
            throw std::runtime_error("Swap chain image(or depth) format has changed");
        }
    }
}

uint32_t Renderer::beginFrame() {
    assert(!isFrameStarted && "Cannot call beginFrame while already in progress");

    auto result = swapChain->acquireNextImage(currentImageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return std::numeric_limits<uint32_t>::max();
    }

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    isFrameStarted = true;

    const auto& commandBuffer = getCurrentCommandBuffer();
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    try {
        commandBuffer.begin(beginInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return currentFrameIndex;
}

void Renderer::beginSwapChainRenderPass(uint32_t frameIndex) {
    assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if frame is not in progress");
    assert(frameIndex == currentFrameIndex && "Cannot begin render pass on command buffer from a different frame");

    const auto& commandBuffer = getCurrentCommandBuffer();

    const auto& extent = swapChain->getSwapChainExtent();
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
    renderPassInfo.renderArea.extent = extent;


    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = std::array<float, 4>{ 0, 0, 0, 1 };
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    vk::Rect2D scissor{{0, 0}, extent};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(uint32_t frameIndex) {
    assert(isFrameStarted && "Cannot call endSwapChainRenderPass if frame is not in progress");
    assert(frameIndex == currentFrameIndex && "Cannot end render pass on command buffer from a different frame");

    getCurrentCommandBuffer().endRenderPass();
}

void Renderer::endFrame(uint32_t frameIndex) {
    assert(isFrameStarted && "Cannot call endFrame if frame is not in progress");
    assert(frameIndex == currentFrameIndex && "Cannot end command buffer from a different frame");

    const auto& commandBuffer = getCurrentCommandBuffer();

    try {
        commandBuffer.end();
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = swapChain->submitCommandBuffers(commandBuffer, currentImageIndex);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || window.wasResized()) {
        std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
        window.resetResized();
        recreateSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

const vk::RenderPass& Renderer::getSwapChainRenderPass() const {
    return swapChain->getRenderPass();
}

/*const vk::DescriptorSetLayout& Renderer::getDescriptorSetLayout() const {
    return descriptorSetLayout;
}*/

bool Renderer::isFrameInProgress() const {
    return isFrameStarted;
}

vk::CommandBuffer& Renderer::getCurrentCommandBuffer() {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
}

/*vk::DescriptorSet& Renderer::getCurrentDescriptorSet()  {
    assert(isFrameStarted && "Cannot get descriptor set when frame not in progress");
    return descriptorSets[currentFrameIndex];
}

std::unique_ptr<AllocatedBuffer>& Renderer::getCurrentUniformBuffer() {
    assert(isFrameStarted && "Cannot get uniform buffer set when frame not in progress");
    return uniformBuffers[currentFrameIndex];
}*/

uint32_t Renderer::getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
}
