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

	//vertex array
	// glCreateVertexArrays(1, &vao);
	// glBindVertexArray(vao);

	//buffers for vertex attributes
	unsigned positionArraySize = sizeof(glm::vec3) * meshData.positions.size();
	unsigned normalArraySize = sizeof(glm::vec3) * meshData.normals.size();

	unsigned totalAttributeBufferSize = positionArraySize + normalArraySize;

	attributeBuffer.create(Buffer::ArrayBuffer, nullptr, totalAttributeBufferSize, Buffer::StaticDraw);
	attributeBuffer.updateData(meshData.positions.data(), positionArraySize, 0);
	attributeBuffer.updateData(meshData.normals.data(), normalArraySize, positionArraySize);

	//buffer for vertex indices
	unsigned indicesArraySize = sizeof(unsigned int) * meshData.indices.size();

	indexBuffer.create(Buffer::ElementArrayBuffer, meshData.indices.data(), indicesArraySize, Buffer::StaticDraw);
	// indexBuffer.bindToTarget();

	//buffer binding index that correspond to the different vertex attributes
	const unsigned POSITION_BUFFER_INDEX = 0;
	const unsigned NORMAL_BUFFER_INDEX = 1;
	const unsigned TEX_COORD_BUFFER_INDEX = 2;

	vao.vertexAttributeShaderLocationToBufferIndex(Shader::POSITION_ATTRIBUTE_INDEX, 0);
	vao.vertexAttributeShaderLocationToBufferIndex(Shader::NORMAL_ATTRIBUTE_INDEX, 0);
	vao.attachElementBuffer(indexBuffer);
	vao.bindVertexAttributeBuffer(POSITION_BUFFER_INDEX, attributeBuffer, 0, sizeof(glm::vec3));
	vao.formatVertexAttributeData(Shader::POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, 0, GL_FALSE);
	vao.formatVertexAttributeData(Shader::NORMAL_ATTRIBUTE_INDEX, 3, GL_FLOAT, positionArraySize, GL_FALSE);
	// vao.enableVertexAttribute(shaderAttributeLocation);

	//vertex attributes
	// glVertexArrayAttribBinding(vao, Shader::POSITION_ATTRIBUTE_INDEX, 0);
	// glVertexArrayAttribBinding(vao, Shader::NORMAL_ATTRIBUTE_INDEX, 0);

	// glVertexArrayElementBuffer(vao, indexBuffer.getBufferObject());
	// glVertexArrayVertexBuffer(vao, POSITION_BUFFER_INDEX, attributeBuffer.getBufferObject(),
	// 	0, sizeof(glm::vec3));

	// glVertexArrayAttribFormat(vao, Shader::POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	// glVertexArrayAttribFormat(vao, Shader::NORMAL_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, positionArraySize);

	// glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE_INDEX);
	// glEnableVertexAttribArray(Shader::NORMAL_ATTRIBUTE_INDEX);
	

	glBindVertexArray(0);
	//put into buffers
	//setup index buffer and shit
	//setup vertex array object
	//setup vertex attributes
	return true;
}

void Mesh::render() {

	// glBindVertexArray(vao);
	vao.bindToContext();
	glDrawElements(GL_TRIANGLES, meshData.indices.size(), GL_UNSIGNED_INT, 0);
	// glBindVertexArray(0);
	vao.unbindFromContext();
}