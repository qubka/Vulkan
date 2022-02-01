#pragma once

namespace Engine {
    class Device;

    class Texture {
        std::string path;

        Device& device;
        vk::Image textureImage;
        vk::DeviceMemory textureImageMemory;
    public:
        Texture(Device& device, std::string path);
        ~Texture();
    };
}
