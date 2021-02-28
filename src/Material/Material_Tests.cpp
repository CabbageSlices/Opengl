#include "./Material_Tests.h"

#include "GLTextureObject.h"
#include "Includes.h"
#include "ShaderProgram/shaderprogram.h"
#include "TextureResource/ImageTextureResource.h"
#include "error_strings.h"
#include "glm/gtx/string_cast.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Mock;
using ::testing::MockFunction;

// test that you can add attribute data to material and fetch it again
TEST_F(MaterialTests, setAndGetAttributeValues) {
    glm::vec4 diffuse(1, 1, 0.5, 1);
    float specularCoefficient = 256;

    mat.setAttribute("diffuseColor", diffuse);
    mat.setAttribute("specularCoefficient", specularCoefficient);

    glm::vec4 returnedDiff = mat.getAttribute<glm::vec4>("diffuseColor");
    float returnedSepc = mat.getAttribute<float>("specularCoefficient");

    EXPECT_EQ(diffuse, returnedDiff);
    EXPECT_EQ(specularCoefficient, returnedSepc);

    try {
        auto val = mat.getAttribute<glm::vec4>("specularCoefficient");
        FAIL() << "get attribute returned a value even though it has the wrong type. Attribute name: specularCoefficient; "
                  "Val : "
               << glm::to_string(val) << endl;
    } catch (string e) {
        EXPECT_EQ(e, INCORRECT_TYPE_ERROR);
    }

    try {
        auto val = mat.getAttribute<glm::vec4>("not found");
        FAIL() << "get attribtue returned a value even thoug hti shouldn't exist. Attribtue name: \'not found\'; Val: "
               << glm::to_string(val) << endl;
    } catch (string e) {
        EXPECT_EQ(e, NOT_FOUND_ERROR);
    }
}

// activating material also activates the correct shader
TEST_F(MaterialTests, materialActivatesCorrectShaderProgram) {
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    ASSERT_EQ(currentProgram, 0);
    mat.activate();

    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    ASSERT_EQ(currentProgram, shader->getShaderProgram());
}

// activating material will bind the mateiral buffer to the uniform buffer's binding index
TEST_F(MaterialTests, materialIsBoundToUniformBlocksBindingIndex) {
    GLint currentProgram;
    int bindingIndexInShader = 21;

    MockFunction<void(GLenum, GLuint, GLuint)> mockBindBufferBase;

    EXPECT_CALL(mockBindBufferBase, Call(Buffer::UniformBuffer, bindingIndexInShader, mat.getBufferObject()));
    auto bindBufferBaseFunc = mockBindBufferBase.AsStdFunction();
    mockglBindBufferBase = bindBufferBaseFunc;

    mat.activate();

    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    ASSERT_EQ(currentProgram, shader->getShaderProgram());
}

// test that the material attirbutes buffer is set up correclty so that it matches the layout of the uniform buffer within
// the shader
TEST_F(MaterialTests, materialBufferHasCorrectMemoryLayout) {
    glm::vec4 diffuse(1, 1, 0.5, 1);
    float specularCoefficient = 256;

    mat.setAttribute("diffuseColor", diffuse);
    mat.setAttribute("specularCoefficient", specularCoefficient);

    mat.activate();

    const int bufferSize = 16 + 4;
    int offsetToSpecular = 16;
    char data[bufferSize];

    glGetNamedBufferSubData(mat.getBufferObject(), 0, bufferSize, (GLvoid *)data);

    glm::vec4 diffuseResult = *((glm::vec4 *)data);
    float specularResult = *((float *)(data + offsetToSpecular));

    EXPECT_EQ(diffuse, diffuseResult);
    EXPECT_EQ(specularCoefficient, specularResult);
}

