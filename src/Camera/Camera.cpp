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

glm::vec3 Camera::determineUpVector(const float &angleAroundHorizontalAxis) {
    // calculate direction of the current up vector of the camera
    // if camera has rotated along the horizontal axis more than 90 degrees or less than -90 degrees,
    // it is upside down and up points in negative y direction.
    // if camera has not rotated beyond those limits then up vector points in positive y direction
    if (angleAroundHorizontalAxis >= PI / 2 && angleAroundHorizontalAxis <= 3 * PI / 2) {
        return glm::vec3(0, -1, 0);
    }

    return glm::vec3(0, 1, 0);
}

Camera::Camera()
    : worldPos(0, 0, 0),
      angleAroundHorizontalAxis(0),
      focalPoint(0, 0, -1),
      defaultFocalPointDistance(5),
      fov(45.0f) {
    projectionMatrix = glm::perspective(fov * degToRad, 4.0f / 3.0f, 0.01f, 30.0f);
}

Camera::Camera(glm::vec3 initialPosition, glm::vec3 initialFocalPoint)
    : worldPos(initialPosition),
      angleAroundHorizontalAxis(0),
      focalPoint(initialFocalPoint),
      defaultFocalPointDistance(5),
      fov(45.0f) {
    projectionMatrix = glm::perspective(fov * degToRad, 4.0f / 3.0f, 0.01f, 30.0f);
}

void Camera::setPosition(float x, float y, float z) {
    glm::vec3 previousForwardDirection = getForward();

    worldPos = {x, y, z};
    focalPoint = worldPos + previousForwardDirection * defaultFocalPointDistance;
}

void Camera::focusOnPoint(float x, float y, float z) {
    // move camera so that the focal point is in the center of the view, but do not rotate camera at all
    // no rotation means keep the same forward direction
    glm::vec3 previousForwardDirection = getForward();

    focalPoint = {x, y, z};
    worldPos = focalPoint - previousForwardDirection * defaultFocalPointDistance;
}

glm::mat4 Camera::calculateWorldToClipMatrix() const {
    glm::mat4 worldToCamera = getWorldToCameraMatrix();

    return projectionMatrix * worldToCamera;
}

glm::mat4 Camera::getWorldToCameraMatrix() const {
    return glm::lookAt(worldPos, focalPoint, determineUpVector(angleAroundHorizontalAxis));
}

glm::vec3 Camera::getPosition() const { return worldPos; }

void Camera::changeFov(const float &fovDeltaRadians) {
    fov += fovDeltaRadians;
    projectionMatrix = glm::perspective(fov, 4.0f / 3.0f, 0.1f, 30.0f);
}

void Camera::moveTowardsTarget(const float &distance) {
    glm::vec3 translationOffset = getForward() * distance;

    worldPos += translationOffset;
}

void Camera::translateIn2DPlane(const glm::vec2 &offset) {
    glm::vec3 up = determineUpVector(angleAroundHorizontalAxis);
    glm::vec3 right = glm::normalize(glm::cross(up, getForward()));

    glm::vec3 rightDirectionOffset = offset.x * right;
    glm::vec3 upDirectionOffset = offset.y * up;

    glm::vec3 translationOffset = (rightDirectionOffset + upDirectionOffset);

    worldPos += translationOffset;
    focalPoint += translationOffset;
}

void Camera::rotateAroundTarget(glm::vec2 rotationDelta) {
    glm::quat rotator = processRotationDelta(rotationDelta);

    // calculate new world position of the camera
    glm::vec3 targetToCamera = worldPos - focalPoint;
    targetToCamera = rotator * targetToCamera;

    worldPos = targetToCamera + focalPoint;
}

void Camera::rotate(glm::vec2 rotationDelta) {
    glm::quat rotator = processRotationDelta(rotationDelta);

    // calculate new focal point position once camera has rotated
    glm::vec3 cameraToTarget = focalPoint - worldPos;
    cameraToTarget = rotator * cameraToTarget;

    focalPoint = worldPos + cameraToTarget;
}

bool Camera::avoidAligningForwardToUp(float &horizontalRotationDelta) {
    // if camera rotation around the horizontal axis is approaching 90 or -90 degrees then the forward
    // vector is about to line up with the global y axis. avoid this by rotating forward or backward according
    // to the rotationdelta to go past 90 degree rotation. calculate the rotation delta required to achieve this new
    // orientation
    if (angleAroundHorizontalAxis < PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta >= PI / 2) {
        horizontalRotationDelta = (PI / 2 + 0.01) - angleAroundHorizontalAxis;
        return true;

    } else if (angleAroundHorizontalAxis > PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta <= PI / 2) {
        horizontalRotationDelta = (PI / 2 - 0.01) - angleAroundHorizontalAxis;
        return true;

    } else if (angleAroundHorizontalAxis > 3 * PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta <= 3 * PI / 2) {
        horizontalRotationDelta = (3 * PI / 2 - 0.01) - angleAroundHorizontalAxis;
        return true;

    } else if (angleAroundHorizontalAxis < 3 * PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta >= 3 * PI / 2) {
        horizontalRotationDelta = (3 * PI / 2 + 0.01) - angleAroundHorizontalAxis;
        return true;
    }

    return false;
}

glm::quat Camera::processRotationDelta(glm::vec2 rotationDelta) {
    // calculate axis of rotation before calculating new up vector
    // because new vector doesn't take effect until rotation has already finished
    // and once the up  vector is changed, the rotation axis will change as well which will result in an incorrect rotation
    glm::vec3 verticalAxis = determineUpVector(angleAroundHorizontalAxis);
    glm::vec3 horizontalAxis = glm::normalize(glm::cross(verticalAxis, getForward()));

    // rotation was modified to avoid aliging the forward vector to the global y axis
    // need to disable rotation around vertical axis for now because the vertical axis is going to switch
    // also need to calculate a new up vector since we went from upside down to right side up or vice-verca
    if (avoidAligningForwardToUp(rotationDelta.x)) {
        rotationDelta.y = 0;
    }

    angleAroundHorizontalAxis += rotationDelta.x;
    angleAroundHorizontalAxis = glm::wrapAngle(angleAroundHorizontalAxis);

    glm::quat xRotator = glm::angleAxis(rotationDelta.x, horizontalAxis);
    glm::quat yRotator = glm::angleAxis(rotationDelta.y, verticalAxis);
    glm::quat rotator = glm::normalize(xRotator * yRotator);

    return rotator;
}

glm::vec3 Camera::getForward() { return glm::normalize(focalPoint - worldPos); }