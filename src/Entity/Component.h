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

private:

    //owning entiry
    Entity* entity;
};