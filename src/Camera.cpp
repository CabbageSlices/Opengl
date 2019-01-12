#define GLM_ENABLE_EXPERIMENTAL

#include "headers\Camera.h"
#include "glm\gtc\matrix_transform.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\ext\quaternion_trigonometric.hpp"
#include "glm\gtx\vector_angle.hpp"
#include "SFML\Window.hpp"
#include "headers\PrintVector.hpp"
#include "glm/gtx/fast_trigonometry.hpp"
#include "headers/Constants.h"

#include <iostream>

using std::cout;
using std::endl;

glm::vec3 Camera::determineUpVector(const float &angleAroundHorizontalAxis) {
	//calculate direction of the current up vector of the camera
	//if camera has rotated along the horizontal axis more than 90 degrees or less than -90 degrees,
	//it is upside down and up points in negative y direction.
	//if camera has not rotated beyond those limits then up vector points in positive y direction
	if(angleAroundHorizontalAxis >= PI / 2 && angleAroundHorizontalAxis <= 3 * PI / 2) {
		return glm::vec3(0,-1,0);
	} 
	
	return glm::vec3(0,1,0);
}

Camera::Camera() :
	worldPos(0, 0, 0),
	up(0, 1, 0),
	forward(0, 0, -1),
	angleAroundHorizontalAxis(0),
	focalPoint(0, 0, 0),
	defaultFocalPointDistance(5),
	fov(45.0f)
{

	focalPoint = worldPos + forward;
	projectionMatrix = glm::perspective(fov * 3.1415f/180.0f, 4.0f/3.0f, 0.1f, 10.0f);
}

void Camera::setPosition(float x, float y, float z) {

	worldPos = {x, y, z};
	focalPoint = worldPos + forward * defaultFocalPointDistance;
}

void Camera::focusOnPoint(float x, float y, float z) {

	focalPoint = {x, y, z};
	worldPos = focalPoint - forward * defaultFocalPointDistance;
}

glm::mat4 Camera::calculateWorldToClipMatrix() {

	glm::mat4 worldToCamera = getWorldToCameraMatrix();

	return projectionMatrix * worldToCamera;
}

glm::mat4 Camera::getWorldToCameraMatrix() {

	return glm::lookAt(worldPos, focalPoint, up);
}

void Camera::changeFov(const float &fovDeltaRadians) {

	fov += fovDeltaRadians;
	projectionMatrix = glm::perspective(fov, 4.0f/3.0f, 0.1f, 10.0f);
}

void Camera::translateIn2DPlane(const glm::vec2 &offset) {

	glm::vec3 right = glm::normalize(glm::cross(up, forward));

	glm::vec3 rightDirectionOffset = offset.x * right;
	glm::vec3 upDirectionOffset = offset.y * up;

	glm::vec3 translationOffset = (rightDirectionOffset + upDirectionOffset);

	worldPos += translationOffset;
	focalPoint += translationOffset;
}

void Camera::rotateAroundTarget(glm::vec2 rotationDelta) {

		// calculate axis of rotation before calculating new up vector
		//because new vector doesn't take effect until rotation has already finished
		//and once the up  vector is changed, the rotation axis will change as well which will result in an incorrect rotation
		glm::vec3 verticalAxis = up;
		glm::vec3 horizontalAxis = glm::normalize(glm::cross(up, forward));

		//rotation was modified to avoid aliging the forward vector to the global y axis
		//need to disable rotation around vertical axis for now because the vertical axis is going to switch
		//also need to calculate a new up vector since we went from upside down to right side up or vice-verca
		if(avoidAligningForwardToUp(rotationDelta.x)) {
			rotationDelta.y = 0;
			up = determineUpVector(angleAroundHorizontalAxis + rotationDelta.x);
		}

		angleAroundHorizontalAxis += rotationDelta.x;
		angleAroundHorizontalAxis = glm::wrapAngle(angleAroundHorizontalAxis);

		glm::quat xRotator = glm::angleAxis(rotationDelta.x, horizontalAxis);
		glm::quat yRotator = glm::angleAxis(rotationDelta.y, verticalAxis);
		glm::quat rotator = glm::normalize(xRotator * yRotator);

		//calculate new world position of the camera
		glm::vec3 targetToCamera = worldPos - focalPoint;
		targetToCamera = rotator * targetToCamera;

		worldPos = targetToCamera + focalPoint;

		forward = glm::normalize(-targetToCamera);
}

bool Camera::avoidAligningForwardToUp(float &horizontalRotationDelta) {
	//if camera rotation around the horizontal axis is approaching 90 or -90 degrees then the forward
	//vector is about to line up with the global y axis. avoid this by rotating forward or backward according
	//to the rotationdelta to go past 90 degree rotation. calculate the rotation delta required to achieve this new
	//orientation
	if(angleAroundHorizontalAxis < PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta >= PI /2) {
		horizontalRotationDelta = (PI / 2 + 0.01) - angleAroundHorizontalAxis;
		return true;

	} else if (angleAroundHorizontalAxis > PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta <= PI / 2) {
		horizontalRotationDelta = (PI / 2 - 0.01) - angleAroundHorizontalAxis;
		return true;

	} else if(angleAroundHorizontalAxis > 3*PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta <= 3*PI/2) {
		horizontalRotationDelta = (3*PI / 2 - 0.01) - angleAroundHorizontalAxis;
		return true;

	} else if (angleAroundHorizontalAxis < 3*PI / 2 && angleAroundHorizontalAxis + horizontalRotationDelta >= 3*PI / 2) {
		horizontalRotationDelta = (3*PI / 2 + 0.01) - angleAroundHorizontalAxis;
		return true;

	}

	return false;
}


