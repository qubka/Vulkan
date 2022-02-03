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
    globalPool = DescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(vk::DescriptorType::eUniformBuffer, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

    uboBuffers.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto buffer = std::make_unique<AllocatedBuffer>(
                device,
                sizeof(UniformBufferObject),
                1,
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        buffer->map();
        uboBuffers.push_back(std::move(buffer));
    }

    globalSetLayout = DescriptorSetLayout::Builder(device)
            .addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
            .build();

    globalDescriptorSets.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorSet descriptorSet;
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, bufferInfo)
                .build(descriptorSet);
        globalDescriptorSets.push_back(descriptorSet);
    }

    // Create renders
    renders.push_back(std::make_unique<MeshRenderer>(device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()));

    // Create systems
    systems.push_back(std::make_unique<TransformSystem>());

    Mesh::Builder meshBuilder{};
    meshBuilder.loadModel("models/cube.obj");
    auto mesh = std::make_shared<Mesh>(device, meshBuilder);

    auto  entity = registry.create();
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
            auto& buffer = uboBuffers[frameIndex];
            buffer->writeToBuffer(&ubo);
            buffer->flush();

            FrameInfo frameInfo{
                frameIndex,
                deltaTime,
                renderer.getCurrentCommandBuffer(),
                globalDescriptorSets[frameIndex],
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

