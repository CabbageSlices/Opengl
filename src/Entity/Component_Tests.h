#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "./Component.h"

class Entity;
class MockComponent : public ComponentBase {
public:
    virtual ~MockComponent() = default;

    MOCK_METHOD(void, registerCallbacksToEntity, (Entity* entity), (override));
};