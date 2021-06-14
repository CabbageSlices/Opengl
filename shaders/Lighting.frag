#ifdef FRAGMENT_BASE
#include "defines.frag"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

struct Light {
    vec4 position;
    vec4 direction;
    vec4 intensity;
    vec4 rangeAndPadding;
};

layout(std140, binding = UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[MAX_DIRECTIONAL_LIGHTS] worldToDirectionalLightClips;
};

// final material data after texutre sampling and stuff, send to lighting computations to
// compute lighting using actual colour and material data
struct Material {
    vec4 diffuseColor;
    float specularCoefficient;
};

layout(std140, binding = DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform DirectionalLights {
    Light directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140, binding = POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform PointLights {
    Light pointLights[MAX_POINT_LIGHTS];
};

layout(std140, binding = LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT) uniform LightBatchInfo {
    int numDirectionalLightsInBatch;
    int numPointLightsInBatch;
    int batchIndex;
};

float computeSpecularFactor(vec3 dirToLight, vec3 surfaceNormal, float specularCoefficient) {
    vec3 toEye = normalize(eyePosition.xyz - worldSpacePosition.xyz);
    vec3 halfVector = normalize(toEye + dirToLight);
    float nDotH = clamp(dot(surfaceNormal, halfVector), 0, 1);

    return pow(nDotH, specularCoefficient);
}

vec4 calculateDirectionalLightIntensity(Light directionalLight, vec3 surfaceNormal, Material material) {
    vec3 dirToLight = normalize(-directionalLight.direction.xyz);

    float specularFactor = computeSpecularFactor(dirToLight, surfaceNormal, material.specularCoefficient);
    float nDotL = clamp(dot(surfaceNormal, dirToLight), 0, 1);

    vec4 diffuseTerm = nDotL * directionalLight.intensity * material.diffuseColor;
    // multiply by the step that way specular highlights only occur if there is diffuse lighting as well
    vec4 specularTerm = directionalLight.intensity * specularFactor * step(0.001, nDotL);
    return diffuseTerm + specularTerm;
}

vec4 calculateDirectionalLightsContribution(vec3 surfaceNormal, Material material) {
    vec4 lightTotal = vec4(0, 0, 0, 0);

    for (int i = 0; i < numDirectionalLightsInBatch; ++i) {
        vec4 contribution = calculateDirectionalLightIntensity(directionalLights[i], surfaceNormal, material);

        vec4 lightSpacePosition = worldToDirectionalLightClips[i] * worldSpacePosition;
        vec3 projCoordinates = lightSpacePosition.xyz / lightSpacePosition.w;
        projCoordinates = projCoordinates * 0.5 + 0.5;
        float closestDepth = texture(directionalLightsShadowMapsSampler, vec3(projCoordinates.xy, i)).r;
        float currentDepth = projCoordinates.z;

        contribution *= currentDepth > closestDepth ? 0 : 1;
        lightTotal += contribution;
    }

    return lightTotal;
}

vec4 calculatePointLightContribution(Light pointLight, vec3 surfaceNormal, Material material) {
    vec3 dirToLight = pointLight.position.xyz - worldSpacePosition.xyz;
    float distSquared = dot(dirToLight, dirToLight);

    dirToLight = normalize(dirToLight);

    // make sure range isn't 0
    float range = max(pointLight.rangeAndPadding.x, 0.01);
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

    for (int i = 0; i < numPointLightsInBatch; ++i) {
        Light light = pointLights[i];

        vec3 lightToFrag = (worldSpacePosition - light.position).xyz;
        float nearestDepth = texture(pointLightsShadowMapsSampler, vec4(lightToFrag, i)).r;

        float currentDepth =
            length(lightToFrag) /
            (light.rangeAndPadding.x *
             2);  // depth is mapped to the range [0, lightRange * 2] in shadow map, so we need to do hte same to compare

        float shadow = nearestDepth <= currentDepth ? 0 : 1;
        lightTotal = lightTotal + calculatePointLightContribution(pointLights[i], surfaceNormal, material) * shadow;
        // lightTotal = vec4(nearestDepth);
    }

    return lightTotal;
}

#define COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(surfaceNormal, material) \
    calculateDirectionalLightsContribution(surfaceNormal, material)
#define COMPUTE_POINT_LIGHT_CONTRIBUTION(surfaceNormal, material) calculatePointLightsContribution(surfaceNormal, material)
#endif