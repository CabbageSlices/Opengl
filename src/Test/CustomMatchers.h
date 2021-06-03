#ifndef __CUSTOMMATCHERS_H__
#define __CUSTOMMATCHERS_H__

#include "glm/gtc/epsilon.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

MATCHER_P2(EXPECT_VEC4_EQ, vec1, vec2, "are equal") { return glm::epsilonEqual(vec1, vec2, glm::epsilon<float>()); }

#endif  // __CUSTOMMATCHERS_H__