#include "Game.hpp"

#include "graphics/Mesh.hpp"
#include "graphics/FrameInfo.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/SwapChain.hpp"
#include "graphics/Descriptors.hpp"

#include "geometry/Ray.hpp"

#include "components/Transform.hpp"
#include "components/Model.hpp"
#include "components/Position.hpp"
#include "components/TransformComponent.hpp"


using Engine::Game;

Game::Game() {
    //glfwInit(); initialize in window ctor
}

void Game::init() {
    /*globalPool =
            DescriptorPool::Builder(device)
                    .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                    .build();*/

    Mesh::Builder meshBuilder{};
    meshBuilder.loadModel("models/cube.obj");
    mesh = std::make_unique<Mesh>(device, meshBuilder);

    auto  entity = registry.create();
    registry.emplace<TransformComponent>(entity);
    registry.emplace<Model>(entity, mesh);

    entity = registry.create();
    auto&t = registry.emplace<TransformComponent>(entity);
    t.translation = {5.0f, 5.0f, 0.0f};
    registry.emplace<Model>(entity, mesh);

}

Game::~Game() {
    glfwTerminate();
}

void Game::run() {
    /*std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
                device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout =
            DescriptorSetLayout::Builder(device)
                    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                    .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
    }*/

    SimpleRenderSystem simpleRenderSystem{
            device,
            renderer.getSwapChainRenderPass()};

    uint32_t frameIndex = 0;
    float currentTime = static_cast<float>(glfwGetTime());
    float previousTime = currentTime;

    while (!window.shouldClose()) {
        glfwPollEvents();

        frameIndex++;
        currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        //camera.screenPointToRay(input.mousePosition());
        //std::cout << glm::to_string(camera.screenPointToRay(input.mousePosition()).getPoint(1.0f)) << std::endl;
        //std::cout << glm::to_string(camera.forward()) << std::endl;

        if (input.getKeyDown(GLFW_KEY_ESCAPE)) {
            window.shouldClose(true);
        }

        if (input.getKeyDown(GLFW_KEY_TAB)) {
            window.toggleCursor();
        }

        camera.update(input, deltaTime);

        GlobalUbo ubo{};
        ubo.projection = camera.getProjection();
        ubo.view = camera.getView();
        //pointLightSystem.update(frameInfo, ubo);
        //uboBuffers[frameIndex]->writeToBuffer(&ubo);
        //uboBuffers[frameIndex]->flush();

        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderEntities({frameIndex, deltaTime, commandBuffer,  camera, registry});
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }

        input.reset();
    }

    vkDeviceWaitIdle(device);
}

int main() {
    auto& game = Engine::Game::instance();
    game.init();
    game.run();
    return 0;
}

