#include "./LightManager_Tests.h"

#include "Test/CustomMatchers.h"

using testing::_;
using testing::Mock;
using ::testing::MockFunction;

TEST_F(LightManagerTests, ableToCreateDirectionalAndPointLights) {
    const glm::vec4 direction(0.5, 0.7, 0.2, 0);
    const glm::vec4 intensity(1, 0.2, 0, 1);
    const glm::vec4 position(1, 2, 300, 1);
    const float range = 12;

    lightManager.createDirectionalLight(position, direction, intensity);

    lightManager.createPointLight(position, intensity, range);

    auto directionalLights = lightManager.getDirectionalLights();
    auto pointLights = lightManager.getPointLights();

    EXPECT_EQ(directionalLights.size(), 1);
    EXPECT_EQ(directionalLights.size(), 1);

    const auto dirLight = directionalLights[0];

    EXPECT_VEC4_EQ(dirLight.getDirection(), direction);
    EXPECT_VEC4_EQ(dirLight.getIntensity(), intensity);
    EXPECT_VEC4_EQ(dirLight.getPosition(), position);

    const auto pointLight = pointLights[0];

    EXPECT_VEC4_EQ(pointLight.getDirection(), direction);
    EXPECT_VEC4_EQ(pointLight.getIntensity(), intensity);
    EXPECT_VEC4_EQ(pointLight.getPosition(), position);
    EXPECT_VEC4_EQ(pointLight.getRange(), range);
}

TEST_F(LightManagerTests, correctlyCalculatesNumberOfBatches) {
    lightManager.setDirectionalLightsPerBatch(5);
    lightManager.setPointLightsPerBatch(1);

    for (unsigned i = 0; i < 5; ++i) {
        lightManager.createDirectionalLight(glm::vec4(1), glm::vec4(1), glm::vec4(1));
        lightManager.createPointLight(glm::vec4(1), glm::vec4(1), 1);
    }

    // number of batches should be 5 because we only allow 1 pointlight per batch
    EXPECT_EQ(lightManager.getBatchCount(), 5);

    lightManager.setPointLightsPerBatch(5);
    lightManager.setDirectionalLightsPerBatch(2);

    EXPECT_EQ(lightManager.getBatchCount(), 3);
}

TEST_F(LightManagerTests, whenNumberOfLightsPerBatchIsChangedtheLightBatchInfoIsResetAsIfNoDataHasBeenProvidedAtAll) {
    createDirectionalLights(10);
    createPointLights(10);

    // first send a batch in so we know that the values actually changed
    lightManager.sendLightBatchToShader(1);

    const Buffer &lightBatchInfoBuffer = lightManager.getLightBatchInfoBuffer();
    const int bufferSize = 12;
    int numDirectionalLights = -1;
    int numPointLights = -1;
    int batchIndex = -1;

    auto readLightBatchInfoBuffer = [bufferSize, &lightBatchInfoBuffer](int &numDirectionalLights, int &numPointLights,
                                                                        int &batchIndex) {
        char data[bufferSize];
        glGetNamedBufferSubData(lightBatchInfoBuffer.getBufferObject(), 0, bufferSize, (GLvoid *)data);
        numDirectionalLights = *((int *)data);
        numPointLights = *((int *)data + 1);
        batchIndex = *((int *)data + 2);
    };
    readLightBatchInfoBuffer(numDirectionalLights, numPointLights, batchIndex);

    EXPECT_EQ(batchIndex, 1);
    EXPECT_EQ(numDirectionalLights, lightManager.getDirectionalLightsPerBatch());
    EXPECT_EQ(numPointLights, lightManager.getPointLightsPerBatch());

    lightManager.setDirectionalLightsPerBatch(4);
    readLightBatchInfoBuffer(numDirectionalLights, numPointLights, batchIndex);

    EXPECT_EQ(numDirectionalLights, 0);
    EXPECT_EQ(numPointLights, 0);
    EXPECT_EQ(batchIndex, 0);

    lightManager.sendLightBatchToShader(1);
    readLightBatchInfoBuffer(numDirectionalLights, numPointLights, batchIndex);

    EXPECT_EQ(numDirectionalLights, lightManager.getDirectionalLightsPerBatch());
    EXPECT_EQ(numPointLights, lightManager.getPointLightsPerBatch());
    EXPECT_EQ(batchIndex, 1);

    lightManager.setPointLightsPerBatch(1);
    readLightBatchInfoBuffer(numDirectionalLights, numPointLights, batchIndex);
    EXPECT_EQ(numDirectionalLights, 0);
    EXPECT_EQ(numPointLights, 0);
    EXPECT_EQ(batchIndex, 0);
}

