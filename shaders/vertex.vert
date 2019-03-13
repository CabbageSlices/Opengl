#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 cameraPos;
out vec3 vs_Normal;

layout (location = 0) uniform mat4 worldToClip;
layout (location = 1) uniform mat4 worldToCamera;
// layout (location = 1) uniform vec4 offset;

layout(std140, binding = 0) uniform CameraBlock {
	
	mat4 worldToClip;
	mat4 worldToCamera;

} camera;

void main(void) {

	vec3 pos = position;
	// vec3 pos = position * 0.5 + vec3(4, 0, -10);
	cameraPos = vec4(pos, 1);
	vs_Normal = normal;
	gl_Position = worldToClip * vec4(pos, 1);
	// gl_Position = vec4(0, 0, 0, 1) + offset;
}