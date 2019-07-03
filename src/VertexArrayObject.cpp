#include "headers/VertexArrayObject.h"
#include "headers/Buffer.h"

VertexArrayObject::VertexArrayObject() :
    vao(0)
    {

    }

void VertexArrayObject::vertexAttributeShaderLocationToBufferIndex(GLuint shaderLocation, GLuint bufferBindingIndex, bool enableAttribute) {

    if(vao == 0) {
        init();
    }

    glVertexArrayAttribBinding(vao, shaderLocation, bufferBindingIndex);

    if(enableAttribute)
        enableVertexAttribute(shaderLocation);
}

void VertexArrayObject::attachElementBuffer( const Buffer &elementArrayBuffer) {

    if(vao == 0) {
        init();
    }

    glVertexArrayElementBuffer(vao, elementArrayBuffer.getBufferObject());
}

void VertexArrayObject::bindVertexAttributeBuffer(GLuint bufferBindingIndex, const Buffer &buffer, GLintptr offset, GLsizei stride) {

    if(vao == 0) {
        init();
    }

    glVertexArrayVertexBuffer(vao, bufferBindingIndex, buffer.getBufferObject(), offset, stride);
}

void VertexArrayObject::formatVertexAttributeData(GLuint shaderAttributeLocation, GLuint numElementsPerEntry, GLenum dataType, GLuint offset, bool normalize) {
    
    if(vao == 0) {
        init();
    }

    glVertexArrayAttribFormat(vao, shaderAttributeLocation, numElementsPerEntry, dataType, normalize, offset);
}

void VertexArrayObject::enableVertexAttribute(GLuint shaderAttributeLocation) {
    glEnableVertexArrayAttrib(vao, shaderAttributeLocation);
}

void VertexArrayObject::bindToContext() {
    glBindVertexArray(vao);
}

void VertexArrayObject::unbindFromContext() {
    glBindVertexArray(0);
}

void VertexArrayObject::init() {

    if(vao != 0)
        return;

    glCreateVertexArrays(1, &vao);
}