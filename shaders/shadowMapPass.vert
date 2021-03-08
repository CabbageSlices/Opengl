#version 460 core
#include "attributeIndices.vert"
#include "defines.frag"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

layout(location = POSITION_ATTRIBUTE_LOCATION) in vec3 position;
layout(location = NORMAL_ATTRIBUTE_LOCATION) in vec3 normal;

out vec4 worldSpacePosition;

layout(std140, binding = UNIFORM_TRANSFORM_MATRICES_BLOCK_BINDING_POINT) uniform SharedMatrices {
    mat4 worldToClip;
    mat4 worldToCamera;
    vec4 eyePosition;
};

layout(std140, binding = MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT) uniform ModelMatrices { mat4 modelToWorld; };

layout(std140, binding = UNIFORM_LIGHT_MATRICES_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[MAX_DIRECTIONAL_LIGHTS] worldToDirectionalLightClips;
};

void main(void) {
    vec3 pos = position;
    worldSpacePosition = modelToWorld * vec4(pos, 1);
    gl_Position = worldToDirectionalLightClips[0] * worldSpacePosition;
}