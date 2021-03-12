#include "./GraphicsWrapper.h"

function<void(GLenum, GLuint, GLuint)> mockglBindBufferBase;
function<void(GLuint, GLuint)> mockglBindTextureUnit;
function<void(GLenum, GLsizeiptr, const GLvoid*, GLenum)> mockNamedBufferData;

RenderingPass currentRenderingPass = RenderingPass::REGULAR_PASS;
bool activateMaterials = true;

void clearAllMockFunctions() {
    mockglBindBufferBase = nullptr;
    mockglBindTextureUnit = nullptr;
    mockNamedBufferData = nullptr;
}

void bindBufferBase(GLenum target, GLuint index, GLuint buffer) {
    if (mockglBindBufferBase) {
        mockglBindBufferBase(target, index, buffer);
        return;
    }
    glBindBufferBase(target, index, buffer);
}

void bindTextureUnit(GLuint textureUnit, GLuint textureObject) {
    if (mockglBindTextureUnit) {
        mockglBindTextureUnit(textureUnit, textureObject);
        return;
    }

    glBindTextureUnit(textureUnit, textureObject);
}

void namedBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
    if (mockNamedBufferData) {
        mockNamedBufferData(target, size, data, usage);
        return;
    }

    glNamedBufferData(target, size, data, usage);
}