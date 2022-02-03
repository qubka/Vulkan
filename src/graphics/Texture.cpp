#include "Texture.hpp"
#include "Image.hpp"
#include "Device.hpp"
#include "AllocatedBuffer.hpp"

using Engine::Texture;

Texture::Texture(Device& device, std::string path, vk::Format format) :
    device{device},
    path{std::move(path)},
    format{format}
{
    int channels = componentCount(format);
    Image image{getPath(), channels};

    width = static_cast<uint32_t>(image.width);
    height = static_cast<uint32_t>(image.height);

    init(image.pixels);
}

Texture::Texture(Device& device, void* pixels, uint32_t width, uint32_t height, vk::Format format) :
    device{device},
    width{width},
    height{height},
    format{format}
{
    init(pixels);
}

void Texture::init(void* pixels) {
    assert(width > 0 && height > 0 && "Width and height must be greater than zero!");
    assert(pixels && "Pixels data can be null");

    vk::DeviceSize size = width * height * componentCount(format);

    AllocatedBuffer stagingBuffer{
            device,
            size,
            1,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            0
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer(pixels);

    device.createImage(width, height, format,
                       vk::ImageTiling::eOptimal,
                       vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                       vk::MemoryPropertyFlagBits::eDeviceLocal,
                       image, memory);

    device.transitionImageLayout(image, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    device.copyBufferToImage(*stagingBuffer, image, width, height, 1);
    device.transitionImageLayout(image, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    view = device.createImageView(image, format, vk::ImageAspectFlagBits::eColor);
}

Texture::~Texture() {
    device.getLogical().destroyImageView(view);
    device.getLogical().destroyImage(image);
    device.getLogical().freeMemory(memory);
}
