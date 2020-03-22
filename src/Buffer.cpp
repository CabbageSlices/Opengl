#include "Buffer.h"
#include <iostream>

using std::cout;
using std::endl;

bool Buffer::create(const Buffer::BindingTarget &target, const void *pointerToData, GLsizeiptr bufferSize, const Buffer::UsageType &usageType) {

    if(buffer != 0) {
        cout << "Error, Buffer already stores data!" << endl;
        return false;
    }

    glCreateBuffers(1, &buffer);
    glNamedBufferData(buffer, bufferSize, pointerToData, usageType);
    bindingTarget = target;

    return true;
}

bool Buffer::updateData(const void *pointerToData, GLsizeiptr dataSize, GLintptr offsetIntoBuffer) {

    if(buffer == 0) {
        cout << "Error, no buffer created" << endl;
        return false;
    }

    glNamedBufferSubData(buffer, offsetIntoBuffer, dataSize, pointerToData);
    return true;
}

void Buffer::bindToTarget() {

    glBindBuffer(bindingTarget, buffer);
}

void Buffer::unbindFromTarget() {
    glBindBuffer(bindingTarget, 0);
}

void Buffer::bindToTargetBindingPoint(int bindingIndex) {
    glBindBufferBase(bindingTarget, bindingIndex, buffer);
}