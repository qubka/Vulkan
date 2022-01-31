#include "Texture.hpp"
#include "Image.hpp"

using Engine::Texture;

Texture::Texture(std::string p) : path(std::move(p)) {
    Image image{path};

}

Texture::~Texture() {
}