TEST_F(LightManagerTests, sendingBatchToShaderReturnsFalseWhenBatchDoesntExist) {
    EXPECT_EQ(lightManager.sendLightBatchToShader(0), false);

    createDirectionalLights(4);
    createPointLights(7);
    EXPECT_EQ(lightManager.sendLightBatchToShader(1), true);

    EXPECT_EQ(lightManager.sendLightBatchToShader(10), false);
}

TEST_F(LightManagerTests, sendingBatchToShaderReturnsTrueWhenBatchExists) {
    createDirectionalLights(2);
    createPointLights(2);
    EXPECT_EQ(lightManager.sendLightBatchToShader(0), true);
}

TEST_F(LightManagerTests, sendingMatrixBatchToShaderReturnsFalseWhenBatchDoesntExist) {
    EXPECT_EQ(lightManager.sendLightMatrixBatchToShader(0), false);

    createDirectionalLights(4);
    createPointLights(7);
    EXPECT_EQ(lightManager.sendLightMatrixBatchToShader(1), true);

    EXPECT_EQ(lightManager.sendLightMatrixBatchToShader(10), false);
}

TEST_F(LightManagerTests, sendingMatrixBatchToShaderReturnsTrueWhenBatchExists) {
    createDirectionalLights(2);
    createPointLights(2);
    EXPECT_EQ(lightManager.sendLightMatrixBatchToShader(0), true);
}

// TODO disable for now cuz i can't change the number of max direciotnal lights in the shader file in tests yet
// TEST_F(LightManagerTests, cannotSetDirectionalLightsOrPointLightsPerBatchThatIsHigherThanTheMaxNumberDefinedInTheShader) {
//     try {
//         lightManager.setDirectionalLightsPerBatch(MAX_DIRECTIONAL_LIGHTS + 1);
//         FAIL() << "Should throw an error when setting too many directional lights but didn't" << endl;
//     } catch (...) {
//     }

//     try {
//         lightManager.setDirectionalLightsPerBatch(MAX_POINT_LIGHTS + 1);
//         FAIL() << "Should throw error when setting too many point lights per batch, but didn't" << endl;
//     } catch (...) {
//     }

//     SUCCEED();
// }

TEST_F(LightManagerTests,
       whenNumberOfDIRECTIONALLightsPerBatchIsChangedTheStorageBuffersAndLightMatrixBuffersAreResizedToFitTheNewAmount) {
    MockFunction<void(GLenum, GLsizeiptr, const GLvoid *, GLenum)> mockNamedBufferData;

    int newDirectionalLightsPerBatch = 8;

    const Buffer &directionalLightsBuffer = lightManager.getDirectionalLightBuffer();
    const Buffer &directionalLightsMatrixBuffer = lightManager.getDirectionalLightMatrixBuffer();

    EXPECT_CALL(mockNamedBufferData,
                Call(directionalLightsBuffer.getBufferObject(), newDirectionalLightsPerBatch * sizeof(Light), _, _));

    // 1 matrix per directional light
    EXPECT_CALL(mockNamedBufferData, Call(directionalLightsMatrixBuffer.getBufferObject(),
                                          newDirectionalLightsPerBatch * sizeof(glm::mat4), _, _));

    mockglNamedBufferData = mockNamedBufferData.AsStdFunction();

    lightManager.setDirectionalLightsPerBatch(newDirectionalLightsPerBatch);
}

