#pragma once

namespace Engine {
    class Camera;

    struct GlobalUbo {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        //glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};  // w is intensity
        //PointLight pointLights[MAX_LIGHTS];
        //int numLights;
    };

    struct FrameInfo {
        uint32_t frameIndex;
        float deltaTime;
        VkCommandBuffer commandBuffer;
        //VkDescriptorSet globalDescriptorSet;
        Camera& camera;
        entt::registry& registry;
    };
}

