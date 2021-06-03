#include "./LightManager.h"

#include "Includes.h"
#include "ShaderProgram/shaderprogram.h"

using std::vector;

LightManager::LightManager(int _numDirectionalLightsPerBatch, int _numPointLightsPerBatch)
    : numDirectionalLightsPerBatch(_numDirectionalLightsPerBatch),
      numPointLightsPerBatch(_numPointLightsPerBatch) {
    createBuffers();
}

void LightManager::createDirectionalLight(const glm::vec4 &position, const glm::vec4 &direction,
                                          const glm::vec4 &intensity) {
    Light light = Light::CreateDirectionalLight(position, direction, intensity);
    directionalLights.push_back(light);

    createDirectionalLightMatrix(light);
}

void LightManager::createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range) {
    Light light = Light::CreatePointLight(position, intensity, range);
    pointLights.push_back(light);

    createPointLightMatrices(light);
}

void LightManager::connectBuffersToShader() {
    directionalLightBuffer.bindToTargetBindingPoint(DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT);
    pointLightBuffer.bindToTargetBindingPoint(POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT);

    directionalLightMatrixBuffer.bindToTargetBindingPoint(UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT);
    pointLightMatrixBuffer.bindToTargetBindingPoint(UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT);

    lightBatchInfoBuffer.bindToTargetBindingPoint(LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT);
}

bool LightManager::sendLightMatrixBatchToShader(unsigned batchIndex) {
    if (batchIndex >= getBatchCount()) {
        return false;
    }

    setDirectionalLightMatricesForBatch(batchIndex);

    LightBatchInfo batchInfo = generateInfoForBatch(batchIndex);

    lightBatchInfoBuffer.updateData(&batchInfo, sizeof(LightBatchInfo), 0);

    return true;
}

void LightManager::setDirectionalLightMatricesForBatch(unsigned batchIndex) {
    int numMatricesToSend =
        calculateNumberOfLightsInBatch(directionalLights.size(), numDirectionalLightsPerBatch, batchIndex);

    int indexFirstMatrix = batchIndex * numDirectionalLightsPerBatch;

    size_t sizeOfDataToSend = numMatricesToSend * sizeof(glm::mat4);

    if (numMatricesToSend > 0) {
        directionalLightMatrixBuffer.updateData(directionalLightMatrices.data() + indexFirstMatrix, sizeOfDataToSend, 0);
    }
}

void LightManager::setPointLightMatricesForBatch(unsigned batchIndex) {}

int LightManager::getBatchCount() {
    int direcitonalLightBatches = ceil((float)directionalLights.size() / numDirectionalLightsPerBatch);
    int pointLightBatches = ceil((float)pointLights.size() / numPointLightsPerBatch);

    return glm::max(direcitonalLightBatches, pointLightBatches);
}

bool LightManager::sendLightBatchToShader(int batchIndex) {
    // not enough batches, indicate failure
    if (batchIndex >= getBatchCount()) {
        return false;
    }

    sendLightBatchToShader(batchIndex, numDirectionalLightsPerBatch, directionalLights, directionalLightBuffer);
    sendLightBatchToShader(batchIndex, numPointLightsPerBatch, pointLights, pointLightBuffer);

    LightBatchInfo batchInfo = generateInfoForBatch(batchIndex);

    lightBatchInfoBuffer.updateData(&batchInfo, sizeof(LightBatchInfo), 0);

    return true;
}

void LightManager::setDirectionalLightsPerBatch(int numLights) {
    // disabling for now until i figure out a way to have a config file for all these
    // defines, that way when testing i can have this be a larger number
    // if (numLights > MAX_DIRECTIONAL_LIGHTS) {
    //     throw "Can't send more direcitonal lights than limit defined in shaders";
    // }

    numDirectionalLightsPerBatch = numLights;
    createDirectionalLightBuffers();
    clearLightBatchInfoBuffer();
}

void LightManager::setPointLightsPerBatch(int numLights) {
    // if (numLights > MAX_DIRECTIONAL_LIGHTS) {
    //     throw "Can't send more point lights than limit defined in shaders";
    // }
    numPointLightsPerBatch = numLights;
    createPointLightBuffers();
    clearLightBatchInfoBuffer();
}

