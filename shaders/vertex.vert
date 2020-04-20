#version 450 core
#include "uniformBlockBindings.vert"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 worldSpacePosition;
out vec3 vs_Normal;

layout (location = 0) uniform mat4 worldToClip;
layout (location = 1) uniform mat4 worldToCamera;
layout (std140, binding = MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT) uniform ModelToWorld  {
	mat4 modelToWorld;
};

void main(void) {

	vec3 pos = position;
	worldSpacePosition = modelToWorld * vec4(pos, 1);
	vs_Normal = normal;
	gl_Position = worldToClip * worldSpacePosition;
}