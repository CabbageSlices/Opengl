#ifdef FRAGMENT_BASE

struct DirectionalLight {
    vec3 direction;
    vec4 intensity;
};

layout (std140, binding = 4) uniform DirectionalLights {
    DirectionalLight directionalLights[2];
};

vec4 calculateDirectionalLightIntensity(DirectionalLight directionalLight) {
	vec3 dirToLight = -directionalLight.direction;
	
	return clamp(dot(vs_Normal, dirToLight), 0, 1) * directionalLight.intensity;
}

vec4 calculateDirectionalLightsIntensity() {
    // return vec4(4,4,4,4) + vec4(4,4,4,4) + vec4(4,4,4,4);
    vec4 lightTotal = vec4(0, 0, 0, 0);
    // lightTotal = vec4(0, 0, 0, 0) + calculateDirectionalLightIntensity(directionalLights[1])+ calculateDirectionalLightIntensity(directionalLights[0]);
    // return lightTotal;
    // return calculateDirectionalLightIntensity(directionalLights[1]) + calculateDirectionalLightIntensity(directionalLights[1]);
    // return vec4(dot(-directionalLights[1].direction, vs_Normal)) * directionalLights[1].intensity;
    for(int i = 0; i < 2; ++i) {

        lightTotal = lightTotal + calculateDirectionalLightIntensity(directionalLights[i]);
    }

    return lightTotal;
}

#define COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(baseColor) calculateDirectionalLightsIntensity() * baseColor

#endif