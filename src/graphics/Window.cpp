#include "Window.hpp"

using Engine::Window;

Window::Window(std::string title, int width, int height) :
    title{std::move(title)},
    width{width},
    height{height},
    aspect{static_cast<float>(width) / static_cast<float>(height)}
{
    assert(width > 0 && height > 0 && "Width or height cannot be negative");
    static auto once = (glfwInit(), true);
    init();
}

Window::~Window() {
    glfwDestroyWindow(window);
}

void Window::init() {
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

glm::vec4 Window::getViewport() const {
#ifdef GLFW_INCLUDE_VULKAN
    return {0, 0, width, height};
#else // OPENGL
    return {0, height, width, -height}; // vertical flip is required
#endif
}
