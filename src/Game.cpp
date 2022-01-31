#include "Game.hpp"
#include "graphics/SimpleRenderSystem.hpp"
#include "graphics/FrameInfo.hpp"

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

    device.getDevice()->waitIdle();
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

