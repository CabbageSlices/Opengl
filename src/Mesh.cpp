#include "headers/Mesh.h"
#include "headers/MeshData.h"
#include "headers/loadFromObj.h"
#include "headers/Shader.h"
#include <iostream>

using std::cout;
using std::endl;

bool Mesh::loadFromFile(const std::string &filename) {

	if(vao != 0 || attributeBuffer != 0) {
		cout << "Already storing a previous mesh, please delete the mesh before loading" << endl;
		return false;
	}

	//load data
	if(!loadFromObj(filename, meshData))
		return false;

	//vertex array
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//buffers for vertex attributes
	unsigned positionArraySize = sizeof(glm::vec3) * meshData.positions.size();
	unsigned normalArraySize = sizeof(glm::vec3) * meshData.normals.size();

	unsigned totalAttributeBufferSize = positionArraySize + normalArraySize;

	glCreateBuffers(1, &attributeBuffer);
	glNamedBufferData(attributeBuffer, totalAttributeBufferSize, nullptr,
		GL_STATIC_DRAW);

	glNamedBufferSubData(attributeBuffer, 0, positionArraySize, meshData.positions.data());
	glNamedBufferSubData(attributeBuffer, positionArraySize, normalArraySize, meshData.normals.data());


	//buffer for vertex indices
	unsigned indicesArraySize = sizeof(unsigned int) * meshData.indices.size();

	glCreateBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glNamedBufferData(indexBuffer, indicesArraySize, meshData.indices.data(), GL_STATIC_DRAW);

	//buffer binding index that correspond to the different vertex attributes
	const unsigned POSITION_BUFFER_INDEX = 0;
	const unsigned NORMAL_BUFFER_INDEX = 1;
	const unsigned TEX_COORD_BUFFER_INDEX = 2;
	
	//vertex attributes
	glVertexArrayAttribBinding(vao, Shader::POSITION_ATTRIBUTE_INDEX, 0);
	glVertexArrayAttribBinding(vao, Shader::NORMAL_ATTRIBUTE_INDEX, 0);

	glVertexArrayVertexBuffer(vao, POSITION_BUFFER_INDEX, attributeBuffer,
		0, sizeof(glm::vec3));

	glVertexArrayAttribFormat(vao, Shader::POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(vao, Shader::NORMAL_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, positionArraySize);

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::NORMAL_ATTRIBUTE_INDEX);
	

	glBindVertexArray(0);
	//put into buffers
	//setup index buffer and shit
	//setup vertex array object
	//setup vertex attributes
	return true;
}

void Mesh::render() {

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, meshData.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}