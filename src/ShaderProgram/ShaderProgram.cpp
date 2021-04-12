#include "./ShaderProgram.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;

    for (auto &shader : shaders) {
        shader.second->deleteShaderObject();
    }
}

bool ShaderProgram::loadAndCompileShaders(const map<Shader::Type, std::string> &shaderFiles,
                                          bool prependIncludeDirectoryAutomatically) {
    for (auto &shaderData : shaderFiles) {
        // cout << shaderData.second << endl;
        shared_ptr<Shader> shader = std::make_shared<Shader>();

        if (!shader->loadAndCompileShader(shaderData.first, shaderData.second, prependIncludeDirectoryAutomatically)) {
            // failed to compile ignore rest of the shaders
            return false;
        }

        // compiled successfully, add to programs
        shaders.insert({shaderData.first, shader});
    }

    return true;
}

bool ShaderProgram::linkProgram() {
    if (shaderProgram != 0) {
        cout << "program already created. ERROR" << endl;
        return false;
    }

    shaderProgram = glCreateProgram();

    for (auto &shaderData : shaders) {
        shared_ptr<Shader> shader = shaderData.second;
        glAttachShader(shaderProgram, shader->getShaderObject());
    }

    glLinkProgram(shaderProgram);

    if (!checkLinkingStatus()) {
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

map<string, GLint> ShaderProgram::getUniformOffsetForAttributes(const vector<string> &attributeNames) {
    map<string, GLint> results;

    if (shaderProgram == 0) {
        return results;
    }

    vector<char *> names;
    for (auto &name : attributeNames) {
        names.push_back(const_cast<char *>(name.c_str()));
    }

    GLuint indexResults[(int)names.size()] = {0};

    vector<GLuint> loadedIndices;
    vector<string> namesOfAttributeWithIndices;

    glGetUniformIndices(shaderProgram, names.size(), names.data(), indexResults);

    for (unsigned i = 0; i < names.size(); ++i) {
        string name = names[i];
        GLuint index = indexResults[i];

        if (index == GL_INVALID_INDEX) {
            continue;
        }

        namesOfAttributeWithIndices.push_back(name);
        loadedIndices.push_back(index);
    }

    // loaded indices, now load the offsets
    GLint offsets[(int)loadedIndices.size()] = {0};

    glGetActiveUniformsiv(shaderProgram, loadedIndices.size(), loadedIndices.data(), GL_UNIFORM_OFFSET, offsets);

    for (unsigned i = 0; i < loadedIndices.size(); ++i) {
        GLint offset = offsets[i];
        string name = namesOfAttributeWithIndices[i];

        if (offset < 0) {
            continue;
        }

        results[name] = offset;
    }

    return results;
}

map<GLuint, GLint> ShaderProgram::getUniformOffsetForAttributes(const vector<GLuint> &attributeUniformIndices) {
    map<GLuint, GLint> results;

    if (shaderProgram == 0) {
        return results;
    }

    // loaded indices, now load the offsets
    GLint offsets[(int)attributeUniformIndices.size()] = {0};

    glGetActiveUniformsiv(shaderProgram, attributeUniformIndices.size(), attributeUniformIndices.data(), GL_UNIFORM_OFFSET,
                          offsets);

    for (unsigned i = 0; i < attributeUniformIndices.size(); ++i) {
        GLint offset = offsets[i];
        GLuint index = attributeUniformIndices[i];

        if (offset < 0) {
            continue;
        }

        results[index] = offset;
    }

    return results;
}

GLuint ShaderProgram::getUniformBlockIndex(const string &uniformBlockName) {
    if (shaderProgram == 0) {
        return GL_INVALID_INDEX;
    }

    char *name = const_cast<char *>(uniformBlockName.c_str());

    GLuint result = glGetUniformBlockIndex(shaderProgram, name);

    return result;
}

GLint ShaderProgram::getUniformBlockProperty(GLuint uniformBlockIndex, UniformBlockProperty property) {
    if (shaderProgram == 0) {
        return GL_INVALID_VALUE;
    }

    GLint value = 0;
    glGetActiveUniformBlockiv(shaderProgram, uniformBlockIndex, (GLenum)property, &value);
    return value;
}

void ShaderProgram::queryUniformAttributeDataFromIndex(const std::vector<GLuint> &uniformIndices,
                                                       std::map<GLint, string> &uniformIndexToAttributeName,
                                                       std::map<GLint, GLenum> &uniformIndexToType,
                                                       std::map<GLint, GLint> &uniformIndexToNumberOfElementsIfArray) {
    if (shaderProgram == 0) {
        throw PROGRAM_DOES_NOT_EXIST;
    }

    for (auto &index : uniformIndices) {
        GLsizei bufferLength = 500;
        GLchar nameBuffer[bufferLength] = {0};
        GLint lengthIfArray = 0;
        GLsizei bytesWrittenIntoBuffer = 0;
        GLenum type = 0;

        glGetActiveUniform(shaderProgram, index, bufferLength, &bytesWrittenIntoBuffer, &lengthIfArray, &type, nameBuffer);

        string name(nameBuffer);

        // no data laoded, something is invalid
        if (bytesWrittenIntoBuffer == 0) {
            continue;
        }
        uniformIndexToAttributeName[index] = name;
        uniformIndexToType[index] = type;
        uniformIndexToNumberOfElementsIfArray[index] = lengthIfArray;
    }
}

vector<GLuint> ShaderProgram::getUniformBlockActiveUniformIndices(GLuint uniformBlockIndex) {
    if (shaderProgram == 0) {
        throw PROGRAM_DOES_NOT_EXIST;
    }

    GLint numberOfIndices =
        getUniformBlockProperty(uniformBlockIndex, UniformBlockProperty::UniformBlockNumberOfActiveUniforms);
    vector<GLuint> indices(numberOfIndices);

    glGetActiveUniformBlockiv(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                              (GLint *)indices.data());
    return indices;
}

void ShaderProgram::getUniform(GLint location, GLint *output) { glGetUniformiv(shaderProgram, location, output); }

void ShaderProgram::getUniform(GLint location, GLuint *output) { glGetUniformuiv(shaderProgram, location, output); }

void ShaderProgram::getUniform(GLint location, GLfloat *output) { glGetUniformfv(shaderProgram, location, output); }
void ShaderProgram::getUniform(GLint location, glm::vec4 *output) {
    glGetUniformfv(shaderProgram, location, (GLfloat *)output);
}

void ShaderProgram::getUniform(GLint location, glm::vec3 *output) {
    glGetUniformfv(shaderProgram, location, (GLfloat *)output);
}

void ShaderProgram::getUniform(GLint location, glm::vec2 *output) {
    glGetUniformfv(shaderProgram, location, (GLfloat *)output);
}