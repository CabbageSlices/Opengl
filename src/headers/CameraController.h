#pragma once

#include "./Camera.h"
#include "glm\glm.hpp"
#include "SFML\Window.hpp"
/**
 * @brief Class that will manage a Camera object and handle input for it
 * 
 */
class CameraController {

public:

	CameraController();

    bool handleEvent(const sf::Event &event, const float &deltaTime);
	void handleInput(const float &deltaTime);

    Camera camera;
private:

    glm::vec2 getMousePosition();

	bool handleMouseEvent(const sf::Event &event, const float &deltaTime);
	void handleKeyboardInput(const float &deltaTime);
	void handleMouseInput(const float &deltaTime);

	void handlePlanerTranslation(const float &deltaTime, const glm::vec2 &mouseDelta);
	void handleRotationAroundTarget(const float &deltaTime, const glm::vec2 &mouseDelta);
	void handleRotation(const float &deltaTime, const glm::vec2 &mouseDelta);

	bool isMiddleMouseHeld;
	bool isLeftMouseHeld;
	glm::vec2 previousMousePosition;

	const float mouseWheelDeltaTranslationRatio;
	const float mouseDeltaCameraTranslationRatio;
	const float mouseDeltaCameraRotationRatio;
};