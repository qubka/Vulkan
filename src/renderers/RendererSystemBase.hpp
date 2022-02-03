#pragma once

namespace Engine {
    class Camera;

    struct FrameInfo {
        uint32_t frameIndex;
        float deltaTime;
        vk::CommandBuffer& cb;
        vk::DescriptorSet& ds;
        Camera& camera;
        entt::registry& registry;
    };

	class RendererSystemBase {
	public:
        virtual ~RendererSystemBase() = default;
		virtual void render(const FrameInfo& frameInfo) = 0;

		/*static void clear();
		static size_t drawCalls;
		static size_t totalCalls;
		static size_t totalVertices;*/
	};
}
