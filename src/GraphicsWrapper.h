#pragma once

#include <functional>
#include <map>
#include <string>

#include "glad/glad.h"

using std::function;
using std::map;
using std::string;

enum class RenderingPass {
    DEPTH_PASS,
    REGULAR_PASS,
};

extern map<RenderingPass, string> renderPassNames;
extern RenderingPass currentRenderingPass;

extern bool activateMaterials;

// all of these mocks must be cleared manually after using
extern function<void(GLenum, GLuint, GLuint)> mockglBindBufferBase;
extern function<void(GLuint, GLuint)> mockglBindTextureUnit;
extern function<void(GLuint, GLsizeiptr, const GLvoid*, GLenum)> mockglNamedBufferData;

void clearAllMockFunctions();

// void createBuffers(GLsizei n, GLuint *buffers);
// void namedBufferData(GLuint buffer, GLsizei size, const void *data, GLenum usage);
// void namedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, const void *data);
// void bindBuffer(GLenum target, GLuint buffer);
void bindBufferBase(GLenum target, GLuint index, GLuint buffer);
void bindTextureUnit(GLuint textureUnit, GLuint textureObject);
void namedBufferData(GLuint target, GLsizeiptr size, const GLvoid* data, GLenum usage);
// void getShaderiv
// void deleteShader
// shaderSource
// compileShader
// getShaderInfoLog
// deleteProgram
// createProgram
// attachShader
// linkProgram
// getProgramiv
// getProgramInfoLog
// getUniformIndices
// getUniformBlockIndex
// getActiveUniformBlockiv
// useProgram
// programUniformMatrix4fv
// programUniform4fv

// define a bunch of open gl functions
