#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (location = 0) uniform mat4 worldToClip;
// layout (location = 1) uniform vec4 offset;

void main(void) {
	gl_Position = worldToClip * vec4(position, 1);
	// gl_Position = vec4(0, 0, 0, 1) + offset;
}