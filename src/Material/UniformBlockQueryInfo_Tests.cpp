#include "./UniformBlockQueryInfo_Tests.h"

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

TEST_F(UniformBlockQueryInfoTests, UniformBlockQueryCorrectlyRetrievesAllBlockDataWithoutBLockInstance) {
    info.queryBlockData(shader, "TestMaterial");

    const int bufferSize = 80;
    int bindingIndexInShader = 21;
    int offsetToSpecular = 16;
    bool blockNamePrefixRequiredForAttributeNames = false;

    EXPECT_EQ(info.uniformBlockSize, bufferSize);
    EXPECT_EQ(info.uniformBlockBindingIndexInShader, bindingIndexInShader);
    EXPECT_EQ(info.attributeOffsets["specularCoefficient"], offsetToSpecular);
    EXPECT_EQ(info.blockNamePrefixRequiredForAttributeNames, blockNamePrefixRequiredForAttributeNames);
}

TEST_F(UniformBlockQueryInfoTests, attributeOffsetsContainsAttributeNamesWithoutBlockNamePrefix) {
    info.queryBlockData(shader, "TestMaterial2");

    for (auto& offset : info.attributeOffsets) {
        EXPECT_EQ(offset.first.find("."), string::npos)
            << "found an attribute name that contains the block name even though it shouldn't: " << offset.first << endl;
    }
}

TEST_F(UniformBlockQueryInfoTests, UniformBlockQueryCorrectlyRetrievesAllBlockDataWithBLockInstance) {
    info.queryBlockData(shader, "TestMaterial2");

    const int bufferSize = 80;
    int bindingIndexInShader = 22;
    int offsetToSpecular = 16;
    bool blockNamePrefixRequiredForAttributeNames = true;

    EXPECT_EQ(info.uniformBlockSize, bufferSize);
    EXPECT_EQ(info.uniformBlockBindingIndexInShader, bindingIndexInShader);
    EXPECT_EQ(info.attributeOffsets["specularCoefficient1"], offsetToSpecular);
    EXPECT_EQ(info.blockNamePrefixRequiredForAttributeNames, blockNamePrefixRequiredForAttributeNames);
}