TEST_F(MaterialTests, canSetAndGetBoolAndGettingBoolAsIntFails) {
    bool diffuseTexture_Provided = true;
    bool otherTexture_Provided = false;

    mat.setAttribute("diffuseTexture_Provided", diffuseTexture_Provided);
    mat.setAttribute("otherTexture_Provided", otherTexture_Provided);

    mat.activate();

    bool diffuseResult = mat.getAttribute<bool>("diffuseTexture_Provided");
    bool otherResult = mat.getAttribute<bool>("otherTexture_Provided");

    EXPECT_EQ(diffuseResult, diffuseTexture_Provided);
    EXPECT_EQ(otherResult, otherTexture_Provided);

    try {
        int val = mat.getAttribute<int>("diffuseTexture_Provided");
        FAIL() << "retreiving bool attribtue as int should not succeed, but returned: " << val << endl;
    } catch (const string &e) {
        EXPECT_EQ(e, INCORRECT_TYPE_ERROR);
    }
}

// in glsl, booleans are stored as 4 bytes. So when we store bytes it must be saved as 4 bytes in the buffer.
// ensure that setting a boolean will set 4 bytes within the buffer
TEST_F(MaterialTests, settingBooleanWillSetCorrectSizeToMatchGLSL) {
    bool diffuseTexture_Provided = true;

    mat.setAttribute("diffuseColor", glm::vec4(9, 9, 9, 9));
    mat.setAttribute("specularCoefficient", 99);
    mat.setAttribute("diffuseTexture_Provided", diffuseTexture_Provided);
    mat.setAttribute("otherTexture_Provided", false);

    mat.activate();

    const int bufferSize = 24;
    int offsetToDiffuse = 20;
    char data[bufferSize];

    glGetNamedBufferSubData(mat.getBufferObject(), 0, bufferSize, (GLvoid *)data);

    int value = *((int *)(data + offsetToDiffuse));

    EXPECT_EQ(value, 1);
}

TEST_F(MaterialTests, whenTextureIsAddedMaterialBooleansAreCreatedAndSetToTrue) {
    const ImageTextureResource::Channels desiredChannels = ImageTextureResource::Channels::AUTO;

    ImageTextureResource image("testing_resources/images/3x3.jpg", desiredChannels);
    shared_ptr<GLTextureObject> obj = image.createGLTextureObject(0);

    mat.setTexture("diffuseTexture", obj);

    try {
        bool textureFlagInMaterial = mat.getAttribute<bool>("diffuseTexture" + Material::materialTextureProvidedFlagPrefix);
        EXPECT_EQ(textureFlagInMaterial, true);
    } catch (...) {
        FAIL() << "Failed to get the diffuseTexture flag from the material" << endl;
    }

    mat.activate();

    const int bufferSize = 24 + 4;
    int offsetToDiffuseFlag = 20;
    char data[bufferSize];

    glGetNamedBufferSubData(mat.getBufferObject(), 0, bufferSize, (GLvoid *)data);

    bool flag = *((bool *)(data + offsetToDiffuseFlag));

    ASSERT_EQ(flag, true);
}

TEST_F(MaterialTests,
       whenTextureIsAddedActivatingMaterialWillBindTheTextureToTheTextureUnitOfTheCorrespondingSamplerInTheShader) {
    const ImageTextureResource::Channels desiredChannels = ImageTextureResource::Channels::AUTO;

    GLuint textureUnit = 23;

    ImageTextureResource image("testing_resources/images/3x3.jpg", desiredChannels);
    shared_ptr<GLTextureObject> obj = image.createGLTextureObject(0);

    MockFunction<void(GLuint, GLuint)> mockBindTextureUnit;

    GLuint textureObj = obj->getTextureObject();

    EXPECT_CALL(mockBindTextureUnit, Call(textureUnit, textureObj));
    auto bindBufferBaseFunc = mockBindTextureUnit.AsStdFunction();
    mockglBindTextureUnit = bindBufferBaseFunc;

    mat.setTexture("diffuseTexture", obj);
    mat.activate();
}

