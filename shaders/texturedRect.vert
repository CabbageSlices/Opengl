#version 450 core

out vec2 texCoord;
vec2 POSITION_DATA[6] = vec2[5] (
    vec2(-1, -1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(1, -1),
    vec2(1, 1),
    vec2(-1, 1)
)

vec2 TEXCOORD_DATA[6] = vec2[5] (
    vec2(-1, -1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(1, -1),
    vec2(1, 1),
    vec2(-1, 1)
)

void main(void) {

	vec3 pos = position;
	worldSpacePosition = modelToWorld * vec4(pos, 1);
	vs_Normal = normal;
	gl_Position = POSITION_DATA[gl_VertexId];
    texCoord = TEXCOORD_DATA[gl_VertexId];
}