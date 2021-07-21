#pragma once

#include <vector>

#include "SFML\Window.hpp"
#include "Transform.h"
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"

class Camera {
  public:
    Camera();
    Camera(glm::vec3 initialPosition);

    void setPosition(const glm::vec3 &position);
    void translate(const glm::vec3 &offset);
    void setOrientation(const glm::quat &orientation);

    glm::mat4 calculateWorldToClipMatrix() const;
    glm::mat4 getWorldToCameraMatrix() const;
    glm::vec3 getPosition() const;

    const Transform &getTransform() { return transform; }
    glm::vec3 getUpVector() const;
    glm::vec3 getForwardVector() const;
    glm::vec3 getRightVector() const;

    // zoom in out
    void changeFov(const float &fovDelta);

  private:
    Transform transform;

    float fov;
    glm::mat4 projectionMatrix;
};
