#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <map>
#include <tiny_obj_loader.h>
#include "glm\glm.hpp"
#include "./PrintVector.hpp"
#include "MeshData.h"
#include "Material.h"

using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::map;
using std::tuple;
using std::make_tuple;

template<typename T>
inline bool checkIsEqual(const T &a, const T &b) {

	T difference = a - b;

	float eplison = 0.001;
	return glm::dot(difference, difference) < eplison * eplison;
}

//tinyobj stores all atttributes as individual floats, store them as vectors so it's easy to index them
//normally if a mesh usees vertex index i, you need to access the attributes as i * 3 + 0,1,2
//which is annoying, so group them together in vectors so you can index them with just i
inline void tinyobjAttributeToMeshAttributes(const tinyobj::attrib_t & attributes, MeshAttributes &meshAttributes) {

	for(unsigned vertexIndex = 0, texCoordIndex = 0;
		vertexIndex < attributes.vertices.size(); vertexIndex += 3, texCoordIndex += 2) {

		glm::vec3 position = {attributes.vertices[vertexIndex], attributes.vertices[vertexIndex+1], attributes.vertices[vertexIndex+2]};
		glm::vec3 normal = {attributes.normals[vertexIndex], attributes.normals[vertexIndex+1], attributes.normals[vertexIndex+2]};

		meshAttributes.positions.push_back(position);
		meshAttributes.normals.push_back(normal);

		//might be missing tex coords
		if(attributes.texcoords.size() > 0) {
			glm::vec2 texCoord = {attributes.texcoords[texCoordIndex], attributes.texcoords[texCoordIndex + 1]};
			meshAttributes.texCoords.push_back(texCoord);
		}
	}
}

//move the normals and positions around so that the position, normals, and texcoord have the same index.
//Opengl only allows 1 index buffer that has to index both position and normal, but obj files have multiple indices
//so for vertex 5, in tinyobj the position might be at index 3, normal at 8, and texcoord at 1
//need to change this so all 3 attributes have the same index, ex: 8
//reorganizes position, normals, and texcoords so they can all be indexed by the same number
//does NOT change: list of vertices that make up a face, the order of the faces being drawn
//meaning if face f1 is made of vertices v1,v2,v3 then it will still be made up of the same vertices
//AND if face f1 is the first face being drawn before this function is called, then afterwards it will still be drawn first
inline void tinyobjAttributeIndicesToOpenglIndices(const MeshAttributes &originalAttributes, const std::vector<tinyobj::index_t> &originalIndices,
	MeshAttributes &reorderedAttributes, vector<unsigned int> &reorderedIndices) {
	
	typedef tuple<int, int, int> VertexIndex;
	map<VertexIndex, int> originalIndexToReorderedIndex;

	for(unsigned i = 0; i < originalIndices.size(); ++i) {

		tinyobj::index_t index = originalIndices[i];
		
		glm::vec3 position = originalAttributes.positions[index.vertex_index];
		glm::vec3 normal = originalAttributes.normals[index.normal_index];

		VertexIndex originalVertex = make_tuple(index.vertex_index, index.normal_index, index.texcoord_index);

		//the set {position, normal, texCoord} forms a unique vertex. If any attribute changes then it becomes a new vertex
		//if a vertex has already been indexed before use the index of the previous vertex instead of making a new entry
		if(originalIndexToReorderedIndex.count(originalVertex) != 0) {
			int combinedIndex = originalIndexToReorderedIndex[originalVertex];
			reorderedIndices.push_back(combinedIndex);
			continue;
		}

		//this combination is unique so it forms a new vertex, must add the vertex data for position, normal, and texcoord and index this new vertex
		reorderedAttributes.positions.push_back(position);
		reorderedAttributes.normals.push_back(normal);
		
		if(originalAttributes.texCoords.size() > 0) {
			glm::vec2 texCoord = originalAttributes.texCoords[index.texcoord_index];
			reorderedAttributes.texCoords.push_back(texCoord);
		}	

		int reorderedIndex = reorderedAttributes.positions.size() - 1;
		reorderedIndices.push_back(reorderedIndex);
		originalIndexToReorderedIndex[originalVertex] = reorderedIndex;

	}
}

