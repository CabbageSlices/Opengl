#include <vector>
#include <iostream>
#include "./headers/Shader.h"
#include "./headers/loadFileToString.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

bool Shader::loadAndCompileShader(const Type &type, const std::string &filename) {

	if(shaderObject != 0) {
		cout << "Shader object already created, ERROR" << endl;
		return false;
	}

	shaderObject = glCreateShader((GLuint)type);

	string source = loadFileToString(filename);
	auto sourceCStr = source.c_str();

	glShaderSource(shaderObject, 1, &sourceCStr, NULL);
	glCompileShader(shaderObject);

	bool isCompilationSuccessful = checkCompilationStatus();

	if(isCompilationSuccessful) {
		return true;
	}

	cout << "Error compiling file: " << filename << endl;

	//failed to compile, print out error log to console.
	printCompilerError();

	return false;
}

void Shader::printCompilerError(const int &bufferSize) {

	vector<char> buffer(bufferSize, '\0');
	glGetShaderInfoLog(shaderObject, bufferSize, nullptr, buffer.data());

	cout << buffer.data() << endl;
}