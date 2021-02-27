#pragma once

#include <functional>

#include "Test/OpenGLTestContext.h"
#include "glad/glad.h"

using std::function;

// all of these mocks must be cleared manually after using
extern function<void(GLenum, GLuint, GLuint)> mockglBindBufferBase;
extern function<void(GLuint, GLuint)> mockglBindTextureUnit;
extern function<void(GLenum, GLsizeiptr, const GLvoid*, GLenum)> mockNamedBufferData;

void clearAllMockFunctions();

// void createBuffers(GLsizei n, GLuint *buffers);
// void namedBufferData(GLuint buffer, GLsizei size, const void *data, GLenum usage);
// void namedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, const void *data);
// void bindBuffer(GLenum target, GLuint buffer);
void bindBufferBase(GLenum target, GLuint index, GLuint buffer);
void bindTextureUnit(GLuint textureUnit, GLuint textureObject);
void namedBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
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
