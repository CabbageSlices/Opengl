#version 450 core
#ifndef FRAGMENT_BASE
#define FRAGMENT_BASE
#define DIRECTIONAL

out vec4 color;

in vec4 cameraPos;
in vec3 vs_Normal;

#ifdef DIRECTIONAL
//#include "directionalLight.frag"
#elif POINT
//#include "pointLight.frag"
#endif

void main(void) {

	color = vec4(1,1,1,1);
	#ifdef COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION
	color = COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(color);
	#endif

}
#endif