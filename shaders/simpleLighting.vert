#version 460 core
#include "attributeIndices.vert"
#include "defines.frag"

layout(location = POSITION_ATTRIBUTE_LOCATION) in vec3 position;
layout(location = NORMAL_ATTRIBUTE_LOCATION) in vec3 normal;

out vec4 worldSpacePosition;
out vec4 lightSpacePosition;
out vec3 vs_Normal;

layout(std140, binding = UNIFORM_TRANSFORM_MATRICES_BLOCK_BINDING_POINT) uniform SharedMatrices {
    mat4 worldToClip;
    mat4 worldToCamera;
    vec4 eyePosition;
};

layout(std140, binding = UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[1] worldToDirectionalLightClips;
};

layout(std140, binding = MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT) uniform ModelMatrices { mat4 modelToWorld; };

void main(void) {
    vec3 pos = position;
    worldSpacePosition = modelToWorld * vec4(pos, 1);
    vs_Normal = normal;  // TODO multiply by inverse transpose
    gl_Position = worldToClip * worldSpacePosition;
    lightSpacePosition = worldToDirectionalLightClips[0] * worldSpacePosition;
}