// test copy constructor
TEST_F(MaterialTests, copyingExistingMaterialToNewMaterialWillCopyDataAndUseSameShadersAndTextures) {
    glm::vec4 diffuse(1, 1, 0.5, 1);
    float specularCoefficient = 256;
    const ImageTextureResource::Channels desiredChannels = ImageTextureResource::Channels::AUTO;
    GLuint textureUnit = 23;

    ImageTextureResource image("testing_resources/images/3x3.jpg", desiredChannels);
    shared_ptr<GLTextureObject> obj = image.createGLTextureObject(0);

    mat.setAttribute("diffuseColor", diffuse);
    mat.setAttribute("specularCoefficient", specularCoefficient);
    mat.setTexture("diffuseTexture", obj);
    mat.setAttribute("otherTexture" + Material::materialTextureProvidedFlagPrefix, false);
    mat.activate();

    MockFunction<void(GLuint, GLuint)> mockBindTextureUnit;

    GLuint textureObj = obj->getTextureObject();

    EXPECT_CALL(mockBindTextureUnit, Call(textureUnit, textureObj));
    auto bindBufferBaseFunc = mockBindTextureUnit.AsStdFunction();
    mockglBindTextureUnit = bindBufferBaseFunc;

    Material otherMat(mat);

    const int bufferSize = 24;
    char originalData[bufferSize];
    char copiedData[bufferSize];

    otherMat.activate();
    glGetNamedBufferSubData(mat.getBufferObject(), 0, bufferSize, (GLvoid *)originalData);
    glGetNamedBufferSubData(otherMat.getBufferObject(), 0, bufferSize, (GLvoid *)copiedData);

    EXPECT_EQ(memcmp(originalData, copiedData, bufferSize), 0);

    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    ASSERT_EQ(currentProgram, shader->getShaderProgram());

    GLenum error = glGetError();
    EXPECT_EQ(error, GL_NO_ERROR);
}

// test copy assignment operator
TEST_F(MaterialTests, copyingExistingMaterialToExistingMaterialWillCopyDataAndUseSameShadersAndTextures) {
    glm::vec4 diffuse(1, 1, 0.5, 1);
    float specularCoefficient = 256;
    const ImageTextureResource::Channels desiredChannels = ImageTextureResource::Channels::AUTO;
    GLuint textureUnit = 23;

    ImageTextureResource image("testing_resources/images/3x3.jpg", desiredChannels);
    shared_ptr<GLTextureObject> obj = image.createGLTextureObject(0);

    mat.setAttribute("diffuseColor", diffuse);
    mat.setAttribute("specularCoefficient", specularCoefficient);
    mat.setTexture("diffuseTexture", obj);
    mat.setAttribute("otherTexture" + Material::materialTextureProvidedFlagPrefix, false);
    mat.activate();

    MockFunction<void(GLuint, GLuint)> mockBindTextureUnit;

    GLuint textureObj = obj->getTextureObject();

    EXPECT_CALL(mockBindTextureUnit, Call(textureUnit, textureObj));
    auto bindBufferBaseFunc = mockBindTextureUnit.AsStdFunction();
    mockglBindTextureUnit = bindBufferBaseFunc;

    Material otherMat("blah", true);
    otherMat = mat;

    const int bufferSize = 24;
    char originalData[bufferSize];
    char copiedData[bufferSize];

    otherMat.activate();
    glGetNamedBufferSubData(mat.getBufferObject(), 0, bufferSize, (GLvoid *)originalData);
    glGetNamedBufferSubData(otherMat.getBufferObject(), 0, bufferSize, (GLvoid *)copiedData);

    EXPECT_EQ(memcmp(originalData, copiedData, bufferSize), 0);

    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    ASSERT_EQ(currentProgram, shader->getShaderProgram());

    GLenum error = glGetError();
    EXPECT_EQ(error, GL_NO_ERROR);
}