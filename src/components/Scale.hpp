#pragma once

namespace Engine {

    struct Scale {
        glm::vec3 scale{1};

        glm::vec3& operator*() { return scale; };
        const glm::vec3& operator*() const { return scale; };
        operator glm::vec3() const { return scale; };
    };
}