#pragma once

#include <vector>
#include <string>
#include <map>
#include "glm\glm.hpp"

struct MeshAttributes {

	void clear() {
		positions.clear();
		normals.clear();
		texCoords.clear();
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
};

//3 indices to make up a face
//each index should refer to position, normal, and texcoords
typedef glm::ivec3 Face;

//collection of faces, this way all indices are consequtive and laid out in memory linearly so you can send them to opengl
typedef std::vector<Face> FaceSet;

struct MeshData {

	void clear() {
		positions.clear();
		normals.clear();
		indices.clear();

		attributes.clear();

		materialFaceMap.clear();
	}

	MeshAttributes attributes;
	
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;


	//map of material name to the faces that use that material
	//maps material id to faceset
	std::map<int, FaceSet> materialFaceMap;

	std::vector<unsigned int> indices;
};