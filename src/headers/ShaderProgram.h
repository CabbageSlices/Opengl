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

private:

	GLuint shaderProgram;
	map<Shader::Type, shared_ptr<Shader> > shaders;
};