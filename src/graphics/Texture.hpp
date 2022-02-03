#pragma once

namespace Engine {
    class Device;

    class Texture {
    public:
        Texture(Device& device, std::string path, vk::Format format);
        Texture(Device& device, void* pixels, uint32_t width, uint32_t height, vk::Format format);
        ~Texture();
        Texture(Texture&&) = delete;
        Texture(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;

        const vk::Image& getImage() const { return image; };
        const vk::ImageView& getView() const { return view; };
        const std::string& getPath() const { return path; };
        uint32_t getWidth() const { return width; };
        uint32_t getHeight() const { return height; };
        vk::Format getFormat() const { return format; };

    private:
        Device& device;
        std::string path;
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;
        uint32_t width;
        uint32_t height;
        vk::Format format;

        void init(void* pixels);
    };
}
