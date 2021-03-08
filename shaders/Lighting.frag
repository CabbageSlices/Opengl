#ifdef FRAGMENT_BASE
#include "defines.frag"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

struct DirectionalLight {
    vec4 position;
    vec3 direction;
    vec4 intensity;
};

struct PointLight {
    vec4 position;
    vec4 intensity;
    vec4 range;  // only the x value is used for the range. Make vec4 in order to make array structure better
};

// final material data after texutre sampling and stuff, send to lighting computations to
// compute lighting using actual colour and material data
struct Material {
    vec4 diffuseColor;
    float specularCoefficient;
};

layout(std140, binding = DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform DirectionalLights {
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140, binding = POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform PointLights {
    PointLight pointLights[MAX_POINT_LIGHTS];
};

float computeSpecularFactor(vec3 dirToLight, vec3 surfaceNormal, float specularCoefficient) {
    vec3 toEye = normalize(eyePosition.xyz - worldSpacePosition.xyz);
    vec3 halfVector = normalize(toEye + dirToLight);
    float nDotH = clamp(dot(surfaceNormal, halfVector), 0, 1);

    return pow(nDotH, specularCoefficient);
}

vec4 calculateDirectionalLightIntensity(DirectionalLight directionalLight, vec3 surfaceNormal, Material material) {
    vec3 dirToLight = normalize(-directionalLight.direction);

    float specularFactor = computeSpecularFactor(dirToLight, surfaceNormal, material.specularCoefficient);
    float nDotL = clamp(dot(surfaceNormal, dirToLight), 0, 1);

    vec4 diffuseTerm = nDotL * directionalLight.intensity * material.diffuseColor;
    // multiply by the step that way specular highlights only occur if there is diffuse lighting as well
    vec4 specularTerm = directionalLight.intensity * specularFactor * step(0.001, nDotL);
    return diffuseTerm + specularTerm;
}

vec4 calculateDirectionalLightsContribution(vec3 surfaceNormal, Material material) {
    vec4 lightTotal = vec4(0, 0, 0, 0);

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i) {
        vec4 contribution = calculateDirectionalLightIntensity(directionalLights[i], surfaceNormal, material);

        if (i == 0) {
            vec3 projCoordinates = lightSpacePosition.xyz / lightSpacePosition.w;
            projCoordinates = projCoordinates * 0.5 + 0.5;
            float closestDepth = texture(shadowMapSampler, projCoordinates.xy).r;
            float currentDepth = projCoordinates.z - 0.005;

            contribution *= currentDepth > closestDepth ? 0 : 1;
        }
        lightTotal += contribution;
    }

    return lightTotal;
}

vec4 calculatePointLightContribution(PointLight pointLight, vec3 surfaceNormal, Material material) {
    vec3 dirToLight = pointLight.position.xyz - worldSpacePosition.xyz;
    float distSquared = dot(dirToLight, dirToLight);

    dirToLight = normalize(dirToLight);

    // make sure range isn't 0
    float range = max(pointLight.range.x, 0.01);
    float attenuation = clamp(1 - distSquared / (range * range), 0, 1);
    attenuation *= attenuation;

    float nDotL = clamp(dot(surfaceNormal, dirToLight), 0, 1);

    vec4 diffuseTerm = nDotL * pointLight.intensity * material.diffuseColor;

    float specularFactor = computeSpecularFactor(dirToLight, surfaceNormal, material.specularCoefficient);
    vec4 specularTerm = pointLight.intensity * specularFactor * step(0.001, nDotL);
    return diffuseTerm + specularTerm;
}

vec4 calculatePointLightsContribution(vec3 surfaceNormal, Material material) {
    vec4 lightTotal = vec4(0, 0, 0, 0);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        lightTotal = lightTotal + calculatePointLightContribution(pointLights[i], surfaceNormal, material);
    }

    return lightTotal;
}

#define COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(surfaceNormal, material) \
    calculateDirectionalLightsContribution(surfaceNormal, material)
#define COMPUTE_POINT_LIGHT_CONTRIBUTION(surfaceNormal, material) calculatePointLightsContribution(surfaceNormal, material)
#endif