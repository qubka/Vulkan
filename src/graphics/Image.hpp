#pragma once

namespace Engine {
    struct Image {
        uint8_t* pixels;
        int width;
        int height;
        int channels;
        size_t size;

        Image(const std::string& path, int channels = 4, bool flip = true);
        ~Image();
        Image(const Image&) = delete;
        Image(Image&&) = delete;
        Image& operator=(const Image&) = delete;
        Image& operator=(Image&&) = delete;
    };
}