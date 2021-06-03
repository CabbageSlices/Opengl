#include "./GraphicsWrapper.h"

map<RenderingPass, string> renderPassNames{{RenderingPass::DEPTH_PASS, "DEPTH_PASS"},
                                           {RenderingPass::REGULAR_PASS, "REGULAR_PASS"}};

function<void(GLenum, GLuint, GLuint)> mockglBindBufferBase;
function<void(GLuint, GLuint)> mockglBindTextureUnit;
function<void(GLuint, GLsizeiptr, const GLvoid*, GLenum)> mockglNamedBufferData;

RenderingPass currentRenderingPass = RenderingPass::REGULAR_PASS;
bool activateMaterials = true;

void clearAllMockFunctions() {
    mockglBindBufferBase = nullptr;
    mockglBindTextureUnit = nullptr;
    mockglNamedBufferData = nullptr;
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

void namedBufferData(GLuint buffer, GLsizeiptr size, const GLvoid* data, GLenum usage) {
    if (mockglNamedBufferData) {
        mockglNamedBufferData(buffer, size, data, usage);
        return;
    }

    glNamedBufferData(buffer, size, data, usage);
}