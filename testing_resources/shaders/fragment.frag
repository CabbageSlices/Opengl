#version 460 core

in vec2 vs_texCoord;
out vec4 fragOut;

layout(std140, binding = 9) uniform TestBlock {
    vec4 dat1;   // offset 0, index 0
    vec3 dat2;   // offset 16, i 1
    int dat3;    // offset 28, index 2
    float dat4;  // offset 32, 3
    vec4 dat5;   // offset 48, 4
};

layout(std140, binding = 21) uniform TestMaterial {
    vec4 diffuseColor;             // offset 0
    float specularCoefficient;     // offset 16
    bool diffuseTexture_Provided;  // offset 20
    bool otherTexture_Provided;    // offset 24
    vec4 randomPaddingToIncreaseSize1;
    vec4 randomPaddingToIncreaseSize2;
    vec4 randomPaddingToIncreaseSize3;
};

layout(std140, binding = 22) uniform TestMaterial2 {
    vec4 diffuseColor1;             // offset 0
    float specularCoefficient1;     // offset 16
    bool diffuseTexture1_Provided;  // offset 20
    bool otherTexture1_Provided;    // offset 24
    vec4 randomPaddingToIncreaseSize11;
    vec4 randomPaddingToIncreaseSize21;
    vec4 randomPaddingToIncreaseSize31;
}
testMaterial2;

layout(binding = 23) uniform sampler2D diffuseTexture_Sampler;
layout(binding = 2) uniform sampler2D otherTexture_Sampler;

layout(binding = 24) uniform sampler2D diffuseTexture1_Sampler;
layout(binding = 25) uniform sampler2D otherTexture1_Sampler;

// TODO send material data to lighting calculation so that diffuse color isn't used for specular highlight
void main(void) {
    vec4 results = texture(diffuseTexture_Sampler, vs_texCoord) + texture(otherTexture_Sampler, vs_texCoord) +
                   texture(diffuseTexture1_Sampler, vs_texCoord) + texture(otherTexture1_Sampler, vs_texCoord);
    fragOut = results;
}