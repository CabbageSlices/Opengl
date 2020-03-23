#pragma once
#include "CallbackList.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-function-mocker.h"
#include "Includes.h"

using ::testing::MockFunction;

namespace callback {

//takes a MockFunction and turns it into an std::function that will call the 
//underlying Call method of the MockFunction object.
//used to insert the callback into a callback list
template<typename CallbackType, typename... Args> 
inline auto getCallback(MockFunction<void(Args...)> &mock) {
    return CallbackType(
        [&](Args&& ...args){
            mock.Call(std::forward<Args>(args)...);
        }
    );
};

class MockBase {
public:
    virtual void Call(int x) = 0;
};

class MockClass : public MockBase {
public:
    MOCK_METHOD(void, Call, (int x), (override));

    auto getCallback() {
        return [&](int x){
            this->Call(x);
        };
    }

    int internalValue = -1;
};

}