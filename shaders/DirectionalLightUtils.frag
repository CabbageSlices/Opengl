#version 450 core

struct Light {
    vec3 direction;
    vec4 intensity;
};

in vec4 cameraPos;
in vec3 vs_Normal;

vec4 calculateLightIntensity(Light directionalLight) {
	vec3 dirToLight = -directionalLight.direction;
	
	return dot(vs_Normal, dirToLight) * directionalLight.intensity;
}