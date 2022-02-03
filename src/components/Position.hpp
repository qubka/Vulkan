#pragma once

namespace Engine {

    struct Position {
        glm::vec3 position{0};

        glm::vec3& operator*() { return position; };
        const glm::vec3& operator*() const { return position; };
        operator glm::vec3() const { return position; };
    };
}