#include "TransformSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Position.hpp"
#include "../components/Rotation.hpp"
#include "../components/Scale.hpp"

using Engine::TransformSystem;

void TransformSystem::update(const SceneInfo& sceneInfo) {
    glm::mat4 m{1};

    auto entities = sceneInfo.registry.view<Transform, const Position, const Rotation, const Scale>();
    for (auto [entity, transform, position, rotation, scale] : entities.each()) {
        glm::mat4 p = glm::translate(m, *position);
        glm::mat4 r = glm::mat4_cast(*rotation);
        glm::mat4 s = glm::scale(m, *scale);
        *transform = p * r * s;
    }
}
