#pragma once
#include <functional>

#include "glm\glm.hpp"

struct LightDataForShader {};
class Light {
  public:
    Light() = default;
    static Light CreateDirectionalLight(const glm::vec4 &position, const glm::vec4 &direction, const glm::vec4 &intensity);
    static Light CreatePointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range);

    const glm::vec4 &getPosition() const { return position; };
    const glm::vec4 &getDirection() const { return direction; };
    const glm::vec4 &getIntensity() const { return intensity; };
    const float &getRange() const { return range; };

  private:
    Light(const glm::vec4 &position, const glm::vec4 &direction, const glm::vec4 &intensity, const float &range);

    glm::vec4 position;
    glm::vec4 direction;
    glm::vec4 intensity;
    float range;  // for point light this will be the range where light actually affects things. For directional lights, this
                  // will be the max depth of the shadow map
    glm::vec3 paddingForGLSLBuffer = glm::vec3(0, 0, 0);  // padding so that range can be a vec4 and be nicely spaced, for
                                                          // now
};
