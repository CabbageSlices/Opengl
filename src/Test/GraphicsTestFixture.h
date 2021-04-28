#pragma once
#include <memory>

#include "GraphicsWrapper.h"
#include "Includes.h"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"
#include "SFML\System.hpp"
#include "components/Component.h"
#include "components/Component_Tests.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::shared_ptr;
using testing::Mock;
using ::testing::MockFunction;

// test fixture that all tests should inherit from in order to use the fake opengl context.
// will reset the context between each test
class GraphicsTest : public ::testing::Test {
  public:
    GraphicsTest() : context(new sf::Context(sf::ContextSettings(24, 8, 4, 4, 6, sf::ContextSettings::Core), 800, 600)) {
        context->setActive(true);
    }

    virtual ~GraphicsTest() = 0;

    virtual void SetUp() override {}

    virtual void TearDown() override {}

  protected:
    shared_ptr<sf::Context> context;
};
