#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

//layout (binding = 1) uniform sampler samp;
//layout (binding = 2) uniform texture2D textures[2];

void main() {
    //outColor = texture(sampler2D(textures[0], samp), fragTexCoord);
    outColor =  vec4(fragColor, 1.0);
}