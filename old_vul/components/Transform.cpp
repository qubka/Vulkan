#include "Transform.hpp"

using Engine::Transform;

glm::mat4& Transform::operator()() {
    return transform;
}

const glm::mat4& Transform::operator()() const {
    return transform;
}

Transform::operator glm::mat4() const {
    return transform;
}
