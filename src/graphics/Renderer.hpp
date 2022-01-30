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

        const vk::RenderPass& getSwapChainRenderPass() const;
        bool isFrameInProgress() const;
        const vk::CommandBuffer& getCurrentCommandBuffer() const;
        uint32_t getFrameIndex() const;

        uint32_t beginFrame();
        void beginSwapChainRenderPass(uint32_t frameIndex);
        void endSwapChainRenderPass(uint32_t frameIndex);
        void endFrame(uint32_t frameIndex);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        Window& window;
        Device& device;

        std::unique_ptr<SwapChain> swapChain;
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;

        uint32_t currentImageIndex{0};
        uint32_t currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}

