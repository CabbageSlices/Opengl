#pragma once
#include "Includes.h"
#include "Entity/Entity.h"

//also consider https://www.cprogramming.com/tutorial/virtual_inheritance.html for multiple component types
class ComponentBase {
public:
    virtual ~ComponentBase() {
        if(entity) {
            entity->removeComponent(this);
            entity = nullptr;
        }
    }

    void registerEntity(Entity* _entity) {
        entity = _entity;
        registerCallbacksToEntity(entity);
    }

    void clearEntity(bool deregisterCallbacks = true) {
        if(deregisterCallbacks && entity)
            entity->removeComponent(this);

        entity = nullptr;
    }

    Entity* getEntity() {
        return entity;
    }

protected:

    //register the derived classes callbacks to the entity
    virtual void registerCallbacksToEntity(Entity *entity) = 0;

private:

    //owning entiry
    Entity* entity = nullptr;
};