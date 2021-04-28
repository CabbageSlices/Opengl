#ifndef __UNIFORMBLOCKQUERY_TESTS_H__
#define __UNIFORMBLOCKQUERY_TESTS_H__

#include "Includes.h"
#include "Material/UniformBlockQueryInfo.h"
#include "Test/GraphicsTestFixture.h"
#include "components/Component.h"
#include "components/Component_Tests.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Mock;
using ::testing::MockFunction;

class UniformBlockQueryInfoTests : public GraphicsTest {
  public:
    UniformBlockQueryInfoTests() : shader(new ShaderProgram()), info() {}

    virtual ~UniformBlockQueryInfoTests() = default;

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
    UniformBlockQueryInfo info;
};

#endif  // __UNIFORMBLOCKQUERY_TESTS_H__