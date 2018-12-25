#pragma once

#include <map>
#include <string>
#include <memory>
#include "./Shader.h"
#include "glad/glad.h"

using std::pair;
using std::map;
using std::shared_ptr;

class ShaderProgram {

public:

	ShaderProgram() : shaderProgram(0), shaders() {}
	ShaderProgram(const map<Shader::Type, std::string> &shaders);
	virtual ~ShaderProgram();

	bool loadAndCompileShaders(const map<Shader::Type, std::string> &shaders);

	/**
	 * @brief      Links all of the added compiled shaders into a shader program
	 *						shaders must be loaded and compiled before calling this function.
	 *						Prints error to console if linking fails.
	 * @return     true if link successfully, false otherwise. 
	 */
	bool linkProgram();

	/**
	 * @brief      Checks if the program linked successfully
	 *
	 * @return     true if linking successful, false otherwise
	 */
	bool checkLinkingStatus();
	void printLinkerError(int bufferSize = 256);

	void useProgram() {
		glUseProgram(shaderProgram);
	}

	void setUniform(const GLint &location, const bool &transpose, const glm::mat4 &mat) {
		glProgramUniformMatrix4fv(shaderProgram, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(mat));
	}

	void setUniform(const GLint &location, const glm::vec4 &vec) {
		glProgramUniform4fv(shaderProgram, location, 1, glm::value_ptr(vec));
	}

	static const unsigned int WORLD_TO_CLIP_UNIFORM_LOCATION = 0;
	static const unsigned int WORLD_TO_CAMERA_UNIFORM_LOCATION = 1;

private:

	GLuint shaderProgram;
	map<Shader::Type, shared_ptr<Shader> > shaders;
};