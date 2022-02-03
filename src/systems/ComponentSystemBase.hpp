#pragma once

namespace Engine {
    class Camera;

    struct SceneInfo {
        float deltaTime;
        Camera& camera;
        entt::registry& registry;
    };

	class ComponentSystemBase {
	public:
        virtual ~ComponentSystemBase() = default;
		virtual void update(const SceneInfo& sceneInfo) = 0;
	};
}
