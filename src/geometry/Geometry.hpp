#pragma once

namespace Engine {
    class Plane;

    class Geometry {
    public:
        static glm::vec3 intersectionPoint(const Plane& a, const Plane& b, const Plane& c);
    };
}