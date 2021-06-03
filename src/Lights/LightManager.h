#pragma once

#include <vector>

#include "./Buffer.h"
#include "./Lights.h"
#include "defines.frag"

// light matrices for a single point light, one matrix for each face
struct PointLightMatrices {
    glm::mat4 matrices[6];

    enum FaceIndex : unsigned {
        RIGHT = 0,
        LEFT = 1,
        UP = 2,
        DOWN = 3,
        FRONT = 4,
        BACK = 5,
    };
};

struct LightBatchInfo {
    int numDirectionalLightsInBatch;
    int numPointLightsInBatch;
    int batchIndex;
};

// TODO write tests
class LightManager {
  public:
    static constexpr unsigned DIRECTIONAL_LIGHTS_PER_BATCH = MAX_DIRECTIONAL_LIGHTS;
    static constexpr unsigned POINT_LIGHTS_PER_BATCH = MAX_POINT_LIGHTS;

    LightManager(int _numDirectionalLightsPerBatch = DIRECTIONAL_LIGHTS_PER_BATCH,
                 int _numPointLightsPerBatch = POINT_LIGHTS_PER_BATCH);
    LightManager(LightManager &other) = delete;
    LightManager &operator=(LightManager &rhs) = delete;

    void createDirectionalLight(const glm::vec4 &position, const glm::vec4 &direction, const glm::vec4 &intensity);
    void createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range);

    // connect the light buffers to the uniform buffer object in the shader so that data from the buffers
    // are received by the shaders
    void connectBuffersToShader();

    // creates a world to light clip matrix for each light in the given batch, and sends it to the shaders
    // UPDATES LIGHT BATCH SO IF YOU USE sendLightBatchToShader FOR BATCH INFO IT WILL BE OVERWRITTEN
    // this is because i don't see any scenario where you want to send lights for one batch but matrices for a different
    // batch
    bool sendLightMatrixBatchToShader(unsigned batchIndex);

    // reorganize the ligts so that the nearest lights are at the front of the array
    // this way when a batch of lights (the first X elements in the array) are sent to the shader, these lights are the
    // closest
    // TODO this, also reset light clip buffer
    void organizeImportantLights(glm::vec3 objectPos);

    // get the number of batches that exist
    // that is total number of lights / number of lights per batch
    int getBatchCount();

    // batch 0 = first X lights that are closest to the current object
    // batch 1 = next X lights, and so on
    // returns true if the batch was selected (there are enough lights to reach that many batches),
    // or returns false if the batch doesn't exist
    bool sendLightBatchToShader(int batchIndex);

    void setDirectionalLightsPerBatch(int numLights);
    void setPointLightsPerBatch(int numLights);

    const std::vector<Light> &getDirectionalLights();
    const std::vector<Light> &getPointLights();

    const Buffer &getDirectionalLightBuffer() { return directionalLightBuffer; }
    const Buffer &getPointLightBuffer() { return pointLightBuffer; }

    const Buffer &getDirectionalLightMatrixBuffer() { return directionalLightMatrixBuffer; }
    const Buffer &getPointLightMatrixBuffer() { return pointLightMatrixBuffer; }

    const Buffer &getLightBatchInfoBuffer() { return lightBatchInfoBuffer; }

    int getDirectionalLightsPerBatch() { return numDirectionalLightsPerBatch; }
    int getPointLightsPerBatch() { return numPointLightsPerBatch; }

    const std::vector<glm::mat4> &getDirectionalLightMatrices() { return directionalLightMatrices; }
    const std::vector<PointLightMatrices> &getPointLightMatrices() { return pointLightMatrices; }

    LightBatchInfo generateInfoForBatch(unsigned batchIndex);

  private:
    void clearLightBatchInfoBuffer();
    void createBuffers();
    void createDirectionalLightBuffers();
    void createPointLightBuffers();

    void sendLightBatchToShader(int batchIndex, int maxLightsPerBatch, const std::vector<Light> &lights,
                                Buffer &lightBufferToUpdate);

    void createDirectionalLightMatrix(const Light &light);
    void createPointLightMatrices(const Light &light);

    void setDirectionalLightMatricesForBatch(unsigned batchIndex);
    void setPointLightMatricesForBatch(unsigned batchIndex);

    std::vector<Light> directionalLights;
    std::vector<Light> pointLights;

    // buffers that contain the currently acctive set of lights that are sent to the shader. THESE DO NOT CONTAIN ALL HTE
    // LIGHTS, only the lights relevant to the current batch being processed
    Buffer directionalLightBuffer;
    Buffer pointLightBuffer;
    Buffer directionalLightMatrixBuffer;
    Buffer pointLightMatrixBuffer;

    Buffer lightBatchInfoBuffer;

    std::vector<glm::mat4> directionalLightMatrices;
    std::vector<PointLightMatrices> pointLightMatrices;

    int numDirectionalLightsPerBatch;
    int numPointLightsPerBatch;
};

// for the given batch number compute the total number of lights that can be sent, if there is a max maxLightsPerBatch
// lights in each batch
int calculateNumberOfLightsInBatch(int totalLightCount, int maxLightsPerBatch, int batchNumber);