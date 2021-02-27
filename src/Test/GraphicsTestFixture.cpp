#include "Test/GraphicsTestFixture.h"

GraphicsTest::~GraphicsTest() {
    context->setActive(false);
    clearAllMockFunctions();
}