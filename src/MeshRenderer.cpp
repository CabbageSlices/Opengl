#include "./MeshRenderer.h"

#include <iostream>

#include "Enums.h"
#include "MeshData.h"
#include "Shader.h"
#include "attributeIndices.vert"
#include "loadFromObj.h"

using std::cout;
using std::endl;

MeshRenderer::MeshRenderer(const shared_ptr<MeshData> &_meshData)
    : meshData(_meshData),
      vao(),
      attributeBuffer(),
      indexBuffer() {
    initializeAttributeBuffers();
    initializeIndexBuffer();
    initializeVertexArrayObject();
}

bool MeshRenderer::loadFromFile(const std::string &filename) {
    if (vao.getVao() != 0 || attributeBuffer.isUsed()) {
        cout << "Already storing a previous mesh, please delete the mesh before loading" << endl;
        return false;
    }

    // load data
    if (!(meshData = loadFromObj(filename))) return false;

    initializeAttributeBuffers();
    initializeIndexBuffer();
    initializeVertexArrayObject();

    return true;
}

void MeshRenderer::render() {
    vao.bindToContext();

    // render faceset of each material
    unsigned int bytesAlreadyRead = 0 * sizeof(unsigned int);

    for (unsigned i = 0; i < meshData->materials.size(); ++i) {
        int materialId = meshData->materials[i]->getId();
        auto mat = meshData->materials[i];

        FaceSet faceset = meshData->materialFaceMap[materialId];

        unsigned numIndicesPerFace = 3;
        unsigned numIndicesCurrentFaceset = faceset.size() * numIndicesPerFace;

        mat->activate();
        glDrawElements(GL_TRIANGLES, numIndicesCurrentFaceset, GL_UNSIGNED_INT,
                       (void *)bytesAlreadyRead);  // wtf? convert integer directly to pointer and not the address

        bytesAlreadyRead += sizeof(Face) * faceset.size();
    }

    vao.unbindFromContext();
}

bool MeshRenderer::initializeAttributeBuffers() {
    unsigned positionArraySize = sizeof(glm::vec3) * meshData->attributes.positions.size();
    unsigned normalArraySize = sizeof(glm::vec3) * meshData->attributes.normals.size();
    unsigned texCoordArraySize = sizeof(glm::vec2) * meshData->attributes.texCoords.size();

    unsigned totalAttributeBufferSize = positionArraySize + normalArraySize + texCoordArraySize;

    attributeBuffer.create(Buffer::ArrayBuffer, nullptr, totalAttributeBufferSize, Buffer::StaticDraw);

    attributeBuffer.updateData(meshData->attributes.positions.data(), positionArraySize, 0);
    attributeBuffer.updateData(meshData->attributes.normals.data(), normalArraySize, positionArraySize);
    attributeBuffer.updateData(meshData->attributes.texCoords.data(), texCoordArraySize,
                               positionArraySize + normalArraySize);

    return true;
}

bool MeshRenderer::initializeIndexBuffer() {
    unsigned indicesArraySize = sizeof(unsigned int) * meshData->numIndices;

    indexBuffer.create(Buffer::ElementArrayBuffer, 0, indicesArraySize, Buffer::StaticDraw);

    // set of indices for every material should be added to the buffer
    // need to keep track of how much data has already been added to the buffer due to previous materials
    // in bytes
    int dataOffset = 0;

    // add each faceset for each material to the index buffer
    for (unsigned i = 0; i < meshData->materials.size(); ++i) {
        // current materials indices
        int materialId = meshData->materials[i]->getId();
        FaceSet faceset = meshData->materialFaceMap[materialId];

        indexBuffer.updateData(faceset.data(), sizeof(Face) * faceset.size(), dataOffset);

        dataOffset += faceset.size() * sizeof(Face);
    }

    return true;
}

void MeshRenderer::initializeVertexArrayObject() {
    // buffer binding index that correspond to the different vertex attributes
    // even though all attributes are in the same buffer, you can still bind different sections of the buffer to different
    // binding points this allows texcoords to be stored in the same buffer even though it is jujst a vec2 instead of a vec3
    // like position and normal
    const unsigned POSITION_BINDING_INDEX = 0;
    const unsigned NORMAL_BINDING_INDEX = 1;
    const unsigned TEXCOORD_BUFFER_BINDING_INDEX = 2;

    // all attribute data is in same buffer, but bind diffrent parts of the buffer to different binding indices
    // that way if different attributes have different number of elements you'll still be able to send the attirbute
    // correctly
    vao.vertexAttributeShaderLocationToBufferIndex(POSITION_ATTRIBUTE_LOCATION, POSITION_BINDING_INDEX);
    vao.vertexAttributeShaderLocationToBufferIndex(NORMAL_ATTRIBUTE_LOCATION, NORMAL_BINDING_INDEX);

    unsigned int positionBufferSize = sizeof(glm::vec3) * meshData->attributes.positions.size();

    vao.attachElementBuffer(indexBuffer);
    vao.bindVertexAttributeBuffer(POSITION_BINDING_INDEX, attributeBuffer, 0, sizeof(glm::vec3));
    vao.bindVertexAttributeBuffer(NORMAL_BINDING_INDEX, attributeBuffer, positionBufferSize, sizeof(glm::vec3));

    // offset into buffer for data is 0 because the buffer was bound starting from the first data element
    // that is, for vertex normals the buffer is bound at position POSITION_BUFFER_SIZE, that way the offset is already
    // incorporated into the buffer itself so no need to pass offset for this
    vao.formatVertexAttributeData(POSITION_ATTRIBUTE_LOCATION, 3, DataType::FLOAT, 0, GL_FALSE);
    vao.formatVertexAttributeData(NORMAL_ATTRIBUTE_LOCATION, 3, DataType::FLOAT, 0, GL_FALSE);

    // no tex coords to send
    if (meshData->attributes.texCoords.size() == 0) return;

    unsigned int normalBufferSize = sizeof(glm::vec3) * meshData->attributes.normals.size();

    // bind same buffer for texcoord since the data is in the same buffer, but offset into the buffer to the position
    // that tex coord starts
    // need to do this binding because texcoords are only vec2 whereas position and normal are vec3, so stride is werong
    vao.bindVertexAttributeBuffer(TEXCOORD_BUFFER_BINDING_INDEX, attributeBuffer, positionBufferSize + normalBufferSize,
                                  sizeof(glm::vec2));

    vao.vertexAttributeShaderLocationToBufferIndex(TEXCOORD_ATTRIBUTE_LOCATION, TEXCOORD_BUFFER_BINDING_INDEX);
    vao.formatVertexAttributeData(TEXCOORD_ATTRIBUTE_LOCATION, 2, DataType::FLOAT, 0);
}