TEST_F(LightManagerTests,
       whenNumberOfPOINTLightsPerBatchIsChangedTheStorageBufferAndMatrixBuffersAreResizedToFitTheNewAmount) {
    MockFunction<void(GLenum, GLsizeiptr, const GLvoid *, GLenum)> mockNamedBufferData;
    int newPointLightsPerBatch = 4;

    const Buffer &pointLightsBuffer = lightManager.getPointLightBuffer();
    const Buffer &pointLightsMatrixBuffer = lightManager.getPointLightMatrixBuffer();

    EXPECT_CALL(mockNamedBufferData, Call(pointLightsBuffer.getBufferObject(), newPointLightsPerBatch * sizeof(Light), _, _))
        .Times(1);

    // buffer size should be 1 matrix per each face of a point light (6), multiplied by number of point lights
    // int numMatrices = newPointLightsPerBatch * 6;
    EXPECT_CALL(mockNamedBufferData,
                Call(pointLightsMatrixBuffer.getBufferObject(), newPointLightsPerBatch * sizeof(PointLightMatrices), _, _))
        .Times(1);
    mockglNamedBufferData = mockNamedBufferData.AsStdFunction();

    lightManager.setPointLightsPerBatch(newPointLightsPerBatch);
}

TEST_F(LightManagerTests, whenBuffersAreConnectedToShaderAllTheBuffersAreBoundToTheBindingPoints) {
    MockFunction<void(GLenum, GLuint, GLuint)> mockBindBufferBase;

    const Buffer &directionalLightsBuffer = lightManager.getDirectionalLightBuffer();
    const Buffer &pointLightsBuffer = lightManager.getPointLightBuffer();
    const Buffer &lightBatchInfoBuffer = lightManager.getLightBatchInfoBuffer();
    const Buffer &directionalLightsMatrixBuffer = lightManager.getDirectionalLightMatrixBuffer();
    const Buffer &pointLightsMatrixBuffer = lightManager.getPointLightMatrixBuffer();

    EXPECT_CALL(mockBindBufferBase, Call(Buffer::UniformBuffer, DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT,
                                         directionalLightsBuffer.getBufferObject()));
    EXPECT_CALL(mockBindBufferBase,
                Call(Buffer::UniformBuffer, POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT, pointLightsBuffer.getBufferObject()));
    EXPECT_CALL(mockBindBufferBase, Call(Buffer::UniformBuffer, LIGHT_BATCH_INFO_UNIFORM_BLOCK_BINDING_POINT,
                                         lightBatchInfoBuffer.getBufferObject()));

    EXPECT_CALL(mockBindBufferBase, Call(Buffer::UniformBuffer, UNIFORM_DIRECTIONAL_LIGHT_MATRIX_BLOCK_BINDING_POINT,
                                         directionalLightsMatrixBuffer.getBufferObject()));

    EXPECT_CALL(mockBindBufferBase, Call(Buffer::UniformBuffer, UNIFORM_POINT_LIGHT_MATRICES_BLOCK_BINDING_POINT,
                                         pointLightsMatrixBuffer.getBufferObject()));

    mockglBindBufferBase = mockBindBufferBase.AsStdFunction();

    lightManager.connectBuffersToShader();
}

TEST_F(LightManagerTests, calculatesTheCorrectNumberOfLightsinAGivenBatch) {
    int totalLights = 7;
    int batchSize = 3;

    EXPECT_EQ(calculateNumberOfLightsInBatch(totalLights, batchSize, 0), batchSize);
    EXPECT_EQ(calculateNumberOfLightsInBatch(totalLights, batchSize, 1), batchSize);
    EXPECT_EQ(calculateNumberOfLightsInBatch(totalLights, batchSize, 2), 1);

    totalLights = 2;
    batchSize = 3;

    EXPECT_EQ(calculateNumberOfLightsInBatch(totalLights, batchSize, 0), 2);

    batchSize = 2;
    EXPECT_EQ(calculateNumberOfLightsInBatch(totalLights, batchSize, 0), 2);

    EXPECT_EQ(calculateNumberOfLightsInBatch(totalLights, batchSize, 10), 0);
}

