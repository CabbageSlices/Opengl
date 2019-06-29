#version 450 core

struct Light {
    vec4 position;
    vec4 intensity;
};

in vec4 cameraPos;
in vec3 vs_Normal;

vec4 calculateLightIntensity(Light pointLight) {
	vec4 dirToLight = pointLight.position - cameraPos;
	float distSquared = dot(dirToLight, dirToLight);

	float attenuation = clamp(1 - distSquared / (4), 0, 1);
	attenuation *= attenuation;

	return dot(vs_Normal, dirToLight.xyz) * pointLight.intensity * attenuation;
}