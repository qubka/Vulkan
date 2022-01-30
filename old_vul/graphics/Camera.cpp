#include "Camera.hpp"
#include "Input.hpp"
#include "Window.hpp"

using Engine::Camera;
using Engine::Ray;

Camera::Camera(Window& window, float speed, float fov, float far, float near) :
    window{window},
    speed{speed},
    fov{fov},
    far{far},
    near{near}
{
    updateViewMatrix();

}

const glm::mat4& Camera::getProjection() const {
    return projectionMatrix;
}

const glm::mat4& Camera::getView() const {
    return viewMatrix;
}

const glm::mat4& Camera::getViewProjection() const {
    return viewProjectionMatrix;
}

const glm::vec3& Camera::getPosition() const {
    return position;
}

const glm::quat& Camera::getRotation() const {
    return rotation;
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

float Camera::getYaw() const {
    return yaw;
}

float Camera::getPitch() const {
    return pitch;
}

float Camera::getFov() const {
    return fov;
}

float Camera::getNear() const {
    return near;
}

float Camera::getFar() const {
    return far;
}

float Camera::getSpeed() const {
    return speed;
}

glm::vec3 Camera::forward() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::forward;
#else
    return rotation * vec3::back;
#endif
}

glm::vec3 Camera::back() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::back;
#else
    return rotation * vec3::forward;
#endif
}

glm::vec3 Camera::up() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::down;
#else
    return rotation * vec3::up;
#endif
}

glm::vec3 Camera::down() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::up;
#else
    return rotation * vec3::down;
#endif
}

glm::vec3 Camera::right() const {
    return rotation * vec3::right;
}

glm::vec3 Camera::left() const {
    return rotation * vec3::left;
}

void Camera::update(const Input& input, float deltaTime) {
    float elapsedTime = 0.01f;
    if (input.getKey(GLFW_KEY_W)) {
        position += rotation * vec3::forward * speed * deltaTime;
    }
    if (input.getKey(GLFW_KEY_S)) {
        position -= rotation * vec3::forward * speed * deltaTime;
    }
    if (input.getKey(GLFW_KEY_D)) {
        position += rotation * vec3::right * speed * deltaTime;
    }
    if (input.getKey(GLFW_KEY_A)) {
        position -= rotation * vec3::right * speed * deltaTime;
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
    glm::vec4 viewport{0, window.getHeight(), window.getWidth(), -window.getHeight()}; // vertical flip is required
    return glm::unProject(glm::vec3{pos, 0}, viewMatrix, projectionMatrix, viewport);
}