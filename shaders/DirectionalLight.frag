#ifdef FRAGMENT_BASE
#define MAX_DIRECTIONAL_LIGHTS 2

struct DirectionalLight {
    vec3 direction;
    vec4 intensity;
};

layout (std140, binding = 4) uniform DirectionalLights {
    DirectionalLight directionalLights[2];
};

vec4 calculateDirectionalLightIntensity(DirectionalLight directionalLight, vec3 surfaceNormal) {
	vec3 dirToLight = -directionalLight.direction;
	
	return clamp(dot(surfaceNormal, dirToLight), 0, 1) * directionalLight.intensity;
}

vec4 calculateDirectionalLightsIntensity(vec3 surfaceNormal) {

    vec4 lightTotal = vec4(0, 0, 0, 0);
    
    for(int i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i) {

        lightTotal = lightTotal + calculateDirectionalLightIntensity(directionalLights[i], surfaceNormal);
    }

    return lightTotal;
}

#define COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(surfaceColor, surfaceNormal) calculateDirectionalLightsIntensity(surfaceNormal) * surfaceColor

#endif