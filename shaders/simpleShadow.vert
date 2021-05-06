#version 460 core
#include "attributeIndices.vert"
#include "defines.frag"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

out vec4 worldPosition;
out vec4 clipPos;

layout(location = POSITION_ATTRIBUTE_LOCATION) in vec3 position;
layout(std140, binding = MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT) uniform ModelMatrices { mat4 modelToWorld; };

layout(std140, binding = UNIFORM_LIGHT_MATRICES_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[MAX_DIRECTIONAL_LIGHTS] worldToDirectionalLightClips;
};

void main(void) {
    vec3 pos = position;
    worldPosition = modelToWorld * vec4(position, 1);
    gl_Position = worldToDirectionalLightClips[0] * worldPosition;
    clipPos = gl_Position;
}