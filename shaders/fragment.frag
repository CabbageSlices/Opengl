#version 450 core
#ifndef FRAGMENT_BASE
#define FRAGMENT_BASE
#define DIRECTIONAL
#define POINT

out vec4 fragOut;

in vec4 worldSpacePosition;
in vec3 vs_Normal;

#include "Lighting.frag"

void main(void) {
	vec3 normal = normalize(vs_Normal);
	vec4 baseColor = vec4(1,1,1,1);
	
	vec4 outputColor = vec4(0);
	
	#ifdef DIRECTIONAL
	outputColor += COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(normal);
	#endif

	#ifdef POINT
	outputColor += COMPUTE_POINT_LIGHT_CONTRIBUTION(normal);
	#endif

	fragOut = outputColor;
}
#endif