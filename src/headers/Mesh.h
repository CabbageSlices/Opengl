#pragma once

#include <vector>
#include <string>
#include "MeshData.h"
#include "ShaderProgram.h"
#include "glad\glad.h"

class Mesh {

public:

	Mesh() : attributeBuffer(0), indexBuffer(0), vao(0) {};
	
	~Mesh() {
		deleteMeshData();
	}

	bool loadFromFile(const std::string &filename);
	void setMaterial(ShaderProgram &program);

	void deleteMeshData() {
		glDeleteBuffers(1, &attributeBuffer);
		glDeleteBuffers(1, &indexBuffer);
		glDeleteVertexArrays(1, &vao);

		attributeBuffer = 0;
		indexBuffer = 0;
		vao = 0;

		meshData.clear();
	}

private:

	MeshData meshData;
	GLuint attributeBuffer;
	GLuint indexBuffer;
	GLuint vao;
};