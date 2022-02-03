#pragma once

namespace Engine {

    struct Rotation {
        glm::quat rotation{1, 0, 0, 0};

        glm::quat& operator*() { return rotation; };
        const glm::quat& operator*() const { return rotation; };
        operator glm::quat() const { return rotation; };
    };
}