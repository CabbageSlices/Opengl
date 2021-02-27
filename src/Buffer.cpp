#include "Buffer.h"

#include <iostream>

using std::cout;
using std::endl;

bool Buffer::create(const Buffer::BufferType &type, const void *pointerToData, GLsizeiptr bufferSize,
                    const Buffer::UsageType &usageType) {
    glGetError();
    if (buffer == 0) {
        glCreateBuffers(1, &buffer);
    }

    namedBufferData(buffer, bufferSize, pointerToData, usageType);
    bufferType = type;

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "ERROR CREAITING BUFF" << endl;
        cout << error << endl;
        return false;
    }

    return true;
}

bool Buffer::updateData(const void *pointerToData, GLsizeiptr dataSize, GLintptr offsetIntoBuffer) {
    glGetError();
    if (buffer == 0) {
        cout << "Error, no buffer created" << endl;
        return false;
    }

    if (dataSize == 0) {
        cout << "no data updated" << endl;
        return false;
    }

    glNamedBufferSubData(buffer, offsetIntoBuffer, dataSize, pointerToData);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "ERROR UPDATING BUFF" << endl;
        cout << error << endl;
        return false;
    }
    return true;
}

void Buffer::bindToTarget() { glBindBuffer(bufferType, buffer); }

void Buffer::unbindFromTarget() { glBindBuffer(bufferType, 0); }

void Buffer::bindToTargetBindingPoint(int bindingIndex) { bindBufferBase(bufferType, bindingIndex, buffer); }