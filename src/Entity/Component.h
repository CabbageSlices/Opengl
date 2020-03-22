#pragma once
#include "Includes.h"
#include "./Entity.h"

//also consider https://www.cprogramming.com/tutorial/virtual_inheritance.html for multiple component types
class ComponentBase {
public:
    virtual ~ComponentBase() {}

    void registerEntity(Entity* _entity) {
        entity = _entity;
        registerCallbacksToEntity(entity);
    }

    Entity* getEntity() {
        return entity;
    }

protected:

    //register the derived classes callbacks to the entity
    virtual void registerCallbacksToEntity(Entity *entity) = 0;

    //list of functions to registger as callback to the entity when the entity is registered. if the entity is already registered tghen the callbacks will be registered manaully,
    //wihtout using this 
    // std::map<Entity::EventCallbackTypes, CallbackList> callbackMap;

private:

    //owning entiry
    Entity* entity;
};