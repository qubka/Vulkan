#include "Game.hpp"
#include "graphics/SimpleRenderSystem.hpp"
#include "graphics/FrameInfo.hpp"
#include "graphics/Buffer.hpp"

using Engine::Game;

Game::Game() {
    //glfwInit(); initialize in window ctor
}

void Game::init() {
}

Game::~Game() {
    glfwTerminate();
}

void Game::run() {
    /*std::vector<std::unique_ptr<Buffer>> uboBuffers;
    uboBuffers.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto uboBuffer = std::make_unique<Buffer>(
                device,
                sizeof(GlobalUbo),
                1,
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible);
        uboBuffer->map();
        uboBuffers.push_back(std::move(uboBuffer));
    }*/

    SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};

    float currentTime = static_cast<float>(glfwGetTime());
    float previousTime = currentTime;

    while (!window.shouldClose()) {
        glfwPollEvents();

        currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        if (input.getKeyDown(GLFW_KEY_ESCAPE)) {
            window.shouldClose(true);
        }

        camera.update(input, deltaTime);

        if (auto frameIndex = renderer.beginFrame(); frameIndex != std::numeric_limits<uint32_t>::max()) {
            renderer.beginSwapChainRenderPass(frameIndex);

            FrameInfo frameInfo{
                frameIndex,
                deltaTime,
                renderer.getCurrentCommandBuffer(),
                camera,
                registry
            };

            simpleRenderSystem.renderEntities(frameInfo);

            renderer.endSwapChainRenderPass(frameIndex);
            renderer.endFrame(frameIndex);
        }

        input.reset();
    }

    device()->waitIdle();
}

int main() {
    auto& game = Engine::Game::instance();
    try {
        game.init();
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception catch: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

