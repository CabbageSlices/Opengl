#ifdef FRAGMENT_BASE
#include "defines.frag"
#include "diffuseMaterial.frag"
#include "uniformBlockBindings.vert"

struct DirectionalLight {
  vec3 direction;
  vec4 intensity;
};

struct PointLight {
	vec4 position;
	vec4 intensity;
	float range;
};

layout (std140, binding = DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform DirectionalLights {
  DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout (std140, binding = POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform PointLights {
  PointLight pointLights[MAX_POINT_LIGHTS];
};

vec4 calculateDirectionalLightIntensity(DirectionalLight directionalLight, vec3 surfaceNormal) {
	vec3 dirToLight = -directionalLight.direction;
	
	return clamp(dot(surfaceNormal, dirToLight), 0, 1) * directionalLight.intensity;
}

vec4 calculateDirectionalLightsIntensity(vec3 surfaceNormal) {

	vec4 lightTotal = vec4(0, 0, 0, 0);
	
	for(int i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i) {

			lightTotal += calculateDirectionalLightIntensity(directionalLights[i], surfaceNormal) ;
	}

	return lightTotal;
}

vec4 calculatePointLightIntensity(PointLight pointLight, vec3 surfaceNormal) {
	vec4 dirToLight = pointLight.position - worldSpacePosition;
	float distSquared = dot(dirToLight, dirToLight);

  //make sure range isn't 0
  float range = max(pointLight.range, 0.01);
	float attenuation = clamp(1 - distSquared / (range * range), 0, 1);
	attenuation *= attenuation;

	return clamp(dot(surfaceNormal, dirToLight.xyz), 0, 1) * pointLight.intensity * attenuation;
}

vec4 calculatePointLightsIntensity(vec3 surfaceNormal) {

	vec4 lightTotal = vec4(0, 0, 0, 0);
	
	for(int i = 0; i < MAX_POINT_LIGHTS; ++i) {

			lightTotal = lightTotal + calculatePointLightIntensity(pointLights[i], surfaceNormal);
	}

	return lightTotal;
}

#define COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(surfaceNormal) calculateDirectionalLightsIntensity(surfaceNormal)
#define COMPUTE_POINT_LIGHT_CONTRIBUTION(surfaceNormal) calculatePointLightsIntensity(surfaceNormal)
#endif