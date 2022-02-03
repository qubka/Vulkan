#pragma once

namespace Engine {
    class Window {
    public:
        Window(std::string title, int width, int height);
        ~Window();
        Window(Window&&) = delete;
        Window(const Window&) = delete;
        Window& operator=(Window&&) = delete;
        Window& operator=(const Window&) = delete;

        operator GLFWwindow*() const { return window; };
        int getWidth() const { return width; };
        int getHeight() const { return height; };
        float getAspect() const { return aspect; };
        const std::string& getTitle() const { return title; };
        glm::vec4 getViewport() const;

        bool shouldClose() const;
        void shouldClose(bool flag) const;

        bool wasResized() const;
        void resetResized();

        void toggleCursor();
        bool isCursorLocked() const;

    private:
        GLFWwindow* window;
        int width;
        int height;
        float aspect;
        std::string title;
        bool resized;
        bool locked;

        void init();

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
}
