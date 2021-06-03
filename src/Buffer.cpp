#include "Buffer.h"

#include <iostream>

using std::cout;
using std::endl;

bool Buffer::create(const Buffer::BufferType &type, const void *pointerToData, GLsizeiptr _bufferSize,
                    const Buffer::UsageType &usageType) {
    if (buffer == 0) {
        glCreateBuffers(1, &buffer);
    }

    namedBufferData(buffer, _bufferSize, pointerToData, usageType);
    bufferType = type;
    bufferSize = _bufferSize;

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "ERROR CREATING BUFF: " << error << endl;
        cout << error << endl;
        return false;
    }

    return true;
}

bool Buffer::updateData(const void *pointerToData, GLsizeiptr dataSize, GLintptr offsetIntoBuffer) {
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

bool Buffer::read(void *destination, GLsizeiptr offset, GLsizeiptr amountToRead) const {
    if (buffer == 0) {
        cout << "Error, no buffer created" << endl;
        return false;
    }

    if (bufferSize == 0) {
        cout << "buffer has no size" << endl;
        return false;
    }

    if (amountToRead <= 0 || offset < 0 || offset + amountToRead > bufferSize) {
        cout << "invalid data range when reading from buffer" << endl;
        return false;
    }
    glGetNamedBufferSubData(buffer, offset, amountToRead, destination);

    return true;
}

bool Buffer::read(void *destination, GLsizeiptr offset) const {
    GLsizeiptr amountToRead = bufferSize - offset;
    return read(destination, offset, amountToRead);
}