#include "CameraController.h"

#include "Constants.h"
#include "PrintVector.hpp"

// TODO camera turning left/right in place is probably wrong
// TODO write camera tests
CameraController::CameraController()
    : isMiddleMouseHeld(false),
      isLeftMouseHeld(false),
      previousMousePosition(-1, -1),
      mouseWheelDeltaTranslationRatio(1),
      mouseDeltaCameraTranslationRatio(1 / 150.0f),
      mouseDeltaCameraRotationRatio(degToRad / 4.0f),
      totalRotationDelta(0, 0),
      distanceToTarget(6),
      camera() {}

CameraController::CameraController(glm::vec3 initialPosition)
    : isMiddleMouseHeld(false),
      isLeftMouseHeld(false),
      previousMousePosition(-1, -1),
      mouseWheelDeltaTranslationRatio(1),
      mouseDeltaCameraTranslationRatio(1 / 150.0f),
      mouseDeltaCameraRotationRatio(degToRad / 4.0f),
      totalRotationDelta(0, 0),
      distanceToTarget(6),
      camera(initialPosition) {}

bool CameraController::handleEvent(const sf::Event &event, const float &deltaTime) {
    return handleMouseEvent(event, deltaTime);
}

void CameraController::handleInput(const float &deltaTime) {
    handleKeyboardInput(deltaTime);
    handleMouseInput(deltaTime);
}

const Camera &CameraController::getCamera() const { return camera; }

glm::vec2 CameraController::getMousePosition() {
    auto mousePos = sf::Mouse::getPosition();
    return {mousePos.x, mousePos.y};
}

bool CameraController::handleMouseEvent(const sf::Event &event, const float &deltaTime) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        glm::vec3 forward = camera.getForwardVector();

        glm::vec3 targetPosition = camera.getPosition() + forward * distanceToTarget;

        float distanceDelta = event.mouseWheelScroll.delta * mouseWheelDeltaTranslationRatio;
        distanceToTarget -= distanceDelta;

        distanceToTarget = glm::clamp(distanceToTarget, 0.5f, 15.0f);

        camera.setPosition(targetPosition - forward * distanceToTarget);

        return true;
    }

    return false;
}

void CameraController::handleKeyboardInput(const float &deltaTime) {}

void CameraController::handleMouseInput(const float &deltaTime) {
    // init mouse position
    // if mouse position is negative, then we haven't set the previous mouse psoition yet, so set it for the first time
    if (previousMousePosition.x < 0) previousMousePosition = getMousePosition();

    glm::vec2 currentMousePos = getMousePosition();
    glm::vec2 mouseDelta = currentMousePos - previousMousePosition;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        handlePlanerTranslation(deltaTime, mouseDelta);

    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
        handleRotationAroundTarget(deltaTime, mouseDelta);
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        handleRotation(deltaTime, mouseDelta);
    }

    previousMousePosition = getMousePosition();
}

void CameraController::handlePlanerTranslation(const float &deltaTime, const glm::vec2 &mouseDelta) {
    const glm::vec2 offset = mouseDelta * mouseDeltaCameraTranslationRatio;

    const glm::vec3 rightVector = camera.getRightVector();
    const glm::vec3 upVector = camera.getUpVector();
    camera.translate(rightVector * offset.x + upVector * offset.y);
}

void CameraController::handleRotationAroundTarget(const float &deltaTime, const glm::vec2 &mouseDelta) {
    glm::vec3 position = camera.getPosition();
    glm::vec3 targetPosition = camera.getForwardVector() * distanceToTarget + position;

    const glm::vec2 rotationDelta = mouseDelta * mouseDeltaCameraRotationRatio;
    totalRotationDelta += rotationDelta;

    camera.setOrientation(glm::quat(glm::vec3(totalRotationDelta.y, -totalRotationDelta.x, 0)));

    glm::vec3 newPosition = targetPosition - camera.getForwardVector() * distanceToTarget;
    camera.setPosition(newPosition);
}

void CameraController::handleRotation(const float &deltaTime, const glm::vec2 &mouseDelta) {
    const glm::vec2 rotationDelta = mouseDelta * mouseDeltaCameraRotationRatio;

    totalRotationDelta += rotationDelta;

    camera.setOrientation(glm::quat(glm::vec3(totalRotationDelta.y, -totalRotationDelta.x, 0)));
}
