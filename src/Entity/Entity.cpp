#include "./Entity.h"
#include "components/Component.h"

Entity::Entity() {
    updateModelToWorldBuffer();
}

bool Entity::addComponent(const std::shared_ptr<ComponentBase> &component) {
    //don't add the same component twice
    if(std::find(components.begin(), components.end(), component) != components.end())
        return false;
    
    component->registerEntity(this);
    components.push_back(component);

    return true;
}

//use copy of pointer to avoid issue illustrated by the comment in te function
void Entity::removeComponent(std::shared_ptr<ComponentBase> component) {
    renderCallbacks.removeCallback(component.get());
    updateCallbacks.removeCallback(component.get());
    component->clearEntity(false);

    //erasure MUST be done last because if this funciton is called from removeAllComponents
    //then the pointer passed as argument to this function will be a reference to a pointer inside the array
    //and erasing the pionter in the array will result in undefined behavior
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
}

void Entity::removeComponent(ComponentBase *component) {
    renderCallbacks.removeCallback(component);
    updateCallbacks.removeCallback(component);

    auto checkIsTarget = [&](std::shared_ptr<ComponentBase> value){
        return value.get() == component;
    };

    component->clearEntity(false);
    components.erase(std::remove_if(components.begin(), components.end(), checkIsTarget), components.end());
}

void Entity::removeAllComponents() {
    for(int i = 0; i < components.size();) {
        removeComponent(components[i]);
    }
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
    updateModelToWorldBuffer();
    modelToWorldBuffer.bindToTargetBindingPoint(MODEL_TO_WORLD_UNIFORM_BLOCK_BINDING_POINT);
    renderCallbacks.executeAll();
}

void Entity::update() {

    updateCallbacks.executeAll();
}

void Entity::updateModelToWorldBuffer() {

    glm::mat4 data = glm::translate(glm::mat4(1.0f), transform.position) * toMat4(transform.orientation);

    if(!modelToWorldBuffer.isUsed()) {
        modelToWorldBuffer.create(Buffer::BufferType::UniformBuffer, glm::value_ptr(data), sizeof(glm::mat4), Buffer::UsageType::StreamDraw);
    }

    modelToWorldBuffer.updateData(glm::value_ptr(data), sizeof(glm::mat4), 0);
}