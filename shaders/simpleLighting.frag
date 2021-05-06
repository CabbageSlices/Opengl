#version 460 core

#define DIFFUSE_TEXTURE_TEXTURE_UNIT 0
#define MATERIAL_UNIFORM_BLOCK_BINDING_POINT 1
#define SHADOWMAP_TEXTURE_UNIT 2
#define DIRECTIONAL_LIGHTS_SHADOWMAP_TEXTURE_UNIT 3
#define UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT 7
#define DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT 4

layout(binding = SHADOWMAP_TEXTURE_UNIT) uniform sampler2D shadowMapSampler;

out vec4 fragOut;

in vec4 worldSpacePosition;
in vec3 vs_Normal;
in vec4 lightSpacePosition;

struct DirectionalLight {
    vec4 position;
    vec3 direction;
    vec4 intensity;
};

layout(std140, binding = MATERIAL_UNIFORM_BLOCK_BINDING_POINT) uniform DiffuseMaterial {
    vec4 diffuseColor;          // offset 0
    float specularCoefficient;  // offset 16
};

layout(std140, binding = UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[1] worldToDirectionalLightClips;
};

layout(std140, binding = DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform DirectionalLights {
    DirectionalLight directionalLights[1];
};

// TODO send material data to lighting calculation so that diffuse color isn't used for specular highlight
void main(void) {
    vec3 normal = normalize(vs_Normal);

    mat4 lightClip = worldToDirectionalLightClips[0];

    DirectionalLight light = directionalLights[0];

    vec3 dirToLight = normalize(vec3(light.position - worldSpacePosition));

    float nDotL = clamp(dot(dirToLight, normal), 0, 1);

    vec4 diffuse = nDotL * light.intensity * diffuseColor;

    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords = projCoords * 0.5 + 0.5;

    // float closestDepth = texture(shadowMapSampler, projCoords.xy).r;
    // float currentDepth = projCoords.z;

    // float shadow = closestDepth < currentDepth ? 0 : 1;

    // fragOut = (diffuse + vec4(0.1) * diffuseColor) * shadow;
    // fragOut = vec4(closestDepth, 0, 0, 1);
    fragOut = texture(shadowMapSampler, projCoords.xy);
}