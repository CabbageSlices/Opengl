#version 460 core
#include "attributeIndices.vert"
#include "defines.frag"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_POINT_LIGHTS * 6 * 3) out;

out vec4 worldSpacePosition;
out flat int pointLightIndexForDepthCalculation;

// can't use same layer for both directional and point light output since they're two separate textures
// this means that the layer numbers aren't continuous, so they must be rendered in separate passes
layout(std140, binding = UNIFORM_SHADOW_MAP_FLAGS_BINDING_POINT) uniform ShadowMapFlags {
    int currentLightPass;  // 0 = direcional light, 1 = point light, MUST USE becuase point light depth is written as linear
                           // depth in world space, but direcitonal light is written
};

layout(std140, binding = UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT) uniform LightMatrices {
    mat4[MAX_DIRECTIONAL_LIGHTS] worldToDirectionalLightClips;
};

layout(std140, binding = UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT) uniform PointLightData {
    mat4[6 *
         MAX_POINT_LIGHTS] worldToPointLightClips;  // 6 matrices per point light. or in other words, 1 matrix per layer-face
};

layout(std140, binding = LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT) uniform LightBatchInfo {
    int numDirectionalLightsInBatch;
    int numPointLightsInBatch;
    int batchIndex;
};

void main() {
    if (currentLightPass == 0) {
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
    } else {
        // per pointlight
        for (int light = 0; light < numPointLightsInBatch; ++light) {
            // int light = 1;
            // per face of pointlight
            for (int face = 0; face < 6; ++face) {
                // output each triangel per pointlight face

                for (int i = 0; i < 3; ++i) {
                    // this way frag sahder knows w hich light to use when calculating the depth of the fragment in light
                    // space
                    pointLightIndexForDepthCalculation = light;

                    // layer should be set to layer face as per the wiki
                    // https://www.khronos.org/opengl/wiki/Cubemap_Texture#Cubemap_array_textures
                    gl_Layer = light * 6 + face;
                    worldSpacePosition = gl_in[i].gl_Position;

                    // this position wont be used for dpeth testing since it will be overwritten in the fragment shader
                    // but we need to set it that we can get hte correct fragment position
                    gl_Position = worldToPointLightClips[gl_Layer] * worldSpacePosition;
                    EmitVertex();
                }
                EndPrimitive();
            }
        }
    }
}