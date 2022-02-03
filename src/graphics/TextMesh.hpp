#pragma once

namespace Engine {
    class Font;

    class TextMesh {
    public:
        TextMesh();
        ~TextMesh();
        TextMesh(const TextMesh&) = delete;
        TextMesh(TextMesh&&) = delete;
        TextMesh& operator=(const TextMesh&) = delete;
        TextMesh& operator=(TextMesh&&) = delete;

        void draw(const std::unique_ptr<Font>& font, const std::string& text, float x, float y, float scale) const;
    };
}

