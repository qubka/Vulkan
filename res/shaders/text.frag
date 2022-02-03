#version 450

layout (location = 0) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler samp;
layout (binding = 2) uniform texture2D textures[2];

layout (push_constant) uniform Push {
    vec4 color;
} push;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler2D(textures[1], samp), fragTexCoord).r);
    outColor = push.color * sampled;
}