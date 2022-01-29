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

        bool shouldClose() const;
        void shouldClose(bool flag) const;

        operator GLFWwindow*() const;
        int getWidth() const;
        int getHeight() const;
        float getAspect() const;
        const std::string& getTitle() const;

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
