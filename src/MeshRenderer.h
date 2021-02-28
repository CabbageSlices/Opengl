#pragma once

#include "Buffer.h"
#include "GraphicsWrapper.h"
#include "Includes.h"
#include "MeshData.h"
#include "ShaderProgram/shaderprogram.h"
#include "VertexArrayObject.h"

/**
 * @brief Class that is used to render a mesh asset. This is a standalone class that can be used to render a
 * given mesh asset. The MeshRendererComponent uses this under-the-hood to render a mesh. This class keeps track
 * of all opengl buffers related to rendering the mesh. This class has no concept of positioning and so does
 * not send any data to the opengl shaders related to the position/projection matrices.
 *
 */
class MeshRenderer {
  public:
    MeshRenderer() : meshData(), vao(), attributeBuffer(), indexBuffer(){};
    MeshRenderer(const shared_ptr<MeshData> &_meshData);

    ~MeshRenderer() { deleteMeshData(); }

    void deleteMeshData() {
        attributeBuffer.deleteBuffer();
        indexBuffer.deleteBuffer();

        vao.deleteVao();

        // don't clear mesh data manually because others might be using the mesh data pointer
        // meshData->clear();
    }

    void render();

  private:
    bool initializeAttributeBuffers();
    bool initializeIndexBuffer();
    void initializeVertexArrayObject();

    shared_ptr<MeshData> meshData;
    VertexArrayObject vao;

    Buffer attributeBuffer;
    Buffer indexBuffer;
};