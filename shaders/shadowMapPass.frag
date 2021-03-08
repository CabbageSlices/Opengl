#version 460 core

out vec4 fragOut;

float near = 0.01;
float far = 30.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void) {
    // fragOut = vec4(1, 0, 0, 1);
    // float depth = texture(s, texCoord).r;
    // fragOut = vec4(vec3(depth), 1);
    // fragOut = vec4(vec3(LinearizeDepth(depth) / far), 1);
    fragOut = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1);
    // fragOut = vec4(vec3(gl_FragCoord.z), 1);
}