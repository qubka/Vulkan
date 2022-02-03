#pragma once

namespace Engine {
    class Window;
    class Device;
    class SwapChain;
    class AllocatedBuffer;
    class Texture;
    class Font;

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

        const vk::RenderPass& getSwapChainRenderPass() const;
        //const vk::DescriptorSetLayout& getDescriptorSetLayout() const;

        bool isFrameInProgress() const;
        //vk::DescriptorSet& getCurrentDescriptorSet();
        vk::CommandBuffer& getCurrentCommandBuffer();
        //std::unique_ptr<Buffer>& getCurrentUniformBuffer();
        uint32_t getFrameIndex() const;

        uint32_t beginFrame();
        void beginSwapChainRenderPass(uint32_t frameIndex);
        void endSwapChainRenderPass(uint32_t frameIndex);
        void endFrame(uint32_t frameIndex);

    private:
        //void createTextures();
        void createCommandBuffers();
        //void createDescriptorSetLayout();
        //void createDescriptorPool();
        //void createUniformBuffers();
        //void createDescriptorSets();
        void createTextureSampler();
        void recreateSwapChain();

        Window& window;
        Device& device;

        std::unique_ptr<SwapChain> swapChain;
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;
        std::vector<vk::DescriptorSet, std::allocator<vk::DescriptorSet>> descriptorSets;
        //std::vector<std::unique_ptr<Buffer>> uniformBuffers;
        //vk::DescriptorSetLayout descriptorSetLayout;
        //vk::DescriptorPool descriptorPool;

        //vk::Sampler textureSampler;
        //std::vector<std::shared_ptr<Texture>> textures;

        //std::unique_ptr<Font> a12;

        uint32_t currentImageIndex{0};
        uint32_t currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}

