#version 460 core

out vec4 fragOut;

in vec2 texCoord;

uniform sampler2D s;

void main(void) {
	fragOut = vec4(1, 0, 0, 1);
	// fragOut = texture(s, texCoord);
}