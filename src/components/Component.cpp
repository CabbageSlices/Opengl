#include "./Component.h"

//  void ComponentBase::registerEntity(Entity* _entity) {
//     entity = _entity;

//     entity->registerCallbacks(callbackMap);
//     clearCallbacks();
// }

// void ComponentBase::registerCallback(Entity::EventCallbackTypes callbackType, std::shared_ptr<NoArgumentVoidFunction> callback) {

//     if(isEntityRegistered()) {
//         entity->registerCallback(callbackType, callback);
//         return;
//     }

//     //entity not registerd yet, store for later
//     callbackMap[callbackType].push_back(callback);
// }
