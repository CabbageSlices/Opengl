#ifdef FRAGMENT_BASE
#define MAX_POINT_LIGHTS 4

struct PointLight {
    vec4 position;
    vec4 intensity;
    float range;
};

layout (std140, binding = 5) uniform PointLights {
    PointLight pointLights[4];
};

vec4 calculatePointLightIntensity(PointLight pointLight, vec3 surfaceNormal) {
	vec4 dirToLight = pointLight.position - cameraPos;
	float distSquared = dot(dirToLight, dirToLight);

    //make sure range isn't 0
    float range = max(pointLight.range, 0.1);
	float attenuation = clamp(1 - distSquared / (range * range), 0, 1);
	attenuation *= attenuation;

	return clamp(dot(surfaceNormal, dirToLight.xyz), 0, 1) * pointLight.intensity * attenuation;
}

vec4 calculatePointLightsIntensity(vec3 surfaceNormal) {

    vec4 lightTotal = vec4(0, 0, 0, 0);
    
    // return pointLights[0].position;
    // return calculatePointLightIntensity(pointLights[0], surfaceNormal);
    for(int i = 0; i < MAX_POINT_LIGHTS; ++i) {

        lightTotal = lightTotal + calculatePointLightIntensity(pointLights[i], surfaceNormal);
    }

    return lightTotal;
}

#define COMPUTE_POINT_LIGHT_CONTRIBUTION(surfaceColor, surfaceNormal) calculatePointLightsIntensity(surfaceNormal) * surfaceColor
#endif