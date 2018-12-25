#version 450 core
out vec4 color;

in vec4 cameraPos;

void main(void) {
	color = cameraPos;
}