TEST_F(LightManagerTests, WhenLightBatchIsSentToShaderLightBatchInfoBufferIsUpdatedWithTheCorrectValuesForTheCurrentBatch) {
    createDirectionalLights(1);
    createPointLights(3);

    lightManager.setDirectionalLightsPerBatch(1);
    lightManager.setPointLightsPerBatch(2);

    // first send a batch in so we know that the values actually changed
    lightManager.sendLightBatchToShader(1);

    const Buffer &lightBatchInfoBuffer = lightManager.getLightBatchInfoBuffer();
    const int bufferSize = 12;
    int numDirectionalLights = -1;
    int numPointLights = -1;
    int batchIndex = -1;

    auto readLightBatchInfoBuffer = [bufferSize, &lightBatchInfoBuffer](int &numDirectionalLights, int &numPointLights,
                                                                        int &batchIndex) {
        char data[bufferSize];
        glGetNamedBufferSubData(lightBatchInfoBuffer.getBufferObject(), 0, bufferSize, (GLvoid *)data);
        numDirectionalLights = *((int *)data);
        numPointLights = *((int *)data + 1);
        batchIndex = *((int *)data + 2);
    };
    readLightBatchInfoBuffer(numDirectionalLights, numPointLights, batchIndex);

    // should be no direcitonal lights, and 1 point light in this batch
    EXPECT_EQ(batchIndex, 1);
    EXPECT_EQ(numDirectionalLights, 0);
    EXPECT_EQ(numPointLights, 1);
}

TEST_F(LightManagerTests, WhenLightBatchIsSentToShaderTheCorrectLightsAreSentToTheBufferForEachBatch) {
    createDirectionalLights(5);
    createPointLights(5);

    const int numDirLightsPerBuffer = 3;
    const int numPointLightsPerBuffer = 2;

    lightManager.setDirectionalLightsPerBatch(numDirLightsPerBuffer);
    lightManager.setPointLightsPerBatch(numPointLightsPerBuffer);

    const Buffer &directionalLightsBuffer = lightManager.getDirectionalLightBuffer();
    const Buffer &pointLightsBuffer = lightManager.getPointLightBuffer();

    vector<Light> directionalLights = lightManager.getDirectionalLights();
    vector<Light> pointLights = lightManager.getPointLights();

    lightManager.sendLightBatchToShader(0);

    vector<Light> directionalLightsSent;
    vector<Light> pointLightsSent;

    directionalLightsBuffer.read(directionalLightsSent);
    pointLightsBuffer.read(pointLightsSent);

    EXPECT_EQ(directionalLightsSent.size(), numDirLightsPerBuffer);
    EXPECT_EQ(pointLightsSent.size(), numPointLightsPerBuffer);

    // first batch is filled for both lights, make sure they are the same
    EXPECT_EQ(memcmp(directionalLightsSent.data(), directionalLights.data(), sizeof(Light) * numDirLightsPerBuffer), 0);
    EXPECT_EQ(memcmp(pointLightsSent.data(), pointLights.data(), sizeof(Light) * numPointLightsPerBuffer), 0);

    // second batch should have 2 correct direcitonal lights and 1 incorrect, and 2 corect point lights
    lightManager.sendLightBatchToShader(1);
    directionalLightsSent.clear();
    pointLightsSent.clear();

    directionalLightsBuffer.read(directionalLightsSent);
    pointLightsBuffer.read(pointLightsSent);

    EXPECT_EQ(directionalLightsSent.size(), numDirLightsPerBuffer);
    EXPECT_EQ(pointLightsSent.size(), numPointLightsPerBuffer);

    size_t matchingPortion = sizeof(Light) * (numDirLightsPerBuffer - 1);

    // to access the lights of batch index 1, we need to index within the light buffers
    EXPECT_EQ(memcmp(directionalLightsSent.data(), directionalLights.data() + numDirLightsPerBuffer, matchingPortion), 0);
    EXPECT_NE(memcmp(directionalLightsSent.data() + matchingPortion,
                     directionalLights.data() + numDirLightsPerBuffer + matchingPortion, sizeof(Light)),
              0);
    EXPECT_EQ(memcmp(pointLightsSent.data(), pointLights.data() + numPointLightsPerBuffer,
                     sizeof(Light) * numPointLightsPerBuffer),
              0);

    // third batch should have 1 corect point light
    lightManager.sendLightBatchToShader(2);
    directionalLightsSent.clear();
    pointLightsSent.clear();

    directionalLightsBuffer.read(directionalLightsSent);
    pointLightsBuffer.read(pointLightsSent);

    EXPECT_EQ(directionalLightsSent.size(), numDirLightsPerBuffer);
    EXPECT_EQ(pointLightsSent.size(), numPointLightsPerBuffer);

    matchingPortion = sizeof(Light);
    EXPECT_EQ(memcmp(pointLightsSent.data(), pointLights.data() + numPointLightsPerBuffer * 2, matchingPortion), 0);
    EXPECT_NE(memcmp(pointLightsSent.data() + matchingPortion,
                     pointLights.data() + matchingPortion + numPointLightsPerBuffer * 2, sizeof(Light)),
              0);
}

