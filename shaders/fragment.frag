#version 450 core
out vec4 color;
	
in vec4 cameraPos;
in vec3 vs_Normal;

struct Light {
	vec4 position;
	vec4 intensity;
};

layout (std140, binding=1) uniform Lights {

	Light light;
} lights;

layout (std140, binding=2) uniform Test {
	vec3 red;
	vec3 green;
	vec3 blue;
} colors;


void main(void) {
	color = vec4(colors.blue, 0);
}