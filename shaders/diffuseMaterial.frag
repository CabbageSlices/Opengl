#ifndef DIFFUSE_MATERIAL
#define DIFFUSE_MATERIAL

layout(std140, binding = 1) uniform DiffuseMaterial {
    vec4 diffuseColor;
};

#endif