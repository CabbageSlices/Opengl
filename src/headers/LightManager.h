#pragma once

#include <vector>
#include "./Lights.h"

class LightManager {

public:

    void createDirectionalLight(const glm::vec4 &direction, const glm::vec4 &intensity);
    void createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range);
    
    const std::vector<DirectionalLight> &getDirectionalLights();
    const std::vector<PointLight> &getPointLights();


private:

    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
};