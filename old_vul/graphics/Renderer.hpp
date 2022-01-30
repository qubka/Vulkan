#pragma once

namespace Engine {
    class Window;
    class Device;
    class SwapChain;

    class Renderer {
    public:
        Renderer(Window& window, Device& device);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        VkRenderPass getSwapChainRenderPass() const;
        bool isFrameInProgress() const;
        VkCommandBuffer getCurrentCommandBuffer() const;
        uint32_t getFrameIndex() const;

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window& window;
        Device& device;

        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex{0};
        uint32_t currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}

