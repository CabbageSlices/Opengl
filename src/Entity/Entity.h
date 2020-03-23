#pragma once

#include "Includes.h"
#include "Transform.h"

//TODO cleanup code in entity, write documentation, write tests
//TODO use callbacklist  for callback handling instead of inside the class itself
class ComponentBase;

typedef std::function<void()> RenderCallback; 
typedef std::function<void()> UpdateCallback; 

class Entity {
public:

    Entity() = default;

    // enum EventCallbackTypes {
    //     Render,
    // };

    void addComponent(std::shared_ptr<ComponentBase> component);
    // void registerCallback(EventCallbackTypes callbackType, std::shared_ptr<NoArgumentVoidFunction> callback);
    void registerRenderCallback(RenderCallback callback);
    void registerUpdateCallback(UpdateCallback callback);

    void update();
    void render();

private:

    template<typename T>
    void executeAll(std::vector<T> &callbacks);

    //disable copying because we don't want the callback functions copied
    //there is a change the callback will be a lambda that captures local variables, whichi will  have incorrect references once copied
    Entity(const Entity &entity2) = delete;
    Entity &operator=(const Entity &entity) = delete;
    // void initCallbackMap();

    Transform transform;

    std::vector<std::shared_ptr<ComponentBase> > components;

    // std::map<EventCallbackTypes, CallbackList*> callbackMap;

    std::vector<RenderCallback> renderCallbacks;
    std::vector<UpdateCallback> updateCallbacks;
    //std::vector<std::function> renderCallbacks; for components to register to, component will register to this, and entity render will call all functiosn there
};