#include "Texture.hpp"
#include "Image.hpp"
#include "Device.hpp"
#include "Buffer.hpp"

using Engine::Texture;

Texture::Texture(Device& device, std::string p) : device{device}, path{std::move(p)} {
    Image image{path};

    Buffer stagingBuffer{
        device,
        image.size,
        1,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        0
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)image.pixels);

    device.createImage(static_cast<uint32_t>(image.width),
                       static_cast<uint32_t>(image.height),
                       vk::Format::eR8G8B8A8Srgb,
                       vk::ImageTiling::eOptimal,
                       vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                       vk::MemoryPropertyFlagBits::eDeviceLocal,
                       textureImage,
                       textureImageMemory);

    device.transitionImageLayout(textureImage,
                                 vk::Format::eR8G8B8A8Srgb,
                                 vk::ImageLayout::eUndefined,
                                 vk::ImageLayout::eTransferDstOptimal);

    device.copyBufferToImage(stagingBuffer.getBuffer(),
                             textureImage,
                             static_cast<uint32_t>(image.width),
                             static_cast<uint32_t>(image.height),
                             1);

    device.transitionImageLayout(textureImage,
                                 vk::Format::eR8G8B8A8Srgb,
                                 vk::ImageLayout::eTransferDstOptimal,
                                 vk::ImageLayout::eShaderReadOnlyOptimal);
}

Texture::~Texture() {
    device()->destroyImage(textureImage);
    device()->freeMemory(textureImageMemory);
}