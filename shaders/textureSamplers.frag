#ifndef TEXTURE_SAMPLERS
#define TEXTURE_SAMPLERS
#include "samplerBindings.frag"

layout(binding = DIFFUSE_TEXTURE_TEXTURE_UNIT) uniform sampler2D diffuseTextureSampler;

layout(binding = SHADOWMAP_TEXTURE_UNIT) uniform sampler2D shadowMapSampler;

#endif