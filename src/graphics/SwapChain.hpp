#pragma once

namespace Engine {
    class Device;
    class SwapChain {
    public:
        SwapChain(Device& device, vk::Extent2D windowExtent);
        ~SwapChain();
        SwapChain(const SwapChain&) = delete;
        SwapChain(SwapChain&&) = delete;
        SwapChain& operator=(const SwapChain&) = delete;
        SwapChain& operator=(SwapChain&&) = delete;

        const vk::Framebuffer& getFrameBuffer(size_t index) const;
        const vk::RenderPass& getRenderPass() const;
        const vk::ImageView& getImageView(size_t index) const;
        const vk::Format& getSwapChainImageFormat() const;
        const vk::Extent2D& getSwapChainExtent() const;
        size_t imageCount() const;
        uint32_t width() const;
        uint32_t height() const;
        float aspect() const;

        vk::Result acquireNextImage(uint32_t& imageIndex) const;
        vk::Result submitCommandBuffers(const vk::CommandBuffer& buffers, const uint32_t& imageIndex);

    private:
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        vk::SwapchainKHR swapChain;
        vk::RenderPass renderPass;
        std::vector<vk::Image> swapChainImages;
        vk::Format swapChainImageFormat;
        vk::Extent2D swapChainExtent;
        std::vector<vk::ImageView> swapChainImageViews;
        std::vector<vk::Framebuffer> swapChainFramebuffers;

        std::vector<vk::Semaphore> imageAvailableSemaphores;
        std::vector<vk::Semaphore> renderFinishedSemaphores;
        std::vector<vk::Fence> inFlightFences;
        size_t currentFrame = 0;

        Device& device;
        vk::Extent2D windowExtent;

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;

    public:
        static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    };
}
