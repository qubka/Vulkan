#pragma once

namespace Engine {
    class Input;
    class Window;
    class Ray;

    class Camera {
    public:
        Camera(Window& window, float speed, float fov, float near, float far);
        ~Camera() = default;
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;

        void update(const Input& input, float deltaTime);

        glm::vec3 forward() const;
        glm::vec3 up() const;
        glm::vec3 right() const;

        const glm::mat4& getProjection() const { return projectionMatrix; };
        const glm::mat4& getView() const { return viewMatrix; };
        const glm::mat4& getViewProjection() const { return viewProjectionMatrix; };
        float getYaw() const { return yaw; };
        float getPitch() const { return pitch; };
        float getFov() const { return fov; };
        float getSpeed() const { return speed; };
        float getNear() const { return near; };
        float getFar() const { return far; };
        const glm::vec3& getPosition() const { return position; };
        const glm::quat& getRotation() const { return rotation; };

        void setPosition(const glm::vec3& pos);
        void setRotation(const glm::quat& rot);
        void setPositionAndRotation(const glm::vec3& pos, const glm::quat& rot);

        Ray screenPointToRay(const glm::vec2& pos) const;
        glm::vec3 screenToWorldPoint(const glm::vec2& pos) const;
    private:
        glm::mat4 projectionMatrix{1};
        glm::mat4 viewMatrix{1};
        glm::mat4 viewProjectionMatrix{1};

        glm::vec3 position{0};
        glm::quat rotation{1, 0, 0, 0};

        float yaw;
        float pitch;
        float speed;
        float fov;
        float far;
        float near;

        Window& window;

        void updateViewMatrix();
    };
}

