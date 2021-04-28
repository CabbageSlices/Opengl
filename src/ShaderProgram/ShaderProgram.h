#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "./Shader.h"
#include "GraphicsWrapper.h"
#include "error_strings.h"

using std::map;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

enum UniformBlockProperty : GLenum {
    UniformBlockBindingIndex = GL_UNIFORM_BLOCK_BINDING,
    UniformBlockSizeInBytes = GL_UNIFORM_BLOCK_DATA_SIZE,
    UniformBlockNumberOfActiveUniforms = GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
};

// TODO when resourse manager is made, move shader array out of shader program and into resource manager
class ShaderProgram {
  public:
    ShaderProgram() : shaderProgram(0), shaders() {}
    ShaderProgram(const map<Shader::Type, std::string> &shaders, bool prependIncludeDirectoryAutomatically = true);
    ShaderProgram(const ShaderProgram &rhs) = delete;
    ShaderProgram &operator=(const ShaderProgram &rhs) = delete;
    virtual ~ShaderProgram();

    /**
     * @brief loads the given list of files and treats the data as the given type of shader.
     * each string should be a FILENAME with a relative path to the file to load.
     * Each shader source file is treated as a separate whole file, meaning if you provide multiple files
     * for a single shader type, they will be linked together instead of concatenating the files together
     *
     */

    bool loadAndCompileShaders(const map<Shader::Type, std::string> &shaders,
                               bool prependIncludeDirectoryAutomatically = true);

    /**
     * @brief Load the given list of files, appends them one after another into a single file, and compile it as the
     * given type of shader.
     *
     * If a shader of the given type has already been added shader list then this new shader will be linked with the old one
     * IT WILL NOT REPLACE SHADERS OF THE SAME TYPE
     *
     * @param type type of shader to compile as
     * @param sourceFileNames list of the names of each file to use as part of the shader source
     * @return true files were loaded and shader compiled successfully
     * @return false either files failed to load or shader failed to compile
     */
    bool loadAndCompileShader(const Shader::Type &type, const std::initializer_list<std::string> &sourceFileNames);

    /**
     * @brief      Links all of the added compiled shaders into a shader program
     *						shaders must be loaded and compiled before calling this function.
     *						Prints error to console if linking fails.
     * If a linked program already exists IT WILL BE DELETED AND REPLACED.
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

    void useProgram() { glUseProgram(shaderProgram); }

    void setUniform(const GLint &location, const bool &transpose, const glm::mat4 &mat) {
        glProgramUniformMatrix4fv(shaderProgram, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(mat));
    }

    void setUniform(const GLint &location, const glm::vec4 &vec) {
        glProgramUniform4fv(shaderProgram, location, 1, glm::value_ptr(vec));
    }

    void setUniform(const GLint &location, const glm::vec3 &vec) {
        glProgramUniform3fv(shaderProgram, location, 1, glm::value_ptr(vec));
    }

    GLint getUniformLocation(const std::string &uniformName) {
        if (shaderProgram == 0) {
            return -1;
        }

        return glGetUniformLocation(shaderProgram, uniformName.c_str());
    }

    /**
     * @brief Get the value of hte uniform with the given name in the program. Throws an exception if there is an error
     * The type of the uniform is determined by the template parameter, calls the appropriate GL getter according tog the
     * type
     * @tparam T
     * @param uniformName
     * @return T
     */
    template <typename T>
    T getUniform(const std::string &uniformName) {
        if (shaderProgram == 0) {
            throw PROGRAM_DOES_NOT_EXIST;
        }
        GLint location = getUniformLocation(uniformName);

        if (location == -1) {
            throw NOT_FOUND_ERROR;
        }

        T output;
        getUniform(location, &output);

        return output;
    }

    /**
     * @brief for each of the given attribute names, get the offset to that attribute within its uniform block
     * if there is an error, it will return an empty map. so be sure to check. If the given attribute doesn't have an offset,
     * it will not be included in the returned map
     *
     * @param attributeNames
     * @return std::map<std::string, GLint>
     */
    std::map<std::string, GLint> getUniformOffsetForAttributes(const vector<string> &attributeNames);

    /**
     * @brief for each of the given attribute uniform index, get the offset to that attribute within its uniform block
     * if there is an error, it will return an empty map. so be sure to check. If the given attribute doesn't have an offset,
     * it will not be included in the returned map
     *
     * @param attributeUniformIndices
     * @return std::map<std::string, GLint>
     */
    std::map<GLuint, GLint> getUniformOffsetForAttributes(const vector<GLuint> &attributeUniformIndices);

    /**
     * @brief get the index of the uniform lbock with the given name. this is NOT the binding index, the block index is a
     * unique identifier f or the block within the shader. returns GL_INVALID_INDEX if there is an error
     *
     * @param uniformBlockName
     * @return int
     */
    GLuint getUniformBlockIndex(const string &uniformBlockName);

    /**
     * @brief Get the specified property for the uniform block with the given index. Returns the property value, or
     * GL_INVALID_VALUE on failure.
     *
     * @param uniformBlockIndex
     * @param property
     * @return GLint
     */
    GLint getUniformBlockProperty(GLuint uniformBlockIndex, UniformBlockProperty property);

    // query the name, type, and nuumber of array elements (will be 1 for on array) of each uniform at the specified indices
    void queryUniformAttributeDataFromIndex(const std::vector<GLuint> &uniformIndices,
                                            std::map<GLint, string> &uniformIndexToAttributeName,
                                            std::map<GLint, GLenum> &uniformIndexToType,
                                            std::map<GLint, GLint> &uniformIndexToNumberOfElementsIfArray);

    /**
     * @brief Returns an array containing the uniform indices of each uniform variable in the given uniform block.
     *
     * @param uniformBlockIndex
     * @return vector<GLint>
     */
    vector<GLuint> getUniformBlockActiveUniformIndices(GLuint uniformBlockIndex);

    GLuint getShaderProgram() { return shaderProgram; }

  private:
    void getUniform(GLint location, GLint *output);
    void getUniform(GLint location, GLuint *output);
    void getUniform(GLint location, GLfloat *output);
    void getUniform(GLint location, glm::vec4 *output);
    void getUniform(GLint location, glm::vec3 *output);
    void getUniform(GLint location, glm::vec2 *output);

    GLuint shaderProgram;
    map<Shader::Type, shared_ptr<Shader> > shaders;
};