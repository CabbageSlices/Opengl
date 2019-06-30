#include "headers/LightManager.h"

using std::vector;

void LightManager::createDirectionalLight(const glm::vec4 &direction, const glm::vec4 &intensity) {

    directionalLights.push_back({direction, intensity});
}

void LightManager::createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range) {

    pointLights.push_back({position, intensity, range});
}

const vector<DirectionalLight> &LightManager::getDirectionalLights() {

    return directionalLights;
}

const vector<PointLight> &LightManager::getPointLights() {

    return pointLights;
}