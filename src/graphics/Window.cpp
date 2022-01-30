#include "Window.hpp"

using Engine::Window;

Window::Window(std::string title, int width, int height) :
    title{std::move(title)},
    width{width},
    height{height},
    aspect{static_cast<float>(width) / static_cast<float>(height)}
{
    static auto once = (glfwInit(), true);
    init();
}

Window::~Window() {
    glfwDestroyWindow(window);
}

void Window::init() {
    if (width < 0 || height < 0) {
        throw std::invalid_argument("width or height cannot be negative");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        throw std::runtime_error("failed to create window!");
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::shouldClose(bool flag) const {
    glfwSetWindowShouldClose(window, flag);
}

Window::operator GLFWwindow*() const {
    return window;
}

int Window::getWidth() const {
    return width;
}

int Window::getHeight() const {
    return height;
}

float Window::getAspect() const {
    return aspect;
}

const std::string& Window::getTitle() const {
    return title;
}

bool Window::wasResized() const {
    return resized;
}

void Window::resetResized() {
    resized = false;
}

void Window::toggleCursor() {
    locked = !locked;
    glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Window::isCursorLocked() const {
    return locked;
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto pWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    pWindow->width = width;
    pWindow->height = height;
    pWindow->aspect = static_cast<float>(width) / static_cast<float>(height);
    pWindow->resized = true;
}