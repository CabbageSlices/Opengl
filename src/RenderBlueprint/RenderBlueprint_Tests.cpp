#include "./RenderBluePrint_Tests.h"

#include <string>

#include "StringMethods.h"
#include "gmock/gmock-function-mocker.h"

using std::string;
using testing::Mock;
using ::testing::MockFunction;

TEST_F(RenderBluePrintTests, throwsErrorIfBlueprintMissingShaderPrograms) {
    const string schema = R"({
        "passes": {
            "depth": {
                "shaderProgram": "defaultDepthShader"
            },
            "regular": {
                "shaderProgram": "somePassThatGeneratesFrameBuffer",
                "materials": ["nameOfMaterial1"]
            }
        }
    })";

    try {
        RenderBlueprint blueprint(schema);
        FAIL() << "didn't throw error even though it should" << endl;
    } catch (const string &err) {
        SUCCEED();
    }
}

TEST_F(RenderBluePrintTests, throwsErrorIfBlueprintMissingPasses) {
    const string schema = R"({
        "shaderPrograms": {
            "defaultDepthShader": {
                "vertex": "depth.vert",
                "fragment": "depth.frag"
            },
            "regularRenderPass": {
                "vertex": "vertex.vert",
                "fragment": "fragment.frag"
            }
        }
    })";

    try {
        RenderBlueprint blueprint(schema);
        FAIL() << "didn't throw error even though it should" << endl;
    } catch (const string &err) {
        SUCCEED();
    }
}

TEST_F(RenderBluePrintTests, allMaterialsAssignedToCorrectPassAndSupportsMultipleMaterials) {
    const string schema = loadFileToString("testing_resources/blueprints/testBlueprint.json");

    RenderBlueprint blueprint(schema);

    const auto &materialsDepth = blueprint.getMaterialsForPass(RenderingPass::DEPTH_PASS);

    EXPECT_EQ(materialsDepth.size(), 2);

    try {
        materialsDepth.at("TestMaterial");
        materialsDepth.at("TestMaterial2");
        SUCCEED();
    } catch (...) {
        FAIL() << "material missing for depth pass." << endl;
    }

    const auto &materialsRegular = blueprint.getMaterialsForPass(RenderingPass::REGULAR_PASS);

    EXPECT_EQ(materialsRegular.size(), 2);

    try {
        materialsRegular.at("TestMaterial");
        materialsRegular.at("TestBlock");
        SUCCEED();
    } catch (...) {
        FAIL() << "material missing for regular pass." << endl;
    }
}

TEST_F(RenderBluePrintTests, correctShaderProgramsReturnedForPass) {
    const string schema = loadFileToString("testing_resources/blueprints/testBlueprint.json");

    RenderBlueprint blueprint(schema);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);

    EXPECT_EQ(shaderDepth.first, "shader1");

    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    EXPECT_EQ(shaderRegular.first, "shader2");
}

