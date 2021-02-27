#version 460 core

in vec2 vs_texCoord;
out vec4 fragOut;

layout(std140, binding = 9) uniform TestBlock {
    vec4 dat1;   // offset 0
    vec3 dat2;   // offset 16
    int dat3;    // offset 28
    float dat4;  // offset 32
    vec4 dat5;   // offset 48
};

layout(std140, binding = 21) uniform TestMaterial {
    vec4 diffuseColor;             // offset 0
    float specularCoefficient;     // offset 16
    bool diffuseTexture_Provided;  // offset 20
    bool otherTexture_Provided;    // offset 24
};

layout(binding = 23) uniform sampler2D diffuseTexture_Sampler;
layout(binding = 2) uniform sampler2D otherTexture_Sampler;

// TODO send material data to lighting calculation so that diffuse color isn't used for specular highlight
void main(void) {
    vec4 results = texture(diffuseTexture_Sampler, vs_texCoord) + texture(otherTexture_Sampler, vs_texCoord);
    fragOut = dat1 + dat5 + vec4(dat2, 1) + vec4(dat3) + vec4(dat4) + diffuseColor + vec4(specularCoefficient) + results;
}