#include "Rotation.hpp"

using Engine::Rotation;

glm::quat& Rotation::operator()() {
    return rotation;
}

const glm::quat& Rotation::operator()() const {
    return rotation;
}

Rotation::operator glm::quat() const {
    return rotation;
}
