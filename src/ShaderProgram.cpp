#include <vector>
#include <iostream>
#include "./ShaderProgram.h"

using std::cout;
using std::endl;
using std::vector;

ShaderProgram::~ShaderProgram() {

	glDeleteProgram(shaderProgram);
	shaderProgram = 0;

	for(auto &shader : shaders) {
		shader.second->deleteShaderObject();
	}
}

bool ShaderProgram::loadAndCompileShaders(const map<Shader::Type, std::string> &shaderFiles) {

	for(auto &shaderData : shaderFiles) {

		// cout << shaderData.second << endl;
		shared_ptr<Shader> shader = std::make_shared<Shader>();

		if(!shader->loadAndCompileShader(shaderData.first, shaderData.second)) {
			
			//failed to compile ignore rest of the shaders
			return false;
		}

		//compiled successfully, add to programs
		shaders.insert( {shaderData.first, shader} );
	}

	return true;
}

bool ShaderProgram::linkProgram() {

	if(shaderProgram != 0) {
		cout << "program already created. ERROR" << endl;
		return false;
	}

	shaderProgram = glCreateProgram();

	for(auto &shaderData : shaders) {

		shared_ptr<Shader> shader = shaderData.second;
		glAttachShader(shaderProgram, shader->getShaderObject());
	}

	glLinkProgram(shaderProgram);

	if(!checkLinkingStatus()) {
		printLinkerError();
		return false;
	}

	return true;
}

bool ShaderProgram::checkLinkingStatus() {

	GLint linkStatus = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

	return linkStatus == 1;
}

void ShaderProgram::printLinkerError(int bufferSize) {

	vector<GLchar> buffer(bufferSize, '\0');
	glGetProgramInfoLog(shaderProgram, bufferSize, nullptr, buffer.data());

	cout << buffer.data() << endl;
}