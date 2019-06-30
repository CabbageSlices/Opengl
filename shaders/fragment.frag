#version 450 core
#ifndef FRAGMENT_BASE
#define FRAGMENT_BASE
#define DIRECTIONAL
#define POINT

out vec4 fragOut;

in vec4 cameraPos;
in vec3 vs_Normal;

#ifdef DIRECTIONAL
//#include "directionalLight.frag"
#endif
#ifdef POINT
//#include "pointLight.frag"
#endif

void main(void) {
	vec3 normal = normalize(vs_Normal);
	vec4 baseColor = vec4(1,1,1,1);
	
	vec4 outputColor = vec4(0);
	
	#ifdef COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION
	outputColor += COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(baseColor, normal);
	#endif

	#ifdef COMPUTE_POINT_LIGHT_CONTRIBUTION
	outputColor += COMPUTE_POINT_LIGHT_CONTRIBUTION(baseColor, normal);
	#endif

	fragOut = outputColor;
}
#endif