TEST_F(RenderBluePrintTests, getMaterialPropertiesQueryReturnedForShaderProgram) {
    const string schema = loadFileToString("testing_resources/blueprints/testBlueprint.json");

    RenderBlueprint blueprint(schema);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);
    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    // depht pass
    auto testMaterialQueryInfoDepth =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestMaterial");
    EXPECT_EQ(testMaterialQueryInfoDepth->getUniformBlockName(), "TestMaterial");
    EXPECT_EQ(testMaterialQueryInfoDepth->uniformBlockBindingIndexInShader, 21);
    EXPECT_EQ(testMaterialQueryInfoDepth->attributeOffsets.size(), 7);
    EXPECT_NE(testMaterialQueryInfoDepth->uniformBlockSize, 0);

    auto testMaterial2QueryInfoDepth =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestMaterial2");

    EXPECT_EQ(testMaterial2QueryInfoDepth->getUniformBlockName(), "TestMaterial2");
    EXPECT_EQ(testMaterial2QueryInfoDepth->uniformBlockBindingIndexInShader, 22);
    EXPECT_EQ(testMaterial2QueryInfoDepth->attributeOffsets.size(), 7);
    EXPECT_NE(testMaterial2QueryInfoDepth->uniformBlockSize, 0);

    auto testMaterialQueryInfoRegular =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestMaterial");
    EXPECT_EQ(testMaterialQueryInfoRegular->getUniformBlockName(), "TestMaterial");
    EXPECT_EQ(testMaterialQueryInfoRegular->uniformBlockBindingIndexInShader, 40);
    EXPECT_EQ(testMaterialQueryInfoRegular->attributeOffsets.size(), 9);
    EXPECT_NE(testMaterialQueryInfoRegular->uniformBlockSize, 0);

    auto testBlockQueryInfoRegular = blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestBlock");
    EXPECT_EQ(testBlockQueryInfoRegular->getUniformBlockName(), "TestBlock");
    EXPECT_EQ(testBlockQueryInfoRegular->uniformBlockBindingIndexInShader, 9);
    EXPECT_EQ(testBlockQueryInfoRegular->attributeOffsets.size(), 5);
    EXPECT_NE(testBlockQueryInfoRegular->uniformBlockSize, 0);
}

TEST_F(RenderBluePrintTests, getMaterialPropertiesQueryThrowsIfMaterialNotUsedByShader) {
    const string schema = loadFileToString("testing_resources/blueprints/testBlueprint.json");

    RenderBlueprint blueprint(schema);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);
    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    try {
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestBlock");
        FAIL() << "depth pass doesnt use TestBlock but something was returned without throwing" << endl;
    } catch (...) {
        SUCCEED();
    }

    try {
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestBlock1");
        FAIL() << "Regular pass doesnt use TestBlock1 but query didnt throw" << endl;
    } catch (...) {
        SUCCEED();
    }
    // depht pass
}

TEST_F(RenderBluePrintTests, ifSameShaderIsUsedForMultiplePassesThenANewShaderIsntCreated) {
    const string &schema = R"(
        {
            "materials": ["TestMaterial", "TestMaterial2", "TestBlock"],
            "shaderPrograms": {
                "shader1": {
                "vertex": "testing_resources/shaders/vertex.vert",
                "fragment": "testing_resources/shaders/fragment.frag",
                "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestMaterial2"]
                },
                "REGULAR_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestBlock"]
                }
            }
        }
    )";

    RenderBlueprint blueprint(schema);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);
    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    EXPECT_EQ(shaderDepth.first, shaderRegular.first);
    EXPECT_EQ(shaderDepth.second, shaderDepth.second);
}

TEST_F(RenderBluePrintTests, ifSameShaderIsUsedForMultiplePassesEachMaterialForThatShaderOnlyHasOneQuery) {
    const string &schema = R"(
        {
            "materials": ["TestMaterial", "TestMaterial2", "TestBlock"],
            "shaderPrograms": {
                "shader1": {
                "vertex": "testing_resources/shaders/vertex.vert",
                "fragment": "testing_resources/shaders/fragment.frag",
                "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestMaterial2"]
                },
                "REGULAR_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestBlock"]
                }
            }
        }
    )";

    RenderBlueprint blueprint(schema);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);
    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    const auto &queryInfos = blueprint.getMaterialPropertiesQueryInfos();
    EXPECT_EQ(queryInfos.size(), 1);

    const auto &queryInfoForShader = queryInfos.at("shader1");
    EXPECT_EQ(queryInfoForShader.size(), 3);
    EXPECT_EQ(queryInfoForShader.count("TestMaterial"), 1);
    EXPECT_EQ(queryInfoForShader.count("TestMaterial2"), 1);
    EXPECT_EQ(queryInfoForShader.count("TestBlock"), 1);
}

