#include "./MaterialPropertiesQueryInfo_Tests.h"

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
TEST_F(MaterialPropertiesQueryInfoTests, TextureSamplerNamesCorrectlyExtractedWhenUniformBlockDoesntHaveInstance) {
    info.queryBlockData(shader, "TestMaterial");

    vector<string> textureNames = info.getTextureNames();

    map<string, int> textureNameLookup{{"diffuseTexture", 1}, {"otherTexture", 1}};

    for (auto &name : textureNames) {
        EXPECT_EQ(textureNameLookup.count(name), 1)
            << "Texture of name \"" << name << "\" extracted, but should not exist" << endl;
    }
}

TEST_F(MaterialPropertiesQueryInfoTests, TextureSamplerNamesCorrectlyExtractedWhenUniformBlockHasInstance) {
    info.queryBlockData(shader, "TestMaterial2");

    vector<string> textureNames = info.getTextureNames();

    map<string, int> textureNameLookup{{"diffuseTexture1", 1}, {"otherTexture1", 1}};

    for (auto &name : textureNames) {
        EXPECT_EQ(textureNameLookup.count(name), 1)
            << "Texture of name \"" << name << "\" extracted, but should not exist" << endl;
    }
}

// TODO above test but for testmaterial2