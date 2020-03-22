#pragma once

#include <vector>
#include <string>
#include "MeshData.h"
#include "ShaderProgram.h"
#include "glad\glad.h"
#include "Buffer.h"
#include "VertexArrayObject.h"

class Mesh {

public:

	Mesh() : attributeBuffer(), indexBuffer(), vao() {};
	
	~Mesh() {
		deleteMeshData();
	}

	bool loadFromFile(const std::string &filename);

	void deleteMeshData() {
		attributeBuffer.deleteBuffer();
		indexBuffer.deleteBuffer();

		vao.deleteVao();

		meshData.clear();
	}

	void render();

private:

	bool initializeAttributeBuffers();
	bool initializeIndexBuffer();
	void initializeVertexArrayObject();

	MeshData meshData;
	VertexArrayObject vao;

	Buffer attributeBuffer;
	Buffer indexBuffer;
};