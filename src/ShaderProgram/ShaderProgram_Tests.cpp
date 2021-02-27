#include "./ShaderProgram_Tests.h"

#include "gmock/gmock-function-mocker.h"

using testing::Mock;
using ::testing::MockFunction;

TEST_F(ShaderProgramTests, offsetToAttributeWithinUniformBlockIsCorrect) {
    std::map<std::string, GLint> offsets;
    offsets["dat1"] = 0;
    offsets["dat2"] = 16;
    offsets["dat3"] = 28;
    offsets["dat4"] = 32;
    offsets["dat5"] = 48;

    vector<string> names;
    for (auto &entry : offsets) {
        names.push_back(entry.first);
    }

    auto returnedOffsets = shader.getUniformOffsetForAttributes(names);

    for (auto &offset : offsets) {
        ASSERT_TRUE(returnedOffsets.count(offset.first) > 0)
            << "Offset of attribute " << offset.first << " is not available" << endl;
        ;
        EXPECT_EQ(offsets[offset.first], returnedOffsets[offset.first])
            << "Offset of " << offset.first << " should be " << offset.second << "; But actual offset is "
            << returnedOffsets[offset.first] << endl;
    }
}

TEST_F(ShaderProgramTests, missingAttributesDoNotReturnAnOffset) {
    vector<string> missingNames = {"missing1", "missing2"};

    auto returnedOffsets = shader.getUniformOffsetForAttributes(missingNames);

    for (auto &name : missingNames) {
        ASSERT_TRUE(returnedOffsets.count(name) == 0)
            << "Offset of attribute " << name << " should not exist, but a value was returned" << endl;
    }
}

TEST_F(ShaderProgramTests, uniformBlockHasValidIndex) {
    string blockName = "TestBlock";

    auto index = shader.getUniformBlockIndex(blockName);

    EXPECT_NE(index, GL_INVALID_INDEX);
}

TEST_F(ShaderProgramTests, nonExistantUniformBlockHasInvalidIndex) {
    string blockName = "MissingBlock";

    auto index = shader.getUniformBlockIndex(blockName);

    EXPECT_EQ(index, GL_INVALID_INDEX);
}

TEST_F(ShaderProgramTests, uniformBlockHasCorrectSize) {
    string blockName = "TestBlock";
    GLint expectedSize = 64;

    auto index = shader.getUniformBlockIndex(blockName);
    auto size = shader.getUniformBlockProperty(index, UniformBlockProperty::UniformBlockSizeInBytes);

    EXPECT_EQ(size, expectedSize);
}

TEST_F(ShaderProgramTests, uniformBlockHasCorrectBindingIndex) {
    string blockName = "TestBlock";
    GLint correctBindingIndex = 9;

    auto index = shader.getUniformBlockIndex(blockName);
    auto bindingIndex = shader.getUniformBlockProperty(index, UniformBlockProperty::UniformBlockBindingIndex);

    EXPECT_EQ(bindingIndex, correctBindingIndex);
}

TEST_F(ShaderProgramTests, canGetUniformVariableLocation) {
    string variableName = "uniformVariable";
    GLint correctLocation = 5;

    GLint location = shader.getUniformLocation(variableName);

    EXPECT_EQ(location, correctLocation);
}

TEST_F(ShaderProgramTests, nonExistingUniformVariableHaveBadLocation) {
    string variableName = "uniformVariableThatDoesntExist";
    GLint correctLocation = -1;

    GLint location = shader.getUniformLocation(variableName);

    EXPECT_EQ(location, correctLocation);
}

TEST_F(ShaderProgramTests, canGetCorrectValueForUniformVariable) {
    string variableName = "uniformVariable";
    glm::vec4 correctValue(1, 2, 3, 4);

    glm::vec4 value = shader.getUniform<glm::vec4>(variableName);

    EXPECT_EQ(correctValue, value);
}

TEST_F(ShaderProgramTests, nonExistingUniformVariableThrowsExceptionWhenGettingValue) {
    string variableName = "missingUniformVariable";

    try {
        glm::vec4 value = shader.getUniform<glm::vec4>(variableName);
        FAIL() << "Getting uniform variable with name: " << variableName
               << " returned a value even though it shouldn't exist:  " << glm::to_string(value) << endl;
    } catch (const string &e) {
        EXPECT_EQ(e, NOT_FOUND_ERROR) << "wrong exception thrown; received: " << e << " ; expected: " << NOT_FOUND_ERROR
                                      << endl;
        SUCCEED();
    }
}

TEST_F(ShaderProgramTests, gettingTheUniformValueForSamplerReturnsTheBindingIndex) {
    string variableName = "diffuseTexture_Sampler";
    GLint binding = 23;

    GLint value = shader.getUniform<GLint>(variableName);

    EXPECT_EQ(binding, value);
}