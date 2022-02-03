#pragma once

#include "ComponentSystemBase.hpp"

namespace Engine {
	class TransformSystem : public ComponentSystemBase {
		void update(const SceneInfo& sceneInfo) override;
	};
}
