#version 460 core
#include "attributeIndices.vert"
#include "defines.frag"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec4 worldSpacePosition;

layout(std140, binding = UNIFORM_SHADOW_MAP_FLAGS_BINDING_POINT) uniform ShadowMapFlags {
    int currentLightPass;  // 0 = direcional light, 1 = point light
};

layout(std140, binding = UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[MAX_DIRECTIONAL_LIGHTS] worldToDirectionalLightClips;
};

layout(std140, binding = UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT) uniform PointLightData {
    mat4[6] worldToPointLightClip;  // one matrix per face of a single point light
    vec4 lightPos;
};

layout(std140, binding = LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT) uniform LightBatchInfo {
    int numDirectionalLightsInBatch;
    int numPointLightsInBatch;
    int batchIndex;
};

void main() {
    // directional light, output one face per directional light layer

    for (int light = 0; light < numDirectionalLightsInBatch; ++light) {
        for (int i = 0; i < 3; ++i) {
            gl_Layer = light;
            worldSpacePosition = gl_in[i].gl_Position;
            gl_Position = worldToDirectionalLightClips[light] * worldSpacePosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}