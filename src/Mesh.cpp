#include "headers/Mesh.h"
#include "headers/MeshData.h"
#include "headers/loadFromObj.h"
#include "headers/Shader.h"
#include <iostream>

using std::cout;
using std::endl;

bool Mesh::loadFromFile(const std::string &filename) {

	if(vao.getVao() != 0 || attributeBuffer.isUsed()) {
		cout << "Already storing a previous mesh, please delete the mesh before loading" << endl;
		return false;
	}

	//load data
	if(!loadFromObj(filename, meshData))
		return false;

	initializeAttributeBuffers();
	initializeIndexBuffer();
	initializeVertexArrayObject();
	
	return true;
}

void Mesh::render() {

	vao.bindToContext();

	//render faceset of each material
	//number of indices that have already been drawn
	unsigned int bytesAlreadyRead = 0 * sizeof(unsigned int);
	
	for(unsigned i = 0; i < meshData.materials.size(); ++i) {

		int materialId = meshData.materials[i]->id;
		FaceSet faceset = meshData.materialFaceMap[materialId];

		unsigned numIndicesPerFace = 3;
		unsigned numIndicesCurrentFaceset = faceset.size() * numIndicesPerFace;

		glDrawElements(GL_TRIANGLES, numIndicesCurrentFaceset, GL_UNSIGNED_INT, (void*)bytesAlreadyRead);//wtf? convert integer directly to pointer and not the address

		 bytesAlreadyRead += sizeof(Face) * faceset.size();
	}
	
	vao.unbindFromContext();
}


bool Mesh::initializeAttributeBuffers() {

	unsigned positionArraySize = sizeof(glm::vec3) * meshData.attributes.positions.size();
	unsigned normalArraySize = sizeof(glm::vec3) * meshData.attributes.normals.size();

	unsigned totalAttributeBufferSize = positionArraySize + normalArraySize;

	attributeBuffer.create(Buffer::ArrayBuffer, nullptr, totalAttributeBufferSize, Buffer::StaticDraw);
	attributeBuffer.updateData(meshData.attributes.positions.data(), positionArraySize, 0);
	attributeBuffer.updateData(meshData.attributes.normals.data(), normalArraySize, positionArraySize);
}	

bool Mesh::initializeIndexBuffer() {

	unsigned indicesArraySize = sizeof(unsigned int) * meshData.numIndices;

	indexBuffer.create(Buffer::ElementArrayBuffer, 0, indicesArraySize, Buffer::StaticDraw);

	//set of indices for every material should be added to the buffer
	//need to keep track of how much data has already been added to the buffer due to previous materials
	//in bytes
	int dataOffset = 0;

	//add each faceset for each material to the index buffer
	for(unsigned i = 0; i < meshData.materials.size(); ++i) {

		//current materials indices
		int materialId = meshData.materials[i]->id;
		FaceSet faceset = meshData.materialFaceMap[materialId];

		indexBuffer.updateData(faceset.data(), sizeof(Face) * faceset.size(), dataOffset);

		dataOffset += faceset.size() * sizeof(Face);
	}

}

void Mesh::initializeVertexArrayObject() {
	//buffer binding index that correspond to the different vertex attributes
	const unsigned POSITION_BUFFER_INDEX = 0;
	const unsigned NORMAL_BUFFER_INDEX = 1;
	const unsigned TEX_COORD_BUFFER_INDEX = 2;

	unsigned int positionBufferSize = sizeof(glm::vec3) * meshData.attributes.positions.size();

	vao.vertexAttributeShaderLocationToBufferIndex(Shader::POSITION_ATTRIBUTE_INDEX, 0);
	vao.vertexAttributeShaderLocationToBufferIndex(Shader::NORMAL_ATTRIBUTE_INDEX, 0);
	vao.attachElementBuffer(indexBuffer);
	vao.bindVertexAttributeBuffer(POSITION_BUFFER_INDEX, attributeBuffer, 0, sizeof(glm::vec3));
	vao.formatVertexAttributeData(Shader::POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, 0, GL_FALSE);
	vao.formatVertexAttributeData(Shader::NORMAL_ATTRIBUTE_INDEX, 3, GL_FLOAT, positionBufferSize, GL_FALSE);
}