#pragma once
#include "Includes.h"
//info for variadic templatesa nd perfect forwarding
//https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-variadic-templates
//https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c

//TODO handle callback return somehow, if needed

/**
 * @brief store a list of callback functions, which all have the same signature.
 * Each function is identified by a unique key, of type CallbackKey
 * the key is used to ensure only one copy of each function is stored, and to handle removal of functions from the list since function objects can't be compared normally
 * Each function is executed by passing all of the supplied argumetns to the given function
 * 
 * @tparam CallbackKey, the type of key used to identify each function
 * @tparam CallbackReturnType return type of each callback funciton
 * @tparam callbackarguments a variable list of argumetns for the callback functions.
 */
template<typename CallbackKey, typename CallbackReturnType, typename... CallbackArguments>
class CallbackList {
public:
    
    typedef std::function<CallbackReturnType(CallbackArguments...)> Callback;

    CallbackList() = default;
    
    //doesn't make sense to copy callback lists for now
    CallbackList(const CallbackList &other) = delete;
    CallbackList &operator=(const CallbackList &rhs) = delete;

    /**
     * @brief Adds the given callback function to the list of callbacks. if a callback with the given key already exsists, it will be replaced wqith the new function
     * 
     * @param callback callback to be executed
     * @param key key to identify the callback
     */
    void addCallback(const Callback &callback, const CallbackKey &key) {
        callbacks[key] = callback;
    }

    /**
     * @brief Removes the callbackw ith the given key
     * 
     * @param key key to identify the callback
     * @return the number of callback functions removed
     */
    int removeCallback(const CallbackKey &key) {
        return callbacks.erase(key);
    }

    /**
     * @brief Executes all the callbacks stored in the list, each callback is given the supplied arguments.
     * Return value of callback is ignored for now
     * 
     */
    void executeAll(CallbackArguments&& ...args) {
        for(auto &callbackPair : callbacks) {
            callbackPair.second( std::forward<CallbackArguments>(args)...);
        }
    }

private:

    std::map<CallbackKey, Callback> callbacks;
};