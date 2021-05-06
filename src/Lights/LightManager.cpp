#include "./LightManager.h"

#include "Includes.h"
#include "ShaderProgram/shaderprogram.h"

using std::vector;

const DirectionalLight LightManager::DUMMY_DIRECTIONAL_LIGHT = DirectionalLight{{0, 20, 0, 1}, {1, 0, 0, 0}, {0, 0, 0, 0}};

const PointLight LightManager::DUMMY_POINT_LIGHT = PointLight{{0, 0, 0, 0}, {0, 0, 0, 0}, 0, {0, 0, 0}};

LightManager::LightManager() {
    pointLightBuffer.create(Buffer::BufferType::UniformBuffer, nullptr, sizeof(PointLight) * POINT_LIGHTS_PER_BATCH,
                            Buffer::UsageType::StreamDraw);
    directionalLightBuffer.create(Buffer::BufferType::UniformBuffer, nullptr,
                                  sizeof(DirectionalLight) * DIRECTIONAL_LIGHTS_PER_BATCH, Buffer::UsageType::StreamDraw);
    initializeLightMatrixBuffer();
}

void LightManager::createDirectionalLight(const glm::vec4 &position, const glm::vec4 &direction,
                                          const glm::vec4 &intensity) {
    DirectionalLight light({position, direction, intensity});
    directionalLights.push_back(light);
    createLightMatrix(light);
}

void LightManager::createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range) {
    pointLights.push_back({position, intensity, range});
}

void LightManager::connectLightDataToShader() {
    directionalLightBuffer.bindToTargetBindingPoint(DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT);
    pointLightBuffer.bindToTargetBindingPoint(POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT);
}

void LightManager::setLightMatrixForBatch(unsigned batchId) {
    int indexFirstLightInBatchRaw = batchId * DIRECTIONAL_LIGHTS_PER_BATCH;
    unsigned directionalLightBatchStart = indexFirstLightInBatchRaw;

    if (directionalLightBatchStart >= directionalLightClipMatrices.size()) {
        return;
    }

    unsigned numDirectionalLightsToSend =
        directionalLightBatchStart + DIRECTIONAL_LIGHTS_PER_BATCH <= directionalLightClipMatrices.size()
            ? DIRECTIONAL_LIGHTS_PER_BATCH
            : directionalLightClipMatrices.size() - directionalLightBatchStart;

    lightMatrixBuffer.updateData(directionalLightClipMatrices.data() + directionalLightBatchStart,
                                 numDirectionalLightsToSend * sizeof(glm::mat4), 0);
}

int LightManager::getBatchCount() {
    int direcitonalLightBatches = ceil((float)directionalLights.size() / DIRECTIONAL_LIGHTS_PER_BATCH);
    int pointLightBatches = ceil((float)pointLights.size() / POINT_LIGHTS_PER_BATCH);

    return glm::max(direcitonalLightBatches, pointLightBatches);
}

void LightManager::sendBatchToShader(int batchId) {
    // not enough batches, indicate failure
    if (batchId >= getBatchCount()) {
        return;
    }

    sendLightBatchToShader(batchId, directionalLights, directionalLightBuffer, DIRECTIONAL_LIGHTS_PER_BATCH,
                           DUMMY_DIRECTIONAL_LIGHT);
    sendLightBatchToShader(batchId, pointLights, pointLightBuffer, POINT_LIGHTS_PER_BATCH, DUMMY_POINT_LIGHT);

    return;
}

// TODO refactor so light batch is computed elsewhere
template <typename T>
void LightManager::sendLightBatchToShader(int batchId, const std::vector<T> &lightsCollection, Buffer &lightBuffer,
                                          const int maxLightInBatch, const T &dummyLight) {
    std::vector<T> lightsToSend = getLightBatch(batchId, lightsCollection, maxLightInBatch, dummyLight);

    // put into light buffer which is connected to the shader uniform block
    lightBuffer.updateData(lightsToSend.data(), sizeof(T) * lightsToSend.size(), 0);
}

template <typename T>
std::vector<T> LightManager::getLightBatch(int batchId, const std::vector<T> &lightsCollection, const int maxLightInBatch,
                                           const T &dummyLight) {
    int indexFirstLightInBatchRaw = batchId * maxLightInBatch;

    vector<T> lightsInBatch;

    // make sure begin and end iterator is valid to avoid segfault
    auto rangeStart = lightsCollection.size() <= indexFirstLightInBatchRaw
                          ? lightsCollection.end()
                          : lightsCollection.begin() + indexFirstLightInBatchRaw;

    auto rangeEnd = (lightsCollection.size() <= indexFirstLightInBatchRaw + maxLightInBatch)
                        ? lightsCollection.end()
                        : lightsCollection.begin() + indexFirstLightInBatchRaw + maxLightInBatch;

    // get all the lights that actually exist, and add them to the batch
    lightsInBatch.insert(lightsInBatch.end(), rangeStart, rangeEnd);

    // determine if you need to use dummy lights for batch
    // if you don't have enough lights to fill current batch, you will need dummy lights
    int numDummyLights = glm::clamp<int>(maxLightInBatch - lightsInBatch.size(), 0, maxLightInBatch);

    // add dummy directional lights
    lightsInBatch.insert(lightsInBatch.end(), numDummyLights, dummyLight);

    return lightsInBatch;
}

const vector<DirectionalLight> &LightManager::getDirectionalLights() { return directionalLights; }

const vector<PointLight> &LightManager::getPointLights() { return pointLights; }

void LightManager::createLightMatrix(const DirectionalLight &light) {
    // ensure that light vector isn't parallel to global up
    glm::vec3 kindOfNormalToLightDirection(light.direction.y, light.direction.x, light.direction.z);
    glm::vec3 up = glm::normalize(glm::cross(glm::vec3(light.direction), kindOfNormalToLightDirection));
    glm::mat4 worldToLight =
        glm::lookAt(glm::vec3(light.position), glm::vec3(light.position + light.direction * 100.0f), up);

    glm::mat4 lightToClip = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    directionalLightClipMatrices.push_back(lightToClip * worldToLight);
}

void LightManager::initializeLightMatrixBuffer() {
    if (lightMatrixBuffer.isUsed()) {
        return;
    }

    constexpr unsigned direcitonalLightMatricesSize = sizeof(glm::mat4) * DIRECTIONAL_LIGHTS_PER_BATCH;
    char data[direcitonalLightMatricesSize] = {0};
    lightMatrixBuffer.create(Buffer::BufferType::UniformBuffer, data, direcitonalLightMatricesSize,
                             Buffer::UsageType::StaticDraw);

    lightMatrixBuffer.bindToTargetBindingPoint(UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT);
}