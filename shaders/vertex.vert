#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 cameraPos;
out vec3 vs_Normal;

layout (location = 0) uniform mat4 worldToClip;
layout (location = 1) uniform mat4 worldToCamera;

void main(void) {

	vec3 pos = position;
	cameraPos = vec4(pos, 1);
	vs_Normal = normal;
	gl_Position = worldToClip * vec4(pos, 1);
}