#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using Engine::Image;

Image::Image(const std::string& path, int channel, bool flip) {
    stbi_set_flip_vertically_on_load(flip);
    pixels = stbi_load(path.c_str(), &width, &height, &channels, channel);
    if (!pixels) {
        throw std::runtime_error("Failed to load image: \"" + path + "\" - " + stbi_failure_reason());
    }
    if (channel != channels) {
        std::cerr << "warning: image \"" << path << "\" does not match expected number of channels" << std::endl;
    }
    size = width * height * std::max(channel, channels);
}

Image::~Image() {
    stbi_image_free(pixels);
}