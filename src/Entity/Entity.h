#pragma once

#include "Includes.h"
#include "Transform.h"
#include "Buffer.h"
#include "CallbackList/CallbackList.h"
#include "glm/gtc/quaternion.hpp"
#include "gtest/gtest.h"

class ComponentBase;

/**
 * @brief A Class to manage entities that can exist in the system
 * keeps a list of components associated with the entity, and uses callbacks to handle
 * update, renders, etc...
 * 
 *  Has a transform object which keeps track of the entity's position
 */
class Entity {
public:

    /**
     * @brief The data type to use to id each callback stored in the entity
     * each callback list can only have 1 callback with a given identifier, if a second callback is 
     * added with the same identifier, then the older callback is replaced
     */
    typedef ComponentBase *CallbackIdentifier;

    typedef void(RenderCallbackSignature)();
    typedef void(UpdateCallbackSignature)();

    typedef std::function<RenderCallbackSignature> RenderCallback; 
    typedef std::function<UpdateCallbackSignature> UpdateCallback;

    Entity();
    virtual ~Entity() {
        removeAllComponents();
    }

    /**
     * @brief Adds the given component to the entity. The Component's corresponding registerEntity function is called
     * and it is expected to register it's functions to the appropriate callback within the entity.
     * Does not allow for duplicate components to be added, components are compared using their pointer address.
     * 
     * @param component component to add
     * @return true if component is added
     * @return false if the given comopnent is already in the entity, does not add the component again
     */
    bool addComponent(const std::shared_ptr<ComponentBase> &component);

    /**
     * @brief removes the given component fomr the entity, as well as all of its corresponding callback functions
     * 
     * @param component component to remove
     */
    //MUST NOT BE REFERENCE, see definition of function for explanation
    void removeComponent(std::shared_ptr<ComponentBase> component);
    void removeComponent(ComponentBase *component);

    /**
     * @brief Removes all componetns attached to this entity, and also removes all callbacks
     * associated with the removed component. If for whatever reason there is a callback that isn't part of a component
     * that is attached, then the callback will NOT be removed
     * 
     */
    void removeAllComponents();

    /**
     * @brief Register the given callback function to the render callback list. Whenever the entity renders, the render callbacks
     * will be executed as well.
     * 
     * @param id the id used to identify the unique callback function.
     * @param callback the callback function to register.
     */
    void registerRenderCallback(const CallbackIdentifier &id, const RenderCallback &callback);
    void registerRenderCallback(CallbackIdentifier &&id, RenderCallback &&callback);

    void registerUpdateCallback(const CallbackIdentifier &id, const UpdateCallback &callback);
    void registerUpdateCallback(CallbackIdentifier &&id, UpdateCallback &&callback);

    void update();
    void render();

    const Transform &getTransform() const {
        return transform;
    }

    /**
     * @brief Set the Position of the given entity, also updates the internal buffer (but does not bind it to shader)
     * 
     * @param position new position
     */
    void setPosition(const glm::vec3 &position) {
        transform.position = position;
        updateModelToWorldBuffer();
    }

    /**
     * @brief Set the Rotation around each LOCAL axis. Angles are in degrees
     * 
     * @param xRot rotation around local x axis, in degrees
     * @param yRot rotation around local y axis in degrees
     * @param zRot rotation around local z axis in degrees
     */
    void setRotation(float xRot, float yRot, float zRot) {
        transform.orientation = glm::quat(glm::vec3(glm::radians(xRot), glm::radians(yRot), glm::radians(zRot)));
        updateModelToWorldBuffer();
    }

    /**
     * @brief rotate the entity locally around the given axis by the given angle in degrees
     * 
     * @param axis local axis to rotate around
     * @param degrees 
     */
    void rotate(glm::vec3 axis, float degrees) {
        transform.orientation = glm::angleAxis(glm::radians(degrees), axis) * transform.orientation;
        updateModelToWorldBuffer();
    }

private:
    void updateModelToWorldBuffer();

    FRIEND_TEST(EntityTest, componentRemainOutOfScop);

    //disable copying because we don't want the callback functions copied
    //there is a change the callback will be a lambda that captures local variables, whichi will  have incorrect references once copied
    Entity(const Entity &entity2) = delete;
    Entity &operator=(const Entity &entity) = delete;

    Transform transform;
    Buffer modelToWorldBuffer;

    std::vector<std::shared_ptr<ComponentBase> > components;

    CallbackList<CallbackIdentifier, void> renderCallbacks;
    CallbackList<CallbackIdentifier, void> updateCallbacks;
};
