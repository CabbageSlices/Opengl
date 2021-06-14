#version 460 core
#include "defines.frag"
#include "uniformBlockBindings.vert"

in flat int pointLightIndexForDepthCalculation;
in vec4 worldSpacePosition;
out vec4 fragOut;

float near = 0.01;
float far = 200.0;

struct Light {
    vec4 position;
    vec4 direction;
    vec4 intensity;
    vec4 rangeAndPadding;
};

layout(std140, binding = POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT) uniform PointLights {
    Light pointLights[MAX_POINT_LIGHTS];
};

// can't use same layer for both directional and point light output since they're two separate textures
// this means that the layer numbers aren't continuous, so they must be rendered in separate passes
layout(std140, binding = UNIFORM_SHADOW_MAP_FLAGS_BINDING_POINT) uniform ShadowMapFlags {
    int currentLightPass;  // 0 = direcional light, 1 = point light, MUST USE becuase point light depth is written as linear
                           // depth in world space, but direcitonal light is written
};

layout(std140, binding = UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT) uniform PointLightData {
    mat4[6 * MAX_POINT_LIGHTS] worldToPointLightClips;  // 6 matrices per point light.
};

layout(std140, binding = LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT) uniform LightBatchInfo {
    int numDirectionalLightsInBatch;
    int numPointLightsInBatch;
    int batchIndex;
};

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void) {
    // MUST WRITE TO GL_FRAGDEPTH https://www.khronos.org/opengl/wiki/Fragment_Shader/Defined_Outputs since it was written to
    // for point light
    gl_FragDepth = gl_FragCoord.z;
    if (currentLightPass == 0) {
        // directional light, we us ethe depth buffer value so no need to override
        float linearDepth = length(worldSpacePosition);
        fragOut = linearDepth > 50 ? vec4(0, 1, 0, 1) : vec4(0, 0, 1, 1);
    } else {
        // point light, we want to output linear depth values
        // we also want to normalize the depth values since our depth value needs to be a range from 0-1

        Light light = pointLights[pointLightIndexForDepthCalculation];

        float linearDepth = length(worldSpacePosition - light.position);

        float pointLightProjectionFarPlane = light.rangeAndPadding.x * 2;

        // divide by far plane to map to [0, 1] range
        // we can use the light range since it does have a limited range
        gl_FragDepth = linearDepth / pointLightProjectionFarPlane;
    }
    // fragOut = vec4(linearDepth / far);
    // fragOut = vec4(gl_Layer, 0, 0, 1);

    // gl_FragDepth = val;
    // for directional light we can just use the output to the depth buffer calculated from the previous stage, so no
    // need ot output
    // gl_FragDepth = 0;
}