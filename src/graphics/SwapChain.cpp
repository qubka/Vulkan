#include "SwapChain.hpp"
#include "Device.hpp"

using Engine::SwapChain;

SwapChain::SwapChain(Device& device, vk::Extent2D windowExtent) : device{device}, windowExtent{windowExtent} {
    init();
}

SwapChain::SwapChain(Device& device, vk::Extent2D windowExtent, std::shared_ptr<SwapChain> previous)
        : device{device}, windowExtent{windowExtent}, oldSwapChain{std::move(previous)} {
    init();

    oldSwapChain = nullptr;
}

SwapChain::~SwapChain() {
    for (const auto& imageView : swapChainImageViews) {
        device().destroyImageView(imageView);
    }

    device().destroyImageView(depthImageView);
    device().destroyImage(depthImage);
    device().freeMemory(depthImageMemory);

    device().destroySwapchainKHR(swapChain);

    for (const auto& framebuffer : swapChainFramebuffers) {
        device().destroyFramebuffer(framebuffer);
    }

    device().destroyRenderPass(renderPass);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device().destroySemaphore(renderFinishedSemaphores[i]);
        device().destroySemaphore(imageAvailableSemaphores[i]);
        device().destroyFence(inFlightFences[i]);
    }
}

void SwapChain::init() {
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    createSyncObjects();
}

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(),
            device.getSurface(),
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            extent,
            1, // imageArrayLayers
            vk::ImageUsageFlagBits::eColorAttachment
    );

    QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapChain == nullptr ? vk::SwapchainKHR(nullptr) : oldSwapChain->swapChain;

    try {
        swapChain = device().createSwapchainKHR(createInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create swap chain!");
    }

    swapChainImages = device().getSwapchainImagesKHR(swapChain);

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const {
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        return {
            std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, windowExtent.width)),
            std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, windowExtent.height))
        };
    }
}

void SwapChain::createImageViews() {
    swapChainImageViews.reserve(swapChainImages.size());

    for (const auto& swapChainImage : swapChainImages) {
        swapChainImageViews.push_back(device.createImageView(swapChainImage, swapChainImageFormat, vk::ImageAspectFlagBits::eColor));
    }
}

void SwapChain::createRenderPass() {
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    try {
        renderPass = device().createRenderPass(renderPassInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void SwapChain::createFramebuffers() {
    swapChainFramebuffers.reserve(swapChainImageViews.size());

    for (const auto& swapChainImageView : swapChainImageViews) {
        std::array<vk::ImageView, 2> attachments = {
            swapChainImageView,
            depthImageView
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        try {
            swapChainFramebuffers.push_back(device().createFramebuffer(framebufferInfo));
        } catch (vk::SystemError& err) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainImages.size(), nullptr);

    try {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            imageAvailableSemaphores.push_back(device().createSemaphore({}));
            renderFinishedSemaphores.push_back(device().createSemaphore({}));
            inFlightFences.push_back(device().createFence({vk::FenceCreateFlagBits::eSignaled}));
        }
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void SwapChain::createDepthResources() {
    vk::Format depthFormat = findDepthFormat();

    device.createImage(
            swapChainExtent.width,
            swapChainExtent.height,
            depthFormat,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            depthImage,
            depthImageMemory);

    depthImageView = device.createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

vk::Format SwapChain::findDepthFormat() const {
    return device.findSupportedFormat({
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    },
    vk::ImageTiling::eOptimal,
    vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

const vk::Framebuffer& SwapChain::getFrameBuffer(size_t index) const {
    return swapChainFramebuffers[index];
}

const vk::RenderPass& SwapChain::getRenderPass() const {
    return renderPass;
}

const vk::ImageView& SwapChain::getImageView(size_t index) const {
    return swapChainImageViews[index];
}

const vk::Format& SwapChain::getSwapChainImageFormat() const {
    return swapChainImageFormat;
}

const vk::Extent2D& SwapChain::getSwapChainExtent() const {
    return swapChainExtent;
}

size_t SwapChain::imageCount() const {
    return swapChainImages.size();
}

vk::Result SwapChain::acquireNextImage(uint32_t& imageIndex) const {
    auto result = device().waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fences");
    }

    auto nextImageKHR = device().acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], nullptr);
    imageIndex = nextImageKHR.value;
    return nextImageKHR.result;
}

vk::Result SwapChain::submitCommandBuffers(const vk::CommandBuffer& buffers, const uint32_t& imageIndex) {
    vk::Fence& fence = inFlightFences[currentFrame];
    vk::Fence* image = imagesInFlight[imageIndex];
    if (image != nullptr) {
        auto result = device().waitForFences(1, image, VK_TRUE, std::numeric_limits<uint64_t>::max());
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to wait for fences");
        }
    }
    imagesInFlight[imageIndex] = &fence;

    vk::SubmitInfo submitInfo{};

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffers;

    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    auto result = device().resetFences(1, &fence);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to reset the fence");
    }

    try {
        device.getGraphicsQueue().submit(submitInfo, fence);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    try {
        result = device.getPresentQueue().presentKHR(presentInfo);
    } catch (vk::OutOfDateKHRError& err) {
        result = vk::Result::eErrorOutOfDateKHR;
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

bool SwapChain::compareSwapFormats(const SwapChain& other) const {
    return /*other.swapChainDepthFormat == swapChainDepthFormat &&*/
            other.swapChainImageFormat == swapChainImageFormat;
}