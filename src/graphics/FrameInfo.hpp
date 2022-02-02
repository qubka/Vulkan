#pragma once

namespace Engine {
    class Camera;

    struct FrameInfo {
        uint32_t frameIndex;
        float deltaTime;
        vk::CommandBuffer& commandBuffer;
        vk::DescriptorSet& descriptorSet;
        Camera& camera;
        entt::registry& registry;
    };
}

