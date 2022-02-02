#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform Ubo {
    mat4 viewProj;
} ubo;

void main() {
    gl_Position = ubo.viewProj * vec4(position, 1.0);
    fragColor = color;
    fragTexCoord = uv;
}