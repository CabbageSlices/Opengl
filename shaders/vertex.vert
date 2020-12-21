#version 460 core
#include "attributeIndices.vert"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

layout(location = POSITION_ATTRIBUTE_LOCATION) in vec3 position;
layout(location = NORMAL_ATTRIBUTE_LOCATION) in vec3 normal;
layout(location = TEXCOORD_ATTRIBUTE_LOCATION) in vec2 texCoord;

out vec4 worldSpacePosition;
out vec3 vs_Normal;
out vec2 vs_TexCoord;

// TODO  change to uniform block
layout(location = WORLD_TO_CLIP_UNIFORM_BUFFER_LOCATION) uniform mat4 worldToClip;
layout(location = WORLD_TO_CAMERA_UNIFORM_BUFFER_LOCATION) uniform mat4 worldToCamera;
layout(std140, binding = MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT) uniform ModelToWorld { mat4 modelToWorld; };

void main(void) {
    vec3 pos = position;
    worldSpacePosition = modelToWorld * vec4(pos, 1);
    vs_Normal = normal;  // TODO multiply by inverse transpose
    gl_Position = worldToClip * worldSpacePosition;
    vs_TexCoord = texCoord;
}