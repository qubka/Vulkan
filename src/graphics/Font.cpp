#include "Font.hpp"
#include "Device.hpp"
#include "Texture.hpp"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

#define NUM_GLYPHS 128

using Engine::Font;
using Engine::Glyph;

Font::Font(Device& device, const FT_Face& face, uint32_t size)  {
    assert(size > 0 && "Size cannot be smaller than zero");

    FT_Set_Pixel_Sizes(face, 0, size);
    const auto& glyph = face->glyph;
    //FT_Set_Char_Size(face, 0, size << 6, 48, 48);

    metrics = 1 + (face->size->metrics.height >> 6);
    int ox = 0, oy = 0;
    int maxDim = metrics * std::ceil(std::sqrt(NUM_GLYPHS / 2));

    width = 1;
    while (width < maxDim) width <<= 1;
    height = width;

    std::vector<unsigned char> pixels(width * height);

    for (unsigned char c = 32; c < NUM_GLYPHS; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT)) {
            throw std::runtime_error("Failed to load glyph '" + std::to_string(c) + "'");
        }

        const auto& bmp = glyph->bitmap;

        if (ox + bmp.width >= width) {
            ox = 0;
            oy += metrics;
        }

        for (int row = 0; row < bmp.rows; ++row) {
            for (int col = 0; col < bmp.width; ++col) {
                int x = ox + col;
                int y = oy + row;
                pixels[y * width + x] = bmp.buffer[row * bmp.pitch + col];
            }
        }

        glyphs.emplace(c, Glyph {
            {glyph->advance.x >> 6, glyph->advance.y >> 6},
            {glyph->bitmap.width, glyph->bitmap.rows},
            {glyph->bitmap_left, glyph->bitmap_top},
            {ox / static_cast<float>(width), oy / static_cast<float>(height)}
        });

        ox += bmp.width + 1;
    }

    texture = std::make_shared<Texture>(device, pixels.data(), static_cast<uint32_t>(width), static_cast<uint32_t>(height), vk::Format::eR8Srgb);

    std::cout << "Generated a " << width << "x " << height << " (" << width * height / 1024 << " kb) texture atlas." << std::endl;
}

Font::~Font() {
}

const Glyph& Font::operator[](unsigned char c) const {
    const auto it = glyphs.find(c);
    return it != glyphs.end() ? it->second : glyphs.at(NUM_GLYPHS - 1);
}
