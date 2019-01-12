#version 450 core
out vec4 color;
	
in vec4 cameraPos;
in vec3 vs_Normal;

void main(void) {
	color = vec4(vs_Normal, 1);
}