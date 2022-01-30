#pragma once

namespace Engine {
    class Window;
    class Device;

    class SwapChain {
    public:
        SwapChain(Device& device, VkExtent2D windowExtent);
        SwapChain(Device& device, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
        ~SwapChain();
        SwapChain(const SwapChain&) = delete;
        SwapChain(SwapChain&&) = delete;
        SwapChain& operator=(const SwapChain&) = delete;
        SwapChain& operator=(SwapChain&&) = delete;

        VkFramebuffer getFrameBuffer(size_t index) const;
        VkRenderPass getRenderPass() const;
        VkImageView getImageView(size_t index) const;
        VkFormat getSwapChainImageFormat() const;
        VkExtent2D getSwapChainExtent() const;
        size_t imageCount() const;
        uint32_t width() const;
        uint32_t height() const;
        float aspect() const;

        VkFormat findDepthFormat() const;
        bool hasStencilComponent(VkFormat format) const;
        VkResult acquireNextImage(uint32_t& imageIndex) const;
        VkResult submitCommandBuffers(const VkCommandBuffer& buffers, const uint32_t& imageIndex);

        bool compareSwapFormats(const SwapChain& swapChain) const;

    private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

        Device& device;

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;
        VkExtent2D windowExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemories;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        VkSwapchainKHR swapChain;
        std::shared_ptr<SwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame{};

    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    };
}

