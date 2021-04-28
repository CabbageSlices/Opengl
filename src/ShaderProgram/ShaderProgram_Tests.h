#include "Includes.h"
#include "ShaderProgram/ShaderProgram.h"
#include "Test/GraphicsTestFixture.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Mock;
using ::testing::MockFunction;

class ShaderProgramTests : public GraphicsTest {
  public:
    ShaderProgramTests() : shader() {}

    virtual ~ShaderProgramTests() = default;

    virtual void SetUp() {
        GraphicsTest::SetUp();
        shader.loadAndCompileShaders({{Shader::Type::Vertex, "testing_resources/shaders/vertex.vert"},
                                      {Shader::Type::Fragment, "testing_resources/shaders/fragment.frag"}},
                                     false);
        shader.linkProgram();
    }

    virtual void TearDown() { GraphicsTest::TearDown(); }

  protected:
    ShaderProgram shader;
};
