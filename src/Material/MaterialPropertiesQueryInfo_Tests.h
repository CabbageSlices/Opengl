#ifndef __MATERIALPROPERTIESQUERY_TESTS_H__
#define __MATERIALPROPERTIESQUERY_TESTS_H__

#include "Includes.h"
#include "Material/MaterialPropertiesQueryInfo.h"
#include "Test/GraphicsTestFixture.h"
#include "components/Component.h"
#include "components/Component_Tests.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Mock;
using ::testing::MockFunction;

class MaterialPropertiesQueryInfoTests : public GraphicsTest {
  public:
    MaterialPropertiesQueryInfoTests() : shader(new ShaderProgram()), info() {}

    virtual ~MaterialPropertiesQueryInfoTests() = default;

    virtual void SetUp() {
        GraphicsTest::SetUp();
        shader->loadAndCompileShaders({{Shader::Type::Vertex, "testing_resources/shaders/vertex.vert"},
                                       {Shader::Type::Fragment, "testing_resources/shaders/fragment.frag"}},
                                      false);
        shader->linkProgram();
    }

    virtual void TearDown() { GraphicsTest::TearDown(); }

  protected:
    shared_ptr<ShaderProgram> shader;
    MaterialPropertiesQueryInfo info;
};

#endif  // __MATERIALPROPERTIESQUERY_TESTS_H__