#pragma once

namespace Engine {

    struct Transform {
        glm::mat4 transform{1};

        glm::mat4& operator*() { return transform; };
        const glm::mat4& operator*() const { return transform; };
        operator glm::mat4() const { return transform; };
    };
}