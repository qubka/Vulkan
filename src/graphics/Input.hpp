#pragma once

namespace Engine {
    class Window;

    class Input {
    public:
        explicit Input(const Window& window);
        ~Input();
        Input(const Input&) = delete;
        Input(Input&&) = delete;
        Input& operator=(const Input&) = delete;
        Input& operator=(Input&&) = delete;

        void reset();

        bool getKey(int keycode) const;
        bool getKeyDown(int keycode) const;

        bool getMouseButton(int button) const;
        bool getMouseButtonDown(int button) const;

        const glm::vec2& mousePosition() const;
        const glm::vec2& mouseDelta() const;

    private:
        bool keys[1032]{};
        uint32_t frames[1032]{};
        uint32_t current{};
        glm::vec2 delta{};
        glm::vec2 position{};

        void setKey(int key, int action);
        void setMouseButton(int button, int action);
        void setCursorPosition(const glm::vec2& pos);

        // Workaround for C++ class using a c-style-callback

        static void cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

        static std::vector<Input*> instances;
    };
}