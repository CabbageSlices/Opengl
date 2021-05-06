#version 460 core
#include "uniformBlockBindings.vert"

in vec4 worldSpacePosition;
out vec4 fragOut;

float near = 0.01;
float far = 200.0;

layout(std140, binding = UNIFORM_SHADOW_MAP_FLAGS_BINDING_POINT) uniform ShadowMapFlags {
    int currentLightPass;  // 0 = direcional light, 1 = point light
};

uniform int currentLightPass1;

layout(std140, binding = UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT) uniform PointLightData {
    mat4[6] worldToPointLightClip;  // one matrix per face of a single point light
    vec4 lightPos;
};

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void) {
    // fragOut = vec4(1, 0, 0, 1);
    // float depth = texture(s, texCoord).r;
    // fragOut = vec4(vec3(depth), 1);
    // fragOut = vec4(vec3(LinearizeDepth(depth) / far), 1);
    // fragOut = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1);
    // fragOut = vec4(vec3(gl_FragCoord.z), 1);

    float val = gl_FragDepth;
    if (currentLightPass1 == 1) {
        // for point light we want to output linear depth of fragment relative to light
        float linearDepth = length(worldSpacePosition - lightPos);
        val = linearDepth / far;
    }
    if (currentLightPass1 == 0) {
        // float linearDepth = LinearizeDepth(gl_FragCoord.z);
        // gl_FragDepth = linearDepth / far;
        // gl_FragDepth = 0;
        val = gl_FragDepth;
        // fragOut = vec4(0, 1, 0, 1);
    }

    float linearDepth = length(worldSpacePosition - lightPos);
    fragOut = linearDepth > 50 ? vec4(0, 1, 0, 1) : vec4(0, 0, 1, 1);
    // fragOut = vec4(linearDepth / far);
    // fragOut = vec4(gl_Layer, 0, 0, 1);

    // gl_FragDepth = val;
    // for directional light we can just use the output to the depth buffer calculated from the previous stage, so no
    // need ot output
    // gl_FragDepth = 0;
}