inline void tinyobjMaterialToCustomMaterial (const tinyobj::material_t &tinyobjMaterial, Material &material) {

	material.diffuse = {tinyobjMaterial.diffuse[0], tinyobjMaterial.diffuse[1], tinyobjMaterial.diffuse[2], tinyobjMaterial.dissolve};
	material.name = tinyobjMaterial.name;
}

inline void tinyobjMaterialsToCustomMaterials(const vector<tinyobj::material_t> &tinyobjMaterials, vector<Material> &convertedMaterials) {

	for(unsigned i = 0; i < tinyobjMaterials.size(); ++i) {

		Material convertedMaterial;
		tinyobjMaterialToCustomMaterial(tinyobjMaterials[i], convertedMaterial);
		convertedMaterials.push_back(convertedMaterial);
	}
}

//materials array MUST NOT BE EMPTY
inline void generateMaterialFaceMap(const vector<unsigned int> &reorderedIndices, const vector<int> &meshMaterialIds,
	const vector<Material> &materials, map<string, FaceSet> &materialFaceMap) {

	//every 3 indices makes a face
	for(unsigned i = 0; i < reorderedIndices.size(); i += 3) {

		int faceId = i / 3;
		Face face(reorderedIndices[i], reorderedIndices[i + 1], reorderedIndices[i + 2]);

		int materialId = meshMaterialIds[faceId];

		//face has no material assigned to it, assign it to whatever material is available
		if(materialId == -1) {
			materialId = materials.size() - 1;
		}

		//TO-DO handle missing materials
		assert(materialId < materials.size());
		Material materialForFace = materials[materialId];

		//faceset does not exist for this material, create it
		if(materialFaceMap.count(materialForFace.name) == 0) {
			materialFaceMap[materialForFace.name] = FaceSet();
		}

		materialFaceMap[materialForFace.name].push_back(face);
	}
}

inline bool loadFromObj(std::string objFileName, MeshData &meshData/* MaterialManager &materialManager, std::vector<MeshData> &meshData*/) {

	//load into tinyobj format
	//reorganize vertex data so that position/normal/etec can be indexed with a single index instead of a unique index per attribute
	//convert every material from tinyobj format to custom material format
	//create meeshadata object
	//return

	//add fileformat if it's missing
	if(objFileName.find(".obj") == std::string::npos) {
		objFileName.append(".obj");
	}

	//there could be multiple objects in the obj file but for now just load the first one
	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string warning;
	string error;
	bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, objFileName.c_str());

	if(!result) {
		cout << error << endl;
		return false;
	}

	MeshAttributes originalAttributes;
	tinyobjAttributeToMeshAttributes(attrib, originalAttributes);

	//generate material
	vector<Material> convertedMaterials;
	tinyobjMaterialsToCustomMaterials(materials, convertedMaterials);

	//add default material so every mesh always has some material
	if(convertedMaterials.size() == 0)
		convertedMaterials.push_back(Material());

	tinyobj::mesh_t mesh = shapes[0].mesh;
	MeshAttributes reorderedAttributes;
	vector<unsigned int> reorderedIndices;
	std::map<std::string, FaceSet> materialFaceMap;

	//reorganize vertex data so that position/normal/etec can be indexed with a single index instead of a unique index per attribute
	tinyobjAttributeIndicesToOpenglIndices(originalAttributes, mesh.indices, reorderedAttributes, reorderedIndices);
	generateMaterialFaceMap(reorderedIndices, mesh.material_ids, convertedMaterials, materialFaceMap);

	// meshData.positions = reorderedAttributes.positions;
	// meshData.normals = reorderedAttributes.normals;
	// meshData.indices = reorderedIndices;

	meshData.attributes = reorderedAttributes;
	meshData.materialFaceMap = materialFaceMap;

	return true;
}


