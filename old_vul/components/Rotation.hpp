#pragma once

namespace Engine {

    struct Rotation {
        glm::quat rotation{1, 0, 0, 0};

        glm::quat& operator()();
        const glm::quat& operator()() const;

        operator glm::quat() const;
    };
}