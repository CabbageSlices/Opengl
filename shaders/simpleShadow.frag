#version 460 core

out vec4 fragOut;
in vec4 worldSpacePosition;
in vec4 clipPos;

float near = 0.01;
float far = 23.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void) {
    float linearDepth = LinearizeDepth(gl_FragCoord.z) / far;
    fragOut = vec4(gl_FragCoord.z, 0, 0, 1);

    fragOut = vec4((clipPos.z / clipPos.w) * 0.5 + 0.5, 0, 0, 1);
    fragOut = vec4(1, 0, 0, 1);
}