#pragma once

#include <vector>
#include "glm\glm.hpp"

struct MeshData {

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;

	std::vector<unsigned int> indices;
};