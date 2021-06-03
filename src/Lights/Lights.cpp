#include "./Lights.h"

#include "glm/gtc/matrix_transform.hpp"

Light Light::CreateDirectionalLight(const glm::vec4 &position, const glm::vec4 &direction, const glm::vec4 &intensity) {
    Light light(position, direction, intensity, 100.0f);

    return light;
}

Light Light::CreatePointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range) {
    Light light(position, glm::vec4(0), intensity, range);

    return light;
}

Light::Light(const glm::vec4 &_position, const glm::vec4 &_direction, const glm::vec4 &_intensity, const float &_range)
    : position(_position),
      direction(_direction),
      intensity(_intensity),
      range(_range) {}
