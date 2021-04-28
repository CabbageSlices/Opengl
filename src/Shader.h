#ifndef _SHADER_HPP_INCLUDED
#define _SHADER_HPP_INCLUDED

#include <iostream>
#include <map>
#include <string>

#include "GraphicsWrapper.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

using std::cout;
using std::endl;
using std::map;
using std::string;

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
    enum Type : GLuint { Vertex = GL_VERTEX_SHADER, Fragment = GL_FRAGMENT_SHADER, Geometry = GL_GEOMETRY_SHADER };

    static const map<Type, string> SHADER_TYPE_TO_STRING;

    Shader() : shaderObject(0) {}

    virtual ~Shader() { deleteShaderObject(); }

    void deleteShaderObject();
    bool checkCompilationStatus();

    GLuint getShaderObject();

    // loads the given shader file, will automatically add the direcotry prefix to the filename so make sure you don't
    // include it. returns true on success, false on failure.
    bool loadAndCompileShader(const Type &type, const std::string &filename,
                              bool prependIncludeDirectoryAutomatically = true);

    // binding indices of vertex attribute
    // always the same for every shader so you don't have to query it everytime
    static const unsigned int POSITION_ATTRIBUTE_INDEX = 0;
    static const unsigned int NORMAL_ATTRIBUTE_INDEX = 1;
    static const unsigned int TEX_COORD_ATTRIBUTE_INDEX = 2;

    /**
     * @brief Set the shader include directory. the directory should be set as a relative path from
     * location where the program is executed. All shader filenamees will have the directory prefix added to them so no need
     * to add it yourself
     *
     * @param _includeDirectory
     */
    static void SetIncludeDirectory(const std::string &_includeDirectory);

    /**
     * @brief Set the Custom Preprocessor Directive Prefix, the symbols that will appear before custom preprocessor
     * directives Example, if you use #include, the prefex should be set to '#' without the quotation marks. Please include
     * the trailing /.   i.e: if your directory is 'shaders' then set the string to 'shaders/'
     *
     * @param _customPreprocessorDirectivePrefix
     */
    static void SetCustomPreprocessorDirectivePrefix(const std::string &_customPreprocessorDirectivePrefix);

    /**
     * @brief runs the shader preprocessor on the given shaderfile and executes the custom preprocessor directives
     *
     * @param shaderFile file containg the shader string, this is NOT the filename, but an actual shader file
     * @return a new shaderfilee with the custom preprocessor directives stripped out and replaced with the result
     */
    static std::string RunPreprocessor(const std::string &shaderFile);

  private:
    void runPreProcessor();
    void printCompilerError(const int &bufferSize = 256);

    GLuint shaderObject;

    static std::string includeDirectory;
    static std::string customPreprocessorDirectivePrefix;
};

#endif