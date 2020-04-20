#pragma once
#include "Includes.h"
#include "./component.h"
#include "MeshRenderer.h"
#include "MeshData.h"

/**
 * @brief mesh renderer component renders the given mesh attatched to the component
 * This is simply a wrapper around a MeshRenderer to integrate Mesh renderer with an entity using the component interface.
 * The bulk of mesh management and rendering is handeled by the mesh renderer. This class relies on the 
 * MeshRenderer class to keep track of all opengl buffers for rendering. This class is in charge of sending position data and other
 * transformation matrices to the shader.
 */
class MeshRendererComponent : public ComponentBase {
public:

    MeshRendererComponent(const shared_ptr<MeshData> &data):
        ComponentBase(),
        renderer(data)
    {}

    virtual ~MeshRendererComponent() = default;
    void render();
    
private:

    virtual void registerCallbacksToEntity(Entity *entity) override;
    
    MeshRenderer renderer;

};