#pragma once

#include <vector>
#include "glm\glm.hpp"
#include "SFML\Window.hpp"
#include "glm\gtc\quaternion.hpp"

class Camera {

public:

	//angle is in radians
	//angle is between 0 and 2pi
	static glm::vec3 determineUpVector(const float &angleAroundHorizontalAxis);

	Camera();

	void setPosition(float x, float y, float z);
	void focusOnPoint(float x, float y, float z);

	glm::mat4 calculateWorldToClipMatrix();
	glm::mat4 getWorldToCameraMatrix();

	//zoom in out
	void changeFov(const float &fovDelta);

	//translate in a plane perpindcular to the screen
	//that is, x axis moves left and right on the screen as viewed by hte user,
	//y axis is up and down as viewed by the user
	//offset is the offset in the 2d plane to move
	void translateIn2DPlane(const glm::vec2 &offset);

	//rotate around the currently focused target
	//must be in radians
	//delta x is rotation around the horizontal axis, y is rotation around vertical axis
	void rotateAroundTarget(glm::vec2 rotationDelta);

	//rotate camera around it's local origin
	//must be in radians
	//delta x is rotation around the horizontal axis, y is rotation around vertical axis
	void rotate(glm::vec2 rotationDelta);

private:

	//if the forward vector aligns with global y axis then the camera wont be able to rotate around x axis anymore
	//since they are perpindicular the cross product is 0 so you can't calculate the right vector.
	//checks if the current orientation + the given delta will be at or cross 90 or -90 degres
	//if so it will calculate a new rotation delta so that when the new delta is added to the current rotation it will not be 
	//90 or -90, and will go in the direction that yo uare trying to rotate.
	//returns true if the delta was changed, false other wise
	//if this returns true you need to calculate a new up vector for hte camera
	bool avoidAligningForwardToUp(float &horizontalRotationDelta);

	glm::vec3 getForward();

	glm::vec3 worldPos;

	float angleAroundHorizontalAxis;

	//point to revolve around when using revolve around target feature
	glm::vec3 focalPoint;
	const float defaultFocalPointDistance;

	float fov;
	glm::mat4 projectionMatrix;
};
