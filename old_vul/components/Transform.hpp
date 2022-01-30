#pragma once

namespace Engine {

    struct Transform {
        glm::mat4 transform{1};

        glm::mat4& operator()();
        const glm::mat4& operator()() const;

        operator glm::mat4() const;
    };
}