#pragma once

namespace Engine {
    class Window;
    class Device;
    class SwapChain;
    class AllocatedBuffer;
    class DescriptorPool;
    class DescriptorLayout;

    struct UniformBufferObject {
        alignas(16) glm::mat4 perspective;
        alignas(16) glm::mat4 orthogonal;
    };

    class Renderer {
    public:
        Renderer(Window& window, Device& device);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        const vk::DescriptorSetLayout& getGlobalLayoutSet() const;
        const vk::RenderPass& getSwapChainRenderPass() const;
        const vk::CommandBuffer& getCurrentCommandBuffer();
        const vk::DescriptorSet& getCurrentDescriptorSet();
        const std::unique_ptr<AllocatedBuffer>& getCurrentUniformBuffer();
        uint32_t getFrameIndex() const;
        bool isFrameInProgress() const;

        uint32_t beginFrame();
        void beginSwapChainRenderPass(uint32_t frameIndex);
        void endSwapChainRenderPass(uint32_t frameIndex);
        void endFrame(uint32_t frameIndex);

    private:
        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorSets();
        void recreateSwapChain();

        Window& window;
        Device& device;

        std::unique_ptr<SwapChain> swapChain;
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;
        std::vector<std::unique_ptr<AllocatedBuffer>> uniformBuffers;

        std::vector<vk::DescriptorSet> globalDescriptorSets;
        std::unique_ptr<DescriptorPool> globalPool;
        std::unique_ptr<DescriptorLayout> globalLayout;

        uint32_t currentImageIndex{0};
        uint32_t currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}

