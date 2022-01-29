#pragma once

#include "graphics/Window.hpp"
#include "graphics/Input.hpp"
#include "graphics/Device.hpp"
#include "graphics/SwapChain.hpp"
#include "graphics/Pipeline.hpp"

#define WIDTH 1280
#define HEIGHT 720

namespace Engine {
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
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;

        Window window{"Engine", WIDTH, HEIGHT};
        Input input{window};
        Device device{window};
        SwapChain swapChain{device, {WIDTH, HEIGHT}};
        std::unique_ptr<Pipeline> pipeline;
        vk::PipelineLayout pipelineLayout;
        //Renderer renderer{window, device};


    };
}