TEST_F(LightManagerTests, WhenDirectionalLightIsCreatedALightMatrixIsCreatedForThatDirectionalLight) {
    lightManager.createDirectionalLight({0, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 0, 1});
    lightManager.createDirectionalLight({0, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 0, 1});
    lightManager.createDirectionalLight({0, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 0, 1});

    auto lightMatrices = lightManager.getDirectionalLightMatrices();

    EXPECT_EQ(lightMatrices.size(), 3);
}

TEST_F(LightManagerTests, WhenPointLightIsCreatedAPointLightMatrixIsCreatedForThatLight) {
    lightManager.createPointLight({0, 0, 0, 1}, {0, 1, 0, 0}, 1);
    lightManager.createPointLight({0, 0, 0, 1}, {0, 1, 0, 0}, 1);
    lightManager.createPointLight({0, 0, 0, 1}, {0, 1, 0, 0}, 1);

    auto lightMatrices = lightManager.getPointLightMatrices();

    EXPECT_EQ(lightMatrices.size(), 3);
}

TEST_F(LightManagerTests,
       WhenLightMatrixBatchIsSentToShaderLightBatchInfoBufferIsUpdatedWithTheCorrectValuesForTheCurrentBatch) {
    createDirectionalLights(1);
    createPointLights(3);

    lightManager.setDirectionalLightsPerBatch(1);
    lightManager.setPointLightsPerBatch(2);

    // first send a batch in so we know that the values actually changed
    lightManager.sendLightMatrixBatchToShader(1);

    const Buffer &lightBatchInfoBuffer = lightManager.getLightBatchInfoBuffer();
    const int bufferSize = 12;
    int numDirectionalLights = -1;
    int numPointLights = -1;
    int batchIndex = -1;

    auto readLightBatchInfoBuffer = [bufferSize, &lightBatchInfoBuffer](int &numDirectionalLights, int &numPointLights,
                                                                        int &batchIndex) {
        char data[bufferSize];
        glGetNamedBufferSubData(lightBatchInfoBuffer.getBufferObject(), 0, bufferSize, (GLvoid *)data);
        numDirectionalLights = *((int *)data);
        numPointLights = *((int *)data + 1);
        batchIndex = *((int *)data + 2);
    };
    readLightBatchInfoBuffer(numDirectionalLights, numPointLights, batchIndex);

    // should be no direcitonal lights, and 1 point light in this batch
    EXPECT_EQ(batchIndex, 1);
    EXPECT_EQ(numDirectionalLights, 0);
    EXPECT_EQ(numPointLights, 1);
}

