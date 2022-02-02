#include "Game.hpp"
#include "graphics/SimpleRenderSystem.hpp"
#include "graphics/FrameInfo.hpp"
#include "graphics/Renderer.hpp"
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
    SimpleRenderSystem simpleRenderSystem{device, renderer};

    float currentTime = static_cast<float>(glfwGetTime());
    float previousTime = currentTime;

    camera.setPosition(glm::vec3{0, 0, 5});

    while (!window.shouldClose()) {
        glfwPollEvents();

        currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        if (input.getKeyDown(GLFW_KEY_ESCAPE)) {
            window.shouldClose(true);
        }

        if (input.getKeyDown(GLFW_KEY_TAB)) {
            window.toggleCursor();
        }

        camera.update(input, deltaTime);

        if (auto frameIndex = renderer.beginFrame(); frameIndex != std::numeric_limits<uint32_t>::max()) {
            renderer.beginSwapChainRenderPass(frameIndex);

            // update
            UniformBufferObject ubo{};
            ubo.viewProj = camera.getViewProjection();
            auto& uniform = renderer.getCurrentUniformBuffer();
            uniform->writeToBuffer(&ubo);
            uniform->flush();

            FrameInfo frameInfo{
                frameIndex,
                deltaTime,
                renderer.getCurrentCommandBuffer(),
                renderer.getCurrentDescriptorSet(),
                camera,
                registry
            };

            simpleRenderSystem.renderEntities(frameInfo);

            renderer.endSwapChainRenderPass(frameIndex);
            renderer.endFrame(frameIndex);
        }

        input.reset();
    }

    device().waitIdle();
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

