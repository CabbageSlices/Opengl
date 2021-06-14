#version 460 core

out vec4 fragOut;

in vec2 texCoord;

layout(binding = 1) uniform sampler2D s;
layout(binding = 0) uniform samplerCubeArray s1;

float near = 0.01;
float far = 30.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void) {
    // float depth = texture(s, texCoord).r;
    // fragOut = vec4(vec3(depth), 1);
    // fragOut = vec4(vec3(LinearizeDepth(depth) / far), 1);

    // float depth = texture(s1, vec3(texCoord, 0)).r;
    // fragOut = vec4(vec3(LinearizeDepth(depth) / far), 1);

    fragOut = texture(s1, vec4(texCoord.x, texCoord.y, -3, 0));
}