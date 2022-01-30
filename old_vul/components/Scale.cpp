#include "Scale.hpp"

using Engine::Scale;

glm::vec3& Scale::operator()() {
    return scale;
}

const glm::vec3& Scale::operator()() const {
    return scale;
}

Scale::operator glm::vec3() const {
    return scale;
}
