#pragma once

#include <vector>
#include <string>
#include "MeshData.h"
#include "ShaderProgram.h"

class Mesh {

public:

	bool loadFromFile(const std::string &filename);
	void setMaterial(ShaderProgram &program);

private:

	std::vector<MeshData> meshData;
};