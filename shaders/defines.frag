#ifndef DEFINES
#define DEFINES
#define MAX_DIRECTIONAL_LIGHTS 2
#define MAX_POINT_LIGHTS 4

#define MATERIAL_UNIFORM_BLOCK_BINDING_POINT 1
#define MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT 2
#define DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT 4
#define POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT 5
#define UNIFORM_TRANSFORM_MATRICES_BLOCK_BINDING_POINT 6
#define UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT 7
#define UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT 8
#define UNIFORM_SHADOW_MAP_FLAGS_BINDING_POINT 9
#define LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT 10

#define WORLD_TO_CLIP_UNIFORM_BUFFER_LOCATION 0
#define WORLD_TO_CAMERA_UNIFORM_BUFFER_LOCATION 1
#define EYE_POSITION_UNIFORM_LOCATION 2

#endif