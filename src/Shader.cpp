#include "./Shader.h"

#include <iostream>
#include <vector>

#include "./StringMethods.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

const map<Shader::Type, string> Shader::SHADER_TYPE_TO_STRING{
    {Shader::Vertex, "vertex"}, {Shader::Fragment, "fragment"}, {Shader::Geometry, "geomtery"}};
string Shader::includeDirectory = "shaders/";
string Shader::customPreprocessorDirectivePrefix = "#";

void Shader::SetIncludeDirectory(const string &_includeDirectory) { includeDirectory = _includeDirectory; }

void Shader::SetCustomPreprocessorDirectivePrefix(const string &_customPreprocessorDirectivePrefix) {
    customPreprocessorDirectivePrefix = _customPreprocessorDirectivePrefix;
}

string Shader::RunPreprocessor(const string &shaderFile) {
    int includeStatementLength = customPreprocessorDirectivePrefix.length() + string("include ").length();

    string processedShader = shaderFile;

    // handle include statements
    size_t includePos = processedShader.find(customPreprocessorDirectivePrefix + "include ");

    while (includePos != string::npos) {
        string includeLine = extractLine(processedShader, includePos);
        string fileName = extractStringFromLine(includeLine, includeStatementLength);
        string file = loadFileToString(includeDirectory + fileName);
        processedShader.replace(includePos, includeLine.length(), file);

        includePos = processedShader.find(customPreprocessorDirectivePrefix + "include ");
    }

    return processedShader;
}

bool Shader::checkCompilationStatus() {
    GLint data;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &data);
    return data == 1;
}

void Shader::deleteShaderObject() {
    glDeleteShader(shaderObject);
    shaderObject = 0;
}

GLuint Shader::getShaderObject() { return shaderObject; }

bool Shader::loadAndCompileShader(const Type &type, const std::string &filename, bool prependIncludeDirectoryAutomatically) {
    if (shaderObject != 0) {
        cout << "Shader object already created, ERROR" << endl;
        return false;
    }

    shaderObject = glCreateShader((GLuint)type);

    string source = loadFileToString(prependIncludeDirectoryAutomatically ? includeDirectory + filename : filename);
    source = RunPreprocessor(source);

    auto sourceCStr = source.c_str();
    glShaderSource(shaderObject, 1, &sourceCStr, NULL);
    glCompileShader(shaderObject);

    bool isCompilationSuccessful = checkCompilationStatus();

    if (isCompilationSuccessful) {
        return true;
    }

    cout << "Error compiling file: " << filename << endl;

    // failed to compile, print out error log to console.
    printCompilerError();

    return false;
}

void Shader::printCompilerError(const int &bufferSize) {
    vector<char> buffer(bufferSize, '\0');
    glGetShaderInfoLog(shaderObject, bufferSize, nullptr, buffer.data());

    cout << buffer.data() << endl;
}