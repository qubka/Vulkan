#include "Texture.hpp"
#include "Image.hpp"
#include "Device.hpp"
#include "AllocatedBuffer.hpp"

using Engine::Texture;

Texture::Texture(Device& device, std::string path, vk::Format format, vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode) :
    device{device},
    path{std::move(path)},
    format{format}
{
    int channels = componentCount(format);
    Image image{getPath(), channels};

    width = static_cast<uint32_t>(image.width);
    height = static_cast<uint32_t>(image.height);

    createImage(image.pixels);
    createSampler(magFilter, minFilter, addressMode, minmapMode);
}

Texture::Texture(Device& device, void* pixels, uint32_t width, uint32_t height, vk::Format format, vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode) :
    device{device},
    width{width},
    height{height},
    format{format}
{
    createImage(pixels);
    createSampler(magFilter, minFilter, addressMode, minmapMode);
}

Texture::~Texture() {
    device.getLogical().destroySampler(sampler);
    device.getLogical().destroyImageView(view);
    device.getLogical().destroyImage(image);
    device.getLogical().freeMemory(memory);
}

void Texture::createImage(void* pixels) {
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

void Texture::createSampler(vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode) {
    vk::PhysicalDeviceProperties properties = device.getPhysical().getProperties();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.addressModeU = addressMode;
    samplerInfo.addressModeV = addressMode;
    samplerInfo.addressModeW = addressMode;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = minmapMode;

    try {
        sampler = device.getLogical().createSampler(samplerInfo);
    } catch (vk::SystemError& err) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

