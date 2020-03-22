#include "./Entity.h"
#include "./Component.h"

void Entity::addComponent(std::shared_ptr<ComponentBase> component) {
    component->registerEntity(this);
    components.push_back(component);
}

// void Entity::registerCallback(EventCallbackTypes callbackType, std::shared_ptr<NoArgumentVoidFunction> callback) {
    
//     if(callbackMap.count(callbackType) == 0) {
//         cout << "Entity contains no callback list for callback " << callbackType << endl;
//     }

//     callbackMap.at(callbackType)->push_back(callback);
// }

void Entity::registerRenderCallback(RenderCallback callback) {
    
    renderCallbacks.push_back(callback);
}

void Entity::registerUpdateCallback(UpdateCallback callback) {
    
    updateCallbacks.push_back(callback);
}

void Entity::update() {

    executeAll(updateCallbacks);
}

void Entity::render() {

    executeAll(renderCallbacks);
}

template<typename T>
void Entity::executeAll(std::vector<T> &callbacks) {
    for(auto &callback : callbacks) {
        callback();
    }
}

// void Entity::initCallbackMap() {
//     callbackMap[Render] = &renderCallbacks;
// }