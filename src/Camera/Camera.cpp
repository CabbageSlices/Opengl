#define GLM_ENABLE_EXPERIMENTAL

#include "Camera.h"

#include <iostream>

#include "Constants.h"
#include "PrintVector.hpp"
#include "SFML\Window.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/fast_trigonometry.hpp"
#include "glm\ext\quaternion_trigonometric.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "glm\gtx\vector_angle.hpp"

using std::cout;
using std::endl;

Camera::Camera() : transform(), fov(50.0f) {
    projectionMatrix = glm::perspective(fov * degToRad, 4.0f / 3.0f, 0.01f, 30.0f);
}

Camera::Camera(glm::vec3 initialPosition) : transform(), fov(50.0f) {
    projectionMatrix = glm::perspective(fov * degToRad, 4.0f / 3.0f, 0.01f, 30.0f);
}

void Camera::setPosition(const glm::vec3 &position) { transform.position = position; }

void Camera::translate(const glm::vec3 &offset) { transform.position += offset; }

void Camera::setOrientation(const glm::quat &orientation) { transform.orientation = glm::normalize(orientation); }

glm::mat4 Camera::calculateWorldToClipMatrix() const { return projectionMatrix * getWorldToCameraMatrix(); }

glm::mat4 Camera::getWorldToCameraMatrix() const {
    // distance to focal point doest matter as long as vector from eye positio nto focal point is in the same direction,
    // so we don't need to store actual location of focal point in camera
    return glm::lookAt(transform.position, transform.position + getForwardVector(), getUpVector());
}

glm::vec3 Camera::getPosition() const { return transform.position; }

glm::vec3 Camera::getUpVector() const {
    glm::vec3 defaultUpVector = glm::vec3(0, 1, 0);

    glm::vec3 rotatedUp = glm::normalize(transform.orientation * defaultUpVector);
    return rotatedUp;
}

glm::vec3 Camera::getForwardVector() const {
    glm::vec3 defaultForwardVector = glm::vec3(0, 0, 1);
    return glm::normalize(transform.orientation * defaultForwardVector);
}

glm::vec3 Camera::getRightVector() const {
    glm::vec3 defaultRightVector = glm::vec3(1, 0, 0);
    return glm::normalize(transform.orientation * defaultRightVector);
}