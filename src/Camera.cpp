#include "headers\Camera.h"
#include "glm\gtc\matrix_transform.hpp"

Camera::Camera() :
	worldPos(0, 0, 0),
	forwardDirection(0, 0, -1)
{
	projectionMatrix = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 10.0f);
}

void Camera::setPosition(float x, float y, float z) {

	worldPos = {x, y, z};
}

void Camera::lookAt(float x, float y, float z) {

	glm::vec3 targetPos{x, y, z};
	forwardDirection = targetPos - worldPos;
}

glm::mat4 Camera::calculateWorldToClipMatrix() {

	glm::mat4 worldToCamera = glm::lookAt(worldPos, worldPos + forwardDirection, {0, 1, 0});

	return projectionMatrix * worldToCamera;
}

glm::mat4 Camera::getWorldToCameraMatrix() {

	return glm::lookAt(worldPos, worldPos + forwardDirection, {0, 1, 0});
}