#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <tiny_obj_loader.h>
#include "glm\glm.hpp"
#include "MeshData.h"

using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::string;

inline ostream& operator<< (ostream& stream, glm::vec3 val) {

	stream << "x: " << val.x << ", y: " << val.y << ", z: " << val.z;
	return stream;
}

template<typename T>
inline bool checkIsEqual(const T &a, const T &b) {

	T difference = a - b;

	float eplison = 0.001;
	return glm::dot(difference, difference) < eplison * eplison;
}


/**
 * @brief      Loads mesh data from obj file
 *
 * @param[in]  filename  The filename of object to load. If .obj extension is missing it will be added
 * @param      meshData  location to store newly loaded mesh data.
 *
 * @return     returns true if it loaded successfully, false otherwise
 */
inline bool loadFromObj(std::string filename, MeshData &meshData) {

	//add fileformat if it's missing
	if(filename.find(".obj") == std::string::npos) {
		filename.append(".obj");
	}

	//there could be multiple objects in the obj file but for now just load the first one
	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string warning;
	string error;
	bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, filename.c_str());

	if(!result) {
		cout << error << endl;
		return false;
	}

	//create a temporary storage for position and normal data
	//use this to move the normals and positions around so that the position and normals
	//have the same index. Opengl only allows 1 index buffer that has to index both position
	//and normal, but obj files have multiple indices
	//assume there is only 1 shape
	
	vector<glm::vec3> originalPositions;
	vector<glm::vec3> originalNormals;

	for(unsigned i = 0; i < attrib.vertices.size(); i += 3) {

		glm::vec3 position = {attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]};
		glm::vec3 normal = {attrib.normals[i], attrib.normals[i+1], attrib.normals[i+2]};

		originalPositions.push_back(position);
		originalNormals.push_back(normal);
	}

	vector<glm::vec3> reorderedPositions;
	vector<glm::vec3> reorderedNormals;
	vector<unsigned int> combinedIndices;

  /**
   * @brief      Finds index of matching position and normal in the reordered array, if there is any.
   * Searches through the reordered arrays to see if the given position and normal have already been
   * inserted into the reordered arrays in the exact same position. If so it is a match and we can use
   * the same index to access both the position and normal, so we can use that index.
   *
   * @param[in]  position  The position
   * @param[in]  normal    The normal
   *
   * @return     index in reoredered arrays where the matching position and normal can be found. -1 if no match.
   */
	auto findMatchingPositionNormalPair = [&](glm::vec3 position, glm::vec3 normal) ->int {

		for(unsigned i = 0; i < reorderedPositions.size(); ++i) {

			bool isPositionMatch = checkIsEqual(position, reorderedPositions[i]);
			bool isNormalMatch = checkIsEqual(normal, reorderedNormals[i]);

			if(isPositionMatch && isNormalMatch) {
				return i;
			}
		}

		return -1;
	};

	auto mesh = shapes[0].mesh;
	for(unsigned i = 0; i < mesh.indices.size(); ++i) {

		auto index = mesh.indices[i];
		//check if the current postiion and normal vector pair have already been added to the reordered
		//vectors. If the given pair is already reordered then get the index for that pair and use it for the
		//new combined index. otherwise add it to the reordered set and then add the index.
		glm::vec3 position = originalPositions[index.vertex_index];
		glm::vec3 normal = originalNormals[index.normal_index];

		int matchingPairIndex = findMatchingPositionNormalPair(position, normal);

		//pair of position and normal already in the array, use the index of previous entry
		if(matchingPairIndex != -1) {
			combinedIndices.push_back(matchingPairIndex);
			continue;
		}

		//current pair has not been stored yet, add it now and get the current index
		reorderedPositions.push_back(position);
		reorderedNormals.push_back(normal);

		combinedIndices.push_back(reorderedPositions.size() - 1);
	}

	//we can store positions and normals as is to the meshdata object
	meshData.positions = reorderedPositions;
	meshData.normals = reorderedNormals;
	meshData.indices = combinedIndices;

	return true;
}