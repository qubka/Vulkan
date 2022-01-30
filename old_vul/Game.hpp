#pragma once

#include "graphics/Window.hpp"
#include "graphics/Input.hpp"
#include "graphics/Device.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Camera.hpp"
#include "graphics/SimpleRenderSystem.hpp"

#define WIDTH 1280
#define HEIGHT 720

namespace Engine {
    class Mesh;
    class DescriptorPool;

    class Game {
        Game();
        ~Game();
    public:
        Game(const Game&) = delete;
        Game(Game&&) = delete;
        Game& operator=(const Game&) = delete;
        Game& operator=(Game&&) = delete;

        void init();
        void run();

        static Game& instance() {
            static Game instance;
            return instance;
        }
    private:
        /*Window window{"Engine", WIDTH, HEIGHT};
        Input input{window};
        Device device{window};
        Renderer renderer{window, device};
        Camera camera{window, 5, 45, 0.1f, 100.0f};*/

        //std::unique_ptr<DescriptorPool> globalPool{};
        //std::shared_ptr<Mesh> mesh{};
        //entt::registry registry{};
    };
}