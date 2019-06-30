#pragma once
#include "glm\glm.hpp"

struct DirectionalLight {

    glm::vec4 direction;
    glm::vec4 intensity;    
};

struct PointLight {
    glm::vec4 position;
    glm::vec4 intensity;
    
    float range;
};