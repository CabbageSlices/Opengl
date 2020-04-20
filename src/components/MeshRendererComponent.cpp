#include "./MeshRendererComponent.h"
#include "Entity/Entity.h"

//TODO implement rest of meshrenderer component
void MeshRendererComponent::registerCallbacksToEntity(Entity *entity) {
    entity->registerRenderCallback(
        this,
        [&](){
            render();
        }
    );
}

void MeshRendererComponent::render() {
    
    renderer.render();
}