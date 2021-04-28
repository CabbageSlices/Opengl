#ifndef __RENDERBLUEPRINT_TESTS_H__
#define __RENDERBLUEPRINT_TESTS_H__

#include "./RenderBlueprint.h"
#include "Includes.h"
#include "StringMethods.h"
#include "Test/GraphicsTestFixture.h"
#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Mock;
using ::testing::MockFunction;

class RenderBluePrintTests : public GraphicsTest {};

#endif  // __RENDERBLUEPRINT_TESTS_H__