TEST_F(RenderBluePrintTests, throwsIfBlueprintAssignsMaterialToShaderAndMaterialDoesntExistInShader) {
    const string &schema = R"(
        {
            "materials": ["TestMaterial", "TestMaterial2", "TestBlock", "notDefinedMaterial"],
            "shaderPrograms": {
                "shader1": {
                "vertex": "testing_resources/shaders/vertex.vert",
                "fragment": "testing_resources/shaders/fragment.frag",
                "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestMaterial2", "notDefinedMaterial"]
                },
                "REGULAR_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestBlock"]
                }
            }
        }
    )";

    try {
        RenderBlueprint blueprint(schema);
        FAIL() << "Should throw because notDefinedMaterial doesnt exist" << endl;
    } catch (...) {
        SUCCEED();
    }
}

TEST_F(RenderBluePrintTests, throwsIfMaterialIsUsedByPassButNotDefinedInMaterialsSection) {
    const string &schema = R"(
        {
            "materials": ["TestMaterial", "TestMaterial2", "TestBlock"],
            "shaderPrograms": {
                "shader1": {
                "vertex": "testing_resources/shaders/vertex.vert",
                "fragment": "testing_resources/shaders/fragment.frag",
                "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestMaterial2", "notDefinedMaterial"]
                },
                "REGULAR_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestBlock"]
                }
            }
        }
    )";

    try {
        RenderBlueprint blueprint(schema);
        FAIL() << "Should throw because notDefinedMaterial wasnt declared in materials section" << endl;
    } catch (...) {
        SUCCEED();
    }
}

TEST_F(RenderBluePrintTests, throwsIfPassUsesShaderThatIsntDefined) {
    const string &schema = R"(
        {
            "materials": ["TestMaterial", "TestMaterial2", "TestBlock"],
            "shaderPrograms": {
                "shader1": {
                "vertex": "testing_resources/shaders/vertex.vert",
                "fragment": "testing_resources/shaders/fragment.frag",
                "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                "shaderProgram": "shader2",
                "materials": ["TestMaterial", "TestMaterial2", "notDefinedMaterial"]
                },
                "REGULAR_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial", "TestBlock"]
                }
            }
        }
    )";

    try {
        RenderBlueprint blueprint(schema);
        FAIL() << "Should throw because shader2 wasnt declared in shaderPrograms section" << endl;
    } catch (...) {
        SUCCEED();
    }
}

TEST_F(RenderBluePrintTests, isPassSupportedReturnsCorrectValue) {
    const string schema = R"(
        {
            "materials": ["TestMaterial", "TestMaterial2"],
            "shaderPrograms": {
                "shader1": {
                "vertex": "testing_resources/shaders/vertex.vert",
                "fragment": "testing_resources/shaders/fragment.frag",
                "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                "shaderProgram": "shader1",
                "materials": ["TestMaterial"]
                }
            }
        }
    )";

    RenderBlueprint blueprint(schema);

    EXPECT_EQ(blueprint.isRenderPassSupported(RenderingPass::DEPTH_PASS), true);
    EXPECT_EQ(blueprint.isRenderPassSupported(RenderingPass::REGULAR_PASS), false);
}

TEST_F(RenderBluePrintTests, renderForPassReturnsCorrectValueWhenPassSupportedOrUnSupported) {
    const string &schema = R"(
        {
            "materials": [
                "TestMaterial",
                "TestMaterial2",
                "TestBlock"
            ],
            "shaderPrograms": {
                "shader1": {
                    "vertex": "testing_resources/shaders/vertex.vert",
                    "fragment": "testing_resources/shaders/fragment.frag",
                    "usingDefaultShaderDirectory": false
                },
                "shader2": {
                    "vertex": "testing_resources/shaders/vertex.vert",
                    "fragment": "testing_resources/shaders/fragment2.frag",
                    "usingDefaultShaderDirectory": false
                }
            },
            "passes": {
                "DEPTH_PASS": {
                    "shaderProgram": "shader1",
                    "materials": [
                        "TestMaterial",
                        "TestMaterial2"
                    ]
                }
            }
        }
    )";
    RenderBlueprint blueprint(schema);

    EXPECT_EQ(activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS), true);
    EXPECT_EQ(activateBlueprintForPass(blueprint, RenderingPass::REGULAR_PASS), false);
}

