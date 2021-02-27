#version 460 core

out vec4 fragOut;

// TODO send material data to lighting calculation so that diffuse color isn't used for specular highlight
void main(void) { fragOut = vec4(1); }