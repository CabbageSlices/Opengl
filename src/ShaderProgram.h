#pragma once

#include <map>
#include <memory>
#include <string>

#include "./Shader.h"
#include "glad/glad.h"

using std::map;
using std::pair;
using std::shared_ptr;

// TODO when resourse manager is made, move shader array out of shader program and into resource manager
class ShaderProgram {
  public:
    ShaderProgram() : shaderProgram(0), shaders() {}
    ShaderProgram(const map<Shader::Type, std::string> &shaders);
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

    bool loadAndCompileShaders(const map<Shader::Type, std::string> &shaders);

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

  private:
    GLuint shaderProgram;
    map<Shader::Type, shared_ptr<Shader> > shaders;
};