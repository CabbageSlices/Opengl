#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "./GraphicsWrapper.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "stb_image.h"
#include "uniformBlockBindings.vert"
#include "uniformLocations.vert"

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;