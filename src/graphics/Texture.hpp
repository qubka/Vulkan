#pragma once

namespace Engine {
    class Texture {
        std::string path;
        //aiTextureType aiType{aiTextureType_DIFFUSE};
    public:
        Texture(std::string path);
        ~Texture();
    };
}
