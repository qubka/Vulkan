#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine {
	class TextMesh;
    class Device;
    class Texture;

    struct Glyph {
        glm::vec2 advance;
        glm::vec2 size;
        glm::vec2 bearing;
        glm::vec2 uv;
    };

	class Font {
	public:
		Font(Device& device, const FT_Face& face, uint32_t size);
		~Font();
        Font(const Font&) = delete;
        Font(Font&&) = delete;
        Font& operator=(const Font&) = delete;
        Font& operator=(Font&&) = delete;

        int getWidth() const { return width; };
        int getHeight() const { return height; };
        int getMetrics() const { return metrics; };
        const std::shared_ptr<Texture>& getTexture() const { return texture; };

        const Glyph& operator[](unsigned char c) const;
    private:
        int width;
        int height;
        int metrics;
        std::map<unsigned char, Glyph> glyphs;
        std::shared_ptr<Texture> texture;
    };
}
