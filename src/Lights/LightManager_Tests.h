#ifndef __LIGHTMANAGER_TESTS_H__
#define __LIGHTMANAGER_TESTS_H__

#include "./LightManager.h"
#include "Test/GraphicsTestFixture.h"
#include "glm/gtc/random.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class LightManagerTests : public GraphicsTest {
  public:
    virtual ~LightManagerTests() {}

  protected:
    void createDirectionalLights(int numToCreate) {
        for (unsigned i = 0; i < numToCreate; ++i) {
            glm::vec4 position(glm::sphericalRand(10), 1);
            glm::vec4 direction(glm::sphericalRand(1), 0);
            glm::vec4 intensity(glm::sphericalRand(1), 1);

            lightManager.createDirectionalLight(position, direction, intensity);
        }
    }
    void createPointLights(int numToCreate) {
        for (unsigned i = 0; i < numToCreate; ++i) {
            glm::vec4 position(glm::sphericalRand(10), 1);
            glm::vec4 intensity(glm::sphericalRand(1), 1);
            float range = glm::linearRand(-10, 10);

            lightManager.createPointLight(position, intensity, range);
        }
    }
    LightManager lightManager;
};
#endif  // __LIGHTMANAGER_TESTS_H__