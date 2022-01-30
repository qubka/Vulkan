#pragma once

namespace Engine {

    struct Position {
        glm::vec3 position{0};

        glm::vec3& operator()();
        const glm::vec3& operator()() const;

        operator glm::vec3() const;
    };
}