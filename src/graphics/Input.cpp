#include "Input.hpp"
#include "Window.hpp"

using Engine::Input;

#define MOUSE_BUTTONS 1024

std::vector<Input*> Input::instances;

Input::Input(const Window& window) {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    instances.push_back(this);
}

Input::~Input() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void Input::reset() {
    current++;
    delta.x = 0;
    delta.y = 0;
}

bool Input::getKey(int keycode) const {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode];
}

bool Input::getKeyDown(int keycode) const {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode] && frames[keycode] == current;
}

bool Input::getMouseButton(int button) const {
    int index = MOUSE_BUTTONS + button;
    return keys[index];
}

bool Input::getMouseButtonDown(int button) const {
    int index = MOUSE_BUTTONS + button;
    return keys[index] && frames[index] == current;
}

void Input::setCursorPosition(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void Input::setKey(int key, int action) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
        frames[key] = current;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
        frames[key] = current;
    }
}

void Input::setMouseButton(int button, int action) {
    int key = MOUSE_BUTTONS + button;
    if (action == GLFW_PRESS) {
        keys[key] = true;
        frames[key] = current;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
        frames[key] = current;
    }
}

const glm::vec2& Input::mousePosition() const {
    return position;
}

const glm::vec2& Input::mouseDelta() const {
    return delta;
}

/* Static callbacks */

void Input::cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY) {
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mode) {
    for (auto* input : instances) {
        input->setMouseButton(button, action);
    }
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    for (auto* input : instances) {
        input->setKey(key, action);
    }
}