// void Camera::handleMouseInput(const float &deltaTime) {

// 	if(previousMousePosition.x < 0)
// 		previousMousePosition = getMousePosition();

// 	glm::vec2 currentMousePos = getMousePosition();
// 	glm::vec2 mouseDelta = currentMousePos - previousMousePosition;


// 	//mouse dragging screen, move camera left right according to it's forward vector
// 	if(sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {

// 		glm::vec3 right = glm::normalize(glm::cross(up, forward));

// 		glm::vec3 rightDirectionOffset = mouseDelta.x * right;
// 		glm::vec3 upDirectionOffset = mouseDelta.y * up;

// 		// glm::vec3 rightDirectionOffset = mouseDelta.x * getCurrentRight();
// 		// glm::vec3 upDirectionOffset = mouseDelta.y * getCurrentUp();

// 		glm::vec3 offset = (rightDirectionOffset + upDirectionOffset) / 200.f;

// 		worldPos += offset;
// 		focalPoint += offset;

// 	}

// 	//mouse dragaging using left mouse button while alt is held, rotate around focal point
// 	if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {

// 		glm::vec2 rotationDelta = mouseDelta * 3.1415f / 180.f / 6.0f;

// 		float PI = 3.141592;

// 		// //axis of rotation
// 		glm::vec3 verticalAxis = up;
// 		glm::vec3 horizontalAxis = glm::normalize(glm::cross(up, forward));

// 		if(angleAroundHorizontalAxis < PI / 2 && angleAroundHorizontalAxis + rotationDelta.y >= PI/2) {

// 				rotationDelta.y = (PI / 2 + 0.01) - angleAroundHorizontalAxis;
// 				up=glm::vec3(0,-1,0);
// 				rotationDelta.x = 0;

// 				cout << angleAroundHorizontalAxis << "    :    " << glm::asin(forward.y) << endl << "UPSIDE DOWN" << endl;
// 		} else if (angleAroundHorizontalAxis > PI / 2 && angleAroundHorizontalAxis + rotationDelta.y <= PI / 2) {
// 				rotationDelta.y = (PI / 2 - 0.01) - angleAroundHorizontalAxis;
// 				up = glm::vec3(0,1,0);
// 				rotationDelta.x = 0;
// 				cout << angleAroundHorizontalAxis << "    :    " << glm::asin(forward.y) << endl << "NORMAL" << endl;
// 		} else if(angleAroundHorizontalAxis > 3*PI / 2 && angleAroundHorizontalAxis + rotationDelta.y <= 3*PI/2) {

// 				rotationDelta.y = (3*PI / 2 - 0.01) - angleAroundHorizontalAxis;
// 				up=glm::vec3(0,-1,0);
// 				rotationDelta.x = 0;

// 				cout << angleAroundHorizontalAxis << "    :    " << glm::asin(forward.y) << endl << "UPSIDE DOWN" << endl;
// 		} else if (angleAroundHorizontalAxis < 3*PI / 2 && angleAroundHorizontalAxis + rotationDelta.y >= 3*PI / 2) {
// 				rotationDelta.y = (3*PI / 2 + 0.01) - angleAroundHorizontalAxis;
// 				up = glm::vec3(0,1,0);
// 				rotationDelta.x = 0;
// 				cout << angleAroundHorizontalAxis << "    :    " << glm::asin(forward.y) << endl << "NORMAL" << endl;
// 		}

// 		angleAroundHorizontalAxis += rotationDelta.y;
// 		yAngle -= rotationDelta.x;

// 		angleAroundHorizontalAxis = glm::wrapAngle(angleAroundHorizontalAxis);
// 		yAngle = glm::wrapAngle(yAngle);

// 		glm::vec3 targetToCamera = worldPos - focalPoint;

// 		glm::quat xRotator = glm::angleAxis(rotationDelta.y, horizontalAxis);
// 		glm::quat yRotator = glm::angleAxis(-rotationDelta.x, verticalAxis);

// 		glm::quat rotator = glm::normalize(xRotator * yRotator);

// 		//calculate new world position of the camera
// 		targetToCamera = rotator * targetToCamera;

// 		worldPos = targetToCamera + focalPoint;

// 		//calculate new orthonormal basis
// 		forward = glm::normalize(-targetToCamera);
// 		// up = glm::normalize(rotator * up);
// 		// glm::vec3 right = glm::normalize(glm::cross(up, forward));
// 		// up = glm::normalize(glm::cross(forward, right));

// 		// if(forward.y < -0.99) {
// 		// 	cout << forward << endl;
// 		// 	cout << "UP:     "   <<up << endl;
// 		// 	cout << horizontalAxis << endl;
// 		// 	cout << angleAroundHorizontalAxis << "  " << yAngle << endl;
// 		// }

// 		// cout <<	glm::asin(forward.y) << endl;

// 		//if you line up the forward vector with the global z axis then the
// 		//up and right vectors will not be lined up with the global
// 		//up and right axis, this is because the camera tilted along the z axis some how
// 		//calculate the tilt and reverse it
// 	}

// 	//update the mouse position
// 	previousMousePosition = getMousePosition();
// }