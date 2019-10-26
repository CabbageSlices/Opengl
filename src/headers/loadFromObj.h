#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include <map>
#include <tiny_obj_loader.h>
#include "glm\glm.hpp"
#include "./PrintVector.hpp"
#include "MeshData.h"
#include "Material.h"

using std::shared_ptr;
using std::make_shared;
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

inline void tinyobjMaterialToCustomMaterial (const tinyobj::material_t &tinyobjMaterial, shared_ptr<Material> &material) {

	material->diffuse = {tinyobjMaterial.diffuse[0], tinyobjMaterial.diffuse[1], tinyobjMaterial.diffuse[2], tinyobjMaterial.dissolve};
	material->name = tinyobjMaterial.name;
}

inline void tinyobjMaterialsToCustomMaterials(const vector<tinyobj::material_t> &tinyobjMaterials, vector<shared_ptr<Material> > &convertedMaterials) {

	for(unsigned i = 0; i < tinyobjMaterials.size(); ++i) {

		shared_ptr<Material> convertedMaterial = make_shared<Material>();
		tinyobjMaterialToCustomMaterial(tinyobjMaterials[i], convertedMaterial);
		convertedMaterials.push_back(convertedMaterial);
	}
}

//materials array MUST NOT BE EMPTY
inline void generateMaterialFaceMap(const vector<unsigned int> &reorderedIndices, const vector<int> &meshMaterialIndices,
	const vector<shared_ptr<Material> > &materials, map<int, FaceSet> &materialFaceMap) {

	//every 3 indices makes a face
	for(unsigned i = 0; i < reorderedIndices.size(); i += 3) {

		int faceId = i / 3;
		Face face{reorderedIndices[i], reorderedIndices[i + 1], reorderedIndices[i + 2]};

		int materialIndex = meshMaterialIndices[faceId];

		//face has no material assigned to it, assign it to whatever material is available
		if(materialIndex == -1) {
			materialIndex = materials.size() - 1;
		}

		//TO-DO handle missing materials
		assert(materialIndex < materials.size());
		shared_ptr<Material> materialForFace = materials[materialIndex];

		//faceset does not exist for this material, create it
		if(materialFaceMap.count(materialForFace->id) == 0) {
			materialFaceMap[materialForFace->id] = FaceSet();
		}
		int id = materialForFace->id;
		materialFaceMap[materialForFace->id].push_back(face);
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
	vector<shared_ptr<Material> > convertedMaterials;
	tinyobjMaterialsToCustomMaterials(materials, convertedMaterials);

	//add default material so every mesh always has some material
	if(convertedMaterials.size() == 0)
		convertedMaterials.push_back(make_shared<Material>());

	tinyobj::mesh_t mesh = shapes[0].mesh;
	MeshAttributes reorderedAttributes;
	vector<unsigned int> reorderedIndices;
	map<int, FaceSet> materialFaceMap;

	//reorganize vertex data so that position/normal/etec can be indexed with a single index instead of a unique index per attribute
	tinyobjAttributeIndicesToOpenglIndices(originalAttributes, mesh.indices, reorderedAttributes, reorderedIndices);
	generateMaterialFaceMap(reorderedIndices, mesh.material_ids, convertedMaterials, materialFaceMap);

	meshData.numIndices = reorderedIndices.size();
	meshData.attributes = reorderedAttributes;
	meshData.materialFaceMap = materialFaceMap;
	meshData.materials = convertedMaterials;

	return true;
}