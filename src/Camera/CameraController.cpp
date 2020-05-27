#include "CameraController.h"
#include "Constants.h"

//TODO camera turning left/right in place is probably wrong
//TODO write camera tests
CameraController::CameraController() :
    isMiddleMouseHeld(false),
    isLeftMouseHeld(false),
    previousMousePosition(-1, -1),
    mouseWheelDeltaTranslationRatio(degToRad * 20),
    mouseDeltaCameraTranslationRatio(1/150.0f),
    mouseDeltaCameraRotationRatio(degToRad / 6.0f),
    camera()
    {

    }

CameraController::CameraController(glm::vec3 initialPosition, glm::vec3 initialFocalPoint) :
    isMiddleMouseHeld(false),
    isLeftMouseHeld(false),
    previousMousePosition(-1, -1),
    mouseWheelDeltaTranslationRatio(degToRad * 20),
    mouseDeltaCameraTranslationRatio(1/150.0f),
    mouseDeltaCameraRotationRatio(degToRad / 6.0f),
    camera(initialPosition, initialFocalPoint)
{

}

bool CameraController::handleEvent(const sf::Event &event, const float &deltaTime) {

    return handleMouseEvent(event, deltaTime);
}

void CameraController::handleInput(const float &deltaTime) {
    handleKeyboardInput(deltaTime);
    handleMouseInput(deltaTime);
}

const Camera &CameraController::getCamera() const {
    return camera;
}

glm::vec2 CameraController::getMousePosition() {
	auto mousePos = sf::Mouse::getPosition();
	return {mousePos.x, mousePos.y};
}

bool CameraController::handleMouseEvent(const sf::Event &event, const float& deltaTime) {

    if(event.type == sf::Event::MouseWheelScrolled) {
        camera.moveTowardsTarget(event.mouseWheelScroll.delta * mouseWheelDeltaTranslationRatio);

        return true;
	}

    return false;
}

void CameraController::handleKeyboardInput(const float &deltaTime) {

}

void CameraController::handleMouseInput(const float &deltaTime) {

    //mouse position hasn't been set at all, update for the first time 
    //so that it can be used this frame
    if(previousMousePosition.x < 0)
		previousMousePosition = getMousePosition();

    glm::vec2 currentMousePos = getMousePosition();
	glm::vec2 mouseDelta = currentMousePos - previousMousePosition;

    if(sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        handlePlanerTranslation(deltaTime, mouseDelta);

    } else if(sf::Mouse::isButtonPressed(sf::Mouse::Left)
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            handleRotationAroundTarget(deltaTime, mouseDelta);
    } else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
         handleRotation(deltaTime, mouseDelta);
    }

    previousMousePosition = getMousePosition();
}

void CameraController::handlePlanerTranslation(const float &deltaTime, const glm::vec2 &mouseDelta) {
    
    const glm::vec2 offset = mouseDelta * mouseDeltaCameraTranslationRatio;
    camera.translateIn2DPlane(offset);
}

void CameraController::handleRotationAroundTarget(const float &deltaTime, const glm::vec2 &mouseDelta) {

    const glm::vec2 rotationDelta = mouseDelta * mouseDeltaCameraRotationRatio;
    
    //horizontal mouse movement is rotation around y axis, vertical movement is around x axis
    //rotation around y axis is inverted so dragging right rotates camera to the left, like in unity
    camera.rotateAroundTarget({rotationDelta.y, -rotationDelta.x});
}

void CameraController::handleRotation(const float &deltaTime, const glm::vec2 &mouseDelta) {

    const glm::vec2 rotationDelta = mouseDelta * mouseDeltaCameraRotationRatio;

    //horizontal mouse movement is rotation around y axis, vertical movement is around x axis
    //rotation around y axis is inverted so dragging right rotates camera to the left, like in unity
    camera.rotate({rotationDelta.y, -rotationDelta.x});
}