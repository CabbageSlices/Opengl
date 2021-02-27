#version 460 core
#ifndef FRAGMENT_BASE
#define FRAGMENT_BASE
#define DIRECTIONAL
#define POINT
#include "diffuseMaterial.frag"
#include "textureSamplers.frag"

layout(std140, binding = UNIFORM_TRANSFORM_MATRICES_BLOCK_BINDING_POINT) uniform SharedMatrices {
    mat4 worldToClip;
    mat4 worldToCamera;
    vec4 eyePosition;
};

out vec4 fragOut;

in vec4 worldSpacePosition;
in vec3 vs_Normal;
in vec2 vs_TexCoord;

uniform sampler2D s;

#include "Lighting.frag"

// TODO send material data to lighting calculation so that diffuse color isn't used for specular highlight
void main(void) {
    vec3 normal = normalize(vs_Normal);

    vec4 outputColor = vec4(0);

    vec4 diffuseTextureSample = diffuseTexture_Provided == 1 ? texture(diffuseTexture_Sampler, vs_TexCoord) : vec4(1);
    vec4 totalMaterialColor = diffuseColor * diffuseTextureSample;

    Material material = Material(totalMaterialColor, specularCoefficient);
#ifdef DIRECTIONAL
    outputColor += COMPUTE_DIRECTIONAL_LIGHT_CONTRIBUTION(normal, material);
#endif

#ifdef POINT
    outputColor += COMPUTE_POINT_LIGHT_CONTRIBUTION(normal, material);
#endif

    fragOut = outputColor;
    // fragOut = diffuseTextureSample;
    // fragOut = texture(s, vs_TexCoord) * outputColor;
    // fragOut = vec4(diffuseTextureSample, 0, 0, 1);
}
#endif