TEST_F(RenderBluePrintTests, correctShaderProgramActivatedWhenBluePrintIsActivatedForAPass) {
    string blueprintSchema = loadFileToString("testing_resources/blueprints/testBlueprint.json");
    RenderBlueprint blueprint(blueprintSchema);

    activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS);
    const auto &depthShaderProgram = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);

    GLuint shaderProgramName = depthShaderProgram.second->getShaderProgram();

    GLint activeShaderProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeShaderProgram);
    EXPECT_EQ(shaderProgramName, activeShaderProgram);

    // regular pass

    activateBlueprintForPass(blueprint, RenderingPass::REGULAR_PASS);
    const auto &regularShaderProgram = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);
    shaderProgramName = regularShaderProgram.second->getShaderProgram();
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeShaderProgram);
    EXPECT_EQ(shaderProgramName, activeShaderProgram);
}

TEST_F(RenderBluePrintTests, materialsAreGivenCorrectQueryInfoWhenActivatedForAPass) {
    string blueprintSchema = loadFileToString("testing_resources/blueprints/testBlueprint.json");
    RenderBlueprint blueprint(blueprintSchema);
    activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);
    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    const auto &depthPassMatQuery = blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestMaterial");
    const auto &depthPassMat2Query =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestMaterial2");

    const auto &depthMaterials = blueprint.getMaterialsForPass(RenderingPass::DEPTH_PASS);

    EXPECT_EQ(depthMaterials.at("TestMaterial")->getQueryInfo(), depthPassMatQuery);
    EXPECT_EQ(depthMaterials.at("TestMaterial2")->getQueryInfo(), depthPassMat2Query);

    activateBlueprintForPass(blueprint, RenderingPass::REGULAR_PASS);

    // regular pass
    const auto &regularPassMatQuery =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestMaterial");
    const auto &regularPassMat2Query =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestBlock");

    const auto &regularMaterials = blueprint.getMaterialsForPass(RenderingPass::REGULAR_PASS);

    EXPECT_EQ(regularMaterials.at("TestMaterial")->getQueryInfo(), regularPassMatQuery);
    EXPECT_EQ(regularMaterials.at("TestBlock")->getQueryInfo(), regularPassMat2Query);
}

TEST_F(RenderBluePrintTests, canSetAnExistingMaterialInABlueprintToAnotherMaterial) {
    string blueprintSchema = loadFileToString("testing_resources/blueprints/testBlueprint.json");
    RenderBlueprint blueprint(blueprintSchema);

    shared_ptr<Material> newMaterial(new Material());
    blueprint.setMaterial("TestMaterial", newMaterial);

    activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS);

    const auto &shaderDepth = blueprint.getShaderProgramForPass(RenderingPass::DEPTH_PASS);
    const auto &shaderRegular = blueprint.getShaderProgramForPass(RenderingPass::REGULAR_PASS);

    const auto &depthPassMatQuery = blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestMaterial");
    const auto &depthPassMat2Query =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderDepth.first, "TestMaterial2");

    const auto &depthMaterials = blueprint.getMaterialsForPass(RenderingPass::DEPTH_PASS);

    EXPECT_EQ(depthMaterials.at("TestMaterial")->getId(), newMaterial->getId());

    EXPECT_EQ(depthMaterials.at("TestMaterial")->getQueryInfo(), depthPassMatQuery);
    EXPECT_EQ(depthMaterials.at("TestMaterial2")->getQueryInfo(), depthPassMat2Query);

    activateBlueprintForPass(blueprint, RenderingPass::REGULAR_PASS);

    // regular pass
    const auto &regularPassMatQuery =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestMaterial");
    const auto &regularPassMat2Query =
        blueprint.getMaterialPropertiesQueryForShaderProgram(shaderRegular.first, "TestBlock");

    const auto &regularMaterials = blueprint.getMaterialsForPass(RenderingPass::REGULAR_PASS);

    EXPECT_EQ(regularMaterials.at("TestMaterial")->getQueryInfo(), regularPassMatQuery);
    EXPECT_EQ(regularMaterials.at("TestMaterial"), newMaterial);
    EXPECT_EQ(regularMaterials.at("TestBlock")->getQueryInfo(), regularPassMat2Query);
}

