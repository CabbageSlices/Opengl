#ifndef DIFFUSE_MATERIAL
#define DIFFUSE_MATERIAL
#include "uniformBlockBindings.vert"

layout(std140, binding = MATERIAL_UNIFORM_BLOCK_BINDING_POINT) uniform DiffuseMaterial {
    vec4 diffuseColor;
    float specularCoefficient;
    bool isDiffuseTextureAvailable;
};

#endif