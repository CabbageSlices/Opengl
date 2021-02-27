#include "./Entity.h"
#include "Includes.h"
#include "Test/GraphicsTestFixture.h"
#include "components/Component.h"
#include "components/Component_Tests.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Mock;
using ::testing::MockFunction;

class EntityTest : public GraphicsTest {
  public:
    EntityTest()
        : GraphicsTest(),
          entity(),
          mockComponent(new MockComponent()),
          mockComponent2(new MockComponent()),
          copyOfMockComponent(mockComponent) {}

    virtual ~EntityTest() {}

  protected:
    Entity entity;
    std::shared_ptr<MockComponent> mockComponent;
    std::shared_ptr<MockComponent> mockComponent2;
    std::shared_ptr<MockComponent> copyOfMockComponent;
    MockFunction<Entity::UpdateCallbackSignature> updateCallback;
    MockFunction<Entity::UpdateCallbackSignature> updateCallback2;
    MockFunction<Entity::RenderCallbackSignature> renderCallback;
};
