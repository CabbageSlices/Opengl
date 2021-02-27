#ifndef DIFFUSE_MATERIAL
#define DIFFUSE_MATERIAL
#include "samplerBindings.frag"
#include "uniformBlockBindings.vert"

layout(std140, binding = MATERIAL_UNIFORM_BLOCK_BINDING_POINT) uniform DiffuseMaterial {
    vec4 diffuseColor;            // offset 0
    float specularCoefficient;    // offset 16
    int diffuseTexture_Provided;  // offset 20
};

layout(binding = DIFFUSE_TEXTURE_TEXTURE_UNIT) uniform sampler2D diffuseTexture_Sampler;

#endif