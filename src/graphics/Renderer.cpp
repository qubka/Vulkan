#include "Renderer.hpp"
#include "Window.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "AllocatedBuffer.hpp"
#include "Descriptors.hpp"

using Engine::Renderer;
using Engine::AllocatedBuffer;
//using Engine::DescriptorAllocator;
//using Engine::DescriptorLayoutCache;

Renderer::Renderer(Window& window, Device& device) : window{window}, device{device} {
    recreateSwapChain();
    createUniformBuffers();
    createDescriptorSets();
    createCommandBuffers();
}

Renderer::~Renderer() {
    device.getLogical().freeCommandBuffers(device.getCommandPool(), commandBuffers);
}

void Renderer::createCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

    try {
        commandBuffers = device.getLogical().allocateCommandBuffers(allocInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to allocateDescriptor command buffers!");
    }
}

void Renderer::createUniformBuffers() {
    uniformBuffers.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto buffer = std::make_unique<AllocatedBuffer>(
            device,
            sizeof(UniformBufferObject),
            1,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        buffer->map();
        uniformBuffers.push_back(std::move(buffer));
    }
}

void Renderer::createDescriptorSets() {
    globalPool = DescriptorPool::Builder(device)
        .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(vk::DescriptorType::eUniformBuffer, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();

    globalLayout = DescriptorLayout::Builder(device)
        .addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
        .build();

    globalDescriptorSets.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorSet descriptorSet;
        auto bufferInfo = uniformBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalLayout, *globalPool)
            .writeBuffer(0, bufferInfo)
            .build(descriptorSet);
        globalDescriptorSets.push_back(descriptorSet);
    }
}

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

const vk::DescriptorSetLayout& Renderer::getGlobalLayoutSet() const {
    return globalLayout->getDescriptorSetLayout();
}

const vk::RenderPass& Renderer::getSwapChainRenderPass() const {
    return swapChain->getRenderPass();
}

bool Renderer::isFrameInProgress() const {
    return isFrameStarted;
}

const vk::CommandBuffer& Renderer::getCurrentCommandBuffer() {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
}

const std::unique_ptr<AllocatedBuffer>& Renderer::getCurrentUniformBuffer() {
    assert(isFrameStarted && "Cannot get uniform buffer when frame not in progress");
    return uniformBuffers[currentFrameIndex];
}

const vk::DescriptorSet& Renderer::getCurrentDescriptorSet() {
    assert(isFrameStarted && "Cannot get descriptor set when frame not in progress");
    return globalDescriptorSets[currentFrameIndex];
}

uint32_t Renderer::getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
}
