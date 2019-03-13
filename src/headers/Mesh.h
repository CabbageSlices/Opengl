#pragma once

#include <vector>
#include <string>
#include "MeshData.h"
#include "ShaderProgram.h"
#include "glad\glad.h"
#include "Buffer.h"

class Mesh {

public:

	Mesh() : attributeBuffer(), indexBuffer(), vao(0) {};
	
	~Mesh() {
		deleteMeshData();
	}

	bool loadFromFile(const std::string &filename);
	void setMaterial(ShaderProgram &program);

	void deleteMeshData() {
		attributeBuffer.deleteBuffer();
		indexBuffer.deleteBuffer();
		glDeleteVertexArrays(1, &vao);

		vao = 0;

		meshData.clear();
	}

	void render();

private:

	MeshData meshData;
	// GLuint indexBuffer;
	GLuint vao;

	Buffer attributeBuffer;
	Buffer indexBuffer;
};