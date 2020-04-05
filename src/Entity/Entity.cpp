#include "./Entity.h"
#include "./Component.h"

bool Entity::addComponent(const std::shared_ptr<ComponentBase> &component) {
    //don't add the same component twice
    if(std::find(components.begin(), components.end(), component) != components.end())
        return false;
    
    component->registerEntity(this);
    components.push_back(component);

    return true;
}

void Entity::removeComponent(const std::shared_ptr<ComponentBase> &component) {
    renderCallbacks.removeCallback(component);
    updateCallbacks.removeCallback(component);
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
}

void Entity::registerRenderCallback(const CallbackIdentifier &id, const RenderCallback &callback) {
    
    renderCallbacks.addCallback(id, callback);
}

void Entity::registerRenderCallback(CallbackIdentifier &&id, RenderCallback &&callback) {
    
    renderCallbacks.addCallback(std::move(id), std::move(callback));
}

void Entity::registerUpdateCallback(const CallbackIdentifier &id, const UpdateCallback &callback) {
    
    updateCallbacks.addCallback(id, callback);
}

void Entity::registerUpdateCallback(CallbackIdentifier &&id, UpdateCallback &&callback) {
    
    updateCallbacks.addCallback(std::move(id), std::move(callback));
}

void Entity::render() {

    renderCallbacks.executeAll();
}

void Entity::update() {

    updateCallbacks.executeAll();
}