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
        Texture(Texture&&) = delete;
        Texture(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;

        const vk::Image& getImage() const;
    };
}