void LightManager::sendLightBatchToShader(int batchIndex, int maxLightsPerBatch, const std::vector<Light> &lights,
                                          Buffer &lightBufferToUpdate) {
    int numLightsToSend = calculateNumberOfLightsInBatch(lights.size(), maxLightsPerBatch, batchIndex);

    int indexFirstLight = batchIndex * maxLightsPerBatch;

    size_t sizeOfDataToSend = numLightsToSend * sizeof(Light);

    if (numLightsToSend > 0) {
        lightBufferToUpdate.updateData(lights.data() + indexFirstLight, sizeOfDataToSend, 0);
    }
}

void LightManager::createDirectionalLightMatrix(const Light &light) {
    glm::vec4 direction = light.getDirection();
    glm::vec4 position = light.getPosition();

    // ensure that light vector isn't parallel to global up
    glm::vec3 kindOfNormalToLightDirection(direction.y, direction.x, direction.z);
    glm::vec3 up = glm::normalize(glm::cross(glm::vec3(direction), kindOfNormalToLightDirection));
    glm::mat4 worldToLight = glm::lookAt(glm::vec3(position), glm::vec3(position + direction * 100.0f), up);

    glm::mat4 lightToClip = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    directionalLightMatrices.push_back(lightToClip * worldToLight);
}

void LightManager::createPointLightMatrices(const Light &light) {}

const vector<Light> &LightManager::getDirectionalLights() { return directionalLights; }

const vector<Light> &LightManager::getPointLights() { return pointLights; }

LightBatchInfo LightManager::generateInfoForBatch(unsigned batchIndex) {
    return LightBatchInfo{
        .numDirectionalLightsInBatch =
            calculateNumberOfLightsInBatch(directionalLights.size(), numDirectionalLightsPerBatch, batchIndex),
        .numPointLightsInBatch = calculateNumberOfLightsInBatch(pointLights.size(), numPointLightsPerBatch, batchIndex),
        .batchIndex = (int)batchIndex,
    };
}

void LightManager::clearLightBatchInfoBuffer() {
    LightBatchInfo batchInfo{0, 0, 0};
    lightBatchInfoBuffer.updateData(&batchInfo, sizeof(LightBatchInfo), 0);
}

void LightManager::createBuffers() {
    createDirectionalLightBuffers();
    createPointLightBuffers();

    lightBatchInfoBuffer.create(Buffer::BufferType::UniformBuffer, nullptr, sizeof(LightBatchInfo),
                                Buffer::UsageType::DynamicDraw);

    clearLightBatchInfoBuffer();
}

void LightManager::createDirectionalLightBuffers() {
    directionalLightBuffer.create(Buffer::BufferType::UniformBuffer, nullptr, sizeof(Light) * numDirectionalLightsPerBatch,
                                  Buffer::UsageType::DynamicDraw);

    directionalLightMatrixBuffer.create(Buffer::BufferType::UniformBuffer, nullptr,
                                        sizeof(glm::mat4) * numDirectionalLightsPerBatch, Buffer::UsageType::DynamicDraw);
}

void LightManager::createPointLightBuffers() {
    pointLightBuffer.create(Buffer::BufferType::UniformBuffer, nullptr, sizeof(Light) * numPointLightsPerBatch,
                            Buffer::UsageType::DynamicDraw);
    pointLightMatrixBuffer.create(Buffer::BufferType::UniformBuffer, nullptr,
                                  sizeof(PointLightMatrices) * numPointLightsPerBatch, Buffer::DynamicDraw);
}

int calculateNumberOfLightsInBatch(int totalLightCount, int maxLightsPerBatch, int batchNumber) {
    int indexFirstLight = batchNumber * maxLightsPerBatch;

    if (indexFirstLight >= totalLightCount) {
        return 0;
    }

    int indexLastLight = indexFirstLight + maxLightsPerBatch - 1;

    return indexLastLight >= totalLightCount ? totalLightCount - indexFirstLight : maxLightsPerBatch;
}