#pragma once

namespace Engine {

    struct Scale {
        glm::vec3 scale{1};

        glm::vec3& operator()();
        const glm::vec3& operator()() const;

        operator glm::vec3() const;
    };
}