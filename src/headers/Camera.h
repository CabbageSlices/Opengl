#pragma once

#include <vector>
#include "glm\glm.hpp"

class Camera {

public:

	Camera();

	void setPosition(float x, float y, float z);
	void lookAt(float x, float y, float z);

	glm::mat4 calculateWorldToClipMatrix();
	glm::mat4 getWorldToCameraMatrix();

private:

	glm::vec3 worldPos;
	glm::vec3 forwardDirection;

	glm::mat4 projectionMatrix;
};