TEST_F(RenderBluePrintTests, whenAMaterialIsReplacedTheMaterialWillRequireABufferUpdate) {
    string blueprintSchema = loadFileToString("testing_resources/blueprints/testBlueprint.json");
    RenderBlueprint blueprint(blueprintSchema);
    activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS);

    shared_ptr<ShaderProgram> shader(new ShaderProgram());
    shared_ptr<MaterialPropertiesQueryInfo> materialQueryInfo(new MaterialPropertiesQueryInfo());

    shader->loadAndCompileShaders({{Shader::Type::Vertex, "testing_resources/shaders/vertex.vert"},
                                   {Shader::Type::Fragment, "testing_resources/shaders/fragment.frag"}},
                                  false);
    shader->linkProgram();
    materialQueryInfo->queryBlockData(shader, "TestMaterial");

    shared_ptr<Material> newMaterial(new Material(materialQueryInfo));
    newMaterial->setAttribute("diffuseColor", glm::vec4(1, 1, 0, 1));

    newMaterial->activate();

    blueprint.setMaterial("TestMaterial", newMaterial);

    EXPECT_EQ(newMaterial->isBufferUpdateRequired(), true);
}

TEST_F(RenderBluePrintTests, throwsIfYouTryToSetAMaterialThatIsntDefinedInTheBlueprint) {
    string blueprintSchema = loadFileToString("testing_resources/blueprints/testBlueprint.json");
    RenderBlueprint blueprint(blueprintSchema);
    activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS);

    shared_ptr<Material> newMaterial(new Material());

    try {
        blueprint.setMaterial("TestMaterial22", newMaterial);
        FAIL() << "Should not be able to set material \'TestMaterial22\'" << endl;
    } catch (...) {
        SUCCEED();
    }
}

TEST_F(RenderBluePrintTests, renderBlueprintPassDoesntNeedToDefineMaterials) {
    const string &schema = R"(
        {
  "materials": ["TestMaterial", "TestMaterial2", "TestBlock"],
  "shaderPrograms": {
    "shader1": {
      "vertex": "testing_resources/shaders/vertex.vert",
      "fragment": "testing_resources/shaders/fragment.frag",
      "usingDefaultShaderDirectory": false
    },
    "shader2": {
      "vertex": "testing_resources/shaders/vertex.vert",
      "fragment": "testing_resources/shaders/fragment2.frag",
      "usingDefaultShaderDirectory": false
    }
  },
  "passes": {
    "DEPTH_PASS": {
      "shaderProgram": "shader1"
    },
    "REGULAR_PASS": {
      "shaderProgram": "shader2",
      "materials": ["TestMaterial", "TestBlock"]
    }
  }
}

    )";

    try {
        RenderBlueprint blueprint(schema);
        activateBlueprintForPass(blueprint, RenderingPass::DEPTH_PASS);
        activateBlueprintForPass(blueprint, RenderingPass::REGULAR_PASS);
        SUCCEED();
    } catch (const std::exception &e) {
        FAIL() << "threw an exception even though it shouldn't. " << endl << e.what();
    } catch (...) {
        FAIL() << "threw an exception even though it shouldn't.";
    }
}