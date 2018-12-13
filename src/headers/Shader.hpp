#ifndef _SHADER_HPP_INCLUDED
#define _SHADER_HPP_INCLUDED

#include "glad/glad.h"
#include "string"

/**
 * @brief      Class to store opengl shader objects.
 * Loads, compiles, and stores shader objects. 
 * Automatically deletes the shader object on deletion
 * 
 */
class Shader {

public:
	/**
	 * @brief   The different possible shader types supported by opengl
	 */
	enum Type : GLuint {
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER
	};

	Shader() : shaderObject(0) {}

	virtual ~Shader() {
		deleteShaderObject();
	}

	void deleteShaderObject() {
		glDeleteShader(shaderObject);
	}
  
  /**
   * @brief      checks if the stored shader object compiled successfully
   *
   * @return     returns true if compilation was successful, false otherwise
   */
	bool checkCompilationStatus() {

		GLint data;
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &data);
		return data == 1;
	}

	bool loadAndCompileShader(const std::string &filename, const Type &type);

private:

	void printCompilerError(const int &bufferSize = 256);

	GLuint shaderObject;
};

#endif