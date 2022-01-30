#include "Position.hpp"

using Engine::Position;

glm::vec3& Position::operator()() {
    return position;
}

const glm::vec3& Position::operator()() const {
    return position;
}

Position::operator glm::vec3() const {
    return position;
}