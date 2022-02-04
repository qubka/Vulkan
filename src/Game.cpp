#include "Game.hpp"

#include "renderers/RendererSystemBase.hpp"
#include "renderers/MeshRenderer.hpp"

#include "systems/ComponentSystemBase.hpp"
#include "systems/TransformSystem.hpp"

#include "graphics/Renderer.hpp"
#include "graphics/AllocatedBuffer.hpp"
#include "graphics/Mesh.hpp"

#include "components/Transform.hpp"
#include "components/Model.hpp"

using Engine::Game;

Game::Game() {
    //glfwInit(); initialize in window ctor
}

void Game::init() {
    // Create renders
    renders.push_back(std::make_unique<MeshRenderer>(device, renderer));

    // Create systems
    systems.push_back(std::make_unique<TransformSystem>());

    Mesh::Builder meshBuilder{};
    meshBuilder.loadModel("models/cube.obj");
    auto mesh = std::make_shared<Mesh>(device, meshBuilder);

    auto entity = registry.create();
    registry.emplace<Transform>(entity, glm::translate(glm::mat4{1}, glm::vec3{5,5,5}));
    registry.emplace<Model>(entity, mesh);

    entity = registry.create();
    registry.emplace<Transform>(entity);
    registry.emplace<Model>(entity, mesh);
}

Game::~Game() {
    glfwTerminate();
}

void Game::run() {
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

        SceneInfo sceneInfo { deltaTime, camera, registry };
        for (const auto& s : systems) {
            s->update(sceneInfo);
        }

        if (auto frameIndex = renderer.beginFrame(); frameIndex != std::numeric_limits<uint32_t>::max()) {
            renderer.beginSwapChainRenderPass(frameIndex);

            // update
            UniformBufferObject ubo{};
            ubo.perspective = camera.getViewProjection();
            ubo.orthogonal = glm::ortho(0, window.getWidth(), 0, window.getHeight());
            auto& buffer = renderer.getCurrentUniformBuffer();
            buffer->writeToBuffer(&ubo);
            buffer->flush();

            FrameInfo frameInfo{
                frameIndex,
                deltaTime,
                camera,
                registry
            };

            for (const auto& r : renders) {
                r->render(frameInfo);
            }

            renderer.endSwapChainRenderPass(frameIndex);
            renderer.endFrame(frameIndex);
        }

        input.reset();
    }

    device.getLogical().waitIdle();
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

