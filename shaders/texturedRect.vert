#version 460 core

out vec2 texCoord;
vec2 POSITION_DATA[6] = vec2[6](vec2(-1, -1), vec2(1, -1), vec2(-1, 1), vec2(1, -1), vec2(1, 1), vec2(-1, 1));

vec2 TEXCOORD_DATA[6] = vec2[6](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 0), vec2(1, 1), vec2(0, 1));

void main(void) {
    gl_Position = vec4(POSITION_DATA[gl_VertexID], 0, 1);
    texCoord = TEXCOORD_DATA[gl_VertexID];
}