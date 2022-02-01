#pragma once

namespace Engine {
    class Camera;

    struct GlobalUbo {
        glm::mat4 model{1};
    };

    struct FrameInfo {
        uint32_t frameIndex;
        float deltaTime;
        vk::CommandBuffer& commandBuffer;
        Camera& camera;
        entt::registry& registry;
    };
}

