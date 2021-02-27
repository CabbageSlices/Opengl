#version 460 core

layout(location = 1) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

out vec2 vs_texCoord;

layout(location = 5) uniform vec4 uniformVariable = vec4(1, 2, 3, 4);

void main(void) {
    gl_Position = vec4(position, 1) + uniformVariable;
    vs_texCoord = texCoord;
}