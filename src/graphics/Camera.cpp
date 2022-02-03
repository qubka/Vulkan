#include "Camera.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "../geometry/Ray.hpp"

using Engine::Camera;
using Engine::Ray;

Camera::Camera(Window& window, float speed, float fov, float near, float far) :
    window{window},
    speed{speed},
    fov{fov},
    near{near},
    far{far}
{
    assert(far > near && "far cannot be less then near");
    assert(speed >= 0.0f && "speed cannot be negative");
    updateViewMatrix();
}

void Camera::setPosition(const glm::vec3& pos) {
    position = pos;
    updateViewMatrix();
}

void Camera::setRotation(const glm::quat& rot) {
    rotation = rot;
    updateViewMatrix();
}

void Camera::setPositionAndRotation(const glm::vec3 &pos, const glm::quat &rot) {
    position = pos;
    rotation = rot;
    updateViewMatrix();
}

/// @link https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/

glm::vec3 Camera::forward() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::forward;
#else
    return rotation * vec3::back;
#endif
}

glm::vec3 Camera::up() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::down;
#else
    return rotation * vec3::up;
#endif
}

glm::vec3 Camera::right() const {
    return rotation * vec3::right;
}

void Camera::update(const Input& input, float deltaTime) {

    if (input.getKey(GLFW_KEY_W)) {
        position += forward() * speed * deltaTime;
    }
    if (input.getKey(GLFW_KEY_S)) {
        position -= forward() * speed * deltaTime;
    }
    if (input.getKey(GLFW_KEY_D)) {
        position += right() * speed * deltaTime;
    }
    if (input.getKey(GLFW_KEY_A)) {
        position -= right() * speed * deltaTime;
    }

    if (window.isCursorLocked()) {
        const glm::vec2& delta = input.mouseDelta() / (static_cast<float>(window.getHeight()) * 2);
        yaw += delta.x;
        pitch += delta.y;

        static constexpr float limit = glm::radians(89.0f);
        if (pitch > limit) {
            pitch = limit;
        }
        if (pitch < -limit) {
            pitch = -limit;
        }

        rotation = glm::quat{ glm::vec3{pitch, yaw, 0} };
    }

    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + forward(), up());
    projectionMatrix = glm::perspective(fov, window.getAspect(), near, far);
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

/// @link https://antongerdelan.net/opengl/raycasting.html
Ray Camera::screenPointToRay(const glm::vec2& pos) const {
    float mouseX = 2 * pos.x / static_cast<float>(window.getWidth() - 1);
    float mouseY = 2 * pos.y / static_cast<float>(window.getHeight() - 1);
#ifdef GLM_FORCE_DEPTH_ZERO_TO_ONE
    glm::vec4 screenPos(mouseX, -mouseY, 0, 1);
#else
    glm::vec4 screenPos(mouseX, -mouseY, -1, 1);
#endif
    glm::vec4 eyeRay = glm::inverse(projectionMatrix) * screenPos;
    eyeRay.z = -1;
    eyeRay.w = 0;
    glm::vec4 worldRay = glm::inverse(viewMatrix) * eyeRay;

    return { position, glm::normalize(glm::vec3{worldRay}) };
}

/// @link https://discourse.libcinder.org/t/screen-to-world-coordinates/1014/2
glm::vec3 Camera::screenToWorldPoint(const glm::vec2& pos) const {
    return glm::unProject(glm::vec3{pos, 0}, viewMatrix, projectionMatrix, window.getViewport());
}