#include "Game.hpp"

using Engine::Game;

Game::Game() {
    //glfwInit(); initialize in window ctor
}

void Game::init() {

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    try {
        pipelineLayout = device()->createPipelineLayout(pipelineLayoutInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    auto config = Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT);
    config.pipelineLayout = pipelineLayout;
    config.renderPass = swapChain.getRenderPass();
    config.subpass = 0;
    pipeline = std::make_unique<Pipeline>(device, "shaders/mesh.vert.spv", "shaders/mesh.frag.spv", config);

    //

    commandBuffers.resize(swapChain.imageCount());

    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    try {
        commandBuffers = device()->allocateCommandBuffers(allocInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

        try {
            commandBuffers[i].begin(beginInfo);
        }
        catch (vk::SystemError& err) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        vk::RenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass = swapChain.getRenderPass();
        renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);
        renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
        renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

        vk::ClearValue clearColor = { std::array<float, 4>{ 1.0f, 0.0f, 0.0f, 1.0f } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        pipeline->bind(commandBuffers[i]);

        commandBuffers[i].draw(3, 1, 0, 0);

        commandBuffers[i].endRenderPass();

        try {
            commandBuffers[i].end();
        } catch (vk::SystemError err) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    //
}

Game::~Game() {
    device()->freeCommandBuffers(device.getCommandPool(), commandBuffers);
    device()->destroyPipelineLayout(pipelineLayout);
    glfwTerminate();
}

void Game::run() {
    uint32_t frameIndex = 0;
    float currentTime = static_cast<float>(glfwGetTime());
    float previousTime = currentTime;

    while (!window.shouldClose()) {
        glfwPollEvents();

        frameIndex++;
        currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        if (input.getKeyDown(GLFW_KEY_ESCAPE)) {
            window.shouldClose(true);
        }

        uint32_t imageIndex;
        auto result = swapChain.acquireNextImage(imageIndex);
        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        result = swapChain.submitCommandBuffers(commandBuffers[imageIndex], imageIndex);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        /*if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            //simpleRenderSystem.renderEntities({frameIndex, deltaTime, commandBuffer,  camera, registry});
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }*/

        input.reset();
    }
}

int main() {
    auto& game = Engine::Game::instance();
    try {
        game.init();
        game.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