/**
 * @brief      Loads mesh data from obj file
 *
 * @param[in]  filename  The filename of object to load. If .obj extension is missing it will be added
 * @param      meshData  location to store newly loaded mesh data.
 *
 * @return     returns true if it loaded successfully, false otherwise
 */
// inline bool loadFromObj(std::string filename, MeshData &meshData) {

// 	//add fileformat if it's missing
// 	if(filename.find(".obj") == std::string::npos) {
// 		filename.append(".obj");
// 	}

// 	//there could be multiple objects in the obj file but for now just load the first one
// 	tinyobj::attrib_t attrib;
// 	vector<tinyobj::shape_t> shapes;
// 	vector<tinyobj::material_t> materials;

// 	string warning;
// 	string error;
// 	bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, filename.c_str());

// 	if(!result) {
// 		cout << error << endl;
// 		return false;
// 	}

// 	//create a temporary storage for position and normal data
// 	//use this to move the normals and positions around so that the position and normals
// 	//have the same index. Opengl only allows 1 index buffer that has to index both position
// 	//and normal, but obj files have multiple indices
// 	//assume there is only 1 shape
	
// 	vector<glm::vec3> originalPositions;
// 	vector<glm::vec3> originalNormals;

// 	for(unsigned i = 0; i < attrib.vertices.size(); i += 3) {

// 		glm::vec3 position = {attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]};
// 		glm::vec3 normal = {attrib.normals[i], attrib.normals[i+1], attrib.normals[i+2]};

// 		originalPositions.push_back(position);
// 		originalNormals.push_back(normal);
// 	}

// 	vector<glm::vec3> reorderedPositions;
// 	vector<glm::vec3> reorderedNormals;
// 	vector<unsigned int> combinedIndices;

//   /**
//    * @brief      Finds index of matching position and normal in the reordered array, if there is any.
//    * Searches through the reordered arrays to see if the given position and normal have already been
//    * inserted into the reordered arrays in the exact same position. If so it is a match and we can use
//    * the same index to access both the position and normal, so we can use that index.
//    *
//    * @param[in]  position  The position
//    * @param[in]  normal    The normal
//    *
//    * @return     index in reoredered arrays where the matching position and normal can be found. -1 if no match.
//    */
// 	auto findMatchingPositionNormalPair = [&](glm::vec3 position, glm::vec3 normal) ->int {

// 		for(unsigned i = 0; i < reorderedPositions.size(); ++i) {

// 			bool isPositionMatch = checkIsEqual(position, reorderedPositions[i]);
// 			bool isNormalMatch = checkIsEqual(normal, reorderedNormals[i]);

// 			if(isPositionMatch && isNormalMatch) {
// 				return i;
// 			}
// 		}

// 		return -1;
// 	};

// 	auto mesh = shapes[0].mesh;
// 	for(unsigned i = 0; i < mesh.indices.size(); ++i) {

// 		auto index = mesh.indices[i];
// 	//	mesh.material_ids[i / 3] each face GROUP OF 3 VERTICES will have a material assigned to it, SAVE IT SOMEHOW
// 		//check if the current postiion and normal vector pair have already been added to the reordered
// 		//vectors. If the given pair is already reordered then get the index for that pair and use it for the
// 		//new combined index. otherwise add it to the reordered set and then add the index.
// 		glm::vec3 position = originalPositions[index.vertex_index];
// 		glm::vec3 normal = originalNormals[index.normal_index];

// 		int matchingPairIndex = findMatchingPositionNormalPair(position, normal);

// 		//pair of position and normal already in the array, use the index of previous entry
// 		if(matchingPairIndex != -1) {
// 			combinedIndices.push_back(matchingPairIndex);
// 			continue;
// 		}

// 		//current pair has not been stored yet, add it now and get the current index
// 		reorderedPositions.push_back(position);
// 		reorderedNormals.push_back(normal);

// 		combinedIndices.push_back(reorderedPositions.size() - 1);
// 	}

// 	//we can store positions and normals as is to the meshdata object
// 	meshData.positions = reorderedPositions;
// 	meshData.normals = reorderedNormals;
// 	meshData.indices = combinedIndices;

// 	return true;
// }