TEST_F(LightManagerTests, WhenDirectionalLightMatrixIsSetForABatchtheCorrectMatricesAreSentForTheGivenBatch) {
    createDirectionalLights(3);

    const int numDirLightsPerBuffer = 2;

    lightManager.setDirectionalLightsPerBatch(numDirLightsPerBuffer);
    lightManager.sendLightMatrixBatchToShader(0);

    const vector<glm::mat4> &directionalLightMatrices = lightManager.getDirectionalLightMatrices();
    const Buffer &directionalLightsMatrixBuffer = lightManager.getDirectionalLightMatrixBuffer();

    vector<glm::mat4> lightMatricesForBatch;
    lightMatricesForBatch.clear();
    directionalLightsMatrixBuffer.read(lightMatricesForBatch);

    size_t amountToCompare = sizeof(glm::mat4) * numDirLightsPerBuffer;
    EXPECT_EQ(memcmp(lightMatricesForBatch.data(), directionalLightMatrices.data(), amountToCompare), 0);

    lightManager.sendLightMatrixBatchToShader(1);
    lightMatricesForBatch.clear();
    directionalLightsMatrixBuffer.read(lightMatricesForBatch);

    // only first item in this batch should match
    amountToCompare = sizeof(glm::mat4);
    size_t offsetToBatch = numDirLightsPerBuffer;
    EXPECT_EQ(memcmp(lightMatricesForBatch.data(), directionalLightMatrices.data() + offsetToBatch, amountToCompare), 0);
}

TEST_F(LightManagerTests, WhenPointLightMatrixIsSetForABatchtheCorrectMatricesAreSentForTheGivenBatch) {
    createPointLights(3);

    const int numPointLightsPerBuffer = 2;

    lightManager.setPointLightsPerBatch(numPointLightsPerBuffer);
    lightManager.sendLightMatrixBatchToShader(0);

    const auto &pointLightMatrices = lightManager.getPointLightMatrices();
    const Buffer &pointLightMatrixBuffer = lightManager.getPointLightMatrixBuffer();

    vector<PointLightMatrices> lightMatricesForBatch;
    lightMatricesForBatch.clear();
    pointLightMatrixBuffer.read(lightMatricesForBatch);

    size_t amountToCompare = sizeof(PointLightMatrices) * numPointLightsPerBuffer;
    EXPECT_EQ(memcmp(lightMatricesForBatch.data(), pointLightMatrices.data(), amountToCompare), 0);

    lightManager.sendLightMatrixBatchToShader(1);
    lightMatricesForBatch.clear();
    pointLightMatrixBuffer.read(lightMatricesForBatch);

    // only first 6 matrices should match
    amountToCompare = sizeof(PointLightMatrices);
    size_t offsetToBatch = numPointLightsPerBuffer;
    EXPECT_EQ(memcmp(lightMatricesForBatch.data(), pointLightMatrices.data() + offsetToBatch, amountToCompare), 0);
}

TEST_F(LightManagerTests,
       LightManagerSendsDefaultLightBatchInfoAboutNoLightsAtAllWhenLightDataIsFirstConnectedUntilALightBatchIsSent) {
    const Buffer &lightBatchInfoBuffer = lightManager.getLightBatchInfoBuffer();
    const int bufferSize = 12;
    char data[bufferSize];

    lightManager.connectBuffersToShader();

    glGetNamedBufferSubData(lightBatchInfoBuffer.getBufferObject(), 0, bufferSize, (GLvoid *)data);

    int numDirectionalLights = *((int *)data);
    int numPointLights = *((int *)data + 1);
    int batchIndex = *((int *)data + 2);

    EXPECT_EQ(numDirectionalLights, 0);
    EXPECT_EQ(numPointLights, 0);
    EXPECT_EQ(batchIndex, 0);
}